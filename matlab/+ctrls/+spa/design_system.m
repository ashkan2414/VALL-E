function spa_results = design_system(G, constraints, time_horizon_s, design_params)

    %% Convert system to discrete
    if isct(G)
        G_discrete = c2d(G, design_params.sample_time_s, 'tustin');
    else
        G_discrete = G;
    end

    % %% Normalize plant gain for numerical conditioning
    % [num_d, den_d] = tfdata(G_discrete, 'v');
    % 
    % % Compute DC gain safely (avoid division by zero if integrator)
    % if abs(polyval(num_d, 1)) > 1e-12 && abs(polyval(den_d, 1)) > 1e-12
    %     dc_gain = abs(polyval(num_d, 1) / polyval(den_d, 1));
    % else
    %     % For integrators or marginally stable systems, use high-frequency gain or unity
    %     dc_gain = abs(num_d(end) / den_d(end));
    % end
    % 
    % if dc_gain == 0
    %     dc_gain = 1;
    % end
    % 
    % G_discrete = G_discrete / dc_gain;   % normalize to roughly unit magnitude
    % spap.normalization_gain = dc_gain;  % store it so you can rescale later

    %% Plant Poles and Coefficients in its Partial Fraction Decomposition
    [stable_real_pairs, stable_complex_pairs, unstable_real_pairs, unstable_complex_pairs] = decompose_tf(G_discrete);

    % --- Stable real poles ---
    [spap.rs_qs, spap.rs_ms, spap.rs_cs] = extract_pole_data(stable_real_pairs);

    % --- Stable complex poles ---
    [spap.cs_qs, spap.cs_ms, spap.cs_cs] = extract_pole_data(stable_complex_pairs);

    % --- Combine all stable poles ---
    [spap.s_qs, spap.s_ms, spap.s_cs] = concat_pole_data(spap.rs_qs, spap.rs_ms, spap.rs_cs, spap.cs_qs, spap.cs_ms, spap.cs_cs);

    % --- Unstable real poles ---
    [spap.ru_qs, spap.ru_ms, spap.ru_cs] = extract_pole_data(unstable_real_pairs);
    
    % --- Unstable complex poles ---
    [spap.cu_qs, spap.cu_ms, spap.cu_cs] = extract_pole_data(unstable_complex_pairs);
    
    % --- Combine all unstable poles ---
    [spap.u_qs, spap.u_ms, spap.u_cs] = concat_pole_data(spap.ru_qs, spap.ru_ms, spap.ru_cs, spap.cu_qs, spap.cu_ms, spap.cu_cs);

    % --- All poles combined ---
    [spap.qs, spap.ms, spap.cs] = concat_pole_data(spap.s_qs, spap.s_ms, spap.s_cs, spap.u_qs, spap.u_ms, spap.u_cs);

    spap.n = length(spap.qs);
    spap.nhat = length(spap.s_qs);
    spap.ntilde = length(spap.u_qs);
    spap.nreal = length(spap.rs_qs);
    spap.ncomplex = length(spap.cs_qs);
    
    %% Poles Chosen in the Simple Pole Approximation of W[z]
    [spap.real_w_poles, spap.complex_w_poles] = separate_real_complex(design_params.poles);
    spap.ps = [spap.real_w_poles spap.complex_w_poles];
    
    spap.mreal = length(spap.real_w_poles);
    spap.mcomplex = length(spap.complex_w_poles);
    spap.m = length(spap.ps);

    nk = spap.ms .* (spap.ms + 1) / 2;
    n_terms = sum(nk);
        
    nk_s = spap.s_ms .* (spap.s_ms + 1) / 2;
    n_terms_s = sum(nk_s);

    nk_u = spap.u_ms .* (spap.u_ms + 1) / 2;
    n_terms_u = sum(nk_u);

    max_mult = max(spap.ms);
    max_mult_s = max(spap.s_ms);
    max_mult_u = max(spap.u_ms);
    
    %% Calculation of alpha, beta, gamma, and gamma hat

    % Alpha
    spap.alpha = zeros(spap.m, 1);
    for i = 1:spap.m
        for k = 1:spap.n
            for rk1 = 1:spap.ms(k)
                spap.alpha(i) = spap.alpha(i) + ((-1)^rk1 * spap.cs(rk1,k)) / (spap.qs(k) - spap.ps(i))^rk1;
            end
        end
    end

    alpha_matrix = zeros(spap.m);
    for i = 1:spap.m
        alpha_matrix(i, i) = spap.alpha(i);
    end


    % Beta
    spap.beta = zeros(spap.n, max_mult, max_mult, spap.m);
    for i = 1:spap.m                      % W[z] pole index
        for k = 1:spap.n                  % plant pole index
            for rk1 = 1:spap.ms(k)          % multiplicity order (outer)
                for rk2 = 1:rk1               % inner multiplicity
                    spap.beta(k, rk1, rk2, i) = ...
                        ((-1)^(rk1 - rk2) * spap.cs(rk1, k)) / ...
                        (spap.qs(k) - spap.ps(i))^(rk1 - rk2 + 1);
                end
            end
        end
    end
    
    beta_matrix = zeros(n_terms, spap.m);
    row = 1;
    for k = 1:spap.n
        for rk1 = 1:spap.ms(k)
            for rk2 = 1:rk1
                beta_matrix(row, :) = squeeze(spap.beta(k, rk1, rk2, :));
                row = row + 1;
            end
        end
    end
    
    % Gamma
    spap.gamma = zeros(spap.n, max_mult_u, max_mult_u, spap.m);
    for i = 1:spap.m                            % SPA pole index
        for kt = spap.nhat+1:spap.n             % unstable pole index (local)
            for rkt1 = 1:spap.ms(kt)
                for rkt2 = 1:rkt1
                    spap.gamma(kt, rkt1, rkt2, i) = spap.beta(kt, rkt1, rkt2, i);
                end
            end
        end
    end

    gamma_matrix = zeros(n_terms_u, spap.m);    
    row = 1;
    for kt = spap.nhat+1:spap.n
        for rkt1 = 1:spap.ms(kt)
            for rkt2 = 1:rkt1
                gamma_matrix(row, :) = squeeze(spap.gamma(kt, rkt1, rkt2, :));
                row = row + 1;
            end
        end
    end
    
    % Gamma Hat
    a_func = @(a,b,ai,bi,i) (-1).^(a - i) .* nchoosek(a + b - i - 1, b - 1) .* (spap.qs(ai) - spap.qs(bi)).^(-a - b + i);

    spap.gamma_hat = zeros(spap.n, max_mult_u, max_mult_u, ...
                           spap.nhat, max_mult_s, max_mult_s);
    for kt = spap.nhat+1:spap.n
        for rkt1 = 1:spap.ms(kt)
            for rkt2 = 1:rkt1
                for khp = 1:spap.nhat       % stable pole index
                    if kt == khp
                        error("This should not ever happen");
                    end

                    for rkhp1 = 1:spap.ms(khp)
                        for rkhp2 = 1:rkhp1
                            spap.gamma_hat(kt, rkt1, rkt2, khp, rkhp1, rkhp2) = a_func(rkt1, rkhp2, kt, khp, rkt2) * spap.cs(rkt1,kt);
                        end
                    end
                end
            end
        end
    end
    
    gamma_hat_matrix = zeros(n_terms_u, n_terms_s);
    row = 1;
    for kt = spap.nhat+1:spap.n
        for rkt1 = 1:spap.ms(kt)
            for rkt2 = 1:rkt1
                col = 1;
                for khp = 1:spap.nhat
                    for rkhp1 = 1:spap.ms(khp)
                        for rkhp2 = 1:rkhp1
                            gamma_hat_matrix(row, col) = spap.gamma_hat(kt, rkt1, rkt2, khp, rkhp1, rkhp2);
                            col = col + 1;
                        end
                    end
                end
                row = row + 1;
            end
        end
    end

    
    %% Determination of A and b matrices for spa equations
    n_beta_rows = size(beta_matrix, 1);
    n_gamma_rows = size(gamma_matrix, 1);
    n_gamma_hat_cols = size(gamma_hat_matrix, 2);
    
    % Construct block matrix A
    spap.A = [
        % Row block 1: alpha constraint
        alpha_matrix, eye(spap.m), zeros(spap.m, n_gamma_hat_cols);
        
        % Row block 2: beta constraint
        beta_matrix, [zeros(n_terms_s, spap.m), eye(n_gamma_hat_cols);
                    zeros(n_beta_rows - n_terms_s, spap.m + n_gamma_hat_cols)];
        
        % Row block 3: gamma/gamma_hat constraint
        zeros(n_gamma_rows, spap.m), gamma_matrix, gamma_hat_matrix
    ];

    % --- build b vector terms for unstable poles ---
    spap.b_gamma = zeros(n_gamma_rows, 1);
    row = 1;
    for kt = spap.nhat+1:spap.n
        for rkt1 = 1:spap.ms(kt)
            for rkt2 = 1:rkt1
                if rkt1 == rkt2
                    spap.b_gamma(row) = -spap.cs(rkt2, kt);
                % else
                %     spap.b_gamma(row) = 0;
                end
                row = row + 1;
            end
        end
    end
    
    % Construct vector b
    spap.b = [
        zeros(spap.m + n_beta_rows, 1);
        spap.b_gamma
    ];
    
    %% Determination of step response matrices
    time_horizon = ceil(time_horizon_s / design_params.sample_time_s);
    n_terms_s = sum(spap.s_ms .* (spap.s_ms + 1) / 2);
    spap.step_ry_matrix = zeros(time_horizon, spap.m + n_terms_s);
    
    for t = 1:time_horizon
        % --- SPA poles (always simple) ---
        for i = 1:spap.m
            p = spap.ps(i);
            spap.step_ry_matrix(t, i) = -(1 - p^t) / (1 - p);
        end
    
        % --- Stable plant poles (may have multiplicity) ---
        col = 1;
        for kh = 1:spap.nhat    
            for rkh1 = 1:spap.ms(kh)
                for rkh2 = 1:rkh1
                    if t >= rkh2
                        s = 0;
                        for j = 0:rkh2-1
                            s = s + nchoosek(t, j)* spap.qs(kh)^(t-j)*(1-spap.qs(kh))^j;
                        end
                        spap.step_ry_matrix(t, spap.m + col) = -(1-s) / (1 - spap.qs(kh))^rkh2;
                    % else
                    %     spap.step_ry_matrix(t, spap.m + col) = 0;
                    end
                    
                    col = col + 1;
                end
            end
        end
    end
    spap.step_ry_matrix = spap.step_ry_matrix * constraints.step_size;
    
    spap.step_ru_matrix = zeros(time_horizon,spap.m);
    for t=1:time_horizon
        for i=1:spap.m
            spap.step_ru_matrix(t,i) = (1 - spap.ps(i)^t) / (1 - spap.ps(i));
        end
    end
    spap.step_ru_matrix = spap.step_ru_matrix * constraints.step_size;
    
    %% Determination of steady state vector
    n_terms_s = sum(spap.s_ms .* (spap.s_ms + 1) / 2);
    spap.steady_state_vector = zeros(1, spap.m + n_terms_s);
    
    % --- SPA poles (always simple) ---
    for i = 1:spap.m
        spap.steady_state_vector(i) = 1 / (1 - spap.ps(i));
    end
    
    % --- Stable plant poles (may have multiplicity) ---
    idx = 1;
    for kh = 1:spap.nhat    
        for rkh1 = 1:spap.ms(kh)
            for rkh2 = 1:rkh1
                spap.steady_state_vector(spap.m + idx) = 1 / (1 - spap.qs(kh))^rkh2;
                idx = idx + 1;
            end
        end
    end
    spap.steady_state_vector = spap.steady_state_vector * constraints.step_size;
    
    %% Defining the variables for the optimization
    wreal = sdpvar(spap.mreal,1,'full');
    wcomplex = sdpvar(spap.mcomplex/2,1,'full','complex');
    w = wreal;
    for i=1:(spap.mcomplex/2)
        w = [w;
             wcomplex(i);
             conj(wcomplex(i))];
    end
    
    xreal = sdpvar(spap.mreal,1,'full');
    xcomplex = sdpvar(spap.mcomplex/2,1,'full','complex');
    x = xreal;
    for i=1:(spap.mcomplex/2)
        x = [x;
             xcomplex(i);
             conj(xcomplex(i))];
    end

    n_terms_rs = sum(spap.rs_ms .* (spap.rs_ms + 1) / 2);
    n_terms_cs = sum(spap.cs_ms .* (spap.cs_ms + 1) / 2);

    xhatreal = sdpvar(n_terms_rs,1,'full');
    xhatcomplex = sdpvar(n_terms_cs/2,1,'full','complex');
    xhat = xhatreal;
    for i=1:(n_terms_cs/2)
        xhat = [xhat;
                xhatcomplex(i);
                conj(xhatcomplex(i))];
    end
    
    %% Defining the objective function and constraints for the optimization
    x_vector = [x; xhat];
    spa_vector = [w; x_vector];
    yss = -spap.steady_state_vector * x_vector;
    u_response = spap.step_ru_matrix * w;
    y_response = spap.step_ry_matrix * x_vector;

    objective = 0;

    % spa constraint
    solver_constraints = [spap.A * spa_vector == spap.b];

    % steady state constraint
    solver_constraints = [solver_constraints,
                   yss == constraints.step_size];

    % input saturation constraint
    solver_constraints = [solver_constraints,
                   max(u_response) <= constraints.max_input,
                   min(u_response) >= constraints.min_input];

    % overshoot constraint
    solver_constraints = [solver_constraints,
                   max(y_response) <= (1 + (constraints.max_os_percent / 100)) * yss];

    % settling time constraint
    j_hat = ceil(constraints.max_settle_time / design_params.sample_time_s);
    y_tail = y_response(j_hat:end, :);
    solver_constraints = [solver_constraints,
                   max(y_tail) <= (1 + (constraints.settle_percentage / 100)) * yss,
                   min(y_tail) >= (1 - (constraints.settle_percentage / 100)) * yss];

    %% Solving the optimization problem
    
    % set some options for YALMIP and solver
    options = sdpsettings('verbose', 0, 'solver', 'mosek');

    % solve the problem
    sol = optimize(solver_constraints,objective,options);
    
    % obtain the solution
    spa_results.success = sol.problem == 0;
    spa_results.design_params = design_params;
    spa_results.constraints = constraints;
    spa_results.params = spap;
    spa_results.w_coeffs = value(w);
    spa_results.x_coeffs = value(x);
    spa_results.xhat_coeffs = value(xhat);
    
    %% Recover the transfer functions
    z = tf('z',design_params.sample_time_s);
    
    % calculate W
    spa_results.W = 0;
    for i=1:spap.m
        spa_results.W = spa_results.W + spa_results.w_coeffs(i) / (z - spap.ps(i));
    end
    
    % calculate X
    spa_results.X = 1;

    % SPA pole contributions (simple poles)
    for i=1:spap.m
        spa_results.X = spa_results.X + spa_results.x_coeffs(i) / (z - spap.ps(i));
    end
    
    % Stable plant contributions (may have multiplicities)    
    idx = 1;
    for kh = 1:spap.nhat
        for rkh1 = 1:spap.ms(kh)
            for rkh2 = 1:rkh1
                spa_results.X = spa_results.X + spa_results.xhat_coeffs(idx) / (z - spap.qs(kh))^rkh2;
                idx = idx + 1;
            end
        end
    end
    
    spa_results.W.Numerator   = cellfun(@real, spa_results.W.Numerator, 'UniformOutput', false);
    spa_results.W.Denominator = cellfun(@real, spa_results.W.Denominator, 'UniformOutput', false);
    spa_results.W = tf(spa_results.W.Numerator, spa_results.W.Denominator, spa_results.W.Ts);

    spa_results.X.Numerator   = cellfun(@real, spa_results.X.Numerator, 'UniformOutput', false);
    spa_results.X.Denominator = cellfun(@real, spa_results.X.Denominator, 'UniformOutput', false);
    spa_results.X = tf(spa_results.X.Numerator, spa_results.X.Denominator, spa_results.X.Ts);
    
    
    spa_results.D = minreal((spa_results.W / spa_results.X), 1e-15);
    spa_results.X = minreal(spa_results.X, 1e-15);
    spa_results.W = minreal(spa_results.W, 1e-15);

    %% Responses
    spa_results.sample_time_s = design_params.sample_time_s;
    spa_results.t = design_params.sample_time_s*(1:time_horizon);
    spa_results.r = constraints.step_size * ones(size(spa_results.t));
    spa_results.step_ry = spap.step_ry_matrix * [spa_results.x_coeffs;spa_results.xhat_coeffs];
    spa_results.step_ru = spap.step_ru_matrix * spa_results.w_coeffs;
end

%% Partial Fraction Decomposition
function [stable_real, stable_complex, unstable_real, unstable_complex] = decompose_tf(varargin)
% DECOMPOSE_TF
% Groups partial fractions by unique pole locations. Each struct contains:
%   .p  -> pole location (scalar)
%   .c  -> vector of coefficients for that pole, in descending multiplicity order
%   .m  -> vector of multiplicities [1, 2, ..., length(c)]
%
% Example:  G(z) = c1/(z-q)^1 + c2/(z-q)^2
%     -> struct: .p = q, .c = [c1, c2], .m = [1, 2]

    % ---- Input handling ----
    if nargin == 1
        sys = varargin{1};
        if ~isa(sys, 'tf')
            error('Single input must be a transfer function (tf object).');
        end
        [num, den] = tfdata(sys, 'v');
    elseif nargin == 2
        num = varargin{1};
        den = varargin{2};
    else
        error('Usage: decompose_tf(sys) or decompose_tf(num, den)');
    end

    % ---- Partial fraction decomposition ----
    [c_raw, p_raw, ~] = residue(num, den);

    % ---- Handle numeric noise for repeated poles ----
    tol = 1e-8;
    rounded_poles = round(p_raw / tol) * tol;
    [unique_poles, ~, ~] = unique(rounded_poles);

    % ---- Group residues by pole ----
    grouped = struct('p', {}, 'c', {}, 'm', {});
    for i = 1:length(unique_poles)
        mask = abs(p_raw - unique_poles(i)) < tol;
        this_c = c_raw(mask);
        grouped(end+1).p = unique_poles(i);
        grouped(end).c = this_c(:).';  % store as row vector
        grouped(end).m = 1:numel(this_c);
    end

    % ---- Classification masks ----
    all_poles = [grouped.p];
    is_stable  = abs(all_poles) < 1;
    is_real    = imag(all_poles) == 0;
    is_complex = imag(all_poles) ~= 0;

    % ---- Split into categories ----
    stable_real      = grouped(is_stable  & is_real);
    stable_complex   = grouped(is_stable  & is_complex);
    unstable_real    = grouped(~is_stable & is_real);
    unstable_complex = grouped(~is_stable & is_complex);
end

function [p_vector, m_vector, c_matrix] = extract_pole_data(pole_structs)
% EXTRACT_POLE_DATA
% Groups a list of pole structs (from decompose_tf) into clean arrays.
%
% Inputs:
%   pole_structs : struct array with fields
%       .p  -> pole location (scalar)
%       .c  -> vector of coefficients for that pole
%       .m  -> multiplicities [1, 2, ..., length(c)]
%
% Outputs:
%   p_vector : (1 × n_poles) vector of unique pole locations
%   m_vector : (1 × n_poles) vector of multiplicities m_k
%   c_matrix : (max_multiplicity x n_poles) matrix of coefficients
%              row k -> pole index
%              col l -> coefficient for multiplicity l
%
% Example:
%   [p_vector, m_vector, c_matrix] = extract_pole_data(stable_real_pairs);

    if isempty(pole_structs)
        c_matrix = [];
        p_vector = [];
        m_vector = [];
        return;
    end

    % Pole locations
    p_vector = [pole_structs.p];

    % Multiplicities
    m_vector = arrayfun(@(s) numel(s.c), pole_structs);

    % Max multiplicity for zero-padding
    max_mult = max(m_vector);
    n_poles = numel(pole_structs);

    % Preallocate coefficient matrix
    c_matrix = zeros(max_mult, n_poles);

    % Fill coefficients row by row
    for k = 1:n_poles
        c_vec = pole_structs(k).c;
        c_matrix(1:numel(c_vec), k) = c_vec;
    end
end

function [p_combined, m_combined, c_combined] = concat_pole_data(p1, m1, c1, p2, m2, c2)
% CONCAT_POLE_DATA
% Concatenates two pole data sets produced by extract_pole_data.
%
% Inputs:
%   p1, m1, c1 : first set of poles, multiplicities, and coefficient matrix
%   p2, m2, c2 : second set of poles, multiplicities, and coefficient matrix
%
% Outputs:
%   p_combined : concatenated pole vector
%   m_combined : concatenated multiplicity vector
%   c_combined : concatenated coefficient matrix (zero-padded)
%
% Notes:
%   c_matrix is assumed to have dimensions (max_multiplicity × n_poles).
%   This function ensures both matrices are padded to the same row count
%   before horizontal concatenation.
%
% Example:
%   [p_all, m_all, c_all] = concat_pole_data(p_stable, m_stable, c_stable, ...
%                                            p_unstable, m_unstable, c_unstable);

    % handle empty inputs gracefully
    if isempty(c1)
        p_combined = p2; 
        m_combined = m2; 
        c_combined = c2; 
        return;
    elseif isempty(c2)
        p_combined = p1; 
        m_combined = m1; 
        c_combined = c1; 
        return;
    end

    % match multiplicity dimensions
    max_mult = max([size(c1,1), size(c2,1)]);
    if size(c1,1) < max_mult
        c1(end+1:max_mult, :) = 0;
    end
    if size(c2,1) < max_mult
        c2(end+1:max_mult, :) = 0;
    end

    % concatenate horizontally (since each column = one pole)
    c_combined = [c1, c2];
    p_combined = [p1, p2];
    m_combined = [m1, m2];
end

function [real_poles, complex_poles] = separate_real_complex(poles)
    % SEPARATE_REAL_COMPLEX
    % Separates a vector of poles into real and complex poles.
    %
    % Inputs:
    %   poles - vector of poles (can be real or complex)
    %
    % Outputs:
    %   real_poles    - vector of poles with zero imaginary part
    %   complex_poles - vector of poles with non-zero imaginary part

    real_mask = imag(poles) == 0;
    complex_mask = imag(poles) ~= 0;

    real_poles = poles(real_mask);
    complex_poles = poles(complex_mask);
end
