#pragma once

#include "Valle/Math/System/Blocks/biquad.hpp"

namespace valle
{

    //==================================================
    // PIDSystem: builds a 1-section SOS via Tustin
    // C(s) = Kp + Ki/s + Kd*s  → bilinear discretization
    // Resulting TF: C(z) = (b0 + b1 z^-1) / (1 + a1 z^-1)
    // packed as SOS row: [b0 b1 b2 a1 a2] with b2=0, a2=0
    //==================================================
    template <typename T>
    class PIDSystem : public BiQuadDF1<T>
    {
    private:
        T m_kp;
        T m_ki;
        T m_kd;
        T m_ts_s;

    public:
        PIDSystem() : BiQuadDF1<T>(), m_kp(0), m_ki(0), m_kd(0), m_ts_s(1)
        {
        }

        PIDSystem(const T pid_kp, const T pid_ki, const T pid_kd, const T sample_time_s)
            : BiQuadDF1<T>(compute_coeffs(pid_kp, pid_ki, pid_kd, sample_time_s))
            , m_kp(pid_kp)
            , m_ki(pid_ki)
            , m_kd(pid_kd)
            , m_ts_s(sample_time_s)
        {
        }

        void set_gains(const T pid_kp, const T pid_ki, const T pid_kd)
        {
            m_kp = pid_kp;
            m_ki = pid_ki;
            m_kd = pid_kd;
            this->set_coeffs(compute_coeffs(m_kp, m_ki, m_kd, m_ts_s));
        }

        void set_sample_time(const T sample_time_s)
        {
            m_ts_s = sample_time_s;
            this->set_coeffs(compute_coeffs(m_kp, m_ki, m_kd, m_ts_s));
        }

    private:
        // NOLINTNEXTLINE(readability-identifier-length)
        constexpr static BiQuadCoeffs<T> compute_coeffs(const T pid_kp,
                                                        const T pid_ki,
                                                        const T pid_kd,
                                                        const T sample_time_s)
        {
            // Bilinear (Tustin):
            // C(z) = Kp
            //      + Ki*(Ts/2) * (1 + z^-1)/(1 - z^-1)
            //      + (Kd/Ts)   * (1 - z^-1)/(1 + z^-1)
            // → common denom D(z) = (1 - z^-1)(1 + z^-1) = 1 - z^-2
            // Numerator after collecting terms:
            // b0 =  Kp + Ki*Ts/2 + Kd/Ts
            // b1 =  Ki*Ts        - 2*(Kd/Ts)
            // b2 = -Kp + Ki*Ts/2 + Kd/Ts
            // Denominator: 1 + a1 z^-1 + a2 z^-2 with a1=0, a2=-1
            const T half_ts = sample_time_s * static_cast<T>(0.5);

            const T biquad_coeff_b0 = pid_kp + (pid_ki * half_ts) + (pid_kd / sample_time_s);
            const T biquad_coeff_b1 = (pid_ki * sample_time_s) - (static_cast<T>(2) * (pid_kd / sample_time_s));
            const T biquad_coeff_b2 = -pid_kp + (pid_ki * half_ts) + (pid_kd / sample_time_s);
            const T biquad_coeff_a0 = static_cast<T>(1);
            const T biquad_coeff_a1 = static_cast<T>(0);
            const T biquad_coeff_a2 = static_cast<T>(-1);

            return BiQuadCoeffs<T>({biquad_coeff_b0, biquad_coeff_b1, biquad_coeff_b2},
                                   {biquad_coeff_a0, biquad_coeff_a1, biquad_coeff_a2});
        }
    };

}  // namespace valle