function display_pfrac(G)
%DISPLAY_PFRAC  Pretty-print partial fraction decomposition of a transfer function
%
%   display_pfrac(G)
%
%   Example:
%       s = tf('s');
%       G = (s+3)/((s+1)*(s+2)^2);
%       display_pfrac(G);

    if ~isa(G, 'tf')
        error('Input must be a transfer function (tf object).');
    end

    % Extract numerator and denominator
    [num, den] = tfdata(G, 'v');
    [r, p, k] = residue(num, den);

    % Detect whether continuous or discrete
    Ts = G.Ts;
    if Ts == 0
        syms s
        var = s;
        varname = 's';
    else
        syms z
        var = z;
        varname = 'z';
    end

    % Build symbolic expression
    expr = sum(r ./ (var - p));
    if ~isempty(k)
        expr = expr + poly2sym(k, var);
    end

    % Display original transfer function
    fprintf('\nTransfer function:\n');
    pretty(poly2sym(num, var) / poly2sym(den, var));

    % Display partial fraction form
    fprintf('\nPartial fraction decomposition:\n');
    pretty(vpa(expr, 5))
    fprintf('\n');
end
