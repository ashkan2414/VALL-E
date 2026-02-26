function [meets_constraints, results, fig] = step_sequence_analysis(t, y, r, constraints, varargin)
    %% --- Parse optional arguments ---
    p = inputParser;
    addParameter(p, 'display_table', true, @islogical);
    addParameter(p, 'display_result', true, @islogical);
    addParameter(p, 'display_plot', true, @islogical);
    addParameter(p, 'plot_title', 'Step Response Constraint Analysis', @isstring);
    addParameter(p, 'x_axis_label', 'Time (s)', @isstring);
    addParameter(p, 'y_axis_label', 'Value', @isstring);
    parse(p, varargin{:});

    display_table = p.Results.display_table;
    display_result = p.Results.display_result;
    display_plot = p.Results.display_plot;
    plot_title   = p.Results.plot_title;
    x_axis_label = p.Results.x_axis_label;
    y_axis_label = p.Results.y_axis_label;


    %% === Analyze Data ===
    step_segments = ctrls.segment_signal_steps(t, y, r);

    empty_result = struct( ...
        'start_time_s', 0, ...
        'end_time_s', 0, ...
        'start_ref', 0, ...
        'target_ref', 0, ...
        'step_size', 0, ...
        'steady_state_error', 0, ...
        'settling_time_s', 0, ...
        'overshoot', 0, ...
        'meets_constraints', false);
    results = repmat(empty_result, numel(step_segments), 1);

    for i = 1:numel(step_segments)
        segment = step_segments(i);

        response_analysis = ctrls.response_analysis( ...
            segment.t, segment.y, segment.r, 'r_start', segment.start_ref);

        response_eval = ctrls.ResponseEval(response_analysis, constraints, false);

        results(i).start_time_s         = segment.t_start;
        results(i).end_time_s           = segment.t_start + segment.t(end);
        results(i).start_ref            = segment.start_ref;
        results(i).target_ref           = segment.target_ref;
        results(i).step_size            = segment.target_ref - segment.start_ref;
        results(i).steady_state_error   = response_analysis.ss_error;
        results(i).settling_time_s      = response_analysis.settling_time;
        results(i).overshoot            = response_analysis.overshoot;
        results(i).meets_constraints    = logical(response_eval.meets_constraints);
    end
    
    meets_constraints_array = arrayfun(@(r) r.meets_constraints, results);
    meets_constraints = all(meets_constraints_array);
    
    if display_table
        results_table = struct2table(results);
        results_table.Properties.VariableNames = { ...
            'Start Time (s)', ...
            'End Time (s)', ...
            'Start Ref', ...
            'Target Ref', ...
            'Step Size', ...
            'SS Error', ...
            'Settling Time (s)', ...
            'Overshoot (%)', ...
            'Meets Constraints' ...
        };
    end

    if display_result
        if meets_constraints
            fprintf("Step response meets constraints! All %d step step segments pass!\n", numel(step_segments));
        else
            num_segments_pass = sum(meets_constraints_array);
            fprintf("Step response does not meet constraints. %d/%d step segments pass.\n", num_segments_pass, numel(step_segments));
        end
    end
    
    fig = NaN;
    %% === Plot: Step Response + Constraint Markers ===
    if display_plot
        fig = figure('Name', plot_title, 'NumberTitle', 'off');
        hold on; grid on;
    
        pass_color = [0.2, 0.8, 0.2];
        fail_color = [0.9, 0.2, 0.2];
        h_pass = []; h_fail = []; h_ref = [];
    
        for i = 1:numel(step_segments)
            seg = step_segments(i);
            seg_t = seg.t + seg.t_start;
    
            if results(i).meets_constraints
                h_pass = plot(seg_t, seg.y, 'Color', pass_color, 'LineWidth', 1.5);
            else
                h_fail = plot(seg_t, seg.y, 'Color', fail_color, 'LineWidth', 1.5);
            end
            h_ref = plot(seg_t, seg.r, 'k--', 'LineWidth', 1.8);
        end
    
        % === Constraint lines ===
        step_start_times = arrayfun(@(s) s.t_start, step_segments);
        ref_starts = arrayfun(@(s) s.start_ref, step_segments);
        ref_targets = arrayfun(@(s) s.target_ref, step_segments);
        step_sizes = ref_targets - ref_starts;
    
        max_overshoot_factor = 1 + (constraints.max_os_percent / 100);
        max_overshoots = unique(round(max_overshoot_factor * step_sizes + ref_starts, 6));
    
        settle_time_factor_upper = 1 + (constraints.settle_percentage / 100);
        settle_time_factor_lower = 1 - (constraints.settle_percentage / 100);
        settle_time_upper_limits = unique(round(ref_targets * settle_time_factor_upper, 6));
        settle_time_lower_limits = unique(round(ref_targets * settle_time_factor_lower, 6));
        settle_time_value_limits = [settle_time_upper_limits; settle_time_lower_limits];
    
        settle_time_limits = step_start_times + constraints.max_settle_time;
    
        h_overshoot   = yline(max_overshoots, ':', 'Color', [0.8 0.2 0.2], 'LineWidth', 1.5);
        h_settle_band = yline(settle_time_value_limits, ':', 'Color', [0.5 0.1 0.7], 'LineWidth', 1.5);
        h_settle_time = xline(settle_time_limits, ':', 'Color', [0.2 0.2 0.8], 'LineWidth', 1.5);
        
        xline(step_start_times, '--', 'LineWidth', 1.8);
    
        xlabel(x_axis_label);
        ylabel(y_axis_label);
        title(plot_title);
    
        % Gather handles and labels
        handles = {h_pass, h_fail, h_ref, h_overshoot, h_settle_band, h_settle_time};
        labels  = {'Pass','Fail','Reference','Max Overshoot','Settling Band','Max Settling Time'};
        
        % Keep only non-empty handles
        valid_idx = ~cellfun(@isempty, handles);
        handles = handles(valid_idx);
        labels  = labels(valid_idx);
        
        % Convert from cell to vector of handles
        handles = cellfun(@(h) h(1), handles, 'UniformOutput', false);
        
        % Flatten into a numeric array of graphics handles
        handles = [handles{:}];
        
        % Now safe to call legend
        if ~isempty(handles)
            legend(handles, labels, 'Location', 'best');
        end
    
        hold off;
    end
end
