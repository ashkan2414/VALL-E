#pragma once

#include "valle/platform/system_interface/sync.hpp"
#include "valle/utils/circular_queue.hpp"
#include "valle/utils/sync.hpp"


namespace valle::system
{
    using CriticalSection = platform::system_interface::CriticalSection;

    template <typename T>
    using Synchronized = Synchronized<T, CriticalSection>;

    template <typename TData, std::size_t tkCapacity, bool tkOverwrite>
    class ISRDataCollector
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
            CriticalSection critical_section;  // Ensure exclusive access to the data queue while clearing it
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
                CriticalSection critical_section;  // Ensure exclusive access to the data queue while copying it
                result = m_data_queue;             // Make a copy of the current data queue
            }

            return result;
        }
    };

}  // namespace valle::system