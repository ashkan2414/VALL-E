    
function simplified_design_eval = simplify_poles(...
    P, constraints, time_horizon_s, design_params, min_poles, sim_model, sim_time_s, sim_params)


    designer = ctrls.spadg.get_designer(P, constraints, time_horizon_s, ...
                                        sim_model, sim_time_s, sim_params);
    
    [simplified_design_eval, valid] = designer.check_one(design_params);
    if ~valid
        error("Design is not valid!")
    end

    sorted_pole_groups = sort_pole_groups(simplified_design_eval.spa_results.params.ps, simplified_design_eval.spa_results.w_coeffs);
    
    top_poles = [];
    for i = 1:length(sorted_pole_groups)
        % Add the poles from the next highest-weighted group
        top_poles = [top_poles; sorted_pole_groups{i}.poles];
        
        % Stop once we have reached or exceeded N poles
        if numel(top_poles) >= min_poles
            break;
        end
    end
    i = i + 1;

    while numel(top_poles) < numel(design_params.poles)
        dp = design_params;
        dp.poles = top_poles;
        
        [design_eval, valid] = designer.check_one(dp);
        if valid
            simplified_design_eval = design_eval;
            return;
        end

        fprintf("Adding pole group %d with weight %f\n", i, sorted_pole_groups{i}.max_weight);
        top_poles = [top_poles; sorted_pole_groups{i}.poles];
        i = i + 1;
    end
end


function sorted_groups = sort_pole_groups(poles, weights)
    if numel(poles) ~= numel(weights)
        error('The ''poles'' and ''weights'' vectors must have the same number of elements.');
    end
    
    % --- Group Poles and Assign Group Weights ---
    
    pole_groups = {}; % Cell array to hold our groups
    tol = 1e-12; % Tolerance for floating point comparisons
    processed_indices = false(size(poles));
    
    for i = 1:length(poles)
        if processed_indices(i)
            continue;
        end
        
        current_pole = poles(i);
        current_weight = weights(i);
        
        % Check if the pole is real
        if abs(imag(current_pole)) < tol
            group.poles = current_pole;
            group.max_weight = current_weight;
            pole_groups{end+1} = group;
            processed_indices(i) = true;
            
        % The pole is complex, find its conjugate partner
        else
            conjugate_pole = conj(current_pole);
            % Search for the conjugate in the rest of the list
            found_conjugate_idx = find(abs(poles(i+1:end) - conjugate_pole) < tol, 1, 'first');
            
            if ~isempty(found_conjugate_idx)
                conjugate_idx = i + found_conjugate_idx;
                conjugate_weight = weights(conjugate_idx);
                
                % Create a group for the pair
                group.poles = [current_pole; conjugate_pole]; % Store as column
                group.max_weight = max(abs(current_weight), abs(conjugate_weight));
                pole_groups{end+1} = group;
                
                % Mark both as processed
                processed_indices(i) = true;
                processed_indices(conjugate_idx) = true;
            else
                % Unpaired complex pole (edge case) - treat as a single group
                group.poles = current_pole;
                group.max_weight = current_weight;
                pole_groups{end+1} = group;
                processed_indices(i) = true;
            end
        end
    end
    
    % --- Sort the Groups by Weight ---
    
    % Extract all the max_weights from the cell array of structs
    group_weights = cellfun(@(x) x.max_weight, pole_groups);
    
    % Sort weights in descending order to get the sorting indices
    [~, sort_order] = sort(group_weights, 'descend');
    
    % Reorder the pole groups according to the sorted weights
    sorted_groups = pole_groups(sort_order);
end

function top_poles = get_n_highest_poles(sorted_groups, N)

    if N <= 0
        top_poles = [];
        return;
    end
        
    top_poles = [];
    for i = 1:length(sorted_groups)
        % Add the poles from the next highest-weighted group
        top_poles = [top_poles; sorted_groups{i}.poles];
        
        % Stop once we have reached or exceeded N poles
        if numel(top_poles) >= N
            break;
        end
    end
end