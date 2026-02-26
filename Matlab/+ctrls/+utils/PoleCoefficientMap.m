classdef PoleCoefficientMap < handle
    % POLECOEFFICIENTMAP
    % Stores and retrieves partial-fraction coefficients for given poles/exponents.
    %
    % Usage:
    %   map = PoleCoefficientMap();                  % empty map
    %   map.add(pole, r, coeff);                     % add or accumulate
    %   val = map.get(pole, r);                      % retrieve coefficient
    %   map = PoleCoefficientMap(sys_tf);            % initialize from transfer function
    %
    % Notes:
    %   Coefficients for same (pole, r) are summed automatically.

    properties (Access = private)
        data
        tol = 1e-10
    end

    methods
        % --- Constructor ---
        function obj = PoleCoefficientMap(sys)
            obj.data = containers.Map('KeyType', 'char', 'ValueType', 'any');

            if nargin == 0
                return; % empty map
            end

            % Construct from transfer function if provided
            [num, den] = tfdata(sys, 'v');
            [c, p, ~] = residue(num, den);

            tol = obj.tol;
            rounded_poles = round(p / tol) * tol;
            [unique_poles, ~, idx] = unique(rounded_poles);

            for i = 1:numel(unique_poles)
                mask = abs(p - unique_poles(i)) < tol;
                c_group = c(mask);
                for r = 1:numel(c_group)
                    key = obj.make_key(unique_poles(i), r);
                    obj.data(key) = c_group(r);
                end
            end
        end

        % --- Add a new (pole, exponent, coeff) triple ---
        function add(obj, pole, r, coeff)
            key = obj.make_key(pole, r);
            if isKey(obj.data, key)
                obj.data(key) = obj.data(key) + coeff;
            else
                obj.data(key) = coeff;
            end
        end

        % --- Retrieve a coefficient ---
        function val = get(obj, pole, r)
            key = obj.make_key(pole, r);
            if isKey(obj.data, key)
                val = obj.data(key);
            else
                val = 0;
            end
        end

        % --- Pretty print (optional) ---
        function disp(obj)
            keys = obj.data.keys;
            fprintf('PoleCoefficientMap contents:\n');
            for i = 1:numel(keys)
                fprintf('  %-15s : %+g\n', keys{i}, obj.data(keys{i}));
            end
        end
        
        % Compare this map against another PoleCoefficientMap
        function results = compare(obj, other, tol_rel)
            if nargin < 3
                tol_rel = 1e-6;
            end
        
            keys_a = obj.data.keys;
            keys_b = other.data.keys;
            allKeys = unique([keys_a, keys_b]);
        
            results = repmat(struct('pole',0,'r',0,'A',NaN,'B',NaN,'match',NaN), numel(allKeys), 1);
        
            for i = 1:numel(allKeys)
                key = allKeys{i};
        
                % --- parse (a+bi)_r or real_r ---
                [p, r] = obj.parse_key(key);
        
                % --- retrieve coefficients from both maps ---
                a_val = NaN;
                b_val = NaN;
                if isKey(obj.data, key)
                    a_val = obj.data(key);
                end
                if isKey(other.data, key)
                    b_val = other.data(key);
                end
        
                % --- tolerance comparison ---
                if ~isnan(a_val) && ~isnan(b_val)
                    % direct or conjugate equivalence allowed
                    match = abs(a_val - b_val) < tol_rel * max(1, abs(a_val)) || ...
                            abs(a_val - conj(b_val)) < tol_rel * max(1, abs(a_val));
                else
                    match = NaN;
                end
        
                % --- store result ---
                results(i) = struct( ...
                    'pole', p, ...
                    'r', r, ...
                    'A', a_val, ...
                    'B', b_val, ...
                    'match', match ...
                );
            end
        end

        %% --- ADDITION (A + B) ---
        function result = plus(A, B)
            result = ctrls.utils.PoleCoefficientMap();
            keys_a = A.data.keys;
            keys_b = B.data.keys;
            allKeys = unique([keys_a, keys_b]);
            for i = 1:numel(allKeys)
                key = allKeys{i};
                valA = 0; valB = 0;
                if isKey(A.data, key)
                    valA = A.data(key);
                end
                if isKey(B.data, key)
                    valB = B.data(key);
                end
                result.data(key) = valA + valB;
            end
        end

        %% --- SUBTRACTION (A - B) ---
        function result = minus(A, B)
            result = ctrls.utils.PoleCoefficientMap();
            keys_a = A.data.keys;
            keys_b = B.data.keys;
            allKeys = unique([keys_a, keys_b]);
            for i = 1:numel(allKeys)
                key = allKeys{i};
                valA = 0; valB = 0;
                if isKey(A.data, key)
                    valA = A.data(key);
                end
                if isKey(B.data, key)
                    valB = B.data(key);
                end
                result.data(key) = valA - valB;
            end
        end
    
        %% --- MULTIPLICATION (A * B) ---
        function result = mtimes(A, B)
            % --- scalar * map ---
            if isnumeric(A) && isa(B, 'ctrls.utils.PoleCoefficientMap')
                result = B.copy_scaled(A);
                return;
            end
        
            % --- map * scalar ---
            if isa(A, 'ctrls.utils.PoleCoefficientMap') && isnumeric(B)
                result = A.copy_scaled(B);
                return;
            end
        
            % --- both maps ---
            if ~isa(A, 'ctrls.utils.PoleCoefficientMap') || ~isa(B, 'ctrls.utils.PoleCoefficientMap')
                error('Both operands must be PoleCoefficientMap or one must be numeric.');
            end

            result = ctrls.utils.PoleCoefficientMap();        
            keys_a = A.data.keys;
            keys_b = B.data.keys;
        
            for i = 1:numel(keys_a)
                [p1, r1] = A.parse_key(keys_a{i});
                a = A.data(keys_a{i});
        
                for j = 1:numel(keys_b)
                    [p2, r2] = B.parse_key(keys_b{j});
                    b = B.data(keys_b{j});
        
                    if abs(p1 - p2) < min(A.tol, B.tol)
                        result.add(p1, r1 + r2, a * b);
                        continue;
                    end
        
                    % --- Terms at pole p1 ---
                    for k = 1:r1
                        coeff = (-1)^(r1 - k) * a * b * nchoosek(r1 + r2 - k - 1, r2 - 1) / (p1 - p2)^(r1 + r2 - k);
                        result.add(p1, k, coeff);
                    end
        
                    % --- Terms at pole p2 ---
                    for k = 1:r2
                        coeff = (-1)^(r2 - k) * a * b * nchoosek(r1 + r2 - k - 1, r1 - 1) / (p2 - p1)^(r1 + r2 - k);
                        result.add(p2, k, coeff);
                    end
                end
            end
        end

        function result = uminus(A)
            result = ctrls.utils.PoleCoefficientMap();
            keys = A.data.keys;
            for i = 1:numel(keys)
                k = keys{i};
                result.data(k) = -A.data(k);
            end
        end

        function out = copy_scaled(obj, scalar)
            out = ctrls.utils.PoleCoefficientMap();
            keys = obj.data.keys;
            for i = 1:numel(keys)
                out.data(keys{i}) = obj.data(keys{i}) * scalar;
            end
        end
    end

    methods (Access = private, Static)
        function key = make_key(pole, r)
            pole = round(pole, 8);  % round real & imag parts to 1e-8
            if isreal(pole)
                key = sprintf('%.8g_%d', pole, r);
            else
                key = sprintf('(%+.8g%+.8gi)_%d', real(pole), imag(pole), r);
            end
        end

        function [p, r] = parse_key(key)
            if startsWith(key, '(') % complex pole key
                tokens = regexp(key, '^\((?<re>[+-]?\d+(\.\d+)?([eE][+-]?\d+)?)?(?<im>[+-]\d+(\.\d+)?([eE][+-]?\d+)?)i\)_(?<r>\d+)$', 'names');
                if isempty(tokens)
                    error('Invalid complex key format: %s', key);
                end
                p = str2double(tokens.re) + 1i * str2double(tokens.im);
                r = str2double(tokens.r);
            else
                parts = split(key, '_');
                p = str2double(parts{1});
                r = str2double(parts{2});
            end
        end
    end
end
