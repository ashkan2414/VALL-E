function print_sos_matlab(D, prefix)
    if nargin < 2
        prefix = 'sys';
    end

    [b, a] = tfdata(D, 'v');
    [D_sos, D_sos_g] = tf2sos(b, a);
    Ts = D.Ts;

    [rows, cols] = size(D_sos);
    if cols ~= 6
        error('Expected SOS matrix with 6 columns [b0 b1 b2 a0 a1 a2].');
    end

    % Variable names
    coeff_var = sprintf('%s_sos_coeffs', prefix);
    gain_var  = sprintf('%s_sos_gain', prefix);
    Ts_var    = sprintf('%s_Ts', prefix);
    num_var   = sprintf('%s_num', prefix);
    den_var   = sprintf('%s_den', prefix);
    tf_var    = sprintf('%s_tf', prefix);

    % Print header
    fprintf('%% Reconstruct transfer function "%s"\n', prefix);
    fprintf('%s = [ ...\n', coeff_var);

    % Print SOS coefficient rows
    for i = 1:rows
        fprintf('    ');
        fprintf('%20.17g, ', D_sos(i,1:end-1));
        fprintf('%20.17g; ...\n', D_sos(i,end));
    end

    % Footer + reconstruction code
    fprintf('];\n');
    fprintf('%s = %.17g;  %% section gain\n', gain_var, D_sos_g);
    fprintf('%s = %.17g;  %% sample time [s]\n', Ts_var, Ts);
    fprintf('[%s, %s] = sos2tf(%s, %s);\n', num_var, den_var, coeff_var, gain_var);
    fprintf('%s = tf(%s, %s, %s);\n', tf_var, num_var, den_var, Ts_var);
end
