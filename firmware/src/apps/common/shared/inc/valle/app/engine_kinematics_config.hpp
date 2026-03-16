#pragma once

#include "valle/math/engine_kinematics.hpp"

namespace valle::app
{
    static constexpr auto skPowerfistEngineKinematicsModel = EngineKinematicsCalculatorModel::kCrankSlider;
    using EngineKinematicsCalculatorT       = EngineKinematicsCalculator<skPowerfistEngineKinematicsModel>;
    using EngineKinematicsCalculatorConfigT = typename EngineKinematicsCalculatorT::ConfigT;

    static constexpr auto kPowerfistEngineKinematicsConfig = EngineKinematicsCalculatorConfigT{
        .reference_config =
            EngineKinematicsCalculatorReferenceConfig{
                .encoder_zero_at_rotor_angle_rad = 0.0F,
                .rotor_angle_at_tdc_rad          = 0.0F,
                .cylinder_pos_norm_at_tdc        = 0.0F,
                .cylinder_pos_norm_at_bdc        = 1.0F,
            },
        .model_config =
            EngineKinematicsCalculatorModelConfig<skPowerfistEngineKinematicsModel>{
                .crank_radius          = 2.17F / 2.0F,  // 2.17in stroke length -> 1.085in crank radius
                .connecting_rod_length = 3.308F,        // 3.308in connecting rod length
            },
    };
    static_assert(kPowerfistEngineKinematicsConfig.is_valid(), "Invalid PowerfistEngineKinematicsConfig provided");

}  // namespace valle::app