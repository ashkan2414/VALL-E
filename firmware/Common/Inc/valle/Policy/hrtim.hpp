#pragma once

#include "stm32g4xx_hal.h"

#ifdef HAL_HRTIM_MODULE_ENABLED

#include <algorithm>
#include <cstdint>

#include "Valle/Utils/hal_utils.hpp"
#include "stm32g474xx.h"
#include "stm32g4xx_ll_hrtim.h"

template <uint8_t tkIndex>
constexpr bool kValidHRTIMIndex = (0 <= tkIndex && tkIndex <= 5);

// ============================================================================
// HARDWARE TRAITS (Map tkIndex -> Registers)
// ============================================================================
template <uint8_t tkIndex>
    requires(kValidHRTIMIndex<tkIndex>)
struct HRTIMTraits;

// Timer A (tkIndex 0)
template <>
struct HRTIMTraits<0>
{
    static constexpr uint32_t  skTimerIdx = HRTIM_TIMERINDEX_TIMER_A;
    static constexpr uint32_t  skOutput1  = HRTIM_OUTPUT_TA1;
    static constexpr uint32_t  skOutput2  = HRTIM_OUTPUT_TA2;
    static constexpr IRQn_Type skIRQn     = HRTIM1_TIMA_IRQn;
};

// Timer B (tkIndex 1)
template <>
struct HRTIMTraits<1>
{
    static constexpr uint32_t  skTimerIdx = HRTIM_TIMERINDEX_TIMER_B;
    static constexpr uint32_t  skOutput1  = HRTIM_OUTPUT_TB1;
    static constexpr uint32_t  skOutput2  = HRTIM_OUTPUT_TB2;
    static constexpr IRQn_Type skIRQn     = HRTIM1_TIMB_IRQn;
};

// Timer C (tkIndex 2)
template <>
struct HRTIMTraits<2>
{
    static constexpr uint32_t  skTimerIdx = HRTIM_TIMERINDEX_TIMER_C;
    static constexpr uint32_t  skOutput1  = HRTIM_OUTPUT_TC1;
    static constexpr uint32_t  skOutput2  = HRTIM_OUTPUT_TC2;
    static constexpr IRQn_Type skIRQn     = HRTIM1_TIMC_IRQn;
};

// Timer D (tkIndex 3)
template <>
struct HRTIMTraits<3>
{
    static constexpr uint32_t  skTimerIdx = HRTIM_TIMERINDEX_TIMER_D;
    static constexpr uint32_t  skOutput1  = HRTIM_OUTPUT_TD1;
    static constexpr uint32_t  skOutput2  = HRTIM_OUTPUT_TD2;
    static constexpr IRQn_Type skIRQn     = HRTIM1_TIMD_IRQn;
};

// Timer E (tkIndex 4)
template <>
struct HRTIMTraits<4>
{
    static constexpr uint32_t  skTimerIdx = HRTIM_TIMERINDEX_TIMER_E;
    static constexpr uint32_t  skOutput1  = HRTIM_OUTPUT_TE1;
    static constexpr uint32_t  skOutput2  = HRTIM_OUTPUT_TE2;
    static constexpr IRQn_Type skIRQn     = HRTIM1_TIME_IRQn;
};

// Timer F (tkIndex 5)
template <>
struct HRTIMTraits<5>
{
    static constexpr uint32_t  skTimerIdx = HRTIM_TIMERINDEX_TIMER_F;
    static constexpr uint32_t  skOutput1  = HRTIM_OUTPUT_TF1;
    static constexpr uint32_t  skOutput2  = HRTIM_OUTPUT_TF2;
    static constexpr IRQn_Type skIRQn     = HRTIM1_TIMF_IRQn;
};

// ============================================================================
// THE HARDWARE POLICY
// ============================================================================
template <uint32_t tkIndex>
    requires(kValidHRTIMIndex<tkIndex>)
class HRTIMPolicy
{
public:
    using Traits                    = HRTIMTraits<tkIndex>;
    static constexpr size_t skIndex = tkIndex;

    // ------------------------------------------------------------------------
    // Initialization
    // ------------------------------------------------------------------------

    static inline void init_deadtime(const float rise_ns, const float fall_ns)
    {
        uint32_t prescaler_val  = 0;
        uint32_t rising_counts  = 0;
        uint32_t falling_counts = 0;

        const uint32_t f_hrtim_hz = get_apb2_timer_clock_freq();

        // Get the max requested time
        const float max_ns = std::max<float>(fall_ns, rise_ns);

        // Calculate "Base Counts"
        // This is the number of ticks required if Prescaler = 0 (Freq = 8 * f_HRTIM).
        // Use uint64_t to prevent overflow (Max ns * 170MHz * 8 can be large).
        uint64_t base_counts = (uint64_t)((max_ns * 8.0f * static_cast<float>(f_hrtim_hz)) / 1e9f);

        // Handle trivial case to avoid clz(0) behavior
        if (base_counts != 0)
        {
            // Calculate Prescaler 'p' using CLZ
            // Formula: p = MSB_Index - 8
            // __builtin_clz(x) returns number of leading zeros.
            // MSB_Index = 31 - clz(x) (for 32-bit numbers)
            // Note: We cast base_counts to u32 because deadtime won't exceed 32-bit range in practice.
            int32_t p = 31 - __builtin_clz((uint32_t)base_counts) - 8;

            // Clamp p to valid range [0, 7]
            prescaler_val = std::clamp<int32_t>(p, 0, 7);

            // Calculate Final Register Values
            // Apply the chosen prescaler (divide counts by 2^p)
            rising_counts  = (uint32_t)((rise_ns * 8.0f * static_cast<float>(f_hrtim_hz) / 1e9f) / (float)(1 << p));
            falling_counts = (uint32_t)((fall_ns * 8.0f * static_cast<float>(f_hrtim_hz) / 1e9f) / (float)(1 << p));

            // Clamp to max allowed values
            if (rising_counts > 511)
            {
                rising_counts = 511;
            }

            if (falling_counts > 511)
            {
                falling_counts = 511;
            }
        }

        LL_HRTIM_DT_SetPrescaler(HRTIM1, Traits::skTimerIdx, prescaler_val);
        LL_HRTIM_DT_SetRisingValue(HRTIM1, Traits::skTimerIdx, rising_counts);
        LL_HRTIM_DT_SetFallingValue(HRTIM1, Traits::skTimerIdx, falling_counts);
        LL_HRTIM_TIM_EnableDeadTime(HRTIM1, Traits::skTimerIdx);
    }

    static inline void init_interrupts(const uint32_t rep, const uint32_t priority)
    {
        // Configure Repetition (For Control Loop Decimation)
        LL_HRTIM_TIM_SetRepetition(HRTIM1, Traits::skTimerIdx, rep);

        // Enable Interrupts
        LL_HRTIM_EnableIT_REP(HRTIM1, Traits::skTimerIdx);
        NVIC_EnableIRQ(Traits::skIRQn);
        NVIC_SetPriority(Traits::skIRQn, priority);
    }

    // ------------------------------------------------------------------------
    // Helpers
    // ------------------------------------------------------------------------

    // Static helper for the ISR
    static inline void clear_interrupt_flag()
    {
        LL_HRTIM_ClearFlag_REP(HRTIM1, Traits::skTimerIdx);
    }

    // ------------------------------------------------------------------------
    // Info
    // ------------------------------------------------------------------------
    static inline uint32_t get_hrtim_freq_hz()
    {
        return get_apb2_timer_clock_freq();
    }

    static inline uint64_t get_hrck_freq_hz()
    {
        const uint32_t f_hrtim_hz = get_hrtim_freq_hz();
        const uint32_t prescaler  = LL_HRTIM_TIM_GetPrescaler(HRTIM1, Traits::skTimerIdx);
        return (f_hrtim_hz << 5) >> prescaler;  // f_hrtim * 32 / (2^prescaler)
    }

    static inline uint64_t get_timer_freq_hz()
    {
        const uint32_t f_hrck_hz = static_cast<uint32_t>(get_hrck_freq_hz());
        const uint32_t period    = LL_HRTIM_TIM_GetPeriod(HRTIM1, Traits::skTimerIdx) + 1;
        return f_hrck_hz / period;
    }

    static inline uint64_t get_int_freq_hz()
    {
        const uint64_t timer_freq_hz = get_timer_freq_hz();
        const uint32_t rep           = LL_HRTIM_TIM_GetRepetition(HRTIM1, Traits::skTimerIdx);
        const uint32_t f_int_hz      = timer_freq_hz / (rep + 1);
        return f_int_hz;
    }

    static inline uint32_t get_prescalar_for_freq(const uint32_t target_freq_hz)
    {
        // We need to find the appropiate prescalar for the clock.
        // Each prescalar will have a minimum allowable PWM frequency since the counter is 16-bit.
        // This minimum frequency is given by f_min = f_hrtim * prescalar / 2^16
        // We need to find the smallest integer i such that f_hrtim * 32 / (2^i * f_target) <= 65535.
        // We arrive at the following condition for i:
        // i > log2(f_hrtim / f_target) - 11 --> i = floor(log2(f_hrtim / f_target)) - 10
        // To implement this efficienctly, we can take advantage of the fact that floor(log2(floor(x))) = floor(log2(x))
        // and then we can use __CLZ to compute floor(log2(x)) = 31 - __CLZ(x) for 32-bit integers.

        const uint32_t f_hrtim_hz = get_hrtim_freq_hz();
        const uint32_t ratio      = f_hrtim_hz / target_freq_hz;
        const int8_t   index      = 31 - __builtin_clz(ratio) - 10;
        return std::clamp<int8_t>(index, 0, 7);
    }

protected:
    HRTIMPolicy() = default;  // Static class
};

using HRTIMPolicyA = HRTIMPolicy<0>;
using HRTIMPolicyB = HRTIMPolicy<1>;
using HRTIMPolicyC = HRTIMPolicy<2>;
using HRTIMPolicyD = HRTIMPolicy<3>;
using HRTIMPolicyE = HRTIMPolicy<4>;
using HRTIMPolicyF = HRTIMPolicy<5>;

// ============================================================================
// HALF-BRIDGE POLICY
// ============================================================================

struct HRTIMHalfBridgeConfig
{
    // --- Basic Timing ---
    uint32_t freq_hz;  // PWM Frequency in Hz

    // --- Interrupts ---
    bool     int_enable;    // Enable periodic interrupt
    uint32_t int_rep;       // Number of PWM periods between interrupts (at 20khz, 0 for 20khz, 1 for 10khz, etc.)
    uint32_t int_priority;  // Interrupt priority (0 = highest)

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

    // --- ADC Synchronization ---
    bool adc_trigger_en;  // specific ADC trigger output enable (e.g. ADC Trigger 1)

    static constexpr HRTIMHalfBridgeConfig default_config()
    {
        HRTIMHalfBridgeConfig config;
        config.freq_hz = 100000;  // 100 kHz is the sweet spot for G4 + Si MOSFETs

        config.int_enable   = true;  // Enable periodic interrupts
        config.int_rep      = 3;     // Run control loop every 4 PWM periods (25 kHz control loop).
        config.int_priority = 1;     // High Priority

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
        config.fault_source = LL_HRTIM_FAULT_1;

        // Center Aligned = TRUE.
        // This creates a symmetrical triangular carrier.
        // It drastically reduces EMI compared to Edge-Aligned.
        config.center_aligned = true;

        // Usually you want this ON to sync ADC to PWM center
        config.adc_trigger_en = true;

        return config;
    }

    inline constexpr uint64_t get_int_freq_hz() const
    {
        return freq_hz / (int_rep + 1);
    }
};

template <uint8_t tkIndex>
    requires(kValidHRTIMIndex<tkIndex>)
class HRTIMHalfBridgePolicy : public HRTIMPolicy<tkIndex>
{
public:
    using Traits = HRTIMTraits<tkIndex>;

    static constexpr size_t skIndex = tkIndex;

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
        const uint32_t f_hrtim_hz   = this->get_hrtim_freq_hz();
        const uint32_t prescaler    = this->get_prescalar_for_freq(config.freq_hz);
        const uint64_t h_hrck_hz    = (f_hrtim_hz << 5) >> prescaler;
        const uint32_t period_ticks = static_cast<uint32_t>(h_hrck_hz / config.freq_hz);

        // Configure Timer Time Base
        LL_HRTIM_TIM_SetPrescaler(HRTIM1, Traits::skTimerIdx, prescaler);
        LL_HRTIM_TIM_SetPeriod(HRTIM1, Traits::skTimerIdx, period_ticks);

        // Configure Compare Unit 1 (Duty Cycle)
        // Initialize to 50% (Zero Force)
        LL_HRTIM_TIM_SetCompare1(HRTIM1, Traits::skTimerIdx, period_ticks / 2);

        // Configure OUTPUT 1 (Forward Phase)
        // Logic: HIGH at Period Start, LOW at Compare Match
        // Note: Use LL_HRTIM_OUT_... functions, not TIM_...
        LL_HRTIM_OUT_SetOutputSetSrc(HRTIM1, Traits::skOutput1, LL_HRTIM_OUTPUTSET_TIMPER);
        LL_HRTIM_OUT_SetOutputResetSrc(HRTIM1, Traits::skOutput1, LL_HRTIM_OUTPUTRESET_TIMCMP1);

        // Configure OUTPUT 2 (Reverse Phase / Complementary)
        // Logic: HIGH at Compare Match, LOW at Period Start
        LL_HRTIM_OUT_SetOutputSetSrc(HRTIM1, Traits::skOutput2, LL_HRTIM_OUTPUTSET_TIMCMP1);
        LL_HRTIM_OUT_SetOutputResetSrc(HRTIM1, Traits::skOutput2, LL_HRTIM_OUTPUTRESET_TIMPER);

        // Deadtime (Optional but recommended for H-Bridge)
        if (config.deadtime_enabled)
        {
            this->init_deadtime(config.deadtime_rise_ns, config.deadtime_fall_ns);
        }

        // Enable Interrupts
        if (config.int_enable)
        {
            this->init_interrupts(config.int_rep, config.int_priority);
        }
    }

    /**
     * @brief Enables the half-bridge output.
     *
     */
    inline void enable_output()
    {
        LL_HRTIM_EnableOutput(HRTIM1, Traits::skOutput1 | Traits::skOutput2);
        LL_HRTIM_TIM_CounterEnable(HRTIM1, Traits::skTimerIdx);
    }

    /**
     * @brief Disables the half-bridge output.
     *
     */
    inline void disable_output()
    {
        LL_HRTIM_DisableOutput(HRTIM1, Traits::skOutput1 | Traits::skOutput2);
        LL_HRTIM_TIM_CounterDisable(HRTIM1, Traits::skTimerIdx);
    }

    // ------------------------------------------------------------------------
    // Runtime Control (Hot Path)
    // ------------------------------------------------------------------------
    // duty_0_1: 0.0 to 1.0 (0.5 = Zero Force)
    inline void set_duty_cycle(const float duty_0_1)
    {
        const float clamed_duty_0_1 = std::clamp(duty_0_1, 0.05f, 0.95f);

        // Get Period (Cached in register)
        uint32_t period  = LL_HRTIM_TIM_GetPeriod(HRTIM1, Traits::skTimerIdx);
        uint32_t compare = (uint32_t)(clamed_duty_0_1 * (float)period);
        LL_HRTIM_TIM_SetCompare1(HRTIM1, Traits::skTimerIdx, compare);
    }
};

using HRTIMHalfBridgePolicyA = HRTIMHalfBridgePolicy<0>;
using HRTIMHalfBridgePolicyB = HRTIMHalfBridgePolicy<1>;
using HRTIMHalfBridgePolicyC = HRTIMHalfBridgePolicy<2>;
using HRTIMHalfBridgePolicyD = HRTIMHalfBridgePolicy<3>;
using HRTIMHalfBridgePolicyE = HRTIMHalfBridgePolicy<4>;
using HRTIMHalfBridgePolicyF = HRTIMHalfBridgePolicy<5>;

// ============================================================================
// INTERRUPT ROUTER (The Socket)
// ============================================================================
template <uint8_t tkIndex>
    requires(kValidHRTIMIndex<tkIndex>)
struct HRTIMIsrRouter
{
    static void handle()
    {
        // Default: Do nothing (Optimized away)
    }
};

#endif  // HAL_HRTIM_MODULE_ENABLED