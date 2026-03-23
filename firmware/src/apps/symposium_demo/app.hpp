#pragma once

#include <atomic>

#include "valle/base/system_build/build.hpp"
#include "valle/system/sync.hpp"
#include "valle/system/timing.hpp"

namespace valle::app
{

    static constexpr bool kSimulatedEncoder = false;

    static constexpr float kIntakeValveOpenPositionMM   = 5.80F;  // 5.77 in reality but lets lie a little here
    static constexpr float kIntakeValveClosePositionMM  = 0.0F;
    static constexpr float kExhaustValveOpenPositionMM  = 5.80F;
    static constexpr float kExhaustValveClosePositionMM = 0.0F;

    static constexpr uint32_t kPositionSampleRateHz = 300;
    static constexpr auto kPositionSamplePeriodS = DurationSecondsF(1.0F / static_cast<float>(kPositionSampleRateHz));

    static constexpr uint32_t kMainControlLoopRateHz = 30;
    static constexpr auto kMainControlLoopPeriodS = DurationSecondsF(1.0F / static_cast<float>(kMainControlLoopRateHz));

    // Position Response Logging Configuration
    static constexpr bool kLogPositionResponse = false;

    // Log position response every 5 seconds.
    static constexpr auto kPositionResponseLogPeriod = DurationSecondsF(1.2F * 4.0F);

    // Collect enough data points to cover log period at the given sample rate
    static constexpr uint32_t kPositionSamples =
        static_cast<uint32_t>(kPositionResponseLogPeriod.count() * kPositionSampleRateHz);

    struct PositionResponseData
    {
        typename system::MillisClock::time_point timestamp{};
        float                                    intake_position_mm;
        float                                    exhaust_position_mm;
        float                                    cycle_position_rad;
    };

    using PositionResponseCollectorT =
        std::conditional_t<kLogPositionResponse,
                           system::ISRDataCollector<PositionResponseData, kPositionSamples, true>,
                           system::ISRDataCollector<PositionResponseData, 1, true>>;
    extern PositionResponseCollectorT g_position_response_collector;

    void init();
    void start();
    void stop();

}  // namespace valle::app
