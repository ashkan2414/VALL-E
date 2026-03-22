#pragma once

#include <atomic>

#include "valle/base/system_build/build.hpp"
#include "valle/system/sync.hpp"
#include "valle/system/timing.hpp"

namespace valle::app
{
    void init();
    void start();
    void stop();

    static constexpr bool kSimulatedEncoder = false;

    static constexpr float kIntakeValveOpenPositionMM   = 0.0F;
    static constexpr float kIntakeValveClosePositionMM  = 0.0F;
    static constexpr float kExhaustValveOpenPositionMM  = 0.0F;
    static constexpr float kExhaustValveClosePositionMM = 0.0F;

    static constexpr uint32_t kPositionSampleRateHz = 300;
    static constexpr auto kPositionSamplePeriodS = DurationSecondsF(1.0F / static_cast<float>(kPositionSampleRateHz));

    static constexpr uint32_t kMainControlLoopRateHz = 30;
    static constexpr auto kMainControlLoopPeriodS = DurationSecondsF(1.0F / static_cast<float>(kMainControlLoopRateHz));

}  // namespace valle::app
