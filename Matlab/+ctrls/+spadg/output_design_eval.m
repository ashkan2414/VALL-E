
function output_design_eval(evaluation, step_min)
    disp("---- Controller Design Params ----");
    evaluation.design_params
    
    [spiral_center, spiral_rmax, fit_err] = ctrls.spa.fit_spiral_from_poles(evaluation.spa_results.params.ps);
    fprintf('\nEstimated spiral parameters:\n');
    fprintf('  Center     = %.6f %+ .6fj\n', real(spiral_center), imag(spiral_center));
    fprintf('  r_max      = %.6f\n', spiral_rmax);
    fprintf('  Fit RMS err= %.6e\n\n', fit_err);

    % plot response
    ctrls.spa.plot_response(evaluation.spa_results);
    ctrls.spa.plot_heat_map(evaluation.spa_results, false);    
    if ~evaluation.spa_results.success
        error("Design is a failure");
    end

    disp("---- Controller TF ----");
    D = minreal(evaluation.spa_results.D, 1e-8)
    fprintf("\n\n");
    
    % Discretize the plant
    G = c2d(evaluation.P, D.Ts, 'tustin');
    
    disp("---- Closed Loop Try TF ----");
    Try = feedback(G*D,1)
    fprintf("\n\n");
    
    disp("---- Controller SOS in CPP Format ----");
    ctrls.spadg.print_sos_cpp(D);
    fprintf("\n\n");
    
    disp("---- Controller SOS in MATLAB Format ----");
    ctrls.spadg.print_sos_matlab(D);
    fprintf("\n\n");

    % Analyze and evaluate design
    fprintf("\n\nMATLAB Response Evaluation\n\n");
    disp(evaluation.response_eval);
    fprintf("\n\n");

    fprintf("\n\nSimulink Response Evaluation\n\n");
    disp(evaluation.sim_response_eval);
    fprintf("\n\n");
    
    % plot simulation response
    figure;
    plot(evaluation.response_analysis.signals.t, evaluation.response_analysis.signals.y + step_min); hold on;
    plot(evaluation.sim_response_analysis.signals.t, evaluation.sim_response_analysis.signals.y);
    plot(evaluation.response_analysis.signals.t, evaluation.response_analysis.signals.r + step_min, '-');
    plot(evaluation.sim_response_analysis.signals.t, evaluation.sim_response_analysis.signals.r, '-');
    legend("computed y", "simulated y", "computed r", "simulated r");

    fprintf("\n\nSimulation Step Sequence Analysis\n\n");
    ctrls.step_sequence_analysis(evaluation.sim_out.t, evaluation.sim_out.y, evaluation.sim_out.r, evaluation.constraints);
end
