function print_sos_cpp(D, varname)
    if nargin < 2
        varname = 'BiQuad';
    end

    [b, a] = tfdata(D, 'v');
    [D_sos, D_sos_g] = tf2sos(b, a);
    [rows, cols] = size(D_sos);
    if cols ~= 6
        error('Expected SOS matrix with 6 columns [b0 b1 b2 a0 a1 a2].');
    end

    b = D_sos(:, 1:3);
    a = D_sos(:, 4:6);

    % ---- HEADER ----
    fprintf('static constexpr unsigned int kSampleTimeMS = %d;\n', round(D.Ts * 1000));
    fprintf('static constexpr size_t kNumSections = %d;\n', rows);
    fprintf('static constexpr std::array<BiQuadCoeffs<TFloat>, kNumSections> k%sCoeffs = {\n', varname);

    % ---- BODY ----
    for i = 1:rows
        fprintf(['    BiQuadCoeffs<TFloat>{ .b = { %.17g, %.17g, %.17g }, ' ...
                 '.a = { %.17g, %.17g, %.17g } },\n'], ...
                 b(i,1), b(i,2), b(i,3), a(i,1), a(i,2), a(i,3));
    end

    % ---- FOOTER ----
    fprintf('};\n');
    fprintf('static constexpr TFloat kGain = %.17g;\n', D_sos_g);
end
