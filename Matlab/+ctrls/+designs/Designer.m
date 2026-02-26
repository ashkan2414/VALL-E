classdef Designer < handle
    % DESIGNER - Encapsulates parameter-space design exploration with caching.
    %
    %   Usage:
    %       d = ctrls.designs.Designer(context_hash, checker_func);
    %       d.run_parallel(search_space);
    %       valid_designs = d.load_valid();
    %
    %   Core responsibilities:
    %       • Wraps BatchedShardCache for resumable persistence
    %       • Hashes parameter sets via ctrls.utils.DataHash
    %       • Caches results from user-supplied checker_func
    %       • Explores parameter grids sequentially or in parallel

    properties
        context_hash
        checker_func
        cache_root
        worker_batch_size = 500
        flush_size = 100
        shard_size = 500
        cache
    end

    methods
        function self = Designer(context_hash, cache_root, checker_func)
            if nargin < 2
                cache_root = 'cache_valid_designs';
            end
            if nargin < 3
                checker_func = missing;
            end

            if ~exist(cache_root, 'dir')
                mkdir(cache_root);
            end

            self.context_hash = context_hash;
            self.cache_root = cache_root;
            self.checker_func = checker_func;

            cache_name = sprintf('context_%s', context_hash);
            self.cache = ctrls.utils.BatchedShardCache(cache_name, ...
                self.flush_size, self.shard_size, cache_root);
        end

        function h = hash_params(~, params)
            h = ctrls.utils.DataHash(params);
        end

        function [payload, valid] = check_one(self, params)
            % CHECK_ONE - Evaluate one parameter set and cache result.
            h = self.hash_params(params);
            if self.cache.contains(h)
                data = self.cache(h);
                payload = data.payload;
                valid = strcmp(data.type, 'valid');
                return;
            end

            if ismissing(self.checker_func)
                error("Designer is read-only without checker_func specified. Can't check new design");
            end

            % Evaluate via user checker
            data = struct('hash', h, 'type', 'invalid');
            [payload, valid] = self.checker_func(params);
            data.payload = payload;
            if valid
                data.type = 'valid';
            end

            % Cache result
            try
                self.cache(h) = data; 
            catch ME
                warning('Cache append failed for hash %s: %s', h, ME.message);
            end
        end

        function valid_designs = run_sequential(self, search_space)
            % RUN_SEQUENTIAL - Iterate through all parameter combinations (single-threaded)
            [fields, values, lens, num_combinations] = ctrls.designs.Designer.prepare_space(search_space);

            done_hashes = self.cache.get_keys();
            param_hashes = self.get_param_hashes(fields, values, lens, num_combinations);
            remaining = self.count_remaining(param_hashes, done_hashes);

            fprintf('[Designer] Context: %s\n', self.context_hash);
            fprintf('Total: %d, Remaining: %d\n', num_combinations, remaining);

            for idx = 1:num_combinations
                params = ctrls.designs.Designer.pick_params_static(idx, fields, values, lens);
                self.check_one(params);
            end
            self.cache.flush();
            valid_designs = self.load_valid(param_hashes);
        end

        function valid_designs = run_parallel(self, search_space)
            % RUN_PARALLEL - Explore parameter space in parallel batches
            [fields, values, lens, num_combinations] = ctrls.designs.Designer.prepare_space(search_space);

            done_hashes = self.cache.get_keys();
            param_hashes = self.get_param_hashes(fields, values, lens, num_combinations);
            remaining = self.count_remaining(param_hashes, done_hashes);

            if remaining == 0
                valid_designs = self.load_valid();
                return;
            end

            if ismissing(self.checker_func)
                error("Designer is read-only without checker_func specified. Can't check new design");
            end

            fprintf('[Designer] Context: %s\n', self.context_hash);
            fprintf('Total: %d, Remaining: %d\n', num_combinations, remaining);

            n_batches = ceil(num_combinations / self.worker_batch_size);
            result_q = parallel.pool.DataQueue;
            afterEach(result_q, @(d) self.handle_result(d));

            progress_q = parallel.pool.DataQueue;
            tracker = ctrls.utils.ProgressTracker(num_combinations, ...
                num_combinations - remaining);
            afterEach(progress_q, @(~) self.on_progress(tracker));

            parfor b = 1:n_batches
                batch_start = (b-1)*self.worker_batch_size + 1;
                batch_end = min(b*self.worker_batch_size, num_combinations);

                local_fields = fields;
                local_values = values;
                local_lens = lens;
                local_done = done_hashes;
                local_checker = self.checker_func;

                for idx = batch_start:batch_end
                    params = ctrls.designs.Designer.pick_params_static(idx, ...
                        local_fields, local_values, local_lens);
                    h = ctrls.utils.DataHash(params);
                    if any(strcmp(h, local_done))
                        continue;
                    end

                    data = struct('hash', h, 'type', 'invalid');
                    [payload, valid] = local_checker(params);
                    data.payload = payload;
                    if valid
                        data.type = 'valid';
                    end
                    send(result_q, data);
                    send(progress_q, []);
                end
            end
            self.cache.flush();
            valid_designs = self.load_valid(param_hashes);
        end

        function valid_designs = load_valid(self, hashes)
            if nargin < 2 || isempty(hashes)
                [~, results] = self.cache.load_items();
            else
                % fetch only requested items (ignores missing keys)
                [results, ~] = self.cache.load_items_with_hashes(hashes);
            end
        
            % Filter for valid entries
            is_valid = cellfun(@(r) isstruct(r) && ...
                isfield(r, "type") && strcmp(r.type, "valid"), results);
        
            valid_results = results(is_valid);
            valid_designs = cellfun(@(r) r.payload, valid_results, "UniformOutput", false);
        
            fprintf('[Designer] Loaded %d valid designs from cache.\n', numel(valid_designs));
        end
    end

    methods (Access = private)
        function param_hashes = get_param_hashes(self, fields, values, lens, num_combinations)
            % preallocate as string array
            param_hashes = strings(num_combinations, 1);
        
            for idx = 1:num_combinations
                params = ctrls.designs.Designer.pick_params_static(idx, fields, values, lens);
                param_hashes(idx) = string(self.hash_params(params));
            end
        end

        function remaining = count_remaining(self, param_hashes, done_hashes)
            remaining = sum(~ismember(param_hashes, done_hashes));
        end

        function handle_result(self, data)
            % if strcmp(data.type, "valid")
            %     fprintf("Found one: %s\n", data.hash);
            % end
            try
                self.cache(data.hash) = data; %#ok<NASGU>
            catch ME
                warning('Cache append failed for hash %s: %s', data.hash, ME.message);
            end
        end

        function on_progress(~, tracker)
            tracker.tick();
            tracker.print();
        end
    end

    methods (Static, Access = private)
        function [fields, values, lens, num_combinations] = prepare_space(search_space)
            fields = fieldnames(search_space);
            values = struct2cell(search_space);
            lens = cellfun(@numel, values);
            num_combinations = prod(lens);
        end

        function params = pick_params_static(idx, fields, values, lens)
            num_params = numel(fields);
            subs = cell(1, num_params);
            [subs{:}] = ind2sub(lens, idx);
            vals = cell(1, num_params);
            for f = 1:num_params
                val = values{f}(subs{f});
                if iscell(val) && isscalar(val)
                    vals{f} = val{1};
                else
                    vals{f} = val;
                end
            end
            params = cell2struct(vals(:), fields(:), 1);
        end
    end
end
