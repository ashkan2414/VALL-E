function [best_design, best_score, all_scores] = optimize_designs(design_evals, weights)
% FIND_BEST_DESIGN
%   Find the best design from a cell array of design structs
%   based on weighted objective scores.
%
% Usage:
%   [best_design, best_score, all_scores] = find_best_design(design_evals, weights)
%
% Inputs:
%   design_evals - cell array of design structs with fields:
%       .response_analysis
%       .response_eval
%
%   weights     - struct defining weights for scoring objectives
%
% Outputs:
%   best_design - struct corresponding to lowest score (best performance)
%   best_score  - numerical best score value
%   all_scores  - vector of all design scores (same order as input)

    % --- Input validation ---
    if ~iscell(design_evals)
        error('Input must be a cell array of design structs.');
    end
    n = numel(design_evals);
    if n == 0
        error('Design list is empty.');
    end

    all_scores = nan(1, n);

    % --- Compute scores for each design ---
    for i = 1:n
        d = design_evals{i};
        if ~isfield(d, 'response_analysis')
            error('Design %d is missing response_analysis field.', i);
        end

        all_scores(i) = ctrls.system_score(d.spa_results.D, d.spa_results.constraints, d.response_analysis, weights);
    end

    % --- Find best design ---
    [best_score, best_idx] = min(all_scores);
    best_design = design_evals{best_idx};

end