#pragma once

#include <stdint.h>

#include <atomic>

namespace valle
{
    class CriticalSection
    {
    private:
        uint32_t m_primask;

    public:
        CriticalSection() : m_primask(__get_PRIMASK())
        {
            __disable_irq();
            __DSB();
            __ISB();
        }

        ~CriticalSection()
        {
            __set_PRIMASK(m_primask);
        };

        // Delete copy/move to prevent accidental double-unlocks
        CriticalSection(const CriticalSection&)            = delete;
        CriticalSection& operator=(const CriticalSection&) = delete;
        CriticalSection(CriticalSection&&)                 = delete;
        CriticalSection& operator=(CriticalSection&&)      = delete;
    };

    template <typename T>
    class Synchronized
    {
    private:
        T m_data;

    public:
        template <typename... Args>
        explicit constexpr Synchronized(Args&&... args) : m_data(std::forward<Args>(args)...)
        {
        }

        /**
         * @brief Execute a function with exclusive access to the data.
         *
         * @tparam F Lambda/Callable type
         * @param func Logic to execute while interrupts are disabled
         */
        template <typename F>
        auto with_lock(F&& func)
        {
            CriticalSection cs;  // --- LOCK ---
            return func(m_data);
        }  // --- UNLOCK (via destructor) ---

        // Prevent direct access
        T* operator->() = delete;
        T& operator*()  = delete;
    };

}  // namespace valle