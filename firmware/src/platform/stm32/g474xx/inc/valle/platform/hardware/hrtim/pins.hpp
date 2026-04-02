#pragma once

#include "valle/platform/hardware/gpio.hpp"
#include "valle/platform/hardware/hrtim/id.hpp"

namespace valle::platform
{
    // ---------------------------------------------------------------------------
    // HRTIM CONTROLLER
    // ---------------------------------------------------------------------------
    enum class HrtimControllerGpioPinType : uint8_t
    {
        kSCIn,
        kSCOut,
    };

    constexpr static uint8_t kHrtimMaxOutputIndex = 2;

    template <HrtimControllerId tkControllerId, HrtimControllerGpioPinType tkPinType, uint8_t tkOutputIdx = 0>
        requires(tkOutputIdx < kHrtimMaxOutputIndex)
    struct HrtimControllerPinMap;

#define DECLARE_HRTIM_PIN_MAP(tkControllerId, tkPinType, tkOutputIdx, port, pin, af) \
    static_assert((tkOutputIdx) < kHrtimMaxOutputIndex);                             \
    template <>                                                                      \
    struct HrtimControllerPinMap<(tkControllerId), (tkPinType), (tkOutputIdx)>       \
    {                                                                                \
        constexpr static GpioPortId              skPortId = (port);                  \
        constexpr static GpioPinId               skPinId  = (pin);                   \
        constexpr static GpioAlternativeFunction skAF     = (af);                    \
    };

    DECLARE_HRTIM_PIN_MAP(HrtimControllerId::kHrtim1,
                          HrtimControllerGpioPinType::kSCIn,
                          0,
                          GpioPortId::kPortB,
                          GpioPinId::kPin2,
                          GpioAlternativeFunction::kAF13);  // PB2
    DECLARE_HRTIM_PIN_MAP(HrtimControllerId::kHrtim1,
                          HrtimControllerGpioPinType::kSCIn,
                          1,
                          GpioPortId::kPortB,
                          GpioPinId::kPin6,
                          GpioAlternativeFunction::kAF12);  // PB6
    DECLARE_HRTIM_PIN_MAP(HrtimControllerId::kHrtim1,
                          HrtimControllerGpioPinType::kSCOut,
                          0,
                          GpioPortId::kPortB,
                          GpioPinId::kPin1,
                          GpioAlternativeFunction::kAF13);  // PB1
    DECLARE_HRTIM_PIN_MAP(HrtimControllerId::kHrtim1,
                          HrtimControllerGpioPinType::kSCOut,
                          1,
                          GpioPortId::kPortB,
                          GpioPinId::kPin3,
                          GpioAlternativeFunction::kAF12);  // PB3

#undef DECLARE_HRTIM_PIN_MAP
    namespace detail
    {
        template <HrtimControllerId          tkControllerId,
                  HrtimControllerGpioPinType tkPinType,
                  uint8_t                    tkOutputIdx,
                  GpioPortId                 tkPortId,
                  GpioPinId                  tkPinId>
        concept CValidSpecificHrtimControllerPin = requires {
            typename HrtimControllerPinMap<tkControllerId, tkPinType, tkOutputIdx>;
            { HrtimControllerPinMap<tkControllerId, tkPinType, tkOutputIdx>::skPortId } -> std::same_as<GpioPortId>;
            { HrtimControllerPinMap<tkControllerId, tkPinType, tkOutputIdx>::skPinId } -> std::same_as<GpioPinId>;
            HrtimControllerPinMap<tkControllerId, tkPinType, tkOutputIdx>::skPortId == tkPortId;
            HrtimControllerPinMap<tkControllerId, tkPinType, tkOutputIdx>::skPinId == tkPinId;
        };

        template <HrtimControllerId          tkControllerId,
                  HrtimControllerGpioPinType tkPinType,
                  GpioPortId                 tkPortId,
                  GpioPinId                  tkPinId,
                  uint8_t... tkIdxs>
        constexpr std::optional<uint8_t> get_hrtim_controller_pin_output_index(
            std::integer_sequence<uint8_t, tkIdxs...>)
        {
            // This will return the first matching index, or -1 if none match
            std::optional<uint8_t> result = std::nullopt;
            ((CValidSpecificHrtimControllerPin<tkControllerId, tkPinType, tkIdxs, tkPortId, tkPinId>
                  ? (result = tkIdxs, true)
                  : false) ||
             ...);
            return result;
        }

        template <HrtimControllerId          tkControllerId,
                  HrtimControllerGpioPinType tkPinType,
                  GpioPortId                 tkPortId,
                  GpioPinId                  tkPinId,
                  uint8_t... tkIdxs>
        constexpr bool check_any_valid_hrtim_controller_pin_map(std::integer_sequence<uint8_t, tkIdxs...>)
        {
            // This will return true if ANY of the indices match
            return get_hrtim_controller_pin_output_index<tkControllerId, tkPinType, tkPortId, tkPinId>(
                       std::integer_sequence<uint8_t, tkIdxs...>{})
                .has_value();
        }
    }  // namespace detail

    template <HrtimControllerId          tkControllerId,
              HrtimControllerGpioPinType tkPinType,
              GpioPortId                 tkPortId,
              GpioPinId                  tkPinId>
    concept CValidHrtimControllerPin =
        detail::check_any_valid_hrtim_controller_pin_map<tkControllerId, tkPinType, tkPortId, tkPinId>(
            std::make_integer_sequence<uint8_t, kHrtimMaxOutputIndex>{});

    template <HrtimControllerId          tkControllerId,
              HrtimControllerGpioPinType tkPinType,
              GpioPortId                 tkPortId,
              GpioPinId                  tkPinId>
        requires(CValidHrtimControllerPin<tkControllerId, tkPinType, tkPortId, tkPinId>)
    constexpr uint8_t kHrtimControllerPinOutputIndex =
        detail::get_hrtim_controller_pin_output_index<tkControllerId, tkPinType, tkPortId, tkPinId>(
            std::make_integer_sequence<uint8_t, kHrtimMaxOutputIndex>{})
            .value();

    template <HrtimControllerId          tkControllerId,
              HrtimControllerGpioPinType tkPinType,
              GpioPortId                 tkPortId,
              GpioPinId                  tkPinId>
        requires(CValidHrtimControllerPin<tkControllerId, tkPinType, tkPortId, tkPinId>)
    constexpr GpioAlternativeFunction kHrtimControllerPinAF =
        HrtimControllerPinMap<tkControllerId,
                              tkPinType,
                              kHrtimControllerPinOutputIndex<tkControllerId, tkPinType, tkPortId, tkPinId>>::skAF;
    // ---------------------------------------------------------------------------
    // HRTIM Fault
    // ---------------------------------------------------------------------------

    template <HrtimControllerId tkControllerId, HrtimFaultId tkFaultId, uint8_t tkOutputIdx = 0>
        requires(tkOutputIdx < kHrtimMaxOutputIndex)
    struct HrtimFaultPinMap;

#define DECLARE_HRTIM_FAULT_PIN_MAP(tkControllerId, tkFaultId, tkOutputIdx, port, pin, af) \
    static_assert((tkOutputIdx) < kHrtimMaxOutputIndex);                                   \
    template <>                                                                            \
    struct HrtimFaultPinMap<(tkControllerId), (tkFaultId), (tkOutputIdx)>                  \
    {                                                                                      \
        constexpr static GpioPortId              skPortId = (port);                        \
        constexpr static GpioPinId               skPinId  = (pin);                         \
        constexpr static GpioAlternativeFunction skAF     = (af);                          \
    };

    DECLARE_HRTIM_FAULT_PIN_MAP(HrtimControllerId::kHrtim1,
                                HrtimFaultId::kFault1,
                                0,
                                GpioPortId::kPortA,
                                GpioPinId::kPin12,
                                GpioAlternativeFunction::kAF13);  // PA12
    DECLARE_HRTIM_FAULT_PIN_MAP(HrtimControllerId::kHrtim1,
                                HrtimFaultId::kFault2,
                                0,
                                GpioPortId::kPortA,
                                GpioPinId::kPin15,
                                GpioAlternativeFunction::kAF13);  // PA15
    DECLARE_HRTIM_FAULT_PIN_MAP(HrtimControllerId::kHrtim1,
                                HrtimFaultId::kFault3,
                                0,
                                GpioPortId::kPortB,
                                GpioPinId::kPin10,
                                GpioAlternativeFunction::kAF13);  // PB10
    DECLARE_HRTIM_FAULT_PIN_MAP(HrtimControllerId::kHrtim1,
                                HrtimFaultId::kFault4,
                                0,
                                GpioPortId::kPortB,
                                GpioPinId::kPin11,
                                GpioAlternativeFunction::kAF13);  // PB11
    DECLARE_HRTIM_FAULT_PIN_MAP(HrtimControllerId::kHrtim1,
                                HrtimFaultId::kFault5,
                                0,
                                GpioPortId::kPortB,
                                GpioPinId::kPin0,
                                GpioAlternativeFunction::kAF13);  // PB0
    DECLARE_HRTIM_FAULT_PIN_MAP(HrtimControllerId::kHrtim1,
                                HrtimFaultId::kFault5,
                                1,
                                GpioPortId::kPortC,
                                GpioPinId::kPin7,
                                GpioAlternativeFunction::kAF3);  // PC7

    DECLARE_HRTIM_FAULT_PIN_MAP(HrtimControllerId::kHrtim1,
                                HrtimFaultId::kFault6,
                                0,
                                GpioPortId::kPortC,
                                GpioPinId::kPin10,
                                GpioAlternativeFunction::kAF13);  // PC10

#undef DECLARE_HRTIM_FAULT_PIN_MAP
    namespace detail
    {
        template <HrtimControllerId tkControllerId,
                  HrtimFaultId      tkFaultId,
                  uint8_t           tkOutputIdx,
                  GpioPortId        tkPortId,
                  GpioPinId         tkPinId>
        concept CValidSpecificHrtimFaultPin = requires {
            typename HrtimFaultPinMap<tkControllerId, tkFaultId, tkOutputIdx>;
            { HrtimFaultPinMap<tkControllerId, tkFaultId, tkOutputIdx>::skPortId } -> std::same_as<GpioPortId>;
            { HrtimFaultPinMap<tkControllerId, tkFaultId, tkOutputIdx>::skPinId } -> std::same_as<GpioPinId>;
            HrtimFaultPinMap<tkControllerId, tkFaultId, tkOutputIdx>::skPortId == tkPortId;
            HrtimFaultPinMap<tkControllerId, tkFaultId, tkOutputIdx>::skPinId == tkPinId;
        };

        template <HrtimControllerId tkControllerId,
                  HrtimFaultId      tkFaultId,
                  GpioPortId        tkPortId,
                  GpioPinId         tkPinId,
                  uint8_t... tkIdxs>
        constexpr std::optional<uint8_t> get_hrtim_fault_pin_output_index(std::integer_sequence<uint8_t, tkIdxs...>)
        {
            // This will return the first matching index, or -1 if none match
            std::optional<uint8_t> result = std::nullopt;
            ((CValidSpecificHrtimFaultPin<tkControllerId, tkFaultId, tkIdxs, tkPortId, tkPinId>
                  ? (result = tkIdxs, true)
                  : false) ||
             ...);
            return result;
        }

        template <HrtimControllerId tkControllerId,
                  HrtimFaultId      tkFaultId,
                  GpioPortId        tkPortId,
                  GpioPinId         tkPinId,
                  uint8_t... tkIdxs>
        constexpr bool check_any_valid_hrtim_fault_pin_map(std::integer_sequence<uint8_t, tkIdxs...>)
        {
            // This will return true if ANY of the indices match
            return get_hrtim_fault_pin_output_index<tkControllerId, tkFaultId, tkPortId, tkPinId>(
                       std::integer_sequence<uint8_t, tkIdxs...>{})
                .has_value();
        }
    }  // namespace detail

    template <HrtimControllerId tkControllerId, HrtimFaultId tkFaultId, GpioPortId tkPortId, GpioPinId tkPinId>
    concept CValidHrtimFaultPin =
        detail::check_any_valid_hrtim_fault_pin_map<tkControllerId, tkFaultId, tkPortId, tkPinId>(
            std::make_integer_sequence<uint8_t, kHrtimMaxOutputIndex>{});

    template <HrtimControllerId tkControllerId, HrtimFaultId tkFaultId, GpioPortId tkPortId, GpioPinId tkPinId>
        requires(CValidHrtimFaultPin<tkControllerId, tkFaultId, tkPortId, tkPinId>)
    constexpr uint8_t kHrtimFaultPinOutputIndex =
        detail::get_hrtim_fault_pin_output_index<tkControllerId, tkFaultId, tkPortId, tkPinId>(
            std::make_integer_sequence<uint8_t, kHrtimMaxOutputIndex>{})
            .value();

    template <HrtimControllerId tkControllerId, HrtimFaultId tkFaultId, GpioPortId tkPortId, GpioPinId tkPinId>
        requires(CValidHrtimFaultPin<tkControllerId, tkFaultId, tkPortId, tkPinId>)
    constexpr GpioAlternativeFunction kHrtimFaultPinAF =
        HrtimFaultPinMap<tkControllerId,
                         tkFaultId,
                         kHrtimFaultPinOutputIndex<tkControllerId, tkFaultId, tkPortId, tkPinId>>::skAF;

    // ---------------------------------------------------------------------------
    // HRTIM EEV
    // ---------------------------------------------------------------------------

    template <HrtimControllerId tkControllerId, HrtimExternalEventId tkExternalEventId, uint8_t tkOutputIdx = 0>
        requires(tkOutputIdx < kHrtimMaxOutputIndex)
    struct HrtimExternalEventPinMap;

#define DECLARE_HRTIM_EXTERNAL_EVENT_PIN_MAP(tkControllerId, tkExternalEventId, tkOutputIdx, port, pin, af) \
    static_assert((tkOutputIdx) < kHrtimMaxOutputIndex);                                                    \
    template <>                                                                                             \
    struct HrtimExternalEventPinMap<(tkControllerId), (tkExternalEventId), (tkOutputIdx)>                   \
    {                                                                                                       \
        constexpr static GpioPortId              skPortId = (port);                                         \
        constexpr static GpioPinId               skPinId  = (pin);                                          \
        constexpr static GpioAlternativeFunction skAF     = (af);                                           \
    };

    DECLARE_HRTIM_EXTERNAL_EVENT_PIN_MAP(HrtimControllerId::kHrtim1,
                                         HrtimExternalEventId::kExternalEvent1,
                                         0,
                                         GpioPortId::kPortC,
                                         GpioPinId::kPin12,
                                         GpioAlternativeFunction::kAF3);  // PC12
    DECLARE_HRTIM_EXTERNAL_EVENT_PIN_MAP(HrtimControllerId::kHrtim1,
                                         HrtimExternalEventId::kExternalEvent2,
                                         0,
                                         GpioPortId::kPortC,
                                         GpioPinId::kPin11,
                                         GpioAlternativeFunction::kAF3);  // PC11
    DECLARE_HRTIM_EXTERNAL_EVENT_PIN_MAP(HrtimControllerId::kHrtim1,
                                         HrtimExternalEventId::kExternalEvent3,
                                         0,
                                         GpioPortId::kPortB,
                                         GpioPinId::kPin7,
                                         GpioAlternativeFunction::kAF13);  // PB7
    DECLARE_HRTIM_EXTERNAL_EVENT_PIN_MAP(HrtimControllerId::kHrtim1,
                                         HrtimExternalEventId::kExternalEvent4,
                                         0,
                                         GpioPortId::kPortB,
                                         GpioPinId::kPin6,
                                         GpioAlternativeFunction::kAF13);  // PB6
    DECLARE_HRTIM_EXTERNAL_EVENT_PIN_MAP(HrtimControllerId::kHrtim1,
                                         HrtimExternalEventId::kExternalEvent5,
                                         0,
                                         GpioPortId::kPortB,
                                         GpioPinId::kPin9,
                                         GpioAlternativeFunction::kAF13);  // PB9
    DECLARE_HRTIM_EXTERNAL_EVENT_PIN_MAP(HrtimControllerId::kHrtim1,
                                         HrtimExternalEventId::kExternalEvent6,
                                         0,
                                         GpioPortId::kPortB,
                                         GpioPinId::kPin5,
                                         GpioAlternativeFunction::kAF13);  // PB5
    DECLARE_HRTIM_EXTERNAL_EVENT_PIN_MAP(HrtimControllerId::kHrtim1,
                                         HrtimExternalEventId::kExternalEvent7,
                                         0,
                                         GpioPortId::kPortB,
                                         GpioPinId::kPin4,
                                         GpioAlternativeFunction::kAF13);  // PB4
    DECLARE_HRTIM_EXTERNAL_EVENT_PIN_MAP(HrtimControllerId::kHrtim1,
                                         HrtimExternalEventId::kExternalEvent8,
                                         0,
                                         GpioPortId::kPortB,
                                         GpioPinId::kPin8,
                                         GpioAlternativeFunction::kAF13);  // PB8
    DECLARE_HRTIM_EXTERNAL_EVENT_PIN_MAP(HrtimControllerId::kHrtim1,
                                         HrtimExternalEventId::kExternalEvent9,
                                         0,
                                         GpioPortId::kPortB,
                                         GpioPinId::kPin3,
                                         GpioAlternativeFunction::kAF13);  // PB3
    DECLARE_HRTIM_EXTERNAL_EVENT_PIN_MAP(HrtimControllerId::kHrtim1,
                                         HrtimExternalEventId::kExternalEvent10,
                                         0,
                                         GpioPortId::kPortC,
                                         GpioPinId::kPin5,
                                         GpioAlternativeFunction::kAF13);  // PC5
    DECLARE_HRTIM_EXTERNAL_EVENT_PIN_MAP(HrtimControllerId::kHrtim1,
                                         HrtimExternalEventId::kExternalEvent10,
                                         1,
                                         GpioPortId::kPortC,
                                         GpioPinId::kPin6,
                                         GpioAlternativeFunction::kAF3);  // PC6
#undef DECLARE_HRTIM_EXTERNAL_EVENT_PIN_MAP
    namespace detail
    {
        template <HrtimControllerId    tkControllerId,
                  HrtimExternalEventId tkExternalEventId,
                  uint8_t              tkOutputIdx,
                  GpioPortId           tkPortId,
                  GpioPinId            tkPinId>
        concept CValidSpecificHrtimExternalEventPin = requires {
            typename HrtimExternalEventPinMap<tkControllerId, tkExternalEventId, tkOutputIdx>;
            {
                HrtimExternalEventPinMap<tkControllerId, tkExternalEventId, tkOutputIdx>::skPortId
            } -> std::same_as<GpioPortId>;
            {
                HrtimExternalEventPinMap<tkControllerId, tkExternalEventId, tkOutputIdx>::skPinId
            } -> std::same_as<GpioPinId>;
            HrtimExternalEventPinMap<tkControllerId, tkExternalEventId, tkOutputIdx>::skPortId == tkPortId;
            HrtimExternalEventPinMap<tkControllerId, tkExternalEventId, tkOutputIdx>::skPinId == tkPinId;
        };

        template <HrtimControllerId    tkControllerId,
                  HrtimExternalEventId tkExternalEventId,
                  GpioPortId           tkPortId,
                  GpioPinId            tkPinId,
                  uint8_t... tkIdxs>
        constexpr std::optional<uint8_t> get_hrtim_eev_pin_output_index(std::integer_sequence<uint8_t, tkIdxs...>)
        {
            // This will return the first matching index, or -1 if none match
            std::optional<uint8_t> result = std::nullopt;
            ((CValidSpecificHrtimExternalEventPin<tkControllerId, tkExternalEventId, tkIdxs, tkPortId, tkPinId>
                  ? (result = tkIdxs, true)
                  : false) ||
             ...);
            return result;
        }

        template <HrtimControllerId    tkControllerId,
                  HrtimExternalEventId tkExternalEventId,
                  GpioPortId           tkPortId,
                  GpioPinId            tkPinId,
                  uint8_t... tkIdxs>
        constexpr bool check_any_valid_hrtim_eev_pin_map(std::integer_sequence<uint8_t, tkIdxs...>)
        {
            // This will return true if ANY of the indices match
            return get_hrtim_eev_pin_output_index<tkControllerId, tkExternalEventId, tkPortId, tkPinId>(
                       std::integer_sequence<uint8_t, tkIdxs...>{})
                .has_value();
        }
    }  // namespace detail

    template <HrtimControllerId    tkControllerId,
              HrtimExternalEventId tkExternalEventId,
              GpioPortId           tkPortId,
              GpioPinId            tkPinId>
    concept CValidHrtimExternalEventPin =
        detail::check_any_valid_hrtim_eev_pin_map<tkControllerId, tkExternalEventId, tkPortId, tkPinId>(
            std::make_integer_sequence<uint8_t, kHrtimMaxOutputIndex>{});

    template <HrtimControllerId    tkControllerId,
              HrtimExternalEventId tkExternalEventId,
              GpioPortId           tkPortId,
              GpioPinId            tkPinId>
        requires(CValidHrtimExternalEventPin<tkControllerId, tkExternalEventId, tkPortId, tkPinId>)
    constexpr uint8_t kHrtimExternalEventPinOutputIndex =
        detail::get_hrtim_eev_pin_output_index<tkControllerId, tkExternalEventId, tkPortId, tkPinId>(
            std::make_integer_sequence<uint8_t, kHrtimMaxOutputIndex>{})
            .value();

    template <HrtimControllerId    tkControllerId,
              HrtimExternalEventId tkExternalEventId,
              GpioPortId           tkPortId,
              GpioPinId            tkPinId>
        requires(CValidHrtimExternalEventPin<tkControllerId, tkExternalEventId, tkPortId, tkPinId>)
    constexpr GpioAlternativeFunction kHrtimExternalEventPinAF = HrtimExternalEventPinMap<
        tkControllerId,
        tkExternalEventId,
        kHrtimExternalEventPinOutputIndex<tkControllerId, tkExternalEventId, tkPortId, tkPinId>>::skAF;

    // ---------------------------------------------------------------------------
    // HRTIM TIMER
    // ---------------------------------------------------------------------------

    enum class HrtimTimerGpioPinType : uint8_t
    {
        kOutput1,
        kOutput2,
    };

    template <HrtimControllerId     tkControllerId,
              HrtimTimerId          tkTimerId,
              HrtimTimerGpioPinType tkPinType,
              uint8_t               tkOutputIdx = 0>
        requires(tkOutputIdx < kHrtimMaxOutputIndex)
    struct HrtimTimerPinMap;

#define DECLARE_HRTIM_TIMER_PIN_MAP(tkControllerId, tkTimerId, tkPinType, tkOutputIdx, port, pin, af) \
    static_assert((tkOutputIdx) < kHrtimMaxOutputIndex);                                              \
    template <>                                                                                       \
    struct HrtimTimerPinMap<(tkControllerId), (tkTimerId), (tkPinType), (tkOutputIdx)>                \
    {                                                                                                 \
        constexpr static GpioPortId              skPortId = (port);                                   \
        constexpr static GpioPinId               skPinId  = (pin);                                    \
        constexpr static GpioAlternativeFunction skAF     = (af);                                     \
    };

    DECLARE_HRTIM_TIMER_PIN_MAP(HrtimControllerId::kHrtim1,
                                HrtimTimerId::kTimerA,
                                HrtimTimerGpioPinType::kOutput1,
                                0,
                                GpioPortId::kPortA,
                                GpioPinId::kPin8,
                                GpioAlternativeFunction::kAF13);  // PA8

    DECLARE_HRTIM_TIMER_PIN_MAP(HrtimControllerId::kHrtim1,
                                HrtimTimerId::kTimerA,
                                HrtimTimerGpioPinType::kOutput2,
                                0,
                                GpioPortId::kPortA,
                                GpioPinId::kPin9,
                                GpioAlternativeFunction::kAF13);  // PA9

    DECLARE_HRTIM_TIMER_PIN_MAP(HrtimControllerId::kHrtim1,
                                HrtimTimerId::kTimerB,
                                HrtimTimerGpioPinType::kOutput1,
                                0,
                                GpioPortId::kPortA,
                                GpioPinId::kPin10,
                                GpioAlternativeFunction::kAF13);  // PA10

    DECLARE_HRTIM_TIMER_PIN_MAP(HrtimControllerId::kHrtim1,
                                HrtimTimerId::kTimerB,
                                HrtimTimerGpioPinType::kOutput2,
                                0,
                                GpioPortId::kPortA,
                                GpioPinId::kPin11,
                                GpioAlternativeFunction::kAF13);  // PA11

    DECLARE_HRTIM_TIMER_PIN_MAP(HrtimControllerId::kHrtim1,
                                HrtimTimerId::kTimerC,
                                HrtimTimerGpioPinType::kOutput1,
                                0,
                                GpioPortId::kPortB,
                                GpioPinId::kPin12,
                                GpioAlternativeFunction::kAF13);  // PB12

    DECLARE_HRTIM_TIMER_PIN_MAP(HrtimControllerId::kHrtim1,
                                HrtimTimerId::kTimerC,
                                HrtimTimerGpioPinType::kOutput2,
                                0,
                                GpioPortId::kPortB,
                                GpioPinId::kPin13,
                                GpioAlternativeFunction::kAF13);  // PB13

    DECLARE_HRTIM_TIMER_PIN_MAP(HrtimControllerId::kHrtim1,
                                HrtimTimerId::kTimerD,
                                HrtimTimerGpioPinType::kOutput1,
                                0,
                                GpioPortId::kPortB,
                                GpioPinId::kPin14,
                                GpioAlternativeFunction::kAF13);  // PB14

    DECLARE_HRTIM_TIMER_PIN_MAP(HrtimControllerId::kHrtim1,
                                HrtimTimerId::kTimerD,
                                HrtimTimerGpioPinType::kOutput2,
                                0,
                                GpioPortId::kPortB,
                                GpioPinId::kPin15,
                                GpioAlternativeFunction::kAF13);  // PB15

    DECLARE_HRTIM_TIMER_PIN_MAP(HrtimControllerId::kHrtim1,
                                HrtimTimerId::kTimerE,
                                HrtimTimerGpioPinType::kOutput1,
                                0,
                                GpioPortId::kPortC,
                                GpioPinId::kPin8,
                                GpioAlternativeFunction::kAF3);  // PC8

    DECLARE_HRTIM_TIMER_PIN_MAP(HrtimControllerId::kHrtim1,
                                HrtimTimerId::kTimerE,
                                HrtimTimerGpioPinType::kOutput2,
                                0,
                                GpioPortId::kPortC,
                                GpioPinId::kPin9,
                                GpioAlternativeFunction::kAF3);  // PC9

    DECLARE_HRTIM_TIMER_PIN_MAP(HrtimControllerId::kHrtim1,
                                HrtimTimerId::kTimerF,
                                HrtimTimerGpioPinType::kOutput1,
                                0,
                                GpioPortId::kPortC,
                                GpioPinId::kPin6,
                                GpioAlternativeFunction::kAF13);  // PC6

    DECLARE_HRTIM_TIMER_PIN_MAP(HrtimControllerId::kHrtim1,
                                HrtimTimerId::kTimerF,
                                HrtimTimerGpioPinType::kOutput2,
                                0,
                                GpioPortId::kPortC,
                                GpioPinId::kPin7,
                                GpioAlternativeFunction::kAF13);  // PC7

#undef DECLARE_HRTIM_PIN_MAP
    namespace detail
    {
        template <HrtimControllerId     tkControllerId,
                  HrtimTimerId          tkTimerId,
                  HrtimTimerGpioPinType tkPinType,
                  uint8_t               tkOutputIdx,
                  GpioPortId            tkPortId,
                  GpioPinId             tkPinId>
        concept CValidSpecificHrtimTimerPin = requires {
            typename HrtimTimerPinMap<tkControllerId, tkTimerId, tkPinType, tkOutputIdx>;
            HrtimTimerPinMap<tkControllerId, tkTimerId, tkPinType, tkOutputIdx>::skPortId == tkPortId;
            HrtimTimerPinMap<tkControllerId, tkTimerId, tkPinType, tkOutputIdx>::skPinId == tkPinId;
        };

        template <HrtimControllerId     tkControllerId,
                  HrtimTimerId          tkTimerId,
                  HrtimTimerGpioPinType tkPinType,
                  GpioPortId            tkPortId,
                  GpioPinId             tkPinId,
                  uint8_t... tkIdxs>
        constexpr std::optional<uint8_t> get_hrtim_timer_pin_output_index(std::integer_sequence<uint8_t, tkIdxs...>)
        {
            // This will return the first matching index, or std::nullopt if none match
            std::optional<uint8_t> result = std::nullopt;
            ((CValidSpecificHrtimTimerPin<tkControllerId, tkTimerId, tkPinType, tkIdxs, tkPortId, tkPinId>
                  ? (result = tkIdxs, true)
                  : false) ||
             ...);
            return result;
        }

        template <HrtimControllerId     tkControllerId,
                  HrtimTimerId          tkTimerId,
                  HrtimTimerGpioPinType tkPinType,
                  GpioPortId            tkPortId,
                  GpioPinId             tkPinId,
                  uint8_t... tkIdxs>
        constexpr bool check_any_valid_hrtim_timer_pin_map(std::integer_sequence<uint8_t, tkIdxs...>)
        {
            // This will return true if ANY of the indices match
            return get_hrtim_timer_pin_output_index<tkControllerId, tkTimerId, tkPinType, tkPortId, tkPinId>(
                       std::integer_sequence<uint8_t, tkIdxs...>{})
                .has_value();
        }

        template <HrtimControllerId     tkControllerId,
                  HrtimTimerId          tkTimerId,
                  HrtimTimerGpioPinType tkPinType,
                  GpioPortId            tkPortId,
                  GpioPinId             tkPinId>
        constexpr bool kAnyValidHrtimTimerPinMap =
            check_any_valid_hrtim_timer_pin_map<tkControllerId, tkTimerId, tkPinType, tkPortId, tkPinId>(
                std::make_integer_sequence<uint8_t, kHrtimMaxOutputIndex>{});

    }  // namespace detail

    template <HrtimControllerId     tkControllerId,
              HrtimTimerId          tkTimerId,
              HrtimTimerGpioPinType tkPinType,
              GpioPortId            tkPortId,
              GpioPinId             tkPinId>
    concept CValidHrtimTimerPin =
        detail::check_any_valid_hrtim_timer_pin_map<tkControllerId, tkTimerId, tkPinType, tkPortId, tkPinId>(
            std::make_integer_sequence<uint8_t, kHrtimMaxOutputIndex>{});

    template <HrtimControllerId     tkControllerId,
              HrtimTimerId          tkTimerId,
              HrtimTimerGpioPinType tkPinType,
              GpioPortId            tkPortId,
              GpioPinId             tkPinId>
        requires(CValidHrtimTimerPin<tkControllerId, tkTimerId, tkPinType, tkPortId, tkPinId>)
    constexpr uint8_t kHrtimTimerPinOutputIndex =
        detail::get_hrtim_timer_pin_output_index<tkControllerId, tkTimerId, tkPinType, tkPortId, tkPinId>(
            std::make_integer_sequence<uint8_t, kHrtimMaxOutputIndex>{})
            .value();

    template <HrtimControllerId     tkControllerId,
              HrtimTimerId          tkTimerId,
              HrtimTimerGpioPinType tkPinType,
              GpioPortId            tkPortId,
              GpioPinId             tkPinId>
        requires(CValidHrtimTimerPin<tkControllerId, tkTimerId, tkPinType, tkPortId, tkPinId>)
    constexpr GpioAlternativeFunction kHrtimTimerPinAF =
        HrtimTimerPinMap<tkControllerId,
                         tkTimerId,
                         tkPinType,
                         kHrtimTimerPinOutputIndex<tkControllerId, tkTimerId, tkPinType, tkPortId, tkPinId>>::skAF;

}  // namespace valle::platform