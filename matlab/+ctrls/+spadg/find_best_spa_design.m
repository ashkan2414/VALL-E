function design_evaluation = find_best_spa_design(P, constraints, time_horizon_s, design_param_search_space, sim_model, sim_time_s, sim_params, objective_weights, invalidate_cache)
    context_hash = ctrls.utils.DataHash({P, constraints, time_horizon_s, design_param_search_space, objective_weights});
    
    cache_dir = "cache_valid_designs";
    best_design_file_cache = fullfile(cache_dir, sprintf('best_design_%s.mat', context_hash));

    %% Design with design generator
    if ~invalidate_cache && exist(best_design_file_cache, 'file') == 2
        % Load cache
        data = load(best_design_file_cache);
        design_evaluation = data.best_design;
        disp('Loaded designs from cache.');
    else
        % Generate all valid designs
        designer = ctrls.spadg.get_designer(P, constraints, time_horizon_s, sim_model, sim_time_s, sim_params);

        valid_designs = designer.run_parallel(design_param_search_space);
        %valid_designs = designer.run_sequential(design_param_search_space);

        if numel(valid_designs) == 0
            error("No valid designs found!");
        end

        % Find best design using objective weights
        [best_design, best_score, all_scores] = ctrls.designs.optimize_designs(valid_designs, objective_weights);
        disp("Found best design with params:");
        disp(best_design.design_params);

        % Save to cache
        save(best_design_file_cache, 'best_design');
        disp('Generated designs and saved to cache.');

       % Extract Results
       design_evaluation = best_design;
    end

    design_evaluation = ctrls.spadg.evaluate_design(P, constraints, time_horizon_s, design_evaluation.design_params, sim_model, sim_time_s, sim_params, false);
end

function [payload, valid] = design_checker(P, constraints, time_horizon_s, design_params, sim_model, sim_time_s, sim_params, exit_early)
    design_evaluation = ctrls.spadg.evaluate_design(P, constraints, time_horizon_s, design_params, sim_model, sim_time_s, sim_params, exit_early);
    valid = ctrls.spadg.evaluation_is_valid(design_evaluation);
    if valid
        payload = design_evaluation;
    else
        payload = [];
    end
end
