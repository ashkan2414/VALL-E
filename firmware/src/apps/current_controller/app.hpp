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
    };

    static constexpr auto   kTargetSettleTime = system::DelayMicros(150);
    static constexpr size_t kCaptureSteps     = 10;
    static constexpr auto   kCaptureDuration  = kTargetSettleTime * kCaptureSteps;
    static constexpr size_t kCaptureSamples =
        std::chrono::duration_cast<system::DelayMicros>(kCaptureDuration).count() * kVCAPWMFreqHz / 1'000'000U;

    using CurrentResponseCollectorT = system::ISRDataCollector<CurrentResponseData, kCaptureSamples, true>;

    extern CurrentResponseCollectorT g_current_response_collector;

    void init();
    void start_vca_controller();
    void stop_vca_controller();
}  // namespace valle::app
