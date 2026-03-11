#pragma once

#include <chrono>

#include "valle/utils/tmp.hpp"

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

    // =========================================================================
    // Common durations
    // =========================================================================

    using DurationSecondsF = std::chrono::duration<float>;
    using DurationMillisF  = std::chrono::duration<float, std::milli>;
    using DurationMicrosF  = std::chrono::duration<float, std::micro>;

    // =========================================================================
    // Timer
    // =========================================================================
    /**
     * @brief Simple Timer class using a specified clock.
     * @tparam TClock Clock type to use.
     */
    template <CPeriod TPeriod, CClock TClock>
    class GenericTimer
    {
    public:
        using RepT      = LargestUnsignedFittingRange<typename TClock::rep>;
        using PeriodT   = TPeriod;
        using DurationT = std::chrono::duration<RepT, PeriodT>;
        using ClockT    = TClock;

    private:
        using time_point = typename TClock::time_point;

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
         * @brief Returns elapsed time converted to a specific unit (e.g. micros).
         * Usage: timer.elapsed_as<std::chrono::microseconds>()
         */
        template <CDuration TTargetDuration>
        [[nodiscard]] auto elapsed_as() const noexcept
        {
            return std::chrono::duration_cast<TTargetDuration>(TClock::now() - m_start);
        }

        /**
         * @brief Returns elapsed time in the timer's configured duration type.
         */
        [[nodiscard]] DurationT elapsed() const noexcept
        {
            return elapsed_as<DurationT>();
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

    // =========================================================================
    // Clock Selection
    // =========================================================================
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

}  // namespace valle