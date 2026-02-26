function design_params = design_params_spiral(sample_time_s, num_poles, rmax, center)
    poles = ctrls.spa.generate_spiral_poles(num_poles, rmax, center);
    design_params = ctrls.spa.design_params(sample_time_s, poles);
end