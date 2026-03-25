#pragma once

#include <algorithm>
#include <chrono>
#include <limits>
#include <type_traits>

#include "valle/math/system/block.hpp"

namespace valle
{
    template <typename T>
    class PIdSystem : public ISystemBlock<PIdSystem<T>, T>
    {
        // Prevent truncation bugs by ensuring T is floating-point
        static_assert(std::is_floating_point_v<T>, "PIdSystem requires a floating-point type");

    private:
        // Core Parameters
        T                        m_kp{0};
        T                        m_ki{0};
        T                        m_kd{0};
        std::chrono::duration<T> m_ts{1.0};

        // Absolute Limits
        T m_output_min{std::numeric_limits<T>::lowest()};
        T m_output_max{std::numeric_limits<T>::max()};

        // Advanced Limits (Inspired by MiniPId)
        T m_max_i_output{0};      // Max contribution of I-term (0 = disabled)
        T m_output_ramp_rate{0};  // Max output change per second (0 = disabled)
        T m_output_filter{0};     // Smoothing: 0.0 (off) to ~0.99 (heavy filtering)

        // State
        T    m_integral{0};
        T    m_prev_error{0};
        T    m_prev_output{0};
        bool m_first_update{true};

    public:
        constexpr PIdSystem() = default;

        // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
        constexpr PIdSystem(T                        pid_kp,
                            T                        pid_ki,
                            T                        pid_kd,
                            std::chrono::duration<T> sample_time,
                            T                        output_min = std::numeric_limits<T>::lowest(),
                            T                        output_max = std::numeric_limits<T>::max())
            : m_kp(pid_kp)
            , m_ki(pid_ki)
            , m_kd(pid_kd)
            , m_ts(sample_time)
            , m_output_min(output_min)
            , m_output_max(output_max)
        {
        }

        constexpr void set_gains(T pid_kp, T pid_ki, T pid_kd)
        {
            m_kp = pid_kp;
            m_ki = pid_ki;
            m_kd = pid_kd;
        }

        constexpr void set_sample_time(std::chrono::duration<T> sample_time)
        {
            m_ts = sample_time;
        }

        // New: Allows configuration of advanced MiniPId features
        constexpr void set_advanced_limits(T max_i_output = T{0}, T output_ramp_rate = T{0}, T output_filter = T{0})
        {
            m_max_i_output     = std::max(T{0}, max_i_output);
            m_output_ramp_rate = std::max(T{0}, output_ramp_rate);
            m_output_filter    = std::clamp(output_filter, T{0}, T{0.99});
        }

        [[nodiscard]] constexpr T process_impl(T error)
        {
            const T dt = m_ts.count();
            if (dt <= T{0})
            {
                return m_prev_output;  // Failsafe: hold last output if dt is invalid
            }

            // Proportional Term
            const T p_term = m_kp * error;

            // Derivative Term (on Error)
            T d_term = T{0};
            if (!m_first_update)
            {
                d_term = m_kd * (error - m_prev_error) / dt;
            }

            // Integral Term with independent Max I-Output Clamping
            T new_integral = m_integral + (error * dt);

            if (m_max_i_output > T{0} && m_ki > T{0})
            {
                // Clamp the physical accumulator, not just the math, to prevent silent windup
                const T max_accum = m_max_i_output / m_ki;
                new_integral      = std::clamp(new_integral, -max_accum, max_accum);
            }

            const T i_term_candidate = m_ki * new_integral;

            // Calculate Unclamped Candidate
            T u_candidate = p_term + i_term_candidate + d_term;

            // Output Ramp Rate Limiting (Slew Limiter)
            if (m_output_ramp_rate > T{0} && !m_first_update)
            {
                // Convert per-second limit to a per-step limit using dt
                const T max_change = m_output_ramp_rate * dt;
                u_candidate        = std::clamp(u_candidate, m_prev_output - max_change, m_prev_output + max_change);
            }

            // Absolute Output Clamping
            T u = std::clamp(u_candidate, m_output_min, m_output_max);

            // Low-Pass Output Filter (Smoothing)
            if (m_output_filter > T{0} && !m_first_update)
            {
                // A weighted average between the old output and the new output
                u = (m_prev_output * m_output_filter) + (u * (T{1} - m_output_filter));
            }

            // Anti-Windup (Conditional Integration)
            // Check against absolute saturation (ignoring ramp/filter limits for windup purposes)
            const bool saturating_high = (u >= m_output_max);
            const bool saturating_low  = (u <= m_output_min);

            if ((!saturating_high && !saturating_low) || (saturating_high && error < T{0}) ||
                (saturating_low && error > T{0}))
            {
                m_integral = new_integral;
            }

            // Update State
            m_prev_error   = error;
            m_prev_output  = u;
            m_first_update = false;

            return u;
        }

        constexpr void reset_impl()
        {
            m_integral     = T{0};
            m_prev_error   = T{0};
            m_prev_output  = T{0};
            m_first_update = true;
        }
    };

}  // namespace valle