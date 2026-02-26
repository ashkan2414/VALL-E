function analysis = response_analysis(varargin)
% RESPONSE_ANALYSIS
% Extracts closed-loop response metrics.
%
% Usage:
%   analysis = response_analysis(P, D)                      % unit step
%   analysis = response_analysis(P, D, t)                   % with custom time vector
%   analysis = response_analysis(P, D, t, r)                % with custom reference
%   analysis = response_analysis(t, y, r)                   % from raw signals
%   analysis = response_analysis(t, y, r, u)                % with input
%   analysis = response_analysis(t, y, r, u, 'r_start', 0.3)% specify start reference
%
% Notes:
%   - 4th positional argument is always 'u' (optional)
%   - Named args come after, e.g., 'r_start', 0.3

    % --- (P,D,...) form ---
    if nargin >= 2 && isa(varargin{1}, 'tf')
        P = varargin{1};
        D = varargin{2};
        switch nargin
            case 2
                analysis = response_analysis_from_tf(P, D);
            case 3
                analysis = response_analysis_from_tf(P, D, varargin{3});
            case 4
                analysis = response_analysis_from_tf(P, D, varargin{3}, varargin{4});
            otherwise
                error('Too many inputs for (P, D, [t], [r]) form.');
        end
        return;
    end

    % --- (t,y,r,...) form ---
    if nargin < 3
        error('Expected at least (t, y, r).');
    end

    t = varargin{1};
    y = varargin{2};
    r = varargin{3};

    % Default optional
    u = [];
    r_start = [];

    % --- separate positional and name-value pairs ---
    % Find first string or char argument that signals the start of name-value pairs
    first_nv_idx = find(cellfun(@(x) ischar(x) || isstring(x), varargin(4:end)), 1);
    if ~isempty(first_nv_idx)
        nv_start = 3 + first_nv_idx; % account for the offset
        pos_args = varargin(1:nv_start-1);
        nv_args = varargin(nv_start:end);
    else
        pos_args = varargin;
        nv_args = {};
    end

    % --- assign positional ---
    if numel(pos_args) >= 4
        u = pos_args{4};
    end

    % --- parse name-value pairs manually ---
    for k = 1:2:numel(nv_args)
        name = lower(string(nv_args{k}));
        value = nv_args{k+1};
        switch name
            case "r_start"
                r_start = value;
            otherwise
                warning('Unknown parameter name: %s', name);
        end
    end

    % --- compute ---
    analysis = response_analysis_from_signals(t, y, r, u, r_start);
end

function analysis = response_analysis_from_signals(t, y, r, u, r_start)
% RESPONSE_ANALYSIS_FROM_SIGNALS
%   Extracts time-domain response metrics from measured signals.

    if nargin < 4 || isempty(u), u = []; end
    if nargin < 5, r_start = []; end

    % --- detect step start ---
    step_idx = find(abs(diff(r)) > 1e-3, 1);
    if isempty(step_idx)
        step_idx = 1;
    else
        step_idx = step_idx + 1;
    end

    % --- segment after step ---
    t_step = t(step_idx:end) - t(step_idx);
    y_step = y(step_idx:end);
    r_step = r(step_idx:end);
    if ~isempty(u)
        u_step = u(step_idx:end);
    else
        u_step = [];
    end

    % --- reference levels ---
    if isempty(r_start)
        if step_idx > 1
            r_initial = r(step_idx - 1);
        else
            r_initial = 0;
        end
    else
        if step_idx > 1
            error("r_start %f provided but r has a step in it", r_start);
        end
        r_initial = r_start;
    end

    r_final = r_step(end);
    ss_error = abs(r_final - y_step(end));

    % --- extract metrics ---
    info = stepinfo(y_step, t_step, r_final, r_initial, 'SettlingTimeThreshold', 0.02);

    % --- package ---
    signals.t = t_step;
    signals.y = y_step;
    signals.r = r_step;
    if ~isempty(u)
        signals.u = u_step;
    end

    analysis.signals = signals;
    analysis.ss_error = ss_error;
    analysis.settling_time = info.SettlingTime;
    analysis.overshoot = info.Overshoot;
    analysis.rise_time = info.RiseTime;

    if ~isempty(u)
        analysis.input_max = max(u_step);
        analysis.input_min = min(u_step);
    else
        analysis.input_max = NaN;
        analysis.input_min = NaN;
    end
end

function analysis = response_analysis_from_tf(P, D, t_k, r_input)
    % Extract response analysis from system/controller pair
    default_sim_time = 10;

    % --- Determine controller sample time ---
    if isnumeric(D)
        Ts_D = 0;
    elseif isa(D, 'tf') || isa(D, 'ss') || isa(D, 'zpk') || isa(D, 'genss')
        Ts_D = D.Ts;
    else
        try
            Ts_D = D.Ts;
        catch
            Ts_D = 0;
        end
    end

    % --- Determine time vector ---
    if nargin < 3
        if Ts_D == 0
            t_k = 0:0.01:default_sim_time;
        else
            t_k = Ts_D:Ts_D:default_sim_time;
        end
    else
        if Ts_D > 0 && any(abs(diff(t_k) - Ts_D) > 1e-12)
            error('Time vector t_k must have increments equal to D.Ts = %g for discrete systems.', Ts_D);
        end
    end

    % --- Reference input ---
    if nargin < 4
        r_input = 1; % default amplitude
    end
    if isscalar(r_input)
        r_k = ones(size(t_k)) * r_input;
    else
        if numel(r_input) ~= numel(t_k)
            error('Reference vector length must match time vector length.');
        end
        r_k = r_input;
    end

    % --- Prepare plant/controller ---
    if isct(P)
        G = c2d(P, Ts_D, 'zoh');

        Tru = feedback(D, G);
        [u_k, ~] = lsim(Tru, r_k, t_k);

        % Build a ZOH version of u(t)
        oversamp = 50;                            % points per sample for plotting
        t_c  = linspace(0, t_k(end), oversamp*numel(t_k));
        idx  = ceil((0:numel(t_c)-1)/oversamp)+1; % map each fine point to a sample
        idx(idx>numel(u_k)) = numel(u_k);
        u_c  = u_k(idx);                          % piecewise-constant control

        if numel(t_c) < 2 || numel(u_c) < 2
            error('Invalid input to lsim: t_c and u_c must each have ≥ 2 samples.');
        end
        if any(~isfinite(u_c)) || any(~isfinite(t_c))
            error('Invalid input to lsim: NaN/Inf detected.');
        end

        % Continuous plant response
        [y_c, t] = lsim(P, u_c, t_c);
        
        r = interp1(t_k, r_k, t_c, 'linear', 'extrap');
        u = u_c;
        y = y_c;
    else
        G = P; % Plant is already discrete
        Try = feedback(G * D, 1);
        Tru = feedback(D, G);

        [u, ~] = lsim(Tru, r_k, t_k);
        [y, t] = lsim(Try, r_k, t_k);
    end

    % --- Extract metrics ---
    analysis = response_analysis_from_signals(t, y, r, u);
end
