#pragma once

#include <optional>

#include "Valle/Board/Device/Devices/hrtim.hpp"

namespace valle
{
    struct HRTIMHalfBridgeOutputConfig
    {
        HRTIMTimerOutputPolarity   polarity    = HRTIMTimerOutputPolarity::kPositive;
        HRTIMTimerOutputIdleMode   idle_mode   = HRTIMTimerOutputIdleMode::kBurst;
        HRTIMTimerOutputIdleLevel  idle_level  = HRTIMTimerOutputIdleLevel::kInactive;
        HRTIMTimerOutputFaultState fault_state = HRTIMTimerOutputFaultState::kInactive;
        HRTIMTimerOutputGPIOConfig gpio_config{};
    };

    struct HRTIMHalfBridgeDriverConfig
    {
        // --- Basic Timing ---
        uint64_t freq_hz    = 50000;  // PWM Frequency in Hz
        uint8_t  repetition = 0;      // Repetition value for interrupt generation

        HRTIMTimerInterruptConfig interrupt_config{};

        // Outputs
        HRTIMHalfBridgeOutputConfig output_config{};

        // Deadtime (Shoot-through protection)
        std::optional<HRTIMDeadTimeConfig> deadtime_config = HRTIMDeadTimeConfig{.rise_ns = 200.0F, .fall_ns = 200.0F};

        // --- Safety & Hardware Limits ---
        float min_duty = 0.0F;

        // CRITICAL for bootstrapped gate drivers.
        // If you hit 100% on a bootstrap driver, the high-side caps drain
        // and the MOSFET turns off (or enters linear region and explodes).
        // Recommended: 0.95 or 0.98.
        float max_duty = 0.95F;  // NOLINT(readability-magic-numbers)

        // --- Polarity / Topology ---
        // If true, PWM is centered (Up-Down count). Better for EMI and ADC sampling.
        // If false, PWM is edge-aligned (Sawtooth). Simpler math.
        bool center_aligned = true;

        [[nodiscard]] constexpr uint64_t get_int_freq_hz() const
        {
            return freq_hz / (repetition + 1);
        }
    };

    template <typename THRTIMTimerDevice>
    class HRTIMHalfBridgeDriver
    {
    public:
        using InjectDevices             = TypeList<THRTIMTimerDevice>;
        using HRTIMTimerT               = THRTIMTimerDevice;
        using ControllerTraitsT         = HRTIMTimerT::ControllerTraitsT;
        using TimerTraitsT              = HRTIMTimerT::TimerTraitsT;
        static constexpr size_t skIndex = HRTIMTimerT::skIndex;

        static constexpr auto skControllerID = HRTIMTimerT::skControllerID;
        static constexpr auto skTimerID      = HRTIMTimerT::skTimerID;

        static_assert(HRTIMTimerT::skHasOutput1Pin && HRTIMTimerT::skHasOutput2Pin,
                      "HRTIMHalfBridgeDriver requires a timer both GPIO outputs for complementary control.");

    private:
        [[no_unique_address]] DeviceRef<HRTIMTimerT> m_timer;
        float                                        m_min_duty = 0.0F;
        float                                        m_max_duty = 1.0F;

    public:
        HRTIMHalfBridgeDriver() = delete;
        explicit HRTIMHalfBridgeDriver(DeviceRef<HRTIMTimerT>&& device) : m_timer(std::move(device))
        {
        }

        // ------------------------------------------------------------------------
        // Initialization
        // ------------------------------------------------------------------------

        /**
         * @brief Initializes the HRTIM half-bridge with the given configuration.
         *
         * @param config  Configuration parameters.
         */
        [[nodiscard]] bool init(const HRTIMHalfBridgeDriverConfig& config)
        {
            if (!m_timer->init())
            {
                return false;
            }

            if (config.min_duty > config.max_duty)
            {
                VALLE_LOG_ERROR("HRTIM{} Timer {} Half-bridge PWM invalid duty cycle limits: min_duty {} > max_duty {}",
                                static_cast<int>(skControllerID),
                                enum_name(skTimerID),
                                config.min_duty,
                                config.max_duty);
                return false;
            }

            if (config.max_duty > 1.0F)
            {
                VALLE_LOG_ERROR("HRTIM{} Timer {} Half-bridge PWM max_duty {} is above 100%!",
                                static_cast<int>(skControllerID),
                                enum_name(skTimerID),
                                config.max_duty);
                return false;
            }

            if (config.min_duty < 0.0F)
            {
                VALLE_LOG_ERROR("HRTIM{} Timer {} Half-bridge PWM min_duty {} is below 0%!",
                                static_cast<int>(skControllerID),
                                enum_name(skTimerID),
                                config.min_duty);
                return false;
            }

            m_min_duty = config.min_duty;
            m_max_duty = config.max_duty;

            // Calculate Period and Prescaler
            const uint32_t f_hrtim_hz = m_timer->get_hrtim_freq_hz();

            const uint64_t min_freq_hz = m_timer->calculate_min_freq_hz(f_hrtim_hz);
            if (config.freq_hz < min_freq_hz)
            {
                VALLE_LOG_ERROR(
                    "HRTIM{} Timer {} Half-bridge PWM target frequency {} Hz is too low. Must be above {} Hz.",
                    static_cast<int>(skControllerID),
                    enum_name(skTimerID),
                    config.freq_hz,
                    min_freq_hz);

                return false;
            }

            const uint64_t max_freq_hz = m_timer->calculate_max_freq_hz(f_hrtim_hz);
            if (config.freq_hz > max_freq_hz)
            {
                VALLE_LOG_ERROR(
                    "HRTIM{} Timer {} Half-bridge PWM target frequency {} Hz is too high. Must be below {} Hz.",
                    static_cast<int>(skControllerID),
                    enum_name(skTimerID),
                    config.freq_hz,
                    max_freq_hz);

                return false;
            }

            const HRTIMTimerPrescaler prescaler =
                m_timer->calculate_timer_prescaler_for_freq(f_hrtim_hz, config.freq_hz);

            const uint64_t h_hrck_hz    = m_timer->calculate_hrck_freq_hz(f_hrtim_hz, prescaler);
            uint16_t       period_ticks = static_cast<uint16_t>(h_hrck_hz / config.freq_hz);

            // In Center-Aligned mode (up-down counting), one full cycle consists of counting up
            // to the period and back down to zero. Therefore, the period register must be set
            // to half the number of ticks required for the full cycle frequency.
            if (config.center_aligned)
            {
                period_ticks /= 2;
            }

            // Configure Compare Unit 1 (Duty Cycle)
            // Initialize to 50%
            LL_HRTIM_TIM_SetCompare1(ControllerTraitsT::skInstance, TimerTraitsT::skLLID, period_ticks / 2);
            HRTIMTimerCountingMode      counting_mode        = HRTIMTimerCountingMode::kUp;
            HRTIMTimerOutputSetSource   output1_set_source   = HRTIMTimerOutputSetSource::kNone;
            HRTIMTimerOutputResetSource output1_reset_source = HRTIMTimerOutputResetSource::kNone;
            HRTIMTimerOutputSetSource   output2_set_source   = HRTIMTimerOutputSetSource::kNone;
            HRTIMTimerOutputResetSource output2_reset_source = HRTIMTimerOutputResetSource::kNone;

            if (config.center_aligned)
            {
                counting_mode = HRTIMTimerCountingMode::kUpDown;

                // Center-aligned PWM: Use CMP1 as both Set and Reset source.
                // In Up-Down counting mode (RM0440 §28.3.6), the hardware automatically
                // swaps Set/Reset crossbar events when the counter is decrementing:
                //   Up-count CMP1 match:   RESET wins (Set becomes Reset) → output LOW
                //   Down-count CMP1 match: SET wins (Reset becomes Set)   → output HIGH
                // This produces a symmetric center-aligned pulse with duty = CMP1/PER,
                // giving a linear 0–100% duty range with compare = duty * period.
                output1_set_source   = HRTIMTimerOutputSetSource::kTimerCompare1;
                output1_reset_source = HRTIMTimerOutputResetSource::kTimerCompare1;

                // Complementary output automatically handled by hardware with deadtime insertion
                if (!config.deadtime_config.has_value())
                {
                    // If deadtime is disabled, we can still use the hardware swapping feature
                    // to generate complementary waveforms.
                    // TODO: verify with oscilloscope that this works as expected without deadtime.
                    output2_set_source   = HRTIMTimerOutputSetSource::kTimerCompare1;
                    output2_reset_source = HRTIMTimerOutputResetSource::kTimerCompare1;
                }
            }
            else
            {
                counting_mode = HRTIMTimerCountingMode::kUp;

                // Logic: HIGH at Period Start, LOW at Compare Match
                output1_set_source   = HRTIMTimerOutputSetSource::kTimerPeriod;
                output1_reset_source = HRTIMTimerOutputResetSource::kTimerCompare1;

                // Complementary output automatically handled by hardware with deadtime insertion
                if (!config.deadtime_config.has_value())
                {
                    // Logic: HIGH at Compare Match, LOW at Period Start
                    output2_set_source   = HRTIMTimerOutputSetSource::kTimerCompare1;
                    output2_reset_source = HRTIMTimerOutputResetSource::kTimerPeriod;
                }
            }

            // Configure Timer Time Base
            if (!m_timer->init_counter(HRTIMTimerCounterConfig{
                    .prescaler     = prescaler,
                    .counter_mode  = HRTIMTimerCounterMode::kContinuous,
                    .counting_mode = counting_mode,
                    .period        = period_ticks,
                    .repetition    = config.repetition,
                }))
            {
                return false;
            }

            if (!m_timer->init_output1(
                    HRTIMTimerVirtualOutputConfig{
                        .set_source   = output1_set_source,
                        .reset_source = output1_reset_source,
                        .polarity     = config.output_config.polarity,
                        .idle_mode    = config.output_config.idle_mode,
                        .idle_level   = config.output_config.idle_level,
                        .fault_state  = config.output_config.fault_state,
                    },
                    config.output_config.gpio_config))
            {
                return false;
            }

            if (!m_timer->init_output2(
                    HRTIMTimerVirtualOutputConfig{
                        .set_source   = output2_set_source,
                        .reset_source = output2_reset_source,
                        .polarity     = config.output_config.polarity,
                        .idle_mode    = config.output_config.idle_mode,
                        .idle_level   = config.output_config.idle_level,
                        .fault_state  = config.output_config.fault_state,
                    },
                    config.output_config.gpio_config))
            {
                return false;
            }

            // Deadtime (Optional but recommended for H-Bridge)
            if (config.deadtime_config.has_value())
            {
                if (!m_timer->init_deadtime(config.deadtime_config.value()))
                {
                    return false;
                }
            }
            else
            {
                m_timer->disable_deadtime();
            }

            // Enable Interrupts
            // Interrupts must be enabled AFTER checking for deadtime since deadtime config might lock registers
            m_timer->enable_interrupts(config.interrupt_config);

            return true;
        }

        /**
         * @brief Enables the half-bridge output.
         *
         */
        inline void enable_output()
        {
            m_timer->enable_outputs();
            m_timer->enable_counter();
        }

        /**
         * @brief Disables the half-bridge output.
         *
         */
        inline void disable_output()
        {
            m_timer->disable_outputs();
            m_timer->disable_counter();
        }

        void set_repetition_counter(uint8_t repetition)
        {
            LL_HRTIM_TIM_SetRepetition(ControllerTraitsT::skInstance, TimerTraitsT::skLLID, repetition);
        }

        // ------------------------------------------------------------------------
        // Runtime Control (Hot Path)
        // ------------------------------------------------------------------------
        // duty: 0.0 to 1.0 (0.5 = Zero Force)
        inline void set_duty_cycle(const float duty)
        {
            const float clamped_duty = std::clamp(duty, m_min_duty, m_max_duty);

            // Get current period ticks (cached in hardware register)
            // TODO: should we cache in memory for faster access?
            const uint32_t period = m_timer->get_period_ticks();

            // Calculate Compare Value
            // duty is 0.0 to 1.0.
            // Result is [min_duty*period, max_duty*period]
            const uint32_t compare = (uint32_t)(clamped_duty * (float)period);

            // Apply Compare Value
            LL_HRTIM_TIM_SetCompare1(ControllerTraitsT::skInstance, TimerTraitsT::skLLID, compare);
        }
    };

    using HRTIM1TimerAHalfBridgeDriver = HRTIMHalfBridgeDriver<HRTIM1TimerADevice>;
    using HRTIM1TimerBHalfBridgeDriver = HRTIMHalfBridgeDriver<HRTIM1TimerBDevice>;
    using HRTIM1TimerCHalfBridgeDriver = HRTIMHalfBridgeDriver<HRTIM1TimerCDevice>;
    using HRTIM1TimerDHalfBridgeDriver = HRTIMHalfBridgeDriver<HRTIM1TimerDDevice>;
    using HRTIM1TimerEHalfBridgeDriver = HRTIMHalfBridgeDriver<HRTIM1TimerEDevice>;
    using HRTIM1TimerFHalfBridgeDriver = HRTIMHalfBridgeDriver<HRTIM1TimerFDevice>;

}  // namespace valle