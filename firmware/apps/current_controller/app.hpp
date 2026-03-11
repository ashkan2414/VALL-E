#pragma once

#include <atomic>

#include "valle/base/hardware_build/build.hpp"
#include "valle/utils/circular_queue.hpp"

namespace valle::app
{
    struct CurrentResponseData
    {
        typename CycleClock::time_point timestamp{};
        float                           target_current{};
        float                           measured_current{};
    };

    static constexpr auto   kTargetSettleTime = DelayMicros(150);
    static constexpr size_t kCaptureSteps     = 10;
    static constexpr auto   kCaptureDuration  = kTargetSettleTime * kCaptureSteps;
    static constexpr size_t kCaptureSamples =
        std::chrono::duration_cast<DelayMicros>(kCaptureDuration).count() * kVCAPWMFreqHz / 1'000'000U;

    using CurrentResponseCollectorT = DataCollector<CurrentResponseData, kCaptureSamples, true>;

    extern CurrentResponseCollectorT g_current_response_collector;

    void init();
    void start_vca_controller();
    void stop_vca_controller();
}  // namespace valle::app
