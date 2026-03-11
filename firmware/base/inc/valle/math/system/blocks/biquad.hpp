#pragma once

#include <Eigen/Dense>
#include <array>
#include <cmath>
#include <ranges>
#include <type_traits>

#include "valle/math/matrix.hpp"
#include "valle/math/system/block.hpp"

namespace valle::system
{

    template <typename T>
    struct BiQuadCoeffs
    {
        Eigen::Vector<T, 3> b{};
        Eigen::Vector<T, 3> a{};

        BiQuadCoeffs() = default;

        template <std::ranges::range TBRange, std::ranges::range TARange>
        BiQuadCoeffs(TBRange&& b_src, TARange&& a_src)
        {
            // Compile-Time Checks (Static Assertions)
            // -----------------------------------------------------------

            using B_T = std::remove_cvref_t<TBRange>;
            using A_T = std::remove_cvref_t<TARange>;

            // Check B: std::array, std::span (static extent), or std::tuple
            if constexpr (requires { std::tuple_size<B_T>::value; })
            {  // NOLINT(bugprone-branch-clone)
                // Note: For dynamic std::span, value is usually -1 (huge), so this passes safely.
                static_assert(std::tuple_size<B_T>::value >= 3,
                              "Numerator (b) input type has fixed size < 3! (e.g. std::array<T, 2>)");
            }
            // Check B: Raw C-Arrays (e.g. float[2])
            else if constexpr (std::is_array_v<B_T>)
            {
                static_assert(std::extent_v<B_T> >= 3, "Numerator (b) raw array size must be >= 3");
            }

            // Check A: (Same logic)
            if constexpr (requires { std::tuple_size<A_T>::value; })
            {  // NOLINT(bugprone-branch-clone)
                static_assert(std::tuple_size<A_T>::value >= 3, "Denominator (a) fixed size < 3!");
            }
            else if constexpr (std::is_array_v<A_T>)
            {
                static_assert(std::extent_v<A_T> >= 3, "Denominator (a) raw array size must be >= 3");
            }

            // -----------------------------------------------------------
            // 2. Runtime Checks (For std::vector, etc.)
            // -----------------------------------------------------------
            if (std::ranges::distance(b_src) < 3 || std::ranges::distance(a_src) < 3)
            {
                throw std::invalid_argument("BiQuadCoeffs requires size >= 3");
            }

            std::ranges::copy(b_src | std::views::take(3), b.begin());
            std::ranges::copy(a_src | std::views::take(3), a.begin());
        }

        BiQuadCoeffs(Eigen::Vector<T, 3>&& b_coeffs, Eigen::Vector<T, 3>&& a_coeffs)
            : b(std::move(b_coeffs)), a(std::move(a_coeffs))
        {
        }

        BiQuadCoeffs(const Eigen::Vector<T, 3>& b_coeffs, const Eigen::Vector<T, 3>& a_coeffs)
            : b(b_coeffs), a(a_coeffs)
        {
        }

        BiQuadCoeffs operator*(T gain) const
        {
            BiQuadCoeffs result;
            result.b = b * gain;
            result.a = a;  // a coefficients remain unchanged
            return result;
        }
    };

    template <typename T>
    struct BiQuadCoeffsNorm
    {
        Eigen::Vector<T, 3> b{};  // b coefficients normalized by a0
        Eigen::Vector<T, 2> a{};  // a1, a2 coefficients normalized by a0

        BiQuadCoeffsNorm() = default;

        template <std::ranges::range TBRange, std::ranges::range TARange>
        BiQuadCoeffsNorm(TBRange&& b_src, TARange&& a_src)
        {
            // Compile-Time Checks (Static Assertions)
            // -----------------------------------------------------------

            using B_T = std::remove_cvref_t<TBRange>;
            using A_T = std::remove_cvref_t<TARange>;

            // Check B: std::array, std::span (static extent), or std::tuple
            if constexpr (requires { std::tuple_size<B_T>::value; })
            {  // NOLINT(bugprone-branch-clone)
                // Note: For dynamic std::span, value is usually -1 (huge), so this passes safely.
                static_assert(std::tuple_size<B_T>::value >= 3,
                              "Numerator (b) input type has fixed size < 3! (e.g. std::array<T, 2>)");
            }
            // Check B: Raw C-Arrays (e.g. float[2])
            else if constexpr (std::is_array_v<B_T>)
            {
                static_assert(std::extent_v<B_T> >= 3, "Numerator (b) raw array size must be >= 3");
            }

            // Check A: (Same logic)
            if constexpr (requires { std::tuple_size<A_T>::value; })
            {  // NOLINT(bugprone-branch-clone)
                static_assert(std::tuple_size<A_T>::value >= 2, "Denominator (a) fixed size < 2!");
            }
            else if constexpr (std::is_array_v<A_T>)
            {
                static_assert(std::extent_v<A_T> >= 2, "Denominator (a) raw array size must be >= 2");
            }

            // Runtime Checks (For std::vector, etc.)
            if (std::ranges::distance(b_src) < 3 || std::ranges::distance(a_src) < 2)
            {
                throw std::invalid_argument("BiQuadCoeffs requires size >= 3 for b and >= 2 for a");
            }

            std::ranges::copy(b_src | std::views::take(3), b.begin());
            std::ranges::copy(a_src | std::views::take(2), a.begin());
        }

        BiQuadCoeffsNorm(Eigen::Vector<T, 3>&& b_coeffs, Eigen::Vector<T, 2>&& a_coeffs)
            : b(std::move(b_coeffs)), a(std::move(a_coeffs))
        {
        }

        BiQuadCoeffsNorm(const Eigen::Vector<T, 3>& b_coeffs, const Eigen::Vector<T, 2>& a_coeffs)
            : b(b_coeffs), a(a_coeffs)
        {
        }

        explicit BiQuadCoeffsNorm(const BiQuadCoeffs<T>& coeffs)
        {
            const T inv_a0 = static_cast<T>(1.0) / coeffs.a[0];

            // Normalize B
            b = coeffs.b * inv_a0;

            // Normalize A (exclude a0, it becomes 1.0)
            a = coeffs.a.template tail<2>() * inv_a0;
        }

        BiQuadCoeffsNorm operator*(T gain) const
        {
            BiQuadCoeffsNorm result;
            result.b = b * gain;
            result.a = a;  // a coefficients remain unchanged
            return result;
        }
    };

    template <typename T>
    T calculate_biquad_df1_norm(const BiQuadCoeffsNorm<T>& coeffs,
                                Eigen::Vector<T, 2>&       input_vector,
                                Eigen::Vector<T, 2>&       output_vector,
                                T                          input)
    {
        // y[n] = (b0*x[n] + b_tail . x_hist) - (a . y_hist)

        // 1. Feed-Forward (Zeros): Split b[0] * input separately so we don't have to build a temporary Vector3
        const T dot_hist    = precise_dot<T>(coeffs.b.template tail<2>(), input_vector);
        T       feedforward = 0;
        if constexpr (std::is_floating_point_v<T>)
        {
            feedforward = std::fma(input, coeffs.b[0], dot_hist);
        }
        else
        {
            feedforward = (input * coeffs.b[0]) + dot_hist;
        }

        // 2. Feed-Back (Poles): Dot product of 'a' coefficients and output history
        const T feedback = precise_dot(coeffs.a, output_vector);

        const T out = feedforward - feedback;

        // The State Shift. "Slide" the history down.
        // We set the tail (bottom) to the head (top), then update the head.

        // x[n-2] = x[n-1]; x[n-1] = input;
        input_vector[1] = input_vector[0];
        input_vector[0] = input;

        // y[n-2] = y[n-1]; y[n-1] = output;
        output_vector[1] = output_vector[0];
        output_vector[0] = out;

        return out;
    }

    // =============================================================================
    // Direct Form 1: Normalizing (Optimized for Float)
    // =============================================================================
    // Pre-calculates (b/a0) and (a/a0).
    // Fastest for floats. Avoids division in the loop.
    template <typename T>
    class BiQuadDF1Norm : public ISystemBlock<BiQuadDF1Norm<T>, T>
    {
    private:
        BiQuadCoeffsNorm<T> m_coeffs{};
        Eigen::Vector<T, 2> m_input_vector{};
        Eigen::Vector<T, 2> m_output_vector{};

    public:
        BiQuadDF1Norm() = default;

        // Initialize coefficients and apply gain immediately
        template <typename TCoeffs>
        explicit BiQuadDF1Norm(const TCoeffs& coeffs, T gain = static_cast<T>(1.0))
            : m_coeffs(BiQuadCoeffsNorm<T>(coeffs) * gain)
            , m_input_vector(Eigen::Vector<T, 2>::Zero())
            , m_output_vector(Eigen::Vector<T, 2>::Zero())
        {
        }

        template <typename TCoeffs>
        void set_coeffs(const TCoeffs& coeffs, T gain = static_cast<T>(1.0))
        {
            m_coeffs = BiQuadCoeffsNorm<T>(coeffs) * gain;
            this->reset();
        }

        T process_impl(T input)
        {
            return calculate_biquad_df1_norm(m_coeffs, m_input_vector, m_output_vector, input);
        }

        void reset_impl()
        {
            m_input_vector.setZero();
            m_output_vector.setZero();
        }
    };

    // =============================================================================
    // Direct Form 1: Non-Normalizing (Optimized for Int/Fixed)
    // =============================================================================
    // Keeps coefficients raw. Divides by a0 at the very end.
    // Essential for Integers to avoid rounding coefficients to zero.
    template <typename T>
    class BiQuadDF1NonNorm : public ISystemBlock<BiQuadDF1NonNorm<T>, T>
    {
    private:
        BiQuadCoeffsNorm<T> m_coeffs{};
        T                   m_a0{};
        Eigen::Vector<T, 2> m_input_vector{};
        Eigen::Vector<T, 2> m_output_vector{};

    public:
        template <typename TCoeffs>
        explicit BiQuadDF1NonNorm(const TCoeffs& coeffs, const T gain = 1.0)
            : m_coeffs(BiQuadCoeffsNorm<T>(coeffs) * gain)
            , m_a0(coeffs.a[0])
            , m_input_vector(Eigen::Vector<T, 2>::Zero())
            , m_output_vector(Eigen::Vector<T, 2>::Zero())
        {
        }

        template <typename TCoeffs>
        void set_coeffs(const TCoeffs& coeffs, T gain = static_cast<T>(1.0))
        {
            m_coeffs = BiQuadCoeffsNorm<T>(coeffs) * gain;
            m_a0     = coeffs.a[0];
            this->reset();
        }

        T process_impl(T input)
        {
            // Calculate output without dividing by a0
            const T out = calculate_biquad_df1_norm(m_coeffs, m_input_vector, m_output_vector, input);

            // Final division by a0
            return out / m_a0;
        }

        void reset_impl()
        {
            m_input_vector.setZero();
            m_output_vector.setZero();
        }
    };

    template <typename T>
    using BiQuadDF1 = std::conditional_t<std::is_floating_point_v<T>, BiQuadDF1Norm<T>, BiQuadDF1NonNorm<T>>;

    // =============================================================================
    // Direct Form 2: Normalizing (Optimized for Float)
    // =============================================================================
    // Uses specific "Canonical" structure. Less memory (2 state vars).
    // Normalized coefficients.
    template <typename T>
    class BiQuadDF2Norm : public ISystemBlock<BiQuadDF2Norm<T>, T>
    {
    private:
        BiQuadCoeffsNorm<T> m_coeffs;
        Eigen::Vector<T, 2> m_state_vector{};  // State variables w[n-1], w[n-2]

    public:
        template <typename TCoeffs>
        explicit BiQuadDF2Norm(const TCoeffs& coeffs, const T gain = 1.0)
            : m_coeffs(BiQuadCoeffsNorm<T>(coeffs) * gain), m_state_vector(Eigen::Vector<T, 2>::Zero())
        {
        }

        template <typename TCoeffs>
        void set_coeffs(const TCoeffs& coeffs, T gain = static_cast<T>(1.0))
        {
            m_coeffs = BiQuadCoeffsNorm<T>(coeffs) * gain;
            this->reset();
        }

        T process_impl(T input)
        {
            // Calculate Intermediate State w[n]
            // Difference Equation: w[n] = x[n] - a1*w[n-1] - a2*w[n-2]
            // Math: w[n] = input - (a_vector . w_vector)

            const T feedback  = precise_dot(m_coeffs.a, m_state_vector);
            const T new_state = input - feedback;

            // Calculate Output y[n]
            // Difference Equation: y[n] = b0*w[n] + b1*w[n-1] + b2*w[n-2]
            // Math: y[n] = (b0 * w[n]) + (b_tail . w_vector)

            const T feedforward_hist = precise_dot(m_coeffs.b.template tail<2>(), m_state_vector);

            T out;
            if constexpr (std::is_floating_point_v<T>)
            {
                // High Precision: Combine the current w[n] term with the history sum
                out = std::fma(new_state, m_coeffs.b[0], feedforward_hist);
            }
            else
            {
                out = (new_state * m_coeffs.b[0]) + feedforward_hist;
            }

            // Shift State
            m_state_vector[1] = m_state_vector[0];  // w[n-2] = w[n-1]
            m_state_vector[0] = new_state;          // w[n-1] = w[n]

            return out;
        }

        void reset_impl()
        {
            m_state_vector.setZero();
        }
    };

    // =============================================================================
    // Direct Form 2: Non-Normalizing (Optimized for Int/Fixed)
    // =============================================================================
    // Direct Form 2 Transposed is essentially required here to avoid massive
    // accumulators, but sticking to Standard DF2 per request requires careful math:
    // (x - a*w) / a0.
    template <typename T>
    class BiQuadDF2NonNorm : public ISystemBlock<BiQuadDF2NonNorm<T>, T>
    {
    private:
        BiQuadCoeffs<T>     m_coeffs;
        Eigen::Vector<T, 2> m_state_vector{};  // State variables w[n-1], w[n-2]

    public:
        template <typename TCoeffs>
        explicit BiQuadDF2NonNorm(const TCoeffs& coeffs, const T gain = 1.0)
            : m_coeffs(BiQuadCoeffs<T>(coeffs) * gain), m_state_vector(Eigen::Vector<T, 2>::Zero())
        {
        }

        template <typename TCoeffs>
        void set_coeffs(const TCoeffs& coeffs, T gain = static_cast<T>(1.0))
        {
            m_coeffs = BiQuadCoeffs<T>(coeffs) * gain;
            this->reset();
        }

        T process_impl(T input)
        {
            // Compute W (Intermediate State)
            // w[n] = (x[n] - a1*w[n-1] - a2*w[n-2]) / a0

            // Calculate the feedback sum: a1*w[n-1] + a2*w[n-2]
            // Note: m_coeffs.a is [a0, a1, a2], so we take the tail<2>.
            const T feedback_sum = precise_dot(m_coeffs.a.template tail<2>(), m_state_vector);
            const T w_acc        = input - feedback_sum;

            // Division by a0 happens here (Non-Normalizing behavior)
            const T new_state = w_acc / m_coeffs.a[0];

            // Compute Output Y
            // y[n] = b0*w[n] + b1*w[n-1] + b2*w[n-2]

            // Calculate history sum: b1*w[n-1] + b2*w[n-2]
            const T feedforward_hist = precise_dot(m_coeffs.b.template tail<2>(), m_state_vector);

            T out;
            if constexpr (std::is_floating_point_v<T>)
            {
                // High precision combine
                out = std::fma(new_state, m_coeffs.b[0], feedforward_hist);
            }
            else
            {
                out = (new_state * m_coeffs.b[0]) + feedforward_hist;
            }

            // Shift State
            m_state_vector[1] = m_state_vector[0];  // w[n-2] = w[n-1]
            m_state_vector[0] = new_state;          // w[n-1] = w[n]

            return out;
        }

        void reset_impl()
        {
            m_state_vector.setZero();
        }
    };

    template <typename T>
    using BiQuadDF2 = std::conditional_t<std::is_floating_point_v<T>, BiQuadDF2Norm<T>, BiQuadDF2NonNorm<T>>;

}  // namespace valle::system