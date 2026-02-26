function constraints = constraints(step_size, min_input, max_input, max_os_percent, settle_percentage, max_settle_time, max_ss_error)
     if nargin < 7
        max_ss_error = 1e-3;
     end
    
    constraints = struct();
    constraints.step_size = step_size;
    constraints.min_input = min_input;
    constraints.max_input = max_input;
    constraints.max_os_percent = max_os_percent;
    constraints.settle_percentage = settle_percentage;
    constraints.max_settle_time = max_settle_time;
    constraints.max_ss_error = max_ss_error;
end