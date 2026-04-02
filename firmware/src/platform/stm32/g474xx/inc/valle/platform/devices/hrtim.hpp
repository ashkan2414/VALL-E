#pragma once

#include <algorithm>
#include <cstdint>

#include "stm32g4xx_ll_bus.h"
#include "valle/platform/core.hpp"
#include "valle/platform/devices/rcc.hpp"
#include "valle/platform/drivers/gpio/alternate_function.hpp"
#include "valle/platform/hdi/hrtim.hpp"

namespace valle::platform
{
    // =============================================================================
    // HRTIM CONTROLLER DEVICE (SHARED DEVICE)
    // =============================================================================

    // -----------------------------------------------------------------------------
    // COMPILE TIME CONFIGURATIONS
    // -----------------------------------------------------------------------------
    struct HrtimControllerCTDefaultConfig
    {
        using SCInPinT  = NullDevice;
        using SCOutPinT = NullDevice;
    };

    template <HrtimControllerId tkControllerId, HrtimControllerGpioPinType tkPinType>
    using HrtimControllerDefaultPin = HrtimControllerPinMap<tkControllerId, tkPinType, 0>;

    template <HrtimControllerId tkControllerId, HrtimControllerGpioPinType tkPinType, typename TPin>
    concept CValidHrtimControllerPin =
        CGpioPin<TPin> && CValidHrtimControllerPin<tkControllerId, tkPinType, TPin::skPortId, TPin::skPinId>;

    template <HrtimControllerId tkControllerId, HrtimControllerGpioPinType tkPinType, typename TPin>
    concept CValidHrtimControllerPinCTConfig =
        CNullDevice<TPin> || CValidHrtimControllerPin<tkControllerId, tkPinType, TPin>;

    template <typename T, HrtimControllerId tkControllerId>
    concept CValidHrtimControllerCTConfig =
        requires {
            typename T::SCInPinT;
            typename T::SCOutPinT;
        } &&
        (CValidHrtimControllerPinCTConfig<tkControllerId, HrtimControllerGpioPinType::kSCIn, typename T::SCInPinT>) &&
        (CValidHrtimControllerPinCTConfig<tkControllerId, HrtimControllerGpioPinType::kSCOut, typename T::SCOutPinT>);

    template <HrtimControllerId tkControllerId>
    struct HrtimControllerCTConfigRegistry
    {
        static constexpr auto skConfig = HrtimControllerCTDefaultConfig{};
    };

#define VALLE_DEFINE_HRTIM_CONTROLLER_CT_CONFIG(tkControllerId, config)                        \
    namespace valle::platform                                                                  \
    {                                                                                          \
        template <>                                                                            \
        struct HrtimControllerCTConfigRegistry<(tkControllerId)>                               \
        {                                                                                      \
            static constexpr auto skConfig = (config);                                         \
            static_assert(CValidHrtimControllerCTConfig<decltype(skConfig), (tkControllerId)>, \
                          "Invalid HRTIM Controller CT Config");                               \
        };                                                                                     \
    }

    // -----------------------------------------------------------------------------
    // HELPERS
    // -----------------------------------------------------------------------------

    namespace detail
    {
        template <HrtimControllerId tkControllerId, HrtimControllerGpioPinType tkPinType, typename TPin>
        struct HrtimControllerGpioPinDriverHelper
        {
            using type = GpioAlternateFunctionDriver<
                TPin,
                kHrtimControllerPinAF<tkControllerId, tkPinType, TPin::skPortId, TPin::skPinId>>;
        };

        template <HrtimControllerId tkControllerId, HrtimControllerGpioPinType tkPinType>
        struct HrtimControllerGpioPinDriverHelper<tkControllerId, tkPinType, NullDevice>
        {
            using type = std::monostate;
        };

    }  // namespace detail

    template <HrtimControllerId tkControllerId, HrtimControllerGpioPinType tkPinType, typename TPin>
    using HrtimControllerGpioPinDriver =
        typename detail::HrtimControllerGpioPinDriverHelper<tkControllerId, tkPinType, TPin>::type;

    // -----------------------------------------------------------------------------
    // DEVICE CLASS
    // -----------------------------------------------------------------------------

    template <HrtimControllerId tkControllerId>
    class HrtimController
    {
    public:
        struct Descriptor : public SharedDeviceDescriptor
        {
        };
        static constexpr HrtimControllerId skControllerId = tkControllerId;

        using ControllerTraitsT          = HrtimControllerTraits<tkControllerId>;
        using CTConfigRegistryT          = HrtimControllerCTConfigRegistry<tkControllerId>;
        static constexpr auto skCTConfig = CTConfigRegistryT::skConfig;
        using CTConfigT                  = decltype(skCTConfig);

        using SCInPinT  = typename CTConfigT::SCInPinT;
        using SCOutPinT = typename CTConfigT::SCOutPinT;

        static constexpr bool skHasSCInPin  = !CNullDevice<SCInPinT>;
        static constexpr bool skHasSCOutPin = !CNullDevice<SCOutPinT>;

        using SCInPinDriverT =
            HrtimControllerGpioPinDriver<tkControllerId, HrtimControllerGpioPinType::kSCIn, SCInPinT>;
        using SCOutPinDriverT =
            HrtimControllerGpioPinDriver<tkControllerId, HrtimControllerGpioPinType::kSCOut, SCOutPinT>;

        using InjectDevices = FilterNullDevices<TypeList<SCInPinT, SCOutPinT>>;

    private:
        SCInPinDriverT  m_scin_pin;
        SCOutPinDriverT m_scout_pin;

    public:
        template <typename... TArgs>
        explicit HrtimController(TArgs&&... args)
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
                VALLE_LOG_ERROR("HRTIM{} DLL Calibration timed out!", kHrtimControllerNumFromId<tkControllerId>);
                return false;
            }

            return true;
        }
    };

    // -----------------------------------------------------------------------------
    // DEVICE ALIASES
    // -----------------------------------------------------------------------------

    using Hrtim1Controller = HrtimController<HrtimControllerId::kHrtim1>;

    // =============================================================================
    // HRTIM FAULT DEVICE (SHARED DEVICE)
    // =============================================================================

    // -----------------------------------------------------------------------------
    // COMPILE TIME CONFIGURATIONS
    // -----------------------------------------------------------------------------
    struct HrtimFaultCTDefaultConfig
    {
        using PinT = NullDevice;
    };

    template <HrtimControllerId tkControllerId, HrtimFaultId tkFaultId, uint8_t tkOutputIdx = 0>
    using HrtimFaultPinDeviceMap = GpioPin<HrtimFaultPinMap<tkControllerId, tkFaultId, tkOutputIdx>::skPortId,
                                           HrtimFaultPinMap<tkControllerId, tkFaultId, tkOutputIdx>::skPinId>;

    template <HrtimControllerId tkControllerId, HrtimFaultId tkFaultId>
    using HrtimFaultDefaultPin = HrtimFaultPinDeviceMap<tkControllerId, tkFaultId, 0>;

    template <HrtimControllerId tkControllerId, HrtimFaultId tkFaultId, typename TPin>
    concept CValidHrtimFaultPin =
        CGpioPin<TPin> && CValidHrtimFaultPin<tkControllerId, tkFaultId, TPin::skPortId, TPin::skPinId>;

    template <typename T, HrtimControllerId tkControllerId, HrtimFaultId tkFaultId>
    concept CValidHrtimFaultCTConfig = requires {
        typename T::PinT;
    } && (CNullDevice<typename T::PinT> || CValidHrtimFaultPin<tkControllerId, tkFaultId, typename T::PinT>);

    template <HrtimControllerId tkControllerId, HrtimFaultId tkFaultId>
    struct HrtimFaultCTConfigRegistry
    {
        static constexpr auto skConfig = HrtimFaultCTDefaultConfig{};
    };

#define VALLE_DEFINE_HRTIM_FAULT_CT_CONFIG(tkControllerId, tkFaultId, config)                          \
    namespace valle::platform                                                                          \
    {                                                                                                  \
        template <>                                                                                    \
        struct HrtimFaultCTConfigRegistry<(tkControllerId), (tkFaultId)>                               \
        {                                                                                              \
            static constexpr auto skConfig = (config);                                                 \
            static_assert(CValidHrtimFaultCTConfig<decltype(skConfig), (tkControllerId), (tkFaultId)>, \
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
        template <HrtimControllerId tkControllerId, HrtimFaultId tkFaultId, typename TPin>
        struct HrtimFaultGpioPinDriverHelper
        {
            using type =
                GpioAlternateFunctionDriver<TPin,
                                            kHrtimFaultPinAF<tkControllerId, tkFaultId, TPin::skPortId, TPin::skPinId>>;
        };

        template <HrtimControllerId tkControllerId, HrtimFaultId tkFaultId>
        struct HrtimFaultGpioPinDriverHelper<tkControllerId, tkFaultId, NullDevice>
        {
            using type = std::monostate;
        };

    }  // namespace detail

    template <HrtimControllerId tkControllerId, HrtimFaultId tkFaultId, typename TPin>
    using HrtimFaultGpioPinDriver =
        typename detail::HrtimFaultGpioPinDriverHelper<tkControllerId, tkFaultId, TPin>::type;

    // -----------------------------------------------------------------------------
    // DEVICE CLASS
    // -----------------------------------------------------------------------------

    template <HrtimControllerId tkControllerId, HrtimFaultId tkFaultId>
    class HrtimFault
    {
    public:
        struct Descriptor : public SharedDeviceDescriptor
        {
        };

        static constexpr HrtimControllerId skControllerId = tkControllerId;
        static constexpr HrtimFaultId      skFaultId      = tkFaultId;

        using ControllerT       = HrtimController<tkControllerId>;
        using ControllerTraitsT = HrtimControllerTraits<tkControllerId>;
        using FaultTraitsT      = HrtimFaultTraits<tkControllerId, skFaultId>;

        using CTConfigRegistryT          = HrtimFaultCTConfigRegistry<tkControllerId, skFaultId>;
        static constexpr auto skCTConfig = CTConfigRegistryT::skConfig;
        using CTConfigT                  = decltype(skCTConfig);

        using PinT       = typename CTConfigT::PinT;
        using PinDriverT = HrtimFaultGpioPinDriver<tkControllerId, skFaultId, PinT>;

        using InjectDevices = FilterNullDevices<TypeList<ControllerT, PinT>>;

    private:
        [[no_unique_address]] DeviceRef<ControllerT> m_controller;
        PinDriverT                                   m_pin;

    public:
        template <typename... TArgs>
        explicit HrtimFault(TArgs&&... args)
            : m_controller(extract_device_ref<true, ControllerT>(std::forward<TArgs>(args)...))
            , m_pin(extract_device_ref<!CNullDevice<PinT>, PinT>(std::forward<TArgs>(args)...))
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

    class HrtimNullFault
    {
    public:
        struct Descriptor : public NullDeviceDescriptor
        {
        };
    };

    // -----------------------------------------------------------------------------
    // DEVICE ALIASES
    // -----------------------------------------------------------------------------

    using Hrtim1Fault1 = HrtimFault<HrtimControllerId::kHrtim1, HrtimFaultId::kFault1>;
    using Hrtim1Fault2 = HrtimFault<HrtimControllerId::kHrtim1, HrtimFaultId::kFault2>;
    using Hrtim1Fault3 = HrtimFault<HrtimControllerId::kHrtim1, HrtimFaultId::kFault3>;
    using Hrtim1Fault4 = HrtimFault<HrtimControllerId::kHrtim1, HrtimFaultId::kFault4>;
    using Hrtim1Fault5 = HrtimFault<HrtimControllerId::kHrtim1, HrtimFaultId::kFault5>;
    using Hrtim1Fault6 = HrtimFault<HrtimControllerId::kHrtim1, HrtimFaultId::kFault6>;

    // =============================================================================
    // HRTIM EXTERNAL EVENT DEVICE (SHARED DEVICE)
    // =============================================================================

    // -----------------------------------------------------------------------------
    // COMPILE TIME CONFIGURATIONS
    // -----------------------------------------------------------------------------
    struct HrtimExternalEventCTDefaultConfig
    {
        using PinT = NullDevice;
    };

    template <HrtimControllerId tkControllerId, HrtimExternalEventId tkExternalEventId, uint8_t tkOutputIdx = 0>
    using HrtimExternalEventPinDeviceMap =
        GpioPin<HrtimExternalEventPinMap<tkControllerId, tkExternalEventId, tkOutputIdx>::skPortId,
                HrtimExternalEventPinMap<tkControllerId, tkExternalEventId, tkOutputIdx>::skPinId>;

    template <HrtimControllerId tkControllerId, HrtimExternalEventId tkExternalEventId>
    using HrtimExternalEventDefaultPin = HrtimExternalEventPinDeviceMap<tkControllerId, tkExternalEventId, 0>;

    template <HrtimControllerId tkControllerId, HrtimExternalEventId tkExternalEventId, typename TPin>
    concept CValidHrtimExternalEventPin =
        CGpioPin<TPin> && CValidHrtimExternalEventPin<tkControllerId, tkExternalEventId, TPin::skPortId, TPin::skPinId>;

    template <typename T, HrtimControllerId tkControllerId, HrtimExternalEventId tkExternalEventId>
    concept CValidHrtimExternalEventCTConfig =
        requires { typename T::PinT; } &&
        (CNullDevice<typename T::PinT> ||
         CValidHrtimExternalEventPin<tkControllerId, tkExternalEventId, typename T::PinT>);

    template <HrtimControllerId tkControllerId, HrtimExternalEventId tkExternalEventId>
    struct HrtimExternalEventCTConfigRegistry
    {
        static constexpr auto skConfig = HrtimExternalEventCTDefaultConfig{};
    };

#define VALLE_DEFINE_HRTIM_EXTERNAL_EVENT_CT_CONFIG(tkControllerId, tkExternalEventId, config)                         \
    namespace valle::platform                                                                                          \
    {                                                                                                                  \
        template <>                                                                                                    \
        struct HrtimExternalEventCTConfigRegistry<(tkControllerId), (tkExternalEventId)>                               \
        {                                                                                                              \
            static constexpr auto skConfig = (config);                                                                 \
            static_assert(CValidHrtimExternalEventCTConfig<decltype(skConfig), (tkControllerId), (tkExternalEventId)>, \
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
        template <HrtimControllerId tkControllerId, HrtimExternalEventId tkExternalEventId, typename TPin>
        struct HrtimExternalEventGpioPinDriverHelper
        {
            using type = GpioAlternateFunctionDriver<
                TPin,
                kHrtimExternalEventPinAF<tkControllerId, tkExternalEventId, TPin::skPortId, TPin::skPinId>>;
        };

        template <HrtimControllerId tkControllerId, HrtimExternalEventId tkExternalEventId>
        struct HrtimExternalEventGpioPinDriverHelper<tkControllerId, tkExternalEventId, NullDevice>
        {
            using type = std::monostate;
        };

    }  // namespace detail

    template <HrtimControllerId tkControllerId, HrtimExternalEventId tkExternalEventId, typename TPin>
    using HrtimExternalEventGpioPinDriver =
        typename detail::HrtimExternalEventGpioPinDriverHelper<tkControllerId, tkExternalEventId, TPin>::type;

    // -----------------------------------------------------------------------------
    // DEVICE CLASS
    // -----------------------------------------------------------------------------

    template <HrtimControllerId tkControllerId, HrtimExternalEventId tkExternalEventId>
    class HrtimExternalEvent
    {
    public:
        struct Descriptor : public SharedDeviceDescriptor
        {
        };

        static constexpr HrtimControllerId    skControllerId    = tkControllerId;
        static constexpr HrtimExternalEventId skExternalEventId = tkExternalEventId;

        using ControllerT          = HrtimController<tkControllerId>;
        using ControllerTraitsT    = HrtimControllerTraits<tkControllerId>;
        using ExternalEventTraitsT = HrtimExternalEventTraits<tkControllerId, skExternalEventId>;

        using CTConfigRegistryT          = HrtimExternalEventCTConfigRegistry<tkControllerId, skExternalEventId>;
        static constexpr auto skCTConfig = CTConfigRegistryT::skConfig;
        using CTConfigT                  = decltype(skCTConfig);

        using PinT       = typename CTConfigT::PinT;
        using PinDriverT = HrtimExternalEventGpioPinDriver<tkControllerId, skExternalEventId, PinT>;

        using InjectDevices = FilterNullDevices<TypeList<ControllerT, PinT>>;

    private:
        [[no_unique_address]] DeviceRef<ControllerT> m_controller;
        PinDriverT                                   m_pin;

    public:
        template <typename... TArgs>
        explicit HrtimExternalEvent(TArgs&&... args)
            : m_controller(extract_device_ref<true, ControllerT>(std::forward<TArgs>(args)...))
            , m_pin(extract_device_ref<!CNullDevice<PinT>, PinT>(std::forward<TArgs>(args)...))
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

    class HrtimNullExternalEvent
    {
    public:
        struct Descriptor : public NullDeviceDescriptor
        {
        };
    };

    // -----------------------------------------------------------------------------
    // DEVICE ALIASES
    // -----------------------------------------------------------------------------

    using Hrtim1ExternalEvent1 = HrtimExternalEvent<HrtimControllerId::kHrtim1, HrtimExternalEventId::kExternalEvent1>;
    using Hrtim1ExternalEvent2 = HrtimExternalEvent<HrtimControllerId::kHrtim1, HrtimExternalEventId::kExternalEvent2>;
    using Hrtim1ExternalEvent3 = HrtimExternalEvent<HrtimControllerId::kHrtim1, HrtimExternalEventId::kExternalEvent3>;
    using Hrtim1ExternalEvent4 = HrtimExternalEvent<HrtimControllerId::kHrtim1, HrtimExternalEventId::kExternalEvent4>;
    using Hrtim1ExternalEvent5 = HrtimExternalEvent<HrtimControllerId::kHrtim1, HrtimExternalEventId::kExternalEvent5>;
    using Hrtim1ExternalEvent6 = HrtimExternalEvent<HrtimControllerId::kHrtim1, HrtimExternalEventId::kExternalEvent6>;
    using Hrtim1ExternalEvent7 = HrtimExternalEvent<HrtimControllerId::kHrtim1, HrtimExternalEventId::kExternalEvent7>;
    using Hrtim1ExternalEvent8 = HrtimExternalEvent<HrtimControllerId::kHrtim1, HrtimExternalEventId::kExternalEvent8>;
    using Hrtim1ExternalEvent9 = HrtimExternalEvent<HrtimControllerId::kHrtim1, HrtimExternalEventId::kExternalEvent9>;
    using Hrtim1ExternalEvent10 =
        HrtimExternalEvent<HrtimControllerId::kHrtim1, HrtimExternalEventId::kExternalEvent10>;

    // =============================================================================
    // HRTIM TIMER DEVICE (UNIQUE DEVICE)
    // =============================================================================

    // ---------------------------------------------------------------------------
    // COMPILE TIME CONFIGURATIONS
    // ---------------------------------------------------------------------------

    struct HrtimTimerCTDefaultConfig
    {
        using Output1PinT = NullDevice;
        using Output2PinT = NullDevice;

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

    template <HrtimControllerId     tkControllerId,
              HrtimTimerId          tkTimerId,
              HrtimTimerGpioPinType tkPinType,
              uint8_t               tkOutputIdx = 0>
    using HrtimTimerPinDeviceMap =
        GpioPin<HrtimTimerPinMap<tkControllerId, tkTimerId, tkPinType, tkOutputIdx>::skPortId,
                HrtimTimerPinMap<tkControllerId, tkTimerId, tkPinType, tkOutputIdx>::skPinId>;

    template <HrtimControllerId tkControllerId, HrtimTimerId tkTimerId, HrtimTimerGpioPinType tkPinType>
    using HrtimTimerDefaultPin = HrtimTimerPinDeviceMap<tkControllerId, tkTimerId, tkPinType, 0>;

    template <HrtimControllerId tkControllerId, HrtimTimerId tkTimerId>
    using HrtimTimerDefaultOutput1Pin =
        HrtimTimerDefaultPin<tkControllerId, tkTimerId, HrtimTimerGpioPinType::kOutput1>;

    template <HrtimControllerId tkControllerId, HrtimTimerId tkTimerId>
    using HrtimTimerDefaultOutput2Pin =
        HrtimTimerDefaultPin<tkControllerId, tkTimerId, HrtimTimerGpioPinType::kOutput2>;

    template <HrtimControllerId tkControllerId, HrtimTimerId tkTimerId, HrtimTimerGpioPinType tkPinType, typename TPin>
    concept CValidHrtimTimerPin =
        CGpioPin<TPin> && CValidHrtimTimerPin<tkControllerId, tkTimerId, tkPinType, TPin::skPortId, TPin::skPinId>;

    template <HrtimControllerId tkControllerId, HrtimTimerId tkTimerId, HrtimTimerGpioPinType tkPinType, typename TPin>
    concept CValidHrtimTimerPinCTConfig =
        (CNullDevice<TPin> || CValidHrtimTimerPin<tkControllerId, tkTimerId, tkPinType, TPin>);

    template <typename T, HrtimControllerId tkControllerId, HrtimTimerId tkTimerId>
    concept CValidHrtimTimerCTConfig = requires { typename T::Output1PinT; } &&
                                       (CValidHrtimTimerPinCTConfig<tkControllerId,
                                                                    tkTimerId,
                                                                    HrtimTimerGpioPinType::kOutput1,
                                                                    typename T::Output1PinT>) &&
                                       (CValidHrtimTimerPinCTConfig<tkControllerId,
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

    template <HrtimControllerId tkControllerId, HrtimTimerId tkTimerId>
    struct HrtimTimerCTConfigRegistry
    {
        static constexpr auto skConfig = HrtimTimerCTDefaultConfig{};
    };

#define VALLE_DEFINE_HRTIM_TIMER_CT_CONFIG(tkControllerId, tkTimerId, config)                          \
    namespace valle::platform                                                                          \
    {                                                                                                  \
        template <>                                                                                    \
        struct HrtimTimerCTConfigRegistry<(tkControllerId), (tkTimerId)>                               \
        {                                                                                              \
            static constexpr auto skConfig = (config);                                                 \
            static_assert(CValidHrtimTimerCTConfig<decltype(skConfig), (tkControllerId), (tkTimerId)>, \
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
        GpioPinSpeedMode speed = GpioPinSpeedMode::kHigh;
        GpioPinPullMode  pull  = GpioPinPullMode::kNoPull;
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
        template <HrtimControllerId     tkControllerId,
                  HrtimTimerId          tkTimerId,
                  HrtimTimerGpioPinType tkPinType,
                  typename TPin>
        struct HrtimTimerGpioPinDriverHelper
        {
            constexpr static auto skAF =
                kHrtimTimerPinAF<tkControllerId, tkTimerId, tkPinType, TPin::skPortId, TPin::skPinId>;
            using type = GpioAlternateFunctionDriver<TPin, skAF>;
        };

        template <HrtimControllerId tkControllerId, HrtimTimerId tkTimerId, HrtimTimerGpioPinType tkPinType>
        struct HrtimTimerGpioPinDriverHelper<tkControllerId, tkTimerId, tkPinType, NullDevice>
        {
            using type = std::monostate;
        };

    }  // namespace detail

    template <HrtimControllerId tkControllerId, HrtimTimerId tkTimerId, HrtimTimerGpioPinType tkPinType, typename TPin>
    using HrtimTimerGpioPinDriver =
        typename detail::HrtimTimerGpioPinDriverHelper<tkControllerId, tkTimerId, tkPinType, TPin>::type;

    // -----------------------------------------------------------------------------
    // DEVICE CLASS
    // -----------------------------------------------------------------------------

    template <HrtimControllerId tkControllerId, HrtimTimerId tkTimerId>
    class HrtimTimer
    {
    public:
        struct Descriptor : public UniqueDeviceDescriptor
        {
        };

        static constexpr HrtimControllerId skControllerId = tkControllerId;
        static constexpr HrtimTimerId      skTimerId      = tkTimerId;

        using ControllerT          = HrtimController<tkControllerId>;
        using ControllerTraitsT    = HrtimControllerTraits<tkControllerId>;
        using TimerTraitsT         = HrtimTimerTraits<tkControllerId, tkTimerId>;
        using InterfaceT           = HrtimTimerInterface<tkControllerId, tkTimerId>;
        using InterruptControllerT = HrtimTimerInterruptController<tkControllerId, tkTimerId>;

        using CTConfigRegistryT          = HrtimTimerCTConfigRegistry<tkControllerId, tkTimerId>;
        static constexpr auto skCTConfig = CTConfigRegistryT::skConfig;
        using CTConfigT                  = decltype(skCTConfig);

        using Output1PinT = typename CTConfigT::Output1PinT;
        using Output2PinT = typename CTConfigT::Output2PinT;

        static constexpr bool skHasOutput1Pin = !CNullDevice<Output1PinT>;
        static constexpr bool skHasOutput2Pin = !CNullDevice<Output2PinT>;

        using Output1PinDriverT =
            HrtimTimerGpioPinDriver<tkControllerId, tkTimerId, HrtimTimerGpioPinType::kOutput1, Output1PinT>;
        using Output2PinDriverT =
            HrtimTimerGpioPinDriver<tkControllerId, tkTimerId, HrtimTimerGpioPinType::kOutput2, Output2PinT>;

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

        using Fault1DeviceT =
            std::conditional_t<skHasFault1, HrtimFault<tkControllerId, HrtimFaultId::kFault1>, HrtimNullFault>;
        using Fault2DeviceT =
            std::conditional_t<skHasFault2, HrtimFault<tkControllerId, HrtimFaultId::kFault2>, HrtimNullFault>;
        using Fault3DeviceT =
            std::conditional_t<skHasFault3, HrtimFault<tkControllerId, HrtimFaultId::kFault3>, HrtimNullFault>;
        using Fault4DeviceT =
            std::conditional_t<skHasFault4, HrtimFault<tkControllerId, HrtimFaultId::kFault4>, HrtimNullFault>;
        using Fault5DeviceT =
            std::conditional_t<skHasFault5, HrtimFault<tkControllerId, HrtimFaultId::kFault5>, HrtimNullFault>;
        using Fault6DeviceT =
            std::conditional_t<skHasFault6, HrtimFault<tkControllerId, HrtimFaultId::kFault6>, HrtimNullFault>;

        using ExternalEvent1DeviceT =
            std::conditional_t<skHasExternalEvent1,
                               HrtimExternalEvent<tkControllerId, HrtimExternalEventId::kExternalEvent1>,
                               HrtimNullExternalEvent>;
        using ExternalEvent2DeviceT =
            std::conditional_t<skHasExternalEvent2,
                               HrtimExternalEvent<tkControllerId, HrtimExternalEventId::kExternalEvent2>,
                               HrtimNullExternalEvent>;
        using ExternalEvent3DeviceT =
            std::conditional_t<skHasExternalEvent3,
                               HrtimExternalEvent<tkControllerId, HrtimExternalEventId::kExternalEvent3>,
                               HrtimNullExternalEvent>;
        using ExternalEvent4DeviceT =
            std::conditional_t<skHasExternalEvent4,
                               HrtimExternalEvent<tkControllerId, HrtimExternalEventId::kExternalEvent4>,
                               HrtimNullExternalEvent>;
        using ExternalEvent5DeviceT =
            std::conditional_t<skHasExternalEvent5,
                               HrtimExternalEvent<tkControllerId, HrtimExternalEventId::kExternalEvent5>,
                               HrtimNullExternalEvent>;
        using ExternalEvent6DeviceT =
            std::conditional_t<skHasExternalEvent6,
                               HrtimExternalEvent<tkControllerId, HrtimExternalEventId::kExternalEvent6>,
                               HrtimNullExternalEvent>;
        using ExternalEvent7DeviceT =
            std::conditional_t<skHasExternalEvent7,
                               HrtimExternalEvent<tkControllerId, HrtimExternalEventId::kExternalEvent7>,
                               HrtimNullExternalEvent>;
        using ExternalEvent8DeviceT =
            std::conditional_t<skHasExternalEvent8,
                               HrtimExternalEvent<tkControllerId, HrtimExternalEventId::kExternalEvent8>,
                               HrtimNullExternalEvent>;
        using ExternalEvent9DeviceT =
            std::conditional_t<skHasExternalEvent9,
                               HrtimExternalEvent<tkControllerId, HrtimExternalEventId::kExternalEvent9>,
                               HrtimNullExternalEvent>;
        using ExternalEvent10DeviceT =
            std::conditional_t<skHasExternalEvent10,
                               HrtimExternalEvent<tkControllerId, HrtimExternalEventId::kExternalEvent10>,
                               HrtimNullExternalEvent>;

        using InjectDevices = FilterNullDevices<TypeList<ControllerT,
                                                         RccInfo<>,
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
        [[no_unique_address]] DeviceRef<ControllerT> m_controller;
        [[no_unique_address]] DeviceRef<RccInfo<>>   m_rcc_info;

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
        explicit HrtimTimer(TArgs&&... args)
            : m_controller(extract_device_ref<true, ControllerT>(std::forward<TArgs>(args)...))
            , m_rcc_info(extract_device_ref<true, RccInfo<>>(std::forward<TArgs>(args)...))
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
                                static_cast<int>(tkControllerId),
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
                                static_cast<int>(tkControllerId),
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
                    .mode  = GpioPinOutputMode::kPushPull,
                    .speed = config.speed,
                    .pull  = config.pull,
                });
            }
            else if constexpr (tkOutputNum == 2)
            {
                return m_output2_pin.init(GpioAlternateFunctionConfig{
                    .mode  = GpioPinOutputMode::kPushPull,
                    .speed = config.speed,
                    .pull  = config.pull,
                });
            }
        }
    };

    // -----------------------------------------------------------------------------
    // DEVICE ALIASES
    // -----------------------------------------------------------------------------

    using Hrtim1TimerA = HrtimTimer<HrtimControllerId::kHrtim1, HrtimTimerId::kTimerA>;
    using Hrtim1TimerB = HrtimTimer<HrtimControllerId::kHrtim1, HrtimTimerId::kTimerB>;
    using Hrtim1TimerC = HrtimTimer<HrtimControllerId::kHrtim1, HrtimTimerId::kTimerC>;
    using Hrtim1TimerD = HrtimTimer<HrtimControllerId::kHrtim1, HrtimTimerId::kTimerD>;
    using Hrtim1TimerE = HrtimTimer<HrtimControllerId::kHrtim1, HrtimTimerId::kTimerE>;
    using Hrtim1TimerF = HrtimTimer<HrtimControllerId::kHrtim1, HrtimTimerId::kTimerF>;

}  // namespace valle::platform
