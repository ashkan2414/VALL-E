#pragma once

#include "Valle/Math/System/Blocks/biquad.hpp"

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
    T m_Ts;

public:
    inline PIDSystem() : BiQuadDF1<T>(), m_kp(0), m_ki(0), m_kd(0), m_Ts(1)
    {
    }

    inline PIDSystem(const T kp, const T ki, const T kd, const T Ts)
        : BiQuadDF1<T>(compute_coeffs(kp, ki, kd, Ts)), m_kp(kp), m_ki(ki), m_kd(kd), m_Ts(Ts)
    {
    }

    inline void set_gains(const T kp, const T ki, const T kd)
    {
        m_kp           = kp;
        m_ki           = ki;
        m_kd           = kd;
        this->coeffs() = compute_coeffs(m_kp, m_ki, m_kd, m_Ts);
        this->reinit();
    }

    inline void set_sample_time(const T Ts)
    {
        m_Ts           = Ts;
        this->coeffs() = compute_coeffs(m_kp, m_ki, m_kd, m_Ts);
        this->reinit();
    }

private:
    constexpr static inline BiQuadCoeffs<T> compute_coeffs(const T Kp, const T Ki, const T Kd, const T Ts)
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
        const T half_ts = Ts * static_cast<T>(0.5);
        const T b0      = Kp + Ki * half_ts + (Kd / Ts);
        const T b1      = Ki * Ts - static_cast<T>(2) * (Kd / Ts);
        const T b2      = -Kp + Ki * half_ts + (Kd / Ts);
        const T a0      = static_cast<T>(1);
        const T a1      = static_cast<T>(0);
        const T a2      = static_cast<T>(-1);

        return BiQuadCoeffs<T>({b0, b1, b2}, {a0, a1, a2});
    }
};