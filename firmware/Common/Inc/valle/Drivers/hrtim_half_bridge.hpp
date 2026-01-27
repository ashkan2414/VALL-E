#pragma once

#include "Valle/Device/Devices/hrtim.hpp"

namespace valle
{

    struct HRTIMHalfBridgeConfig
    {
        // --- Basic Timing ---
        uint32_t freq_hz = 100000;  // PWM Frequency in Hz

        // Interrupts
        std::optional<HRTIMRepIntConfig> int_config = std::nullopt;

        // Outputs
        HRTIMOutputChannelConfig output_channel_config = HRTIMOutputChannelConfig{};

        // Deadtime (Shoot-through protection)
        std::optional<HRTIMDeadTimeConfig> deadtime_config = HRTIMDeadTimeConfig{.rise_ns = 200.0F, .fall_ns = 200.0F};

        // --- Safety & Hardware Limits ---
        float min_duty = 0.0F;  // Clamp (0.0 to 1.0). Usually 0.0.

        // Clamp (0.0 to 1.0). CRITICAL for bootstrapped gate drivers.
        // If you hit 100% on a bootstrap driver, the high-side caps drain
        // and the MOSFET turns off (or enters linear region and explodes).
        // Recommended: 0.95 or 0.98.
        float max_duty = 0.95F;  // NOLINT(readability-magic-numbers)

        std::optional<uint32_t> fault_source = std::nullopt;  // Fault source

        // --- Polarity / Topology ---
        // If true, PWM is centered (Up-Down count). Better for EMI and ADC sampling.
        // If false, PWM is edge-aligned (Sawtooth). Simpler math.
        bool center_aligned = true;

        [[nodiscard]] constexpr uint64_t get_int_freq_hz() const
        {
            if (!int_config.has_value())
            {
                return 0;
            }

            return freq_hz / (int_config->int_rep + 1);
        }
    };

    template <typename THRTIMTimerDevice>
    class HRTIMHalfBridgeDriver
    {
    public:
        using InjectDevices             = TypeList<THRTIMTimerDevice>;
        using HRTIMTimerDeviceT         = THRTIMTimerDevice;
        using ControllerTraitsT         = HRTIMTimerDeviceT::ControllerTraitsT;
        using TimerTraitsT              = HRTIMTimerDeviceT::TimerTraitsT;
        static constexpr size_t skIndex = HRTIMTimerDeviceT::skIndex;

    private:
        [[no_unique_address]] DeviceRef<HRTIMTimerDeviceT> m_device;
        HRTIMHalfBridgeConfig                              m_config;

    public:
        HRTIMHalfBridgeDriver() = delete;
        HRTIMHalfBridgeDriver(DeviceRef<HRTIMTimerDeviceT> device) : m_device(std::move(device))
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
        void init(const HRTIMHalfBridgeConfig& config)
        {
            // Calculate Period and Prescaler
            const uint32_t f_hrtim_hz   = m_device.get().get_hrtim_freq_hz();
            const uint32_t prescaler    = m_device.get().get_prescalar_for_freq(config.freq_hz);
            const uint64_t h_hrck_hz    = (f_hrtim_hz << 5) >> prescaler;
            const uint32_t period_ticks = static_cast<uint32_t>(h_hrck_hz / config.freq_hz);

            // Configure Timer Time Base
            LL_HRTIM_TIM_SetPrescaler(ControllerTraitsT::skInstance, TimerTraitsT::skTimerIdx, prescaler);
            LL_HRTIM_TIM_SetPeriod(ControllerTraitsT::skInstance, TimerTraitsT::skTimerIdx, period_ticks);

            // Configure Compare Unit 1 (Duty Cycle)
            // Initialize to 50% (Zero Force)
            LL_HRTIM_TIM_SetCompare1(ControllerTraitsT::skInstance, TimerTraitsT::skTimerIdx, period_ticks / 2);

            LL_HRTIM_TIM_SetCounterMode(
                ControllerTraitsT::skInstance, TimerTraitsT::skTimerIdx, LL_HRTIM_MODE_CONTINUOUS);
            if (config.center_aligned)
            {
                LL_HRTIM_TIM_SetCountingMode(
                    ControllerTraitsT::skInstance, TimerTraitsT::skTimerIdx, LL_HRTIM_COUNTING_MODE_UP_DOWN);

                // Configure OUTPUT 1 (Forward Phase)
                // Logic: HIGH at Period Start, LOW at Compare Match
                LL_HRTIM_OUT_SetOutputSetSrc(
                    ControllerTraitsT::skInstance, TimerTraitsT::skOutput1, LL_HRTIM_OUTPUTSET_TIMCMP1);
                LL_HRTIM_OUT_SetOutputResetSrc(
                    ControllerTraitsT::skInstance, TimerTraitsT::skOutput1, LL_HRTIM_OUTPUTRESET_NONE);

                // Complimentary output automatically handled by hardware with deadtime insertion
                if (!config.deadtime_config.has_value())
                {
                    // Configure OUTPUT 2 (Reverse Phase / Complementary)
                    // Logic: HIGH at Compare Match, LOW at Period Start
                    LL_HRTIM_OUT_SetOutputSetSrc(
                        ControllerTraitsT::skInstance, TimerTraitsT::skOutput2, LL_HRTIM_OUTPUTSET_NONE);
                    LL_HRTIM_OUT_SetOutputResetSrc(
                        ControllerTraitsT::skInstance, TimerTraitsT::skOutput2, LL_HRTIM_OUTPUTRESET_TIMCMP1);
                }
            }
            else
            {
                LL_HRTIM_TIM_SetCountingMode(
                    ControllerTraitsT::skInstance, TimerTraitsT::skTimerIdx, LL_HRTIM_COUNTING_MODE_UP);

                // Configure OUTPUT 1 (Forward Phase)
                // Logic: HIGH at Period Start, LOW at Compare Match
                LL_HRTIM_OUT_SetOutputSetSrc(
                    ControllerTraitsT::skInstance, TimerTraitsT::skOutput1, LL_HRTIM_OUTPUTSET_TIMPER);
                LL_HRTIM_OUT_SetOutputResetSrc(
                    ControllerTraitsT::skInstance, TimerTraitsT::skOutput1, LL_HRTIM_OUTPUTRESET_TIMCMP1);

                // Complimentary output automatically handled by hardware with deadtime insertion
                if (!config.deadtime_config.has_value())
                {
                    // Configure OUTPUT 2 (Reverse Phase / Complementary)
                    // Logic: HIGH at Compare Match, LOW at Period Start
                    LL_HRTIM_OUT_SetOutputSetSrc(
                        ControllerTraitsT::skInstance, TimerTraitsT::skOutput2, LL_HRTIM_OUTPUTSET_TIMCMP1);
                    LL_HRTIM_OUT_SetOutputResetSrc(
                        ControllerTraitsT::skInstance, TimerTraitsT::skOutput2, LL_HRTIM_OUTPUTRESET_TIMPER);
                }
            }

            m_device.get().init_output1_channel(config.output_channel_config);
            m_device.get().init_output2_channel(config.output_channel_config);

            // Deadtime (Optional but recommended for H-Bridge)
            if (config.deadtime_config.has_value())
            {
                m_device.get().init_deadtime(config.deadtime_config.value());
            }
            else
            {
                m_device.get().disable_deadtime();
            }

            // Fault Setup
            if (config.fault_source.has_value())
            {
                // Enable specific fault sources for this output
                LL_HRTIM_TIM_EnableFault(
                    ControllerTraitsT::skInstance, TimerTraitsT::skTimerIdx, config.fault_source.value());
                // Lock if requested
                // if (config.fault_lock)
                // {
                //     LL_HRTIM_FLT_Lock(ControllerTraitsT::skInstance, LL_HRTIM_FAULT_LOCK_READ_WRITE);
                // }
            }

            // Enable Interrupts
            if (config.int_config.has_value())
            {
                m_device.get().init_rep_int(config.int_config.value());
            }

            enable_output();

            // Start Timer
            LL_HRTIM_TIM_CounterEnable(ControllerTraitsT::skInstance, TimerTraitsT::skTimerIdx);

            m_config = config;
        }

        /**
     * @brief Enables the half-bridge output.
     *
     */
        inline void enable_output()
        {
            LL_HRTIM_EnableOutput(ControllerTraitsT::skInstance, TimerTraitsT::skOutput1 | TimerTraitsT::skOutput2);
            LL_HRTIM_TIM_CounterEnable(ControllerTraitsT::skInstance, TimerTraitsT::skTimerIdx);
        }

        /**
     * @brief Disables the half-bridge output.
     *
     */
        inline void disable_output()
        {
            LL_HRTIM_DisableOutput(ControllerTraitsT::skInstance, TimerTraitsT::skOutput1 | TimerTraitsT::skOutput2);
            LL_HRTIM_TIM_CounterDisable(ControllerTraitsT::skInstance, TimerTraitsT::skTimerIdx);
        }

        // ------------------------------------------------------------------------
        // Runtime Control (Hot Path)
        // ------------------------------------------------------------------------
        // duty: 0.0 to 1.0 (0.5 = Zero Force)
        inline void set_duty_cycle(const float duty)
        {
            const float clamed_duty = std::clamp(duty, m_config.min_duty, m_config.max_duty);

            // Get Period (Cached in register)
            const uint32_t period  = m_device.get().get_period_ticks();
            const uint32_t compare = (uint32_t)(clamed_duty * (float)period);
            LL_HRTIM_TIM_SetCompare1(ControllerTraitsT::skInstance, TimerTraitsT::skTimerIdx, compare);
        }
    };

    using HRTIM1TimerAHalfBridgeDriver = HRTIMHalfBridgeDriver<HRTIM1TimerADevice>;
    using HRTIM1TimerBHalfBridgeDriver = HRTIMHalfBridgeDriver<HRTIM1TimerBDevice>;
    using HRTIM1TimerCHalfBridgeDriver = HRTIMHalfBridgeDriver<HRTIM1TimerCDevice>;
    using HRTIM1TimerDHalfBridgeDriver = HRTIMHalfBridgeDriver<HRTIM1TimerDDevice>;
    using HRTIM1TimerEHalfBridgeDriver = HRTIMHalfBridgeDriver<HRTIM1TimerEDevice>;
    using HRTIM1TimerFHalfBridgeDriver = HRTIMHalfBridgeDriver<HRTIM1TimerFDevice>;

}  // namespace valle