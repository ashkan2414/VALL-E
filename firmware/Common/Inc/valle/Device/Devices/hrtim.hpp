#pragma once

#include <algorithm>
#include <cstdint>

#include "Valle/Core/Logging/logger.hpp"
#include "Valle/Core/timing.hpp"
#include "Valle/Device/Traits/hrtim.hpp"
#include "Valle/Device/device_core.hpp"
#include "Valle/Utils/hal_utils.hpp"
#include "stm32g4xx_ll_bus.h"

namespace valle
{

    // =============================================================================
    // CONFIGURATIONS
    // =============================================================================

    struct HRTIMOutputChannelConfig
    {
        HRTIMOutputPolarity   polarity    = HRTIMOutputPolarity::kPositive;
        HRTIMOutputIdleLevel  idle_level  = HRTIMOutputIdleLevel::kInactive;
        HRTIMOutputIdleMode   idle_mode   = HRTIMOutputIdleMode::kBurst;
        HRTIMOutputFaultState fault_state = HRTIMOutputFaultState::kInactive;
    };

    struct HRTIMDeadTimeConfig
    {
        float rise_ns = 0.0F;  // Rising deadtime in nanoseconds
        float fall_ns = 0.0F;  // Falling deadtime in nanoseconds

        // RM0440 Section 29.3.12: DTFLK/DTRLK bits can only be written once after reset
        // Once set, they cannot be cleared until MCU reset
        // Default true for safety, but allow dynamic deadtime adjustment if needed
        bool lock_registers = true;
    };

    struct HRTIMRepIntConfig
    {
        // Repetition value for interrupt generation
        uint32_t int_rep = 0;

        // NVIC Priority (0 = Highest, 15 = Lowest)
        uint32_t int_priority = 5;  // NOLINT(readability-magic-numbers)
    };

    // =============================================================================
    // COMPILE TIME CONFIGURATIONS
    // =============================================================================

    // ============================================================================
    // INTERRUPT TRAITS
    // ============================================================================

    enum class HRTIMInterruptType : uint8_t
    {
        kFault1 = 0,         // FLT1
        kFault2,             // FLT2
        kFault3,             // FLT3
        kFault4,             // FLT4
        kFault5,             // FLT5
        kFault6,             // FLT6
        kSystemFault,        // SFLT
        kDLLReady,           // DLLRDY
        kBurstModePeriod,    // BMPER
        kSync,               // SYNC
        kCompare1,           // CMP1
        kCompare2,           // CMP2
        kCompare3,           // CMP3
        kCompare4,           // CMP4
        kUpdate,             // UPDATE
        kRepitition,         // REP
        kCapture1,           // CPT1
        kCapture2,           // CPT2
        kOutput1Set,         // SET1
        kOutput1Reset,       // RST1
        kOutput2Set,         // SET2
        kOutput2Reset,       // RST2
        kReset,              // RST
        kDelayedProtection,  // DLYPRT
    };

    template <HRTIMControllerID tkControllerID, HRTIMTimerID tkTimerID, HRTIMInterruptType tkIntType>
    struct HRTIMInterruptTraits;

#define DEFINE_HRTIM_INT_TRAIT(tkIntType, ll_name)                                                           \
    template <HRTIMControllerID tkControllerID, HRTIMTimerID tkTimerID>                                      \
    struct HRTIMInterruptTraits<tkControllerID, tkTimerID, tkIntType>                                        \
    {                                                                                                        \
        static inline void enable()                                                                          \
        {                                                                                                    \
            LL_HRTIM_EnableIT_##ll_name(HRTIMControllerTraits<tkControllerID>::skInstance,                   \
                                        HRTIMTimerTraits<tkControllerID, tkTimerID>::skTimerIdx);            \
        }                                                                                                    \
        static inline void disable()                                                                         \
        {                                                                                                    \
            LL_HRTIM_DisableIT_##ll_name(HRTIMControllerTraits<tkControllerID>::skInstance,                  \
                                         HRTIMTimerTraits<tkControllerID, tkTimerID>::skTimerIdx);           \
        }                                                                                                    \
        static inline bool is_enabled()                                                                      \
        {                                                                                                    \
            return LL_HRTIM_IsEnabledIT_##ll_name(HRTIMControllerTraits<tkControllerID>::skInstance,         \
                                                  HRTIMTimerTraits<tkControllerID, tkTimerID>::skTimerIdx);  \
        }                                                                                                    \
        static inline bool flag_active()                                                                     \
        {                                                                                                    \
            return LL_HRTIM_IsActiveFlag_##ll_name(HRTIMControllerTraits<tkControllerID>::skInstance,        \
                                                   HRTIMTimerTraits<tkControllerID, tkTimerID>::skTimerIdx); \
        }                                                                                                    \
                                                                                                             \
        static inline bool is_pending()                                                                      \
        {                                                                                                    \
            return flag_active() && is_enabled();                                                            \
        }                                                                                                    \
        static inline void ack()                                                                             \
        {                                                                                                    \
            LL_HRTIM_ClearFlag_##ll_name(HRTIMControllerTraits<tkControllerID>::skInstance,                  \
                                         HRTIMTimerTraits<tkControllerID, tkTimerID>::skTimerIdx);           \
        }                                                                                                    \
    };

    DEFINE_HRTIM_INT_TRAIT(HRTIMInterruptType::kFault1, FLT1);
    DEFINE_HRTIM_INT_TRAIT(HRTIMInterruptType::kFault2, FLT2);
    DEFINE_HRTIM_INT_TRAIT(HRTIMInterruptType::kFault3, FLT3);
    DEFINE_HRTIM_INT_TRAIT(HRTIMInterruptType::kFault4, FLT4);
    DEFINE_HRTIM_INT_TRAIT(HRTIMInterruptType::kFault5, FLT5);
    DEFINE_HRTIM_INT_TRAIT(HRTIMInterruptType::kFault6, FLT6);
    DEFINE_HRTIM_INT_TRAIT(HRTIMInterruptType::kSystemFault, SYSFLT);
    DEFINE_HRTIM_INT_TRAIT(HRTIMInterruptType::kDLLReady, DLLRDY);
    DEFINE_HRTIM_INT_TRAIT(HRTIMInterruptType::kBurstModePeriod, BMPER);
    DEFINE_HRTIM_INT_TRAIT(HRTIMInterruptType::kSync, SYNC);
    DEFINE_HRTIM_INT_TRAIT(HRTIMInterruptType::kCompare1, CMP1);
    DEFINE_HRTIM_INT_TRAIT(HRTIMInterruptType::kCompare2, CMP2);
    DEFINE_HRTIM_INT_TRAIT(HRTIMInterruptType::kCompare3, CMP3);
    DEFINE_HRTIM_INT_TRAIT(HRTIMInterruptType::kCompare4, CMP4);
    DEFINE_HRTIM_INT_TRAIT(HRTIMInterruptType::kUpdate, UPD);
    DEFINE_HRTIM_INT_TRAIT(HRTIMInterruptType::kRepitition, REP);
    DEFINE_HRTIM_INT_TRAIT(HRTIMInterruptType::kCapture1, CPT1);
    DEFINE_HRTIM_INT_TRAIT(HRTIMInterruptType::kCapture2, CPT2);
    DEFINE_HRTIM_INT_TRAIT(HRTIMInterruptType::kOutput1Set, SET1);
    DEFINE_HRTIM_INT_TRAIT(HRTIMInterruptType::kOutput1Reset, RST1);
    DEFINE_HRTIM_INT_TRAIT(HRTIMInterruptType::kOutput2Set, SET2);
    DEFINE_HRTIM_INT_TRAIT(HRTIMInterruptType::kOutput2Reset, RST2);
    DEFINE_HRTIM_INT_TRAIT(HRTIMInterruptType::kReset, RST);
    DEFINE_HRTIM_INT_TRAIT(HRTIMInterruptType::kDelayedProtection, DLYPRT);

#undef DEFINE_HRTIM_INT_TRAIT

    // =============================================================================
    // GLOBAL ISR ROUTER
    // =============================================================================
    /**
     * @brief Global ISR Router for a specific HRTIM.
     * Specializing this allows you to handle the entire ISR in one function
     * (e.g., when delegating to the ST HAL).
     *
     * @tparam tkControllerID HRTIM Controller ID.
     * @tparam tkTimerID      HRTIM Timer ID.
     */
    template <HRTIMControllerID tkControllerID, HRTIMTimerID tkTimerID>
        requires(kValidHRTIMID<tkControllerID>)
    struct HRTIMGlobalISRRouter
    {
        using UnboundIsrHandlerTag = void;
        static void handle()
        {
            // Default: Do nothing (Optimized away)
        }
    };

    // =============================================================================
    // GRANULAR ISR ROUTER
    // ============================================================================

    /**
     * @brief HRTIM Interrupt Router.
     *
     * Specialize this template in your application or driver to bind
     * logic to specific HRTIM interrupts.
     *
     * @tparam tkControllerID The HRTIM peripheral index the interrupt belongs to.
     * @tparam tkTimerID The HRTIM Timer index the interrupt belongs to.
     * @tparam tkIntType The interrupt type triggered.
     */
    template <HRTIMControllerID tkControllerID, HRTIMTimerID tkTimerID, HRTIMInterruptType tkIntType>
        requires(kValidHRTIMID<tkControllerID>)
    struct HRTIMISRRouter
    {
        using UnboundIsrHandlerTag = void;

        static void handle()
        {
            // Default implementation does nothing.
        }
    };

    // =============================================================================
    // FORWARD DECLARATIONS
    // =============================================================================

    class HRTIMRootDevice;

    template <HRTIMControllerID tkControllerID>
        requires(kValidHRTIMID<tkControllerID>)
    class HRTIMControllerDevice;

    template <HRTIMControllerID tkControllerID, HRTIMTimerID tkTimerID>
        requires(kValidHRTIMID<tkControllerID>)
    class HRTIMTimerDevice;

    // =============================================================================
    // HRTIM DEVICE (SHARED DEVICE)
    // =============================================================================

    class HRTIMRootDevice
    {
    public:
        struct Descriptor : public InterfaceDeviceDescriptor
        {
            using Children = DeviceTreeList<HRTIMControllerDevice<1>>;
        };
    };

    template <HRTIMControllerID tkControllerID>
        requires(kValidHRTIMID<tkControllerID>)
    class HRTIMControllerDevice
    {
    public:
        struct Descriptor : public SharedDeviceDescriptor
        {
            using Children = DeviceTreeList<HRTIMTimerDevice<tkControllerID, HRTIMTimerID::kA>,
                                            HRTIMTimerDevice<tkControllerID, HRTIMTimerID::kB>,
                                            HRTIMTimerDevice<tkControllerID, HRTIMTimerID::kC>,
                                            HRTIMTimerDevice<tkControllerID, HRTIMTimerID::kD>,
                                            HRTIMTimerDevice<tkControllerID, HRTIMTimerID::kE>,
                                            HRTIMTimerDevice<tkControllerID, HRTIMTimerID::kF>>;
        };

        using ControllerTraitsT = HRTIMControllerTraits<tkControllerID>;

        static constexpr HRTIMControllerID skControllerID = tkControllerID;

    private:
        static inline bool s_initialized = false;

    public:
        [[nodiscard]] static inline bool init()
        {
            if (s_initialized)
            {
                VALLE_LOG_WARN("HRTIM{} already initialized, skipping re-initialization.", tkControllerID);
                return true;
            }

            // Enable Bus Clock
            LL_APB2_GRP1_EnableClock(ControllerTraitsT::skClock);

            // Wait for clock to stabilize
            // RM0440: Allow a few APB2 cycles for clock propagation
            delay_cycles(10);

            // Start DLL Calibration
            LL_HRTIM_StartDLLCalibration(ControllerTraitsT::skInstance);

            // Wait for calibration to finish with precise timeout
            // RM0440 Section 29.3.15: DLL calibration time = 14 µs typical
            // Allow up to 100 µs
            const bool calibration_success = wait_for_with_timeout_us(
                []() { return LL_HRTIM_IsActiveFlag_DLLRDY(ControllerTraitsT::skInstance) != 0; }, 100);

            if (!calibration_success)
            {
                VALLE_LOG_ERROR("HRTIM{} DLL Calibration timed out!", tkControllerID);
                return false;
            }

            s_initialized = true;
            return true;
        }

        [[nodiscard]] inline bool post_init()
        {
            // Do nothing for now
            return true;
        }

        static inline void init_output_channel(const uint32_t output, const HRTIMOutputChannelConfig& config)
        {
            LL_HRTIM_OUT_SetPolarity(ControllerTraitsT::skInstance, output, static_cast<uint32_t>(config.polarity));
            LL_HRTIM_OUT_SetIdleLevel(ControllerTraitsT::skInstance, output, static_cast<uint32_t>(config.idle_level));
            LL_HRTIM_OUT_SetIdleMode(ControllerTraitsT::skInstance, output, static_cast<uint32_t>(config.idle_mode));
            LL_HRTIM_OUT_SetFaultState(
                ControllerTraitsT::skInstance, output, static_cast<uint32_t>(config.fault_state));
        }
    };

    using HRTIM1ControllerDevice = HRTIMControllerDevice<1>;

    // =============================================================================
    // HRTIM TIMER DEVICE (UNIQUE DEVICE)
    // =============================================================================

    template <HRTIMControllerID tkControllerID, HRTIMTimerID tkTimerID>
        requires(kValidHRTIMID<tkControllerID>)
    class HRTIMTimerDevice
    {
    public:
        struct Descriptor : public UniqueDeviceDescriptor
        {
        };

        using ControllerT       = HRTIMControllerDevice<tkControllerID>;
        using DependDevices     = TypeList<ControllerT>;
        using ControllerTraitsT = HRTIMControllerTraits<tkControllerID>;
        using TimerTraitsT      = HRTIMTimerTraits<tkControllerID, tkTimerID>;

        static constexpr HRTIMControllerID skControllerID = tkControllerID;
        static constexpr HRTIMTimerID      skTimerID      = tkTimerID;

    public:
        // ------------------------------------------------------------------------
        // Initialization
        // ------------------------------------------------------------------------

        static inline void init_output1_channel(const HRTIMOutputChannelConfig& config)
        {
            ControllerT::init_output_channel(TimerTraitsT::skOutput1, config);
        }

        static inline void init_output2_channel(const HRTIMOutputChannelConfig& config)
        {
            ControllerT::init_output_channel(TimerTraitsT::skOutput2, config);
        }

        static inline void init_deadtime(const HRTIMDeadTimeConfig& config)
        {
            // Validate deadtime against hardware limits
            // RM0440 Section 29.3.12: Max deadtime = 511 × tDTG where tDTG = tHRCK × 2^DTPRSC
            // At 170 MHz with max prescaler (7): 511 × (1/(8×170MHz)) × 128 ≈ 48 µs
            const uint32_t f_hrtim_hz = get_apb2_timer_clock_freq();
            const float    max_hw_deadtime_ns =
                511.0F * 128.0F / (8.0F * static_cast<float>(f_hrtim_hz)) * 1e9F;  // ~48,000 ns @ 170 MHz

            const float max_requested_ns = std::max<float>(config.fall_ns, config.rise_ns);

            // Validate requested deadtime
            if (max_requested_ns > max_hw_deadtime_ns)
            {
                // ERROR: Requested deadtime exceeds hardware capability
                VALLE_LOG_ERROR("HRTIM{} Timer {} requested deadtime ({} ns) exceeds hardware limit ({} ns)!",
                                static_cast<int>(tkControllerID),
                                enum_name(tkTimerID),
                                max_requested_ns,
                                max_hw_deadtime_ns);
            }

            uint32_t prescaler_val  = 0;
            uint32_t rising_counts  = 0;
            uint32_t falling_counts = 0;

            // Calculate "Base Counts"
            // This is the number of ticks required if Prescaler = 0 (Freq = 8 * f_HRTIM).
            // Use uint64_t to prevent overflow (Max ns * 170MHz * 8 can be large).
            uint64_t base_counts = (uint64_t)((max_requested_ns * 8.0F * static_cast<float>(f_hrtim_hz)) / 1e9F);

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
                rising_counts =
                    (uint32_t)((config.rise_ns * 8.0F * static_cast<float>(f_hrtim_hz) / 1e9F) / (float)(1 << p));
                falling_counts =
                    (uint32_t)((config.fall_ns * 8.0F * static_cast<float>(f_hrtim_hz) / 1e9F) / (float)(1 << p));

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

            LL_HRTIM_DT_SetPrescaler(ControllerTraitsT::skInstance, TimerTraitsT::skTimerIdx, prescaler_val);
            LL_HRTIM_DT_SetRisingValue(ControllerTraitsT::skInstance, TimerTraitsT::skTimerIdx, rising_counts);
            LL_HRTIM_DT_SetFallingValue(ControllerTraitsT::skInstance, TimerTraitsT::skTimerIdx, falling_counts);

            // RM0440 Section 29.3.12: Lock bits are write-once, cannot be cleared without MCU reset
            if (config.lock_registers)
            {
                LL_HRTIM_DT_LockFalling(ControllerTraitsT::skInstance, TimerTraitsT::skTimerIdx);
                LL_HRTIM_DT_LockRising(ControllerTraitsT::skInstance, TimerTraitsT::skTimerIdx);
            }

            LL_HRTIM_TIM_EnableDeadTime(ControllerTraitsT::skInstance, TimerTraitsT::skTimerIdx);
        }

        static inline void disable_deadtime()
        {
            LL_HRTIM_DT_SetRisingValue(ControllerTraitsT::skInstance, TimerTraitsT::skTimerIdx, 0);
            LL_HRTIM_DT_SetFallingValue(ControllerTraitsT::skInstance, TimerTraitsT::skTimerIdx, 0);
            LL_HRTIM_TIM_DisableDeadTime(ControllerTraitsT::skInstance, TimerTraitsT::skTimerIdx);
        }

        static inline void init_rep_int(const HRTIMRepIntConfig& config)
        {
            // Configure Repetition (For Control Loop Decimation)
            LL_HRTIM_TIM_SetRepetition(ControllerTraitsT::skInstance, TimerTraitsT::skTimerIdx, config.int_rep);

            // Enable Interrupts
            LL_HRTIM_EnableIT_REP(ControllerTraitsT::skInstance, TimerTraitsT::skTimerIdx);
            NVIC_EnableIRQ(TimerTraitsT::skIRQn);
            NVIC_SetPriority(TimerTraitsT::skIRQn, config.int_priority);
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
            const uint32_t prescaler =
                LL_HRTIM_TIM_GetPrescaler(ControllerTraitsT::skInstance, TimerTraitsT::skTimerIdx);
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
            const uint32_t rep = LL_HRTIM_TIM_GetRepetition(ControllerTraitsT::skInstance, TimerTraitsT::skTimerIdx);
            const uint32_t f_int_hz = timer_freq_hz / (rep + 1);
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
            // To implement this efficienctly, we can take advantage of the fact that floor(log2(floor(x))) =
            // floor(log2(x)) and then we can use __CLZ to compute floor(log2(x)) = 31 - __CLZ(x) for 32-bit integers.

            // Validate input to prevent divide-by-zero and __builtin_clz(0)
            // RM0440 Section 29.3.6: HRTIM frequency range is ~81 Hz to 5.3 MHz @ 170 MHz

            // Minimum: f_HRTIM / (32 × 65536) = 170 MHz / 2,097,152 ≈ 81 Hz
            // Maximum: f_HRTIM / 32 = 170 MHz / 32 ≈ 5.3 MHz

            constexpr uint32_t kMinPrescaler = 0;
            constexpr uint32_t kMaxPrescaler = 7;

            const uint32_t f_hrtim_hz = get_hrtim_freq_hz();
            const uint32_t kMinFreqHz = 81;
            const uint32_t kMaxFreqHz = f_hrtim_hz / 32;

            // Validate and clamp target frequency to valid range
            if (target_freq_hz < kMinFreqHz)
            {
                VALLE_LOG_WARN("HRTIM{} Timer {} target frequency {} Hz is too low, clamping to 81 Hz.",
                               static_cast<int>(tkControllerID),
                               enum_name(tkTimerID),
                               target_freq_hz);

                // Too low - use minimum (prescaler 7)
                return kMaxPrescaler;
            }

            if (target_freq_hz > kMaxFreqHz)
            {
                VALLE_LOG_WARN("HRTIM{} Timer {} target frequency {} Hz is too high, clamping to {} Hz.",
                               static_cast<int>(tkControllerID),
                               enum_name(tkTimerID),
                               target_freq_hz,
                               kMaxFreqHz);

                // Too high - use maximum (prescaler 0)
                return kMinPrescaler;
            }

            const uint32_t ratio = f_hrtim_hz / target_freq_hz;
            const int8_t   index = static_cast<int8_t>(31 - __builtin_clz(ratio) - 10);
            return std::clamp<int8_t>(index, kMinPrescaler, kMaxPrescaler);
        }

        [[nodiscard]] static inline uint32_t get_period_ticks()
        {
            return LL_HRTIM_TIM_GetPeriod(ControllerTraitsT::skInstance, TimerTraitsT::skTimerIdx);
        }
    };

    using HRTIM1TimerADevice = HRTIMTimerDevice<1, HRTIMTimerID::kA>;
    using HRTIM1TimerBDevice = HRTIMTimerDevice<1, HRTIMTimerID::kB>;
    using HRTIM1TimerCDevice = HRTIMTimerDevice<1, HRTIMTimerID::kC>;
    using HRTIM1TimerDDevice = HRTIMTimerDevice<1, HRTIMTimerID::kD>;
    using HRTIM1TimerEDevice = HRTIMTimerDevice<1, HRTIMTimerID::kE>;
    using HRTIM1TimerFDevice = HRTIMTimerDevice<1, HRTIMTimerID::kF>;

}  // namespace valle