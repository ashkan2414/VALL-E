#pragma once

#include <stm32g4xx_hal.h>

#include <chrono>
#include <cstdint>

#include "valle/base/platform_support/system_backend/timing.hpp"
#include "valle/platform/core/clock.hpp"
#include "valle/platform/core/config.hpp"
#include "valle/platform/hardware/rcc.hpp"


namespace valle::platform
{
    // =========================================================================
    // CLOCKS
    // =========================================================================
    using PlatformTimingConfigT          = typename PlatformCTConfigTraitsT::TimingConfigT;
    constexpr auto kPlatformTimingConfig = kPlatformCTConfig.timing_config;

    /**
     * @brief System Clock based on HAL_GetTick().
     * Provides time in milliseconds since system start.
     */
    struct SystemClock
    {
        using rep                       = int64_t;
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
            return time_point(duration(static_cast<rep>(static_cast<uint64_t>(HAL_GetTick()))));
        }
    };

    /**
     * @brief Clock based on CPU cycles using DWT CYCCNT register.
     * @note DWT must be enabled for this to work.
     */
    struct CycleClock
    {
        using rep                       = int64_t;
        using period                    = std::ratio<1, kPlatformTimingConfig.core_clock_freq_hz>;
        using duration                  = std::chrono::duration<rep, period>;
        using time_point                = std::chrono::time_point<CycleClock>;
        static constexpr bool is_steady = true;  // NOLINT(readability-identifier-naming)

        static void init()
        {
            expect(RCCInterface::get_hclk_freq_hz() == kPlatformTimingConfig.core_clock_freq_hz,
                   "CycleClock requires HCLK to be equal to core clock frequency");

            CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
            DWT->CYCCNT = 0;
            DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
        }

        static time_point now() noexcept
        {
            return time_point(duration(static_cast<rep>(static_cast<uint64_t>(DWT->CYCCNT))));
        }
    };

    // =========================================================================
    // WAIT POLICY
    // =========================================================================

    struct TimingWaitPolicy
    {
        static void wait()
        {
            __WFI();
        }

        static void busy_wait()
        {
            __NOP();
        }
    };

    // =========================================================================
    // TIMING CONTEXT
    // =========================================================================

    struct TimingContext
        : public platform_support::system_backend::TimingContextBase<TimingWaitPolicy, SystemClock, CycleClock>
    {
        // =========================================================================
        // CYCLE UTILS
        // =========================================================================
        using CyclesTraitsT =
            platform_support::system_backend::PeriodTimingTraits<typename CycleClock::period, CycleClock>;

        using DurationCyclesRepT = typename CyclesTraitsT::DurationRepT;
        using DurationCyclesT    = typename CyclesTraitsT::DurationT;

        using DelayCyclesRepT = typename CyclesTraitsT::DelayRepT;
        using DelayCyclesT    = typename CyclesTraitsT::DelayT;

        using TimeoutCyclesRepT = typename CyclesTraitsT::TimeoutRepT;
        using TimeoutCyclesT    = typename CyclesTraitsT::TimeoutT;

        using TimerCycleT = typename CyclesTraitsT::TimerT;

        using TimingUtilsCycleT = platform_support::system_backend::PeriodTimingUtils<CyclesTraitsT, TimingWaitPolicy>;

        static void delay_cycles(const DelayCyclesRepT cycles_delay) noexcept
        {
            TimingUtilsCycleT::delay(DelayCyclesT(cycles_delay));
        }

        static void delay_cycles_busy(const DelayCyclesRepT cycles_delay) noexcept
        {
            TimingUtilsCycleT::delay_busy(DelayCyclesT(cycles_delay));
        }

        /**
         * @brief Waits for a condition to become true or until timeout in cycles.
         *
         * @tparam TFunc The type of the condition function to evaluate.
         * @param condition The condition to wait for. It should be a delegate that returns a bool. True means the
         * condition is met.
         * @param timeout_cycles The maximum time to wait in cycles.
         * @return true If the condition became true within the timeout.
         * @return false If the timeout elapsed before the condition became true.
         */
        template <CTimeoutConditionFunc TFunc>
        [[nodiscard]] static bool wait_for_with_timeout_cycles(TFunc&&                 condition,
                                                               const TimeoutCyclesRepT timeout_cycles)
        {
            return TimingUtilsCycleT::template wait_for_with_timeout<TFunc>(std::forward<TFunc>(condition),
                                                                            TimeoutCyclesT(timeout_cycles));
        }
    };

}  // namespace valle::platform