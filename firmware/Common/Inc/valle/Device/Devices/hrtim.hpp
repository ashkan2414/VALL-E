#pragma once

#include <algorithm>
#include <cstdint>

#include "Valle/Core/Logging/logger.hpp"
#include "Valle/Core/timing.hpp"
#include "Valle/Device/Traits/hrtim.hpp"
#include "Valle/Device/device_core.hpp"
#include "Valle/Drivers/gpio.hpp"
#include "Valle/Utils/hal_utils.hpp"
#include "stm32g4xx_ll_bus.h"

namespace valle
{

    // =============================================================================
    // CONFIGURATIONS
    // =============================================================================
    struct HRTIMTimerCounterConfig
    {
        HRTIMTimerPrescaler    prescaler     = HRTIMTimerPrescaler::kDiv1;
        HRTIMTimerCounterMode  counter_mode  = HRTIMTimerCounterMode::kContinuous;
        HRTIMTimerCountingMode counting_mode = HRTIMTimerCountingMode::kUp;
        uint16_t               period        = 0xFFFF;
        uint8_t                repetition    = 0;
    };

    struct HRTIMVirtualOutputConfig
    {
        HRTIMOutputSetSource          set_source            = HRTIMOutputSetSource::kNone;
        HRTIMOutputResetSource        reset_source          = HRTIMOutputResetSource::kNone;
        HRTIMOutputPolarity           polarity              = HRTIMOutputPolarity::kPositive;
        HRTIMOutputIdleMode           idle_mode             = HRTIMOutputIdleMode::kNoIdle;
        HRTIMOutputIdleLevel          idle_level            = HRTIMOutputIdleLevel::kInactive;
        HRTIMOutputFaultState         fault_state           = HRTIMOutputFaultState::kInactive;
        HRTIMOutputChopperMode        chopper_mode          = HRTIMOutputChopperMode::kDisabled;
        HRTIMOutputBurstModeEntryMode burst_mode_entry_mode = HRTIMOutputBurstModeEntryMode::kRegular;
    };

    struct HRTIMOutputGPIOConfig
    {
        GPIOSpeedMode speed = GPIOSpeedMode::kHigh;
        GPIOPullMode  pull  = GPIOPullMode::kNoPull;
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

    struct HRTIMTimerInterruptMask
    {
        bool enable_compare1 : 1           = false;
        bool enable_compare2 : 1           = false;
        bool enable_compare3 : 1           = false;
        bool enable_compare4 : 1           = false;
        bool enable_update : 1             = false;
        bool enable_repetition : 1         = false;
        bool enable_capture1 : 1           = false;
        bool enable_capture2 : 1           = false;
        bool enable_output1_set : 1        = false;
        bool enable_output1_reset : 1      = false;
        bool enable_output2_set : 1        = false;
        bool enable_output2_reset : 1      = false;
        bool enable_reset : 1              = false;
        bool enable_delayed_protection : 1 = false;

        bool operator==(const HRTIMTimerInterruptMask&) const = default;
    };

    struct HRTIMTimerInterruptConfig
    {
        uint32_t                priority = 5;  // NVIC Priority (0-15)  // NOLINT(readability-magic-numbers)
        HRTIMTimerInterruptMask interrupts{};
    };

    struct HRTIMFaultConfig
    {
        HRTIMFaultSource                        source             = HRTIMFaultSource::kDigitalInput;
        HRTIMFaultPolarity                      polarity           = HRTIMFaultPolarity::kHigh;
        HRTIMFaultFilter                        filter             = HRTIMFaultFilter::kNone;
        HRTIMFaultPrescaler                     prescaler          = HRTIMFaultPrescaler::kDiv1;
        std::optional<HRTIMFaultBlankingSource> blanking_source    = std::nullopt;
        HRTIMFaultResetMode                     reset_mode         = HRTIMFaultResetMode::kUnconditional;
        uint32_t                                counting_threshold = 1;  // Fault counting threshold
        bool                                    lock               = true;
    };

    struct HRTIMEEVConfig
    {
        HRTIMEEVSource      source      = HRTIMEEVSource::kSource1;
        HRTIMEEVPolarity    polarity    = HRTIMEEVPolarity::kHigh;
        HRTIMEEVSensitivity sensitivity = HRTIMEEVSensitivity::kLevel;
        HRTIMEEVFastMode    fast_mode   = HRTIMEEVFastMode::kDisable;
        HRTIMEEVFilter      filter      = HRTIMEEVFilter::kNone;
        HRTIMEEVPrescaler   prescaler   = HRTIMEEVPrescaler::kDiv1;
    };

    // =============================================================================
    // COMPILE TIME CONFIGURATIONS
    // =============================================================================

    // -----------------------------------------------------------------------------
    // HRTIM CONTROLLER CT CONFIG
    // -----------------------------------------------------------------------------
    struct HRTIMControllerCTConfigDefaults
    {
        using SCInPinT  = GPIONullPinDevice;
        using SCOutPinT = GPIONullPinDevice;
    };

    template <HRTIMControllerID tkControllerID, HRTIMControllerGPIOPinType tkPinType>
    using HRTIMControllerDefaultPinDevice = HRTIMControllerPinMap<tkControllerID, tkPinType, 0>;

    template <HRTIMControllerID tkControllerID, HRTIMControllerGPIOPinType tkPinType, typename TPinDevice>
    concept CValidHRTIMControllerPinDevice =
        CGPIOPinDevice<TPinDevice> &&
        CValidHRTIMControllerPin<tkControllerID, tkPinType, TPinDevice::skPortID, TPinDevice::skPinID>;

    template <HRTIMControllerID tkControllerID, HRTIMControllerGPIOPinType tkPinType, typename TPinDevice>
    concept CValidHRTIMControllerPinCTConfig =
        CNullGPIOPinDevice<TPinDevice> || CValidHRTIMControllerPinDevice<tkControllerID, tkPinType, TPinDevice>;

    template <typename T, HRTIMControllerID tkControllerID>
    concept CValidHRTIMControllerCTConfig =
        requires {
            typename T::SCInPinT;
            typename T::SCOutPinT;
        } &&
        (CValidHRTIMControllerPinCTConfig<tkControllerID, HRTIMControllerGPIOPinType::kSCIn, typename T::SCInPinT>) &&
        (CValidHRTIMControllerPinCTConfig<tkControllerID, HRTIMControllerGPIOPinType::kSCOut, typename T::SCOutPinT>);

    template <HRTIMControllerID tkControllerID>
    struct HRTIMControllerCTConfigTraits
    {
        static constexpr auto skConfig = HRTIMControllerCTConfigDefaults{};
    };

#define VALLE_DEFINE_HRTIM_CONTROLLER_CT_CONFIG(tkControllerID, config)                        \
    namespace valle                                                                            \
    {                                                                                          \
        template <>                                                                            \
        struct HRTIMControllerCTConfigTraits<(tkControllerID)>                                 \
        {                                                                                      \
            static constexpr auto skConfig = (config);                                         \
            static_assert(CValidHRTIMControllerCTConfig<decltype(skConfig), (tkControllerID)>, \
                          "Invalid HRTIM Controller CT Config");                               \
        };                                                                                     \
    }

    // -----------------------------------------------------------------------------
    // HRTIM FAULT
    // -----------------------------------------------------------------------------
    struct HRTIMFaultCTConfigDefaults
    {
        using PinT = GPIONullPinDevice;
    };

    template <HRTIMControllerID tkControllerID, HRTIMFaultID tkFaultID, uint8_t tkOutputIdx = 0>
    using HRTIMFaultPinDeviceMap = GPIOPinDevice<HRTIMFaultPinMap<tkControllerID, tkFaultID, tkOutputIdx>::skPortID,
                                                 HRTIMFaultPinMap<tkControllerID, tkFaultID, tkOutputIdx>::skPinID>;

    template <HRTIMControllerID tkControllerID, HRTIMFaultID tkFaultID>
    using HRTIMFaultDefaultPinDevice = HRTIMFaultPinDeviceMap<tkControllerID, tkFaultID, 0>;

    template <HRTIMControllerID tkControllerID, HRTIMFaultID tkFaultID, typename TPinDevice>
    concept CValidHRTIMFaultPinDevice =
        CGPIOPinDevice<TPinDevice> &&
        CValidHRTIMFaultPin<tkControllerID, tkFaultID, TPinDevice::skPortID, TPinDevice::skPinID>;

    template <typename T, HRTIMControllerID tkControllerID, HRTIMFaultID tkFaultID>
    concept CValidHRTIMFaultCTConfig =
        requires { typename T::PinT; } && (CNullGPIOPinDevice<typename T::PinT> ||
                                           CValidHRTIMFaultPinDevice<tkControllerID, tkFaultID, typename T::PinT>);

    template <HRTIMControllerID tkControllerID, HRTIMFaultID tkFaultID>
    struct HRTIMFaultCTConfigTraits
    {
        static constexpr auto skConfig = HRTIMFaultCTConfigDefaults{};
    };

#define VALLE_DEFINE_HRTIM_FAULT_CT_CONFIG(tkControllerID, tkFaultID, config)                          \
    namespace valle                                                                                    \
    {                                                                                                  \
        template <>                                                                                    \
        struct HRTIMFaultCTConfigTraits<(tkControllerID), (tkFaultID)>                                 \
        {                                                                                              \
            static constexpr auto skConfig = (config);                                                 \
            static_assert(CValidHRTIMFaultCTConfig<decltype(skConfig), (tkControllerID), (tkFaultID)>, \
                          "Invalid HRTIM Fault CT Config");                                            \
        };                                                                                             \
    }

    // -----------------------------------------------------------------------------
    // HRTIM EEV
    // -----------------------------------------------------------------------------
    struct HRTIMEEVCTConfigDefaults
    {
        using PinT = GPIONullPinDevice;
    };

    template <HRTIMControllerID tkControllerID, HRTIMEEVID tkEEVID, uint8_t tkOutputIdx = 0>
    using HRTIMEEVPinDeviceMap = GPIOPinDevice<HRTIMEEVPinMap<tkControllerID, tkEEVID, tkOutputIdx>::skPortID,
                                               HRTIMEEVPinMap<tkControllerID, tkEEVID, tkOutputIdx>::skPinID>;

    template <HRTIMControllerID tkControllerID, HRTIMEEVID tkEEVID>
    using HRTIMEEVDefaultPinDevice = HRTIMEEVPinDeviceMap<tkControllerID, tkEEVID, 0>;

    template <HRTIMControllerID tkControllerID, HRTIMEEVID tkEEVID, typename TPinDevice>
    concept CValidHRTIMEEVPinDevice =
        CGPIOPinDevice<TPinDevice> &&
        CValidHRTIMEEVPin<tkControllerID, tkEEVID, TPinDevice::skPortID, TPinDevice::skPinID>;

    template <typename T, HRTIMControllerID tkControllerID, HRTIMEEVID tkEEVID>
    concept CValidHRTIMEEVCTConfig = requires {
        typename T::PinT;
    } && (CNullGPIOPinDevice<typename T::PinT> || CValidHRTIMEEVPinDevice<tkControllerID, tkEEVID, typename T::PinT>);

    template <HRTIMControllerID tkControllerID, HRTIMEEVID tkEEVID>
    struct HRTIMEEVCTConfigTraits
    {
        static constexpr auto skConfig = HRTIMEEVCTConfigDefaults{};
    };

#define VALLE_DEFINE_HRTIM_EEV_CT_CONFIG(tkControllerID, tkEEVID, config)                          \
    namespace valle                                                                                \
    {                                                                                              \
        template <>                                                                                \
        struct HRTIMEEVCTConfigTraits<(tkControllerID), (tkEEVID)>                                 \
        {                                                                                          \
            static constexpr auto skConfig = (config);                                             \
            static_assert(CValidHRTIMEEVCTConfig<decltype(skConfig), (tkControllerID), (tkEEVID)>, \
                          "Invalid HRTIM EEV CT Config");                                          \
        };                                                                                         \
    }

    // ---------------------------------------------------------------------------
    // HRTIM TIMER
    // ---------------------------------------------------------------------------

    struct HRTIMTimerCTConfigDefaults
    {
        using Output1PinT = GPIONullPinDevice;
        using Output2PinT = GPIONullPinDevice;

        constexpr static bool skUseFault1 = false;
        constexpr static bool skUseFault2 = false;
        constexpr static bool skUseFault3 = false;
        constexpr static bool skUseFault4 = false;
        constexpr static bool skUseFault5 = false;
        constexpr static bool skUseFault6 = false;

        constexpr static bool skUseEEV1  = false;
        constexpr static bool skUseEEV2  = false;
        constexpr static bool skUseEEV3  = false;
        constexpr static bool skUseEEV4  = false;
        constexpr static bool skUseEEV5  = false;
        constexpr static bool skUseEEV6  = false;
        constexpr static bool skUseEEV7  = false;
        constexpr static bool skUseEEV8  = false;
        constexpr static bool skUseEEV9  = false;
        constexpr static bool skUseEEV10 = false;
    };

    template <HRTIMControllerID     tkControllerID,
              HRTIMTimerID          tkTimerID,
              HRTIMTimerGPIOPinType tkPinType,
              uint8_t               tkOutputIdx = 0>
    using HRTIMTimerPinDeviceMap =
        GPIOPinDevice<HRTIMTimerPinMap<tkControllerID, tkTimerID, tkPinType, tkOutputIdx>::skPortID,
                      HRTIMTimerPinMap<tkControllerID, tkTimerID, tkPinType, tkOutputIdx>::skPinID>;

    template <HRTIMControllerID tkControllerID, HRTIMTimerID tkTimerID, HRTIMTimerGPIOPinType tkPinType>
    using HRTIMTimerDefaultPinDevice = HRTIMTimerPinDeviceMap<tkControllerID, tkTimerID, tkPinType, 0>;

    template <HRTIMControllerID tkControllerID, HRTIMTimerID tkTimerID>
    using HRTIMTimerDefaultOutput1PinDevice =
        HRTIMTimerDefaultPinDevice<tkControllerID, tkTimerID, HRTIMTimerGPIOPinType::kOutput1>;

    template <HRTIMControllerID tkControllerID, HRTIMTimerID tkTimerID>
    using HRTIMTimerDefaultOutput2PinDevice =
        HRTIMTimerDefaultPinDevice<tkControllerID, tkTimerID, HRTIMTimerGPIOPinType::kOutput2>;

    template <HRTIMControllerID     tkControllerID,
              HRTIMTimerID          tkTimerID,
              HRTIMTimerGPIOPinType tkPinType,
              typename TPinDevice>
    concept CValidHRTIMTimerPinDevice =
        CGPIOPinDevice<TPinDevice> &&
        CValidHRTIMTimerPin<tkControllerID, tkTimerID, tkPinType, TPinDevice::skPortID, TPinDevice::skPinID>;

    template <HRTIMControllerID     tkControllerID,
              HRTIMTimerID          tkTimerID,
              HRTIMTimerGPIOPinType tkPinType,
              typename TPinDevice>
    concept CValidHRTIMTimerPinCTConfig =
        (CNullGPIOPinDevice<TPinDevice> || CValidHRTIMTimerPinDevice<tkControllerID, tkTimerID, tkPinType, TPinDevice>);

    template <typename T, HRTIMControllerID tkControllerID, HRTIMTimerID tkTimerID>
    concept CValidHRTIMTimerCTConfig = requires { typename T::Output1PinT; } &&
                                       (CValidHRTIMTimerPinCTConfig<tkControllerID,
                                                                    tkTimerID,
                                                                    HRTIMTimerGPIOPinType::kOutput1,
                                                                    typename T::Output1PinT>) &&
                                       (CValidHRTIMTimerPinCTConfig<tkControllerID,
                                                                    tkTimerID,
                                                                    HRTIMTimerGPIOPinType::kOutput2,
                                                                    typename T::Output2PinT>) &&
                                       std::is_same_v<std::remove_cvref_t<decltype(T::skUseFault1)>, bool> &&
                                       std::is_same_v<std::remove_cvref_t<decltype(T::skUseFault2)>, bool> &&
                                       std::is_same_v<std::remove_cvref_t<decltype(T::skUseFault3)>, bool> &&
                                       std::is_same_v<std::remove_cvref_t<decltype(T::skUseFault4)>, bool> &&
                                       std::is_same_v<std::remove_cvref_t<decltype(T::skUseFault5)>, bool> &&
                                       std::is_same_v<std::remove_cvref_t<decltype(T::skUseFault6)>, bool> &&
                                       std::is_same_v<std::remove_cvref_t<decltype(T::skUseEEV1)>, bool> &&
                                       std::is_same_v<std::remove_cvref_t<decltype(T::skUseEEV2)>, bool> &&
                                       std::is_same_v<std::remove_cvref_t<decltype(T::skUseEEV3)>, bool> &&
                                       std::is_same_v<std::remove_cvref_t<decltype(T::skUseEEV4)>, bool> &&
                                       std::is_same_v<std::remove_cvref_t<decltype(T::skUseEEV5)>, bool> &&
                                       std::is_same_v<std::remove_cvref_t<decltype(T::skUseEEV6)>, bool> &&
                                       std::is_same_v<std::remove_cvref_t<decltype(T::skUseEEV7)>, bool> &&
                                       std::is_same_v<std::remove_cvref_t<decltype(T::skUseEEV8)>, bool> &&
                                       std::is_same_v<std::remove_cvref_t<decltype(T::skUseEEV9)>, bool> &&
                                       std::is_same_v<std::remove_cvref_t<decltype(T::skUseEEV10)>, bool>;

    template <HRTIMControllerID tkControllerID, HRTIMTimerID tkTimerID>
    struct HRTIMTimerCTConfigTraits
    {
        static constexpr auto skConfig = HRTIMTimerCTConfigDefaults{};
    };

#define VALLE_DEFINE_HRTIM_TIMER_CT_CONFIG(tkControllerID, tkTimerID, config)                          \
    namespace valle                                                                                    \
    {                                                                                                  \
        template <>                                                                                    \
        struct HRTIMTimerCTConfigTraits<(tkControllerID), (tkTimerID)>                                 \
        {                                                                                              \
            static constexpr auto skConfig = (config);                                                 \
            static_assert(CValidHRTIMTimerCTConfig<decltype(skConfig), (tkControllerID), (tkTimerID)>, \
                          "Invalid HRTIM Timer CT Config");                                            \
        };                                                                                             \
    }

    // ============================================================================
    // INTERRUPT TRAITS
    // ============================================================================

    // -----------------------------------------------------------------------------
    // HRTIM CONTROLLER INTERRUPTS
    // -----------------------------------------------------------------------------

    enum class HRTIMControllerInterruptType : uint8_t
    {
        kDLLReady,         // DLLRDY
        kBurstModePeriod,  // BMPER
        kSync,             // SYNC
    };

    template <HRTIMControllerID tkControllerID, HRTIMControllerInterruptType tkIntType>
    struct HRTIMControllerInterruptTraits;

#define DEFINE_HRTIM_INT_TRAIT(tkIntType, ll_name)                                                     \
    template <HRTIMControllerID tkControllerID>                                                        \
    struct HRTIMControllerInterruptTraits<tkControllerID, tkIntType>                                   \
    {                                                                                                  \
        static inline void enable()                                                                    \
        {                                                                                              \
            LL_HRTIM_EnableIT_##ll_name(HRTIMControllerTraits<tkControllerID>::skInstance);            \
        }                                                                                              \
        static inline void disable()                                                                   \
        {                                                                                              \
            LL_HRTIM_DisableIT_##ll_name(HRTIMControllerTraits<tkControllerID>::skInstance);           \
        }                                                                                              \
        static inline bool is_enabled()                                                                \
        {                                                                                              \
            return LL_HRTIM_IsEnabledIT_##ll_name(HRTIMControllerTraits<tkControllerID>::skInstance);  \
        }                                                                                              \
        static inline bool flag_active()                                                               \
        {                                                                                              \
            return LL_HRTIM_IsActiveFlag_##ll_name(HRTIMControllerTraits<tkControllerID>::skInstance); \
        }                                                                                              \
                                                                                                       \
        static inline bool is_pending()                                                                \
        {                                                                                              \
            return flag_active() && is_enabled();                                                      \
        }                                                                                              \
        static inline void ack()                                                                       \
        {                                                                                              \
            LL_HRTIM_ClearFlag_##ll_name(HRTIMControllerTraits<tkControllerID>::skInstance);           \
        }                                                                                              \
    };

    DEFINE_HRTIM_INT_TRAIT(HRTIMControllerInterruptType::kDLLReady, DLLRDY);
    DEFINE_HRTIM_INT_TRAIT(HRTIMControllerInterruptType::kBurstModePeriod, BMPER);
    DEFINE_HRTIM_INT_TRAIT(HRTIMControllerInterruptType::kSync, SYNC);

#undef DEFINE_HRTIM_INT_TRAIT

    // -----------------------------------------------------------------------------
    // HRTIM FAULT INTERRUPTS
    // -----------------------------------------------------------------------------
    enum class HRTIMFaultInterruptType : uint8_t
    {
        kSystemFault,  // SYSFLT
        kFault1,       // FLT1
        kFault2,       // FLT2
        kFault3,       // FLT3
        kFault4,       // FLT4
        kFault5,       // FLT5
        kFault6,       // FLT6
    };

    template <HRTIMControllerID tkControllerID, HRTIMFaultInterruptType tkIntType>
    struct HRTIMFaultInterruptTraits;

#define DEFINE_HRTIM_FAULT_INT_TRAIT(tkIntType, ll_name)                                               \
    template <HRTIMControllerID tkControllerID>                                                        \
    struct HRTIMFaultInterruptTraits<tkControllerID, tkIntType>                                        \
    {                                                                                                  \
        static inline void enable()                                                                    \
        {                                                                                              \
            LL_HRTIM_EnableIT_##ll_name(HRTIMControllerTraits<tkControllerID>::skInstance);            \
        }                                                                                              \
        static inline void disable()                                                                   \
        {                                                                                              \
            LL_HRTIM_DisableIT_##ll_name(HRTIMControllerTraits<tkControllerID>::skInstance);           \
        }                                                                                              \
        static inline bool is_enabled()                                                                \
        {                                                                                              \
            return LL_HRTIM_IsEnabledIT_##ll_name(HRTIMControllerTraits<tkControllerID>::skInstance);  \
        }                                                                                              \
        static inline bool flag_active()                                                               \
        {                                                                                              \
            return LL_HRTIM_IsActiveFlag_##ll_name(HRTIMControllerTraits<tkControllerID>::skInstance); \
        }                                                                                              \
        static inline bool is_pending()                                                                \
        {                                                                                              \
            return flag_active() && is_enabled();                                                      \
        }                                                                                              \
        static inline void ack()                                                                       \
        {                                                                                              \
            LL_HRTIM_ClearFlag_##ll_name(HRTIMControllerTraits<tkControllerID>::skInstance);           \
        }                                                                                              \
    };

    DEFINE_HRTIM_FAULT_INT_TRAIT(HRTIMFaultInterruptType::kSystemFault, SYSFLT);
    DEFINE_HRTIM_FAULT_INT_TRAIT(HRTIMFaultInterruptType::kFault1, FLT1);
    DEFINE_HRTIM_FAULT_INT_TRAIT(HRTIMFaultInterruptType::kFault2, FLT2);
    DEFINE_HRTIM_FAULT_INT_TRAIT(HRTIMFaultInterruptType::kFault3, FLT3);
    DEFINE_HRTIM_FAULT_INT_TRAIT(HRTIMFaultInterruptType::kFault4, FLT4);
    DEFINE_HRTIM_FAULT_INT_TRAIT(HRTIMFaultInterruptType::kFault5, FLT5);
    DEFINE_HRTIM_FAULT_INT_TRAIT(HRTIMFaultInterruptType::kFault6, FLT6);

#undef DEFINE_HRTIM_FAULT_INT_TRAIT

    // -----------------------------------------------------------------------------
    // HRTIM MASTER TIMER INTERRUPTS
    // -----------------------------------------------------------------------------

    enum class HRTIMMasterTimerInterruptType : uint8_t
    {
        kCompare1,    // CMP1
        kCompare2,    // CMP2
        kCompare3,    // CMP3
        kCompare4,    // CMP4
        kUpdate,      // UPDATE
        kRepetition,  // REP
    };

    template <HRTIMControllerID tkControllerID, HRTIMMasterTimerInterruptType tkIntType>
    struct HRTIMMasterTimerInterruptTraits;

#define DEFINE_HRTIM_INT_TRAIT(tkIntType, ll_name)                                                                  \
    template <HRTIMControllerID tkControllerID>                                                                     \
    struct HRTIMMasterTimerInterruptTraits<tkControllerID, tkIntType>                                               \
    {                                                                                                               \
        static inline void enable()                                                                                 \
        {                                                                                                           \
            LL_HRTIM_EnableIT_##ll_name(HRTIMControllerTraits<tkControllerID>::skInstance, LL_HRTIM_TIMER_MASTER);  \
        }                                                                                                           \
        static inline void disable()                                                                                \
        {                                                                                                           \
            LL_HRTIM_DisableIT_##ll_name(HRTIMControllerTraits<tkControllerID>::skInstance, LL_HRTIM_TIMER_MASTER); \
        }                                                                                                           \
        static inline bool is_enabled()                                                                             \
        {                                                                                                           \
            return LL_HRTIM_IsEnabledIT_##ll_name(HRTIMControllerTraits<tkControllerID>::skInstance,                \
                                                  LL_HRTIM_TIMER_MASTER);                                           \
        }                                                                                                           \
        static inline bool flag_active()                                                                            \
        {                                                                                                           \
            return LL_HRTIM_IsActiveFlag_##ll_name(HRTIMControllerTraits<tkControllerID>::skInstance,               \
                                                   LL_HRTIM_TIMER_MASTER);                                          \
        }                                                                                                           \
                                                                                                                    \
        static inline bool is_pending()                                                                             \
        {                                                                                                           \
            return flag_active() && is_enabled();                                                                   \
        }                                                                                                           \
        static inline void ack()                                                                                    \
        {                                                                                                           \
            LL_HRTIM_ClearFlag_##ll_name(HRTIMControllerTraits<tkControllerID>::skInstance, LL_HRTIM_TIMER_MASTER); \
        }                                                                                                           \
    };

    DEFINE_HRTIM_INT_TRAIT(HRTIMMasterTimerInterruptType::kCompare1, CMP1);
    DEFINE_HRTIM_INT_TRAIT(HRTIMMasterTimerInterruptType::kCompare2, CMP2);
    DEFINE_HRTIM_INT_TRAIT(HRTIMMasterTimerInterruptType::kCompare3, CMP3);
    DEFINE_HRTIM_INT_TRAIT(HRTIMMasterTimerInterruptType::kCompare4, CMP4);
    DEFINE_HRTIM_INT_TRAIT(HRTIMMasterTimerInterruptType::kUpdate, UPDATE);
    DEFINE_HRTIM_INT_TRAIT(HRTIMMasterTimerInterruptType::kRepetition, REP);

#undef DEFINE_HRTIM_INT_TRAIT

    // -----------------------------------------------------------------------------
    // HRTIM TIMER INTERRUPTS
    // -----------------------------------------------------------------------------

    enum class HRTIMTimerInterruptType : uint8_t
    {
        kCompare1,           // CMP1
        kCompare2,           // CMP2
        kCompare3,           // CMP3
        kCompare4,           // CMP4
        kUpdate,             // UPDATE
        kRepetition,         // REP
        kCapture1,           // CPT1
        kCapture2,           // CPT2
        kOutput1Set,         // SET1
        kOutput1Reset,       // RST1
        kOutput2Set,         // SET2
        kOutput2Reset,       // RST2
        kReset,              // RST
        kDelayedProtection,  // DLYPRT
    };

    template <HRTIMControllerID tkControllerID, HRTIMTimerID tkTimerID, HRTIMTimerInterruptType tkIntType>
    struct HRTIMTimerInterruptTraits;

#define DEFINE_HRTIM_INT_TRAIT(tkIntType, ll_name)                                                           \
    template <HRTIMControllerID tkControllerID, HRTIMTimerID tkTimerID>                                      \
    struct HRTIMTimerInterruptTraits<tkControllerID, tkTimerID, tkIntType>                                   \
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

    DEFINE_HRTIM_INT_TRAIT(HRTIMTimerInterruptType::kCompare1, CMP1);
    DEFINE_HRTIM_INT_TRAIT(HRTIMTimerInterruptType::kCompare2, CMP2);
    DEFINE_HRTIM_INT_TRAIT(HRTIMTimerInterruptType::kCompare3, CMP3);
    DEFINE_HRTIM_INT_TRAIT(HRTIMTimerInterruptType::kCompare4, CMP4);
    DEFINE_HRTIM_INT_TRAIT(HRTIMTimerInterruptType::kUpdate, UPDATE);
    DEFINE_HRTIM_INT_TRAIT(HRTIMTimerInterruptType::kRepetition, REP);
    DEFINE_HRTIM_INT_TRAIT(HRTIMTimerInterruptType::kCapture1, CPT1);
    DEFINE_HRTIM_INT_TRAIT(HRTIMTimerInterruptType::kCapture2, CPT2);
    DEFINE_HRTIM_INT_TRAIT(HRTIMTimerInterruptType::kOutput1Set, SET1);
    DEFINE_HRTIM_INT_TRAIT(HRTIMTimerInterruptType::kOutput1Reset, RST1);
    DEFINE_HRTIM_INT_TRAIT(HRTIMTimerInterruptType::kOutput2Set, SET2);
    DEFINE_HRTIM_INT_TRAIT(HRTIMTimerInterruptType::kOutput2Reset, RST2);
    DEFINE_HRTIM_INT_TRAIT(HRTIMTimerInterruptType::kReset, RST);
    DEFINE_HRTIM_INT_TRAIT(HRTIMTimerInterruptType::kDelayedProtection, DLYPRT);

#undef DEFINE_HRTIM_INT_TRAIT
    // -----------------------------------------------------------------------------
    // GLOBAL CONTROLLER AND MASTER TIMER ISR ROUTER
    // -----------------------------------------------------------------------------
    /**
     * @brief Global Controller and Master Timer ISR Router for a specific HRTIM.
     * Specializing this allows you to handle the entire ISR in one function
     * (e.g., when delegating to the ST HAL). Binding this takes precedence over
     * all controller and master timer ISR routers.
     *
     * @tparam tkControllerID HRTIM Controller ID.
     */
    template <HRTIMControllerID tkControllerID>
        requires(kValidHRTIMID<tkControllerID>)
    struct HRTIMGlobalMasterISRRouter
    {
        using UnboundIsrHandlerTag = void;
        static void handle()
        {
            // Default: Do nothing (Optimized away)
        }
    };

    // -----------------------------------------------------------------------------
    // GLOBAL CONTROLLER ISR ROUTER
    // -----------------------------------------------------------------------------
    /**
     * @brief Global Controller ISR Router for a specific HRTIM.
     * Specializing this allows you to handle the entire ISR in one function
     * (e.g., when delegating to the ST HAL).
     *
     * @tparam tkControllerID HRTIM Controller ID.
     */
    template <HRTIMControllerID tkControllerID>
        requires(kValidHRTIMID<tkControllerID>)
    struct HRTIMGlobalControllerISRRouter
    {
        using UnboundIsrHandlerTag = void;
        static void handle()
        {
            // Default: Do nothing (Optimized away)
        }
    };

    // -----------------------------------------------------------------------------
    // GRANULAR CONTROLLER ISR ROUTER
    // -----------------------------------------------------------------------------
    /**
     * @brief Controller ISR Router for a specific HRTIM.
     * Specializing this allows you to handle the entire ISR in one function
     * (e.g., when delegating to the ST HAL).
     *
     * @tparam tkControllerID HRTIM Controller ID.
     */
    template <HRTIMControllerID tkControllerID, HRTIMControllerInterruptType tkIntType>
        requires(kValidHRTIMID<tkControllerID>)
    struct HRTIMControllerISRRouter
    {
        using UnboundIsrHandlerTag = void;
        static void handle()
        {
            // Default: Do nothing (Optimized away)
        }
    };

    // -----------------------------------------------------------------------------
    // GLOBAL FAULT ISR ROUTER
    // -----------------------------------------------------------------------------
    /**
     * @brief Global Fault ISR Router for a specific HRTIM.
     * Specializing this allows you to handle the entire ISR in one function
     * (e.g., when delegating to the ST HAL).
     *
     * @tparam tkControllerID HRTIM Controller ID.
     */
    template <HRTIMControllerID tkControllerID>
        requires(kValidHRTIMID<tkControllerID>)
    struct HRTIMGlobalFaultISRRouter
    {
        using UnboundIsrHandlerTag = void;
        static void handle()
        {
            // Default: Do nothing (Optimized away)
        }
    };

    // -----------------------------------------------------------------------------
    // GRANULAR FAULT ISR ROUTER
    // -----------------------------------------------------------------------------
    /**
     * @brief Fault ISR Router for a specific HRTIM.
     * Specializing this allows you to handle the entire ISR in one function
     * (e.g., when delegating to the ST HAL).
     *
     * @tparam tkControllerID HRTIM Controller ID.
     */
    template <HRTIMControllerID tkControllerID, HRTIMFaultInterruptType tkIntType>
        requires(kValidHRTIMID<tkControllerID>)
    struct HRTIMFaultISRRouter
    {
        using UnboundIsrHandlerTag = void;
        static void handle()
        {
            // Default: Do nothing (Optimized away)
        }
    };

    // -----------------------------------------------------------------------------
    // GLOBAL MASTER TIMER ISR ROUTER
    // -----------------------------------------------------------------------------
    /**
     * @brief Global Master Timer ISR Router for a specific HRTIM.
     * Specializing this allows you to handle the entire ISR in one function
     * (e.g., when delegating to the ST HAL).
     *
     * @tparam tkControllerID HRTIM Controller ID.
     */
    template <HRTIMControllerID tkControllerID>
        requires(kValidHRTIMID<tkControllerID>)
    struct HRTIMGlobalMasterTimerISRRouter
    {
        using UnboundIsrHandlerTag = void;
        static void handle()
        {
            // Default: Do nothing (Optimized away)
        }
    };

    // -----------------------------------------------------------------------------
    // GRANULAR MASTER TIMER ISR ROUTER
    // -----------------------------------------------------------------------------
    /**
     * @brief Master Timer ISR Router for a specific HRTIM.
     * Specializing this allows you to handle the entire ISR in one function
     * (e.g., when delegating to the ST HAL).
     *
     * @tparam tkControllerID HRTIM Controller ID.
     */
    template <HRTIMControllerID tkControllerID, HRTIMMasterTimerInterruptType tkIntType>
        requires(kValidHRTIMID<tkControllerID>)
    struct HRTIMMasterTimerISRRouter
    {
        using UnboundIsrHandlerTag = void;
        static void handle()
        {
            // Default: Do nothing (Optimized away)
        }
    };

    // -----------------------------------------------------------------------------
    // GLOBAL TIMER ISR ROUTER
    // -----------------------------------------------------------------------------
    /**
     * @brief Global Timer ISR Router for a specific HRTIM.
     * Specializing this allows you to handle the entire ISR in one function
     * (e.g., when delegating to the ST HAL).
     *
     * @tparam tkControllerID HRTIM Controller ID.
     * @tparam tkTimerID HRTIM Timer ID.
     */
    template <HRTIMControllerID tkControllerID, HRTIMTimerID tkTimerID>
        requires(kValidHRTIMID<tkControllerID>)
    struct HRTIMGlobalTimerISRRouter
    {
        using UnboundIsrHandlerTag = void;
        static void handle()
        {
            // Default: Do nothing (Optimized away)
        }
    };

    // -----------------------------------------------------------------------------
    // GRANULAR TIMER ISR ROUTER
    // -----------------------------------------------------------------------------

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
    template <HRTIMControllerID tkControllerID, HRTIMTimerID tkTimerID, HRTIMTimerInterruptType tkIntType>
        requires(kValidHRTIMID<tkControllerID>)
    struct HRTIMTimerISRRouter
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

    template <HRTIMControllerID tkControllerID, HRTIMFaultID tkFaultID>
        requires(kValidHRTIMID<tkControllerID>)
    class HRTIMFaultDevice;

    template <HRTIMControllerID tkControllerID, HRTIMEEVID tkEEVID>
        requires(kValidHRTIMID<tkControllerID>)
    class HRTIMEEVDevice;

    template <HRTIMControllerID tkControllerID, HRTIMTimerID tkTimerID>
        requires(kValidHRTIMID<tkControllerID>)
    class HRTIMTimerDevice;

    // =============================================================================
    // HRTIM ROOT DEVICE (INTERFACE DEVICE)
    // =============================================================================

    class HRTIMRootDevice
    {
    public:
        struct Descriptor : public InterfaceDeviceDescriptor
        {
            using Children = DeviceTreeList<HRTIMControllerDevice<1>>;
        };
    };

    // =============================================================================
    // HRTIM CONTROLLER DEVICE (SHARED DEVICE)
    // =============================================================================

    namespace detail
    {
        template <HRTIMControllerID tkControllerID, HRTIMControllerGPIOPinType tkPinType, typename TPinDevice>
        struct HRTIMControllerGPIOPinDriverHelper
        {
            using type = GPIOAlternativeFunctionDriver<
                TPinDevice,
                kHRTIMControllerPinAF<tkControllerID, tkPinType, TPinDevice::skPortID, TPinDevice::skPinID>>;
        };

        template <HRTIMControllerID tkControllerID, HRTIMControllerGPIOPinType tkPinType>
        struct HRTIMControllerGPIOPinDriverHelper<tkControllerID, tkPinType, GPIONullPinDevice>
        {
            using type = std::monostate;
        };

    }  // namespace detail

    template <HRTIMControllerID tkControllerID, HRTIMControllerGPIOPinType tkPinType, typename TPinDevice>
    using HRTIMControllerGPIOPinDriver =
        typename detail::HRTIMControllerGPIOPinDriverHelper<tkControllerID, tkPinType, TPinDevice>::type;

    template <HRTIMControllerID tkControllerID>
        requires(kValidHRTIMID<tkControllerID>)
    class HRTIMControllerDevice
    {
    public:
        struct Descriptor : public SharedDeviceDescriptor
        {
            using Children = DeviceTreeList<HRTIMFaultDevice<tkControllerID, HRTIMFaultID::kFault1>,
                                            HRTIMFaultDevice<tkControllerID, HRTIMFaultID::kFault2>,
                                            HRTIMFaultDevice<tkControllerID, HRTIMFaultID::kFault3>,
                                            HRTIMFaultDevice<tkControllerID, HRTIMFaultID::kFault4>,
                                            HRTIMFaultDevice<tkControllerID, HRTIMFaultID::kFault5>,
                                            HRTIMFaultDevice<tkControllerID, HRTIMFaultID::kFault6>,
                                            HRTIMEEVDevice<tkControllerID, HRTIMEEVID::kEEV1>,
                                            HRTIMEEVDevice<tkControllerID, HRTIMEEVID::kEEV2>,
                                            HRTIMEEVDevice<tkControllerID, HRTIMEEVID::kEEV3>,
                                            HRTIMEEVDevice<tkControllerID, HRTIMEEVID::kEEV4>,
                                            HRTIMEEVDevice<tkControllerID, HRTIMEEVID::kEEV5>,
                                            HRTIMEEVDevice<tkControllerID, HRTIMEEVID::kEEV6>,
                                            HRTIMEEVDevice<tkControllerID, HRTIMEEVID::kEEV7>,
                                            HRTIMEEVDevice<tkControllerID, HRTIMEEVID::kEEV8>,
                                            HRTIMEEVDevice<tkControllerID, HRTIMEEVID::kEEV9>,
                                            HRTIMEEVDevice<tkControllerID, HRTIMEEVID::kEEV10>,
                                            HRTIMTimerDevice<tkControllerID, HRTIMTimerID::kA>,
                                            HRTIMTimerDevice<tkControllerID, HRTIMTimerID::kB>,
                                            HRTIMTimerDevice<tkControllerID, HRTIMTimerID::kC>,
                                            HRTIMTimerDevice<tkControllerID, HRTIMTimerID::kD>,
                                            HRTIMTimerDevice<tkControllerID, HRTIMTimerID::kE>,
                                            HRTIMTimerDevice<tkControllerID, HRTIMTimerID::kF>>;
        };
        static constexpr HRTIMControllerID skControllerID = tkControllerID;

        using ControllerTraitsT          = HRTIMControllerTraits<tkControllerID>;
        using CTConfigTraitsT            = HRTIMControllerCTConfigTraits<tkControllerID>;
        static constexpr auto skCTConfig = CTConfigTraitsT::skConfig;
        using CTConfigT                  = decltype(skCTConfig);

        using SCInPinT  = typename CTConfigT::SCInPinT;
        using SCOutPinT = typename CTConfigT::SCOutPinT;

        static constexpr bool skHasSCInPin  = !CNullGPIOPinDevice<SCInPinT>;
        static constexpr bool skHasSCOutPin = !CNullGPIOPinDevice<SCOutPinT>;

        using SCInPinDriverT =
            HRTIMControllerGPIOPinDriver<tkControllerID, HRTIMControllerGPIOPinType::kSCIn, SCInPinT>;
        using SCOutPinDriverT =
            HRTIMControllerGPIOPinDriver<tkControllerID, HRTIMControllerGPIOPinType::kSCOut, SCOutPinT>;

        using DependDevices = TypeList<HRTIMRootDevice>;
        using InjectDevices = FilterNullDevices<TypeList<SCInPinT, SCOutPinT>>;

    private:
        SCInPinDriverT  m_scin_pin;
        SCOutPinDriverT m_scout_pin;

    public:
        template <typename... TArgs>
        explicit HRTIMControllerDevice(TArgs&&... args)
            : m_scin_pin(extract_device_ref<skHasSCInPin, SCInPinT>(std::forward<TArgs>(args)...))
            , m_scout_pin(extract_device_ref<skHasSCOutPin, SCOutPinT>(std::forward<TArgs>(args)...))
        {
        }

        [[nodiscard]] inline bool init()
        {
            // Enable Bus Clock
            ControllerTraitsT::enable_clock();

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

            return true;
        }

        [[nodiscard]] inline bool post_init()
        {
            // Do nothing for now
            return true;
        }
    };

    using HRTIM1ControllerDevice = HRTIMControllerDevice<1>;

    // =============================================================================
    // HRTIM FAULT DEVICE (SHARED DEVICE)
    // =============================================================================
    namespace detail
    {
        template <HRTIMControllerID tkControllerID, HRTIMFaultID tkFaultID, typename TPinDevice>
        struct HRTIMFaultGPIOPinDriverHelper
        {
            using type = GPIOAlternativeFunctionDriver<
                TPinDevice,
                kHRTIMFaultPinAF<tkControllerID, tkFaultID, TPinDevice::skPortID, TPinDevice::skPinID>>;
        };

        template <HRTIMControllerID tkControllerID, HRTIMFaultID tkFaultID>
        struct HRTIMFaultGPIOPinDriverHelper<tkControllerID, tkFaultID, GPIONullPinDevice>
        {
            using type = std::monostate;
        };

    }  // namespace detail

    template <HRTIMControllerID tkControllerID, HRTIMFaultID tkFaultID, typename TPinDevice>
    using HRTIMFaultGPIOPinDriver =
        typename detail::HRTIMFaultGPIOPinDriverHelper<tkControllerID, tkFaultID, TPinDevice>::type;

    template <HRTIMControllerID tkControllerID, HRTIMFaultID tkFaultID>
        requires(kValidHRTIMID<tkControllerID>)
    class HRTIMFaultDevice
    {
    public:
        struct Descriptor : public SharedDeviceDescriptor
        {
        };

        static constexpr HRTIMControllerID skControllerID = tkControllerID;
        static constexpr HRTIMFaultID      skFaultID      = tkFaultID;

        using ControllerT       = HRTIMControllerDevice<tkControllerID>;
        using ControllerTraitsT = HRTIMControllerTraits<tkControllerID>;
        using FaultTraitsT      = HRTIMFaultTraits<tkControllerID, tkFaultID>;

        using CTConfigTraitsT            = HRTIMFaultCTConfigTraits<tkControllerID, tkFaultID>;
        static constexpr auto skCTConfig = CTConfigTraitsT::skConfig;
        using CTConfigT                  = decltype(skCTConfig);

        using PinT       = typename CTConfigT::PinT;
        using PinDriverT = HRTIMFaultGPIOPinDriver<tkControllerID, tkFaultID, PinT>;

        using InjectDevices = FilterNullDevices<TypeList<ControllerT, PinT>>;

    private:
        [[no_unique_address]] DeviceRef<ControllerT> m_controller;
        PinDriverT                                   m_pin;

    public:
        template <typename... TArgs>
        explicit HRTIMFaultDevice(TArgs&&... args)
            : m_controller(extract_device_ref<true, ControllerT>(std::forward<TArgs>(args)...))
            , m_pin(extract_device_ref<!CNullGPIOPinDevice<PinT>, PinT>(std::forward<TArgs>(args)...))
        {
        }

        // ------------------------------------------------------------------------
        // Initialization
        // ------------------------------------------------------------------------
        [[nodiscard]] inline bool init(const HRTIMFaultConfig& config)
        {
            LL_HRTIM_FLT_SetSrc(
                ControllerTraitsT::skInstance, static_cast<uint32_t>(tkFaultID), static_cast<uint32_t>(config.source));

            LL_HRTIM_FLT_SetPolarity(ControllerTraitsT::skInstance,
                                     static_cast<uint32_t>(tkFaultID),
                                     static_cast<uint32_t>(config.polarity));

            LL_HRTIM_FLT_SetFilter(
                ControllerTraitsT::skInstance, static_cast<uint32_t>(tkFaultID), static_cast<uint32_t>(config.filter));

            LL_HRTIM_FLT_SetPrescaler(ControllerTraitsT::skInstance,
                                      static_cast<uint32_t>(tkFaultID),
                                      static_cast<uint32_t>(config.prescaler));

            if (config.blanking_source.has_value())
            {
                LL_HRTIM_FLT_EnableBlanking(ControllerTraitsT::skInstance, static_cast<uint32_t>(tkFaultID));
                LL_HRTIM_FLT_SetBlankingSrc(ControllerTraitsT::skInstance,
                                            static_cast<uint32_t>(tkFaultID),
                                            static_cast<uint32_t>(config.blanking_source.value()));
            }
            else
            {
                LL_HRTIM_FLT_DisableBlanking(ControllerTraitsT::skInstance, static_cast<uint32_t>(tkFaultID));
            }

            LL_HRTIM_FLT_SetResetMode(ControllerTraitsT::skInstance,
                                      static_cast<uint32_t>(tkFaultID),
                                      static_cast<uint32_t>(config.reset_mode));
            LL_HRTIM_FLT_SetCountingThreshold(ControllerTraitsT::skInstance,
                                              static_cast<uint32_t>(tkFaultID),
                                              static_cast<uint32_t>(config.counting_threshold));

            if (config.lock)
            {
                LL_HRTIM_FLT_Lock(ControllerTraitsT::skInstance, static_cast<uint32_t>(tkFaultID));
            }

            return true;
        }

        [[nodiscard]] inline bool post_init()
        {
            // Do nothing for now
            return true;
        }
    };

    using HRTIM1Fault1Device = HRTIMFaultDevice<1, HRTIMFaultID::kFault1>;
    using HRTIM1Fault2Device = HRTIMFaultDevice<1, HRTIMFaultID::kFault2>;
    using HRTIM1Fault3Device = HRTIMFaultDevice<1, HRTIMFaultID::kFault3>;
    using HRTIM1Fault4Device = HRTIMFaultDevice<1, HRTIMFaultID::kFault4>;
    using HRTIM1Fault5Device = HRTIMFaultDevice<1, HRTIMFaultID::kFault5>;
    using HRTIM1Fault6Device = HRTIMFaultDevice<1, HRTIMFaultID::kFault6>;

    class HRTIMNullFaultDevice
    {
    public:
        struct Descriptor : public NullDeviceDescriptor
        {
        };
    };

    // =============================================================================
    // HRTIM FAULT DEVICE (SHARED DEVICE)
    // =============================================================================
    namespace detail
    {
        template <HRTIMControllerID tkControllerID, HRTIMEEVID tkEEVID, typename TPinDevice>
        struct HRTIMEEVGPIOPinDriverHelper
        {
            using type = GPIOAlternativeFunctionDriver<
                TPinDevice,
                kHRTIMEEVPinAF<tkControllerID, tkEEVID, TPinDevice::skPortID, TPinDevice::skPinID>>;
        };

        template <HRTIMControllerID tkControllerID, HRTIMEEVID tkEEVID>
        struct HRTIMEEVGPIOPinDriverHelper<tkControllerID, tkEEVID, GPIONullPinDevice>
        {
            using type = std::monostate;
        };

    }  // namespace detail

    template <HRTIMControllerID tkControllerID, HRTIMEEVID tkEEVID, typename TPinDevice>
    using HRTIMEEVGPIOPinDriver =
        typename detail::HRTIMEEVGPIOPinDriverHelper<tkControllerID, tkEEVID, TPinDevice>::type;

    template <HRTIMControllerID tkControllerID, HRTIMEEVID tkEEVID>
        requires(kValidHRTIMID<tkControllerID>)
    class HRTIMEEVDevice
    {
    public:
        struct Descriptor : public SharedDeviceDescriptor
        {
        };

        static constexpr HRTIMControllerID skControllerID = tkControllerID;
        static constexpr HRTIMEEVID        skEEVID        = tkEEVID;

        using ControllerT       = HRTIMControllerDevice<tkControllerID>;
        using ControllerTraitsT = HRTIMControllerTraits<tkControllerID>;
        using EEVTraitsT        = HRTIMEEVTraits<tkControllerID, tkEEVID>;

        using CTConfigTraitsT            = HRTIMEEVCTConfigTraits<tkControllerID, tkEEVID>;
        static constexpr auto skCTConfig = CTConfigTraitsT::skConfig;
        using CTConfigT                  = decltype(skCTConfig);

        using PinT       = typename CTConfigT::PinT;
        using PinDriverT = HRTIMEEVGPIOPinDriver<tkControllerID, tkEEVID, PinT>;

        using InjectDevices = FilterNullDevices<TypeList<ControllerT, PinT>>;

    private:
        [[no_unique_address]] DeviceRef<ControllerT> m_controller;
        PinDriverT                                   m_pin;

    public:
        template <typename... TArgs>
        explicit HRTIMEEVDevice(TArgs&&... args)
            : m_controller(extract_device_ref<true, ControllerT>(std::forward<TArgs>(args)...))
            , m_pin(extract_device_ref<!CNullGPIOPinDevice<PinT>, PinT>(std::forward<TArgs>(args)...))
        {
        }

        // ------------------------------------------------------------------------
        // Initialization
        // ------------------------------------------------------------------------
        [[nodiscard]] inline bool init(const HRTIMEEVConfig& config)
        {
            LL_HRTIM_EE_SetSrc(
                ControllerTraitsT::skInstance, static_cast<uint32_t>(tkEEVID), static_cast<uint32_t>(config.source));

            LL_HRTIM_EE_SetPolarity(
                ControllerTraitsT::skInstance, static_cast<uint32_t>(tkEEVID), static_cast<uint32_t>(config.polarity));

            LL_HRTIM_EE_SetSensitivity(ControllerTraitsT::skInstance,
                                       static_cast<uint32_t>(tkEEVID),
                                       static_cast<uint32_t>(config.sensitivity));

            LL_HRTIM_EE_SetFastMode(
                ControllerTraitsT::skInstance, static_cast<uint32_t>(tkEEVID), static_cast<uint32_t>(config.fast_mode));

            LL_HRTIM_EE_SetFilter(
                ControllerTraitsT::skInstance, static_cast<uint32_t>(tkEEVID), static_cast<uint32_t>(config.filter));

            LL_HRTIM_EE_SetPrescaler(
                ControllerTraitsT::skInstance, static_cast<uint32_t>(tkEEVID), static_cast<uint32_t>(config.prescaler));

            return true;
        }

        [[nodiscard]] inline bool post_init()
        {
            // Do nothing for now
            return true;
        }
    };

    using HRTIM1EEV1Device  = HRTIMEEVDevice<1, HRTIMEEVID::kEEV1>;
    using HRTIM1EEV2Device  = HRTIMEEVDevice<1, HRTIMEEVID::kEEV2>;
    using HRTIM1EEV3Device  = HRTIMEEVDevice<1, HRTIMEEVID::kEEV3>;
    using HRTIM1EEV4Device  = HRTIMEEVDevice<1, HRTIMEEVID::kEEV4>;
    using HRTIM1EEV5Device  = HRTIMEEVDevice<1, HRTIMEEVID::kEEV5>;
    using HRTIM1EEV6Device  = HRTIMEEVDevice<1, HRTIMEEVID::kEEV6>;
    using HRTIM1EEV7Device  = HRTIMEEVDevice<1, HRTIMEEVID::kEEV7>;
    using HRTIM1EEV8Device  = HRTIMEEVDevice<1, HRTIMEEVID::kEEV8>;
    using HRTIM1EEV9Device  = HRTIMEEVDevice<1, HRTIMEEVID::kEEV9>;
    using HRTIM1EEV10Device = HRTIMEEVDevice<1, HRTIMEEVID::kEEV10>;

    class HRTIMNullEEVDevice
    {
    public:
        struct Descriptor : public NullDeviceDescriptor
        {
        };
    };

    // =============================================================================
    // HRTIM TIMER DEVICE (UNIQUE DEVICE)
    // =============================================================================

    namespace detail
    {
        template <HRTIMControllerID     tkControllerID,
                  HRTIMTimerID          tkTimerID,
                  HRTIMTimerGPIOPinType tkPinType,
                  typename TPinDevice>
        struct HRTIMTimerGPIOPinDriverHelper
        {
            constexpr static auto skAF =
                kHRTIMTimerPinAF<tkControllerID, tkTimerID, tkPinType, TPinDevice::skPortID, TPinDevice::skPinID>;
            using type = GPIOAlternativeFunctionDriver<TPinDevice, skAF>;
        };

        template <HRTIMControllerID tkControllerID, HRTIMTimerID tkTimerID, HRTIMTimerGPIOPinType tkPinType>
        struct HRTIMTimerGPIOPinDriverHelper<tkControllerID, tkTimerID, tkPinType, GPIONullPinDevice>
        {
            using type = std::monostate;
        };

    }  // namespace detail

    template <HRTIMControllerID     tkControllerID,
              HRTIMTimerID          tkTimerID,
              HRTIMTimerGPIOPinType tkPinType,
              typename TPinDevice>
    using HRTIMTimerGPIOPinDriver =
        typename detail::HRTIMTimerGPIOPinDriverHelper<tkControllerID, tkTimerID, tkPinType, TPinDevice>::type;

    template <HRTIMControllerID tkControllerID, HRTIMTimerID tkTimerID>
        requires(kValidHRTIMID<tkControllerID>)
    class HRTIMTimerDevice
    {
    public:
        struct Descriptor : public UniqueDeviceDescriptor
        {
        };

        static constexpr HRTIMControllerID skControllerID = tkControllerID;
        static constexpr HRTIMTimerID      skTimerID      = tkTimerID;

        using ControllerT       = HRTIMControllerDevice<tkControllerID>;
        using ControllerTraitsT = HRTIMControllerTraits<tkControllerID>;
        using TimerTraitsT      = HRTIMTimerTraits<tkControllerID, tkTimerID>;

        using CTConfigTraitsT            = HRTIMTimerCTConfigTraits<tkControllerID, tkTimerID>;
        static constexpr auto skCTConfig = CTConfigTraitsT::skConfig;
        using CTConfigT                  = decltype(skCTConfig);

        using Output1PinT = typename CTConfigT::Output1PinT;
        using Output2PinT = typename CTConfigT::Output2PinT;

        static constexpr bool skHasOutput1Pin = !CNullGPIOPinDevice<Output1PinT>;
        static constexpr bool skHasOutput2Pin = !CNullGPIOPinDevice<Output2PinT>;

        using Output1PinDriverT =
            HRTIMTimerGPIOPinDriver<tkControllerID, tkTimerID, HRTIMTimerGPIOPinType::kOutput1, Output1PinT>;
        using Output2PinDriverT =
            HRTIMTimerGPIOPinDriver<tkControllerID, tkTimerID, HRTIMTimerGPIOPinType::kOutput2, Output2PinT>;

        static constexpr bool skHasFault1 = CTConfigT::skUseFault1;
        static constexpr bool skHasFault2 = CTConfigT::skUseFault2;
        static constexpr bool skHasFault3 = CTConfigT::skUseFault3;
        static constexpr bool skHasFault4 = CTConfigT::skUseFault4;
        static constexpr bool skHasFault5 = CTConfigT::skUseFault5;
        static constexpr bool skHasFault6 = CTConfigT::skUseFault6;
        static constexpr bool skHasFaults =
            skHasFault1 || skHasFault2 || skHasFault3 || skHasFault4 || skHasFault5 || skHasFault6;

        static constexpr bool skHasEEV1  = CTConfigT::skUseEEV1;
        static constexpr bool skHasEEV2  = CTConfigT::skUseEEV2;
        static constexpr bool skHasEEV3  = CTConfigT::skUseEEV3;
        static constexpr bool skHasEEV4  = CTConfigT::skUseEEV4;
        static constexpr bool skHasEEV5  = CTConfigT::skUseEEV5;
        static constexpr bool skHasEEV6  = CTConfigT::skUseEEV6;
        static constexpr bool skHasEEV7  = CTConfigT::skUseEEV7;
        static constexpr bool skHasEEV8  = CTConfigT::skUseEEV8;
        static constexpr bool skHasEEV9  = CTConfigT::skUseEEV9;
        static constexpr bool skHasEEV10 = CTConfigT::skUseEEV10;

        using Fault1DeviceT = std::
            conditional_t<skHasFault1, HRTIMFaultDevice<tkControllerID, HRTIMFaultID::kFault1>, HRTIMNullFaultDevice>;
        using Fault2DeviceT = std::
            conditional_t<skHasFault2, HRTIMFaultDevice<tkControllerID, HRTIMFaultID::kFault2>, HRTIMNullFaultDevice>;
        using Fault3DeviceT = std::
            conditional_t<skHasFault3, HRTIMFaultDevice<tkControllerID, HRTIMFaultID::kFault3>, HRTIMNullFaultDevice>;
        using Fault4DeviceT = std::
            conditional_t<skHasFault4, HRTIMFaultDevice<tkControllerID, HRTIMFaultID::kFault4>, HRTIMNullFaultDevice>;
        using Fault5DeviceT = std::
            conditional_t<skHasFault5, HRTIMFaultDevice<tkControllerID, HRTIMFaultID::kFault5>, HRTIMNullFaultDevice>;
        using Fault6DeviceT = std::
            conditional_t<skHasFault6, HRTIMFaultDevice<tkControllerID, HRTIMFaultID::kFault6>, HRTIMNullFaultDevice>;

        using EEV1DeviceT =
            std::conditional_t<skHasEEV1, HRTIMEEVDevice<tkControllerID, HRTIMEEVID::kEEV1>, HRTIMNullEEVDevice>;
        using EEV2DeviceT =
            std::conditional_t<skHasEEV2, HRTIMEEVDevice<tkControllerID, HRTIMEEVID::kEEV2>, HRTIMNullEEVDevice>;
        using EEV3DeviceT =
            std::conditional_t<skHasEEV3, HRTIMEEVDevice<tkControllerID, HRTIMEEVID::kEEV3>, HRTIMNullEEVDevice>;
        using EEV4DeviceT =
            std::conditional_t<skHasEEV4, HRTIMEEVDevice<tkControllerID, HRTIMEEVID::kEEV4>, HRTIMNullEEVDevice>;
        using EEV5DeviceT =
            std::conditional_t<skHasEEV5, HRTIMEEVDevice<tkControllerID, HRTIMEEVID::kEEV5>, HRTIMNullEEVDevice>;
        using EEV6DeviceT =
            std::conditional_t<skHasEEV6, HRTIMEEVDevice<tkControllerID, HRTIMEEVID::kEEV6>, HRTIMNullEEVDevice>;
        using EEV7DeviceT =
            std::conditional_t<skHasEEV7, HRTIMEEVDevice<tkControllerID, HRTIMEEVID::kEEV7>, HRTIMNullEEVDevice>;
        using EEV8DeviceT =
            std::conditional_t<skHasEEV8, HRTIMEEVDevice<tkControllerID, HRTIMEEVID::kEEV8>, HRTIMNullEEVDevice>;
        using EEV9DeviceT =
            std::conditional_t<skHasEEV9, HRTIMEEVDevice<tkControllerID, HRTIMEEVID::kEEV9>, HRTIMNullEEVDevice>;
        using EEV10DeviceT =
            std::conditional_t<skHasEEV10, HRTIMEEVDevice<tkControllerID, HRTIMEEVID::kEEV10>, HRTIMNullEEVDevice>;

        using InjectDevices = FilterNullDevices<TypeList<ControllerT,
                                                         Output1PinT,
                                                         Output2PinT,
                                                         Fault1DeviceT,
                                                         Fault2DeviceT,
                                                         Fault3DeviceT,
                                                         Fault4DeviceT,
                                                         Fault5DeviceT,
                                                         Fault6DeviceT,
                                                         EEV1DeviceT,
                                                         EEV2DeviceT,
                                                         EEV3DeviceT,
                                                         EEV4DeviceT,
                                                         EEV5DeviceT,
                                                         EEV6DeviceT,
                                                         EEV7DeviceT,
                                                         EEV8DeviceT,
                                                         EEV9DeviceT,
                                                         EEV10DeviceT>>;

    private:
        [[no_unique_address]] DeviceRef<ControllerT>                           m_controller;
        Output1PinDriverT                                                      m_output1_pin;
        Output2PinDriverT                                                      m_output2_pin;
        [[no_unique_address]] ConditionalDeviceRef<skHasFault1, Fault1DeviceT> m_fault1_device;
        [[no_unique_address]] ConditionalDeviceRef<skHasFault2, Fault2DeviceT> m_fault2_device;
        [[no_unique_address]] ConditionalDeviceRef<skHasFault3, Fault3DeviceT> m_fault3_device;
        [[no_unique_address]] ConditionalDeviceRef<skHasFault4, Fault4DeviceT> m_fault4_device;
        [[no_unique_address]] ConditionalDeviceRef<skHasFault5, Fault5DeviceT> m_fault5_device;
        [[no_unique_address]] ConditionalDeviceRef<skHasFault6, Fault6DeviceT> m_fault6_device;
        [[no_unique_address]] ConditionalDeviceRef<skHasEEV1, EEV1DeviceT>     m_eev1_device;
        [[no_unique_address]] ConditionalDeviceRef<skHasEEV2, EEV2DeviceT>     m_eev2_device;
        [[no_unique_address]] ConditionalDeviceRef<skHasEEV3, EEV3DeviceT>     m_eev3_device;
        [[no_unique_address]] ConditionalDeviceRef<skHasEEV4, EEV4DeviceT>     m_eev4_device;
        [[no_unique_address]] ConditionalDeviceRef<skHasEEV5, EEV5DeviceT>     m_eev5_device;
        [[no_unique_address]] ConditionalDeviceRef<skHasEEV6, EEV6DeviceT>     m_eev6_device;
        [[no_unique_address]] ConditionalDeviceRef<skHasEEV7, EEV7DeviceT>     m_eev7_device;
        [[no_unique_address]] ConditionalDeviceRef<skHasEEV8, EEV8DeviceT>     m_eev8_device;
        [[no_unique_address]] ConditionalDeviceRef<skHasEEV9, EEV9DeviceT>     m_eev9_device;
        [[no_unique_address]] ConditionalDeviceRef<skHasEEV10, EEV10DeviceT>   m_eev10_device;

    public:
        template <typename... TArgs>
        explicit HRTIMTimerDevice(TArgs&&... args)
            : m_controller(extract_device_ref<true, ControllerT>(std::forward<TArgs>(args)...))
            , m_output1_pin(extract_device_ref<skHasOutput1Pin, Output1PinT>(std::forward<TArgs>(args)...))
            , m_output2_pin(extract_device_ref<skHasOutput2Pin, Output2PinT>(std::forward<TArgs>(args)...))
            , m_fault1_device(extract_device_ref<skHasFault1, Fault1DeviceT>(std::forward<TArgs>(args)...))
            , m_fault2_device(extract_device_ref<skHasFault2, Fault2DeviceT>(std::forward<TArgs>(args)...))
            , m_fault3_device(extract_device_ref<skHasFault3, Fault3DeviceT>(std::forward<TArgs>(args)...))
            , m_fault4_device(extract_device_ref<skHasFault4, Fault4DeviceT>(std::forward<TArgs>(args)...))
            , m_fault5_device(extract_device_ref<skHasFault5, Fault5DeviceT>(std::forward<TArgs>(args)...))
            , m_fault6_device(extract_device_ref<skHasFault6, Fault6DeviceT>(std::forward<TArgs>(args)...))
            , m_eev1_device(extract_device_ref<skHasEEV1, EEV1DeviceT>(std::forward<TArgs>(args)...))
            , m_eev2_device(extract_device_ref<skHasEEV2, EEV2DeviceT>(std::forward<TArgs>(args)...))
            , m_eev3_device(extract_device_ref<skHasEEV3, EEV3DeviceT>(std::forward<TArgs>(args)...))
            , m_eev4_device(extract_device_ref<skHasEEV4, EEV4DeviceT>(std::forward<TArgs>(args)...))
            , m_eev5_device(extract_device_ref<skHasEEV5, EEV5DeviceT>(std::forward<TArgs>(args)...))
            , m_eev6_device(extract_device_ref<skHasEEV6, EEV6DeviceT>(std::forward<TArgs>(args)...))
            , m_eev7_device(extract_device_ref<skHasEEV7, EEV7DeviceT>(std::forward<TArgs>(args)...))
            , m_eev8_device(extract_device_ref<skHasEEV8, EEV8DeviceT>(std::forward<TArgs>(args)...))
            , m_eev9_device(extract_device_ref<skHasEEV9, EEV9DeviceT>(std::forward<TArgs>(args)...))
            , m_eev10_device(extract_device_ref<skHasEEV10, EEV10DeviceT>(std::forward<TArgs>(args)...))
        {
        }

        // ------------------------------------------------------------------------
        // Initialization
        // ------------------------------------------------------------------------
        [[nodiscard]] inline bool init()
        {
            if constexpr (skHasFaults)
            {
                uint32_t fault_sources = 0;
                if constexpr (skHasFault1)
                {
                    fault_sources |= static_cast<uint32_t>(HRTIMFaultID::kFault1);
                }

                if constexpr (skHasFault2)
                {
                    fault_sources |= static_cast<uint32_t>(HRTIMFaultID::kFault2);
                }

                if constexpr (skHasFault3)
                {
                    fault_sources |= static_cast<uint32_t>(HRTIMFaultID::kFault3);
                }

                if constexpr (skHasFault4)
                {
                    fault_sources |= static_cast<uint32_t>(HRTIMFaultID::kFault4);
                }

                if constexpr (skHasFault5)
                {
                    fault_sources |= static_cast<uint32_t>(HRTIMFaultID::kFault5);
                }

                if constexpr (skHasFault6)
                {
                    fault_sources |= static_cast<uint32_t>(HRTIMFaultID::kFault6);
                }

                LL_HRTIM_TIM_EnableFault(ControllerTraitsT::skInstance, TimerTraitsT::skTimerIdx, fault_sources);
            }

            return true;
        }

        // ------------------------------------------------------------------------
        // Counter
        // ------------------------------------------------------------------------

        void init_counter(const HRTIMTimerCounterConfig& config)
        {
            LL_HRTIM_TIM_SetPrescaler(
                ControllerTraitsT::skInstance, TimerTraitsT::skTimerIdx, static_cast<uint32_t>(config.prescaler));
            LL_HRTIM_TIM_SetCounterMode(
                ControllerTraitsT::skInstance, TimerTraitsT::skTimerIdx, static_cast<uint32_t>(config.counter_mode));
            LL_HRTIM_TIM_SetCountingMode(
                ControllerTraitsT::skInstance, TimerTraitsT::skTimerIdx, static_cast<uint32_t>(config.counting_mode));
            LL_HRTIM_TIM_SetPeriod(ControllerTraitsT::skInstance, TimerTraitsT::skTimerIdx, config.period);
            LL_HRTIM_TIM_SetRepetition(ControllerTraitsT::skInstance, TimerTraitsT::skTimerIdx, config.repetition);
        }

        void enable_counter()
        {
            LL_HRTIM_TIM_CounterEnable(ControllerTraitsT::skInstance, TimerTraitsT::skTimerIdx);
        }

        void disable_counter()
        {
            LL_HRTIM_TIM_CounterDisable(ControllerTraitsT::skInstance, TimerTraitsT::skTimerIdx);
        }

        // ------------------------------------------------------------------------
        // Outputs
        // ------------------------------------------------------------------------

        void init_output1_virtual(const HRTIMVirtualOutputConfig& config)
        {
            init_output<1>(config);
        }

        void init_output1(const HRTIMVirtualOutputConfig& config, const HRTIMOutputGPIOConfig& gpio_config)
            requires(skHasOutput1Pin)
        {
            init_output<1>(config);
            init_output_gpio<1>(gpio_config);
        }

        void enable_output1()
        {
            LL_HRTIM_TIM_EnableOutput(ControllerTraitsT::skInstance, TimerTraitsT::skOutput1);
        }

        void disable_output1()
        {
            LL_HRTIM_TIM_DisableOutput(ControllerTraitsT::skInstance, TimerTraitsT::skOutput1);
        }

        void init_output2_virtual(const HRTIMVirtualOutputConfig& config)
        {
            init_output<2>(config);
        }

        void init_output2(const HRTIMVirtualOutputConfig& config, const HRTIMOutputGPIOConfig& gpio_config)
            requires(skHasOutput2Pin)
        {
            init_output<2>(config);
            init_output_gpio<2>(gpio_config);
        }

        void enable_output2()
        {
            LL_HRTIM_TIM_EnableOutput(ControllerTraitsT::skInstance, TimerTraitsT::skOutput2);
        }

        void disable_output2()
        {
            LL_HRTIM_TIM_DisableOutput(ControllerTraitsT::skInstance, TimerTraitsT::skOutput2);
        }

        void enable_outputs()
        {
            LL_HRTIM_EnableOutput(ControllerTraitsT::skInstance, TimerTraitsT::skOutput1 | TimerTraitsT::skOutput2);
        }

        void disable_outputs()
        {
            LL_HRTIM_DisableOutput(ControllerTraitsT::skInstance, TimerTraitsT::skOutput1 | TimerTraitsT::skOutput2);
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

        // ------------------------------------------------------------------------
        // Interrupts
        // ------------------------------------------------------------------------

        static inline void enable_interrupts(const HRTIMTimerInterruptConfig& config)
        {
            if (config.interrupts == HRTIMTimerInterruptMask{})  // No interrupts enabled
            {
                return;
            }

            if (config.interrupts.enable_compare1)
            {
                HRTIMTimerInterruptTraits<skControllerID, skTimerID, HRTIMTimerInterruptType::kCompare1>::enable();
            }

            if (config.interrupts.enable_compare2)
            {
                HRTIMTimerInterruptTraits<skControllerID, skTimerID, HRTIMTimerInterruptType::kCompare2>::enable();
            }

            if (config.interrupts.enable_compare3)
            {
                HRTIMTimerInterruptTraits<skControllerID, skTimerID, HRTIMTimerInterruptType::kCompare3>::enable();
            }

            if (config.interrupts.enable_compare4)
            {
                HRTIMTimerInterruptTraits<skControllerID, skTimerID, HRTIMTimerInterruptType::kCompare4>::enable();
            }

            if (config.interrupts.enable_update)
            {
                HRTIMTimerInterruptTraits<skControllerID, skTimerID, HRTIMTimerInterruptType::kUpdate>::enable();
            }

            if (config.interrupts.enable_repetition)
            {
                HRTIMTimerInterruptTraits<skControllerID, skTimerID, HRTIMTimerInterruptType::kRepetition>::enable();
            }

            if (config.interrupts.enable_capture1)
            {
                HRTIMTimerInterruptTraits<skControllerID, skTimerID, HRTIMTimerInterruptType::kCapture1>::enable();
            }

            if (config.interrupts.enable_capture2)
            {
                HRTIMTimerInterruptTraits<skControllerID, skTimerID, HRTIMTimerInterruptType::kCapture2>::enable();
            }

            if (config.interrupts.enable_output1_set)
            {
                HRTIMTimerInterruptTraits<skControllerID, skTimerID, HRTIMTimerInterruptType::kOutput1Set>::enable();
            }

            if (config.interrupts.enable_output1_reset)
            {
                HRTIMTimerInterruptTraits<skControllerID, skTimerID, HRTIMTimerInterruptType::kOutput1Reset>::enable();
            }

            if (config.interrupts.enable_output2_set)
            {
                HRTIMTimerInterruptTraits<skControllerID, skTimerID, HRTIMTimerInterruptType::kOutput2Set>::enable();
            }

            if (config.interrupts.enable_output2_reset)
            {
                HRTIMTimerInterruptTraits<skControllerID, skTimerID, HRTIMTimerInterruptType::kOutput2Reset>::enable();
            }

            if (config.interrupts.enable_reset)
            {
                HRTIMTimerInterruptTraits<skControllerID, skTimerID, HRTIMTimerInterruptType::kReset>::enable();
            }

            if (config.interrupts.enable_delayed_protection)
            {
                HRTIMTimerInterruptTraits<skControllerID, skTimerID, HRTIMTimerInterruptType::kDelayedProtection>::
                    enable();
            }

            NVIC_SetPriority(TimerTraitsT::skIRQn, config.priority);
            NVIC_EnableIRQ(TimerTraitsT::skIRQn);
        }

        // ------------------------------------------------------------------------
        // Info
        // ------------------------------------------------------------------------
        [[nodiscard]] static inline uint32_t get_hrtim_freq_hz()
        {
            return get_apb2_timer_clock_freq();
        }

        [[nodiscard]] static inline uint64_t calculate_hrck_freq_hz(const uint32_t            f_hrtim_hz,
                                                                    const HRTIMTimerPrescaler prescaler)
        {
            return (f_hrtim_hz << 5) >> static_cast<uint32_t>(prescaler);  // f_hrtim * 32 / (2^prescaler)
        }

        [[nodiscard]] static inline uint64_t get_hrck_freq_hz()
        {
            const uint32_t            f_hrtim_hz = get_hrtim_freq_hz();
            const HRTIMTimerPrescaler prescaler  = static_cast<HRTIMTimerPrescaler>(
                LL_HRTIM_TIM_GetPrescaler(ControllerTraitsT::skInstance, TimerTraitsT::skTimerIdx));
            return calculate_hrck_freq_hz(f_hrtim_hz, prescaler);
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

        [[nodiscard]] static inline HRTIMTimerPrescaler get_prescaler_for_freq(const uint32_t target_freq_hz)
        {
            // We need to find the appropriate prescaler for the clock.
            // Each prescaler will have a minimum allowable PWM frequency since the counter is 16-bit.
            // This minimum frequency is given by f_min = f_hrtim * prescaler / 2^16
            // We need to find the smallest integer i such that f_hrtim * 32 / (2^i * f_target) <= 65535.
            // We arrive at the following condition for i:
            // i > log2(f_hrtim / f_target) - 11 --> i = floor(log2(f_hrtim / f_target)) - 10
            // To implement this efficiently, we can take advantage of the fact that floor(log2(floor(x))) =
            // floor(log2(x)) and then we can use __CLZ to compute floor(log2(x)) = 31 - __CLZ(x) for 32-bit integers.

            // Validate input to prevent divide-by-zero and __builtin_clz(0)
            // RM0440 Section 29.3.6: HRTIM frequency range is ~81 Hz to 5.3 MHz @ 170 MHz

            // Minimum: f_HRTIM / (32 × 65536) = 170 MHz / 2,097,152 ≈ 81 Hz
            // Maximum: f_HRTIM / 32 = 170 MHz / 32 ≈ 5.3 MHz

            static_assert(HRTIMTimerPrescaler::kMul32 == 0, "Assumes prescaler enum starts at 0 with Mul32");
            static_assert(HRTIMTimerPrescaler::kMul16 == 1, "Assumes prescaler enum values are sequential");
            static_assert(HRTIMTimerPrescaler::kMul8 == 2, "Assumes prescaler enum values are sequential");
            static_assert(HRTIMTimerPrescaler::kMul4 == 3, "Assumes prescaler enum values are sequential");
            static_assert(HRTIMTimerPrescaler::kMul2 == 4, "Assumes prescaler enum values are sequential");
            static_assert(HRTIMTimerPrescaler::kDiv1 == 5, "Assumes prescaler enum values are sequential");
            static_assert(HRTIMTimerPrescaler::kDiv2 == 6, "Assumes prescaler enum values are sequential");
            static_assert(HRTIMTimerPrescaler::kDiv4 == 7, "Assumes prescaler enum values are sequential");

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
                return static_cast<HRTIMTimerPrescaler>(kMaxPrescaler);
            }

            if (target_freq_hz > kMaxFreqHz)
            {
                VALLE_LOG_WARN("HRTIM{} Timer {} target frequency {} Hz is too high, clamping to {} Hz.",
                               static_cast<int>(tkControllerID),
                               enum_name(tkTimerID),
                               target_freq_hz,
                               kMaxFreqHz);

                // Too high - use maximum (prescaler 0)
                return static_cast<HRTIMTimerPrescaler>(kMinPrescaler);
            }

            const uint32_t ratio = f_hrtim_hz / target_freq_hz;
            const int8_t   index = static_cast<int8_t>(31 - __builtin_clz(ratio) - 10);
            return static_cast<HRTIMTimerPrescaler>(std::clamp<int8_t>(index, kMinPrescaler, kMaxPrescaler));
        }

        [[nodiscard]] static inline uint32_t get_period_ticks()
        {
            return LL_HRTIM_TIM_GetPeriod(ControllerTraitsT::skInstance, TimerTraitsT::skTimerIdx);
        }

    private:
        template <uint8_t tkOutputNum>
            requires(tkOutputNum == 1 || tkOutputNum == 2)
        inline void init_output(const HRTIMVirtualOutputConfig& config)
        {
            const uint32_t output = (tkOutputNum == 1) ? TimerTraitsT::skOutput1 : TimerTraitsT::skOutput2;
            LL_HRTIM_OUT_SetOutputSetSrc(
                ControllerTraitsT::skInstance, output, static_cast<uint32_t>(config.set_source));
            LL_HRTIM_OUT_SetOutputResetSrc(
                ControllerTraitsT::skInstance, output, static_cast<uint32_t>(config.reset_source));
            LL_HRTIM_OUT_SetPolarity(ControllerTraitsT::skInstance, output, static_cast<uint32_t>(config.polarity));
            LL_HRTIM_OUT_SetIdleMode(ControllerTraitsT::skInstance, output, static_cast<uint32_t>(config.idle_mode));
            LL_HRTIM_OUT_SetIdleLevel(ControllerTraitsT::skInstance, output, static_cast<uint32_t>(config.idle_level));
            LL_HRTIM_OUT_SetFaultState(
                ControllerTraitsT::skInstance, output, static_cast<uint32_t>(config.fault_state));
            LL_HRTIM_OUT_SetChopperMode(
                ControllerTraitsT::skInstance, output, static_cast<uint32_t>(config.chopper_mode));
            LL_HRTIM_OUT_SetBMEntryMode(
                ControllerTraitsT::skInstance, output, static_cast<uint32_t>(config.burst_mode_entry_mode));
        }

        template <uint8_t tkOutputNum>
            requires(tkOutputNum == 1 || tkOutputNum == 2)
        void init_output_gpio(const HRTIMOutputGPIOConfig& config)
            requires((tkOutputNum == 1 && skHasOutput1Pin) || (tkOutputNum == 2 && skHasOutput2Pin))
        {
            if constexpr (tkOutputNum == 1)
            {
                m_output1_pin.init(GPIOAlternativeFunctionConfig{
                    .mode  = GPIOAlternateFunctionMode::kPushPull,
                    .speed = config.speed,
                    .pull  = config.pull,
                });
            }
            else if constexpr (tkOutputNum == 2)
            {
                m_output2_pin.init(GPIOAlternativeFunctionConfig{
                    .mode  = GPIOAlternateFunctionMode::kPushPull,
                    .speed = config.speed,
                    .pull  = config.pull,
                });
            }
        }
    };

    using HRTIM1TimerADevice = HRTIMTimerDevice<1, HRTIMTimerID::kA>;
    using HRTIM1TimerBDevice = HRTIMTimerDevice<1, HRTIMTimerID::kB>;
    using HRTIM1TimerCDevice = HRTIMTimerDevice<1, HRTIMTimerID::kC>;
    using HRTIM1TimerDDevice = HRTIMTimerDevice<1, HRTIMTimerID::kD>;
    using HRTIM1TimerEDevice = HRTIMTimerDevice<1, HRTIMTimerID::kE>;
    using HRTIM1TimerFDevice = HRTIMTimerDevice<1, HRTIMTimerID::kF>;

}  // namespace valle