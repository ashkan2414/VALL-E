#pragma once

#include <optional>

#include "valle/platform/devices/hrtim.hpp"

namespace valle::platform
{
    struct HrtimHalfBridgeOutputConfig
    {
        HrtimTimerOutputPolarity   polarity    = HrtimTimerOutputPolarity::kPositive;
        HrtimTimerOutputIdleMode   idle_mode   = HrtimTimerOutputIdleMode::kBurst;
        HrtimTimerOutputIdleLevel  idle_level  = HrtimTimerOutputIdleLevel::kInactive;
        HrtimTimerOutputFaultState fault_state = HrtimTimerOutputFaultState::kInactive;
        HrtimTimerOutputGpioConfig gpio_config{};
    };

    struct HrtimHalfBridgeDriverConfig
    {
        // --- Basic Timing ---
        uint64_t               freq_hz       = 50000;  // PWM Frequency in Hz
        uint8_t                repetition    = 0;      // Repetition value for interrupt generation
        HrtimTimerRolloverMode rollover_mode = HrtimTimerRolloverMode::kPeriodReset;

        HrtimTimerInterruptConfig interrupt_config{};

        // Outputs
        HrtimHalfBridgeOutputConfig output_config{};

        // Deadtime (Shoot-through protection)
        std::optional<HrtimTimerDeadTimeConfig> deadtime_config =
            HrtimTimerDeadTimeConfig{.rise_ns = 200.0F, .fall_ns = 200.0F};

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

        // --- Advanced Features (not commonly used) ---
        // Set this to match adc trigger if using ADC sync for better timing accuracy.
        HrtimTimerCompareUnit compare_unit = HrtimTimerCompareUnit::kCompare1;

        [[nodiscard]] constexpr uint64_t get_int_freq_hz() const
        {
            return freq_hz / (repetition + 1);
        }
    };

    template <typename THrtimTimerDevice>
    class HrtimHalfBridgeDriver
    {
    public:
        using InjectDevices     = TypeList<THrtimTimerDevice>;
        using HrtimTimerT       = THrtimTimerDevice;
        using ControllerTraitsT = HrtimTimerT::ControllerTraitsT;
        using TimerTraitsT      = HrtimTimerT::TimerTraitsT;
        using InterfaceT        = HrtimTimerT::InterfaceT;

        static constexpr size_t skIndex = HrtimTimerT::skIndex;

        static constexpr auto skPeripheralId = HrtimTimerT::skPeripheralId;
        static constexpr auto skTimerId      = HrtimTimerT::skTimerId;

        static_assert(HrtimTimerT::skHasOutput1Pin && HrtimTimerT::skHasOutput2Pin,
                      "HrtimHalfBridgeDriver requires a timer both GPIO outputs for complementary control.");

    private:
        [[no_unique_address]] DeviceRef<HrtimTimerT> m_timer;
        HrtimTimerCompareUnit                        m_compare_unit = HrtimTimerCompareUnit::kCompare1;
        float                                        m_min_duty     = 0.0F;
        float                                        m_max_duty     = 1.0F;

    public:
        HrtimHalfBridgeDriver() = delete;
        explicit HrtimHalfBridgeDriver(DeviceRef<HrtimTimerT>&& device) : m_timer(std::move(device))
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
        [[nodiscard]] bool init(const HrtimHalfBridgeDriverConfig& config)
        {
            if (!m_timer->init())
            {
                return false;
            }

            if (config.min_duty > config.max_duty)
            {
                VALLE_LOG_ERROR("HRTIM{} Timer {} Half-bridge PWM invalid duty cycle limits: min_duty {} > max_duty {}",
                                static_cast<int>(skPeripheralId),
                                enum_name(skTimerId),
                                config.min_duty,
                                config.max_duty);
                return false;
            }

            if (config.max_duty > 1.0F)
            {
                VALLE_LOG_ERROR("HRTIM{} Timer {} Half-bridge PWM max_duty {} is above 100%!",
                                static_cast<int>(skPeripheralId),
                                enum_name(skTimerId),
                                config.max_duty);
                return false;
            }

            if (config.min_duty < 0.0F)
            {
                VALLE_LOG_ERROR("HRTIM{} Timer {} Half-bridge PWM min_duty {} is below 0%!",
                                static_cast<int>(skPeripheralId),
                                enum_name(skTimerId),
                                config.min_duty);
                return false;
            }

            m_compare_unit = config.compare_unit;
            m_min_duty     = config.min_duty;
            m_max_duty     = config.max_duty;

            // Calculate Period and Prescaler
            const uint32_t f_hrtim_hz = m_timer->get_freq_hz();

            const uint64_t min_freq_hz = InterfaceT::calculate_min_freq_hz(f_hrtim_hz);
            if (config.freq_hz < min_freq_hz)
            {
                VALLE_LOG_ERROR(
                    "HRTIM{} Timer {} Half-bridge PWM target frequency {} Hz is too low. Must be above {} Hz.",
                    static_cast<int>(skPeripheralId),
                    enum_name(skTimerId),
                    config.freq_hz,
                    min_freq_hz);

                return false;
            }

            const uint64_t max_freq_hz = InterfaceT::calculate_max_freq_hz(f_hrtim_hz);
            if (config.freq_hz > max_freq_hz)
            {
                VALLE_LOG_ERROR(
                    "HRTIM{} Timer {} Half-bridge PWM target frequency {} Hz is too high. Must be below {} Hz.",
                    static_cast<int>(skPeripheralId),
                    enum_name(skTimerId),
                    config.freq_hz,
                    max_freq_hz);

                return false;
            }

            const HrtimTimerPrescaler prescaler =
                InterfaceT::calculate_timer_prescaler_for_freq(f_hrtim_hz, config.freq_hz);

            const uint64_t h_hrck_hz    = InterfaceT::calculate_hrck_freq_hz(f_hrtim_hz, prescaler);
            uint16_t       period_ticks = static_cast<uint16_t>(h_hrck_hz / config.freq_hz);

            // In Center-Aligned mode (up-down counting), one full cycle consists of counting up
            // to the period and back down to zero. Therefore, the period register must be set
            // to half the number of ticks required for the full cycle frequency.
            if (config.center_aligned)
            {
                period_ticks /= 2;
            }

            // Configure Compare Unit (Duty Cycle), initialize to 50%
            InterfaceT::set_compare(m_compare_unit, period_ticks / 2);
            HrtimTimerCountingMode      counting_mode        = HrtimTimerCountingMode::kUp;
            HrtimTimerOutputSetSource   output1_set_source   = HrtimTimerOutputSetSource::kNone;
            HrtimTimerOutputResetSource output1_reset_source = HrtimTimerOutputResetSource::kNone;
            HrtimTimerOutputSetSource   output2_set_source   = HrtimTimerOutputSetSource::kNone;
            HrtimTimerOutputResetSource output2_reset_source = HrtimTimerOutputResetSource::kNone;

            const HrtimTimerOutputSetSource compare_unit_set_source =
                InterfaceT::set_source_for_compare_unit(config.compare_unit);
            const HrtimTimerOutputResetSource compare_unit_reset_source =
                InterfaceT::reset_source_for_compare_unit(config.compare_unit);

            if (config.center_aligned)
            {
                counting_mode = HrtimTimerCountingMode::kUpDown;

                // Center-aligned PWM: Use CMP1 as both Set and Reset source.
                // In Up-Down counting mode (RM0440 §28.3.6), the hardware automatically
                // swaps Set/Reset crossbar events when the counter is decrementing:
                //   Up-count CMP1 match:   RESET wins (Set becomes Reset) → output LOW
                //   Down-count CMP1 match: SET wins (Reset becomes Set)   → output HIGH
                // This produces a symmetric center-aligned pulse with duty = CMP1/PER,
                // giving a linear 0–100% duty range with compare = duty * period.
                output1_set_source   = HrtimTimerOutputSetSource::kNone;
                output1_reset_source = compare_unit_reset_source;

                // Complementary output automatically handled by hardware with deadtime insertion
                if (!config.deadtime_config.has_value())
                {
                    // If deadtime is disabled, we can still use the hardware swapping feature
                    // to generate complementary waveforms.
                    // TODO: verify with oscilloscope that this works as expected without deadtime.
                    output2_set_source   = compare_unit_set_source;
                    output2_reset_source = HrtimTimerOutputResetSource::kNone;
                }
            }
            else
            {
                counting_mode = HrtimTimerCountingMode::kUp;

                // Logic: HIGH at Period Start, LOW at Compare Match
                output1_set_source   = HrtimTimerOutputSetSource::kTimerPeriod;
                output1_reset_source = compare_unit_reset_source;

                // Complementary output automatically handled by hardware with deadtime insertion
                if (!config.deadtime_config.has_value())
                {
                    // Logic: HIGH at Compare Match, LOW at Period Start
                    output2_set_source   = compare_unit_set_source;
                    output2_reset_source = HrtimTimerOutputResetSource::kTimerPeriod;
                }
            }

            // Configure Timer Time Base
            if (!m_timer->init_counter(HrtimTimerCounterConfig{
                    .prescaler     = prescaler,
                    .counter_mode  = HrtimTimerCounterMode::kContinuous,
                    .counting_mode = counting_mode,
                    .rollover_mode = config.rollover_mode,
                    .period        = period_ticks,
                    .repetition    = config.repetition,
                }))
            {
                return false;
            }

            if (!m_timer->init_output1(
                    HrtimTimerVirtualOutputConfig{
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
                    HrtimTimerVirtualOutputConfig{
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
         * @brief Initializes an ADC trigger for the timer. This allows the timer to trigger ADC conversions at specific
         * points in the PWM cycle, which is critical for synchronized sampling in motor control applications.
         *
         * @tparam tkTriggerId The ID of the ADC trigger to configure (e.g., kTrig1, kTrig2, etc.). Each timer has
         * multiple trigger outputs that can be independently configured.
         * @param config The configuration for the ADC trigger, including the source event (e.g., timer period, compare
         * match, etc.) and the rollover mode (when the trigger should occur in up-down counting).
         * @return true if the ADC trigger was successfully initialized, false otherwise (e.g., invalid trigger ID,
         * incompatible configuration, etc.).
         * @return false if the trigger ID is invalid or if the configuration is incompatible with the timer's counting
         * mode. For example, certain trigger sources may not be valid in center-aligned mode, and the function should
         * validate this before applying the configuration.
         */
        template <HrtimTimerAdcTriggerId tkTriggerId>
        [[nodiscard]] inline bool init_adc_trigger(const HrtimTimerAdcTriggerConfig<skTimerId, tkTriggerId>& config)
        {
            return m_timer->init_adc_trigger(config);
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
            LL_HRTIM_TIM_SetRepetition(ControllerTraitsT::skInstance, TimerTraitsT::skLLId, repetition);
        }

        // ------------------------------------------------------------------------
        // Runtime Control (Hot Path)
        // ------------------------------------------------------------------------
        // duty: 0.0 to 1.0
        inline void set_duty_cycle(const float duty)
        {
            const float clamped_duty = std::clamp(duty, m_min_duty, m_max_duty);

            // Get current period ticks (cached in hardware register)
            // TODO: should we cache in memory for faster access?
            const uint32_t period = InterfaceT::get_period_ticks();

            // Calculate Compare Value
            // duty is 0.0 to 1.0.
            // Result is [min_duty*period, max_duty*period]
            const uint32_t compare = static_cast<uint32_t>(clamped_duty * static_cast<float>(period));

            // Apply Compare Value
            InterfaceT::set_compare(m_compare_unit, compare);
        }
    };

    using Hrtim1TimerAHalfBridgeDriver = HrtimHalfBridgeDriver<Hrtim1TimerADevice>;
    using Hrtim1TimerBHalfBridgeDriver = HrtimHalfBridgeDriver<Hrtim1TimerBDevice>;
    using Hrtim1TimerCHalfBridgeDriver = HrtimHalfBridgeDriver<Hrtim1TimerCDevice>;
    using Hrtim1TimerDHalfBridgeDriver = HrtimHalfBridgeDriver<Hrtim1TimerDDevice>;
    using Hrtim1TimerEHalfBridgeDriver = HrtimHalfBridgeDriver<Hrtim1TimerEDevice>;
    using Hrtim1TimerFHalfBridgeDriver = HrtimHalfBridgeDriver<Hrtim1TimerFDevice>;

}  // namespace valle::platform