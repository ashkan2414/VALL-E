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

    template <HrtimPeripheralId tkPeripheralId, HrtimControllerGpioPinType tkPinType, uint8_t tkOutputIdx = 0>
        requires(tkOutputIdx < kHrtimMaxOutputIndex)
    struct HrtimControllerPinMap;

#define DECLARE_HRTIM_PIN_MAP(tkPeripheralId, tkPinType, tkOutputIdx, port, pin, af) \
    static_assert((tkOutputIdx) < kHrtimMaxOutputIndex);                             \
    template <>                                                                      \
    struct HrtimControllerPinMap<(tkPeripheralId), (tkPinType), (tkOutputIdx)>       \
    {                                                                                \
        constexpr static GpioPortId              skPortId = (port);                  \
        constexpr static GpioPinId               skPinId  = (pin);                   \
        constexpr static GpioAlternativeFunction skAF     = (af);                    \
    };

    DECLARE_HRTIM_PIN_MAP(HrtimPeripheralId::kHrtim1,
                          HrtimControllerGpioPinType::kSCIn,
                          0,
                          GpioPortId::kPortB,
                          GpioPinId::kPin2,
                          GpioAlternativeFunction::kAF13);  // PB2
    DECLARE_HRTIM_PIN_MAP(HrtimPeripheralId::kHrtim1,
                          HrtimControllerGpioPinType::kSCIn,
                          1,
                          GpioPortId::kPortB,
                          GpioPinId::kPin6,
                          GpioAlternativeFunction::kAF12);  // PB6
    DECLARE_HRTIM_PIN_MAP(HrtimPeripheralId::kHrtim1,
                          HrtimControllerGpioPinType::kSCOut,
                          0,
                          GpioPortId::kPortB,
                          GpioPinId::kPin1,
                          GpioAlternativeFunction::kAF13);  // PB1
    DECLARE_HRTIM_PIN_MAP(HrtimPeripheralId::kHrtim1,
                          HrtimControllerGpioPinType::kSCOut,
                          1,
                          GpioPortId::kPortB,
                          GpioPinId::kPin3,
                          GpioAlternativeFunction::kAF12);  // PB3

#undef DECLARE_HRTIM_PIN_MAP
    namespace detail
    {
        template <HrtimPeripheralId          tkPeripheralId,
                  HrtimControllerGpioPinType tkPinType,
                  uint8_t                    tkOutputIdx,
                  GpioPortId                 tkPortId,
                  GpioPinId                  tkPinId>
        concept CValidSpecificHrtimControllerPin = requires {
            typename HrtimControllerPinMap<tkPeripheralId, tkPinType, tkOutputIdx>;
            { HrtimControllerPinMap<tkPeripheralId, tkPinType, tkOutputIdx>::skPortId } -> std::same_as<GpioPortId>;
            { HrtimControllerPinMap<tkPeripheralId, tkPinType, tkOutputIdx>::skPinId } -> std::same_as<GpioPinId>;
            HrtimControllerPinMap<tkPeripheralId, tkPinType, tkOutputIdx>::skPortId == tkPortId;
            HrtimControllerPinMap<tkPeripheralId, tkPinType, tkOutputIdx>::skPinId == tkPinId;
        };

        template <HrtimPeripheralId          tkPeripheralId,
                  HrtimControllerGpioPinType tkPinType,
                  GpioPortId                 tkPortId,
                  GpioPinId                  tkPinId,
                  uint8_t... tkIdxs>
        constexpr std::optional<uint8_t> get_hrtim_controller_pin_output_index(
            std::integer_sequence<uint8_t, tkIdxs...>)
        {
            // This will return the first matching index, or -1 if none match
            std::optional<uint8_t> result = std::nullopt;
            ((CValidSpecificHrtimControllerPin<tkPeripheralId, tkPinType, tkIdxs, tkPortId, tkPinId>
                  ? (result = tkIdxs, true)
                  : false) ||
             ...);
            return result;
        }

        template <HrtimPeripheralId          tkPeripheralId,
                  HrtimControllerGpioPinType tkPinType,
                  GpioPortId                 tkPortId,
                  GpioPinId                  tkPinId,
                  uint8_t... tkIdxs>
        constexpr bool check_any_valid_hrtim_controller_pin_map(std::integer_sequence<uint8_t, tkIdxs...>)
        {
            // This will return true if ANY of the indices match
            return get_hrtim_controller_pin_output_index<tkPeripheralId, tkPinType, tkPortId, tkPinId>(
                       std::integer_sequence<uint8_t, tkIdxs...>{})
                .has_value();
        }
    }  // namespace detail

    template <HrtimPeripheralId          tkPeripheralId,
              HrtimControllerGpioPinType tkPinType,
              GpioPortId                 tkPortId,
              GpioPinId                  tkPinId>
    concept CValidHrtimControllerPin =
        detail::check_any_valid_hrtim_controller_pin_map<tkPeripheralId, tkPinType, tkPortId, tkPinId>(
            std::make_integer_sequence<uint8_t, kHrtimMaxOutputIndex>{});

    template <HrtimPeripheralId          tkPeripheralId,
              HrtimControllerGpioPinType tkPinType,
              GpioPortId                 tkPortId,
              GpioPinId                  tkPinId>
        requires(CValidHrtimControllerPin<tkPeripheralId, tkPinType, tkPortId, tkPinId>)
    constexpr uint8_t kHrtimControllerPinOutputIndex =
        detail::get_hrtim_controller_pin_output_index<tkPeripheralId, tkPinType, tkPortId, tkPinId>(
            std::make_integer_sequence<uint8_t, kHrtimMaxOutputIndex>{})
            .value();

    template <HrtimPeripheralId          tkPeripheralId,
              HrtimControllerGpioPinType tkPinType,
              GpioPortId                 tkPortId,
              GpioPinId                  tkPinId>
        requires(CValidHrtimControllerPin<tkPeripheralId, tkPinType, tkPortId, tkPinId>)
    constexpr GpioAlternativeFunction kHrtimControllerPinAF =
        HrtimControllerPinMap<tkPeripheralId,
                              tkPinType,
                              kHrtimControllerPinOutputIndex<tkPeripheralId, tkPinType, tkPortId, tkPinId>>::skAF;
    // ---------------------------------------------------------------------------
    // HRTIM Fault
    // ---------------------------------------------------------------------------

    template <HrtimPeripheralId tkPeripheralId, HrtimFaultId tkFaultId, uint8_t tkOutputIdx = 0>
        requires(tkOutputIdx < kHrtimMaxOutputIndex)
    struct HrtimFaultPinMap;

#define DECLARE_HRTIM_FAULT_PIN_MAP(tkPeripheralId, tkFaultId, tkOutputIdx, port, pin, af) \
    static_assert((tkOutputIdx) < kHrtimMaxOutputIndex);                                   \
    template <>                                                                            \
    struct HrtimFaultPinMap<(tkPeripheralId), (tkFaultId), (tkOutputIdx)>                  \
    {                                                                                      \
        constexpr static GpioPortId              skPortId = (port);                        \
        constexpr static GpioPinId               skPinId  = (pin);                         \
        constexpr static GpioAlternativeFunction skAF     = (af);                          \
    };

    DECLARE_HRTIM_FAULT_PIN_MAP(HrtimPeripheralId::kHrtim1,
                                HrtimFaultId::kFault1,
                                0,
                                GpioPortId::kPortA,
                                GpioPinId::kPin12,
                                GpioAlternativeFunction::kAF13);  // PA12
    DECLARE_HRTIM_FAULT_PIN_MAP(HrtimPeripheralId::kHrtim1,
                                HrtimFaultId::kFault2,
                                0,
                                GpioPortId::kPortA,
                                GpioPinId::kPin15,
                                GpioAlternativeFunction::kAF13);  // PA15
    DECLARE_HRTIM_FAULT_PIN_MAP(HrtimPeripheralId::kHrtim1,
                                HrtimFaultId::kFault3,
                                0,
                                GpioPortId::kPortB,
                                GpioPinId::kPin10,
                                GpioAlternativeFunction::kAF13);  // PB10
    DECLARE_HRTIM_FAULT_PIN_MAP(HrtimPeripheralId::kHrtim1,
                                HrtimFaultId::kFault4,
                                0,
                                GpioPortId::kPortB,
                                GpioPinId::kPin11,
                                GpioAlternativeFunction::kAF13);  // PB11
    DECLARE_HRTIM_FAULT_PIN_MAP(HrtimPeripheralId::kHrtim1,
                                HrtimFaultId::kFault5,
                                0,
                                GpioPortId::kPortB,
                                GpioPinId::kPin0,
                                GpioAlternativeFunction::kAF13);  // PB0
    DECLARE_HRTIM_FAULT_PIN_MAP(HrtimPeripheralId::kHrtim1,
                                HrtimFaultId::kFault5,
                                1,
                                GpioPortId::kPortC,
                                GpioPinId::kPin7,
                                GpioAlternativeFunction::kAF3);  // PC7

    DECLARE_HRTIM_FAULT_PIN_MAP(HrtimPeripheralId::kHrtim1,
                                HrtimFaultId::kFault6,
                                0,
                                GpioPortId::kPortC,
                                GpioPinId::kPin10,
                                GpioAlternativeFunction::kAF13);  // PC10

#undef DECLARE_HRTIM_FAULT_PIN_MAP
    namespace detail
    {
        template <HrtimPeripheralId tkPeripheralId,
                  HrtimFaultId      tkFaultId,
                  uint8_t           tkOutputIdx,
                  GpioPortId        tkPortId,
                  GpioPinId         tkPinId>
        concept CValidSpecificHrtimFaultPin = requires {
            typename HrtimFaultPinMap<tkPeripheralId, tkFaultId, tkOutputIdx>;
            { HrtimFaultPinMap<tkPeripheralId, tkFaultId, tkOutputIdx>::skPortId } -> std::same_as<GpioPortId>;
            { HrtimFaultPinMap<tkPeripheralId, tkFaultId, tkOutputIdx>::skPinId } -> std::same_as<GpioPinId>;
            HrtimFaultPinMap<tkPeripheralId, tkFaultId, tkOutputIdx>::skPortId == tkPortId;
            HrtimFaultPinMap<tkPeripheralId, tkFaultId, tkOutputIdx>::skPinId == tkPinId;
        };

        template <HrtimPeripheralId tkPeripheralId,
                  HrtimFaultId      tkFaultId,
                  GpioPortId        tkPortId,
                  GpioPinId         tkPinId,
                  uint8_t... tkIdxs>
        constexpr std::optional<uint8_t> get_hrtim_fault_pin_output_index(std::integer_sequence<uint8_t, tkIdxs...>)
        {
            // This will return the first matching index, or -1 if none match
            std::optional<uint8_t> result = std::nullopt;
            ((CValidSpecificHrtimFaultPin<tkPeripheralId, tkFaultId, tkIdxs, tkPortId, tkPinId>
                  ? (result = tkIdxs, true)
                  : false) ||
             ...);
            return result;
        }

        template <HrtimPeripheralId tkPeripheralId,
                  HrtimFaultId      tkFaultId,
                  GpioPortId        tkPortId,
                  GpioPinId         tkPinId,
                  uint8_t... tkIdxs>
        constexpr bool check_any_valid_hrtim_fault_pin_map(std::integer_sequence<uint8_t, tkIdxs...>)
        {
            // This will return true if ANY of the indices match
            return get_hrtim_fault_pin_output_index<tkPeripheralId, tkFaultId, tkPortId, tkPinId>(
                       std::integer_sequence<uint8_t, tkIdxs...>{})
                .has_value();
        }
    }  // namespace detail

    template <HrtimPeripheralId tkPeripheralId, HrtimFaultId tkFaultId, GpioPortId tkPortId, GpioPinId tkPinId>
    concept CValidHrtimFaultPin =
        detail::check_any_valid_hrtim_fault_pin_map<tkPeripheralId, tkFaultId, tkPortId, tkPinId>(
            std::make_integer_sequence<uint8_t, kHrtimMaxOutputIndex>{});

    template <HrtimPeripheralId tkPeripheralId, HrtimFaultId tkFaultId, GpioPortId tkPortId, GpioPinId tkPinId>
        requires(CValidHrtimFaultPin<tkPeripheralId, tkFaultId, tkPortId, tkPinId>)
    constexpr uint8_t kHrtimFaultPinOutputIndex =
        detail::get_hrtim_fault_pin_output_index<tkPeripheralId, tkFaultId, tkPortId, tkPinId>(
            std::make_integer_sequence<uint8_t, kHrtimMaxOutputIndex>{})
            .value();

    template <HrtimPeripheralId tkPeripheralId, HrtimFaultId tkFaultId, GpioPortId tkPortId, GpioPinId tkPinId>
        requires(CValidHrtimFaultPin<tkPeripheralId, tkFaultId, tkPortId, tkPinId>)
    constexpr GpioAlternativeFunction kHrtimFaultPinAF =
        HrtimFaultPinMap<tkPeripheralId,
                         tkFaultId,
                         kHrtimFaultPinOutputIndex<tkPeripheralId, tkFaultId, tkPortId, tkPinId>>::skAF;

    // ---------------------------------------------------------------------------
    // HRTIM EEV
    // ---------------------------------------------------------------------------

    template <HrtimPeripheralId tkPeripheralId, HrtimExternalEventId tkExternalEventId, uint8_t tkOutputIdx = 0>
        requires(tkOutputIdx < kHrtimMaxOutputIndex)
    struct HrtimExternalEventPinMap;

#define DECLARE_HRTIM_EXTERNAL_EVENT_PIN_MAP(tkPeripheralId, tkExternalEventId, tkOutputIdx, port, pin, af) \
    static_assert((tkOutputIdx) < kHrtimMaxOutputIndex);                                                    \
    template <>                                                                                             \
    struct HrtimExternalEventPinMap<(tkPeripheralId), (tkExternalEventId), (tkOutputIdx)>                   \
    {                                                                                                       \
        constexpr static GpioPortId              skPortId = (port);                                         \
        constexpr static GpioPinId               skPinId  = (pin);                                          \
        constexpr static GpioAlternativeFunction skAF     = (af);                                           \
    };

    DECLARE_HRTIM_EXTERNAL_EVENT_PIN_MAP(HrtimPeripheralId::kHrtim1,
                                         HrtimExternalEventId::kExternalEvent1,
                                         0,
                                         GpioPortId::kPortC,
                                         GpioPinId::kPin12,
                                         GpioAlternativeFunction::kAF3);  // PC12
    DECLARE_HRTIM_EXTERNAL_EVENT_PIN_MAP(HrtimPeripheralId::kHrtim1,
                                         HrtimExternalEventId::kExternalEvent2,
                                         0,
                                         GpioPortId::kPortC,
                                         GpioPinId::kPin11,
                                         GpioAlternativeFunction::kAF3);  // PC11
    DECLARE_HRTIM_EXTERNAL_EVENT_PIN_MAP(HrtimPeripheralId::kHrtim1,
                                         HrtimExternalEventId::kExternalEvent3,
                                         0,
                                         GpioPortId::kPortB,
                                         GpioPinId::kPin7,
                                         GpioAlternativeFunction::kAF13);  // PB7
    DECLARE_HRTIM_EXTERNAL_EVENT_PIN_MAP(HrtimPeripheralId::kHrtim1,
                                         HrtimExternalEventId::kExternalEvent4,
                                         0,
                                         GpioPortId::kPortB,
                                         GpioPinId::kPin6,
                                         GpioAlternativeFunction::kAF13);  // PB6
    DECLARE_HRTIM_EXTERNAL_EVENT_PIN_MAP(HrtimPeripheralId::kHrtim1,
                                         HrtimExternalEventId::kExternalEvent5,
                                         0,
                                         GpioPortId::kPortB,
                                         GpioPinId::kPin9,
                                         GpioAlternativeFunction::kAF13);  // PB9
    DECLARE_HRTIM_EXTERNAL_EVENT_PIN_MAP(HrtimPeripheralId::kHrtim1,
                                         HrtimExternalEventId::kExternalEvent6,
                                         0,
                                         GpioPortId::kPortB,
                                         GpioPinId::kPin5,
                                         GpioAlternativeFunction::kAF13);  // PB5
    DECLARE_HRTIM_EXTERNAL_EVENT_PIN_MAP(HrtimPeripheralId::kHrtim1,
                                         HrtimExternalEventId::kExternalEvent7,
                                         0,
                                         GpioPortId::kPortB,
                                         GpioPinId::kPin4,
                                         GpioAlternativeFunction::kAF13);  // PB4
    DECLARE_HRTIM_EXTERNAL_EVENT_PIN_MAP(HrtimPeripheralId::kHrtim1,
                                         HrtimExternalEventId::kExternalEvent8,
                                         0,
                                         GpioPortId::kPortB,
                                         GpioPinId::kPin8,
                                         GpioAlternativeFunction::kAF13);  // PB8
    DECLARE_HRTIM_EXTERNAL_EVENT_PIN_MAP(HrtimPeripheralId::kHrtim1,
                                         HrtimExternalEventId::kExternalEvent9,
                                         0,
                                         GpioPortId::kPortB,
                                         GpioPinId::kPin3,
                                         GpioAlternativeFunction::kAF13);  // PB3
    DECLARE_HRTIM_EXTERNAL_EVENT_PIN_MAP(HrtimPeripheralId::kHrtim1,
                                         HrtimExternalEventId::kExternalEvent10,
                                         0,
                                         GpioPortId::kPortC,
                                         GpioPinId::kPin5,
                                         GpioAlternativeFunction::kAF13);  // PC5
    DECLARE_HRTIM_EXTERNAL_EVENT_PIN_MAP(HrtimPeripheralId::kHrtim1,
                                         HrtimExternalEventId::kExternalEvent10,
                                         1,
                                         GpioPortId::kPortC,
                                         GpioPinId::kPin6,
                                         GpioAlternativeFunction::kAF3);  // PC6
#undef DECLARE_HRTIM_EXTERNAL_EVENT_PIN_MAP
    namespace detail
    {
        template <HrtimPeripheralId    tkPeripheralId,
                  HrtimExternalEventId tkExternalEventId,
                  uint8_t              tkOutputIdx,
                  GpioPortId           tkPortId,
                  GpioPinId            tkPinId>
        concept CValidSpecificHrtimExternalEventPin = requires {
            typename HrtimExternalEventPinMap<tkPeripheralId, tkExternalEventId, tkOutputIdx>;
            {
                HrtimExternalEventPinMap<tkPeripheralId, tkExternalEventId, tkOutputIdx>::skPortId
            } -> std::same_as<GpioPortId>;
            {
                HrtimExternalEventPinMap<tkPeripheralId, tkExternalEventId, tkOutputIdx>::skPinId
            } -> std::same_as<GpioPinId>;
            HrtimExternalEventPinMap<tkPeripheralId, tkExternalEventId, tkOutputIdx>::skPortId == tkPortId;
            HrtimExternalEventPinMap<tkPeripheralId, tkExternalEventId, tkOutputIdx>::skPinId == tkPinId;
        };

        template <HrtimPeripheralId    tkPeripheralId,
                  HrtimExternalEventId tkExternalEventId,
                  GpioPortId           tkPortId,
                  GpioPinId            tkPinId,
                  uint8_t... tkIdxs>
        constexpr std::optional<uint8_t> get_hrtim_eev_pin_output_index(std::integer_sequence<uint8_t, tkIdxs...>)
        {
            // This will return the first matching index, or -1 if none match
            std::optional<uint8_t> result = std::nullopt;
            ((CValidSpecificHrtimExternalEventPin<tkPeripheralId, tkExternalEventId, tkIdxs, tkPortId, tkPinId>
                  ? (result = tkIdxs, true)
                  : false) ||
             ...);
            return result;
        }

        template <HrtimPeripheralId    tkPeripheralId,
                  HrtimExternalEventId tkExternalEventId,
                  GpioPortId           tkPortId,
                  GpioPinId            tkPinId,
                  uint8_t... tkIdxs>
        constexpr bool check_any_valid_hrtim_eev_pin_map(std::integer_sequence<uint8_t, tkIdxs...>)
        {
            // This will return true if ANY of the indices match
            return get_hrtim_eev_pin_output_index<tkPeripheralId, tkExternalEventId, tkPortId, tkPinId>(
                       std::integer_sequence<uint8_t, tkIdxs...>{})
                .has_value();
        }
    }  // namespace detail

    template <HrtimPeripheralId    tkPeripheralId,
              HrtimExternalEventId tkExternalEventId,
              GpioPortId           tkPortId,
              GpioPinId            tkPinId>
    concept CValidHrtimExternalEventPin =
        detail::check_any_valid_hrtim_eev_pin_map<tkPeripheralId, tkExternalEventId, tkPortId, tkPinId>(
            std::make_integer_sequence<uint8_t, kHrtimMaxOutputIndex>{});

    template <HrtimPeripheralId    tkPeripheralId,
              HrtimExternalEventId tkExternalEventId,
              GpioPortId           tkPortId,
              GpioPinId            tkPinId>
        requires(CValidHrtimExternalEventPin<tkPeripheralId, tkExternalEventId, tkPortId, tkPinId>)
    constexpr uint8_t kHrtimExternalEventPinOutputIndex =
        detail::get_hrtim_eev_pin_output_index<tkPeripheralId, tkExternalEventId, tkPortId, tkPinId>(
            std::make_integer_sequence<uint8_t, kHrtimMaxOutputIndex>{})
            .value();

    template <HrtimPeripheralId    tkPeripheralId,
              HrtimExternalEventId tkExternalEventId,
              GpioPortId           tkPortId,
              GpioPinId            tkPinId>
        requires(CValidHrtimExternalEventPin<tkPeripheralId, tkExternalEventId, tkPortId, tkPinId>)
    constexpr GpioAlternativeFunction kHrtimExternalEventPinAF = HrtimExternalEventPinMap<
        tkPeripheralId,
        tkExternalEventId,
        kHrtimExternalEventPinOutputIndex<tkPeripheralId, tkExternalEventId, tkPortId, tkPinId>>::skAF;

    // ---------------------------------------------------------------------------
    // HRTIM TIMER
    // ---------------------------------------------------------------------------

    enum class HrtimTimerGpioPinType : uint8_t
    {
        kOutput1,
        kOutput2,
    };

    template <HrtimPeripheralId     tkPeripheralId,
              HrtimTimerId          tkTimerId,
              HrtimTimerGpioPinType tkPinType,
              uint8_t               tkOutputIdx = 0>
        requires(tkOutputIdx < kHrtimMaxOutputIndex)
    struct HrtimTimerPinMap;

#define DECLARE_HRTIM_TIMER_PIN_MAP(tkPeripheralId, tkTimerId, tkPinType, tkOutputIdx, port, pin, af) \
    static_assert((tkOutputIdx) < kHrtimMaxOutputIndex);                                              \
    template <>                                                                                       \
    struct HrtimTimerPinMap<(tkPeripheralId), (tkTimerId), (tkPinType), (tkOutputIdx)>                \
    {                                                                                                 \
        constexpr static GpioPortId              skPortId = (port);                                   \
        constexpr static GpioPinId               skPinId  = (pin);                                    \
        constexpr static GpioAlternativeFunction skAF     = (af);                                     \
    };

    DECLARE_HRTIM_TIMER_PIN_MAP(HrtimPeripheralId::kHrtim1,
                                HrtimTimerId::kTimerA,
                                HrtimTimerGpioPinType::kOutput1,
                                0,
                                GpioPortId::kPortA,
                                GpioPinId::kPin8,
                                GpioAlternativeFunction::kAF13);  // PA8

    DECLARE_HRTIM_TIMER_PIN_MAP(HrtimPeripheralId::kHrtim1,
                                HrtimTimerId::kTimerA,
                                HrtimTimerGpioPinType::kOutput2,
                                0,
                                GpioPortId::kPortA,
                                GpioPinId::kPin9,
                                GpioAlternativeFunction::kAF13);  // PA9

    DECLARE_HRTIM_TIMER_PIN_MAP(HrtimPeripheralId::kHrtim1,
                                HrtimTimerId::kTimerB,
                                HrtimTimerGpioPinType::kOutput1,
                                0,
                                GpioPortId::kPortA,
                                GpioPinId::kPin10,
                                GpioAlternativeFunction::kAF13);  // PA10

    DECLARE_HRTIM_TIMER_PIN_MAP(HrtimPeripheralId::kHrtim1,
                                HrtimTimerId::kTimerB,
                                HrtimTimerGpioPinType::kOutput2,
                                0,
                                GpioPortId::kPortA,
                                GpioPinId::kPin11,
                                GpioAlternativeFunction::kAF13);  // PA11

    DECLARE_HRTIM_TIMER_PIN_MAP(HrtimPeripheralId::kHrtim1,
                                HrtimTimerId::kTimerC,
                                HrtimTimerGpioPinType::kOutput1,
                                0,
                                GpioPortId::kPortB,
                                GpioPinId::kPin12,
                                GpioAlternativeFunction::kAF13);  // PB12

    DECLARE_HRTIM_TIMER_PIN_MAP(HrtimPeripheralId::kHrtim1,
                                HrtimTimerId::kTimerC,
                                HrtimTimerGpioPinType::kOutput2,
                                0,
                                GpioPortId::kPortB,
                                GpioPinId::kPin13,
                                GpioAlternativeFunction::kAF13);  // PB13

    DECLARE_HRTIM_TIMER_PIN_MAP(HrtimPeripheralId::kHrtim1,
                                HrtimTimerId::kTimerD,
                                HrtimTimerGpioPinType::kOutput1,
                                0,
                                GpioPortId::kPortB,
                                GpioPinId::kPin14,
                                GpioAlternativeFunction::kAF13);  // PB14

    DECLARE_HRTIM_TIMER_PIN_MAP(HrtimPeripheralId::kHrtim1,
                                HrtimTimerId::kTimerD,
                                HrtimTimerGpioPinType::kOutput2,
                                0,
                                GpioPortId::kPortB,
                                GpioPinId::kPin15,
                                GpioAlternativeFunction::kAF13);  // PB15

    DECLARE_HRTIM_TIMER_PIN_MAP(HrtimPeripheralId::kHrtim1,
                                HrtimTimerId::kTimerE,
                                HrtimTimerGpioPinType::kOutput1,
                                0,
                                GpioPortId::kPortC,
                                GpioPinId::kPin8,
                                GpioAlternativeFunction::kAF3);  // PC8

    DECLARE_HRTIM_TIMER_PIN_MAP(HrtimPeripheralId::kHrtim1,
                                HrtimTimerId::kTimerE,
                                HrtimTimerGpioPinType::kOutput2,
                                0,
                                GpioPortId::kPortC,
                                GpioPinId::kPin9,
                                GpioAlternativeFunction::kAF3);  // PC9

    DECLARE_HRTIM_TIMER_PIN_MAP(HrtimPeripheralId::kHrtim1,
                                HrtimTimerId::kTimerF,
                                HrtimTimerGpioPinType::kOutput1,
                                0,
                                GpioPortId::kPortC,
                                GpioPinId::kPin6,
                                GpioAlternativeFunction::kAF13);  // PC6

    DECLARE_HRTIM_TIMER_PIN_MAP(HrtimPeripheralId::kHrtim1,
                                HrtimTimerId::kTimerF,
                                HrtimTimerGpioPinType::kOutput2,
                                0,
                                GpioPortId::kPortC,
                                GpioPinId::kPin7,
                                GpioAlternativeFunction::kAF13);  // PC7

#undef DECLARE_HRTIM_PIN_MAP
    namespace detail
    {
        template <HrtimPeripheralId     tkPeripheralId,
                  HrtimTimerId          tkTimerId,
                  HrtimTimerGpioPinType tkPinType,
                  uint8_t               tkOutputIdx,
                  GpioPortId            tkPortId,
                  GpioPinId             tkPinId>
        concept CValidSpecificHrtimTimerPin = requires {
            typename HrtimTimerPinMap<tkPeripheralId, tkTimerId, tkPinType, tkOutputIdx>;
            HrtimTimerPinMap<tkPeripheralId, tkTimerId, tkPinType, tkOutputIdx>::skPortId == tkPortId;
            HrtimTimerPinMap<tkPeripheralId, tkTimerId, tkPinType, tkOutputIdx>::skPinId == tkPinId;
        };

        template <HrtimPeripheralId     tkPeripheralId,
                  HrtimTimerId          tkTimerId,
                  HrtimTimerGpioPinType tkPinType,
                  GpioPortId            tkPortId,
                  GpioPinId             tkPinId,
                  uint8_t... tkIdxs>
        constexpr std::optional<uint8_t> get_hrtim_timer_pin_output_index(std::integer_sequence<uint8_t, tkIdxs...>)
        {
            // This will return the first matching index, or std::nullopt if none match
            std::optional<uint8_t> result = std::nullopt;
            ((CValidSpecificHrtimTimerPin<tkPeripheralId, tkTimerId, tkPinType, tkIdxs, tkPortId, tkPinId>
                  ? (result = tkIdxs, true)
                  : false) ||
             ...);
            return result;
        }

        template <HrtimPeripheralId     tkPeripheralId,
                  HrtimTimerId          tkTimerId,
                  HrtimTimerGpioPinType tkPinType,
                  GpioPortId            tkPortId,
                  GpioPinId             tkPinId,
                  uint8_t... tkIdxs>
        constexpr bool check_any_valid_hrtim_timer_pin_map(std::integer_sequence<uint8_t, tkIdxs...>)
        {
            // This will return true if ANY of the indices match
            return get_hrtim_timer_pin_output_index<tkPeripheralId, tkTimerId, tkPinType, tkPortId, tkPinId>(
                       std::integer_sequence<uint8_t, tkIdxs...>{})
                .has_value();
        }

        template <HrtimPeripheralId     tkPeripheralId,
                  HrtimTimerId          tkTimerId,
                  HrtimTimerGpioPinType tkPinType,
                  GpioPortId            tkPortId,
                  GpioPinId             tkPinId>
        constexpr bool kAnyValidHrtimTimerPinMap =
            check_any_valid_hrtim_timer_pin_map<tkPeripheralId, tkTimerId, tkPinType, tkPortId, tkPinId>(
                std::make_integer_sequence<uint8_t, kHrtimMaxOutputIndex>{});

    }  // namespace detail

    template <HrtimPeripheralId     tkPeripheralId,
              HrtimTimerId          tkTimerId,
              HrtimTimerGpioPinType tkPinType,
              GpioPortId            tkPortId,
              GpioPinId             tkPinId>
    concept CValidHrtimTimerPin =
        detail::check_any_valid_hrtim_timer_pin_map<tkPeripheralId, tkTimerId, tkPinType, tkPortId, tkPinId>(
            std::make_integer_sequence<uint8_t, kHrtimMaxOutputIndex>{});

    template <HrtimPeripheralId     tkPeripheralId,
              HrtimTimerId          tkTimerId,
              HrtimTimerGpioPinType tkPinType,
              GpioPortId            tkPortId,
              GpioPinId             tkPinId>
        requires(CValidHrtimTimerPin<tkPeripheralId, tkTimerId, tkPinType, tkPortId, tkPinId>)
    constexpr uint8_t kHrtimTimerPinOutputIndex =
        detail::get_hrtim_timer_pin_output_index<tkPeripheralId, tkTimerId, tkPinType, tkPortId, tkPinId>(
            std::make_integer_sequence<uint8_t, kHrtimMaxOutputIndex>{})
            .value();

    template <HrtimPeripheralId     tkPeripheralId,
              HrtimTimerId          tkTimerId,
              HrtimTimerGpioPinType tkPinType,
              GpioPortId            tkPortId,
              GpioPinId             tkPinId>
        requires(CValidHrtimTimerPin<tkPeripheralId, tkTimerId, tkPinType, tkPortId, tkPinId>)
    constexpr GpioAlternativeFunction kHrtimTimerPinAF =
        HrtimTimerPinMap<tkPeripheralId,
                         tkTimerId,
                         tkPinType,
                         kHrtimTimerPinOutputIndex<tkPeripheralId, tkTimerId, tkPinType, tkPortId, tkPinId>>::skAF;

}  // namespace valle::platform