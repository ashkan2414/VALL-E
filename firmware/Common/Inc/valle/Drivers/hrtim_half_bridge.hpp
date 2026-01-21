#pragma once

#include "Valle/Device/Devices/hrtim.hpp"

struct HRTIMHalfBridgeConfig
{
    // --- Basic Timing ---
    uint32_t freq_hz;  // PWM Frequency in Hz

    // --- Interrupts ---
    bool     int_enable;    // Enable periodic interrupt
    uint32_t int_rep;       // Number of PWM periods between interrupts (at 20khz, 0 for 20khz, 1 for 10khz, etc.)
    uint32_t int_priority;  // Interrupt priority (0 = highest)

    // --- Outputs ---
    HRTIMOutputChannelConfig output_channel_config;

    // --- Deadtime (Shoot-through protection) ---
    bool  deadtime_enabled;  // Enable deadtime insertion
    float deadtime_fall_ns;  // Deadtime in nanoseconds for falling edge
    float deadtime_rise_ns;  // Deadtime in nanoseconds for rising edge

    // --- Safety & Hardware Limits ---
    float min_duty;  // Clamp (0.0 to 1.0). Usually 0.0.
    float max_duty;  // Clamp (0.0 to 1.0). CRITICAL for bootstrapped gate drivers.
                     // If you hit 100% on a bootstrap driver, the high-side caps drain
                     // and the MOSFET turns off (or enters linear region and explodes).
                     // Recommended: 0.95 or 0.98.

    bool     fault_enable;  // Enable hardware fault input (e.g., Comparator detection)
    uint32_t fault_source;  // LL_HRTIM_FAULT_1, LL_HRTIM_FAULT_2, etc.

    // --- Polarity / Topology ---
    // If true, PWM is centered (Up-Down count). Better for EMI and ADC sampling.
    // If false, PWM is edge-aligned (Sawtooth). Simpler math.
    bool center_aligned;

    [[nodiscard]] inline constexpr uint64_t get_int_freq_hz() const
    {
        return freq_hz / (int_rep + 1);
    }

    [[nodiscard]] static constexpr HRTIMHalfBridgeConfig default_config()
    {
        HRTIMHalfBridgeConfig config;
        config.freq_hz = 100000;  // 100 kHz is the sweet spot for G4 + Si MOSFETs

        config.int_enable   = true;  // Enable periodic interrupts
        config.int_rep      = 3;     // Run control loop every 4 PWM periods (25 kHz control loop).
        config.int_priority = 1;     // High Priority

        // Default Output Channel Config
        config.output_channel_config = HRTIMOutputChannelConfig::get_default();

        // Enable Deadtime. NEVER disable this on a real bridge unless you enjoy fire.
        config.deadtime_enabled = true;

        // 200ns is a "Safe" conservative value for standard MOSFETs.
        // Tune this down (to ~50ns) later based on your specific FET datasheet
        // (Rise Time + Fall Time + Delay).
        config.deadtime_fall_ns = 200.0f;
        config.deadtime_rise_ns = 200.0f;

        // Allow full 0% off.
        config.min_duty = 0.00f;

        // Cap at 95% to ensure the Low-Side turns on for at least 5% of the time.
        // This recharges the bootstrap capacitor for the High-Side gate driver.
        config.max_duty = 0.95f;

        // Defaults for complex features usually OFF until explicitly needed.
        config.fault_enable = false;
        config.fault_source = LL_HRTIM_FAULT_1 | LL_HRTIM_FAULT_2;

        // Center Aligned = TRUE.
        // This creates a symmetrical triangular carrier.
        // It drastically reduces EMI compared to Edge-Aligned.
        config.center_aligned = true;

        return config;
    }
};

template <typename TDevice>
class HRTIMHalfBridgeDriver
{
public:
    using InjectDevices             = TypeList<TDevice>;
    using DeviceT                   = TDevice;
    using TraitsT                   = DeviceT::TraitsT;
    static constexpr size_t skIndex = DeviceT::skIndex;

private:
    DeviceRef<DeviceT>    m_device;
    HRTIMHalfBridgeConfig m_config;

public:
    HRTIMHalfBridgeDriver(DeviceRef<DeviceT> device) : m_device(std::move(device))
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
        LL_HRTIM_TIM_SetPrescaler(HRTIM1, TraitsT::skTimerIdx, prescaler);
        LL_HRTIM_TIM_SetPeriod(HRTIM1, TraitsT::skTimerIdx, period_ticks);

        // Configure Compare Unit 1 (Duty Cycle)
        // Initialize to 50% (Zero Force)
        LL_HRTIM_TIM_SetCompare1(HRTIM1, TraitsT::skTimerIdx, period_ticks / 2);

        LL_HRTIM_TIM_SetCounterMode(HRTIM1, TraitsT::skTimerIdx, LL_HRTIM_MODE_CONTINUOUS);
        if (config.center_aligned)
        {
            LL_HRTIM_TIM_SetCountingMode(HRTIM1, TraitsT::skTimerIdx, LL_HRTIM_COUNTING_MODE_UP_DOWN);

            // Configure OUTPUT 1 (Forward Phase)
            // Logic: HIGH at Period Start, LOW at Compare Match
            LL_HRTIM_OUT_SetOutputSetSrc(HRTIM1, TraitsT::skOutput1, LL_HRTIM_OUTPUTSET_TIMCMP1);
            LL_HRTIM_OUT_SetOutputResetSrc(HRTIM1, TraitsT::skOutput1, LL_HRTIM_OUTPUTRESET_NONE);

            // Complimentary output automatically handled by hardware with deadtime insertion
            if (!config.deadtime_enabled)
            {
                // Configure OUTPUT 2 (Reverse Phase / Complementary)
                // Logic: HIGH at Compare Match, LOW at Period Start
                LL_HRTIM_OUT_SetOutputSetSrc(HRTIM1, TraitsT::skOutput2, LL_HRTIM_OUTPUTSET_NONE);
                LL_HRTIM_OUT_SetOutputResetSrc(HRTIM1, TraitsT::skOutput2, LL_HRTIM_OUTPUTRESET_TIMCMP1);
            }
        }
        else
        {
            LL_HRTIM_TIM_SetCountingMode(HRTIM1, TraitsT::skTimerIdx, LL_HRTIM_COUNTING_MODE_UP);

            // Configure OUTPUT 1 (Forward Phase)
            // Logic: HIGH at Period Start, LOW at Compare Match
            LL_HRTIM_OUT_SetOutputSetSrc(HRTIM1, TraitsT::skOutput1, LL_HRTIM_OUTPUTSET_TIMPER);
            LL_HRTIM_OUT_SetOutputResetSrc(HRTIM1, TraitsT::skOutput1, LL_HRTIM_OUTPUTRESET_TIMCMP1);

            // Complimentary output automatically handled by hardware with deadtime insertion
            if (!config.deadtime_enabled)
            {
                // Configure OUTPUT 2 (Reverse Phase / Complementary)
                // Logic: HIGH at Compare Match, LOW at Period Start
                LL_HRTIM_OUT_SetOutputSetSrc(HRTIM1, TraitsT::skOutput2, LL_HRTIM_OUTPUTSET_TIMCMP1);
                LL_HRTIM_OUT_SetOutputResetSrc(HRTIM1, TraitsT::skOutput2, LL_HRTIM_OUTPUTRESET_TIMPER);
            }
        }

        m_device.get().init_output1_channel(config.output_channel_config);
        m_device.get().init_output2_channel(config.output_channel_config);

        // Deadtime (Optional but recommended for H-Bridge)
        if (config.deadtime_enabled)
        {
            m_device.get().init_deadtime(config.deadtime_rise_ns, config.deadtime_fall_ns);
        }
        else
        {
            m_device.get().disable_deadtime();
        }

        // Fault Setup
        if (config.fault_enable)
        {
            // Enable specific fault sources for this output
            LL_HRTIM_TIM_EnableFault(HRTIM1, TraitsT::skTimerIdx, config.fault_source);
            // Lock if requested
            // if (config.fault_lock)
            // {
            //     LL_HRTIM_FLT_Lock(HRTIM1, LL_HRTIM_FAULT_LOCK_READ_WRITE);
            // }
        }

        // Enable Interrupts
        if (config.int_enable)
        {
            m_device.get().init_interrupts(config.int_rep, config.int_priority);
        }

        enable_output();

        // Start Timer
        LL_HRTIM_TIM_CounterEnable(HRTIM1, TraitsT::skTimerIdx);

        m_config = config;
    }

    /**
     * @brief Enables the half-bridge output.
     *
     */
    inline void enable_output()
    {
        LL_HRTIM_EnableOutput(HRTIM1, TraitsT::skOutput1 | TraitsT::skOutput2);
        LL_HRTIM_TIM_CounterEnable(HRTIM1, TraitsT::skTimerIdx);
    }

    /**
     * @brief Disables the half-bridge output.
     *
     */
    inline void disable_output()
    {
        LL_HRTIM_DisableOutput(HRTIM1, TraitsT::skOutput1 | TraitsT::skOutput2);
        LL_HRTIM_TIM_CounterDisable(HRTIM1, TraitsT::skTimerIdx);
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
        LL_HRTIM_TIM_SetCompare1(HRTIM1, TraitsT::skTimerIdx, compare);
    }
};

using HRTIMHalfBridgeDriverA = HRTIMHalfBridgeDriver<HRTIMTimerDeviceA>;
using HRTIMHalfBridgeDriverB = HRTIMHalfBridgeDriver<HRTIMTimerDeviceB>;
using HRTIMHalfBridgeDriverC = HRTIMHalfBridgeDriver<HRTIMTimerDeviceC>;
using HRTIMHalfBridgeDriverD = HRTIMHalfBridgeDriver<HRTIMTimerDeviceD>;
using HRTIMHalfBridgeDriverE = HRTIMHalfBridgeDriver<HRTIMTimerDeviceE>;
using HRTIMHalfBridgeDriverF = HRTIMHalfBridgeDriver<HRTIMTimerDeviceF>;