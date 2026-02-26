classdef ProgressTracker < handle
    properties
        count = 0
        n_total = 0
        initial = 0
        start_time
    end
    methods
        function obj = ProgressTracker(n_total, initial)
            obj.n_total = n_total;
            obj.start_time = tic;
            obj.count = initial;
            obj.initial = initial;
        end

        function tick(obj)
            obj.count = obj.count + 1;
        end

        function print(obj)
            elapsed = toc(obj.start_time);
            frac_done = obj.count / obj.n_total;
            pct_done = frac_done * 100;

            if mod(obj.count, 50) ~= 0 && obj.count < obj.n_total
                return;
            end

            tasks_per_sec = (obj.count - obj.initial) / elapsed;
            remaining = (obj.n_total - obj.count) / tasks_per_sec;
            fprintf('[Progress] %d/%d (%.1f%%) | ETA: %s | Elapsed: %s\n', ...
                obj.count, obj.n_total, pct_done, ...
                format_time(remaining), format_time(elapsed));
        end
    end
end

function time_str = format_time(seconds)
    if isnan(seconds)
        time_str = '--';
    elseif seconds < 60
        time_str = sprintf('%.1fs', seconds);
    elseif seconds < 3600
        time_str = sprintf('%dm %.0fs', floor(seconds/60), mod(seconds,60));
    else
        time_str = sprintf('%dh %dm', floor(seconds/3600), mod(floor(seconds/60),60));
    end
end
