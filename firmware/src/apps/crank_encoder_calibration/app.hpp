#include "valle/base/system_build/build.hpp"
#include "valle/math/engine_kinematics.hpp"

namespace valle::app
{
    using EngineKinematicsCalculatorT       = EngineKinematicsCalculator<EngineKinematicsCalculatorModel::kCrankSlider>;
    using EngineKinematicsCalculatorConfigT = typename EngineKinematicsCalculatorT::ConfigT;
    extern EngineKinematicsCalculatorT g_engine_kinematics_calculator;

    void init();
}  // namespace valle::app
