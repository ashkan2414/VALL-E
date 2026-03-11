#pragma once

#include "valle/math/system/block.hpp"

namespace valle::system
{
    template <typename T>
    class PIDSystem : public ISystemBlock<PIDSystem<T>, T>
    {
    private:
        T                        m_kp           = static_cast<T>(0.0);
        T                        m_ki           = static_cast<T>(0.0);
        T                        m_kd           = static_cast<T>(0.0);
        std::chrono::duration<T> m_ts           = std::chrono::duration<T>(1.0);
        T                        m_integral     = static_cast<T>(0.0);  // Integral accumulator
        T                        m_prev_error   = static_cast<T>(0.0);  // Previous error (for derivative)
        bool                     m_first_update = true;                 // Flag to handle first update (no prev error)

    public:
        PIDSystem() = default;

        // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
        PIDSystem(const T pid_kp, const T pid_ki, const T pid_kd, const std::chrono::duration<T> sample_time)
            : m_kp(pid_kp), m_ki(pid_ki), m_kd(pid_kd), m_ts(sample_time)
        {
        }

        void set_gains(const T pid_kp, const T pid_ki, const T pid_kd)
        {
            m_kp = pid_kp;
            m_ki = pid_ki;
            m_kd = pid_kd;
        }

        void set_sample_time(const std::chrono::duration<T> sample_time)
        {
            m_ts = sample_time;
        }

        [[nodiscard]] T process_impl(const T error)
        {
            // Proportional term
            T p_term = m_kp * error;

            // Integral term
            m_integral += error * m_ts.count();
            T i_term = m_ki * m_integral;

            // Derivative term
            T d_term = T{};
            if (!m_first_update)
            {
                d_term = m_kd * (error - m_prev_error) / m_ts.count();
            }
            else
            {
                m_first_update = false;  // skip derivative first step
            }

            m_prev_error = error;

            return p_term + i_term + d_term;
        }

        void reset_impl()
        {
            m_integral     = static_cast<T>(0.0);
            m_prev_error   = static_cast<T>(0.0);
            m_first_update = true;
        }
    };

}  // namespace valle::system