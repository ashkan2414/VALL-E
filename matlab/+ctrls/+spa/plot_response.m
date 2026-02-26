function fig = plot_response(spa_results)
    fig = figure;  % create a new figure

    % --- Subplot 1: Step response output y[k] ---
    subplot(2,1,1);          % 2 rows, 1 column, first plot
    plot(spa_results.t, spa_results.step_ry, 'LineWidth', 1.5); hold on;
    plot(spa_results.t, spa_results.r, '-', 'LineWidth', 1.5); hold on;
    xlabel('Time [s]');
    ylabel('y[k]');
    title('Output Response');
    grid on;

    % --- Subplot 2: Step response input u[k] ---
    subplot(2,1,2);          % 2 rows, 1 column, second plot
    plot(spa_results.t, spa_results.step_ru, 'LineWidth', 1.5);
    xlabel('Time [s]');
    ylabel('u[k]');
    title('Control Input');
    grid on;
end