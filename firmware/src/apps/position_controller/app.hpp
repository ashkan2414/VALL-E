#pragma once

#include <atomic>

#include "valle/base/system_build/build.hpp"
#include "valle/system/sync.hpp"
#include "valle/system/timing.hpp"

namespace valle::app
{
    static constexpr bool kSimulatedEncoder = false;

    static constexpr float kIntakeValveOpenPositionMM   = 5.77F;
    static constexpr float kIntakeValveClosePositionMM  = 0.0F;
    static constexpr float kExhaustValveOpenPositionMM  = 5.77F;
    static constexpr float kExhaustValveClosePositionMM = 0.0F;

    static constexpr uint32_t kPositionSampleRateHz = 300;
    static constexpr auto kPositionSamplePeriodS = DurationSecondsF(1.0F / static_cast<float>(kPositionSampleRateHz));

    static constexpr LDC161XChannel kResponseTargetValveChannel = kIntakeValvePositionChannel;

    struct PositionResponseData
    {
        typename system::MillisClock::time_point timestamp{};
        float                                    target_position{};
        float                                    measured_position{};
    };

    static constexpr auto   kTargetSettleTime = system::DelayMillis(100);
    static constexpr size_t kCaptureSteps     = 10;
    static constexpr auto   kCaptureDuration  = kTargetSettleTime * kCaptureSteps;
    static constexpr size_t kCaptureSamples =
        std::chrono::duration_cast<DurationSecondsF>(kCaptureDuration).count() * kPositionSampleRateHz;

    using PositionResponseCollectorT = system::ISRDataCollector<PositionResponseData, kCaptureSamples, true>;

    extern PositionResponseCollectorT g_position_response_collector;

    void init();
    void start();
    void stop();

}  // namespace valle::app
