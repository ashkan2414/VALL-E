#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <numbers>
#include <type_traits>

namespace valle
{
    struct EngineKinematicsCalculatorReferenceConfig
    {
        using ValueT = float;

        // Rotor angle when normalized encoder count = 0.
        ValueT encoder_zero_at_rotor_angle_rad = static_cast<ValueT>(0.0);

        // Rotor angle at which the cylinder is at TDC.
        ValueT rotor_angle_at_tdc_rad = static_cast<ValueT>(0.0);

        // Normalized cylinder position convention
        ValueT cylinder_pos_norm_at_tdc = static_cast<ValueT>(0.0);
        ValueT cylinder_pos_norm_at_bdc = static_cast<ValueT>(1.0);

        [[nodiscard]] constexpr bool is_valid() const noexcept
        {
            return cylinder_pos_norm_at_bdc != cylinder_pos_norm_at_tdc;
        }
    };

    enum class EngineKinematicsCalculatorModel
    {
        kSinusoidal,   // Idealized sinusoidal piston motion (no connecting rod length, purely circular)
        kCrankSlider,  // Standard crank-slider piston configuration
    };

    template <EngineKinematicsCalculatorModel tkModel>
    struct EngineKinematicsCalculatorModelConfig;

    // =========================================================================
    // CONFIG: SINUSOIDAL
    // =========================================================================

    template <>
    struct EngineKinematicsCalculatorModelConfig<EngineKinematicsCalculatorModel::kSinusoidal>
    {
        [[nodiscard]] static constexpr bool is_valid() noexcept
        {
            return true;  // No parameters, always valid
        }
    };

    // =========================================================================
    // CONFIG: CRANK-SLIDER
    // =========================================================================

    template <>
    struct EngineKinematicsCalculatorModelConfig<EngineKinematicsCalculatorModel::kCrankSlider>
    {
        using ValueT = typename EngineKinematicsCalculatorReferenceConfig::ValueT;

        // Crank-slider geometry
        ValueT crank_radius          = static_cast<ValueT>(0.0);
        ValueT connecting_rod_length = static_cast<ValueT>(0.0);

        [[nodiscard]] constexpr bool is_valid() const noexcept
        {
            return (crank_radius > static_cast<ValueT>(0)) && (connecting_rod_length > static_cast<ValueT>(0)) &&
                   (connecting_rod_length >= crank_radius);
        }
    };

    template <EngineKinematicsCalculatorModel tkModel>
    struct EngineKinematicsCalculatorConfig
    {
        using ValueT = typename EngineKinematicsCalculatorReferenceConfig::ValueT;

        EngineKinematicsCalculatorReferenceConfig      reference_config{};
        EngineKinematicsCalculatorModelConfig<tkModel> model_config{};

        [[nodiscard]] constexpr bool is_valid() const noexcept
        {
            return reference_config.is_valid() && model_config.is_valid();
        }
    };

    template <EngineKinematicsCalculatorModel tkModel>
    class EngineKinematicsCalculator
    {
    public:
        using ConfigT                 = EngineKinematicsCalculatorConfig<tkModel>;
        using ValueT                  = typename ConfigT::ValueT;
        static constexpr auto skModel = tkModel;

        static constexpr bool skIsSinusoidal  = skModel == EngineKinematicsCalculatorModel::kSinusoidal;
        static constexpr bool skIsCrankSlider = skModel == EngineKinematicsCalculatorModel::kCrankSlider;

    private:
        static constexpr ValueT skNormalizedCountMin = static_cast<ValueT>(0.0);
        static constexpr ValueT skNormalizedCountMax = static_cast<ValueT>(1.0);
        static constexpr ValueT skFullRevolutionRad  = static_cast<ValueT>(2.0) * std::numbers::pi_v<ValueT>;

        struct PrecomputedValues
        {
            ValueT sin_mid;
            ValueT sin_amp;
            ValueT crank_pos_norm_span;

            explicit constexpr PrecomputedValues(const ConfigT& config)
            {
                const ValueT tdc    = config.reference_config.cylinder_pos_norm_at_tdc;
                const ValueT bdc    = config.reference_config.cylinder_pos_norm_at_bdc;
                sin_mid             = static_cast<ValueT>(0.5) * (tdc + bdc);
                crank_pos_norm_span = bdc - tdc;
                sin_amp             = static_cast<ValueT>(0.5) * crank_pos_norm_span;
            }
        };

        ConfigT           m_config{};
        PrecomputedValues m_precomp{m_config};

    public:
        explicit constexpr EngineKinematicsCalculator(const ConfigT& config) noexcept
            : m_config(config), m_precomp(config)
        {
        }

        constexpr void set_config(const ConfigT& config) noexcept
        {
            m_config  = config;
            m_precomp = PrecomputedValues(config);
        }

        [[nodiscard]] static constexpr ValueT wrap_norm(ValueT value) noexcept
        {
            while (value >= skNormalizedCountMax)
            {
                value -= static_cast<ValueT>(1.0);
            }

            while (value < skNormalizedCountMin)
            {
                value += static_cast<ValueT>(1.0);
            }

            return value;
        }

        [[nodiscard]] static constexpr ValueT wrap_angle_rad(ValueT angle_rad) noexcept
        {
            while (angle_rad >= skFullRevolutionRad)
            {
                angle_rad -= skFullRevolutionRad;
            }

            while (angle_rad < static_cast<ValueT>(0.0))
            {
                angle_rad += skFullRevolutionRad;
            }

            return angle_rad;
        }

        // ---------------------------------------------------------------------
        // ENCODER <-> ROTOR ANGLE
        // ---------------------------------------------------------------------

        [[nodiscard]] constexpr ValueT norm_count_to_rotor_angle_rad(const ValueT norm_count) const noexcept
        {
            const ValueT wrapped_count = wrap_norm(norm_count);
            const ValueT rotor_angle =
                m_config.reference_config.encoder_zero_at_rotor_angle_rad + (wrapped_count * skFullRevolutionRad);

            return wrap_angle_rad(rotor_angle);
        }

        [[nodiscard]] constexpr ValueT rotor_angle_rad_to_norm_count(const ValueT rotor_angle_rad) const noexcept
        {
            const ValueT wrapped_rotor_angle = wrap_angle_rad(rotor_angle_rad);
            const ValueT norm =
                (wrapped_rotor_angle - m_config.reference_config.encoder_zero_at_rotor_angle_rad) / skFullRevolutionRad;

            return wrap_norm(norm);
        }

        // ---------------------------------------------------------------------
        // ROTOR <-> CYLINDER ANGLE
        // ---------------------------------------------------------------------

        [[nodiscard]] constexpr ValueT rotor_angle_rad_to_cylinder_angle_rad(
            const ValueT rotor_angle_rad) const noexcept
        {
            return wrap_angle_rad(rotor_angle_rad - m_config.reference_config.rotor_angle_at_tdc_rad);
        }

        [[nodiscard]] constexpr ValueT cylinder_angle_rad_to_rotor_angle_rad(
            const ValueT cylinder_angle_rad) const noexcept
        {
            return wrap_angle_rad(cylinder_angle_rad + m_config.reference_config.rotor_angle_at_tdc_rad);
        }

        // ---------------------------------------------------------------------
        // CYLINDER POSITION (SINUSOIDAL APPROXIMATION)
        // ---------------------------------------------------------------------

        [[nodiscard]] constexpr ValueT cylinder_angle_rad_to_cylinder_pos_norm_approx(
            ValueT cylinder_angle_rad) const noexcept
        {
            const ValueT wrapped_angle = wrap_angle_rad(cylinder_angle_rad);
            return m_precomp.sin_mid - (m_precomp.sin_amp * std::cos(wrapped_angle));
        }

        [[nodiscard]] constexpr ValueT rotor_angle_rad_to_cylinder_pos_norm_approx(
            ValueT rotor_angle_rad) const noexcept
        {
            return cylinder_angle_rad_to_cylinder_pos_norm_approx(
                rotor_angle_rad_to_cylinder_angle_rad(rotor_angle_rad));
        }

        [[nodiscard]] constexpr ValueT norm_count_to_cylinder_pos_norm_approx(ValueT norm_count) const noexcept
        {
            return rotor_angle_rad_to_cylinder_pos_norm_approx(norm_count_to_rotor_angle_rad(norm_count));
        }

        // ---------------------------------------------------------------------
        // PHYSICAL DISPLACEMENT (CRANK-SLIDER MODEL)
        // ---------------------------------------------------------------------

        [[nodiscard]] constexpr ValueT cylinder_angle_rad_to_cylinder_displacement(
            const ValueT cylinder_angle_rad) const noexcept
            requires(skIsCrankSlider)
        {
            const ValueT theta = wrap_angle_rad(cylinder_angle_rad);
            const ValueT r     = m_config.model_config.crank_radius;
            const ValueT l     = m_config.model_config.connecting_rod_length;

            const ValueT sin_theta  = std::sin(theta);
            const ValueT cos_theta  = std::cos(theta);
            const ValueT under_root = (l * l) - (r * r * sin_theta * sin_theta);
            const ValueT x          = (r * cos_theta) + std::sqrt(under_root);
            const ValueT x_max      = r + l;

            return x_max - x;
        }

        [[nodiscard]] constexpr ValueT rotor_angle_rad_to_cylinder_displacement(
            const ValueT rotor_angle_rad) const noexcept
            requires(skIsCrankSlider)
        {
            return cylinder_angle_rad_to_cylinder_displacement(rotor_angle_rad_to_cylinder_angle_rad(rotor_angle_rad));
        }

        [[nodiscard]] constexpr ValueT norm_count_to_cylinder_displacement(const ValueT norm_count) const noexcept
            requires(skIsCrankSlider)
        {
            return rotor_angle_rad_to_cylinder_displacement(norm_count_to_rotor_angle_rad(norm_count));
        }

        // ---------------------------------------------------------------------
        // CYLINDER POSITION (CRANK-SLIDER MODEL)
        // ---------------------------------------------------------------------
        [[nodiscard]] constexpr ValueT cylinder_displacement_to_cylinder_pos_norm(const ValueT disp) const noexcept
            requires(skIsCrankSlider)
        {
            const ValueT stroke = static_cast<ValueT>(2) * m_config.model_config.crank_radius;
            const ValueT pos_01 = disp / stroke;

            const ValueT tdc = m_config.reference_config.cylinder_pos_norm_at_tdc;
            return tdc + (m_precomp.crank_pos_norm_span * pos_01);
        }

        [[nodiscard]] constexpr ValueT cylinder_angle_rad_to_cylinder_pos_norm(ValueT cylinder_angle_rad) const noexcept
            requires(skIsCrankSlider)
        {
            return cylinder_displacement_to_cylinder_pos_norm(
                cylinder_angle_rad_to_cylinder_displacement(cylinder_angle_rad));
        }

        [[nodiscard]] constexpr ValueT rotor_angle_rad_to_cylinder_pos_norm(ValueT rotor_angle_rad) const noexcept
            requires(skIsCrankSlider)
        {
            return cylinder_angle_rad_to_cylinder_pos_norm(rotor_angle_rad_to_cylinder_angle_rad(rotor_angle_rad));
        }

        [[nodiscard]] constexpr ValueT norm_count_to_cylinder_pos_norm(ValueT norm_count) const noexcept
            requires(skIsCrankSlider)
        {
            return rotor_angle_rad_to_cylinder_pos_norm(norm_count_to_rotor_angle_rad(norm_count));
        }

        // ---------------------------------------------------------------------
        // INVERSE CYLINDER POSITION (SINUSOIDAL APPROXIMATION)
        // ---------------------------------------------------------------------

        [[nodiscard]] constexpr ValueT cylinder_pos_norm_to_cylinder_angle_rad_approx(
            const ValueT cylinder_pos_norm) const noexcept
        {
            if (m_precomp.sin_amp == static_cast<ValueT>(0))
            {
                return static_cast<ValueT>(0);
            }

            const ValueT cos_theta = std::clamp((m_precomp.sin_mid - cylinder_pos_norm) / m_precomp.sin_amp,
                                                static_cast<ValueT>(-1.0),
                                                static_cast<ValueT>(1.0));

            return std::acos(cos_theta);
        }

        [[nodiscard]] constexpr ValueT cylinder_pos_norm_to_rotor_angle_rad_approx(
            const ValueT cylinder_pos_norm) const noexcept
        {
            return cylinder_angle_rad_to_rotor_angle_rad(
                cylinder_pos_norm_to_cylinder_angle_rad_approx(cylinder_pos_norm));
        }

        [[nodiscard]] constexpr ValueT cylinder_pos_norm_to_norm_count_approx(
            const ValueT cylinder_pos_norm) const noexcept
        {
            return rotor_angle_rad_to_norm_count(cylinder_pos_norm_to_rotor_angle_rad_approx(cylinder_pos_norm));
        }
    };
}  // namespace valle