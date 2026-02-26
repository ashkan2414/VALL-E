function score = system_score(D, constraints, response_analysis, weights, norm_factors)
% SYSTEM_SCORE
% Compute a weighted numerical score based on response_analysis, controller simplicity,
% and optional normalization factors for each objective.
%
% Inputs:
%   D                  - controller transfer function
%   constraints        - struct containing constraints
%   response_analysis  - struct from extract_response_metrics()
%   weights            - struct with fields for each objective mode:
%                        .overshoot
%                        .settling_time
%                        .control_signal
%                        .output_signal
%                        .poles
%   norm_factors       - (optional) struct with fields matching objectives, each containing a scaling factor
%
% Output:
%   score    - scalar numerical score (lower = better)

    % --- Set default normalization factors ---
    default_factors = struct( ...
        'overshoot',          constraints.max_os_percent, ...                                               % % overshoot
        'settling_time',      constraints.max_settle_time, ...                                              % seconds
        'control_signal',     max(abs(constraints.max_input), abs(constraints.min_input)), ...  % typical control magnitude
        'output_signal',      constraints.step_size, ...                                                    % typical output signal magnitude
        'poles',              10 ...                                                                                   % typical number of poles
    );

    if nargin < 5 || isempty(norm_factors)
        norm_factors = default_factors;
    else
        % Fill missing factors from defaults
        fn = fieldnames(default_factors);
        for i = 1:numel(fn)
            if ~isfield(norm_factors, fn{i})
                norm_factors.(fn{i}) = default_factors.(fn{i});
            end
        end
    end

    % --- Extract signals ---
    u = response_analysis.signals.u;
    y = response_analysis.signals.y;

    % --- Initialize objectives struct ---
    objectives = struct();

    % =========================================================
    % Objective 1: Minimum overshoot
    % =========================================================
    objectives.minimum_os = response_analysis.overshoot / norm_factors.overshoot;

    % =========================================================
    % Objective 2: Settling time
    % =========================================================
    if isfield(response_analysis, 'settling_time')
        objectives.settling_time = response_analysis.settling_time / norm_factors.settling_time;
    else
        objectives.settling_time = 0; 
    end

    % =========================================================
    % Objective 3: Economic (reduce actuator wear, smooth control)
    % =========================================================
    normalized_u = u / norm_factors.control_signal;
    J1 = max(abs(normalized_u));      
    if numel(normalized_u) > 1
        du = diff(normalized_u);
        J2 = norm(du, inf); 
    else
        J2 = 0;
    end
    objectives.economic = mean([J1, J2]);

    % =========================================================
    % Objective 4: Sustainability (minimize control energy)
    % =========================================================
    objectives.sustainability = (norm(normalized_u, 2)^2) / numel(normalized_u);

    % =========================================================
    % Objective 5: Output variability
    % =========================================================
    normalized_y = y / norm_factors.output_signal;
    if numel(normalized_y) > 1
        dy = diff(normalized_y);
        objectives.output_variability = norm(dy, inf);
    else
        objectives.output_variability = 0;
    end

    % =========================================================
    % Objective 6: Simplicity (number of poles in D)
    % =========================================================
    try
        poles_D = pole(D);          
        objectives.simplicity = numel(poles_D) / norm_factors.poles;  % fewer poles = simpler
    catch
        warning('Could not compute poles of D. Setting simplicity to Inf.');
        objectives.simplicity = Inf;
    end

    % =========================================================
    % Clip all normalized objectives to [0,1] to avoid runaway scores
    % =========================================================
    obj_names = fieldnames(objectives);
    for i = 1:numel(obj_names)
        objectives.(obj_names{i}) = max(0, min(1, objectives.(obj_names{i})));
    end

    % =========================================================
    % Weighted aggregation
    % =========================================================
    % Fill missing weights with 0
    for i = 1:numel(obj_names)
        if ~isfield(weights, obj_names{i})
            weights.(obj_names{i}) = 0;
        end
    end

    % Normalize weights to sum to 1
    w_vec = cellfun(@(f) weights.(f), obj_names);
    if sum(w_vec) == 0
        warning('All weights are zero; setting equal weights.');
        w_vec = ones(size(w_vec));
    end
    w_vec = w_vec / sum(w_vec);

    % Combine scores
    J_vec = cellfun(@(f) objectives.(f), obj_names);
    score = sum(w_vec .* J_vec);

end
