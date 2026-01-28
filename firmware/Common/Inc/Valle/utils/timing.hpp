#pragma once

#include <chrono>
#include <cstdint>

#include "stm32g4xx_hal.h"

namespace valle
{
    constexpr uint32_t kSystemClockFreqHz = 170'000'000;  // 170 MHz

    // 1 tick = 1 millisecond (Standard HAL resolution)
    using ms_ticks = std::chrono::duration<uint32_t, std::milli>;

    // 1 tick = 1 CPU cycle (Assuming 170MHz for G474)
    // Note: We use std::ratio<1, 170'000'000> to define the period of one cycle.
    using cycle_ticks = std::chrono::duration<uint32_t, std::ratio<1, kSystemClockFreqHz>>;

    // =========================================================================
    // CLOCKS
    // =========================================================================

    /**
     * @brief System Clock based on HAL_GetTick().
     * Provides time in milliseconds since system start.
     */
    struct SystemClock
    {
        using rep                       = uint32_t;
        using period                    = std::milli;  // 1/1000 seconds
        using duration                  = std::chrono::duration<rep, period>;
        using time_point                = std::chrono::time_point<SystemClock>;
        static constexpr bool is_steady = true;

        static time_point now() noexcept
        {
            return time_point(duration(HAL_GetTick()));
        }
    };

    /**
     * @brief Clock based on CPU cycles using DWT CYCCNT register.
     * @note DWT must be enabled for this to work.
     */
    struct CycleClock
    {
        using rep                       = uint32_t;
        using period                    = std::ratio<1, kSystemClockFreqHz>;  // 1 cycle @ 170MHz
        using duration                  = std::chrono::duration<rep, period>;
        using time_point                = std::chrono::time_point<CycleClock>;
        static constexpr bool is_steady = true;

        static time_point now() noexcept
        {
            // Ensure DWT is enabled
            if (!(CoreDebug->DEMCR & CoreDebug_DEMCR_TRCENA_Msk))
            {
                CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
                DWT->CYCCNT = 0;
                DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
            }
            return time_point(duration(DWT->CYCCNT));
        }
    };

    // =========================================================================
    // TIMER
    // =========================================================================

    /**
     * @brief Simple Timer class using a specified clock.
     * @tparam TClock Clock type to use (default: SystemClock).
     */
    template <typename TClock = SystemClock>
    class GenericTimer
    {
    public:
        using time_point = typename TClock::time_point;
        using duration   = typename TClock::duration;

    private:
        time_point m_start;

    public:
        /**
         * @brief Constructs and starts the timer.
         */
        constexpr GenericTimer() noexcept : m_start(TClock::now())
        {
        }

        /**
         * @brief Resets the timer to the current time.
         */
        void reset() noexcept
        {
            m_start = TClock::now();
        }

        /**
         * @brief Returns elapsed time in the clock's native ticks.
         */
        [[nodiscard]] duration elapsed() const noexcept
        {
            return TClock::now() - m_start;
        }

        /**
         * @brief Returns elapsed time converted to a specific unit (e.g. micros).
         * Usage: timer.elapsed_as<std::chrono::microseconds>()
         */
        template <typename TTargetDuration>
        [[nodiscard]] auto elapsed_as() const noexcept
        {
            return std::chrono::duration_cast<TTargetDuration>(elapsed());
        }

        /**
         * @brief Check if a duration has passed.
         * Accepts any chrono duration (seconds, ms, us, etc.)
         */
        template <typename Rep, typename Period>
        [[nodiscard]] bool has_passed(std::chrono::duration<Rep, Period> limit) const noexcept
        {
            return elapsed() >= limit;
        }
    };

    using Timer          = GenericTimer<SystemClock>;
    using PrecisionTimer = GenericTimer<CycleClock>;

    // =========================================================================
    // DELAY UTILITIES
    // =========================================================================

    template <typename TDuration, typename TClock = SystemClock>
    inline void delay(TDuration wait_duration) noexcept
    {
        const auto end_time = TClock::now() + wait_duration;
        while (TClock::now() < end_time)
        {
            __WFI();  // Wait For Interrupt
        }
    }

    inline void delay_ms(const uint32_t ms) noexcept
    {
        delay<std::chrono::milliseconds, SystemClock>(std::chrono::milliseconds(ms));
    }

    inline void delay_us(const uint32_t us) noexcept
    {
        delay<std::chrono::microseconds, CycleClock>(std::chrono::microseconds(us));
    }
}  // namespace valle