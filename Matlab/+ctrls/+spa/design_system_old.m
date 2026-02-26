function spa_results = design_system(G, constraints, time_horizon_s, design_params)

    %% Convert system to discrete
    if isct(G)
        G_discrete = c2d(G, design_params.sample_time_s, 'zoh');
    else
        G_discrete = G;
    end


    %% Plant Poles and Coefficients in its Partial Fraction Decomposition
    [stable_real_pairs, stable_complex_pairs, unstable_real_pairs, unstable_complex_pairs]= decompose_tf(G_discrete);
    
    % Extract residues and poles
    spa_params.stable_real_plant_cs = stable_real_pairs(:,1)';
    spa_params.stable_real_plant_poles = stable_real_pairs(:,2)';
    
    spa_params.stable_complex_plant_cs = stable_complex_pairs(:,1)';
    spa_params.stable_complex_plant_poles = stable_complex_pairs(:,2)';
    
    spa_params.stable_plant_cs = [spa_params.stable_real_plant_cs spa_params.stable_complex_plant_cs];
    spa_params.stable_plant_poles = [spa_params.stable_real_plant_poles spa_params.stable_complex_plant_poles];
    
    spa_params.unstable_real_plant_cs = unstable_real_pairs(:,1)';
    spa_params.unstable_real_plant_poles = unstable_real_pairs(:,2)';
    
    spa_params.unstable_complex_plant_cs = unstable_complex_pairs(:,1)';
    spa_params.unstable_complex_plant_poles = unstable_complex_pairs(:,2)';
    
    spa_params.unstable_plant_cs = [spa_params.unstable_real_plant_cs spa_params.unstable_complex_plant_cs];
    spa_params.unstable_plant_poles = [spa_params.unstable_real_plant_poles spa_params.unstable_complex_plant_poles];
    
    spa_params.cs = [spa_params.stable_plant_cs spa_params.unstable_plant_cs];
    spa_params.qs = [spa_params.stable_plant_poles spa_params.unstable_plant_poles];
    
    spa_params.n = length(spa_params.qs);
    spa_params.nhat = length(spa_params.stable_plant_poles);
    spa_params.nreal = length(spa_params.stable_real_plant_poles);
    spa_params.ncomplex = length(spa_params.stable_complex_plant_poles);
    
    %% Poles Chosen in the Simple Pole Approximation of W[z]
    min_number_poles = length(spa_params.unstable_plant_poles) * 2;
    num_provided_poles = length(design_params.poles);
    if num_provided_poles < min_number_poles 
        error("Number of poles specified (%i) less than minimum based on number of system unstable poles (%i)", num_provided_poles, min_number_poles);
    end
    
    [spa_params.real_w_poles, spa_params.complex_w_poles] = separate_real_complex(design_params.poles);
    spa_params.ps = [spa_params.real_w_poles spa_params.complex_w_poles];
    
    spa_params.mreal = length(spa_params.real_w_poles);
    spa_params.mcomplex = length(spa_params.complex_w_poles);
    spa_params.m = length(spa_params.ps);
    
    %% Calculation of alpha, beta, gamma, and gamma hat
    spa_params.alpha = zeros(spa_params.m);
    for i=1:spa_params.m
        for k=1:spa_params.n
            spa_params.alpha(i,i) = spa_params.alpha(i,i) + spa_params.cs(k) / (spa_params.ps(i) - spa_params.qs(k));
        end
    end
    
    spa_params.beta = zeros(spa_params.n,spa_params.m);
    for i=1:spa_params.m
        for k=1:spa_params.n
            spa_params.beta(k,i) = spa_params.cs(k) / (spa_params.qs(k) - spa_params.ps(i));
        end
    end
    
    spa_params.gamma = zeros(spa_params.n-spa_params.nhat,spa_params.m);
    for i=1:spa_params.m
        for j=(spa_params.nhat+1):spa_params.n
            spa_params.gamma(j-spa_params.nhat,i) = spa_params.cs(j) / (spa_params.qs(j) - spa_params.ps(i));
        end
    end
    
    spa_params.gamma_hat = zeros(spa_params.n-spa_params.nhat,spa_params.nhat);
    for k=1:spa_params.nhat
        for j=(spa_params.nhat+1):spa_params.n
            spa_params.gamma_hat(j-spa_params.nhat,k) = spa_params.cs(j) / (spa_params.qs(j) - spa_params.qs(k));
        end
    end
    
    %% Determination of A and b matrices for spa equations
    spa_params.A = [spa_params.alpha eye(spa_params.m) zeros(spa_params.m,spa_params.nhat);
         spa_params.beta [zeros(spa_params.nhat,spa_params.m) eye(spa_params.nhat);
               zeros(size(spa_params.beta,1)-spa_params.nhat,spa_params.m+spa_params.nhat)];
         zeros(size(spa_params.gamma)) spa_params.gamma spa_params.gamma_hat];
    
    spa_params.b = [zeros(spa_params.m+size(spa_params.beta,1),1);
         -spa_params.unstable_plant_cs'];
    
    %% Determination of step response matrices
    time_horizon = ceil(time_horizon_s / design_params.sample_time_s);
    spa_params.step_ry_matrix = zeros(time_horizon,spa_params.m+spa_params.nhat);
    for k=1:time_horizon
        for i=1:spa_params.m
            spa_params.step_ry_matrix(k,i) = - (1 - spa_params.ps(i)^k) / (1 - spa_params.ps(i));
        end
        for j=1:spa_params.nhat
            spa_params.step_ry_matrix(k,spa_params.m+j) = - (1 - spa_params.qs(j)^k) / (1 - spa_params.qs(j));
        end
    end
    spa_params.step_ry_matrix = spa_params.step_ry_matrix * constraints.step_size;
    
    spa_params.step_ru_matrix = zeros(time_horizon,spa_params.m);
    for k=1:time_horizon
        for i=1:spa_params.m
            spa_params.step_ru_matrix(k,i) = (1 - spa_params.ps(i)^k) / (1 - spa_params.ps(i));
        end
    end
    spa_params.step_ru_matrix = spa_params.step_ru_matrix * constraints.step_size;
    
    %% Determination of steady state vector
    spa_params.steady_state_vector = zeros(1,spa_params.m+spa_params.nhat);
    
    for i=1:spa_params.m
        spa_params.steady_state_vector(i) = 1 / (1 - spa_params.ps(i));
    end
    
    for k=1:spa_params.nhat
        spa_params.steady_state_vector(spa_params.m+k) = 1 / (1 - spa_params.qs(k));
    end

    spa_params.steady_state_vector = spa_params.steady_state_vector * constraints.step_size;
    
    %% Defining the variables for the optimization
    wreal = sdpvar(spa_params.mreal,1,'full');
    wcomplex = sdpvar(spa_params.mcomplex/2,1,'full','complex');
    w = wreal;
    for i=1:(spa_params.mcomplex/2)
        w = [w;
             wcomplex(i);
             conj(wcomplex(i))];
    end
    
    xreal = sdpvar(spa_params.mreal,1,'full');
    xcomplex = sdpvar(spa_params.mcomplex/2,1,'full','complex');
    x = xreal;
    for i=1:(spa_params.mcomplex/2)
        x = [x;
             xcomplex(i);
             conj(xcomplex(i))];
    end
    
    xhatreal = sdpvar(spa_params.nreal,1,'full');
    xhatcomplex = sdpvar(spa_params.ncomplex/2,1,'full','complex');
    xhat = xhatreal;
    for i=1:(spa_params.ncomplex/2)
        xhat = [xhat;
                xhatcomplex(i);
                conj(xhatcomplex(i))];
    end
    
    %% Defining the objective function and constraints for the optimization
    x_vector = [x; xhat];
    spa_vector = [w; x_vector];
    yss = -spa_params.steady_state_vector * x_vector;
    
    objective = 0;

    % spa constraint
    solver_constraints = [spa_params.A * spa_vector == spa_params.b];

   % steady state constraint
    solver_constraints = [solver_constraints,
                   yss == constraints.step_size];

    % input saturation constraint
    solver_constraints = [solver_constraints,
                   max(spa_params.step_ru_matrix * w) <= constraints.max_input,
                   min(spa_params.step_ru_matrix * w) >= constraints.min_input];

    % overshoot constraint
    solver_constraints = [solver_constraints,
                   max(spa_params.step_ry_matrix * x_vector) <= (1 + (constraints.max_os_percent / 100)) * yss];

    % settling time constraint
    j_hat = ceil(constraints.max_settle_time / design_params.sample_time_s);
    solver_constraints = [solver_constraints,
                   max(spa_params.step_ry_matrix(j_hat:end, :) * x_vector) <= (1 + (constraints.settle_percentage / 100)) * yss,
                   min(spa_params.step_ry_matrix(j_hat:end, :) * x_vector) >= (1 - (constraints.settle_percentage / 100)) * yss];
    
    %% Solving the optimization problem
    
    % set some options for YALMIP and solver
    options = sdpsettings('verbose', 0, 'solver', 'mosek');

    % solve the problem
    sol = optimize(solver_constraints,objective,options);
    
    % obtain the solution
    spa_results.success = sol.problem == 0;
    spa_results.design_params = design_params;
    spa_results.constraints = constraints;
    spa_results.params = spa_params;
    spa_results.w = value(w);
    spa_results.x = value(x);
    spa_results.xhat = value(xhat);
    
    %% Recover the transfer functions
    z = tf('z',design_params.sample_time_s);
    
    % calculate W
    spa_results.W = 0;
    for i=1:spa_params.m
        spa_results.W = spa_results.W + spa_results.w(i) / (z - spa_params.ps(i));
    end
    
    % calculate X
    spa_results.X = 1;
    for i=1:spa_params.m
        spa_results.X = spa_results.X + spa_results.x(i) / (z - spa_params.ps(i));
    end
    
    for k=1:spa_params.nhat
        spa_results.X = spa_results.X + spa_results.xhat(k) / (z - spa_params.qs(k));
    end
    

    spa_results.W.Numerator   = cellfun(@real, spa_results.W.Numerator, 'UniformOutput', false);
    spa_results.W.Denominator = cellfun(@real, spa_results.W.Denominator, 'UniformOutput', false);
    spa_results.W = tf(spa_results.W.Numerator, spa_results.W.Denominator, spa_results.W.Ts);

    spa_results.X.Numerator   = cellfun(@real, spa_results.X.Numerator, 'UniformOutput', false);
    spa_results.X.Denominator = cellfun(@real, spa_results.X.Denominator, 'UniformOutput', false);
    spa_results.X = tf(spa_results.X.Numerator, spa_results.X.Denominator, spa_results.X.Ts);
    
    
    spa_results.D = minreal(spa_results.W / spa_results.X);
    spa_results.X = minreal(spa_results.X);
    spa_results.W = minreal(spa_results.W);

    %% Responses
    spa_results.sample_time_s = design_params.sample_time_s;
    spa_results.t = design_params.sample_time_s*(1:time_horizon);
    spa_results.r = constraints.step_size * ones(size(spa_results.t));
    spa_results.step_ry = spa_params.step_ry_matrix * [spa_results.x;spa_results.xhat];
    spa_results.step_ru = spa_params.step_ru_matrix * spa_results.w;
end

%% Partial Fraction Decomposition
function [stable_real_pairs, stable_complex_pairs, unstable_real_pairs, unstable_complex_pairs] = decompose_tf(varargin)
    % DECOMPOSE_TF
    % Performs partial fraction decomposition and groups residues with their poles.
    %
    % Usage:
    %   [stableRealPairs, stableComplexPairs, unstableRealPairs, unstableComplexPairs] = decompose_tf(sys)
    %       where sys is a transfer function (tf object)
    %
    %   [stableRealPairs, stableComplexPairs, unstableRealPairs, unstableComplexPairs] = decompose_tf(num, den)
    %
    % Each output is an N×2 matrix:
    %   column 1 -> residue
    %   column 2 -> corresponding pole

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
    [cs, poles, ~] = residue(num, den);

    % ---- Classification masks ----
    stable_mask    = abs(poles) < 1;
    unstable_mask  = abs(poles) >= 1;

    real_mask      = imag(poles) == 0;
    complex_mask   = imag(poles) ~= 0;

    % ---- Group classification ----
    stable_real_pairs     = [cs(stable_mask & real_mask), poles(stable_mask & real_mask)];
    stable_complex_pairs  = [cs(stable_mask & complex_mask), poles(stable_mask & complex_mask)];
    unstable_real_pairs   = [cs(unstable_mask & real_mask), poles(unstable_mask & real_mask)];
    unstable_complex_pairs = [cs(unstable_mask & complex_mask), poles(unstable_mask & complex_mask)];
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
