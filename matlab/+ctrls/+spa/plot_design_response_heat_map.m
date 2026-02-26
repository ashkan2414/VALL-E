function fig = plot_design_response_heat_map(design_params, response_analysis)
    if ~iscell(design_params)
        error('Input must be a cell array of design structs.');
    end

    num_designs = numel(design_params);

    analysis_metrics = {"ss_error", "settling_time", "overshoot", "rise_time", "input_min", "input_max"};
    labels = { ...
        'Steady State Error', ...
        'Settling Time (s)', ...
        'Overshoot %', ...
        "Rise Time (s)", ...
        "Input Min", ...
        "Input Max", ...
    };

    num_metrics = numel(analysis_metrics);
    data = zeros(num_designs, num_metrics);
    % Extract fields
    for i = 1:num_designs
        for j = 1:num_metrics
            data(i,j) = response_analysis{i}.(analysis_metrics{j});
        end
    end

    % --- Plot setup ---
    fig = figure('Name', 'Design Response Analysis Heat Map', 'Color', 'w');
    t = tiledlayout(ceil(num_metrics/2), 2, 'TileSpacing', 'compact', 'Padding', 'compact');

    % --- Plot each metric vs. score ---
    for k = 1:num_metrics
        nexttile(t);
        scatter(1:num_designs, data(:, k), 50, data(:, k), 'filled');
        xlabel("Design Index");
        ylabel(labels{k});
        title(sprintf('Design %s Map', labels{k}));
        grid on;
        colormap('parula');
        colorbar;

        % --- Make points clickable ---
        for i = 1:num_designs
            % Store struct in the scatter UserData
            h.UserData{i} = design_params{i};
        end

        % Assign callback for click
        h.ButtonDownFcn = @(src, event)onScatterClick(src, event);
    end

    % --- Global adjustments ---
    sgtitle('Parameter Metrics vs Design Score');
end

% --- Callback function ---
function onScatterClick(src, ~)
    % Get mouse click position
    cp = get(gca, 'CurrentPoint'); % [x y z]
    xclick = cp(1,1);
    yclick = cp(1,2);

    % Find the closest point in x
    [~, idx] = min(abs(src.XData - xclick));
    
    % Display the associated struct
    disp('--- Design Parameters ---');
    disp(src.UserData{idx});
end