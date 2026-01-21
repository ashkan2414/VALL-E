#pragma once

#include "stm32g4xx_hal.h"

#ifdef HAL_HRTIM_MODULE_ENABLED

#include <algorithm>
#include <cstdint>

#include "Valle/Device/device_core.hpp"
#include "Valle/Utils/hal_utils.hpp"
#include "stm32g474xx.h"
#include "stm32g4xx_ll_bus.h"
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
enum class HRTIMOutputPolarity
{
    kPositive = LL_HRTIM_OUT_POSITIVE_POLARITY,  /// Output active high
    kNegative = LL_HRTIM_OUT_NEGATIVE_POLARITY   /// Output active low
};

enum class HRTIMOutputIdleLevel
{
    kInactive = LL_HRTIM_OUT_IDLELEVEL_INACTIVE,  /// Inactive when idle
    kActive   = LL_HRTIM_OUT_IDLELEVEL_ACTIVE     /// Active when idle
};

enum class HRTIMOutputIdleMode
{
    kNoIdle = LL_HRTIM_OUT_NO_IDLE,          /// Never idle
    kBurst  = LL_HRTIM_OUT_IDLE_WHEN_BURST,  /// Idle during burst mode
};

enum class HRTIMOutputFaultState
{
    kInactive = LL_HRTIM_OUT_FAULTSTATE_INACTIVE,   /// Output inactive on fault
    kActive   = LL_HRTIM_OUT_FAULTSTATE_ACTIVE,     /// Output active on fault
    kNoAction = LL_HRTIM_OUT_FAULTSTATE_NO_ACTION,  /// No action on fault
    kHighZ    = LL_HRTIM_OUT_FAULTSTATE_HIGHZ       /// High impedance on fault
};

struct HRTIMOutputChannelConfig
{
    HRTIMOutputPolarity   polarity;
    HRTIMOutputIdleLevel  idle_level;
    HRTIMOutputIdleMode   idle_mode;
    HRTIMOutputFaultState fault_state;

    [[nodiscard]] static inline constexpr HRTIMOutputChannelConfig get_default()
    {
        return HRTIMOutputChannelConfig{.polarity    = HRTIMOutputPolarity::kPositive,
                                        .idle_level  = HRTIMOutputIdleLevel::kInactive,
                                        .idle_mode   = HRTIMOutputIdleMode::kBurst,
                                        .fault_state = HRTIMOutputFaultState::kInactive};
    }
};

template <uint32_t tkIndex>
    requires(kValidHRTIMIndex<tkIndex>)
class HRTIMTimerDevice;

class HRTIMDevice
{
public:
    struct Descriptor : public SharedDeviceDescriptor
    {
        using Children = DeviceList<HRTIMTimerDevice<0>,
                                    HRTIMTimerDevice<1>,
                                    HRTIMTimerDevice<2>,
                                    HRTIMTimerDevice<3>,
                                    HRTIMTimerDevice<4>,
                                    HRTIMTimerDevice<5>>;
    };

    static inline void init()
    {
        // Enable Bus Clock
        LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_HRTIM1);

        // Start DLL Calibration
        LL_HRTIM_StartDLLCalibration(HRTIM1);

        // Wait for calibration to finish
        while (LL_HRTIM_IsActiveFlag_DLLRDY(HRTIM1) == 0);
    }

    static inline void init_output_channel(const uint32_t output, const HRTIMOutputChannelConfig& config)
    {
        LL_HRTIM_OUT_SetPolarity(HRTIM1, output, static_cast<uint32_t>(config.polarity));
        LL_HRTIM_OUT_SetIdleLevel(HRTIM1, output, static_cast<uint32_t>(config.idle_level));
        LL_HRTIM_OUT_SetIdleMode(HRTIM1, output, static_cast<uint32_t>(config.idle_mode));
        LL_HRTIM_OUT_SetFaultState(HRTIM1, output, static_cast<uint32_t>(config.fault_state));
    }
};

template <uint32_t tkIndex>
    requires(kValidHRTIMIndex<tkIndex>)
class HRTIMTimerDevice
{
public:
    struct Descriptor : public UniqueDeviceDescriptor
    {
    };

    using InjectDevices             = TypeList<HRTIMDevice>;
    using TraitsT                   = HRTIMTraits<tkIndex>;
    static constexpr size_t skIndex = tkIndex;
    friend class HRTIMDevice;

private:
    DeviceRef<HRTIMDevice> m_device;

public:
    HRTIMTimerDevice(DeviceRef<HRTIMDevice>&& device) : m_device(device)
    {
    }

    // ------------------------------------------------------------------------
    // Initialization
    // ------------------------------------------------------------------------

    static inline void init_output1_channel(const HRTIMOutputChannelConfig& config)
    {
        HRTIMDevice::init_output_channel(TraitsT::skOutput1, config);
    }

    static inline void init_output2_channel(const HRTIMOutputChannelConfig& config)
    {
        HRTIMDevice::init_output_channel(TraitsT::skOutput2, config);
    }

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

        LL_HRTIM_DT_SetPrescaler(HRTIM1, TraitsT::skTimerIdx, prescaler_val);
        LL_HRTIM_DT_SetRisingValue(HRTIM1, TraitsT::skTimerIdx, rising_counts);
        LL_HRTIM_DT_SetFallingValue(HRTIM1, TraitsT::skTimerIdx, falling_counts);
        LL_HRTIM_DT_LockFalling(HRTIM1, TraitsT::skTimerIdx);
        LL_HRTIM_DT_LockRising(HRTIM1, TraitsT::skTimerIdx);
        LL_HRTIM_TIM_EnableDeadTime(HRTIM1, TraitsT::skTimerIdx);
    }

    static inline void disable_deadtime()
    {
        LL_HRTIM_DT_SetRisingValue(HRTIM1, TraitsT::skTimerIdx, 0);
        LL_HRTIM_DT_SetFallingValue(HRTIM1, TraitsT::skTimerIdx, 0);
        LL_HRTIM_TIM_DisableDeadTime(HRTIM1, TraitsT::skTimerIdx);
    }

    static inline void init_interrupts(const uint32_t rep, const uint32_t priority)
    {
        // Configure Repetition (For Control Loop Decimation)
        LL_HRTIM_TIM_SetRepetition(HRTIM1, TraitsT::skTimerIdx, rep);

        // Enable Interrupts
        LL_HRTIM_EnableIT_REP(HRTIM1, TraitsT::skTimerIdx);
        NVIC_EnableIRQ(TraitsT::skIRQn);
        NVIC_SetPriority(TraitsT::skIRQn, priority);
    }

    // ------------------------------------------------------------------------
    // Helpers
    // ------------------------------------------------------------------------

    // Static helper for the ISR
    static inline void clear_interrupt_flag()
    {
        LL_HRTIM_ClearFlag_REP(HRTIM1, TraitsT::skTimerIdx);
    }

    // ------------------------------------------------------------------------
    // Info
    // ------------------------------------------------------------------------
    [[nodiscard]] static inline uint32_t get_hrtim_freq_hz()
    {
        return get_apb2_timer_clock_freq();
    }

    [[nodiscard]] static inline uint64_t get_hrck_freq_hz()
    {
        const uint32_t f_hrtim_hz = get_hrtim_freq_hz();
        const uint32_t prescaler  = LL_HRTIM_TIM_GetPrescaler(HRTIM1, TraitsT::skTimerIdx);
        return (f_hrtim_hz << 5) >> prescaler;  // f_hrtim * 32 / (2^prescaler)
    }

    [[nodiscard]] static inline uint64_t get_timer_freq_hz()
    {
        const uint32_t f_hrck_hz = static_cast<uint32_t>(get_hrck_freq_hz());
        const uint32_t period    = get_period_ticks() + 1;
        return f_hrck_hz / period;
    }

    [[nodiscard]] static inline uint64_t get_int_freq_hz()
    {
        const uint64_t timer_freq_hz = get_timer_freq_hz();
        const uint32_t rep           = LL_HRTIM_TIM_GetRepetition(HRTIM1, TraitsT::skTimerIdx);
        const uint32_t f_int_hz      = timer_freq_hz / (rep + 1);
        return f_int_hz;
    }

    [[nodiscard]] static inline uint32_t get_prescalar_for_freq(const uint32_t target_freq_hz)
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

    [[nodiscard]] static inline uint32_t get_period_ticks()
    {
        return LL_HRTIM_TIM_GetPeriod(HRTIM1, TraitsT::skTimerIdx);
    }
};

using HRTIMTimerDeviceA = HRTIMTimerDevice<0>;
using HRTIMTimerDeviceB = HRTIMTimerDevice<1>;
using HRTIMTimerDeviceC = HRTIMTimerDevice<2>;
using HRTIMTimerDeviceD = HRTIMTimerDevice<3>;
using HRTIMTimerDeviceE = HRTIMTimerDevice<4>;
using HRTIMTimerDeviceF = HRTIMTimerDevice<5>;

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