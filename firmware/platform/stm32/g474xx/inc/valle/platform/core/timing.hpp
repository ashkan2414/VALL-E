#pragma once

#include <delegate/delegate.h>
#include <stm32g4xx_hal.h>

#include <chrono>
#include <cstdint>

#include "valle/platform/core/clock.hpp"

namespace valle
{

    // 1 tick = 1 millisecond (Standard HAL resolution)
    using ms_ticks = std::chrono::duration<uint32_t, std::milli>;

    // 1 tick = 1 CPU cycle (Assuming 170MHz for G474)
    // Note: We use std::ratio<1, 170'000'000> to define the period of one cycle.
    // DWT CYCCNT is only 32 bits so at 170MHz it will overflow every ~25.3 seconds.
    // This is safe for short duration measurements up to 2^31 or ~12.6 seconds.
    // TODO: Implement overflow interrupt and keep a 64-bit extended count if needed.
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
        static constexpr bool is_steady = true;  // NOLINT(readability-identifier-naming)

        static void init()
        {
            // Nothing to do, HAL_GetTick is already initialized by HAL_Init
        }

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
        static constexpr bool is_steady = true;  // NOLINT(readability-identifier-naming)

        static void init()
        {
            static bool initialized = false;
            if (initialized)
            {
                return;
            }

            CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
            DWT->CYCCNT = 0;
            DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
            initialized = true;
        }

        static time_point now() noexcept
        {
            // Ensure DWT is enabled
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

    template <typename TDuration>
    using ClockForDuration = std::conditional_t<TDuration::period::den >= 1000, SystemClock, CycleClock>;

    // =========================================================================
    // DELAY UTILITIES
    // =========================================================================

    template <typename TDuration, typename TClock = ClockForDuration<TDuration>, bool tkBusyWait>
    inline void delay(TDuration wait_duration) noexcept
    {
        const auto end_time = TClock::now() + std::chrono::duration_cast<typename TClock::duration>(wait_duration);
        while (TClock::now() < end_time)
        {
            if constexpr (tkBusyWait)
            {
                __NOP();
            }
            else
            {
                __WFI();  // Wait For Interrupt
            }
        }
    }

    template <bool tkBusyWait>
    inline void delay_ms_base(const uint32_t ms_delay) noexcept
    {
        delay<std::chrono::milliseconds, SystemClock, tkBusyWait>(std::chrono::milliseconds(ms_delay));
    }

    inline void delay_ms(const uint32_t ms_delay) noexcept
    {
        delay_ms_base<false>(ms_delay);
    }

    inline void delay_ms_busy(const uint32_t ms_delay) noexcept
    {
        delay_ms_base<true>(ms_delay);
    }

    template <typename T, bool tkBusyWait>
        requires(std::is_convertible_v<T, uint32_t> || std::is_floating_point_v<T>)
    inline void delay_us_base(const T us_delay) noexcept
    {
        using DurationT = std::chrono::duration<T, std::micro>;
        delay<DurationT, CycleClock, tkBusyWait>(DurationT(us_delay));
    }

    template <typename T>
        requires(std::is_convertible_v<T, uint32_t> || std::is_floating_point_v<T>)
    inline void delay_us(const T us_delay) noexcept
    {
        delay_us_base<T, false>(us_delay);
    }

    template <typename T>
        requires(std::is_convertible_v<T, uint32_t> || std::is_floating_point_v<T>)
    inline void delay_us_busy(const T us_delay) noexcept
    {
        delay_us_base<T, true>(us_delay);
    }

    template <bool tkBusyWait>
    inline void delay_cycles_base(const uint32_t cycles) noexcept
    {
        delay<cycle_ticks, CycleClock, tkBusyWait>(cycle_ticks(cycles));
    }

    inline void delay_cycles(const uint32_t cycles) noexcept
    {
        delay_cycles_base<false>(cycles);
    }

    inline void delay_cycles_busy(const uint32_t cycles) noexcept
    {
        delay_cycles_base<true>(cycles);
    }

    // =========================================================================
    // WAIT UTILITIES
    // =========================================================================

    /**
     * @brief Waits for a condition to become true or until timeout duration elapses.
     *
     * @tparam TDuration The type of the timeout duration.
     * @tparam TClock The clock type used for timing.
     * @param condition The condition to wait for. It should be a delegate that returns a bool. True means the condition
     * is met.
     * @param timeout_duration The maximum duration to wait.
     * @return true If the condition became true within the timeout.
     * @return false If the timeout elapsed before the condition became true.
     */
    template <typename TDuration, typename TClock = ClockForDuration<TDuration>>
    inline bool wait_for_with_timeout(delegate::Delegate<bool>&& condition, const TDuration timeout_duration)
    {
        const auto end_time = TClock::now() + timeout_duration;
        while (TClock::now() < end_time)
        {
            if (condition())
            {
                return true;
            }
        }

        return false;
    }

    /**
     * @brief Waits for a condition to become true or until timeout in milliseconds elapses.
     *
     * @param condition The condition to wait for. It should be a delegate that returns a bool. True means the condition
     * is met.
     * @param timeout_ms The maximum time to wait in milliseconds.
     * @return true If the condition became true within the timeout.
     * @return false If the timeout elapsed before the condition became true.
     */
    inline bool wait_for_with_timeout_ms(delegate::Delegate<bool>&& condition, const uint32_t timeout_ms)
    {
        return wait_for_with_timeout<std::chrono::milliseconds, SystemClock>(std::move(condition),
                                                                             std::chrono::milliseconds(timeout_ms));
    }

    /**
     * @brief Waits for a condition to become true or until timeout in microseconds elapses.
     *
     * @param condition The condition to wait for. It should be a delegate that returns a bool. True means the condition
     * is met.
     * @param timeout_us The maximum time to wait in microseconds.
     * @return true If the condition became true within the timeout.
     * @return false If the timeout elapsed before the condition became true.
     */
    inline bool wait_for_with_timeout_us(delegate::Delegate<bool>&& condition, const uint32_t timeout_us)
    {
        return wait_for_with_timeout<std::chrono::microseconds, CycleClock>(std::move(condition),
                                                                            std::chrono::microseconds(timeout_us));
    }

    /**
     * @brief Waits for a condition to become true or until timeout in CPU cycles elapses.
     *
     * @param condition The condition to wait for. It should be a delegate that returns a bool. True means the condition
     * is met.
     * @param timeout_cycles The maximum time to wait in CPU cycles.
     * @return true If the condition became true within the timeout.
     * @return false If the timeout elapsed before the condition became true.
     */
    inline bool wait_for_with_timeout_cycles(delegate::Delegate<bool>&& condition, const uint32_t timeout_cycles)
    {
        return wait_for_with_timeout<cycle_ticks, CycleClock>(std::move(condition), cycle_ticks(timeout_cycles));
    }

}  // namespace valle