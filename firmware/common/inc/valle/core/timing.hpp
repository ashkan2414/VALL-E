#pragma once

#include <atomic>
#include <chrono>

#include "valle/utils/template_utils.hpp"

namespace valle
{

    namespace detail
    {
        // Helper to check if a type is a std::ratio
        template <typename T>
        struct IsRatio : std::false_type
        {
        };
        template <intmax_t N, intmax_t D>
        struct IsRatio<std::ratio<N, D>> : std::true_type
        {
        };
    }  // namespace detail

    template <typename T>
    concept CPeriod = detail::IsRatio<T>::value;

    template <typename T>
    concept CDurationRep = std::is_integral_v<T> && !std::is_floating_point_v<T>;

    template <typename T>
    concept CDuration = CDurationRep<typename T::rep> && CPeriod<typename T::period> &&
                        std::is_same_v<T, std::chrono::duration<typename T::rep, typename T::period>>;

    template <typename T>
    concept CClock =
        requires {
            // Must have the required internal types
            typename T::rep;
            typename T::period;
            typename T::duration;
            typename T::time_point;

            // is_steady must be a constexpr boolean
            { T::is_steady } -> std::convertible_to<bool>;

            // now() must be a static function returning the time_point
            { T::now() } -> std::same_as<typename T::time_point>;
        } &&
        // Rep must be signed integer and not floating point
        CDurationRep<typename T::rep> && std::is_signed_v<typename T::rep> &&
        // Period must be a std::ratio
        CPeriod<typename T::period> &&
        // Duration must be std::chrono::duration<rep, period>
        CDuration<typename T::duration> &&
        // duration must be std::chrono::duration<rep, period>
        std::same_as<typename T::duration, std::chrono::duration<typename T::rep, typename T::period>> &&
        // time_point must be std::chrono::time_point<T, duration>
        std::same_as<typename T::time_point, std::chrono::time_point<T, typename T::duration>>;

    template <typename T>
    concept CDelayRep = std::is_integral_v<T> && std::is_unsigned_v<T> && !std::is_floating_point_v<T>;

    template <typename TDuration>
    concept CDelayDuration = CDelayRep<typename TDuration::rep>;

    template <typename T>
    concept CTimeoutRep = CDelayRep<T>;

    template <typename TDuration>
    concept CTimeoutDuration = CTimeoutRep<typename TDuration::rep>;

    template <typename TFunc>
    concept CConditionFunc = requires(TFunc func) {
        { func() } -> std::same_as<bool>;
    };

    /**
     * @brief Simple Timer class using a specified clock.
     * @tparam TClock Clock type to use.
     */
    template <CClock TClock>
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
        template <CDuration TTargetDuration>
        [[nodiscard]] auto elapsed_as() const noexcept
        {
            return std::chrono::duration_cast<TTargetDuration>(elapsed());
        }

        /**
         * @brief Check if a duration has passed.
         * Accepts any chrono duration (seconds, ms, us, etc.)
         */
        template <CDuration TDuration>
        [[nodiscard]] bool has_passed(TDuration limit) const noexcept
        {
            return elapsed() >= limit;
        }
    };

    struct WaitPolicyBase
    {
        static void wait()
        {
            __NOP();
        }

        static void busy_wait()
        {
            __NOP();
        }
    };

    template <typename T>
    concept CWaitPolicy = requires {
        { T::wait() } -> std::same_as<void>;
        { T::busy_wait() } -> std::same_as<void>;
    };

    template <CClock TClock, CPeriod TTargetPeriod>
    struct ClockTimingTraits
    {
        using ClockT  = TClock;
        using PeriodT = TTargetPeriod;

        using ClockPeriodT = typename TClock::period;
        using SignedRepT   = typename TClock::rep;
        using UnsignedRepT = SmallestUnsignedFittingRange<SignedRepT>;

        using DurationRepT = SignedRepT;
        using DurationT    = std::chrono::duration<DurationRepT, PeriodT>;

        using DelayRepT = UnsignedRepT;
        using DelayT    = std::chrono::duration<DelayRepT, PeriodT>;

        using TimeoutRepT = UnsignedRepT;
        using TimeoutT    = std::chrono::duration<TimeoutRepT, PeriodT>;
    };

    template <typename TTraits, CWaitPolicy TWaitPolicy = WaitPolicyBase>
    struct ClockTimingUtils
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
        template <CConditionFunc TFunc>
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

    namespace detail
    {
        // Helper to represent a "null" result if no clock matches the criteria
        struct NoClock
        {
            using period = std::ratio<0>;
        };

        // Meta-function to decide which of two clocks is "better" based on the criteria:
        // 1. Must have period >= TTargetPeriod
        // 2. Must have the smallest period among those that satisfy (1)
        template <CPeriod TTargetPeriod, typename TClockA, typename TClockB>
        struct SelectBetter
        {
            // Check if clocks satisfy the "<= TTargetPeriod" constraint
            static constexpr bool skAOk =
                !std::is_same_v<TClockA, NoClock> && std::ratio_less_equal_v<typename TClockA::period, TTargetPeriod>;
            static constexpr bool skBOk =
                !std::is_same_v<TClockB, NoClock> && std::ratio_less_equal_v<typename TClockB::period, TTargetPeriod>;

            using type = std::conditional_t<
                !skAOk && !skBOk,
                NoClock,  // Neither matches
                std::conditional_t<
                    skAOk && !skBOk,
                    TClockA,  // Only A matches
                    std::conditional_t<
                        !skAOk && skBOk,
                        TClockB,  // Only B matches
                        std::conditional_t<std::ratio_greater_v<typename TClockA::period, typename TClockB::period>,
                                           TClockA,
                                           TClockB  // Both fit: Pick the one with the LARGER period (coarser)
                                           >>>>;
        };

        // Recursive implementation for the variadic pack
        template <CPeriod TPeriod, CClock... TClocks>
        struct ClockForPeriodImpl;

        // Base case: Single clock
        template <CPeriod TPeriod, CClock TClock>
        struct ClockForPeriodImpl<TPeriod, TClock>
        {
            using type = std::conditional_t<std::ratio_less_equal_v<typename TClock::period, TPeriod>, TClock, NoClock>;
        };

        // Recursive case
        template <CPeriod TPeriod, CClock THead, CClock... TTail>
        struct ClockForPeriodImpl<TPeriod, THead, TTail...>
        {
            using type =
                typename SelectBetter<TPeriod, THead, typename ClockForPeriodImpl<TPeriod, TTail...>::type>::type;
        };

    }  // namespace detail

    template <CPeriod TPeriod, CClock... TClocks>
    using ClockForPeriod = typename detail::ClockForPeriodImpl<TPeriod, TClocks...>::type;

    template <CWaitPolicy TWaitPolicy, CClock... TClocks>
    struct TimingUtilsBase
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

        using SecondTraitsT = ClockTimingTraits<SecondClockT, std::ratio<1, 1>>;
        using MilliTraitsT  = ClockTimingTraits<MilliClockT, std::milli>;
        using MicroTraitsT  = ClockTimingTraits<MicroClockT, std::micro>;

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

        // --------------------------------------------------------------------------
        // DELAY UTILITIES
        // --------------------------------------------------------------------------
        using TimingUtilsSecondT = ClockTimingUtils<SecondTraitsT, WaitPolicyT>;
        using TimingUtilsMillisT = ClockTimingUtils<MilliTraitsT, WaitPolicyT>;
        using TimingUtilsMicrosT = ClockTimingUtils<MicroTraitsT, WaitPolicyT>;

        template <CDelayDuration TDuration, typename TClock = ClockForDurationT<TDuration>>
        static void delay(TDuration wait_duration) noexcept
        {
            using TraitsT = ClockTimingTraits<TClock, typename TDuration::period>;
            using UtilsT  = ClockTimingUtils<TraitsT, WaitPolicyT>;
            UtilsT::delay(wait_duration);
        }

        template <CDelayDuration TDuration, typename TClock = ClockForDurationT<TDuration>>
        static void delay_busy(TDuration wait_duration) noexcept
        {
            using TraitsT = ClockTimingTraits<TClock, typename TDuration::period>;
            using UtilsT  = ClockTimingUtils<TraitsT, WaitPolicyT>;
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
        template <CConditionFunc TFunc, CTimeoutDuration TDuration, typename TClock = ClockForDurationT<TDuration>>
        [[nodiscard]] static bool wait_for_with_timeout(TFunc&& condition, const TDuration timeout_duration)
        {
            using TraitsT = ClockTimingTraits<TClock, typename TDuration::period>;
            using UtilsT  = ClockTimingUtils<TraitsT, WaitPolicyT>;
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
        template <CConditionFunc TFunc>
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
        template <CConditionFunc TFunc>
        [[nodiscard]] static bool wait_for_with_timeout_us(TFunc&& condition, const TimeoutMicrosRepT timeout_us)
        {
            return TimingUtilsMicrosT::template wait_for_with_timeout<TFunc>(std::forward<TFunc>(condition),
                                                                             TimeoutMicrosT(timeout_us));
        }

        template <CConditionFunc TFunc, std::unsigned_integral T>
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

    using DurationSecondsF = std::chrono::duration<float>;
    using DurationMillisF  = std::chrono::duration<float, std::milli>;
    using DurationMicrosF  = std::chrono::duration<float, std::micro>;

}  // namespace valle