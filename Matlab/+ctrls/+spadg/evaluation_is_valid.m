function valid = evaluation_is_valid(design_evaluation)
    valid = design_evaluation.spa_results.success;
    valid = valid && design_evaluation.response_eval.meets_constraints;
    valid = valid && design_evaluation.sim_response_eval.meets_constraints;
end