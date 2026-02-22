#pragma once

#include "Valle/Board/Traits/HRTIM/id.hpp"
#include "Valle/Board/Traits/gpio.hpp"

namespace valle
{
    // ---------------------------------------------------------------------------
    // HRTIM CONTROLLER
    // ---------------------------------------------------------------------------
    enum class HRTIMControllerGPIOPinType : uint8_t
    {
        kSCIn,
        kSCOut,
    };

    constexpr static uint8_t kHRTIMMaxOutputIndex = 2;

    template <HRTIMControllerID tkControllerID, HRTIMControllerGPIOPinType tkPinType, uint8_t tkOutputIdx = 0>
        requires(tkOutputIdx < kHRTIMMaxOutputIndex)
    struct HRTIMControllerPinMap;

#define DECLARE_HRTIM_PIN_MAP(tkControllerID, tkPinType, tkOutputIdx, port, pin, af) \
    static_assert((tkOutputIdx) < kHRTIMMaxOutputIndex);                             \
    template <>                                                                      \
    struct HRTIMControllerPinMap<(tkControllerID), (tkPinType), (tkOutputIdx)>       \
    {                                                                                \
        constexpr static GPIOPortID              skPortID = (port);                  \
        constexpr static GPIOPinID               skPinID  = (pin);                   \
        constexpr static GPIOAlternativeFunction skAF     = (af);                    \
    };

    DECLARE_HRTIM_PIN_MAP(
        1, HRTIMControllerGPIOPinType::kSCIn, 0, GPIOPortID::kB, 2, GPIOAlternativeFunction::kAF13);  // PB2
    DECLARE_HRTIM_PIN_MAP(
        1, HRTIMControllerGPIOPinType::kSCIn, 1, GPIOPortID::kB, 6, GPIOAlternativeFunction::kAF12);  // PB6
    DECLARE_HRTIM_PIN_MAP(
        1, HRTIMControllerGPIOPinType::kSCOut, 0, GPIOPortID::kB, 1, GPIOAlternativeFunction::kAF13);  // PB1
    DECLARE_HRTIM_PIN_MAP(
        1, HRTIMControllerGPIOPinType::kSCOut, 1, GPIOPortID::kB, 3, GPIOAlternativeFunction::kAF12);  // PB3

#undef DECLARE_HRTIM_PIN_MAP
    namespace detail
    {
        template <HRTIMControllerID          tkControllerID,
                  HRTIMControllerGPIOPinType tkPinType,
                  uint8_t                    tkOutputIdx,
                  GPIOPortID                 tkPortID,
                  GPIOPinID                  tkPinID>
        concept CValidSpecificHRTIMControllerPin = requires {
            typename HRTIMControllerPinMap<tkControllerID, tkPinType, tkOutputIdx>;
            { HRTIMControllerPinMap<tkControllerID, tkPinType, tkOutputIdx>::skPortID } -> std::same_as<GPIOPortID>;
            { HRTIMControllerPinMap<tkControllerID, tkPinType, tkOutputIdx>::skPinID } -> std::same_as<GPIOPinID>;
            HRTIMControllerPinMap<tkControllerID, tkPinType, tkOutputIdx>::skPortID == tkPortID;
            HRTIMControllerPinMap<tkControllerID, tkPinType, tkOutputIdx>::skPinID == tkPinID;
        };

        template <HRTIMControllerID          tkControllerID,
                  HRTIMControllerGPIOPinType tkPinType,
                  GPIOPortID                 tkPortID,
                  GPIOPinID                  tkPinID,
                  uint8_t... tkIdxs>
        constexpr std::optional<uint8_t> get_hrtim_controller_pin_output_index(
            std::integer_sequence<uint8_t, tkIdxs...>)
        {
            // This will return the first matching index, or -1 if none match
            std::optional<uint8_t> result = std::nullopt;
            ((CValidSpecificHRTIMControllerPin<tkControllerID, tkPinType, tkIdxs, tkPortID, tkPinID>
                  ? (result = tkIdxs, true)
                  : false) ||
             ...);
            return result;
        }

        template <HRTIMControllerID          tkControllerID,
                  HRTIMControllerGPIOPinType tkPinType,
                  GPIOPortID                 tkPortID,
                  GPIOPinID                  tkPinID,
                  uint8_t... tkIdxs>
        constexpr bool check_any_valid_hrtim_controller_pin_map(std::integer_sequence<uint8_t, tkIdxs...>)
        {
            // This will return true if ANY of the indices match
            return get_hrtim_controller_pin_output_index<tkControllerID, tkPinType, tkPortID, tkPinID>(
                       std::integer_sequence<uint8_t, tkIdxs...>{})
                .has_value();
        }
    }  // namespace detail

    template <HRTIMControllerID          tkControllerID,
              HRTIMControllerGPIOPinType tkPinType,
              GPIOPortID                 tkPortID,
              GPIOPinID                  tkPinID>
    concept CValidHRTIMControllerPin =
        detail::check_any_valid_hrtim_controller_pin_map<tkControllerID, tkPinType, tkPortID, tkPinID>(
            std::make_integer_sequence<uint8_t, kHRTIMMaxOutputIndex>{});

    template <HRTIMControllerID          tkControllerID,
              HRTIMControllerGPIOPinType tkPinType,
              GPIOPortID                 tkPortID,
              GPIOPinID                  tkPinID>
        requires(CValidHRTIMControllerPin<tkControllerID, tkPinType, tkPortID, tkPinID>)
    constexpr uint8_t kHRTIMControllerPinOutputIndex =
        detail::get_hrtim_controller_pin_output_index<tkControllerID, tkPinType, tkPortID, tkPinID>(
            std::make_integer_sequence<uint8_t, kHRTIMMaxOutputIndex>{})
            .value();

    template <HRTIMControllerID          tkControllerID,
              HRTIMControllerGPIOPinType tkPinType,
              GPIOPortID                 tkPortID,
              GPIOPinID                  tkPinID>
        requires(CValidHRTIMControllerPin<tkControllerID, tkPinType, tkPortID, tkPinID>)
    constexpr GPIOAlternativeFunction kHRTIMControllerPinAF =
        HRTIMControllerPinMap<tkControllerID,
                              tkPinType,
                              kHRTIMControllerPinOutputIndex<tkControllerID, tkPinType, tkPortID, tkPinID>>::skAF;
    // ---------------------------------------------------------------------------
    // HRTIM Fault
    // ---------------------------------------------------------------------------

    template <HRTIMControllerID tkControllerID, HRTIMFaultID tkFaultID, uint8_t tkOutputIdx = 0>
        requires(tkOutputIdx < kHRTIMMaxOutputIndex)
    struct HRTIMFaultPinMap;

#define DECLARE_HRTIM_FAULT_PIN_MAP(tkControllerID, tkFaultID, tkOutputIdx, port, pin, af) \
    static_assert((tkOutputIdx) < kHRTIMMaxOutputIndex);                                   \
    template <>                                                                            \
    struct HRTIMFaultPinMap<(tkControllerID), (tkFaultID), (tkOutputIdx)>                  \
    {                                                                                      \
        constexpr static GPIOPortID              skPortID = (port);                        \
        constexpr static GPIOPinID               skPinID  = (pin);                         \
        constexpr static GPIOAlternativeFunction skAF     = (af);                          \
    };

    DECLARE_HRTIM_FAULT_PIN_MAP(
        1, HRTIMFaultID::kFault1, 0, GPIOPortID::kA, 12, GPIOAlternativeFunction::kAF13);  // PA12
    DECLARE_HRTIM_FAULT_PIN_MAP(
        1, HRTIMFaultID::kFault2, 0, GPIOPortID::kA, 15, GPIOAlternativeFunction::kAF13);  // PA15
    DECLARE_HRTIM_FAULT_PIN_MAP(
        1, HRTIMFaultID::kFault3, 0, GPIOPortID::kB, 10, GPIOAlternativeFunction::kAF13);  // PB10
    DECLARE_HRTIM_FAULT_PIN_MAP(
        1, HRTIMFaultID::kFault4, 0, GPIOPortID::kB, 11, GPIOAlternativeFunction::kAF13);  // PB11
    DECLARE_HRTIM_FAULT_PIN_MAP(1, HRTIMFaultID::kFault5, 0, GPIOPortID::kB, 0, GPIOAlternativeFunction::kAF13);  // PB0
    DECLARE_HRTIM_FAULT_PIN_MAP(1, HRTIMFaultID::kFault5, 1, GPIOPortID::kC, 7, GPIOAlternativeFunction::kAF3);   // PC7
    DECLARE_HRTIM_FAULT_PIN_MAP(
        1, HRTIMFaultID::kFault6, 0, GPIOPortID::kC, 10, GPIOAlternativeFunction::kAF13);  // PC10

#undef DECLARE_HRTIM_FAULT_PIN_MAP
    namespace detail
    {
        template <HRTIMControllerID tkControllerID,
                  HRTIMFaultID      tkFaultID,
                  uint8_t           tkOutputIdx,
                  GPIOPortID        tkPortID,
                  GPIOPinID         tkPinID>
        concept CValidSpecificHRTIMFaultPin = requires {
            typename HRTIMFaultPinMap<tkControllerID, tkFaultID, tkOutputIdx>;
            { HRTIMFaultPinMap<tkControllerID, tkFaultID, tkOutputIdx>::skPortID } -> std::same_as<GPIOPortID>;
            { HRTIMFaultPinMap<tkControllerID, tkFaultID, tkOutputIdx>::skPinID } -> std::same_as<GPIOPinID>;
            HRTIMFaultPinMap<tkControllerID, tkFaultID, tkOutputIdx>::skPortID == tkPortID;
            HRTIMFaultPinMap<tkControllerID, tkFaultID, tkOutputIdx>::skPinID == tkPinID;
        };

        template <HRTIMControllerID tkControllerID,
                  HRTIMFaultID      tkFaultID,
                  GPIOPortID        tkPortID,
                  GPIOPinID         tkPinID,
                  uint8_t... tkIdxs>
        constexpr std::optional<uint8_t> get_hrtim_fault_pin_output_index(std::integer_sequence<uint8_t, tkIdxs...>)
        {
            // This will return the first matching index, or -1 if none match
            std::optional<uint8_t> result = std::nullopt;
            ((CValidSpecificHRTIMFaultPin<tkControllerID, tkFaultID, tkIdxs, tkPortID, tkPinID>
                  ? (result = tkIdxs, true)
                  : false) ||
             ...);
            return result;
        }

        template <HRTIMControllerID tkControllerID,
                  HRTIMFaultID      tkFaultID,
                  GPIOPortID        tkPortID,
                  GPIOPinID         tkPinID,
                  uint8_t... tkIdxs>
        constexpr bool check_any_valid_hrtim_fault_pin_map(std::integer_sequence<uint8_t, tkIdxs...>)
        {
            // This will return true if ANY of the indices match
            return get_hrtim_fault_pin_output_index<tkControllerID, tkFaultID, tkPortID, tkPinID>(
                       std::integer_sequence<uint8_t, tkIdxs...>{})
                .has_value();
        }
    }  // namespace detail

    template <HRTIMControllerID tkControllerID, HRTIMFaultID tkFaultID, GPIOPortID tkPortID, GPIOPinID tkPinID>
    concept CValidHRTIMFaultPin =
        detail::check_any_valid_hrtim_fault_pin_map<tkControllerID, tkFaultID, tkPortID, tkPinID>(
            std::make_integer_sequence<uint8_t, kHRTIMMaxOutputIndex>{});

    template <HRTIMControllerID tkControllerID, HRTIMFaultID tkFaultID, GPIOPortID tkPortID, GPIOPinID tkPinID>
        requires(CValidHRTIMFaultPin<tkControllerID, tkFaultID, tkPortID, tkPinID>)
    constexpr uint8_t kHRTIMFaultPinOutputIndex =
        detail::get_hrtim_fault_pin_output_index<tkControllerID, tkFaultID, tkPortID, tkPinID>(
            std::make_integer_sequence<uint8_t, kHRTIMMaxOutputIndex>{})
            .value();

    template <HRTIMControllerID tkControllerID, HRTIMFaultID tkFaultID, GPIOPortID tkPortID, GPIOPinID tkPinID>
        requires(CValidHRTIMFaultPin<tkControllerID, tkFaultID, tkPortID, tkPinID>)
    constexpr GPIOAlternativeFunction kHRTIMFaultPinAF =
        HRTIMFaultPinMap<tkControllerID,
                         tkFaultID,
                         kHRTIMFaultPinOutputIndex<tkControllerID, tkFaultID, tkPortID, tkPinID>>::skAF;

    // ---------------------------------------------------------------------------
    // HRTIM EEV
    // ---------------------------------------------------------------------------

    template <HRTIMControllerID tkControllerID, HRTIMEEVID tkEEVID, uint8_t tkOutputIdx = 0>
        requires(tkOutputIdx < kHRTIMMaxOutputIndex)
    struct HRTIMEEVPinMap;

#define DECLARE_HRTIM_EEV_PIN_MAP(tkControllerID, tkEEVID, tkOutputIdx, port, pin, af) \
    static_assert((tkOutputIdx) < kHRTIMMaxOutputIndex);                               \
    template <>                                                                        \
    struct HRTIMEEVPinMap<(tkControllerID), (tkEEVID), (tkOutputIdx)>                  \
    {                                                                                  \
        constexpr static GPIOPortID              skPortID = (port);                    \
        constexpr static GPIOPinID               skPinID  = (pin);                     \
        constexpr static GPIOAlternativeFunction skAF     = (af);                      \
    };

    DECLARE_HRTIM_EEV_PIN_MAP(1, HRTIMEEVID::kEEV1, 0, GPIOPortID::kC, 12, GPIOAlternativeFunction::kAF3);   // PC12
    DECLARE_HRTIM_EEV_PIN_MAP(1, HRTIMEEVID::kEEV2, 0, GPIOPortID::kC, 11, GPIOAlternativeFunction::kAF3);   // PC11
    DECLARE_HRTIM_EEV_PIN_MAP(1, HRTIMEEVID::kEEV3, 0, GPIOPortID::kB, 7, GPIOAlternativeFunction::kAF13);   // PB7
    DECLARE_HRTIM_EEV_PIN_MAP(1, HRTIMEEVID::kEEV4, 0, GPIOPortID::kB, 6, GPIOAlternativeFunction::kAF13);   // PB6
    DECLARE_HRTIM_EEV_PIN_MAP(1, HRTIMEEVID::kEEV5, 0, GPIOPortID::kB, 9, GPIOAlternativeFunction::kAF13);   // PB9
    DECLARE_HRTIM_EEV_PIN_MAP(1, HRTIMEEVID::kEEV6, 0, GPIOPortID::kB, 5, GPIOAlternativeFunction::kAF13);   // PB5
    DECLARE_HRTIM_EEV_PIN_MAP(1, HRTIMEEVID::kEEV7, 0, GPIOPortID::kB, 4, GPIOAlternativeFunction::kAF13);   // PB4
    DECLARE_HRTIM_EEV_PIN_MAP(1, HRTIMEEVID::kEEV8, 0, GPIOPortID::kB, 8, GPIOAlternativeFunction::kAF13);   // PB8
    DECLARE_HRTIM_EEV_PIN_MAP(1, HRTIMEEVID::kEEV9, 0, GPIOPortID::kB, 3, GPIOAlternativeFunction::kAF13);   // PB3
    DECLARE_HRTIM_EEV_PIN_MAP(1, HRTIMEEVID::kEEV10, 0, GPIOPortID::kC, 5, GPIOAlternativeFunction::kAF13);  // PC5
    DECLARE_HRTIM_EEV_PIN_MAP(1, HRTIMEEVID::kEEV10, 1, GPIOPortID::kC, 6, GPIOAlternativeFunction::kAF3);   // PC6
#undef DECLARE_HRTIM_EEV_PIN_MAP
    namespace detail
    {
        template <HRTIMControllerID tkControllerID,
                  HRTIMEEVID        tkEEVID,
                  uint8_t           tkOutputIdx,
                  GPIOPortID        tkPortID,
                  GPIOPinID         tkPinID>
        concept CValidSpecificHRTIMEEVPin = requires {
            typename HRTIMEEVPinMap<tkControllerID, tkEEVID, tkOutputIdx>;
            { HRTIMEEVPinMap<tkControllerID, tkEEVID, tkOutputIdx>::skPortID } -> std::same_as<GPIOPortID>;
            { HRTIMEEVPinMap<tkControllerID, tkEEVID, tkOutputIdx>::skPinID } -> std::same_as<GPIOPinID>;
            HRTIMEEVPinMap<tkControllerID, tkEEVID, tkOutputIdx>::skPortID == tkPortID;
            HRTIMEEVPinMap<tkControllerID, tkEEVID, tkOutputIdx>::skPinID == tkPinID;
        };

        template <HRTIMControllerID tkControllerID,
                  HRTIMEEVID        tkEEVID,
                  GPIOPortID        tkPortID,
                  GPIOPinID         tkPinID,
                  uint8_t... tkIdxs>
        constexpr std::optional<uint8_t> get_hrtim_eev_pin_output_index(std::integer_sequence<uint8_t, tkIdxs...>)
        {
            // This will return the first matching index, or -1 if none match
            std::optional<uint8_t> result = std::nullopt;
            ((CValidSpecificHRTIMEEVPin<tkControllerID, tkEEVID, tkIdxs, tkPortID, tkPinID> ? (result = tkIdxs, true)
                                                                                            : false) ||
             ...);
            return result;
        }

        template <HRTIMControllerID tkControllerID,
                  HRTIMEEVID        tkEEVID,
                  GPIOPortID        tkPortID,
                  GPIOPinID         tkPinID,
                  uint8_t... tkIdxs>
        constexpr bool check_any_valid_hrtim_eev_pin_map(std::integer_sequence<uint8_t, tkIdxs...>)
        {
            // This will return true if ANY of the indices match
            return get_hrtim_eev_pin_output_index<tkControllerID, tkEEVID, tkPortID, tkPinID>(
                       std::integer_sequence<uint8_t, tkIdxs...>{})
                .has_value();
        }
    }  // namespace detail

    template <HRTIMControllerID tkControllerID, HRTIMEEVID tkEEVID, GPIOPortID tkPortID, GPIOPinID tkPinID>
    concept CValidHRTIMEEVPin = detail::check_any_valid_hrtim_eev_pin_map<tkControllerID, tkEEVID, tkPortID, tkPinID>(
        std::make_integer_sequence<uint8_t, kHRTIMMaxOutputIndex>{});

    template <HRTIMControllerID tkControllerID, HRTIMEEVID tkEEVID, GPIOPortID tkPortID, GPIOPinID tkPinID>
        requires(CValidHRTIMEEVPin<tkControllerID, tkEEVID, tkPortID, tkPinID>)
    constexpr uint8_t kHRTIMEEVPinOutputIndex =
        detail::get_hrtim_eev_pin_output_index<tkControllerID, tkEEVID, tkPortID, tkPinID>(
            std::make_integer_sequence<uint8_t, kHRTIMMaxOutputIndex>{})
            .value();

    template <HRTIMControllerID tkControllerID, HRTIMEEVID tkEEVID, GPIOPortID tkPortID, GPIOPinID tkPinID>
        requires(CValidHRTIMEEVPin<tkControllerID, tkEEVID, tkPortID, tkPinID>)
    constexpr GPIOAlternativeFunction kHRTIMEEVPinAF =
        HRTIMEEVPinMap<tkControllerID, tkEEVID, kHRTIMEEVPinOutputIndex<tkControllerID, tkEEVID, tkPortID, tkPinID>>::
            skAF;

    // ---------------------------------------------------------------------------
    // HRTIM TIMER
    // ---------------------------------------------------------------------------

    enum class HRTIMTimerGPIOPinType : uint8_t
    {
        kOutput1,
        kOutput2,
    };

    template <HRTIMControllerID     tkControllerID,
              HRTIMTimerID          tkTimerID,
              HRTIMTimerGPIOPinType tkPinType,
              uint8_t               tkOutputIdx = 0>
        requires(tkOutputIdx < kHRTIMMaxOutputIndex)
    struct HRTIMTimerPinMap;

#define DECLARE_HRTIM_TIMER_PIN_MAP(tkControllerID, tkTimerID, tkPinType, tkOutputIdx, port, pin, af) \
    static_assert((tkOutputIdx) < kHRTIMMaxOutputIndex);                                              \
    template <>                                                                                       \
    struct HRTIMTimerPinMap<(tkControllerID), (tkTimerID), (tkPinType), (tkOutputIdx)>                \
    {                                                                                                 \
        constexpr static GPIOPortID              skPortID = (port);                                   \
        constexpr static GPIOPinID               skPinID  = (pin);                                    \
        constexpr static GPIOAlternativeFunction skAF     = (af);                                     \
    };

    DECLARE_HRTIM_TIMER_PIN_MAP(1,
                                HRTIMTimerID::kA,
                                HRTIMTimerGPIOPinType::kOutput1,
                                0,
                                GPIOPortID::kA,
                                8,
                                GPIOAlternativeFunction::kAF13);  // PA8

    DECLARE_HRTIM_TIMER_PIN_MAP(1,
                                HRTIMTimerID::kA,
                                HRTIMTimerGPIOPinType::kOutput2,
                                0,
                                GPIOPortID::kA,
                                9,
                                GPIOAlternativeFunction::kAF13);  // PA9

    DECLARE_HRTIM_TIMER_PIN_MAP(1,
                                HRTIMTimerID::kB,
                                HRTIMTimerGPIOPinType::kOutput1,
                                0,
                                GPIOPortID::kA,
                                10,
                                GPIOAlternativeFunction::kAF13);  // PA10

    DECLARE_HRTIM_TIMER_PIN_MAP(1,
                                HRTIMTimerID::kB,
                                HRTIMTimerGPIOPinType::kOutput2,
                                0,
                                GPIOPortID::kA,
                                11,
                                GPIOAlternativeFunction::kAF13);  // PA11

    DECLARE_HRTIM_TIMER_PIN_MAP(1,
                                HRTIMTimerID::kC,
                                HRTIMTimerGPIOPinType::kOutput1,
                                0,
                                GPIOPortID::kB,
                                12,
                                GPIOAlternativeFunction::kAF13);  // PB12

    DECLARE_HRTIM_TIMER_PIN_MAP(1,
                                HRTIMTimerID::kC,
                                HRTIMTimerGPIOPinType::kOutput2,
                                0,
                                GPIOPortID::kB,
                                13,
                                GPIOAlternativeFunction::kAF13);  // PB13

    DECLARE_HRTIM_TIMER_PIN_MAP(1,
                                HRTIMTimerID::kD,
                                HRTIMTimerGPIOPinType::kOutput1,
                                0,
                                GPIOPortID::kB,
                                14,
                                GPIOAlternativeFunction::kAF13);  // PB14

    DECLARE_HRTIM_TIMER_PIN_MAP(1,
                                HRTIMTimerID::kD,
                                HRTIMTimerGPIOPinType::kOutput2,
                                0,
                                GPIOPortID::kB,
                                15,
                                GPIOAlternativeFunction::kAF13);  // PB15

    DECLARE_HRTIM_TIMER_PIN_MAP(1,
                                HRTIMTimerID::kE,
                                HRTIMTimerGPIOPinType::kOutput1,
                                0,
                                GPIOPortID::kC,
                                8,
                                GPIOAlternativeFunction::kAF3);  // PC8

    DECLARE_HRTIM_TIMER_PIN_MAP(1,
                                HRTIMTimerID::kE,
                                HRTIMTimerGPIOPinType::kOutput2,
                                0,
                                GPIOPortID::kC,
                                9,
                                GPIOAlternativeFunction::kAF3);  // PC9

    DECLARE_HRTIM_TIMER_PIN_MAP(1,
                                HRTIMTimerID::kF,
                                HRTIMTimerGPIOPinType::kOutput1,
                                0,
                                GPIOPortID::kC,
                                6,
                                GPIOAlternativeFunction::kAF13);  // PC6

    DECLARE_HRTIM_TIMER_PIN_MAP(1,
                                HRTIMTimerID::kF,
                                HRTIMTimerGPIOPinType::kOutput2,
                                0,
                                GPIOPortID::kC,
                                7,
                                GPIOAlternativeFunction::kAF13);  // PC7

#undef DECLARE_HRTIM_PIN_MAP
    namespace detail
    {
        template <HRTIMControllerID     tkControllerID,
                  HRTIMTimerID          tkTimerID,
                  HRTIMTimerGPIOPinType tkPinType,
                  uint8_t               tkOutputIdx,
                  GPIOPortID            tkPortID,
                  GPIOPinID             tkPinID>
        concept CValidSpecificHRTIMTimerPin = requires {
            typename HRTIMTimerPinMap<tkControllerID, tkTimerID, tkPinType, tkOutputIdx>;
            HRTIMTimerPinMap<tkControllerID, tkTimerID, tkPinType, tkOutputIdx>::skPortID == tkPortID;
            HRTIMTimerPinMap<tkControllerID, tkTimerID, tkPinType, tkOutputIdx>::skPinID == tkPinID;
        };

        template <HRTIMControllerID     tkControllerID,
                  HRTIMTimerID          tkTimerID,
                  HRTIMTimerGPIOPinType tkPinType,
                  GPIOPortID            tkPortID,
                  GPIOPinID             tkPinID,
                  uint8_t... tkIdxs>
        constexpr std::optional<uint8_t> get_hrtim_timer_pin_output_index(std::integer_sequence<uint8_t, tkIdxs...>)
        {
            // This will return the first matching index, or std::nullopt if none match
            std::optional<uint8_t> result = std::nullopt;
            ((CValidSpecificHRTIMTimerPin<tkControllerID, tkTimerID, tkPinType, tkIdxs, tkPortID, tkPinID>
                  ? (result = tkIdxs, true)
                  : false) ||
             ...);
            return result;
        }

        template <HRTIMControllerID     tkControllerID,
                  HRTIMTimerID          tkTimerID,
                  HRTIMTimerGPIOPinType tkPinType,
                  GPIOPortID            tkPortID,
                  GPIOPinID             tkPinID,
                  uint8_t... tkIdxs>
        constexpr bool check_any_valid_hrtim_timer_pin_map(std::integer_sequence<uint8_t, tkIdxs...>)
        {
            // This will return true if ANY of the indices match
            return get_hrtim_timer_pin_output_index<tkControllerID, tkTimerID, tkPinType, tkPortID, tkPinID>(
                       std::integer_sequence<uint8_t, tkIdxs...>{})
                .has_value();
        }

        template <HRTIMControllerID     tkControllerID,
                  HRTIMTimerID          tkTimerID,
                  HRTIMTimerGPIOPinType tkPinType,
                  GPIOPortID            tkPortID,
                  GPIOPinID             tkPinID>
        constexpr bool kAnyValidHRTIMTimerPinMap =
            check_any_valid_hrtim_timer_pin_map<tkControllerID, tkTimerID, tkPinType, tkPortID, tkPinID>(
                std::make_integer_sequence<uint8_t, kHRTIMMaxOutputIndex>{});

    }  // namespace detail

    template <HRTIMControllerID     tkControllerID,
              HRTIMTimerID          tkTimerID,
              HRTIMTimerGPIOPinType tkPinType,
              GPIOPortID            tkPortID,
              GPIOPinID             tkPinID>
    concept CValidHRTIMTimerPin =
        detail::check_any_valid_hrtim_timer_pin_map<tkControllerID, tkTimerID, tkPinType, tkPortID, tkPinID>(
            std::make_integer_sequence<uint8_t, kHRTIMMaxOutputIndex>{});

    template <HRTIMControllerID     tkControllerID,
              HRTIMTimerID          tkTimerID,
              HRTIMTimerGPIOPinType tkPinType,
              GPIOPortID            tkPortID,
              GPIOPinID             tkPinID>
        requires(CValidHRTIMTimerPin<tkControllerID, tkTimerID, tkPinType, tkPortID, tkPinID>)
    constexpr uint8_t kHRTIMTimerPinOutputIndex =
        detail::get_hrtim_timer_pin_output_index<tkControllerID, tkTimerID, tkPinType, tkPortID, tkPinID>(
            std::make_integer_sequence<uint8_t, kHRTIMMaxOutputIndex>{})
            .value();

    template <HRTIMControllerID     tkControllerID,
              HRTIMTimerID          tkTimerID,
              HRTIMTimerGPIOPinType tkPinType,
              GPIOPortID            tkPortID,
              GPIOPinID             tkPinID>
        requires(CValidHRTIMTimerPin<tkControllerID, tkTimerID, tkPinType, tkPortID, tkPinID>)
    constexpr GPIOAlternativeFunction kHRTIMTimerPinAF =
        HRTIMTimerPinMap<tkControllerID,
                         tkTimerID,
                         tkPinType,
                         kHRTIMTimerPinOutputIndex<tkControllerID, tkTimerID, tkPinType, tkPortID, tkPinID>>::skAF;

}  // namespace valle