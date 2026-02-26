function poles = generate_spiral_poles(total, rmax, center)
    % --- ensure all are column vectors ---
    total  = total(:);
    rmax   = rmax(:);
    center = center(:);

    % --- determine target length ---
    n = max([numel(total), numel(rmax), numel(center)]);

    % --- broadcast scalars ---
    if isscalar(total), total  = repmat(total,  n, 1); end
    if isscalar(rmax), rmax   = repmat(rmax,   n, 1); end
    if isscalar(center), center = repmat(center, n, 1); end

    % --- validate lengths ---
    if ~(numel(total) == n && numel(rmax) == n && numel(center) == n)
        error('Non-scalar inputs must have the same length.');
    end

    % --- compute ---
    poles = cell(n,1);
    for i = 1:n
        t = total(i);
        r = rmax(i);
        c = center(i);

        if mod(t,2) ~= 0
            error('Total number of poles must be even (got %d).', t);
        end

        reals = zeros(1, t);
        imags = zeros(1, t);

        for k = 1:(t/2)
            rk = r * sqrt(k / (t/2));
            thetak = 2 * sqrt(pi*k);
            reals(2*(k-1)+1) = rk * cos(thetak);
            imags(2*(k-1)+1) = rk * sin(thetak);
            reals(2*k)       = rk * cos(-thetak);
            imags(2*k)       = rk * sin(-thetak);
        end

        poles{i} = reals + 1i*imags + c;
    end

    if isscalar(poles)
        poles = poles{:};
    end
end
