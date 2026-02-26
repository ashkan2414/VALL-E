classdef BatchedShardCache < handle
    % BatchedShardCache
    % -----------------------------------------------------------------
    % Transactional, batched + sharded cache with dictionary storage.
    % Each data item is identified by a unique hash.
    %
    % Data is only committed to disk during flush().
    % Supports staged additions, modifications, and deletions.
    %
    % R2022b+ required (dictionary class).
    %
    % Example:
    %   cache = BatchedShardCache("context1", 100, 1000, "cache_dir");
    %   cache.add("abcd1234", data);
    %   cache.flush();
    % -----------------------------------------------------------------

    properties (SetAccess = private)
        name
        base_dir
        ctx_dir
        shards_dir
        manifest_file
        flush_size
        default_shard_size
        manifest
    end

    properties (Access = private)
        new_data      % dictionary(hash -> data)
        changed_data  % dictionary(hash -> data)
        removed_data  % dictionary(hash -> true)
    end

    %==================================================================
    % Constructor & Public API
    %==================================================================
    methods
        function obj = BatchedShardCache(name, flush_size, default_shard_size, base_dir)
            if nargin < 4, base_dir = "cache"; end
            if nargin < 3, default_shard_size = 1000; end
            if nargin < 2, flush_size = 100; end

            obj.name = string(name);
            obj.flush_size = flush_size;
            obj.default_shard_size = default_shard_size;

            obj.base_dir = char(base_dir);
            obj.ctx_dir = fullfile(obj.base_dir, obj.name);
            obj.shards_dir = fullfile(obj.ctx_dir, "shards");
            obj.manifest_file = fullfile(obj.ctx_dir, "manifest.mat");

            obj.ensure_dirs_exist();
            obj.initialize_dictionaries();
            obj.manifest = obj.load_or_init_manifest();

            fprintf('[Cache] "%s" initialized (flush=%d, shard=%d)\n', ...
                obj.name, obj.flush_size, obj.manifest.shard_size);
        end

        function add(obj, hash, data, overwrite)
            if nargin < 3
                error('Usage: cache.add(hash, data, [overwrite])');
            end
            if nargin < 4
                overwrite = true;
            end
            hash = string(hash);

            if isKey(obj.removed_data, hash)
                remove(obj.removed_data, hash);
            end

            on_disk = isKey(obj.manifest.hash_to_shard, hash);
            in_new  = isKey(obj.new_data, hash);
            in_chg  = isKey(obj.changed_data, hash);

            if overwrite
                if on_disk
                    obj.changed_data(hash) = data;
                    if in_new, remove(obj.new_data, hash); end
                else
                    obj.new_data(hash) = data;
                    if in_chg, remove(obj.changed_data, hash); end
                end
            else
                if ~(on_disk || in_new || in_chg)
                    obj.new_data(hash) = data;
                end
            end

            if obj.pending_count() >= obj.flush_size
                obj.flush();
            end
        end

        function obj = subsasgn(obj, S, value)
            % Overload "my_obj(hash) = data" to behave like add(hash, data)
            switch S(1).type
                case '()'
                    key = string(S(1).subs{1});
                    if isscalar(S)
                        % Direct assignment: cache("hash") = data
                        obj.add(key, value);
                    else
                        error('Nested indexing not supported.');
                    end
                otherwise
                    % Fall back to default behavior (for .method calls, etc.)
                    obj = builtin('subsasgn', obj, S, value);
            end
        end

        function varargout = subsref(obj, S)
            % SUBSREF - Supports:
            %   - Value access: cache("hash")
            %   - Normal method/property access: cache.method(...), cache.prop
            %   - Correctly forwards multiple outputs for methods
        
            switch S(1).type
                case '()'
                    % Dictionary-like access: cache("hash")
                    if numel(S(1).subs) ~= 1
                        error('Invalid number of subscripts. Use cache("hash") to access items.');
                    end
                    if nargout > 1
                        error('Too many output arguments for value access via () indexing.');
                    end
        
                    key = string(S(1).subs{1});
                    val = obj.get(key);
        
                    % Allow chaining (e.g., cache("x").field)
                    if numel(S) > 1
                        varargout{1} = builtin('subsref', val, S(2:end));
                    else
                        varargout{1} = val;
                    end
        
                otherwise
                    % Methods/properties: forward exactly the caller's expected outputs
                    [varargout{1:nargout}] = builtin('subsref', obj, S);
            end
        end

        function remove(obj, hash)
            hash = string(hash);
        
            % If it’s a newly staged item, just drop it (never flushed)
            if isKey(obj.new_data, hash)
                remove(obj.new_data, hash);
                return;
            end
        
            % If it’s a changed item, it still exists on disk → mark for removal
            if isKey(obj.changed_data, hash)
                remove(obj.changed_data, hash);
            end
        
            % If it exists on disk (manifest), mark for removal
            if isKey(obj.manifest.hash_to_shard, hash)
                obj.removed_data(hash) = true;
            end
        end

        function flush(obj)
            if obj.pending_count() == 0 && obj.removed_count() == 0
                return;
            end

            % Merge remove & change operations for each shard
            ops_by_shard = obj.collect_shard_operations();

            % Apply all changes per shard in one pass
            shard_ids = keys(ops_by_shard);
            for i = 1:numel(shard_ids)
                sid = shard_ids(i);
                ops = ops_by_shard(sid);
                shard_dict = obj.load_shard(sid);

                % Apply removals
                obj.apply_removals(shard_dict, ops.remove);

                % Apply modifications
                obj.apply_changes(shard_dict, sid, ops.change_keys, ops.change_vals);

                obj.save_shard(sid, shard_dict);
            end

            % Clear staged remove/change data
            obj.changed_data = dictionary(string.empty, struct.empty);
            obj.removed_data = dictionary(string.empty, logical.empty);

            % Add new data (reuse shards first)
            obj.commit_new_data();

            % Finalize manifest
            obj.save_manifest();
        end

        function value = get(obj, hash)
            % GET - Retrieve the value associated with a specific hash.
            % Searches in-memory buffers first, then disk shards.
            %
            % Usage:
            %   val = cache.get("abc123")
        
            hash = string(hash);
        
            % 1. If it's marked for removal, it's effectively deleted
            if isKey(obj.removed_data, hash)
                error('Key "%s" was marked for deletion and is no longer available.', hash);
            end
        
            % 2. Check changed (modified) entries
            if isKey(obj.changed_data, hash)
                value = obj.changed_data(hash);
                return;
            end
        
            % 3. Check new (unflushed) entries
            if isKey(obj.new_data, hash)
                value = obj.new_data(hash);
                return;
            end
        
            % 4. If exists on disk, locate and load
            if isKey(obj.manifest.hash_to_shard, hash)
                shard_id = obj.manifest.hash_to_shard(hash);
                shard_dict = obj.load_shard(shard_id);
        
                if isKey(shard_dict, hash)
                    value = shard_dict(hash);
                    return;
                else
                    error('Key "%s" missing in shard #%d (manifest inconsistency).', hash, shard_id);
                end
            end
        
            % 5. Not found anywhere
            error('Key "%s" not found in cache.', hash);
        end

        function [k, v] = load_items(obj)
            committed = dictionary(string.empty, struct.empty);
            files = dir(fullfile(obj.shards_dir, "shard_*.mat"));

            for i = 1:numel(files)
                S = load(fullfile(obj.shards_dir, files(i).name), "data");
                if isfield(S, "data") && isa(S.data, "dictionary")
                    d = S.data;
                    ks = keys(d);
                    vs = values(d, "cell");
                    for k = 1:numel(ks)
                        committed(ks(k)) = vs{k};
                    end
                end
            end

            obj.overlay_staged_edits(committed);
            k = keys(committed);
            v = values(committed, "cell");
        end

        function [values, found_hashes] = load_items_with_hashes(obj, query_hashes)
            % LOAD_ITEMS_WITH_HASHES (optimized)
            % Retrieves cached items for given hashes efficiently by grouping lookups by shard.
            %
            % Usage:
            %   [values, found_hashes] = cache.load_items_with_hashes(["h1","h2","h3"]);
        
            if nargin < 2
                error('You must provide an array of hashes to load.');
            end
        
            query_hashes = string(query_hashes(:));
            values = cell(size(query_hashes));
            found_mask = false(size(query_hashes));
        
            % --- Stage 1: Exclude deleted ---
            to_search = query_hashes(~isKey(obj.removed_data, query_hashes));
        
            % --- Stage 2: Look in in-memory buffers ---
            for i = 1:numel(to_search)
                h = to_search(i);
                if isKey(obj.new_data, h)
                    values{i} = obj.new_data(h);
                    found_mask(i) = true;
                    continue;
                end
                if isKey(obj.changed_data, h)
                    values{i} = obj.changed_data(h);
                    found_mask(i) = true;
                end
            end
        
            % --- Stage 3: Group remaining hashes by shard ---
            remaining = to_search(~found_mask);
            if isempty(remaining)
                found_hashes = query_hashes(found_mask);
                values = values(found_mask);
                return;
            end
        
            shard_ids = obj.lookup_shards(remaining);
            valid_mask = ~isnan(shard_ids);
            remaining = remaining(valid_mask);
            shard_ids = shard_ids(valid_mask);
        
            if isempty(remaining)
                found_hashes = query_hashes(found_mask);
                values = values(found_mask);
                return;
            end
        
            % Build shard -> list of hashes map
            unique_shards = unique(shard_ids);
            for s = 1:numel(unique_shards)
                sid = unique_shards(s);
                idxs = shard_ids == sid;
                shard_hashes = remaining(idxs);
        
                % Load once per shard
                shard_dict = obj.load_shard(sid);
                shard_keys = keys(shard_dict);
        
                % Fetch items for all hashes in this shard
                for j = 1:numel(shard_hashes)
                    h = shard_hashes(j);
                    if isKey(shard_dict, h)
                        target_idx = find(query_hashes == h, 1);
                        values{target_idx} = shard_dict(h);
                        found_mask(target_idx) = true;
                    end
                end
            end
        
            % --- Return only found ones ---
            found_hashes = query_hashes(found_mask);
            values = values(found_mask);
        end

        function hashes = get_keys(obj)
            h = keys(obj.manifest.hash_to_shard);
            if obj.new_count() > 0, h = [h; keys(obj.new_data)]; end
            if obj.changed_count() > 0, h = [h; keys(obj.changed_data)]; end
            if obj.removed_count() > 0
                rk = keys(obj.removed_data);
                mark = dictionary(h, true(1, numel(h)));
                for i = 1:numel(rk)
                    if isKey(mark, rk(i)), remove(mark, rk(i)); end
                end
                hashes = keys(mark);
            else
                hashes = unique(h);
            end
        end

        function result = contains(obj, hash)
            result = ismember(hash, obj.get_keys());
        end

        function delete(obj)
            obj.flush();
        end
    end

    %==================================================================
    % Private Implementation
    %==================================================================
    methods (Access = private)
        % ---------------- Directory & Manifest ----------------
        function ensure_dirs_exist(obj)
            if ~exist(obj.base_dir, "dir"), mkdir(obj.base_dir); end
            if ~exist(obj.ctx_dir, "dir"), mkdir(obj.ctx_dir); end
            if ~exist(obj.shards_dir, "dir"), mkdir(obj.shards_dir); end
        end

        function initialize_dictionaries(obj)
            obj.new_data = dictionary(string.empty, struct.empty);
            obj.changed_data = dictionary(string.empty, struct.empty);
            obj.removed_data = dictionary(string.empty, logical.empty);
        end

        function manifest = load_or_init_manifest(obj)
            base = struct( ...
                "next_shard_id", 1, ...
                "hash_to_shard", dictionary(string.empty, double.empty), ...
                "shard_sizes", dictionary(double.empty, double.empty), ...
                "shard_size", obj.default_shard_size, ...
                "created", datetime("now"));
            if exist(obj.manifest_file, "file")
                S = load(obj.manifest_file, "-mat");
                f = fieldnames(base);
                for i = 1:numel(f)
                    if isfield(S, f{i}), base.(f{i}) = S.(f{i}); end
                end
            end
            manifest = base;
        end

        function save_manifest(obj)
            mf = obj.manifest;
            save(obj.manifest_file, "-struct", "mf", "-v7.3");
        end

        % ---------------- Shard Utilities ----------------
        function path = shard_path(obj, shard_id)
            path = fullfile(obj.shards_dir, sprintf("shard_%06d.mat", shard_id));
        end

        function d = load_shard(obj, shard_id)
            path = obj.shard_path(shard_id);
            if exist(path, "file")
                S = load(path, "data");
                if isfield(S, "data") && isa(S.data, "dictionary")
                    d = S.data;
                else
                    d = dictionary(string.empty, struct.empty);
                end
            else
                d = dictionary(string.empty, struct.empty);
            end
        end

        function save_shard(obj, shard_id, shard_dict)
            data = shard_dict; 
            save(obj.shard_path(shard_id), "data", "-v7.3");
            obj.manifest.shard_sizes(double(shard_id)) = numel(keys(shard_dict));
        end

        function sid = start_new_shard(obj)
            sid = obj.manifest.next_shard_id;
            obj.manifest.next_shard_id = sid + 1;
            obj.manifest.shard_sizes(double(sid)) = 0;
        end

        % ---------------- Flush Subroutines ----------------
        function ops_by_shard = collect_shard_operations(obj)
            ops_by_shard = dictionary(double.empty, struct.empty);

            % Removals
            if obj.removed_count() > 0
                rem_keys = keys(obj.removed_data);
                shards = obj.lookup_shards(rem_keys);
                for i = 1:numel(rem_keys)
                    sid = shards(i);
                    if ~isfinite(sid), continue; end
                    if ~isKey(ops_by_shard, sid)
                        ops_by_shard(sid) = obj.make_empty_ops();
                    end
                    ops = ops_by_shard(sid);
                    ops.remove = [ops.remove; rem_keys(i)];
                    ops_by_shard(sid) = ops;
                end
            end

            % Changes
            if obj.changed_count() > 0
                chg_keys = keys(obj.changed_data);
                chg_vals = values(obj.changed_data, "cell");
                shards = obj.lookup_shards(chg_keys);
                for i = 1:numel(chg_keys)
                    sid = shards(i);
                    if ~isfinite(sid), continue; end
                    if ~isKey(ops_by_shard, sid)
                        ops_by_shard(sid) = obj.make_empty_ops();
                    end
                    ops = ops_by_shard(sid);
                    ops.change_keys = [ops.change_keys; chg_keys(i)];
                    ops.change_vals = [ops.change_vals; chg_vals{i}];
                    ops_by_shard(sid) = ops;
                end
            end
        end

        function ops = make_empty_ops(~)
            ops = struct('remove', strings(0,1), 'change_keys', strings(0,1), 'change_vals', {cell(0,1)});
        end

        function apply_removals(obj, shard_dict, keys_to_remove)
            for k = 1:numel(keys_to_remove)
                hk = keys_to_remove(k);
                if isKey(shard_dict, hk), remove(shard_dict, hk); end
                if isKey(obj.manifest.hash_to_shard, hk)
                    remove(obj.manifest.hash_to_shard, hk);
                end
            end
        end

        function apply_changes(obj, shard_dict, sid, change_keys, change_vals)
            for k = 1:numel(change_keys)
                hk = change_keys(k);
                shard_dict(hk) = change_vals{k};
                obj.manifest.hash_to_shard(hk) = sid;
            end
        end

        function commit_new_data(obj)
            if obj.new_count() == 0, return; end
            new_keys = keys(obj.new_data);
            new_vals = values(obj.new_data, 'cell');

            % Fill shards with space
            shard_ids = obj.sorted_shard_ids();
            i = 1;
            for sidx = 1:numel(shard_ids)
                sid = shard_ids(sidx);
                cap = obj.shard_capacity_left(sid);
                if cap <= 0, continue; end
                if i > numel(new_keys), break; end
                n = min(cap, numel(new_keys) - (i-1));
                shard_dict = obj.load_shard(sid);
                sub_keys = new_keys(i:i+n-1);
                sub_vals = new_vals(i:i+n-1);
                for k = 1:n
                    shard_dict(sub_keys(k)) = sub_vals{k};
                    obj.manifest.hash_to_shard(sub_keys(k)) = sid;
                end
                obj.save_shard(sid, shard_dict);
                i = i + n;
            end

            % Start new shards if needed
            while i <= numel(new_keys)
                sid = obj.start_new_shard();
                cap = obj.shard_capacity_left(sid);
                n = min(cap, numel(new_keys) - (i-1));
                shard_dict = dictionary(string.empty, struct.empty);
                sub_keys = new_keys(i:i+n-1);
                sub_vals = new_vals(i:i+n-1);
                for k = 1:n
                    shard_dict(sub_keys(k)) = sub_vals{k};
                    obj.manifest.hash_to_shard(sub_keys(k)) = sid;
                end
                obj.save_shard(sid, shard_dict);
                i = i + n;
            end

            obj.new_data = dictionary(string.empty, struct.empty);
        end

        % ---------------- Misc Helpers ----------------
        function overlay_staged_edits(obj, committed)
            if obj.removed_count() > 0
                rk = keys(obj.removed_data);
                for i = 1:numel(rk)
                    if isKey(committed, rk(i)), remove(committed, rk(i)); end
                end
            end
            if obj.changed_count() > 0
                ck = keys(obj.changed_data);
                cv = values(obj.changed_data, "cell");
                for i = 1:numel(ck)
                    committed(ck(i)) = cv{i};
                end
            end
            if obj.new_count() > 0
                nk = keys(obj.new_data);
                nv = values(obj.new_data, "cell");
                for i = 1:numel(nk)
                    committed(nk(i)) = nv{i};
                end
            end
        end

        function shards = lookup_shards(obj, hash_list)
            shards = NaN(numel(hash_list), 1);
            for i = 1:numel(hash_list)
                h = hash_list(i);
                if isKey(obj.manifest.hash_to_shard, h)
                    shards(i) = obj.manifest.hash_to_shard(h);
                end
            end
        end

        function ids = sorted_shard_ids(obj)
            ids = sort(keys(obj.manifest.shard_sizes));
        end

        function n = shard_capacity_left(obj, shard_id)
            if isKey(obj.manifest.shard_sizes, double(shard_id))
                cur = obj.manifest.shard_sizes(double(shard_id));
            else
                cur = 0;
            end
            n = max(0, obj.manifest.shard_size - cur);
        end

        % ---------------- Counter Utilities ----------------
        function n = pending_count(obj)
            n = obj.dict_count(obj.new_data) + obj.dict_count(obj.changed_data);
        end

        function n = new_count(obj), n = obj.dict_count(obj.new_data); end
        function n = changed_count(obj), n = obj.dict_count(obj.changed_data); end
        function n = removed_count(obj), n = obj.dict_count(obj.removed_data); end

        function n = dict_count(~, d)
            if isa(d, "dictionary"), n = numel(keys(d));
            else, n = 0; end
        end
    end
end
