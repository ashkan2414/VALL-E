function fig = plot_design_param_heat_map(params, scores)
    if ~iscell(params)
        error('Input must be a cell array of design structs.');
    end

    num_designs = numel(params);
    
    % Preallocate arrays
    sample_time_s   = zeros(1, num_designs);
    num_poles = zeros(1, num_designs);

    rmax = zeros(1, num_designs);

    % Extract fields
    for i = 1:num_designs
        sample_time_s(i) = params{i}.sample_time_s;
        num_poles(i)  = numel(params{i}.poles);
        
        [~, rmax_est, ~] = ctrls.spa.fit_spiral_from_poles(params{i}.poles);
        rmax(i) = rmax_est;
    end
    
    % --- Collect metrics ---
    metrics = struct( ...
        'sample_time_s', sample_time_s, ...
        'num_poles', num_poles, ...
        'rmax', rmax);

    % Pretty display names for plotting
    labels = { ...
        'Sample time (s)', ...
        'Number of poles', ...
        'Spiral r_{max}'};

    metric_names = fieldnames(metrics);
    num_metrics = numel(metric_names);

    % --- Plot setup ---
    fig = figure('Name', 'Design Parameter Heat Map', 'Color', 'w');
    t = tiledlayout(ceil(num_metrics/2), 2, 'TileSpacing', 'compact', 'Padding', 'compact');

    % --- Plot each metric vs. score ---
    for k = 1:num_metrics
        nexttile(t);
        metric_data = metrics.(metric_names{k});

        scatter(metric_data, scores, 50, scores, 'filled'); % color encodes score too
        xlabel(labels{k});
        ylabel('Design Score');
        title(sprintf('%s vs Score', labels{k}));
        grid on;
        colormap('parula');
        colorbar;
    end

    % --- Global adjustments ---
    sgtitle('Parameter Metrics vs Design Score');
end
