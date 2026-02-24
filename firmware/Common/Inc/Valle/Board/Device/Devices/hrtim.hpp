#pragma once

#include <algorithm>
#include <cstdint>

#include "Valle/Board/Device/device_core.hpp"
#include "Valle/Board/Drivers/gpio.hpp"
#include "Valle/Board/Traits/hrtim.hpp"
#include "Valle/Core/Logging/logger.hpp"
#include "Valle/Core/timing.hpp"
#include "Valle/Utils/hal_utils.hpp"
#include "stm32g4xx_ll_bus.h"

namespace valle
{

    // =============================================================================
    // FORWARD DECLARATIONS
    // =============================================================================

    class HRTIMRootDevice;

    template <HRTIMControllerID tkControllerID>
        requires(kValidHRTIMControllerID<tkControllerID>)
    class HRTIMControllerDevice;

    template <HRTIMControllerID tkControllerID, HRTIMFaultID tkFaultID>
        requires(kValidHRTIMControllerID<tkControllerID>)
    class HRTIMFaultDevice;

    template <HRTIMControllerID tkControllerID, HRTIMEEVID tkEEVID>
        requires(kValidHRTIMControllerID<tkControllerID>)
    class HRTIMEEVDevice;

    template <HRTIMControllerID tkControllerID, HRTIMTimerID tkTimerID>
        requires(kValidHRTIMControllerID<tkControllerID>)
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

    // -----------------------------------------------------------------------------
    // COMPILE TIME CONFIGURATIONS
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
    // HELPERS
    // -----------------------------------------------------------------------------

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

    // -----------------------------------------------------------------------------
    // DEVICE CLASS
    // -----------------------------------------------------------------------------

    template <HRTIMControllerID tkControllerID>
        requires(kValidHRTIMControllerID<tkControllerID>)
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

    // -----------------------------------------------------------------------------
    // DEVICE ALIASES
    // -----------------------------------------------------------------------------

    using HRTIM1ControllerDevice = HRTIMControllerDevice<1>;

    // =============================================================================
    // HRTIM FAULT DEVICE (SHARED DEVICE)
    // =============================================================================

    // -----------------------------------------------------------------------------
    // COMPILE TIME CONFIGURATIONS
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
    // CONFIGURATIONS
    // -----------------------------------------------------------------------------
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

    // -----------------------------------------------------------------------------
    // HELPERS
    // -----------------------------------------------------------------------------

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

    // -----------------------------------------------------------------------------
    // DEVICE CLASS
    // -----------------------------------------------------------------------------

    template <HRTIMControllerID tkControllerID, HRTIMFaultID tkFaultID>
        requires(kValidHRTIMControllerID<tkControllerID>)
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
        using FaultTraitsT      = HRTIMFaultTraits<tkControllerID, skFaultID>;

        using CTConfigTraitsT            = HRTIMFaultCTConfigTraits<tkControllerID, skFaultID>;
        static constexpr auto skCTConfig = CTConfigTraitsT::skConfig;
        using CTConfigT                  = decltype(skCTConfig);

        using PinT       = typename CTConfigT::PinT;
        using PinDriverT = HRTIMFaultGPIOPinDriver<tkControllerID, skFaultID, PinT>;

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
                ControllerTraitsT::skInstance, FaultTraitsT::skLLID, static_cast<uint32_t>(config.source));

            LL_HRTIM_FLT_SetPolarity(
                ControllerTraitsT::skInstance, FaultTraitsT::skLLID, static_cast<uint32_t>(config.polarity));

            LL_HRTIM_FLT_SetFilter(
                ControllerTraitsT::skInstance, FaultTraitsT::skLLID, static_cast<uint32_t>(config.filter));

            LL_HRTIM_FLT_SetPrescaler(
                ControllerTraitsT::skInstance, FaultTraitsT::skLLID, static_cast<uint32_t>(config.prescaler));

            if (config.blanking_source.has_value())
            {
                LL_HRTIM_FLT_EnableBlanking(ControllerTraitsT::skInstance, FaultTraitsT::skLLID);
                LL_HRTIM_FLT_SetBlankingSrc(ControllerTraitsT::skInstance,
                                            FaultTraitsT::skLLID,
                                            static_cast<uint32_t>(config.blanking_source.value()));
            }
            else
            {
                LL_HRTIM_FLT_DisableBlanking(ControllerTraitsT::skInstance, FaultTraitsT::skLLID);
            }

            LL_HRTIM_FLT_SetResetMode(
                ControllerTraitsT::skInstance, FaultTraitsT::skLLID, static_cast<uint32_t>(config.reset_mode));
            LL_HRTIM_FLT_SetCountingThreshold(
                ControllerTraitsT::skInstance, FaultTraitsT::skLLID, static_cast<uint32_t>(config.counting_threshold));

            if (config.lock)
            {
                LL_HRTIM_FLT_Lock(ControllerTraitsT::skInstance, FaultTraitsT::skLLID);
            }

            return true;
        }

        [[nodiscard]] inline bool post_init()
        {
            // Do nothing for now
            return true;
        }
    };

    class HRTIMNullFaultDevice
    {
    public:
        struct Descriptor : public NullDeviceDescriptor
        {
        };
    };

    // -----------------------------------------------------------------------------
    // DEVICE ALIASES
    // -----------------------------------------------------------------------------

    using HRTIM1Fault1Device = HRTIMFaultDevice<1, HRTIMFaultID::kFault1>;
    using HRTIM1Fault2Device = HRTIMFaultDevice<1, HRTIMFaultID::kFault2>;
    using HRTIM1Fault3Device = HRTIMFaultDevice<1, HRTIMFaultID::kFault3>;
    using HRTIM1Fault4Device = HRTIMFaultDevice<1, HRTIMFaultID::kFault4>;
    using HRTIM1Fault5Device = HRTIMFaultDevice<1, HRTIMFaultID::kFault5>;
    using HRTIM1Fault6Device = HRTIMFaultDevice<1, HRTIMFaultID::kFault6>;

    // =============================================================================
    // HRTIM EXTERNAL EVENT DEVICE (SHARED DEVICE)
    // =============================================================================

    // -----------------------------------------------------------------------------
    // COMPILE TIME CONFIGURATIONS
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

    // -----------------------------------------------------------------------------
    // CONFIGURATIONS
    // -----------------------------------------------------------------------------
    struct HRTIMEEVConfig
    {
        HRTIMEEVSource      source      = HRTIMEEVSource::kSource1;
        HRTIMEEVPolarity    polarity    = HRTIMEEVPolarity::kHigh;
        HRTIMEEVSensitivity sensitivity = HRTIMEEVSensitivity::kLevel;
        HRTIMEEVFastMode    fast_mode   = HRTIMEEVFastMode::kDisable;
        HRTIMEEVFilter      filter      = HRTIMEEVFilter::kNone;
        HRTIMEEVPrescaler   prescaler   = HRTIMEEVPrescaler::kDiv1;
    };

    // -----------------------------------------------------------------------------
    // HELPERS
    // -----------------------------------------------------------------------------

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

    // -----------------------------------------------------------------------------
    // DEVICE CLASS
    // -----------------------------------------------------------------------------

    template <HRTIMControllerID tkControllerID, HRTIMEEVID tkEEVID>
        requires(kValidHRTIMControllerID<tkControllerID>)
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
        using EEVTraitsT        = HRTIMEEVTraits<tkControllerID, skEEVID>;

        using CTConfigTraitsT            = HRTIMEEVCTConfigTraits<tkControllerID, skEEVID>;
        static constexpr auto skCTConfig = CTConfigTraitsT::skConfig;
        using CTConfigT                  = decltype(skCTConfig);

        using PinT       = typename CTConfigT::PinT;
        using PinDriverT = HRTIMEEVGPIOPinDriver<tkControllerID, skEEVID, PinT>;

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
            LL_HRTIM_EE_SetSrc(ControllerTraitsT::skInstance, EEVTraitsT::skLLID, static_cast<uint32_t>(config.source));

            LL_HRTIM_EE_SetPolarity(
                ControllerTraitsT::skInstance, EEVTraitsT::skLLID, static_cast<uint32_t>(config.polarity));

            LL_HRTIM_EE_SetSensitivity(
                ControllerTraitsT::skInstance, EEVTraitsT::skLLID, static_cast<uint32_t>(config.sensitivity));

            LL_HRTIM_EE_SetFastMode(
                ControllerTraitsT::skInstance, EEVTraitsT::skLLID, static_cast<uint32_t>(config.fast_mode));

            LL_HRTIM_EE_SetFilter(
                ControllerTraitsT::skInstance, EEVTraitsT::skLLID, static_cast<uint32_t>(config.filter));

            LL_HRTIM_EE_SetPrescaler(
                ControllerTraitsT::skInstance, EEVTraitsT::skLLID, static_cast<uint32_t>(config.prescaler));

            return true;
        }

        [[nodiscard]] inline bool post_init()
        {
            // Do nothing for now
            return true;
        }
    };

    class HRTIMNullEEVDevice
    {
    public:
        struct Descriptor : public NullDeviceDescriptor
        {
        };
    };

    // -----------------------------------------------------------------------------
    // DEVICE ALIASES
    // -----------------------------------------------------------------------------

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

    // =============================================================================
    // HRTIM TIMER DEVICE (UNIQUE DEVICE)
    // =============================================================================

    // ---------------------------------------------------------------------------
    // COMPILE TIME CONFIGURATIONS
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

    // -----------------------------------------------------------------------------
    // CONFIGURATIONS
    // -----------------------------------------------------------------------------
    struct HRTIMTimerCounterConfig
    {
        HRTIMTimerPrescaler    prescaler     = HRTIMTimerPrescaler::kDiv1;
        HRTIMTimerCounterMode  counter_mode  = HRTIMTimerCounterMode::kContinuous;
        HRTIMTimerCountingMode counting_mode = HRTIMTimerCountingMode::kUp;
        uint16_t               period        = 0xFFFF;
        uint8_t                repetition    = 0;
    };

    struct HRTIMTimerVirtualOutputConfig
    {
        HRTIMTimerOutputSetSource          set_source            = HRTIMTimerOutputSetSource::kNone;
        HRTIMTimerOutputResetSource        reset_source          = HRTIMTimerOutputResetSource::kNone;
        HRTIMTimerOutputPolarity           polarity              = HRTIMTimerOutputPolarity::kPositive;
        HRTIMTimerOutputIdleMode           idle_mode             = HRTIMTimerOutputIdleMode::kNoIdle;
        HRTIMTimerOutputIdleLevel          idle_level            = HRTIMTimerOutputIdleLevel::kInactive;
        HRTIMTimerOutputFaultState         fault_state           = HRTIMTimerOutputFaultState::kInactive;
        HRTIMTimerOutputChopperMode        chopper_mode          = HRTIMTimerOutputChopperMode::kDisabled;
        HRTIMTimerOutputBurstModeEntryMode burst_mode_entry_mode = HRTIMTimerOutputBurstModeEntryMode::kRegular;
    };

    struct HRTIMTimerOutputGPIOConfig
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

    // -----------------------------------------------------------------------------
    // HELPERS
    // -----------------------------------------------------------------------------

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

    // -----------------------------------------------------------------------------
    // DEVICE CLASS
    // -----------------------------------------------------------------------------

    template <HRTIMControllerID tkControllerID, HRTIMTimerID tkTimerID>
        requires(kValidHRTIMControllerID<tkControllerID>)
    class HRTIMTimerDevice
    {
    public:
        struct Descriptor : public UniqueDeviceDescriptor
        {
        };

        static constexpr HRTIMControllerID skControllerID = tkControllerID;
        static constexpr HRTIMTimerID      skTimerID      = tkTimerID;

        using ControllerT          = HRTIMControllerDevice<tkControllerID>;
        using ControllerTraitsT    = HRTIMControllerTraits<tkControllerID>;
        using TimerTraitsT         = HRTIMTimerTraits<tkControllerID, tkTimerID>;
        using InterruptControllerT = HRTIMTimerInterruptController<tkControllerID, tkTimerID>;

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

                LL_HRTIM_TIM_EnableFault(ControllerTraitsT::skInstance, TimerTraitsT::skLLID, fault_sources);
            }

            return true;
        }

        // ------------------------------------------------------------------------
        // Counter
        // ------------------------------------------------------------------------

        [[nodiscard]] bool init_counter(const HRTIMTimerCounterConfig& config)
        {
            LL_HRTIM_TIM_SetPrescaler(
                ControllerTraitsT::skInstance, TimerTraitsT::skLLID, static_cast<uint32_t>(config.prescaler));
            LL_HRTIM_TIM_SetCounterMode(
                ControllerTraitsT::skInstance, TimerTraitsT::skLLID, static_cast<uint32_t>(config.counter_mode));
            LL_HRTIM_TIM_SetCountingMode(
                ControllerTraitsT::skInstance, TimerTraitsT::skLLID, static_cast<uint32_t>(config.counting_mode));
            LL_HRTIM_TIM_SetPeriod(ControllerTraitsT::skInstance, TimerTraitsT::skLLID, config.period);
            LL_HRTIM_TIM_SetRepetition(ControllerTraitsT::skInstance, TimerTraitsT::skLLID, config.repetition);
            return true;
        }

        void enable_counter()
        {
            LL_HRTIM_TIM_CounterEnable(ControllerTraitsT::skInstance, TimerTraitsT::skLLID);
        }

        void disable_counter()
        {
            LL_HRTIM_TIM_CounterDisable(ControllerTraitsT::skInstance, TimerTraitsT::skLLID);
        }

        // ------------------------------------------------------------------------
        // Outputs
        // ------------------------------------------------------------------------

        [[nodiscard]] bool init_output1_virtual(const HRTIMTimerVirtualOutputConfig& config)
        {
            return init_output<1>(config);
        }

        [[nodiscard]] bool init_output1(const HRTIMTimerVirtualOutputConfig& config,
                                        const HRTIMTimerOutputGPIOConfig&    gpio_config)
            requires(skHasOutput1Pin)
        {
            if (!init_output<1>(config))
            {
                return false;
            }

            return init_output_gpio<1>(gpio_config);
        }

        void enable_output1()
        {
            LL_HRTIM_TIM_EnableOutput(ControllerTraitsT::skInstance, TimerTraitsT::skOutput1);
        }

        void disable_output1()
        {
            LL_HRTIM_TIM_DisableOutput(ControllerTraitsT::skInstance, TimerTraitsT::skOutput1);
        }

        [[nodiscard]] bool init_output2_virtual(const HRTIMTimerVirtualOutputConfig& config)
        {
            return init_output<2>(config);
        }

        [[nodiscard]] bool init_output2(const HRTIMTimerVirtualOutputConfig& config,
                                        const HRTIMTimerOutputGPIOConfig&    gpio_config)
            requires(skHasOutput2Pin)
        {
            if (!init_output<2>(config))
            {
                return false;
            }
            return init_output_gpio<2>(gpio_config);
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

        [[nodiscard]] bool init_deadtime(const HRTIMDeadTimeConfig& config)
        {
            // Validate deadtime against hardware limits
            // RM0440 Section 29.3.12: Max deadtime = 511 × tDTG where tDTG = tHRCK × 2^DTPRSC
            // At 170 MHz with max prescaler (7): 511 × (1/(8×170MHz)) × 128 ≈ 48 µs
            const uint32_t f_hrtim_hz = get_hrtim_freq_hz();

            const float min_requested_ns = std::min<float>(config.fall_ns, config.rise_ns);
            const float max_requested_ns = std::max<float>(config.fall_ns, config.rise_ns);

            const float min_hw_deadtime_ns = calculate_min_deadtime_ns(f_hrtim_hz);
            if (min_requested_ns < min_hw_deadtime_ns)
            {
                // ERROR: Negative deadtime is invalid
                VALLE_LOG_ERROR("HRTIM{} Timer {} requested deadtime ({} ns) is below hardware limit ({} ns)!",
                                static_cast<int>(tkControllerID),
                                enum_name(tkTimerID),
                                min_requested_ns,
                                min_hw_deadtime_ns);
                return false;
            }

            const float max_hw_deadtime_ns = calculate_max_deadtime_ns(f_hrtim_hz);
            if (max_requested_ns > max_hw_deadtime_ns)
            {
                // ERROR: Requested deadtime exceeds hardware capability
                VALLE_LOG_ERROR("HRTIM{} Timer {} requested deadtime ({} ns) exceeds hardware limit ({} ns)!",
                                static_cast<int>(tkControllerID),
                                enum_name(tkTimerID),
                                max_requested_ns,
                                max_hw_deadtime_ns);
                return false;
            }

            const HRTIMTimerDeadtimePrescaler prescaler_val =
                calculate_deadtime_prescaler(f_hrtim_hz, max_requested_ns);
            const uint32_t rising_counts  = calculate_deadtime_count(f_hrtim_hz, prescaler_val, config.rise_ns);
            const uint32_t falling_counts = calculate_deadtime_count(f_hrtim_hz, prescaler_val, config.fall_ns);

            LL_HRTIM_DT_SetPrescaler(
                ControllerTraitsT::skInstance, TimerTraitsT::skLLID, static_cast<uint32_t>(prescaler_val));

            LL_HRTIM_DT_SetRisingValue(ControllerTraitsT::skInstance, TimerTraitsT::skLLID, rising_counts);
            LL_HRTIM_DT_SetFallingValue(ControllerTraitsT::skInstance, TimerTraitsT::skLLID, falling_counts);

            // RM0440 Section 29.3.12: Lock bits are write-once, cannot be cleared without MCU reset
            if (config.lock_registers)
            {
                LL_HRTIM_DT_LockFalling(ControllerTraitsT::skInstance, TimerTraitsT::skLLID);
                LL_HRTIM_DT_LockRising(ControllerTraitsT::skInstance, TimerTraitsT::skLLID);
            }

            LL_HRTIM_TIM_EnableDeadTime(ControllerTraitsT::skInstance, TimerTraitsT::skLLID);
            return true;
        }

        static inline void disable_deadtime()
        {
            LL_HRTIM_DT_SetRisingValue(ControllerTraitsT::skInstance, TimerTraitsT::skLLID, 0);
            LL_HRTIM_DT_SetFallingValue(ControllerTraitsT::skInstance, TimerTraitsT::skLLID, 0);
            LL_HRTIM_TIM_DisableDeadTime(ControllerTraitsT::skInstance, TimerTraitsT::skLLID);
        }

        // ------------------------------------------------------------------------
        // Interrupts
        // ------------------------------------------------------------------------

        static void enable_interrupts(const HRTIMTimerInterruptConfig& config)
        {
            InterruptControllerT::enable_interrupts(config);
        }

        static void disable_interrupts()
        {
            InterruptControllerT::disable_interrupts();
        }

        // ------------------------------------------------------------------------
        // Info
        // ------------------------------------------------------------------------
        [[nodiscard]] static inline uint32_t get_hrtim_freq_hz()
        {
            return get_apb2_timer_clock_freq();
        }

        [[nodiscard]] static inline HRTIMTimerPrescaler get_timer_prescaler()
        {
            return static_cast<HRTIMTimerPrescaler>(
                LL_HRTIM_TIM_GetPrescaler(ControllerTraitsT::skInstance, TimerTraitsT::skLLID));
        }

        [[nodiscard]] static inline uint8_t timer_prescaler_to_prescaler_num(HRTIMTimerPrescaler prescaler)
        {
            static_assert(static_cast<uint32_t>(HRTIMTimerPrescaler::kMul32) == 0,
                          "Assumes prescaler enum starts at 0 with Mul32");
            static_assert(static_cast<uint32_t>(HRTIMTimerPrescaler::kMul16) == 1,
                          "Assumes prescaler enum values are sequential");
            static_assert(static_cast<uint32_t>(HRTIMTimerPrescaler::kMul8) == 2,
                          "Assumes prescaler enum values are sequential");
            static_assert(static_cast<uint32_t>(HRTIMTimerPrescaler::kMul4) == 3,
                          "Assumes prescaler enum values are sequential");
            static_assert(static_cast<uint32_t>(HRTIMTimerPrescaler::kMul2) == 4,
                          "Assumes prescaler enum values are sequential");
            static_assert(static_cast<uint32_t>(HRTIMTimerPrescaler::kDiv1) == 5,
                          "Assumes prescaler enum values are sequential");
            static_assert(static_cast<uint32_t>(HRTIMTimerPrescaler::kDiv2) == 6,
                          "Assumes prescaler enum values are sequential");
            static_assert(static_cast<uint32_t>(HRTIMTimerPrescaler::kDiv4) == 7,
                          "Assumes prescaler enum values are sequential");

            return static_cast<uint8_t>(prescaler);
        }

        [[nodiscard]] static constexpr inline HRTIMTimerPrescaler timer_prescaler_num_to_prescaler(
            uint8_t prescaler_num)
        {
            static_assert(static_cast<uint32_t>(HRTIMTimerPrescaler::kMul32) == 0,
                          "Assumes prescaler enum starts at 0 with Mul32");
            static_assert(static_cast<uint32_t>(HRTIMTimerPrescaler::kMul16) == 1,
                          "Assumes prescaler enum values are sequential");
            static_assert(static_cast<uint32_t>(HRTIMTimerPrescaler::kMul8) == 2,
                          "Assumes prescaler enum values are sequential");
            static_assert(static_cast<uint32_t>(HRTIMTimerPrescaler::kMul4) == 3,
                          "Assumes prescaler enum values are sequential");
            static_assert(static_cast<uint32_t>(HRTIMTimerPrescaler::kMul2) == 4,
                          "Assumes prescaler enum values are sequential");
            static_assert(static_cast<uint32_t>(HRTIMTimerPrescaler::kDiv1) == 5,
                          "Assumes prescaler enum values are sequential");
            static_assert(static_cast<uint32_t>(HRTIMTimerPrescaler::kDiv2) == 6,
                          "Assumes prescaler enum values are sequential");
            static_assert(static_cast<uint32_t>(HRTIMTimerPrescaler::kDiv4) == 7,
                          "Assumes prescaler enum values are sequential");

            return static_cast<HRTIMTimerPrescaler>(prescaler_num);
        }

        [[nodiscard]] static inline uint32_t get_period_ticks()
        {
            return LL_HRTIM_TIM_GetPeriod(ControllerTraitsT::skInstance, TimerTraitsT::skLLID);
        }

        [[nodiscard]] static inline uint32_t get_repetition()
        {
            return LL_HRTIM_TIM_GetRepetition(ControllerTraitsT::skInstance, TimerTraitsT::skLLID);
        }

        [[nodiscard]] static inline uint64_t calculate_hrck_freq_hz(const uint32_t            f_hrtim_hz,
                                                                    const HRTIMTimerPrescaler prescaler)
        {
            return (f_hrtim_hz << (5 - timer_prescaler_to_prescaler_num(prescaler)));  // f_hrtim * 32 / (2^prescaler)
        }

        [[nodiscard]] static inline uint64_t get_hrck_freq_hz()
        {
            return calculate_hrck_freq_hz(get_hrtim_freq_hz(), get_timer_prescaler());
        }

        [[nodiscard]] static constexpr inline uint64_t calculate_timer_freq_hz(const uint64_t f_hrck_hz,
                                                                               const uint32_t period_ticks)
        {
            return f_hrck_hz / (period_ticks + 1);
        }

        [[nodiscard]] static inline uint64_t get_timer_freq_hz()
        {
            return calculate_timer_freq_hz(get_hrck_freq_hz(), get_period_ticks());
        }

        [[nodiscard]] static constexpr inline uint64_t calculate_int_freq_hz(const uint32_t f_timer_hz,
                                                                             const uint32_t repetition)
        {
            return f_timer_hz / (repetition + 1);
        }

        [[nodiscard]] static inline uint64_t get_int_freq_hz()
        {
            return calculate_int_freq_hz(get_timer_freq_hz(), get_repetition());
        }

        [[nodiscard]] static constexpr inline uint64_t calculate_min_freq_hz(const uint32_t f_hrtim_hz)
        {
            // Minimum frequency is achieved with maximum prescaler (7) and maximum period (65535)
            return calculate_timer_freq_hz(calculate_hrck_freq_hz(f_hrtim_hz, HRTIMTimerPrescaler::kDiv4), 65535);
        }

        [[nodiscard]] static constexpr inline uint64_t calculate_max_freq_hz(const uint32_t f_hrtim_hz)
        {
            // Maximum frequency is achieved with minimum prescaler (0) and minimum period (1)
            return calculate_timer_freq_hz(calculate_hrck_freq_hz(f_hrtim_hz, HRTIMTimerPrescaler::kMul32), 1);
        }

        [[nodiscard]] static constexpr inline HRTIMTimerPrescaler calculate_timer_prescaler_for_freq(
            const uint32_t f_hrtim_hz, const uint64_t target_freq_hz)
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

            constexpr uint32_t kMinPrescaler = 0;
            constexpr uint32_t kMaxPrescaler = 7;

            // Validate and clamp target frequency to valid range
            if (target_freq_hz < calculate_min_freq_hz(f_hrtim_hz))
            {
                // Too low - use minimum (prescaler 7)
                return static_cast<HRTIMTimerPrescaler>(kMaxPrescaler);
            }

            if (target_freq_hz > calculate_max_freq_hz(f_hrtim_hz))
            {
                // Too high - use maximum (prescaler 0)
                return static_cast<HRTIMTimerPrescaler>(kMinPrescaler);
            }

            const uint32_t ratio = f_hrtim_hz / target_freq_hz;
            const int8_t   index = static_cast<int8_t>(31 - __builtin_clz(ratio) - 10);
            return timer_prescaler_num_to_prescaler(std::clamp<int8_t>(index, kMinPrescaler, kMaxPrescaler));
        }

        // -------------------------------------------------------------------------
        // DEADTIME
        // -------------------------------------------------------------------------

        [[nodiscard]] static constexpr inline HRTIMTimerDeadtimePrescaler deadtime_prescaler_num_to_prescaler(
            uint8_t prescaler_num)
        {
            static_assert(static_cast<HRTIMTimerDeadtimePrescaler>(0 << HRTIM_DTR_DTPRSC_Pos) ==
                              HRTIMTimerDeadtimePrescaler::kMul8,
                          "Assumes deadtime prescaler enum values are sequential and start at 0 with Mul8");

            static_assert(static_cast<HRTIMTimerDeadtimePrescaler>(1 << HRTIM_DTR_DTPRSC_Pos) ==
                              HRTIMTimerDeadtimePrescaler::kMul4,
                          "Assumes deadtime prescaler enum values are sequential and start at 0 with Mul8");

            static_assert(static_cast<HRTIMTimerDeadtimePrescaler>(2 << HRTIM_DTR_DTPRSC_Pos) ==
                              HRTIMTimerDeadtimePrescaler::kMul2,
                          "Assumes deadtime prescaler enum values are sequential and start at 0 with Mul8");

            static_assert(static_cast<HRTIMTimerDeadtimePrescaler>(3 << HRTIM_DTR_DTPRSC_Pos) ==
                              HRTIMTimerDeadtimePrescaler::kDiv1,
                          "Assumes deadtime prescaler enum values are sequential and start at 0 with Mul8");

            static_assert(static_cast<HRTIMTimerDeadtimePrescaler>(4 << HRTIM_DTR_DTPRSC_Pos) ==
                              HRTIMTimerDeadtimePrescaler::kDiv2,
                          "Assumes deadtime prescaler enum values are sequential and start at 0 with Mul8");

            static_assert(static_cast<HRTIMTimerDeadtimePrescaler>(5 << HRTIM_DTR_DTPRSC_Pos) ==
                              HRTIMTimerDeadtimePrescaler::kDiv4,
                          "Assumes deadtime prescaler enum values are sequential and start at 0 with Mul8");

            static_assert(static_cast<HRTIMTimerDeadtimePrescaler>(6 << HRTIM_DTR_DTPRSC_Pos) ==
                              HRTIMTimerDeadtimePrescaler::kDiv8,
                          "Assumes deadtime prescaler enum values are sequential and start at 0 with Mul8");

            static_assert(static_cast<HRTIMTimerDeadtimePrescaler>(7 << HRTIM_DTR_DTPRSC_Pos) ==
                              HRTIMTimerDeadtimePrescaler::kDiv16,
                          "Assumes deadtime prescaler enum values are sequential and start at 0 with Mul8");

            uint8_t clamped_num = std::clamp<uint8_t>(prescaler_num, 0, 7);

            // Deadtime prescaler value is offset by bit 0 of the DTRRSC register (HRTIM_DTR_DTPRSC_Pos)
            return static_cast<HRTIMTimerDeadtimePrescaler>(clamped_num << HRTIM_DTR_DTPRSC_Pos);
        }

        [[nodiscard]] static inline constexpr uint8_t deadtime_prescaler_to_prescaler_num(
            const HRTIMTimerDeadtimePrescaler prescaler)
        {
            return (static_cast<uint32_t>(prescaler) >> HRTIM_DTR_DTPRSC_Pos) & 0x7;
        }

        [[nodiscard]] static constexpr inline float calculate_deadtime_period(
            const uint32_t f_hrtim_hz, const HRTIMTimerDeadtimePrescaler prescaler)
        {
            // Deadtime period is given by t_deadtime = 1 / f_deadtime where f_deadtime = f_hrtim * 8 / 2^(prescaler).
            // Therefore, t_deadtime = 2^(prescaler) / (f_hrtim * 8) in seconds, convert to ns
            const uint32_t prescaler_num = deadtime_prescaler_to_prescaler_num(prescaler);
            return (static_cast<float>(1ULL << prescaler_num) * 1e9F) / (static_cast<float>(f_hrtim_hz) * 8.0F);
        }

        [[nodiscard]] static constexpr inline float calculate_max_deadtime_ns(const uint32_t f_hrtim_hz)
        {
            return 511 * calculate_deadtime_period(f_hrtim_hz, HRTIMTimerDeadtimePrescaler::kDiv16);
        }

        [[nodiscard]] static constexpr inline float calculate_min_deadtime_ns(const uint32_t f_hrtim_hz)
        {
            return calculate_deadtime_period(f_hrtim_hz, HRTIMTimerDeadtimePrescaler::kMul8);
        }

        [[nodiscard]] static constexpr inline HRTIMTimerDeadtimePrescaler calculate_deadtime_prescaler(
            const uint32_t f_hrtim_hz, const float target_deadtime_ns)
        {
            // The maximum deadtime time is given by the deadtime clock period multiplied by 511 (max register value).
            // The deadtime clock frequency is given by f_deadtime = f_hrtim * 8 / 2^(prescaler).
            // The deadtime period in ns is given by t_deadtime = 1 / f_deadtime = 2^(prescaler) / (f_hrtim * 8) * 10^9.
            // The maximum deadtime is given by t_max_deadtime = t_deadtime * 511 = 511 * 2^(prescaler) / (f_hrtim * 8)
            // * 10^9. We need to ensure that target_deadtime_ns <= t_max_deadtime, which gives us the condition:
            // target_deadtime_ns <= 511 * 2^(prescaler) / (f_hrtim * 8) * 10^9
            // Rearranging gives us:
            // 2^(prescaler) >= (target_deadtime_ns * f_hrtim * 8) / (511 * 10^9)
            // Taking log2 of both sides gives us:
            // prescaler >= log2((target_deadtime_ns * f_hrtim * 8) / (511 * 10^9))
            // prescaler = ceil(log2((target_deadtime_ns * f_hrtim * 8) / (511 * 10^9)))

            if (target_deadtime_ns < calculate_min_deadtime_ns(f_hrtim_hz))
            {
                // Too low - use minimum (prescaler 0)
                return HRTIMTimerDeadtimePrescaler::kMul8;
            }

            if (target_deadtime_ns > calculate_max_deadtime_ns(f_hrtim_hz))
            {
                // Too high - use maximum (prescaler 7)
                return HRTIMTimerDeadtimePrescaler::kDiv16;
            }

            const double threshold =
                (static_cast<double>(target_deadtime_ns) * static_cast<double>(f_hrtim_hz) * 8.0) / (511.0 * 1e9);

            if (threshold <= 1.0)
            {
                // Too low - use minimum (prescaler 0)
                return HRTIMTimerDeadtimePrescaler::kMul8;
            }

            const uint8_t prescaler_num = static_cast<uint8_t>(std::ceil(std::log2(threshold)));
            return deadtime_prescaler_num_to_prescaler(prescaler_num);
        }

        [[nodiscard]] static constexpr inline uint32_t calculate_deadtime_count(
            const uint32_t f_hrtim_hz, const HRTIMTimerDeadtimePrescaler prescaler, const float target_deadtime_ns)
        {
            // Once we have the appropriate prescaler, we can calculate the required counts for the desired deadtime.
            // counts is given by: counts = target_deadtime_ns / t_deadtime where t_deadtime = 2^(prescaler) / (f_hrtim
            // * 8) * 10^9. Rearranging gives us: counts = (target_deadtime_ns * f_hrtim * 8) / (2^(prescaler) * 10^9)

            const float deadtime_period_ns = calculate_deadtime_period(f_hrtim_hz, prescaler);
            return std::clamp(static_cast<uint32_t>(target_deadtime_ns / deadtime_period_ns),
                              static_cast<uint32_t>(0U),
                              static_cast<uint32_t>(511U));
        }

    private:
        template <uint8_t tkOutputNum>
            requires(tkOutputNum == 1 || tkOutputNum == 2)
        [[nodiscard]] bool init_output(const HRTIMTimerVirtualOutputConfig& config)
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
            return true;
        }

        template <uint8_t tkOutputNum>
            requires(tkOutputNum == 1 || tkOutputNum == 2)
        [[nodiscard]] bool init_output_gpio(const HRTIMTimerOutputGPIOConfig& config)
            requires((tkOutputNum == 1 && skHasOutput1Pin) || (tkOutputNum == 2 && skHasOutput2Pin))
        {
            if constexpr (tkOutputNum == 1)
            {
                return m_output1_pin.init(GPIOAlternativeFunctionConfig{
                    .mode  = GPIOAlternateFunctionMode::kPushPull,
                    .speed = config.speed,
                    .pull  = config.pull,
                });
            }
            else if constexpr (tkOutputNum == 2)
            {
                return m_output2_pin.init(GPIOAlternativeFunctionConfig{
                    .mode  = GPIOAlternateFunctionMode::kPushPull,
                    .speed = config.speed,
                    .pull  = config.pull,
                });
            }
        }
    };

    // -----------------------------------------------------------------------------
    // DEVICE ALIASES
    // -----------------------------------------------------------------------------

    using HRTIM1TimerADevice = HRTIMTimerDevice<1, HRTIMTimerID::kA>;
    using HRTIM1TimerBDevice = HRTIMTimerDevice<1, HRTIMTimerID::kB>;
    using HRTIM1TimerCDevice = HRTIMTimerDevice<1, HRTIMTimerID::kC>;
    using HRTIM1TimerDDevice = HRTIMTimerDevice<1, HRTIMTimerID::kD>;
    using HRTIM1TimerEDevice = HRTIMTimerDevice<1, HRTIMTimerID::kE>;
    using HRTIM1TimerFDevice = HRTIMTimerDevice<1, HRTIMTimerID::kF>;

}  // namespace valle