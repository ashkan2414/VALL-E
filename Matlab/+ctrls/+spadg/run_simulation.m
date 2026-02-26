function out_signals = run_simulation(sim_model, sim_time, controller_tf, sim_params)
    step_time = ceil(3 / controller_tf.Ts) * controller_tf.Ts;

    % Configure SimulationInput for parallel safety
    simIn = Simulink.SimulationInput(sim_model);
    simIn = simIn.setVariable('T', controller_tf.Ts);
    simIn = simIn.setVariable('step_time', step_time);
    simIn = simIn.setVariable('D', controller_tf);

    fields = fieldnames(sim_params);
    for i = 1:numel(fields)
        simIn = simIn.setVariable(fields{i}, sim_params.(fields{i}));
    end

    simIn = simIn.setModelParameter('StopTime', num2str(sim_time + step_time));

    % Run the simulation (parallel-safe)
    simOut = sim(simIn, 'ShowProgress', 'off');

    % Extract outputs
    out_signals.t = simOut.t_sim; 
    out_signals.y = simOut.y_sim; 
    out_signals.r = simOut.r_sim; 
    out_signals.u = simOut.u_sim;
end
