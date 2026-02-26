function segments = segment_signal_steps(t, y, r)
    %% --- sanity checks ---
    if nargin < 3
        error('Usage: segment_signal_steps(t, y, r)');
    end
    if ~isvector(t) || ~isvector(y) || ~isvector(r)
        error('All inputs must be vectors.');
    end
    if numel(t) ~= numel(y) || numel(t) ~= numel(r)
        error('Inputs t, y, and r must have equal length.');
    end

    % ensure columns
    t = t(:);
    y = y(:);
    r = r(:);

    %% --- detect reference transitions ---
    change_threshold = 1e-6;  % increase if reference has noise
    dr = diff(r);
    change_idx = find(abs(dr) > change_threshold) + 1;

    step_bounds = [1; change_idx; numel(t)];
    n_segments = numel(step_bounds) - 1;

    %% --- allocate output ---
    segments = repmat(struct( ...
        'start_ref', [], ...
        'target_ref', [], ...
        't_start', [], ...
        't', [], ...
        'y', [], ...
        'r', []), n_segments, 1);

    %% --- segment extraction ---
    for i = 1:n_segments
        idx_start = step_bounds(i);
        idx_end   = step_bounds(i+1) - 1;
        if idx_end <= idx_start, continue; end

        % extract segment
        t_seg_abs = t(idx_start:idx_end);
        y_seg     = y(idx_start:idx_end);
        r_seg     = r(idx_start:idx_end);

        % relative time
        t_seg = t_seg_abs - t_seg_abs(1);

        % determine start & target
        if i == 1
            start_ref = r_seg(1);
        else
            start_ref = r(step_bounds(i)-1);
        end
        target_ref = mean(r_seg);

        % assign struct
        segments(i).start_ref  = start_ref;
        segments(i).target_ref = target_ref;
        segments(i).t_start    = t_seg_abs(1);
        segments(i).t          = t_seg;
        segments(i).y          = y_seg;
        segments(i).r          = r_seg;
    end

    is_step = abs([segments.target_ref] - [segments.start_ref]) > 1e-6;
    segments = segments(is_step);
end
