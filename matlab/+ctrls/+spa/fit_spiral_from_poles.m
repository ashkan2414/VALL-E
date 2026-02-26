function [center, rmax_est, fit_err] = fit_spiral_from_poles(poles, varargin)
%FIT_SPIRAL_FROM_POLES  Fit spiral parameters (center, rmax) to a set of poles.
%
%   [center, rmax_est, fit_err] = fit_spiral_from_poles(poles)
%   [center, rmax_est, fit_err] = fit_spiral_from_poles(poles, 'Center', c0)
%   [center, rmax_est, fit_err] = fit_spiral_from_poles(poles, 'Bounds', bounds)
%
%   Inputs:
%       poles : complex vector of poles (should include conjugate pairs)
%
%   Optional name-value pairs:
%       'Center' : Fix the center to a known complex value (anchor mode)
%       'Bounds' : [rmin rmax] bound for r_max search (default [0, Inf])
%
%   Outputs:
%       center     : estimated spiral center
%       rmax_est   : estimated spiral maximum radius
%       fit_err    : RMS residual fitting error
%
%   Notes:
%       - Fits spiral form used in generate_spiral_poles.
%       - Uses fminsearch to minimize squared radius residuals.
%       - Supports fixing center or jointly fitting center and rmax.
%
%   Example:
%       p = generate_spiral_poles(8, 0.7, -0.2); p = p{1};
%       fit_spiral_from_poles(p);           % full fit
%       fit_spiral_from_poles(p,'Center',-0.2); % anchored center
%

    % --- defaults ---
    opts.center_fixed = [];
    opts.bounds = [0, Inf];

    % --- parse name-value args ---
    for i = 1:2:numel(varargin)
        name = lower(varargin{i});
        val  = varargin{i+1};
        switch name
            case 'center'
                opts.center_fixed = val;
            case 'bounds'
                opts.bounds = val;
            otherwise
                error('Unknown parameter: %s', name);
        end
    end

    % --- preprocess poles ---
    poles = poles(:);
    t = numel(poles);
    if mod(t,2) ~= 0
        error('Number of poles must be even.');
    end

    % upper half of conjugates
    p_upper = poles(imag(poles) > 0);
    if isempty(p_upper)
        p_upper = poles;
    end
    n = numel(p_upper);
    k = (1:n)';
    expected_ratio = sqrt(k / (t/2));

    % --- objective: squared radius error ---
    function err = spiral_obj(params)
        % params = [real(c), imag(c), rmax]
        c = params(1) + 1i*params(2);
        rmax = params(3);
        r = abs(p_upper - c);
        r_pred = rmax * expected_ratio;
        err = mean((r - r_pred).^2);
    end

    % --- initial guesses ---
    if isempty(opts.center_fixed)
        c0 = mean(real(poles)) + 1i*mean(imag(poles(imag(poles)>0)));
    else
        c0 = opts.center_fixed;
    end
    r0 = max(abs(poles - c0));

    % --- run optimization ---
    if isempty(opts.center_fixed)
        % full search for [center, rmax]
        x0 = [real(c0), imag(c0), r0];
        best = fminsearch(@spiral_obj, x0, optimset('Display','off'));
        center = best(1) + 1i*best(2);
        rmax_est = best(3);
    else
        % anchor center, optimize rmax only
        c_fixed = opts.center_fixed;
        f = @(rmax) mean((abs(p_upper - c_fixed) - rmax * expected_ratio).^2);
        rmax_est = fminbnd(f, opts.bounds(1), opts.bounds(2));
        center = c_fixed;
    end

    % --- compute residual error ---
    r = abs(p_upper - center);
    r_fit = rmax_est * expected_ratio;
    fit_err = sqrt(mean((r - r_fit).^2));
end
