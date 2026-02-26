function design_evaluation = optimize_best_spa_design( ...
    P, constraints, time_horizon_s, initial_design_params, ...
    min_poles, sim_model, sim_time_s, sim_params, objective_weights)

    % 0. Evaluate initial design
    base_eval = evaluate_base_design(P, constraints, time_horizon_s, ...
                                     initial_design_params, sim_model, sim_time_s, sim_params);

    designer = ctrls.spadg.get_designer(P, constraints, time_horizon_s, ...
                                        sim_model, sim_time_s, sim_params);

    orig_poles = initial_design_params.poles(:);

    % 1. Identify conjugate pole groups
    [single_subsets, groups] = remove_distinct_poles_with_conjugates(orig_poles);

    % 2. Determine which groups are individually removable
    removable_groups = find_single_removable_groups( ...
        single_subsets, initial_design_params, designer);

    if isempty(removable_groups)
        design_evaluation = base_eval;
        return;
    end

    % 3. Hierarchical multi-group removal search
    all_valid_sets = hierarchical_group_search(removable_groups, groups, orig_poles, ...
                                               initial_design_params, designer, min_poles);

    % 4. Convert valid group sets into pole vectors
    pole_sets = build_pole_sets(all_valid_sets, groups, orig_poles);

    % 5. Evaluate all valid designs in parallel
    search = initial_design_params;
    search.poles = {pole_sets};
    valid_designs = designer.run_parallel(search);

    % 6. Pick the best valid design
    [best_design, ~, ~] = ctrls.designs.optimize_designs(valid_designs, objective_weights);
    design_evaluation = best_design;
end

function base_eval = evaluate_base_design(P, constraints, time_horizon_s, initial_params, sim_model, sim_time_s, sim_params)
    base_eval = ctrls.spadg.evaluate_design(P, constraints, time_horizon_s, initial_params, sim_model, sim_time_s, sim_params, true);
    if ~ctrls.spadg.evaluation_is_valid(base_eval)
        error("Initial design does not pass evaluation!");
    end
end

function removable_groups = find_single_removable_groups(single_subsets, initial_params, designer)
    results = run_parallel_wrapped(single_subsets, initial_params, designer);
    removable_groups = find(~cellfun(@isempty, results));
end

function all_valid_sets = hierarchical_group_search(removable_groups, groups, orig_poles, ...
                                                    initial_params, designer, min_poles)

    n = numel(orig_poles);
    % Ensure removable_groups is a plain numeric vector
    if iscell(removable_groups)
        removable_groups = cell2mat(removable_groups);
    end
    removable_groups = removable_groups(:).';

    % Start with individually removable groups
    current_layer = num2cell(removable_groups(:));
    all_valid_sets = current_layer;

    while ~isempty(current_layer)

        fprintf("Starting Pole Removal Layer %d\n", numel(current_layer{1}));

        % ---------- BUILD NEXT-LAYER CANDIDATES ----------
        candidates = {};
        for i = 1:numel(current_layer)
            S = current_layer{i};
            lastg = S(end);

            for g = removable_groups
                if g > lastg
                    new_set = [S g];

                    % Minimum pole count enforcement
                    rm_idx = unique(cell2mat(groups(new_set)));
                    if n - numel(rm_idx) < min_poles
                        continue;
                    end

                    candidates{end+1} = new_set; %#ok<AGROW>
                end
            end
        end

        if isempty(candidates)
            break;
        end

        % ---------- BUILD POLE SETS FOR THESE CANDIDATES ----------
        search_pole_sets = cell(numel(candidates),1);

        for ci = 1:numel(candidates)
            g_idx = candidates{ci};
            rm_idx = unique(cell2mat(groups(g_idx))); 
            keep = true(n,1);
            keep(rm_idx) = false;
            search_pole_sets{ci} = orig_poles(keep);
        end

        % ---------- RUN PARALLEL & WRAP ----------
        results = run_parallel_wrapped(search_pole_sets, initial_params, designer);

        % ---------- PICK ONLY NONEMPTY RESULTS ----------
        valid_idx = find(~cellfun(@isempty, results));

        if isempty(valid_idx)
            break;
        end

        % Next layer candidates are the ones whose designs passed
        next_layer = candidates(valid_idx);

        % Save and proceed deeper
        all_valid_sets = [all_valid_sets; next_layer(:)];
        current_layer = next_layer;
    end
end

function pole_sets = build_pole_sets(all_valid_sets, groups, orig_poles)
    n = numel(orig_poles);
    pole_sets = cell(numel(all_valid_sets),1);

    for i = 1:numel(all_valid_sets)
        g_idx = all_valid_sets{i};
        rm_idx = unique(cell2mat(groups(g_idx)));
        keep = true(n,1); keep(rm_idx) = false;

        pole_sets{i} = orig_poles(keep);
    end
end


function [subsets, groups] = remove_distinct_poles_with_conjugates(poles)
    poles = poles(:);
    n = numel(poles);

    tol = 1e-12;
    used = false(n,1);
    groups = {};

    for i = 1:n
        if used(i)
            continue
        end

        p = poles(i);

        if abs(imag(p)) < tol
            group = find(abs(poles - p) < tol);
        else
            is_p      = abs(poles - p) < tol;
            is_conj_p = abs(poles - conj(p)) < tol;
            group = find(is_p | is_conj_p);
        end

        used(group) = true;
        groups{end+1} = group;
    end

    % Single-removal subsets
    num_groups = numel(groups);
    subsets = cell(num_groups,1);

    for k = 1:num_groups
        rm = groups{k};
        keep = true(n,1);
        keep(rm) = false;
        subsets{k} = poles(keep);
    end
end


function results = run_parallel_wrapped(search_pole_sets, initial_params, designer)
    % Output cell array aligned with search_pole_sets
    N = numel(search_pole_sets);
    results = cell(N,1);

    % Build the actual search space struct
    search = initial_params;
    search.poles = search_pole_sets;

    % Run the parallel evaluation (only valid ones returned)
    valid_designs = designer.run_parallel(search);

    if isempty(valid_designs)
        % Everything failed; results remain empty
        return;
    end

    % Tolerance for comparing pole vectors
    tol = 1e-9;

    % For each returned valid design, match back to its originating pole-set
    for k = 1:numel(valid_designs)
        poles_valid = valid_designs{k}.design_params.poles;

        % Find which input entry matches this result
        for i = 1:N
            if pole_sets_equal(search_pole_sets{i}, poles_valid, tol)
                results{i} = valid_designs{k};
                break;
            end
        end
    end
end


function tf = pole_sets_equal(p1, p2, tol)
    if numel(p1) ~= numel(p2)
        tf = false;
        return;
    end

    p1 = p1(:);
    p2 = p2(:);

    if isempty(p1) && isempty(p2)
        tf = true;
        return;
    end

    tf = all(abs(p1 - p2) < tol);
end
