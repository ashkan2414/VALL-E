#pragma once

#include <algorithm>
#include <cstdint>

#include "stm32g4xx_ll_bus.h"
#include "valle/platform/core.hpp"
#include "valle/platform/devices/rcc.hpp"
#include "valle/platform/drivers/gpio/alternate_function.hpp"
#include "valle/platform/hardware/hrtim.hpp"

namespace valle::platform
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
    struct HRTIMControllerCTDefaultConfig
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
        static constexpr auto skConfig = HRTIMControllerCTDefaultConfig{};
    };

#define VALLE_DEFINE_HRTIM_CONTROLLER_CT_CONFIG(tkControllerID, config)                        \
    namespace valle::platform                                                                  \
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

        [[nodiscard]] bool init()
        {
            // Enable Bus Clock
            ControllerTraitsT::enable_clock();

            // Wait for clock to stabilize
            // RM0440: Allow a few APB2 cycles for clock propagation
            TimingContext::delay_cycles_busy(10);

            // Start DLL Calibration
            LL_HRTIM_StartDLLCalibration(ControllerTraitsT::skInstance);

            // Wait for calibration to finish with precise timeout
            // RM0440 Section 29.3.15: DLL calibration time = 14 µs typical
            // Allow up to 100 µs
            const bool calibration_success = TimingContext::wait_for_with_timeout_us(
                []() { return LL_HRTIM_IsActiveFlag_DLLRDY(ControllerTraitsT::skInstance) != 0; }, 100);

            if (!calibration_success)
            {
                VALLE_LOG_ERROR("HRTIM{} DLL Calibration timed out!", tkControllerID);
                return false;
            }

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
    struct HRTIMFaultCTDefaultConfig
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
        static constexpr auto skConfig = HRTIMFaultCTDefaultConfig{};
    };

#define VALLE_DEFINE_HRTIM_FAULT_CT_CONFIG(tkControllerID, tkFaultID, config)                          \
    namespace valle::platform                                                                          \
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
    struct HRTIMEEVCTDefaultConfig
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
        static constexpr auto skConfig = HRTIMEEVCTDefaultConfig{};
    };

#define VALLE_DEFINE_HRTIM_EEV_CT_CONFIG(tkControllerID, tkEEVID, config)                          \
    namespace valle::platform                                                                      \
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

    struct HRTIMTimerCTDefaultConfig
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
        static constexpr auto skConfig = HRTIMTimerCTDefaultConfig{};
    };

#define VALLE_DEFINE_HRTIM_TIMER_CT_CONFIG(tkControllerID, tkTimerID, config)                          \
    namespace valle::platform                                                                          \
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
        HRTIMTimerRolloverMode rollover_mode = HRTIMTimerRolloverMode::kPeriodReset;
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

    struct HRTIMTimerDeadTimeConfig
    {
        float rise_ns = 0.0F;  // Rising deadtime in nanoseconds
        float fall_ns = 0.0F;  // Falling deadtime in nanoseconds

        // RM0440 Section 29.3.12: DTFLK/DTRLK bits can only be written once after reset
        // Once set, they cannot be cleared until MCU reset
        // Default true for safety, but allow dynamic deadtime adjustment if needed
        bool lock_registers = true;
    };

    template <HRTIMTimerID tkTimerID, HRTIMTimerADCTriggerID tkTriggerID>
    struct HRTIMTimerADCTriggerConfig
    {
        static constexpr HRTIMTimerID           skTimerID   = tkTimerID;
        static constexpr HRTIMTimerADCTriggerID skTriggerID = tkTriggerID;

        HRTIMTimerADCTriggerSourceID<tkTimerID, tkTriggerID> source{};
        HRTIMTimerADCRolloverMode                            rollover_mode = HRTIMTimerADCRolloverMode::kPeriodReset;
        uint8_t                                              post_scaler   = 0;  // Between 0 and 31
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
        using InterfaceT           = HRTIMTimerInterface<tkControllerID, tkTimerID>;
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
                                                         RCCInfoDevice<>,
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
        [[no_unique_address]] DeviceRef<ControllerT>     m_controller;
        [[no_unique_address]] DeviceRef<RCCInfoDevice<>> m_rcc_info;

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
            , m_rcc_info(extract_device_ref<true, RCCInfoDevice<>>(std::forward<TArgs>(args)...))
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
            LL_HRTIM_TIM_SetRollOverMode(
                ControllerTraitsT::skInstance, TimerTraitsT::skLLID, static_cast<uint32_t>(config.rollover_mode));
            LL_HRTIM_TIM_SetPeriod(ControllerTraitsT::skInstance, TimerTraitsT::skLLID, config.period);
            LL_HRTIM_TIM_SetRepetition(ControllerTraitsT::skInstance, TimerTraitsT::skLLID, config.repetition);
            return true;
        }

        void enable_counter()
        {
            InterfaceT::enable_counter();
        }

        void disable_counter()
        {
            InterfaceT::disable_counter();
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

        void enable_output1()
        {
            InterfaceT::enable_output1();
        }

        void disable_output1()
        {
            InterfaceT::disable_output1();
        }

        void enable_output2()
        {
            InterfaceT::enable_output2();
        }

        void disable_output2()
        {
            InterfaceT::disable_output2();
        }

        void enable_outputs()
        {
            InterfaceT::enable_outputs();
        }

        void disable_outputs()
        {
            InterfaceT::disable_outputs();
        }

        [[nodiscard]] bool init_deadtime(const HRTIMTimerDeadTimeConfig& config)
        {
            // Validate deadtime against hardware limits
            // RM0440 Section 29.3.12: Max deadtime = 511 × tDTG where tDTG = tHRCK × 2^DTPRSC
            // At 170 MHz with max prescaler (7): 511 × (1/(8×170MHz)) × 128 ≈ 48 µs
            const uint32_t f_hrtim_hz = get_freq_hz();

            const float min_requested_ns = std::min<float>(config.fall_ns, config.rise_ns);
            const float max_requested_ns = std::max<float>(config.fall_ns, config.rise_ns);

            const float min_hw_deadtime_ns = InterfaceT::calculate_min_deadtime_ns(f_hrtim_hz);
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

            const float max_hw_deadtime_ns = InterfaceT::calculate_max_deadtime_ns(f_hrtim_hz);
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
                InterfaceT::calculate_deadtime_prescaler(f_hrtim_hz, max_requested_ns);
            const uint32_t rising_counts =
                InterfaceT::calculate_deadtime_count(f_hrtim_hz, prescaler_val, config.rise_ns);
            const uint32_t falling_counts =
                InterfaceT::calculate_deadtime_count(f_hrtim_hz, prescaler_val, config.fall_ns);

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
        // INFO
        // ------------------------------------------------------------------------
        [[nodiscard]] uint32_t get_freq_hz()
        {
            return m_rcc_info->get_apb2_timer_freq_hz();
        }

        // ------------------------------------------------------------------------
        // ADC
        // ------------------------------------------------------------------------

        /**
         * @brief Initializes an ADC trigger for the timer. This allows the timer to trigger ADC conversions at specific
         * points in the PWM cycle, which is critical for synchronized sampling in motor control applications.
         *
         * @tparam tkTriggerID The ID of the ADC trigger to configure (e.g., kTrig1, kTrig2, etc.). Each timer has
         * multiple trigger outputs that can be independently configured.
         * @param config The configuration for the ADC trigger, including the source event (e.g., timer period, compare
         * match, etc.) and the rollover mode (when the trigger should occur in up-down counting).
         * @return true if the ADC trigger was successfully initialized, false otherwise (e.g., invalid trigger ID,
         * incompatible configuration, etc.).
         * @return false if the trigger ID is invalid or if the configuration is incompatible with the timer's counting
         * mode. For example, certain trigger sources may not be valid in center-aligned mode, and the function should
         * validate this before applying the configuration.
         */
        template <HRTIMTimerADCTriggerID tkTriggerID>
        [[nodiscard]] bool init_adc_trigger(const HRTIMTimerADCTriggerConfig<tkTimerID, tkTriggerID>& config)
        {
            LL_HRTIM_SetADCTrigUpdate(
                ControllerTraitsT::skInstance, static_cast<uint32_t>(tkTriggerID), TimerTraitsT::skADCTrigUpdateID);

            LL_HRTIM_SetADCTrigSrc(ControllerTraitsT::skInstance,
                                   static_cast<uint32_t>(tkTriggerID),
                                   static_cast<uint32_t>(config.source));

            LL_HRTIM_SetADCPostScaler(ControllerTraitsT::skInstance,
                                      static_cast<uint32_t>(tkTriggerID),
                                      static_cast<uint32_t>(config.post_scaler));

            LL_HRTIM_TIM_SetADCRollOverMode(
                ControllerTraitsT::skInstance, TimerTraitsT::skLLID, static_cast<uint32_t>(config.rollover_mode));

            return true;
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

}  // namespace valle::platform