#pragma once

#include <cmath>
#include <numbers>

#include "valle/app/platform/modules/amt10x.hpp"
#include "valle/math/engine_kinematics.hpp"

namespace valle::platform::app
{

    static constexpr auto skAMT10xCrankEncoderEngineKinematicsModel = EngineKinematicsCalculatorModel::kCrankSlider;
    using AMT10xCrankEncoderEngineKinematicsCalculatorT =
        EngineKinematicsCalculator<skAMT10xCrankEncoderEngineKinematicsModel>;
    using AMT10xCrankEncoderEngineKinematicsCalculatorConfigT =
        typename AMT10xCrankEncoderEngineKinematicsCalculatorT::ConfigT;

    struct AMT10xCrankEncoderModuleConfig
    {
        AMT10xCrankEncoderEngineKinematicsCalculatorConfigT engine_kinematics;
        float                                               home_rad = 0.0F;
        bool                                                reverse_direction{false};
    };

    template <typename TEncoder>
    class AMT10xCrankEncoderModuleX
    {
    public:
        using EncoderT       = TEncoder;
        using ConfigT        = AMT10xCrankEncoderModuleConfig;
        using EncoderConfigT = typename EncoderT::ConfigT;

        using InjectDevices = typename GetInjectDevices<EncoderT>::type;
        using DependDevices = typename GetAdditionalDependDevices<EncoderT>::type;

        static constexpr valle::app::AMT10xPPR skPPR = EncoderT::skPPR;

    private:
        static constexpr float skRadPerStroke = std::numbers::pi_v<float>;
        static constexpr float skRadPerRev    = 2.0F * skRadPerStroke;
        static constexpr float skRadPerCycle  = 2.0F * skRadPerRev;

        static constexpr float skDegPerStroke = 180.0F;
        static constexpr float skDegPerRev    = 2.0F * skDegPerStroke;
        static constexpr float skDegPerCycle  = 2.0F * skDegPerRev;

        // 4x decoding, 2 revolutions per cycle for 4-stroke engine
        static constexpr uint32_t skCounterAutoReload = (static_cast<uint32_t>(skPPR) * 4U * 2U) - 1;

        EncoderT                                      m_encoder{};
        AMT10xCrankEncoderEngineKinematicsCalculatorT m_engine_kinematics_calculator{};

    public:
        template <typename... TArgs>
        explicit AMT10xCrankEncoderModuleX(TArgs&&... args) : m_encoder(std::forward<TArgs>(args)...)
        {
        }

        [[nodiscard]] bool init(const ConfigT& config)
        {
            m_engine_kinematics_calculator.init(config.engine_kinematics);

            return m_encoder.init(EncoderConfigT{
                .encoder_config =
                    platform::TIMQuadEncoderConfig{
                        .gpio_config =
                            platform::GPIOAlternateFunctionConfig{
                                .mode  = platform::GPIOAlternateFunctionMode::kPushPull,
                                .speed = platform::GPIOSpeedMode::kMedium,
                                .pull  = platform::GPIOPullMode::kNoPull,
                            },
                        .encoder_config =
                            platform::TIMControllerEncoderConfig{
                                .mode = platform::TIMControllerEncoderMode::kX4TimerInput12,
                                .ch_config =
                                    platform::TIMChannelInputCaptureConfig{
                                        .active_input = platform::TIMChannelInputCaptureActiveInput::kDirectTimerInput,
                                        .prescaler    = platform::TIMChannelInputCapturePrescaler::kDiv1,
                                        .filter       = platform::TIMChannelInputCaptureFilter::kFreqDiv2N8Samples,
                                        .polarity     = platform::TIMChannelInputCapturePolarity::kRising,
                                    },
                                .ch2_polarity = platform::TIMChannelInputCapturePolarity::kRising,
                                .auto_reload  = static_cast<uint32_t>(skCounterAutoReload),
                            }},
                .home_rad          = config.home_rad,
                .reverse_direction = config.reverse_direction});
        }

        void enable()
        {
            m_encoder.enable();
        }

        void disable()
        {
            m_encoder.disable();
        }

        void reset()
        {
            m_encoder.reset();
        }

        void set_as_home()
        {
            m_encoder.set_as_home();
        }

        [[nodiscard]] bool is_index_active() const
        {
            return m_encoder.is_index_active();
        }

        [[nodiscard]] bool run_homing_sequence(valle::system::TimeoutMillis timeout)
        {
            return m_encoder.run_homing_sequence(timeout);
        }

        [[nodiscard]] EncoderT& get_encoder()
        {
            return m_encoder;
        }

        // -------------------------------------------------------------------------
        // ENCODER POSITION NORMALIZED
        // -------------------------------------------------------------------------

        [[nodiscard]] float get_encoder_pos_norm_cycle() const
        {
            // Encoder reports abs position in normalized revolutions so we need to wrap it to get the cycle position.
            return std::fmod(m_encoder.get_pos_norm_abs() * 0.5F, 1.0F);
        }

        [[nodiscard]] float get_encoder_pos_norm_rev() const
        {
            return m_encoder.get_pos_norm_rel();
        }

        [[nodiscard]] float get_encoder_pos_norm_stroke() const
        {
            // Similar to count_stroke, we need to wrap the normalized revolution value to get the stroke position.
            return std::fmod(get_encoder_pos_norm_rev() * 2.0F, 1.0F);
        }

        // -------------------------------------------------------------------------
        // ENCODER POSITION RADIANS
        // -------------------------------------------------------------------------

        /**
         * @brief Returns absolute shaft pos in radians (multi-turn).
         */
        [[nodiscard]] float get_encoder_pos_rad_cycle() const
        {
            return std::fmod(m_encoder.get_pos_rad_abs(), skRadPerCycle);
        }

        /**
         * @brief Returns revolution pos in radians [0, 2pi).
         */
        [[nodiscard]] float get_encoder_pos_rad_rev() const
        {
            return m_encoder.get_pos_rad_rel();
        }

        [[nodiscard]] float get_encoder_pos_rad_stroke() const
        {
            return std::fmod(get_encoder_pos_rad_rev(), skRadPerStroke);
        }

        // ------------------------------------------------------------------------
        // ENCODER POSITION DEGREES
        // ------------------------------------------------------------------------

        /**
         * @brief Returns absolute shaft pos in degrees (multi-turn).
         */
        [[nodiscard]] float get_encoder_pos_deg_cycle() const
        {
            return std::fmod(m_encoder.get_pos_deg_abs(), skDegPerCycle);
        }

        /**
         * @brief Returns revolution pos in degrees [0, 360).
         */
        [[nodiscard]] float get_encoder_pos_deg_rev() const
        {
            return m_encoder.get_pos_deg_rel();
        }

        /**
         * @brief Returns stroke pos in degrees [0, 180).
         *
         * @return float
         */
        [[nodiscard]] float get_encoder_pos_deg_stroke() const
        {
            return std::fmod(get_encoder_pos_deg_rev(), skDegPerStroke);
        }

        // ------------------------------------------------------------------------
        // Rotor angle
        // ------------------------------------------------------------------------
        [[nodiscard]] float get_crank_angle_rad_cycle() const
        {
            return m_engine_kinematics_calculator.encoder_pos_norm_cycle_to_crank_angle_rad_cycle(
                get_encoder_pos_norm_cycle());
        }

        [[nodiscard]] float get_crank_angle_rad_rev() const
        {
            return m_engine_kinematics_calculator.encoder_pos_norm_rev_to_crank_angle_rad_rev(
                get_encoder_pos_norm_rev());
        }

        [[nodiscard]] float get_crank_angle_rad_stroke() const
        {
            return m_engine_kinematics_calculator.encoder_pos_norm_stroke_to_crank_angle_rad_stroke(
                get_encoder_pos_norm_stroke());
        }

        // ------------------------------------------------------------------------
        // Cylinder Angle (WITHIN REVOLUTION)
        // ------------------------------------------------------------------------
        [[nodiscard]] float get_cylinder_angle_rad() const
        {
            return m_engine_kinematics_calculator.crank_angle_rad_to_cylinder_angle_rad(get_crank_angle_rad_rev());
        }

        // ------------------------------------------------------------------------
        // Cylinder Position (WITHIN REVOLUTION)
        // ------------------------------------------------------------------------
        [[nodiscard]] float get_cylinder_pos_norm_approx() const
        {
            return m_engine_kinematics_calculator.cylinder_angle_rad_to_cylinder_pos_norm_approx(
                get_cylinder_angle_rad());
        }

        [[nodiscard]] float get_cylinder_pos_norm() const
        {
            return m_engine_kinematics_calculator.cylinder_angle_rad_to_cylinder_pos_norm(get_cylinder_angle_rad());
        }

        // ------------------------------------------------------------------------
        // Cylinder Displacement
        // ------------------------------------------------------------------------
        [[nodiscard]] float get_cylinder_displacement() const
        {
            return m_engine_kinematics_calculator.cylinder_angle_rad_to_cylinder_displacement(get_cylinder_angle_rad());
        }

        // ------------------------------------------------------------------------
        // Stroke
        // --------------------------------------------------------------------------
        [[nodiscard]] EngineStroke get_stroke() const
        {
            return m_engine_kinematics_calculator.encoder_pos_norm_cycle_to_stroke(get_encoder_pos_norm_cycle());
        }

        [[nodiscard]] std::pair<EngineStroke, float> get_stroke_pos_norm() const
        {
            return {get_stroke(), get_cylinder_pos_norm()};
        }

        [[nodiscard]] std::pair<EngineStroke, float> get_stroke_pos_norm_approx() const
        {
            return {get_stroke(), get_cylinder_pos_norm_approx()};
        }
    };
}  // namespace valle::platform::app