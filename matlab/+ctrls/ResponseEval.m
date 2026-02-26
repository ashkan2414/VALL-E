classdef ResponseEval
    properties
        response_analysis        % struct containing ss_error, settling_time, overshoot, input_max, input_min
        constraints              % struct containing max_settle_time, max_os_percent, min_input, max_input
        meets_ss_error
        meets_settling
        meets_overshoot
        meets_max_input
        meets_min_input
        meets_constraints
    end
    
    methods
        function obj = ResponseEval(response_analysis, constraints, input_required)
            if nargin < 3
                input_required = true;
            end

            % Constructor: evaluates criteria and stores results
            obj.response_analysis = response_analysis;
            obj.constraints = constraints;
            
            obj.meets_ss_error  = response_analysis.ss_error < constraints.max_ss_error;
            obj.meets_settling  = response_analysis.settling_time <= constraints.max_settle_time;
            obj.meets_overshoot = response_analysis.overshoot <= constraints.max_os_percent;

            if (~isnan(response_analysis.input_max) && isscalar(response_analysis.input_max)) || input_required
                obj.meets_max_input = response_analysis.input_max <= constraints.max_input;
            else
                obj.meets_max_input = NaN;
            end
            
            if (~isnan(response_analysis.input_min) && isscalar(response_analysis.input_min)) || input_required
                obj.meets_min_input = response_analysis.input_min >= constraints.min_input;
            else
                obj.meets_min_input = NaN;
            end

            obj.meets_constraints = obj.meets_ss_error && obj.meets_settling && obj.meets_overshoot;
            if ~isnan(obj.meets_max_input)
                obj.meets_constraints = obj.meets_constraints && obj.meets_max_input;
            end

            if ~isnan(obj.meets_min_input)
                obj.meets_constraints = obj.meets_constraints && obj.meets_min_input;
            end
        end
        
        function disp(obj)
            % Custom display method
            fprintf('--- Response Evaluation ---\n');
            fprintf('Steady-state error: %.4f <= %.4f --> %s\n', obj.response_analysis.ss_error, obj.constraints.max_ss_error, ternary(obj.meets_ss_error, 'PASS', 'FAIL'));
            fprintf('Settling time: %.4f s <= %.4f s --> %s\n', obj.response_analysis.settling_time, obj.constraints.max_settle_time, ternary(obj.meets_settling, 'PASS', 'FAIL'));
            fprintf('Overshoot: %.2f%% <= %.2f%% --> %s\n', obj.response_analysis.overshoot, obj.constraints.max_os_percent, ternary(obj.meets_overshoot, 'PASS', 'FAIL'));
            
            if ~isnan(obj.meets_max_input)
                fprintf('Max input: %.2f <= %.2f --> %s\n', obj.response_analysis.input_max, obj.constraints.max_input, ternary(obj.meets_max_input, 'PASS', 'FAIL'));
            end
            if ~isnan(obj.meets_min_input)
                fprintf('Min input: %.2f >= %.2f --> %s\n', obj.response_analysis.input_min, obj.constraints.min_input, ternary(obj.meets_min_input, 'PASS', 'FAIL'));
            end
            fprintf('Overall: %s\n', ternary(obj.meets_constraints, 'PASS', 'FAIL'));
        end
    end
end

function out = ternary(cond, valTrue, valFalse)
    % Simple ternary function
    if cond
        out = valTrue;
    else
        out = valFalse;
    end
end