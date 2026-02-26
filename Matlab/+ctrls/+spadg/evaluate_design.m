function design_evaluation = evaluate_design(P, constraints, time_horizon_s, design_params, sim_model, sim_time_s, sim_params, exit_early)

    design_evaluation = struct( ...
        'P', P, ...
        'constraints', constraints, ...
        'design_params', design_params);

    spa_results = ctrls.spa.design_system(P, constraints, time_horizon_s, design_params);
    design_evaluation.spa_results = spa_results;
    
    try
        design_evaluation.response_analysis = ctrls.response_analysis(P, spa_results.D, spa_results.t, spa_results.r);
        design_evaluation.response_eval = ctrls.ResponseEval(design_evaluation.response_analysis, constraints);
    catch ME
        return;
    end

    if exit_early && ~design_evaluation.response_eval.meets_constraints
        return;
    end

    out = ctrls.spadg.run_simulation(sim_model, sim_time_s, spa_results.D, sim_params);
    design_evaluation.sim_out = out;
    design_evaluation.sim_response_analysis = ctrls.response_analysis(out.t, out.y, out.r, out.u);
    design_evaluation.sim_response_eval = ctrls.ResponseEval(design_evaluation.sim_response_analysis, constraints);
end