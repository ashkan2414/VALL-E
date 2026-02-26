function valid_designs = get_cached_valid_designs(context_hash, cache_root)

    if ~exist(cache_root, 'dir'), return; end

    cache_name = sprintf('context_%s', context_hash);
    cache = ctrls.utils.BatchedShardCache(cache_name, 100, 1000, cache_root);
    [~, results] = cache.load_items();
    is_valid = cellfun(@(r) isstruct(r) && isfield(r, "type") && strcmp(r.type, "valid"), results);
    valid_results = results(is_valid);
    valid_designs = cellfun(@(r) r.payload, valid_results, "UniformOutput", false);
end