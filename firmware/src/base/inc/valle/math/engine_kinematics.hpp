#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <numbers>
#include <type_traits>

namespace valle
{
    enum class EngineStroke : uint8_t
    {
        kIntake      = 0,
        kCompression = 1,
        kPower       = 2,
        kExhaust     = 3,
    };

    struct EngineKinematicsCalculatorReferenceConfig
    {
        using ValueT = float;

        // Rotor angle when normalized encoder count = 0.
        ValueT encoder_zero_at_crank_angle_rad = static_cast<ValueT>(0.0);

        // Rotor angle at which the cylinder is at TDC.
        ValueT crank_angle_at_tdc_rad = static_cast<ValueT>(0.0);

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
        static constexpr ValueT skStokeRad          = std::numbers::pi_v<ValueT>;
        static constexpr ValueT skFullRevolutionRad = static_cast<ValueT>(2.0) * skStokeRad;
        static constexpr ValueT skFullCycleRad      = static_cast<ValueT>(2.0) * skFullRevolutionRad;

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
        constexpr EngineKinematicsCalculator() noexcept = default;

        explicit constexpr EngineKinematicsCalculator(const ConfigT& config) noexcept
            : m_config(config), m_precomp(config)
        {
        }

        constexpr void init(const ConfigT& config) noexcept
        {
            m_config  = config;
            m_precomp = PrecomputedValues(config);
        }

        [[nodiscard]] static constexpr ValueT wrap_norm(ValueT value) noexcept
        {
            while (value >= static_cast<ValueT>(1.0))
            {
                value -= static_cast<ValueT>(1.0);
            }

            while (value < static_cast<ValueT>(0.0))
            {
                value += static_cast<ValueT>(1.0);
            }

            return value;
        }

        [[nodiscard]] static constexpr ValueT wrap_angle_rad_cycle(ValueT angle_rad) noexcept
        {
            while (angle_rad >= skFullCycleRad)
            {
                angle_rad -= skFullCycleRad;
            }

            while (angle_rad < static_cast<ValueT>(0.0))
            {
                angle_rad += skFullCycleRad;
            }

            return angle_rad;
        }

        [[nodiscard]] static constexpr ValueT wrap_angle_rad_rev(ValueT angle_rad) noexcept
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

        [[nodiscard]] constexpr ValueT wrap_angle_rad_stroke(ValueT angle_rad) const noexcept
        {
            while (angle_rad >= skStokeRad)
            {
                angle_rad -= skStokeRad;
            }

            while (angle_rad < static_cast<ValueT>(0.0))
            {
                angle_rad += skStokeRad;
            }

            return angle_rad;
        }

        // ---------------------------------------------------------------------
        // ENCODER <-> STROKE
        // ---------------------------------------------------------------------
        [[nodiscard]] constexpr EngineStroke encoder_pos_norm_cycle_to_stroke(
            const ValueT encoder_pos_norm) const noexcept
        {
            static_assert(static_cast<uint8_t>(EngineStroke::kIntake) == 0 &&
                              static_cast<uint8_t>(EngineStroke::kCompression) == 1 &&
                              static_cast<uint8_t>(EngineStroke::kPower) == 2 &&
                              static_cast<uint8_t>(EngineStroke::kExhaust) == 3,
                          "EngineStroke enum values must be 0, 1, 2, 3 for this calculation to work");

            const ValueT  wrapped_count = wrap_norm(encoder_pos_norm);
            const uint8_t idx           = static_cast<uint8_t>(wrapped_count * 4.0F);
            return static_cast<EngineStroke>(idx);
        }

        [[nodiscard]] constexpr EngineStroke crank_angle_rad_cycle_to_stroke(
            const ValueT crank_angle_rad) const noexcept
        {
            return encoder_pos_norm_cycle_to_stroke(crank_angle_rad / skFullCycleRad);
        }

        // ---------------------------------------------------------------------
        // ENCODER <-> ROTOR ANGLE
        // ---------------------------------------------------------------------

        [[nodiscard]] constexpr ValueT encoder_pos_norm_cycle_to_crank_angle_rad_cycle(
            const ValueT encoder_pos_norm) const noexcept
        {
            // Since 1 cycle = 2 revolutions for a 4-stroke engine, we need to multiply the normalized count by 2
            const ValueT wrapped_count = wrap_norm(encoder_pos_norm);
            const ValueT crank_angle =
                m_config.reference_config.encoder_zero_at_crank_angle_rad + (wrapped_count * skFullCycleRad);

            return wrap_angle_rad_cycle(crank_angle);
        }

        [[nodiscard]] constexpr ValueT encoder_pos_norm_rev_to_crank_angle_rad_rev(
            const ValueT encoder_pos_norm) const noexcept
        {
            const ValueT wrapped_count = wrap_norm(encoder_pos_norm);
            const ValueT crank_angle =
                m_config.reference_config.encoder_zero_at_crank_angle_rad + (wrapped_count * skFullRevolutionRad);

            return wrap_angle_rad_rev(crank_angle);
        }

        [[nodiscard]] constexpr ValueT encoder_pos_norm_stroke_to_crank_angle_rad_stroke(
            const ValueT encoder_pos_norm) const noexcept
        {
            const ValueT wrapped_count = wrap_norm(encoder_pos_norm);
            const ValueT crank_angle =
                m_config.reference_config.encoder_zero_at_crank_angle_rad + (wrapped_count * skStokeRad);

            return wrap_angle_rad_stroke(crank_angle);
        }

        [[nodiscard]] constexpr ValueT crank_angle_rad_cycle_to_encoder_pos_norm_cycle(
            const ValueT crank_angle_rad) const noexcept
        {
            const ValueT wrapped_crank_angle = wrap_angle_rad_cycle(crank_angle_rad);
            const ValueT norm =
                (wrapped_crank_angle - m_config.reference_config.encoder_zero_at_crank_angle_rad) / skFullCycleRad;

            return wrap_norm(norm);
        }

        [[nodiscard]] constexpr ValueT crank_angle_rad_rev_to_encoder_pos_norm_rev(
            const ValueT crank_angle_rad) const noexcept
        {
            const ValueT wrapped_crank_angle = wrap_angle_rad_rev(crank_angle_rad);
            const ValueT norm =
                (wrapped_crank_angle - m_config.reference_config.encoder_zero_at_crank_angle_rad) / skFullRevolutionRad;

            return wrap_norm(norm);
        }

        [[nodiscard]] constexpr ValueT crank_angle_rad_stroke_to_encoder_pos_norm_stroke(
            const ValueT crank_angle_rad) const noexcept
        {
            const ValueT wrapped_crank_angle = wrap_angle_rad_stroke(crank_angle_rad);
            const ValueT norm =
                (wrapped_crank_angle - m_config.reference_config.encoder_zero_at_crank_angle_rad) / skStokeRad;

            return wrap_norm(norm);
        }

        // ---------------------------------------------------------------------
        // ROTOR <-> CYLINDER ANGLE
        // ---------------------------------------------------------------------

        [[nodiscard]] constexpr ValueT crank_angle_rad_to_cylinder_angle_rad(
            const ValueT crank_angle_rad) const noexcept
        {
            return wrap_angle_rad_rev(crank_angle_rad - m_config.reference_config.crank_angle_at_tdc_rad);
        }

        [[nodiscard]] constexpr ValueT cylinder_angle_rad_to_crank_angle_rad(
            const ValueT cylinder_angle_rad) const noexcept
        {
            return wrap_angle_rad_rev(cylinder_angle_rad + m_config.reference_config.crank_angle_at_tdc_rad);
        }

        // ---------------------------------------------------------------------
        // CYLINDER POSITION (SINUSOIDAL APPROXIMATION)
        // ---------------------------------------------------------------------

        [[nodiscard]] constexpr ValueT cylinder_angle_rad_to_cylinder_pos_norm_approx(
            ValueT cylinder_angle_rad) const noexcept
        {
            const ValueT wrapped_angle = wrap_angle_rad_rev(cylinder_angle_rad);
            return m_precomp.sin_mid - (m_precomp.sin_amp * std::cos(wrapped_angle));
        }

        [[nodiscard]] constexpr ValueT crank_angle_rad_to_cylinder_pos_norm_approx(
            ValueT crank_angle_rad) const noexcept
        {
            return cylinder_angle_rad_to_cylinder_pos_norm_approx(
                crank_angle_rad_to_cylinder_angle_rad(crank_angle_rad));
        }

        [[nodiscard]] constexpr ValueT encoder_pos_norm_to_cylinder_pos_norm_approx(
            ValueT encoder_pos_norm) const noexcept
        {
            return crank_angle_rad_to_cylinder_pos_norm_approx(encoder_pos_norm_to_crank_angle_rad(encoder_pos_norm));
        }

        // ---------------------------------------------------------------------
        // PHYSICAL DISPLACEMENT (CRANK-SLIDER MODEL)
        // ---------------------------------------------------------------------

        [[nodiscard]] constexpr ValueT cylinder_angle_rad_to_cylinder_displacement(
            const ValueT cylinder_angle_rad) const noexcept
            requires(skIsCrankSlider)
        {
            const ValueT theta = wrap_angle_rad_rev(cylinder_angle_rad);
            const ValueT r     = m_config.model_config.crank_radius;
            const ValueT l     = m_config.model_config.connecting_rod_length;

            const ValueT sin_theta  = std::sin(theta);
            const ValueT cos_theta  = std::cos(theta);
            const ValueT under_root = (l * l) - (r * r * sin_theta * sin_theta);
            const ValueT x          = (r * cos_theta) + std::sqrt(under_root);
            const ValueT x_max      = r + l;

            return x_max - x;
        }

        [[nodiscard]] constexpr ValueT crank_angle_rad_to_cylinder_displacement(
            const ValueT crank_angle_rad) const noexcept
            requires(skIsCrankSlider)
        {
            return cylinder_angle_rad_to_cylinder_displacement(crank_angle_rad_to_cylinder_angle_rad(crank_angle_rad));
        }

        [[nodiscard]] constexpr ValueT encoder_pos_norm_to_cylinder_displacement(
            const ValueT encoder_pos_norm) const noexcept
            requires(skIsCrankSlider)
        {
            return crank_angle_rad_to_cylinder_displacement(encoder_pos_norm_to_crank_angle_rad(encoder_pos_norm));
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

        [[nodiscard]] constexpr ValueT crank_angle_rad_to_cylinder_pos_norm(ValueT crank_angle_rad) const noexcept
            requires(skIsCrankSlider)
        {
            return cylinder_angle_rad_to_cylinder_pos_norm(crank_angle_rad_to_cylinder_angle_rad(crank_angle_rad));
        }

        [[nodiscard]] constexpr ValueT encoder_pos_norm_to_cylinder_pos_norm(ValueT encoder_pos_norm) const noexcept
            requires(skIsCrankSlider)
        {
            return crank_angle_rad_to_cylinder_pos_norm(encoder_pos_norm_to_crank_angle_rad(encoder_pos_norm));
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

        [[nodiscard]] constexpr ValueT cylinder_pos_norm_to_crank_angle_rad_approx(
            const ValueT cylinder_pos_norm) const noexcept
        {
            return cylinder_angle_rad_to_crank_angle_rad(
                cylinder_pos_norm_to_cylinder_angle_rad_approx(cylinder_pos_norm));
        }

        [[nodiscard]] constexpr ValueT cylinder_pos_norm_to_encoder_pos_norm_approx(
            const ValueT cylinder_pos_norm) const noexcept
        {
            return crank_angle_rad_to_encoder_pos_norm(cylinder_pos_norm_to_crank_angle_rad_approx(cylinder_pos_norm));
        }
    };
}  // namespace valle