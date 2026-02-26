function designer = get_designer(P, constraints, time_horizon_s, sim_model, sim_time_s, sim_params)
    context_hash = ctrls.utils.DataHash({P, constraints, time_horizon_s});
    cache_dir = "cache_valid_designs";

    % Generate all valid designs
    checker_func = @(params) design_checker(P, constraints, time_horizon_s, params, sim_model, sim_time_s, sim_params, true);
    designer = ctrls.designs.Designer(context_hash, cache_dir, checker_func);
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