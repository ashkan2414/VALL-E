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
    // HRTIM CONTROLLER DEVICE (SHARED DEVICE)
    // =============================================================================

    // -----------------------------------------------------------------------------
    // COMPILE TimE CONFIGURATIONS
    // -----------------------------------------------------------------------------
    struct HrtimControllerCTDefaultConfig
    {
        using SCInPinT  = GpioNullPinDevice;
        using SCOutPinT = GpioNullPinDevice;
    };

    template <HrtimPeripheralId tkPeripheralId, HrtimControllerGpioPinType tkPinType>
    using HrtimControllerDefaultPinDevice = HrtimControllerPinMap<tkPeripheralId, tkPinType, 0>;

    template <HrtimPeripheralId tkPeripheralId, HrtimControllerGpioPinType tkPinType, typename TPinDevice>
    concept CValidHrtimControllerPinDevice =
        CGpioPinDevice<TPinDevice> &&
        CValidHrtimControllerPin<tkPeripheralId, tkPinType, TPinDevice::skPortId, TPinDevice::skPinId>;

    template <HrtimPeripheralId tkPeripheralId, HrtimControllerGpioPinType tkPinType, typename TPinDevice>
    concept CValidHrtimControllerPinCTConfig =
        CNullGpioPinDevice<TPinDevice> || CValidHrtimControllerPinDevice<tkPeripheralId, tkPinType, TPinDevice>;

    template <typename T, HrtimPeripheralId tkPeripheralId>
    concept CValidHrtimControllerCTConfig =
        requires {
            typename T::SCInPinT;
            typename T::SCOutPinT;
        } &&
        (CValidHrtimControllerPinCTConfig<tkPeripheralId, HrtimControllerGpioPinType::kSCIn, typename T::SCInPinT>) &&
        (CValidHrtimControllerPinCTConfig<tkPeripheralId, HrtimControllerGpioPinType::kSCOut, typename T::SCOutPinT>);

    template <HrtimPeripheralId tkPeripheralId>
    struct HrtimControllerCTConfigRegistry
    {
        static constexpr auto skConfig = HrtimControllerCTDefaultConfig{};
    };

#define VALLE_DEFINE_HRTIM_CONTROLLER_CT_CONFIG(tkPeripheralId, config)                        \
    namespace valle::platform                                                                  \
    {                                                                                          \
        template <>                                                                            \
        struct HrtimControllerCTConfigRegistry<(tkPeripheralId)>                               \
        {                                                                                      \
            static constexpr auto skConfig = (config);                                         \
            static_assert(CValidHrtimControllerCTConfig<decltype(skConfig), (tkPeripheralId)>, \
                          "Invalid HRTIM Controller CT Config");                               \
        };                                                                                     \
    }

    // -----------------------------------------------------------------------------
    // HELPERS
    // -----------------------------------------------------------------------------

    namespace detail
    {
        template <HrtimPeripheralId tkPeripheralId, HrtimControllerGpioPinType tkPinType, typename TPinDevice>
        struct HrtimControllerGpioPinDriverHelper
        {
            using type = GpioAlternateFunctionDriver<
                TPinDevice,
                kHrtimControllerPinAF<tkPeripheralId, tkPinType, TPinDevice::skPortId, TPinDevice::skPinId>>;
        };

        template <HrtimPeripheralId tkPeripheralId, HrtimControllerGpioPinType tkPinType>
        struct HrtimControllerGpioPinDriverHelper<tkPeripheralId, tkPinType, GpioNullPinDevice>
        {
            using type = std::monostate;
        };

    }  // namespace detail

    template <HrtimPeripheralId tkPeripheralId, HrtimControllerGpioPinType tkPinType, typename TPinDevice>
    using HrtimControllerGpioPinDriver =
        typename detail::HrtimControllerGpioPinDriverHelper<tkPeripheralId, tkPinType, TPinDevice>::type;

    // -----------------------------------------------------------------------------
    // DEVICE CLASS
    // -----------------------------------------------------------------------------

    template <HrtimPeripheralId tkPeripheralId>
    class HrtimControllerDevice
    {
    public:
        struct Descriptor : public SharedDeviceDescriptor
        {
        };
        static constexpr HrtimPeripheralId skPeripheralId = tkPeripheralId;

        using ControllerTraitsT          = HrtimControllerTraits<tkPeripheralId>;
        using CTConfigRegistryT          = HrtimControllerCTConfigRegistry<tkPeripheralId>;
        static constexpr auto skCTConfig = CTConfigRegistryT::skConfig;
        using CTConfigT                  = decltype(skCTConfig);

        using SCInPinT  = typename CTConfigT::SCInPinT;
        using SCOutPinT = typename CTConfigT::SCOutPinT;

        static constexpr bool skHasSCInPin  = !CNullGpioPinDevice<SCInPinT>;
        static constexpr bool skHasSCOutPin = !CNullGpioPinDevice<SCOutPinT>;

        using SCInPinDriverT =
            HrtimControllerGpioPinDriver<tkPeripheralId, HrtimControllerGpioPinType::kSCIn, SCInPinT>;
        using SCOutPinDriverT =
            HrtimControllerGpioPinDriver<tkPeripheralId, HrtimControllerGpioPinType::kSCOut, SCOutPinT>;

        using InjectDevices = FilterNullDevices<TypeList<SCInPinT, SCOutPinT>>;

    private:
        SCInPinDriverT  m_scin_pin;
        SCOutPinDriverT m_scout_pin;

    public:
        template <typename... TArgs>
        explicit HrtimControllerDevice(TArgs&&... args)
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
                VALLE_LOG_ERROR("HRTIM{} DLL Calibration timed out!", kHrtimControllerNumFromId<tkPeripheralId>);
                return false;
            }

            return true;
        }
    };

    // -----------------------------------------------------------------------------
    // DEVICE ALIASES
    // -----------------------------------------------------------------------------

    using Hrtim1ControllerDevice = HrtimControllerDevice<HrtimPeripheralId::kHrtim1>;

    // =============================================================================
    // HRTIM FAULT DEVICE (SHARED DEVICE)
    // =============================================================================

    // -----------------------------------------------------------------------------
    // COMPILE TimE CONFIGURATIONS
    // -----------------------------------------------------------------------------
    struct HrtimFaultCTDefaultConfig
    {
        using PinT = GpioNullPinDevice;
    };

    template <HrtimPeripheralId tkPeripheralId, HrtimFaultId tkFaultId, uint8_t tkOutputIdx = 0>
    using HrtimFaultPinDeviceMap = GpioPinDevice<HrtimFaultPinMap<tkPeripheralId, tkFaultId, tkOutputIdx>::skPortId,
                                                 HrtimFaultPinMap<tkPeripheralId, tkFaultId, tkOutputIdx>::skPinId>;

    template <HrtimPeripheralId tkPeripheralId, HrtimFaultId tkFaultId>
    using HrtimFaultDefaultPinDevice = HrtimFaultPinDeviceMap<tkPeripheralId, tkFaultId, 0>;

    template <HrtimPeripheralId tkPeripheralId, HrtimFaultId tkFaultId, typename TPinDevice>
    concept CValidHrtimFaultPinDevice =
        CGpioPinDevice<TPinDevice> &&
        CValidHrtimFaultPin<tkPeripheralId, tkFaultId, TPinDevice::skPortId, TPinDevice::skPinId>;

    template <typename T, HrtimPeripheralId tkPeripheralId, HrtimFaultId tkFaultId>
    concept CValidHrtimFaultCTConfig =
        requires { typename T::PinT; } && (CNullGpioPinDevice<typename T::PinT> ||
                                           CValidHrtimFaultPinDevice<tkPeripheralId, tkFaultId, typename T::PinT>);

    template <HrtimPeripheralId tkPeripheralId, HrtimFaultId tkFaultId>
    struct HrtimFaultCTConfigRegistry
    {
        static constexpr auto skConfig = HrtimFaultCTDefaultConfig{};
    };

#define VALLE_DEFINE_HRTIM_FAULT_CT_CONFIG(tkPeripheralId, tkFaultId, config)                          \
    namespace valle::platform                                                                          \
    {                                                                                                  \
        template <>                                                                                    \
        struct HrtimFaultCTConfigRegistry<(tkPeripheralId), (tkFaultId)>                               \
        {                                                                                              \
            static constexpr auto skConfig = (config);                                                 \
            static_assert(CValidHrtimFaultCTConfig<decltype(skConfig), (tkPeripheralId), (tkFaultId)>, \
                          "Invalid HRTIM Fault CT Config");                                            \
        };                                                                                             \
    }

    // -----------------------------------------------------------------------------
    // CONFIGURATIONS
    // -----------------------------------------------------------------------------
    struct HrtimFaultConfig
    {
        HrtimFaultSource                        source             = HrtimFaultSource::kDigitalInput;
        HrtimFaultPolarity                      polarity           = HrtimFaultPolarity::kHigh;
        HrtimFaultFilter                        filter             = HrtimFaultFilter::kNone;
        HrtimFaultPrescaler                     prescaler          = HrtimFaultPrescaler::kDiv1;
        std::optional<HrtimFaultBlankingSource> blanking_source    = std::nullopt;
        HrtimFaultResetMode                     reset_mode         = HrtimFaultResetMode::kUnconditional;
        uint32_t                                counting_threshold = 1;  // Fault counting threshold
        bool                                    lock               = true;
    };

    // -----------------------------------------------------------------------------
    // HELPERS
    // -----------------------------------------------------------------------------

    namespace detail
    {
        template <HrtimPeripheralId tkPeripheralId, HrtimFaultId tkFaultId, typename TPinDevice>
        struct HrtimFaultGpioPinDriverHelper
        {
            using type = GpioAlternateFunctionDriver<
                TPinDevice,
                kHrtimFaultPinAF<tkPeripheralId, tkFaultId, TPinDevice::skPortId, TPinDevice::skPinId>>;
        };

        template <HrtimPeripheralId tkPeripheralId, HrtimFaultId tkFaultId>
        struct HrtimFaultGpioPinDriverHelper<tkPeripheralId, tkFaultId, GpioNullPinDevice>
        {
            using type = std::monostate;
        };

    }  // namespace detail

    template <HrtimPeripheralId tkPeripheralId, HrtimFaultId tkFaultId, typename TPinDevice>
    using HrtimFaultGpioPinDriver =
        typename detail::HrtimFaultGpioPinDriverHelper<tkPeripheralId, tkFaultId, TPinDevice>::type;

    // -----------------------------------------------------------------------------
    // DEVICE CLASS
    // -----------------------------------------------------------------------------

    template <HrtimPeripheralId tkPeripheralId, HrtimFaultId tkFaultId>
    class HrtimFaultDevice
    {
    public:
        struct Descriptor : public SharedDeviceDescriptor
        {
        };

        static constexpr HrtimPeripheralId skPeripheralId = tkPeripheralId;
        static constexpr HrtimFaultId      skFaultId      = tkFaultId;

        using ControllerT       = HrtimControllerDevice<tkPeripheralId>;
        using ControllerTraitsT = HrtimControllerTraits<tkPeripheralId>;
        using FaultTraitsT      = HrtimFaultTraits<tkPeripheralId, skFaultId>;

        using CTConfigRegistryT          = HrtimFaultCTConfigRegistry<tkPeripheralId, skFaultId>;
        static constexpr auto skCTConfig = CTConfigRegistryT::skConfig;
        using CTConfigT                  = decltype(skCTConfig);

        using PinT       = typename CTConfigT::PinT;
        using PinDriverT = HrtimFaultGpioPinDriver<tkPeripheralId, skFaultId, PinT>;

        using InjectDevices = FilterNullDevices<TypeList<ControllerT, PinT>>;

    private:
        [[no_unique_address]] DeviceRef<ControllerT> m_controller;
        PinDriverT                                   m_pin;

    public:
        template <typename... TArgs>
        explicit HrtimFaultDevice(TArgs&&... args)
            : m_controller(extract_device_ref<true, ControllerT>(std::forward<TArgs>(args)...))
            , m_pin(extract_device_ref<!CNullGpioPinDevice<PinT>, PinT>(std::forward<TArgs>(args)...))
        {
        }

        // ------------------------------------------------------------------------
        // Initialization
        // ------------------------------------------------------------------------
        [[nodiscard]] inline bool init(const HrtimFaultConfig& config)
        {
            LL_HRTIM_FLT_SetSrc(
                ControllerTraitsT::skInstance, FaultTraitsT::skLLId, static_cast<uint32_t>(config.source));

            LL_HRTIM_FLT_SetPolarity(
                ControllerTraitsT::skInstance, FaultTraitsT::skLLId, static_cast<uint32_t>(config.polarity));

            LL_HRTIM_FLT_SetFilter(
                ControllerTraitsT::skInstance, FaultTraitsT::skLLId, static_cast<uint32_t>(config.filter));

            LL_HRTIM_FLT_SetPrescaler(
                ControllerTraitsT::skInstance, FaultTraitsT::skLLId, static_cast<uint32_t>(config.prescaler));

            if (config.blanking_source.has_value())
            {
                LL_HRTIM_FLT_EnableBlanking(ControllerTraitsT::skInstance, FaultTraitsT::skLLId);
                LL_HRTIM_FLT_SetBlankingSrc(ControllerTraitsT::skInstance,
                                            FaultTraitsT::skLLId,
                                            static_cast<uint32_t>(config.blanking_source.value()));
            }
            else
            {
                LL_HRTIM_FLT_DisableBlanking(ControllerTraitsT::skInstance, FaultTraitsT::skLLId);
            }

            LL_HRTIM_FLT_SetResetMode(
                ControllerTraitsT::skInstance, FaultTraitsT::skLLId, static_cast<uint32_t>(config.reset_mode));
            LL_HRTIM_FLT_SetCountingThreshold(
                ControllerTraitsT::skInstance, FaultTraitsT::skLLId, static_cast<uint32_t>(config.counting_threshold));

            if (config.lock)
            {
                LL_HRTIM_FLT_Lock(ControllerTraitsT::skInstance, FaultTraitsT::skLLId);
            }

            return true;
        }
    };

    class HrtimNullFaultDevice
    {
    public:
        struct Descriptor : public NullDeviceDescriptor
        {
        };
    };

    // -----------------------------------------------------------------------------
    // DEVICE ALIASES
    // -----------------------------------------------------------------------------

    using Hrtim1Fault1Device = HrtimFaultDevice<HrtimPeripheralId::kHrtim1, HrtimFaultId::kFault1>;
    using Hrtim1Fault2Device = HrtimFaultDevice<HrtimPeripheralId::kHrtim1, HrtimFaultId::kFault2>;
    using Hrtim1Fault3Device = HrtimFaultDevice<HrtimPeripheralId::kHrtim1, HrtimFaultId::kFault3>;
    using Hrtim1Fault4Device = HrtimFaultDevice<HrtimPeripheralId::kHrtim1, HrtimFaultId::kFault4>;
    using Hrtim1Fault5Device = HrtimFaultDevice<HrtimPeripheralId::kHrtim1, HrtimFaultId::kFault5>;
    using Hrtim1Fault6Device = HrtimFaultDevice<HrtimPeripheralId::kHrtim1, HrtimFaultId::kFault6>;

    // =============================================================================
    // HRTIM EXTERNAL EVENT DEVICE (SHARED DEVICE)
    // =============================================================================

    // -----------------------------------------------------------------------------
    // COMPILE TimE CONFIGURATIONS
    // -----------------------------------------------------------------------------
    struct HrtimExternalEventCTDefaultConfig
    {
        using PinT = GpioNullPinDevice;
    };

    template <HrtimPeripheralId tkPeripheralId, HrtimExternalEventId tkExternalEventId, uint8_t tkOutputIdx = 0>
    using HrtimExternalEventPinDeviceMap =
        GpioPinDevice<HrtimExternalEventPinMap<tkPeripheralId, tkExternalEventId, tkOutputIdx>::skPortId,
                      HrtimExternalEventPinMap<tkPeripheralId, tkExternalEventId, tkOutputIdx>::skPinId>;

    template <HrtimPeripheralId tkPeripheralId, HrtimExternalEventId tkExternalEventId>
    using HrtimExternalEventDefaultPinDevice = HrtimExternalEventPinDeviceMap<tkPeripheralId, tkExternalEventId, 0>;

    template <HrtimPeripheralId tkPeripheralId, HrtimExternalEventId tkExternalEventId, typename TPinDevice>
    concept CValidHrtimExternalEventPinDevice =
        CGpioPinDevice<TPinDevice> &&
        CValidHrtimExternalEventPin<tkPeripheralId, tkExternalEventId, TPinDevice::skPortId, TPinDevice::skPinId>;

    template <typename T, HrtimPeripheralId tkPeripheralId, HrtimExternalEventId tkExternalEventId>
    concept CValidHrtimExternalEventCTConfig =
        requires { typename T::PinT; } &&
        (CNullGpioPinDevice<typename T::PinT> ||
         CValidHrtimExternalEventPinDevice<tkPeripheralId, tkExternalEventId, typename T::PinT>);

    template <HrtimPeripheralId tkPeripheralId, HrtimExternalEventId tkExternalEventId>
    struct HrtimExternalEventCTConfigRegistry
    {
        static constexpr auto skConfig = HrtimExternalEventCTDefaultConfig{};
    };

#define VALLE_DEFINE_HRTIM_EXTERNAL_EVENT_CT_CONFIG(tkPeripheralId, tkExternalEventId, config)                         \
    namespace valle::platform                                                                                          \
    {                                                                                                                  \
        template <>                                                                                                    \
        struct HrtimExternalEventCTConfigRegistry<(tkPeripheralId), (tkExternalEventId)>                               \
        {                                                                                                              \
            static constexpr auto skConfig = (config);                                                                 \
            static_assert(CValidHrtimExternalEventCTConfig<decltype(skConfig), (tkPeripheralId), (tkExternalEventId)>, \
                          "Invalid HRTIM ExternalEvent CT Config");                                                    \
        };                                                                                                             \
    }

    // -----------------------------------------------------------------------------
    // CONFIGURATIONS
    // -----------------------------------------------------------------------------
    struct HrtimExternalEventConfig
    {
        HrtimExternalEventSource      source      = HrtimExternalEventSource::kSource1;
        HrtimExternalEventPolarity    polarity    = HrtimExternalEventPolarity::kHigh;
        HrtimExternalEventSensitivity sensitivity = HrtimExternalEventSensitivity::kLevel;
        HrtimExternalEventFastMode    fast_mode   = HrtimExternalEventFastMode::kDisable;
        HrtimExternalEventFilter      filter      = HrtimExternalEventFilter::kNone;
        HrtimExternalEventPrescaler   prescaler   = HrtimExternalEventPrescaler::kDiv1;
    };

    // -----------------------------------------------------------------------------
    // HELPERS
    // -----------------------------------------------------------------------------

    namespace detail
    {
        template <HrtimPeripheralId tkPeripheralId, HrtimExternalEventId tkExternalEventId, typename TPinDevice>
        struct HrtimExternalEventGpioPinDriverHelper
        {
            using type = GpioAlternateFunctionDriver<
                TPinDevice,
                kHrtimExternalEventPinAF<tkPeripheralId, tkExternalEventId, TPinDevice::skPortId, TPinDevice::skPinId>>;
        };

        template <HrtimPeripheralId tkPeripheralId, HrtimExternalEventId tkExternalEventId>
        struct HrtimExternalEventGpioPinDriverHelper<tkPeripheralId, tkExternalEventId, GpioNullPinDevice>
        {
            using type = std::monostate;
        };

    }  // namespace detail

    template <HrtimPeripheralId tkPeripheralId, HrtimExternalEventId tkExternalEventId, typename TPinDevice>
    using HrtimExternalEventGpioPinDriver =
        typename detail::HrtimExternalEventGpioPinDriverHelper<tkPeripheralId, tkExternalEventId, TPinDevice>::type;

    // -----------------------------------------------------------------------------
    // DEVICE CLASS
    // -----------------------------------------------------------------------------

    template <HrtimPeripheralId tkPeripheralId, HrtimExternalEventId tkExternalEventId>
    class HrtimExternalEventDevice
    {
    public:
        struct Descriptor : public SharedDeviceDescriptor
        {
        };

        static constexpr HrtimPeripheralId    skPeripheralId    = tkPeripheralId;
        static constexpr HrtimExternalEventId skExternalEventId = tkExternalEventId;

        using ControllerT          = HrtimControllerDevice<tkPeripheralId>;
        using ControllerTraitsT    = HrtimControllerTraits<tkPeripheralId>;
        using ExternalEventTraitsT = HrtimExternalEventTraits<tkPeripheralId, skExternalEventId>;

        using CTConfigRegistryT          = HrtimExternalEventCTConfigRegistry<tkPeripheralId, skExternalEventId>;
        static constexpr auto skCTConfig = CTConfigRegistryT::skConfig;
        using CTConfigT                  = decltype(skCTConfig);

        using PinT       = typename CTConfigT::PinT;
        using PinDriverT = HrtimExternalEventGpioPinDriver<tkPeripheralId, skExternalEventId, PinT>;

        using InjectDevices = FilterNullDevices<TypeList<ControllerT, PinT>>;

    private:
        [[no_unique_address]] DeviceRef<ControllerT> m_controller;
        PinDriverT                                   m_pin;

    public:
        template <typename... TArgs>
        explicit HrtimExternalEventDevice(TArgs&&... args)
            : m_controller(extract_device_ref<true, ControllerT>(std::forward<TArgs>(args)...))
            , m_pin(extract_device_ref<!CNullGpioPinDevice<PinT>, PinT>(std::forward<TArgs>(args)...))
        {
        }

        // ------------------------------------------------------------------------
        // Initialization
        // ------------------------------------------------------------------------
        [[nodiscard]] inline bool init(const HrtimExternalEventConfig& config)
        {
            LL_HRTIM_EE_SetSrc(
                ControllerTraitsT::skInstance, ExternalEventTraitsT::skLLId, static_cast<uint32_t>(config.source));

            LL_HRTIM_EE_SetPolarity(
                ControllerTraitsT::skInstance, ExternalEventTraitsT::skLLId, static_cast<uint32_t>(config.polarity));

            LL_HRTIM_EE_SetSensitivity(
                ControllerTraitsT::skInstance, ExternalEventTraitsT::skLLId, static_cast<uint32_t>(config.sensitivity));

            LL_HRTIM_EE_SetFastMode(
                ControllerTraitsT::skInstance, ExternalEventTraitsT::skLLId, static_cast<uint32_t>(config.fast_mode));

            LL_HRTIM_EE_SetFilter(
                ControllerTraitsT::skInstance, ExternalEventTraitsT::skLLId, static_cast<uint32_t>(config.filter));

            LL_HRTIM_EE_SetPrescaler(
                ControllerTraitsT::skInstance, ExternalEventTraitsT::skLLId, static_cast<uint32_t>(config.prescaler));

            return true;
        }
    };

    class HrtimNullExternalEventDevice
    {
    public:
        struct Descriptor : public NullDeviceDescriptor
        {
        };
    };

    // -----------------------------------------------------------------------------
    // DEVICE ALIASES
    // -----------------------------------------------------------------------------

    using Hrtim1ExternalEvent1Device =
        HrtimExternalEventDevice<HrtimPeripheralId::kHrtim1, HrtimExternalEventId::kExternalEvent1>;
    using Hrtim1ExternalEvent2Device =
        HrtimExternalEventDevice<HrtimPeripheralId::kHrtim1, HrtimExternalEventId::kExternalEvent2>;
    using Hrtim1ExternalEvent3Device =
        HrtimExternalEventDevice<HrtimPeripheralId::kHrtim1, HrtimExternalEventId::kExternalEvent3>;
    using Hrtim1ExternalEvent4Device =
        HrtimExternalEventDevice<HrtimPeripheralId::kHrtim1, HrtimExternalEventId::kExternalEvent4>;
    using Hrtim1ExternalEvent5Device =
        HrtimExternalEventDevice<HrtimPeripheralId::kHrtim1, HrtimExternalEventId::kExternalEvent5>;
    using Hrtim1ExternalEvent6Device =
        HrtimExternalEventDevice<HrtimPeripheralId::kHrtim1, HrtimExternalEventId::kExternalEvent6>;
    using Hrtim1ExternalEvent7Device =
        HrtimExternalEventDevice<HrtimPeripheralId::kHrtim1, HrtimExternalEventId::kExternalEvent7>;
    using Hrtim1ExternalEvent8Device =
        HrtimExternalEventDevice<HrtimPeripheralId::kHrtim1, HrtimExternalEventId::kExternalEvent8>;
    using Hrtim1ExternalEvent9Device =
        HrtimExternalEventDevice<HrtimPeripheralId::kHrtim1, HrtimExternalEventId::kExternalEvent9>;
    using Hrtim1ExternalEvent10Device =
        HrtimExternalEventDevice<HrtimPeripheralId::kHrtim1, HrtimExternalEventId::kExternalEvent10>;

    // =============================================================================
    // HRTIM TIMER DEVICE (UNIQUE DEVICE)
    // =============================================================================

    // ---------------------------------------------------------------------------
    // COMPILE TimE CONFIGURATIONS
    // ---------------------------------------------------------------------------

    struct HrtimTimerCTDefaultConfig
    {
        using Output1PinT = GpioNullPinDevice;
        using Output2PinT = GpioNullPinDevice;

        constexpr static bool skUseFault1 = false;
        constexpr static bool skUseFault2 = false;
        constexpr static bool skUseFault3 = false;
        constexpr static bool skUseFault4 = false;
        constexpr static bool skUseFault5 = false;
        constexpr static bool skUseFault6 = false;

        constexpr static bool skUseExternalEvent1  = false;
        constexpr static bool skUseExternalEvent2  = false;
        constexpr static bool skUseExternalEvent3  = false;
        constexpr static bool skUseExternalEvent4  = false;
        constexpr static bool skUseExternalEvent5  = false;
        constexpr static bool skUseExternalEvent6  = false;
        constexpr static bool skUseExternalEvent7  = false;
        constexpr static bool skUseExternalEvent8  = false;
        constexpr static bool skUseExternalEvent9  = false;
        constexpr static bool skUseExternalEvent10 = false;
    };

    template <HrtimPeripheralId     tkPeripheralId,
              HrtimTimerId          tkTimerId,
              HrtimTimerGpioPinType tkPinType,
              uint8_t               tkOutputIdx = 0>
    using HrtimTimerPinDeviceMap =
        GpioPinDevice<HrtimTimerPinMap<tkPeripheralId, tkTimerId, tkPinType, tkOutputIdx>::skPortId,
                      HrtimTimerPinMap<tkPeripheralId, tkTimerId, tkPinType, tkOutputIdx>::skPinId>;

    template <HrtimPeripheralId tkPeripheralId, HrtimTimerId tkTimerId, HrtimTimerGpioPinType tkPinType>
    using HrtimTimerDefaultPinDevice = HrtimTimerPinDeviceMap<tkPeripheralId, tkTimerId, tkPinType, 0>;

    template <HrtimPeripheralId tkPeripheralId, HrtimTimerId tkTimerId>
    using HrtimTimerDefaultOutput1PinDevice =
        HrtimTimerDefaultPinDevice<tkPeripheralId, tkTimerId, HrtimTimerGpioPinType::kOutput1>;

    template <HrtimPeripheralId tkPeripheralId, HrtimTimerId tkTimerId>
    using HrtimTimerDefaultOutput2PinDevice =
        HrtimTimerDefaultPinDevice<tkPeripheralId, tkTimerId, HrtimTimerGpioPinType::kOutput2>;

    template <HrtimPeripheralId     tkPeripheralId,
              HrtimTimerId          tkTimerId,
              HrtimTimerGpioPinType tkPinType,
              typename TPinDevice>
    concept CValidHrtimTimerPinDevice =
        CGpioPinDevice<TPinDevice> &&
        CValidHrtimTimerPin<tkPeripheralId, tkTimerId, tkPinType, TPinDevice::skPortId, TPinDevice::skPinId>;

    template <HrtimPeripheralId     tkPeripheralId,
              HrtimTimerId          tkTimerId,
              HrtimTimerGpioPinType tkPinType,
              typename TPinDevice>
    concept CValidHrtimTimerPinCTConfig =
        (CNullGpioPinDevice<TPinDevice> || CValidHrtimTimerPinDevice<tkPeripheralId, tkTimerId, tkPinType, TPinDevice>);

    template <typename T, HrtimPeripheralId tkPeripheralId, HrtimTimerId tkTimerId>
    concept CValidHrtimTimerCTConfig = requires { typename T::Output1PinT; } &&
                                       (CValidHrtimTimerPinCTConfig<tkPeripheralId,
                                                                    tkTimerId,
                                                                    HrtimTimerGpioPinType::kOutput1,
                                                                    typename T::Output1PinT>) &&
                                       (CValidHrtimTimerPinCTConfig<tkPeripheralId,
                                                                    tkTimerId,
                                                                    HrtimTimerGpioPinType::kOutput2,
                                                                    typename T::Output2PinT>) &&
                                       std::is_same_v<std::remove_cvref_t<decltype(T::skUseFault1)>, bool> &&
                                       std::is_same_v<std::remove_cvref_t<decltype(T::skUseFault2)>, bool> &&
                                       std::is_same_v<std::remove_cvref_t<decltype(T::skUseFault3)>, bool> &&
                                       std::is_same_v<std::remove_cvref_t<decltype(T::skUseFault4)>, bool> &&
                                       std::is_same_v<std::remove_cvref_t<decltype(T::skUseFault5)>, bool> &&
                                       std::is_same_v<std::remove_cvref_t<decltype(T::skUseFault6)>, bool> &&
                                       std::is_same_v<std::remove_cvref_t<decltype(T::skUseExternalEvent1)>, bool> &&
                                       std::is_same_v<std::remove_cvref_t<decltype(T::skUseExternalEvent2)>, bool> &&
                                       std::is_same_v < std::remove_cvref_t<decltype(T::skUseExternalEvent3)>,
            booExternalEvent&&      std::is_same_v<std::remove_cvref_t<decltype(T::skUseExternalEvent4)>, bool>&&
                                    std::is_same_v<std::remove_cvref_t<decltype(T::skUseExternalEvent5)>, bool>&&
                                    std::is_same_v<std::remove_cvref_t<decltype(T::skUseExternalEvent6)>, bool>&&
                                    std::is_same_v<std::remove_cvref_t<decltype(T::skUseExternalEvent7)>, bool>&&
                                    std::is_same_v<std::remove_cvref_t<decltype(T::skUseExternalEvent8)>, bool>&&
                                    std::is_same_v<std::remove_cvref_t<decltype(T::skUseExternalEvent9)>, bool>&&
                                    std::is_same_v<std::remove_cvref_t<decltype(T::skUseExternalEvent10)>, bool>;

    template <HrtimPeripheralId tkPeripheralId, HrtimTimerId tkTimerId>
    struct HrtimTimerCTConfigRegistry
    {
        static constexpr auto skConfig = HrtimTimerCTDefaultConfig{};
    };

#define VALLE_DEFINE_HRTIM_TIMER_CT_CONFIG(tkPeripheralId, tkTimerId, config)                          \
    namespace valle::platform                                                                          \
    {                                                                                                  \
        template <>                                                                                    \
        struct HrtimTimerCTConfigRegistry<(tkPeripheralId), (tkTimerId)>                               \
        {                                                                                              \
            static constexpr auto skConfig = (config);                                                 \
            static_assert(CValidHrtimTimerCTConfig<decltype(skConfig), (tkPeripheralId), (tkTimerId)>, \
                          "Invalid HRTIM Timer CT Config");                                            \
        };                                                                                             \
    }

    // -----------------------------------------------------------------------------
    // CONFIGURATIONS
    // -----------------------------------------------------------------------------
    struct HrtimTimerCounterConfig
    {
        HrtimTimerPrescaler    prescaler     = HrtimTimerPrescaler::kDiv1;
        HrtimTimerCounterMode  counter_mode  = HrtimTimerCounterMode::kContinuous;
        HrtimTimerCountingMode counting_mode = HrtimTimerCountingMode::kUp;
        HrtimTimerRolloverMode rollover_mode = HrtimTimerRolloverMode::kPeriodReset;
        uint16_t               period        = 0xFFFF;
        uint8_t                repetition    = 0;
    };

    struct HrtimTimerVirtualOutputConfig
    {
        HrtimTimerOutputSetSource          set_source            = HrtimTimerOutputSetSource::kNone;
        HrtimTimerOutputResetSource        reset_source          = HrtimTimerOutputResetSource::kNone;
        HrtimTimerOutputPolarity           polarity              = HrtimTimerOutputPolarity::kPositive;
        HrtimTimerOutputIdleMode           idle_mode             = HrtimTimerOutputIdleMode::kNoIdle;
        HrtimTimerOutputIdleLevel          idle_level            = HrtimTimerOutputIdleLevel::kInactive;
        HrtimTimerOutputFaultState         fault_state           = HrtimTimerOutputFaultState::kInactive;
        HrtimTimerOutputChopperMode        chopper_mode          = HrtimTimerOutputChopperMode::kDisabled;
        HrtimTimerOutputBurstModeEntryMode burst_mode_entry_mode = HrtimTimerOutputBurstModeEntryMode::kRegular;
    };

    struct HrtimTimerOutputGpioConfig
    {
        GpioSpeedMode speed = GpioSpeedMode::kHigh;
        GpioPullMode  pull  = GpioPullMode::kNoPull;
    };

    struct HrtimTimerDeadTimeConfig
    {
        float rise_ns = 0.0F;  // Rising deadtime in nanoseconds
        float fall_ns = 0.0F;  // Falling deadtime in nanoseconds

        // RM0440 Section 29.3.12: DTFLK/DTRLK bits can only be written once after reset
        // Once set, they cannot be cleared until MCU reset
        // Default true for safety, but allow dynamic deadtime adjustment if needed
        bool lock_registers = true;
    };

    template <HrtimTimerId tkTimerId, HrtimTimerAdcTriggerId tkTriggerId>
    struct HrtimTimerAdcTriggerConfig
    {
        static constexpr HrtimTimerId           skTimerId   = tkTimerId;
        static constexpr HrtimTimerAdcTriggerId skTriggerId = tkTriggerId;

        HrtimTimerAdcTriggerSourceId<tkTimerId, tkTriggerId> source{};
        HrtimTimerAdcRolloverMode                            rollover_mode = HrtimTimerAdcRolloverMode::kPeriodReset;
        uint8_t                                              post_scaler   = 0;  // Between 0 and 31
    };

    // -----------------------------------------------------------------------------
    // HELPERS
    // -----------------------------------------------------------------------------

    namespace detail
    {
        template <HrtimPeripheralId     tkPeripheralId,
                  HrtimTimerId          tkTimerId,
                  HrtimTimerGpioPinType tkPinType,
                  typename TPinDevice>
        struct HrtimTimerGpioPinDriverHelper
        {
            constexpr static auto skAF =
                kHrtimTimerPinAF<tkPeripheralId, tkTimerId, tkPinType, TPinDevice::skPortId, TPinDevice::skPinId>;
            using type = GpioAlternateFunctionDriver<TPinDevice, skAF>;
        };

        template <HrtimPeripheralId tkPeripheralId, HrtimTimerId tkTimerId, HrtimTimerGpioPinType tkPinType>
        struct HrtimTimerGpioPinDriverHelper<tkPeripheralId, tkTimerId, tkPinType, GpioNullPinDevice>
        {
            using type = std::monostate;
        };

    }  // namespace detail

    template <HrtimPeripheralId     tkPeripheralId,
              HrtimTimerId          tkTimerId,
              HrtimTimerGpioPinType tkPinType,
              typename TPinDevice>
    using HrtimTimerGpioPinDriver =
        typename detail::HrtimTimerGpioPinDriverHelper<tkPeripheralId, tkTimerId, tkPinType, TPinDevice>::type;

    // -----------------------------------------------------------------------------
    // DEVICE CLASS
    // -----------------------------------------------------------------------------

    template <HrtimPeripheralId tkPeripheralId, HrtimTimerId tkTimerId>
    class HrtimTimerDevice
    {
    public:
        struct Descriptor : public UniqueDeviceDescriptor
        {
        };

        static constexpr HrtimPeripheralId skPeripheralId = tkPeripheralId;
        static constexpr HrtimTimerId      skTimerId      = tkTimerId;

        using ControllerT          = HrtimControllerDevice<tkPeripheralId>;
        using ControllerTraitsT    = HrtimControllerTraits<tkPeripheralId>;
        using TimerTraitsT         = HrtimTimerTraits<tkPeripheralId, tkTimerId>;
        using InterfaceT           = HrtimTimerInterface<tkPeripheralId, tkTimerId>;
        using InterruptControllerT = HrtimTimerInterruptController<tkPeripheralId, tkTimerId>;

        using CTConfigRegistryT          = HrtimTimerCTConfigRegistry<tkPeripheralId, tkTimerId>;
        static constexpr auto skCTConfig = CTConfigRegistryT::skConfig;
        using CTConfigT                  = decltype(skCTConfig);

        using Output1PinT = typename CTConfigT::Output1PinT;
        using Output2PinT = typename CTConfigT::Output2PinT;

        static constexpr bool skHasOutput1Pin = !CNullGpioPinDevice<Output1PinT>;
        static constexpr bool skHasOutput2Pin = !CNullGpioPinDevice<Output2PinT>;

        using Output1PinDriverT =
            HrtimTimerGpioPinDriver<tkPeripheralId, tkTimerId, HrtimTimerGpioPinType::kOutput1, Output1PinT>;
        using Output2PinDriverT =
            HrtimTimerGpioPinDriver<tkPeripheralId, tkTimerId, HrtimTimerGpioPinType::kOutput2, Output2PinT>;

        static constexpr bool skHasFault1 = CTConfigT::skUseFault1;
        static constexpr bool skHasFault2 = CTConfigT::skUseFault2;
        static constexpr bool skHasFault3 = CTConfigT::skUseFault3;
        static constexpr bool skHasFault4 = CTConfigT::skUseFault4;
        static constexpr bool skHasFault5 = CTConfigT::skUseFault5;
        static constexpr bool skHasFault6 = CTConfigT::skUseFault6;
        static constexpr bool skHasFaults =
            skHasFault1 || skHasFault2 || skHasFault3 || skHasFault4 || skHasFault5 || skHasFault6;

        static constexpr bool skHasExternalEvent1  = CTConfigT::skUseExternalEvent1;
        static constexpr bool skHasExternalEvent2  = CTConfigT::skUseExternalEvent2;
        static constexpr bool skHasExternalEvent3  = CTConfigT::skUseExternalEvent3;
        static constexpr bool skHasExternalEvent4  = CTConfigT::skUseExternalEvent4;
        static constexpr bool skHasExternalEvent5  = CTConfigT::skUseExternalEvent5;
        static constexpr bool skHasExternalEvent6  = CTConfigT::skUseExternalEvent6;
        static constexpr bool skHasExternalEvent7  = CTConfigT::skUseExternalEvent7;
        static constexpr bool skHasExternalEvent8  = CTConfigT::skUseExternalEvent8;
        static constexpr bool skHasExternalEvent9  = CTConfigT::skUseExternalEvent9;
        static constexpr bool skHasExternalEvent10 = CTConfigT::skUseExternalEvent10;

        using Fault1DeviceT = std::
            conditional_t<skHasFault1, HrtimFaultDevice<tkPeripheralId, HrtimFaultId::kFault1>, HrtimNullFaultDevice>;
        using Fault2DeviceT = std::
            conditional_t<skHasFault2, HrtimFaultDevice<tkPeripheralId, HrtimFaultId::kFault2>, HrtimNullFaultDevice>;
        using Fault3DeviceT = std::
            conditional_t<skHasFault3, HrtimFaultDevice<tkPeripheralId, HrtimFaultId::kFault3>, HrtimNullFaultDevice>;
        using Fault4DeviceT = std::
            conditional_t<skHasFault4, HrtimFaultDevice<tkPeripheralId, HrtimFaultId::kFault4>, HrtimNullFaultDevice>;
        using Fault5DeviceT = std::
            conditional_t<skHasFault5, HrtimFaultDevice<tkPeripheralId, HrtimFaultId::kFault5>, HrtimNullFaultDevice>;
        using Fault6DeviceT = std::
            conditional_t<skHasFault6, HrtimFaultDevice<tkPeripheralId, HrtimFaultId::kFault6>, HrtimNullFaultDevice>;

        using ExternalEvent1DeviceT =
            std::conditional_t<skHasExternalEvent1,
                               HrtimExternalEventDevice<tkPeripheralId, HrtimExternalEventId::kExternalEvent1>,
                               HrtimNullExternalEventDevice>;
        using ExternalEvent2DeviceT =
            std::conditional_t<skHasExternalEvent2,
                               HrtimExternalEventDevice<tkPeripheralId, HrtimExternalEventId::kExternalEvent2>,
                               HrtimNullExternalEventDevice>;
        using ExternalEvent3DeviceT =
            std::conditional_t<skHasExternalEvent3,
                               HrtimExternalEventDevice<tkPeripheralId, HrtimExternalEventId::kExternalEvent3>,
                               HrtimNullExternalEventDevice>;
        using ExternalEvent4DeviceT =
            std::conditional_t<skHasExternalEvent4,
                               HrtimExternalEventDevice<tkPeripheralId, HrtimExternalEventId::kExternalEvent4>,
                               HrtimNullExternalEventDevice>;
        using ExternalEvent5DeviceT =
            std::conditional_t<skHasExternalEvent5,
                               HrtimExternalEventDevice<tkPeripheralId, HrtimExternalEventId::kExternalEvent5>,
                               HrtimNullExternalEventDevice>;
        using ExternalEvent6DeviceT =
            std::conditional_t<skHasExternalEvent6,
                               HrtimExternalEventDevice<tkPeripheralId, HrtimExternalEventId::kExternalEvent6>,
                               HrtimNullExternalEventDevice>;
        using ExternalEvent7DeviceT =
            std::conditional_t<skHasExternalEvent7,
                               HrtimExternalEventDevice<tkPeripheralId, HrtimExternalEventId::kExternalEvent7>,
                               HrtimNullExternalEventDevice>;
        using ExternalEvent8DeviceT =
            std::conditional_t<skHasExternalEvent8,
                               HrtimExternalEventDevice<tkPeripheralId, HrtimExternalEventId::kExternalEvent8>,
                               HrtimNullExternalEventDevice>;
        using ExternalEvent9DeviceT =
            std::conditional_t<skHasExternalEvent9,
                               HrtimExternalEventDevice<tkPeripheralId, HrtimExternalEventId::kExternalEvent9>,
                               HrtimNullExternalEventDevice>;
        using ExternalEvent10DeviceT =
            std::conditional_t<skHasExternalEvent10,
                               HrtimExternalEventDevice<tkPeripheralId, HrtimExternalEventId::kExternalEvent10>,
                               HrtimNullExternalEventDevice>;

        using InjectDevices = FilterNullDevices<TypeList<ControllerT,
                                                         RccInfoDevice<>,
                                                         Output1PinT,
                                                         Output2PinT,
                                                         Fault1DeviceT,
                                                         Fault2DeviceT,
                                                         Fault3DeviceT,
                                                         Fault4DeviceT,
                                                         Fault5DeviceT,
                                                         Fault6DeviceT,
                                                         ExternalEvent1DeviceT,
                                                         ExternalEvent2DeviceT,
                                                         ExternalEvent3DeviceT,
                                                         ExternalEvent4DeviceT,
                                                         ExternalEvent5DeviceT,
                                                         ExternalEvent6DeviceT,
                                                         ExternalEvent7DeviceT,
                                                         ExternalEvent8DeviceT,
                                                         ExternalEvent9DeviceT,
                                                         ExternalEvent10DeviceT>>;

    private:
        [[no_unique_address]] DeviceRef<ControllerT>     m_controller;
        [[no_unique_address]] DeviceRef<RccInfoDevice<>> m_rcc_info;

        Output1PinDriverT                                                                        m_output1_pin;
        Output2PinDriverT                                                                        m_output2_pin;
        [[no_unique_address]] ConditionalDeviceRef<skHasFault1, Fault1DeviceT>                   m_fault1_device;
        [[no_unique_address]] ConditionalDeviceRef<skHasFault2, Fault2DeviceT>                   m_fault2_device;
        [[no_unique_address]] ConditionalDeviceRef<skHasFault3, Fault3DeviceT>                   m_fault3_device;
        [[no_unique_address]] ConditionalDeviceRef<skHasFault4, Fault4DeviceT>                   m_fault4_device;
        [[no_unique_address]] ConditionalDeviceRef<skHasFault5, Fault5DeviceT>                   m_fault5_device;
        [[no_unique_address]] ConditionalDeviceRef<skHasFault6, Fault6DeviceT>                   m_fault6_device;
        [[no_unique_address]] ConditionalDeviceRef<skHasExternalEvent1, ExternalEvent1DeviceT>   m_eev1_device;
        [[no_unique_address]] ConditionalDeviceRef<skHasExternalEvent2, ExternalEvent2DeviceT>   m_eev2_device;
        [[no_unique_address]] ConditionalDeviceRef<skHasExternalEvent3, ExternalEvent3DeviceT>   m_eev3_device;
        [[no_unique_address]] ConditionalDeviceRef<skHasExternalEvent4, ExternalEvent4DeviceT>   m_eev4_device;
        [[no_unique_address]] ConditionalDeviceRef<skHasExternalEvent5, ExternalEvent5DeviceT>   m_eev5_device;
        [[no_unique_address]] ConditionalDeviceRef<skHasExternalEvent6, ExternalEvent6DeviceT>   m_eev6_device;
        [[no_unique_address]] ConditionalDeviceRef<skHasExternalEvent7, ExternalEvent7DeviceT>   m_eev7_device;
        [[no_unique_address]] ConditionalDeviceRef<skHasExternalEvent8, ExternalEvent8DeviceT>   m_eev8_device;
        [[no_unique_address]] ConditionalDeviceRef<skHasExternalEvent9, ExternalEvent9DeviceT>   m_eev9_device;
        [[no_unique_address]] ConditionalDeviceRef<skHasExternalEvent10, ExternalEvent10DeviceT> m_eev10_device;

    public:
        template <typename... TArgs>
        explicit HrtimTimerDevice(TArgs&&... args)
            : m_controller(extract_device_ref<true, ControllerT>(std::forward<TArgs>(args)...))
            , m_rcc_info(extract_device_ref<true, RccInfoDevice<>>(std::forward<TArgs>(args)...))
            , m_output1_pin(extract_device_ref<skHasOutput1Pin, Output1PinT>(std::forward<TArgs>(args)...))
            , m_output2_pin(extract_device_ref<skHasOutput2Pin, Output2PinT>(std::forward<TArgs>(args)...))
            , m_fault1_device(extract_device_ref<skHasFault1, Fault1DeviceT>(std::forward<TArgs>(args)...))
            , m_fault2_device(extract_device_ref<skHasFault2, Fault2DeviceT>(std::forward<TArgs>(args)...))
            , m_fault3_device(extract_device_ref<skHasFault3, Fault3DeviceT>(std::forward<TArgs>(args)...))
            , m_fault4_device(extract_device_ref<skHasFault4, Fault4DeviceT>(std::forward<TArgs>(args)...))
            , m_fault5_device(extract_device_ref<skHasFault5, Fault5DeviceT>(std::forward<TArgs>(args)...))
            , m_fault6_device(extract_device_ref<skHasFault6, Fault6DeviceT>(std::forward<TArgs>(args)...))
            , m_eev1_device(
                  extract_device_ref<skHasExternalEvent1, ExternalEvent1DeviceT>(std::forward<TArgs>(args)...))
            , m_eev2_device(
                  extract_device_ref<skHasExternalEvent2, ExternalEvent2DeviceT>(std::forward<TArgs>(args)...))
            , m_eev3_device(
                  extract_device_ref<skHasExternalEvent3, ExternalEvent3DeviceT>(std::forward<TArgs>(args)...))
            , m_eev4_device(
                  extract_device_ref<skHasExternalEvent4, ExternalEvent4DeviceT>(std::forward<TArgs>(args)...))
            , m_eev5_device(
                  extract_device_ref<skHasExternalEvent5, ExternalEvent5DeviceT>(std::forward<TArgs>(args)...))
            , m_eev6_device(
                  extract_device_ref<skHasExternalEvent6, ExternalEvent6DeviceT>(std::forward<TArgs>(args)...))
            , m_eev7_device(
                  extract_device_ref<skHasExternalEvent7, ExternalEvent7DeviceT>(std::forward<TArgs>(args)...))
            , m_eev8_device(
                  extract_device_ref<skHasExternalEvent8, ExternalEvent8DeviceT>(std::forward<TArgs>(args)...))
            , m_eev9_device(
                  extract_device_ref<skHasExternalEvent9, ExternalEvent9DeviceT>(std::forward<TArgs>(args)...))
            , m_eev10_device(
                  extract_device_ref<skHasExternalEvent10, ExternalEvent10DeviceT>(std::forward<TArgs>(args)...))
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
                    fault_sources |= static_cast<uint32_t>(HrtimFaultId::kFault1);
                }

                if constexpr (skHasFault2)
                {
                    fault_sources |= static_cast<uint32_t>(HrtimFaultId::kFault2);
                }

                if constexpr (skHasFault3)
                {
                    fault_sources |= static_cast<uint32_t>(HrtimFaultId::kFault3);
                }

                if constexpr (skHasFault4)
                {
                    fault_sources |= static_cast<uint32_t>(HrtimFaultId::kFault4);
                }

                if constexpr (skHasFault5)
                {
                    fault_sources |= static_cast<uint32_t>(HrtimFaultId::kFault5);
                }

                if constexpr (skHasFault6)
                {
                    fault_sources |= static_cast<uint32_t>(HrtimFaultId::kFault6);
                }

                LL_HRTIM_TIM_EnableFault(ControllerTraitsT::skInstance, TimerTraitsT::skLLId, fault_sources);
            }

            return true;
        }

        // ------------------------------------------------------------------------
        // Counter
        // ------------------------------------------------------------------------

        [[nodiscard]] bool init_counter(const HrtimTimerCounterConfig& config)
        {
            LL_HRTIM_TIM_SetPrescaler(
                ControllerTraitsT::skInstance, TimerTraitsT::skLLId, static_cast<uint32_t>(config.prescaler));
            LL_HRTIM_TIM_SetCounterMode(
                ControllerTraitsT::skInstance, TimerTraitsT::skLLId, static_cast<uint32_t>(config.counter_mode));
            LL_HRTIM_TIM_SetCountingMode(
                ControllerTraitsT::skInstance, TimerTraitsT::skLLId, static_cast<uint32_t>(config.counting_mode));
            LL_HRTIM_TIM_SetRollOverMode(
                ControllerTraitsT::skInstance, TimerTraitsT::skLLId, static_cast<uint32_t>(config.rollover_mode));
            LL_HRTIM_TIM_SetPeriod(ControllerTraitsT::skInstance, TimerTraitsT::skLLId, config.period);
            LL_HRTIM_TIM_SetRepetition(ControllerTraitsT::skInstance, TimerTraitsT::skLLId, config.repetition);
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

        [[nodiscard]] bool init_output1_virtual(const HrtimTimerVirtualOutputConfig& config)
        {
            return init_output<1>(config);
        }

        [[nodiscard]] bool init_output1(const HrtimTimerVirtualOutputConfig& config,
                                        const HrtimTimerOutputGpioConfig&    gpio_config)
            requires(skHasOutput1Pin)
        {
            if (!init_output<1>(config))
            {
                return false;
            }

            return init_output_gpio<1>(gpio_config);
        }

        [[nodiscard]] bool init_output2_virtual(const HrtimTimerVirtualOutputConfig& config)
        {
            return init_output<2>(config);
        }

        [[nodiscard]] bool init_output2(const HrtimTimerVirtualOutputConfig& config,
                                        const HrtimTimerOutputGpioConfig&    gpio_config)
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

        [[nodiscard]] bool init_deadtime(const HrtimTimerDeadTimeConfig& config)
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
                                static_cast<int>(tkPeripheralId),
                                enum_name(tkTimerId),
                                min_requested_ns,
                                min_hw_deadtime_ns);
                return false;
            }

            const float max_hw_deadtime_ns = InterfaceT::calculate_max_deadtime_ns(f_hrtim_hz);
            if (max_requested_ns > max_hw_deadtime_ns)
            {
                // ERROR: Requested deadtime exceeds hardware capability
                VALLE_LOG_ERROR("HRTIM{} Timer {} requested deadtime ({} ns) exceeds hardware limit ({} ns)!",
                                static_cast<int>(tkPeripheralId),
                                enum_name(tkTimerId),
                                max_requested_ns,
                                max_hw_deadtime_ns);
                return false;
            }

            const HrtimTimerDeadtimePrescaler prescaler_val =
                InterfaceT::calculate_deadtime_prescaler(f_hrtim_hz, max_requested_ns);
            const uint32_t rising_counts =
                InterfaceT::calculate_deadtime_count(f_hrtim_hz, prescaler_val, config.rise_ns);
            const uint32_t falling_counts =
                InterfaceT::calculate_deadtime_count(f_hrtim_hz, prescaler_val, config.fall_ns);

            LL_HRTIM_DT_SetPrescaler(
                ControllerTraitsT::skInstance, TimerTraitsT::skLLId, static_cast<uint32_t>(prescaler_val));

            LL_HRTIM_DT_SetRisingValue(ControllerTraitsT::skInstance, TimerTraitsT::skLLId, rising_counts);
            LL_HRTIM_DT_SetFallingValue(ControllerTraitsT::skInstance, TimerTraitsT::skLLId, falling_counts);

            // RM0440 Section 29.3.12: Lock bits are write-once, cannot be cleared without MCU reset
            if (config.lock_registers)
            {
                LL_HRTIM_DT_LockFalling(ControllerTraitsT::skInstance, TimerTraitsT::skLLId);
                LL_HRTIM_DT_LockRising(ControllerTraitsT::skInstance, TimerTraitsT::skLLId);
            }

            LL_HRTIM_TIM_EnableDeadTime(ControllerTraitsT::skInstance, TimerTraitsT::skLLId);
            return true;
        }

        static inline void disable_deadtime()
        {
            LL_HRTIM_DT_SetRisingValue(ControllerTraitsT::skInstance, TimerTraitsT::skLLId, 0);
            LL_HRTIM_DT_SetFallingValue(ControllerTraitsT::skInstance, TimerTraitsT::skLLId, 0);
            LL_HRTIM_TIM_DisableDeadTime(ControllerTraitsT::skInstance, TimerTraitsT::skLLId);
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
        [[nodiscard]] bool init_adc_trigger(const HrtimTimerAdcTriggerConfig<tkTimerId, tkTriggerId>& config)
        {
            LL_HRTIM_SetADCTrigUpdate(
                ControllerTraitsT::skInstance, static_cast<uint32_t>(tkTriggerId), TimerTraitsT::skAdcTrigUpdateId);

            LL_HRTIM_SetADCTrigSrc(ControllerTraitsT::skInstance,
                                   static_cast<uint32_t>(tkTriggerId),
                                   static_cast<uint32_t>(config.source));

            LL_HRTIM_SetADCPostScaler(ControllerTraitsT::skInstance,
                                      static_cast<uint32_t>(tkTriggerId),
                                      static_cast<uint32_t>(config.post_scaler));

            LL_HRTIM_TIM_SetADCRollOverMode(
                ControllerTraitsT::skInstance, TimerTraitsT::skLLId, static_cast<uint32_t>(config.rollover_mode));

            return true;
        }

        // ------------------------------------------------------------------------
        // Interrupts
        // ------------------------------------------------------------------------

        static void enable_interrupts(const HrtimTimerInterruptConfig& config)
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
        [[nodiscard]] bool init_output(const HrtimTimerVirtualOutputConfig& config)
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
        [[nodiscard]] bool init_output_gpio(const HrtimTimerOutputGpioConfig& config)
            requires((tkOutputNum == 1 && skHasOutput1Pin) || (tkOutputNum == 2 && skHasOutput2Pin))
        {
            if constexpr (tkOutputNum == 1)
            {
                return m_output1_pin.init(GpioAlternateFunctionConfig{
                    .mode  = GpioAlternateFunctionMode::kPushPull,
                    .speed = config.speed,
                    .pull  = config.pull,
                });
            }
            else if constexpr (tkOutputNum == 2)
            {
                return m_output2_pin.init(GpioAlternateFunctionConfig{
                    .mode  = GpioAlternateFunctionMode::kPushPull,
                    .speed = config.speed,
                    .pull  = config.pull,
                });
            }
        }
    };

    // -----------------------------------------------------------------------------
    // DEVICE ALIASES
    // -----------------------------------------------------------------------------

    using Hrtim1TimerADevice = HrtimTimerDevice<HrtimPeripheralId::kHrtim1, HrtimTimerId::kTimerA>;
    using Hrtim1TimerBDevice = HrtimTimerDevice<HrtimPeripheralId::kHrtim1, HrtimTimerId::kTimerB>;
    using Hrtim1TimerCDevice = HrtimTimerDevice<HrtimPeripheralId::kHrtim1, HrtimTimerId::kTimerC>;
    using Hrtim1TimerDDevice = HrtimTimerDevice<HrtimPeripheralId::kHrtim1, HrtimTimerId::kTimerD>;
    using Hrtim1TimerEDevice = HrtimTimerDevice<HrtimPeripheralId::kHrtim1, HrtimTimerId::kTimerE>;
    using Hrtim1TimerFDevice = HrtimTimerDevice<HrtimPeripheralId::kHrtim1, HrtimTimerId::kTimerF>;

}  // namespace valle::platform
