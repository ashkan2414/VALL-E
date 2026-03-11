#pragma once

#include "valle/utils/timing.hpp"

namespace valle::platform_support::system_backend
{
    template <typename TFunc>
    concept CTimeoutConditionFunc = requires(TFunc func) {
        { func() } -> std::same_as<bool>;
    };

    struct TimingWaitPolicyBase
    {
        static void wait()
        {
            //  __NOP();
        }

        static void busy_wait()
        {
            // __NOP();
        }
    };

    template <typename T>
    concept CTimingWaitPolicy = requires {
        { T::wait() } -> std::same_as<void>;
        { T::busy_wait() } -> std::same_as<void>;
    };

    template <CPeriod TTargetPeriod, CClock TClock>
    struct PeriodTimingTraits
    {
        using PeriodT = TTargetPeriod;
        using ClockT  = TClock;

        using SignedRepT   = typename TClock::rep;
        using UnsignedRepT = SmallestUnsignedFittingRange<SignedRepT>;

        using DurationRepT = SignedRepT;
        using DurationT    = std::chrono::duration<DurationRepT, PeriodT>;

        using DelayRepT = UnsignedRepT;
        using DelayT    = std::chrono::duration<DelayRepT, PeriodT>;

        using TimeoutRepT = UnsignedRepT;
        using TimeoutT    = std::chrono::duration<TimeoutRepT, PeriodT>;

        using TimerT = GenericTimer<PeriodT, ClockT>;
    };

    template <typename TTraits, CTimingWaitPolicy TWaitPolicy = TimingWaitPolicyBase>
    struct PeriodTimingUtils
    {
        using TraitsT     = TTraits;
        using WaitPolicyT = TWaitPolicy;

        using ClockT       = typename TraitsT::ClockT;
        using DurationRepT = typename TraitsT::DurationRepT;
        using DurationT    = typename TraitsT::DurationT;
        using DelayRepT    = typename TraitsT::DelayRepT;
        using DelayT       = typename TraitsT::DelayT;
        using TimeoutRepT  = typename TraitsT::TimeoutRepT;
        using TimeoutT     = typename TraitsT::TimeoutT;

        template <bool tkBusy>
        static void delay_base(DelayT wait_duration) noexcept
        {
            const auto end_time = ClockT::now() + std::chrono::duration_cast<typename ClockT::duration>(wait_duration);
            while (ClockT::now() < end_time)
            {
                if constexpr (tkBusy)
                {
                    TWaitPolicy::busy_wait();
                }
                else
                {
                    TWaitPolicy::wait();
                }
            }
        }

        static void delay(DelayT wait_duration) noexcept
        {
            delay_base<false>(wait_duration);
        }

        static void delay_busy(DelayT wait_duration) noexcept
        {
            delay_base<true>(wait_duration);
        }

        /**
         * @brief Waits for a condition to become true or until timeout duration elapses.
         *
         * @tparam TFunc The type of the condition function to evaluate.
         * @tparam TDuration The type of the timeout duration (e.g. std::chrono::milliseconds).
         * @param condition The condition to wait for. It should be a delegate that returns a bool. True means the
         * condition is met.
         * @param timeout_duration The maximum duration to wait.
         * @return true If the condition became true within the timeout.
         * @return false If the timeout elapsed before the condition became true.
         */
        template <CTimeoutConditionFunc TFunc>
        static bool wait_for_with_timeout(TFunc&& condition, const TimeoutT timeout_duration)
        {
            const auto end_time = ClockT::now() + timeout_duration;
            while (!condition())
            {
                if (ClockT::now() >= end_time)
                {
                    return false;
                }
            }

            return true;
        }
    };

    template <CTimingWaitPolicy TWaitPolicy, CClock... TClocks>
    struct TimingContextBase
    {
        template <CPeriod TPeriod>
        using ClockForPeriodT = ClockForPeriod<TPeriod, TClocks...>;

        template <typename TDuration>
        using ClockForDurationT = ClockForPeriodT<typename TDuration::period>;

        using WaitPolicyT = TWaitPolicy;

        using SecondClockT = ClockForPeriodT<std::ratio<1, 1>>;
        static_assert(!std::is_same_v<SecondClockT, detail::NoClock>, "No suitable clock found for seconds duration");

        using MilliClockT = ClockForPeriodT<std::milli>;
        static_assert(!std::is_same_v<MilliClockT, detail::NoClock>,
                      "No suitable clock found for milliseconds duration");

        using MicroClockT = ClockForPeriodT<std::micro>;
        static_assert(!std::is_same_v<MicroClockT, detail::NoClock>,
                      "No suitable clock found for microseconds duration");

        using SecondTraitsT = PeriodTimingTraits<std::ratio<1, 1>, SecondClockT>;
        using MilliTraitsT  = PeriodTimingTraits<std::milli, MilliClockT>;
        using MicroTraitsT  = PeriodTimingTraits<std::micro, MicroClockT>;

        // Duration types
        using DurationSecondsRepT = typename SecondTraitsT::DurationRepT;
        using DurationMillisRepT  = typename MilliTraitsT::DurationRepT;
        using DurationMicrosRepT  = typename MicroTraitsT::DurationRepT;

        using DurationSecondsT = typename SecondTraitsT::DurationT;
        using DurationMillisT  = typename MilliTraitsT::DurationT;
        using DurationMicrosT  = typename MicroTraitsT::DurationT;

        using DurationSecondsFT = std::chrono::duration<float>;
        using DurationMillisFT  = std::chrono::duration<float, std::milli>;
        using DurationMicrosFT  = std::chrono::duration<float, std::micro>;

        // Delay types
        using DelaySecondsRepT = typename SecondTraitsT::DelayRepT;
        using DelayMillisRepT  = typename MilliTraitsT::DelayRepT;
        using DelayMicrosRepT  = typename MicroTraitsT::DelayRepT;

        using DelaySecondsT = typename SecondTraitsT::DelayT;
        using DelayMillisT  = typename MilliTraitsT::DelayT;
        using DelayMicrosT  = typename MicroTraitsT::DelayT;

        // Timeout types (alias of delay types)
        using TimeoutSecondsRepT = typename SecondTraitsT::TimeoutRepT;
        using TimeoutMillisRepT  = typename MilliTraitsT::TimeoutRepT;
        using TimeoutMicrosRepT  = typename MicroTraitsT::TimeoutRepT;

        using TimeoutSecondsT = typename SecondTraitsT::TimeoutT;
        using TimeoutMillisT  = typename MilliTraitsT::TimeoutT;
        using TimeoutMicrosT  = typename MicroTraitsT::TimeoutT;

        // Timers
        using TimerSecondT = typename SecondTraitsT::TimerT;
        using TimerMilliT  = typename MilliTraitsT::TimerT;
        using TimerMicroT  = typename MicroTraitsT::TimerT;

        // --------------------------------------------------------------------------
        // DELAY UTILITIES
        // --------------------------------------------------------------------------
        using TimingUtilsSecondT = PeriodTimingUtils<SecondTraitsT, WaitPolicyT>;
        using TimingUtilsMillisT = PeriodTimingUtils<MilliTraitsT, WaitPolicyT>;
        using TimingUtilsMicrosT = PeriodTimingUtils<MicroTraitsT, WaitPolicyT>;

        template <CDelayDuration TDuration, typename TClock = ClockForDurationT<TDuration>>
        static void delay(TDuration wait_duration) noexcept
        {
            using TraitsT = PeriodTimingTraits<typename TDuration::period, TClock>;
            using UtilsT  = PeriodTimingUtils<TraitsT, WaitPolicyT>;
            UtilsT::delay(wait_duration);
        }

        template <CDelayDuration TDuration, typename TClock = ClockForDurationT<TDuration>>
        static void delay_busy(TDuration wait_duration) noexcept
        {
            using TraitsT = PeriodTimingTraits<typename TDuration::period, TClock>;
            using UtilsT  = PeriodTimingUtils<TraitsT, WaitPolicyT>;
            UtilsT::delay_busy(wait_duration);
        }

        static void delay_ms(const DelayMillisRepT ms_delay) noexcept
        {
            TimingUtilsMillisT::delay(DelayMillisT(ms_delay));
        }

        static void delay_ms_busy(const DelayMillisRepT ms_delay) noexcept
        {
            TimingUtilsMillisT::delay_busy(DelayMillisT(ms_delay));
        }

        static void delay_us(const DelayMicrosRepT us_delay) noexcept
        {
            TimingUtilsMicrosT::delay(DelayMicrosT(us_delay));
        }

        static void delay_us_busy(const DelayMicrosRepT us_delay) noexcept
        {
            TimingUtilsMicrosT::delay_busy(DelayMicrosT(us_delay));
        }

        template <std::unsigned_integral T>
        static void delay_countdown(const T delay_count)
        {
            T count = delay_count;
            while (count > 0)
            {
                std::atomic_signal_fence(std::memory_order_seq_cst);
                WaitPolicyT::busy_wait();
                --count;
            }
        }

        // --------------------------------------------------------------------------
        // TIMEOUT UTILITIES
        // --------------------------------------------------------------------------
        /**
         * @brief Waits for a condition to become true or until timeout duration elapses.
         *
         * @tparam TFunc The type of the condition function to evaluate.
         * @tparam TDuration The type of the timeout duration.
         * @tparam TClock The clock type used for timing.
         * @param condition The condition to wait for. It should be a delegate that returns a bool. True means the
         * condition is met.
         * @param timeout_duration The maximum duration to wait.
         * @return true If the condition became true within the timeout.
         * @return false If the timeout elapsed before the condition became true.
         */
        template <CTimeoutConditionFunc TFunc,
                  CTimeoutDuration      TDuration,
                  typename TClock = ClockForDurationT<TDuration>>
        [[nodiscard]] static bool wait_for_with_timeout(TFunc&& condition, const TDuration timeout_duration)
        {
            using TraitsT = PeriodTimingTraits<typename TDuration::period, TClock>;
            using UtilsT  = PeriodTimingUtils<TraitsT, WaitPolicyT>;
            return UtilsT::template wait_for_with_timeout<TFunc>(std::forward<TFunc>(condition),
                                                                 TimeoutT(timeout_duration));
        }

        /**
         * @brief Waits for a condition to become true or until timeout in milliseconds elapses.
         *
         * @tparam TFunc The type of the condition function to evaluate.
         * @param condition The condition to wait for. It should be a delegate that returns a bool. True means the
         * condition is met.
         * @param timeout_ms The maximum time to wait in milliseconds.
         * @return true If the condition became true within the timeout.
         * @return false If the timeout elapsed before the condition became true.
         */
        template <CTimeoutConditionFunc TFunc>
        [[nodiscard]] static bool wait_for_with_timeout_ms(TFunc&& condition, const TimeoutMillisRepT timeout_ms)
        {
            return TimingUtilsMillisT::template wait_for_with_timeout<TFunc>(std::forward<TFunc>(condition),
                                                                             TimeoutMillisT(timeout_ms));
        }

        /**
         * @brief Waits for a condition to become true or until timeout in microseconds elapses.
         *
         * @tparam TFunc The type of the condition function to evaluate.
         * @param condition The condition to wait for. It should be a delegate that returns a bool. True means the
         * condition is met.
         * @param timeout_us The maximum time to wait in microseconds.
         * @return true If the condition became true within the timeout.
         * @return false If the timeout elapsed before the condition became true.
         */
        template <CTimeoutConditionFunc TFunc>
        [[nodiscard]] static bool wait_for_with_timeout_us(TFunc&& condition, const TimeoutMicrosRepT timeout_us)
        {
            return TimingUtilsMicrosT::template wait_for_with_timeout<TFunc>(std::forward<TFunc>(condition),
                                                                             TimeoutMicrosT(timeout_us));
        }

        template <CTimeoutConditionFunc TFunc, std::unsigned_integral T>
        static bool wait_for_with_timeout_countdown(TFunc&& condition, const T timeout_count)
        {
            T count = timeout_count;
            while (!condition())
            {
                std::atomic_signal_fence(std::memory_order_seq_cst);
                if (count == 0)
                {
                    return false;
                }

                --count;
            }

            return true;
        }
    };

}  // namespace valle::platform_support::system_backend