function design_params = design_params(sample_time_s, poles)
    design_params = struct();
    design_params.sample_time_s = sample_time_s;
    design_params.poles = poles;
end