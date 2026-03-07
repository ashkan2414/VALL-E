#pragma once

#include <atomic>

#include "valle/core/system/hardware.hpp"
#include "valle/utils/circular_queue.hpp"

namespace valle::app
{
    struct CurrentResponseData
    {
        typename CycleClock::time_point timestamp{};
        float                           target_current{};
        float                           measured_current{};
    };

    template <typename TData, std::size_t tkCapacity, bool tkOverwrite>
    class DataCollector
    {
    public:
        using DataT  = TData;
        using QueueT = CircularQueue<DataT, tkCapacity, tkOverwrite>;

    private:
        Atomic<bool> m_capturing{};
        QueueT       m_data_queue{};

    public:
        void start_capture()
        {
            CriticalSection cs;
            m_data_queue.clear();
            m_capturing.store(true, std::memory_order_release);
        }

        void stop_capture()
        {
            m_capturing.store(false, std::memory_order_release);
        }

        [[nodiscard]] bool is_capturing() const
        {
            return m_capturing.load(std::memory_order_acquire);
        }

        bool push_data(const TData& data)
        {
            if (!is_capturing())
            {
                return false;
            }

            (void)m_data_queue.push(data);
            return true;
        }

        [[nodiscard]] QueueT get_captured_data() const
        {
            QueueT result;

            {
                CriticalSection cs;
                result = m_data_queue;  // Make a copy of the current data queue
            }

            return result;
        }
    };

    constexpr uint32_t kVCAPWMFreqHz          = 60000U;         // 60 kHz PWM Frequency
    constexpr uint32_t kCurrentSamplingFreqHz = kVCAPWMFreqHz;  // Sample at PWM frequency for synchronization

    static constexpr auto   kTargetSettleTime = DurationMicros(150);
    static constexpr size_t kCaptureSteps     = 10;
    static constexpr auto   kCaptureDuration  = kTargetSettleTime * kCaptureSteps;
    static constexpr size_t kCaptureSamples =
        std::chrono::duration_cast<DurationMicros>(kCaptureDuration).count() * kVCAPWMFreqHz / 1'000'000U;

    using CurrentResponseCollectorT = DataCollector<CurrentResponseData, kCaptureSamples, true>;

    extern CurrentResponseCollectorT g_current_response_collector;

    void init();
}  // namespace valle::app
