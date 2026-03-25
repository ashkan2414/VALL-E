#pragma once

#include <atomic>

#include "valle/base/system_build/build.hpp"
#include "valle/system/sync.hpp"
#include "valle/system/timing.hpp"

namespace valle::app
{
    struct CurrentResponseData
    {
        typename system::MicrosClock::time_point timestamp{};
        float                                    target_current{};
        float                                    measured_current{};
        float                                    duty_cycle{};
    };

    static constexpr uint32_t kVcaPwmFreqHz     = 60000U;  // 60 kHz PWM Frequency
    static constexpr auto     kTargetSettleTime = system::DelayMicros(150);
    static constexpr size_t   kCaptureSteps     = 10;
    static constexpr auto     kCaptureDuration  = kTargetSettleTime * kCaptureSteps;
    static constexpr size_t   kCaptureSamples =
        std::chrono::duration_cast<system::DelayMicros>(kCaptureDuration).count() * kVcaPwmFreqHz / 1'000'000U;

    using CurrentResponseCollectorT = system::ISRDataCollector<CurrentResponseData, kCaptureSamples, true>;

    extern CurrentResponseCollectorT g_current_response_collector;

    void init();
}  // namespace valle::app
