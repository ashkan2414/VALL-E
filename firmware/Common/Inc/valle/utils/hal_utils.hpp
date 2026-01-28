#pragma once

#include <stdint.h>

#include <atomic>

#include "stm32g4xx.h"
#include "stm32g4xx_hal_rcc.h"
#include "stm32g4xx_ll_rcc.h"

namespace valle
{

    /**
     * @brief Get the APB2 Timer Clock Frequency (TIM1, TIM8, HRTIM1)
     * @return uint32_t Frequency in Hz
     */
    inline uint32_t get_apb2_timer_clock_freq()
    {
        const uint32_t pclk2_freq     = HAL_RCC_GetPCLK2Freq();
        const uint32_t apb2_prescaler = LL_RCC_GetAPB2Prescaler();
        if (apb2_prescaler == LL_RCC_APB2_DIV_1)
        {
            return pclk2_freq;
        }

        return pclk2_freq * 2;
    }

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