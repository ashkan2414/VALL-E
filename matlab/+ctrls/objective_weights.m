function weights = objective_weights(mode, custom_weights)
% OBJECTIVE_WEIGHTS
%   Generate a normalized weight struct for scoring controller designs,
%   now including settling_time and simplicity.
%
% Usage:
%   weights = objective_weights('balanced')
%   weights = objective_weights('economic')
%   weights = objective_weights('custom', struct('minimum_os',0.5,'economic',0.2,'settling_time',0.1,'simplicity',0.1))
%
% Inputs:
%   mode           - string specifying preset weighting scheme:
%                    'balanced'       equal weighting
%                    'economic'       emphasize actuator wear reduction
%                    'sustainability' emphasize control energy
%                    'performance'    emphasize response quality (overshoot + settling time)
%                    'simplicity'     emphasize simpler controllers
%                    'custom'         use provided custom_weights
%
%   custom_weights - (optional) struct with custom fields and values
%
% Output:
%   weights        - struct with fields:
%                      .minimum_os
%                      .settling_time
%                      .economic
%                      .sustainability
%                      .output_variability
%                      .simplicity
%
%   All weights are normalized to sum to 1.

    % --- Default initialization ---
    weights = struct( ...
        'minimum_os',         0, ...
        'settling_time',      0, ...
        'economic',           0, ...
        'sustainability',     0, ...
        'output_variability', 0, ...
        'simplicity',         0 ...
    );

    % --- Define presets ---
    switch lower(mode)
        case 'balanced'
            weights = set_fields(weights, [1 1 1 1 1 1]);

        case 'economic'
            weights = set_fields(weights, [0.1 0.05 0.6 0.2 0.05 0.0]);

        case 'sustainability'
            weights = set_fields(weights, [0.1 0.05 0.2 0.6 0.05 0.0]);

        case 'performance'
            weights = set_fields(weights, [0.8 0.7 0.2 0.1 0.1 0.1]);

        case 'simplicity'
            weights = set_fields(weights, [0.1 0.05 0.1 0.1 0.05 0.6]);

        case 'custom'
            if nargin < 2 || isempty(custom_weights)
                error('For mode "custom", you must provide custom_weights struct.');
            end
            fn = fieldnames(custom_weights);
            for i = 1:numel(fn)
                if isfield(weights, fn{i})
                    weights.(fn{i}) = custom_weights.(fn{i});
                else
                    warning('Unknown weight field "%s" ignored.', fn{i});
                end
            end

        otherwise
            error('Unknown mode: %s. Choose "balanced", "economic", "sustainability", "performance", "simplicity", or "custom".', mode);
    end

    % --- Normalize weights to sum to 1 ---
    vals = struct2array(weights);
    if sum(vals) == 0
        warning('All weights are zero; setting equal weights.');
        vals = ones(size(vals));
    end
    vals = vals / sum(vals);

    fn = fieldnames(weights);
    for i = 1:numel(fn)
        weights.(fn{i}) = vals(i);
    end
end

function weights = set_fields(weights, vals)
    fn = fieldnames(weights);
    for i = 1:numel(fn)
        weights.(fn{i}) = vals(i);
    end
end
