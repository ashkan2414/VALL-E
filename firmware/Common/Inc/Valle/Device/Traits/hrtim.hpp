#pragma once

#include "Valle/Device/Traits/gpio.hpp"
#include "stm32g4xx_ll_bus.h"
#include "stm32g4xx_ll_hrtim.h"

namespace valle
{

    // ============================================================================
    // DEVICE IDs & CHECKS
    // ============================================================================

    using HRTIMControllerID = uint8_t;

    template <HRTIMControllerID tkControllerID>
    constexpr bool kValidHRTIMID = (tkControllerID == 1);

    enum class HRTIMTimerID : uint8_t
    {
        kA = 0,
        kB = 1,
        kC = 2,
        kD = 3,
        kE = 4,
        kF = 5,
    };

    enum class HRTIMFaultID : uint32_t
    {
        kFault1 = LL_HRTIM_FAULT_1,
        kFault2 = LL_HRTIM_FAULT_2,
        kFault3 = LL_HRTIM_FAULT_3,
        kFault4 = LL_HRTIM_FAULT_4,
        kFault5 = LL_HRTIM_FAULT_5,
        kFault6 = LL_HRTIM_FAULT_6,
    };

    enum class HRTIMEEVID : uint32_t
    {
        kEEV1  = LL_HRTIM_EVENT_1,
        kEEV2  = LL_HRTIM_EVENT_2,
        kEEV3  = LL_HRTIM_EVENT_3,
        kEEV4  = LL_HRTIM_EVENT_4,
        kEEV5  = LL_HRTIM_EVENT_5,
        kEEV6  = LL_HRTIM_EVENT_6,
        kEEV7  = LL_HRTIM_EVENT_7,
        kEEV8  = LL_HRTIM_EVENT_8,
        kEEV9  = LL_HRTIM_EVENT_9,
        kEEV10 = LL_HRTIM_EVENT_10,
    };

    // ============================================================================
    // HARDWARE TRAITS
    // ============================================================================
    template <HRTIMControllerID tkControllerID>
        requires(kValidHRTIMID<tkControllerID>)
    struct HRTIMControllerTraits;

    template <>
    struct HRTIMControllerTraits<1>
    {
        static inline HRTIM_TypeDef* const skInstance = HRTIM1;
        static constexpr uint32_t          skClock    = LL_APB2_GRP1_PERIPH_HRTIM1;

        static inline void enable_clock()
        {
            LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_HRTIM1);
        }
    };

    // ---------------------------------------------------------------------------
    // HRTIM FAULT
    // ---------------------------------------------------------------------------
    template <HRTIMControllerID tkControllerID, HRTIMFaultID tkFaultID>
    struct HRTIMFaultTraits
    {
    };

    // ---------------------------------------------------------------------------
    // HRTIM EEV
    // ---------------------------------------------------------------------------
    template <HRTIMControllerID tkControllerID, HRTIMEEVID tkEEVID>
    struct HRTIMEEVTraits
    {
    };

    // ----------------------------------------------------------------------------
    // HRTIM TIMER
    // ----------------------------------------------------------------------------
    template <HRTIMControllerID tkControllerID, HRTIMTimerID tkTimerID>
    struct HRTIMTimerTraits;

    // HRTIM1 Timer A (tkHRTIMTimerID 0)
    template <>
    struct HRTIMTimerTraits<1, HRTIMTimerID::kA>
    {
        static constexpr uint32_t  skTimerIdx = HRTIM_TIMERINDEX_TIMER_A;
        static constexpr uint32_t  skOutput1  = HRTIM_OUTPUT_TA1;
        static constexpr uint32_t  skOutput2  = HRTIM_OUTPUT_TA2;
        static constexpr IRQn_Type skIRQn     = HRTIM1_TIMA_IRQn;
    };

    // HRTIM1 Timer B
    template <>
    struct HRTIMTimerTraits<1, HRTIMTimerID::kB>
    {
        static constexpr uint32_t  skTimerIdx = HRTIM_TIMERINDEX_TIMER_B;
        static constexpr uint32_t  skOutput1  = HRTIM_OUTPUT_TB1;
        static constexpr uint32_t  skOutput2  = HRTIM_OUTPUT_TB2;
        static constexpr IRQn_Type skIRQn     = HRTIM1_TIMB_IRQn;
    };

    // HRTIM1 Timer C
    template <>
    struct HRTIMTimerTraits<1, HRTIMTimerID::kC>
    {
        static constexpr uint32_t  skTimerIdx = HRTIM_TIMERINDEX_TIMER_C;
        static constexpr uint32_t  skOutput1  = HRTIM_OUTPUT_TC1;
        static constexpr uint32_t  skOutput2  = HRTIM_OUTPUT_TC2;
        static constexpr IRQn_Type skIRQn     = HRTIM1_TIMC_IRQn;
    };

    // HRTIM1 Timer D
    template <>
    struct HRTIMTimerTraits<1, HRTIMTimerID::kD>
    {
        static constexpr uint32_t  skTimerIdx = HRTIM_TIMERINDEX_TIMER_D;
        static constexpr uint32_t  skOutput1  = HRTIM_OUTPUT_TD1;
        static constexpr uint32_t  skOutput2  = HRTIM_OUTPUT_TD2;
        static constexpr IRQn_Type skIRQn     = HRTIM1_TIMD_IRQn;
    };

    // HRTIM1 Timer E
    template <>
    struct HRTIMTimerTraits<1, HRTIMTimerID::kE>
    {
        static constexpr uint32_t  skTimerIdx = HRTIM_TIMERINDEX_TIMER_E;
        static constexpr uint32_t  skOutput1  = HRTIM_OUTPUT_TE1;
        static constexpr uint32_t  skOutput2  = HRTIM_OUTPUT_TE2;
        static constexpr IRQn_Type skIRQn     = HRTIM1_TIME_IRQn;
    };

    // HRTIM1 Timer F
    template <>
    struct HRTIMTimerTraits<1, HRTIMTimerID::kF>
    {
        static constexpr uint32_t  skTimerIdx = HRTIM_TIMERINDEX_TIMER_F;
        static constexpr uint32_t  skOutput1  = HRTIM_OUTPUT_TF1;
        static constexpr uint32_t  skOutput2  = HRTIM_OUTPUT_TF2;
        static constexpr IRQn_Type skIRQn     = HRTIM1_TIMF_IRQn;
    };

    // ============================================================================
    // PIN MAPPINGS
    // ============================================================================

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

    // ============================================================================
    // ENUMERATIONS
    // ============================================================================
    enum class HRTIMFaultSource : uint32_t
    {
        kDigitalInput = LL_HRTIM_FLT_SRC_DIGITALINPUT,
        kInternal     = LL_HRTIM_FLT_SRC_INTERNAL,
        kEEVInput     = LL_HRTIM_FLT_SRC_EEVINPUT,
    };

    enum class HRTIMFaultPolarity : uint32_t
    {
        kLow  = LL_HRTIM_FLT_POLARITY_LOW,
        kHigh = LL_HRTIM_FLT_POLARITY_HIGH,
    };

    enum class HRTIMFaultFilter : uint32_t
    {
        kNone     = LL_HRTIM_FLT_FILTER_NONE,
        kFilter1  = LL_HRTIM_FLT_FILTER_1,
        kFilter2  = LL_HRTIM_FLT_FILTER_2,
        kFilter3  = LL_HRTIM_FLT_FILTER_3,
        kFilter4  = LL_HRTIM_FLT_FILTER_4,
        kFilter5  = LL_HRTIM_FLT_FILTER_5,
        kFilter6  = LL_HRTIM_FLT_FILTER_6,
        kFilter7  = LL_HRTIM_FLT_FILTER_7,
        kFilter8  = LL_HRTIM_FLT_FILTER_8,
        kFilter9  = LL_HRTIM_FLT_FILTER_9,
        kFilter10 = LL_HRTIM_FLT_FILTER_10,
        kFilter11 = LL_HRTIM_FLT_FILTER_11,
        kFilter12 = LL_HRTIM_FLT_FILTER_12,
        kFilter13 = LL_HRTIM_FLT_FILTER_13,
        kFilter14 = LL_HRTIM_FLT_FILTER_14,
        kFilter15 = LL_HRTIM_FLT_FILTER_15
    };

    enum class HRTIMFaultPrescaler : uint32_t
    {
        kDiv1 = LL_HRTIM_FLT_PRESCALER_DIV1,
        kDiv2 = LL_HRTIM_FLT_PRESCALER_DIV2,
        kDiv4 = LL_HRTIM_FLT_PRESCALER_DIV4,
        kDiv8 = LL_HRTIM_FLT_PRESCALER_DIV8,
    };

    enum class HRTIMFaultBlankingSource : uint32_t
    {
        kRSTAligned = LL_HRTIM_FLT_BLANKING_RSTALIGNED,
        kMoving     = LL_HRTIM_FLT_BLANKING_MOVING,
    };

    enum class HRTIMFaultResetMode : uint32_t
    {
        kUnconditional = LL_HRTIM_FLT_COUNTERRST_UNCONDITIONAL,
        kConditional   = LL_HRTIM_FLT_COUNTERRST_CONDITIONAL
    };

    enum class HRTIMEEVSource : uint32_t
    {
        kSource1 = 0x00000000U,
        kSource2 = HRTIM_EECR1_EE1SRC_0,
        kSource3 = HRTIM_EECR1_EE1SRC_1,
        kSource4 = HRTIM_EECR1_EE1SRC_1 | HRTIM_EECR1_EE1SRC_0,
    };

    enum class HRTIMEEVPolarity : uint32_t
    {
        kHigh = LL_HRTIM_EE_POLARITY_HIGH,
        kLow  = LL_HRTIM_EE_POLARITY_LOW,
    };

    enum class HRTIMEEVSensitivity : uint32_t
    {
        kLevel       = LL_HRTIM_EE_SENSITIVITY_LEVEL,
        kRisingEdge  = LL_HRTIM_EE_SENSITIVITY_RISINGEDGE,
        kFallingEdge = LL_HRTIM_EE_SENSITIVITY_FALLINGEDGE,
        kBothEdges   = LL_HRTIM_EE_SENSITIVITY_BOTHEDGES
    };

    enum class HRTIMEEVFastMode : uint32_t
    {
        kDisable = LL_HRTIM_EE_FASTMODE_DISABLE,
        kEnable  = LL_HRTIM_EE_FASTMODE_ENABLE,
    };

    enum class HRTIMEEVFilter : uint32_t
    {
        kNone     = LL_HRTIM_EE_FILTER_NONE,
        kFilter1  = LL_HRTIM_EE_FILTER_1,
        kFilter2  = LL_HRTIM_EE_FILTER_2,
        kFilter3  = LL_HRTIM_EE_FILTER_3,
        kFilter4  = LL_HRTIM_EE_FILTER_4,
        kFilter5  = LL_HRTIM_EE_FILTER_5,
        kFilter6  = LL_HRTIM_EE_FILTER_6,
        kFilter7  = LL_HRTIM_EE_FILTER_7,
        kFilter8  = LL_HRTIM_EE_FILTER_8,
        kFilter9  = LL_HRTIM_EE_FILTER_9,
        kFilter10 = LL_HRTIM_EE_FILTER_10,
        kFilter11 = LL_HRTIM_EE_FILTER_11,
        kFilter12 = LL_HRTIM_EE_FILTER_12,
        kFilter13 = LL_HRTIM_EE_FILTER_13,
        kFilter14 = LL_HRTIM_EE_FILTER_14,
        kFilter15 = LL_HRTIM_EE_FILTER_15
    };

    enum class HRTIMEEVPrescaler : uint32_t
    {
        kDiv1 = LL_HRTIM_EE_PRESCALER_DIV1,
        kDiv2 = LL_HRTIM_EE_PRESCALER_DIV2,
        kDiv4 = LL_HRTIM_EE_PRESCALER_DIV4,
        kDiv8 = LL_HRTIM_EE_PRESCALER_DIV8,
    };

    enum class HRTIMTimerPrescaler : uint32_t
    {
        kMul32 = LL_HRTIM_PRESCALERRATIO_MUL32,
        kMul16 = LL_HRTIM_PRESCALERRATIO_MUL16,
        kMul8  = LL_HRTIM_PRESCALERRATIO_MUL8,
        kMul4  = LL_HRTIM_PRESCALERRATIO_MUL4,
        kMul2  = LL_HRTIM_PRESCALERRATIO_MUL2,
        kDiv1  = LL_HRTIM_PRESCALERRATIO_DIV1,
        kDiv2  = LL_HRTIM_PRESCALERRATIO_DIV2,
        kDiv4  = LL_HRTIM_PRESCALERRATIO_DIV4,
    };

    enum class HRTIMTimerCounterMode : uint32_t
    {
        kContinuous = LL_HRTIM_MODE_CONTINUOUS,
        kSingleShot = LL_HRTIM_MODE_SINGLESHOT,
        kBurst      = LL_HRTIM_MODE_RETRIGGERABLE,
    };

    enum class HRTIMTimerCountingMode : uint32_t
    {
        kUp     = LL_HRTIM_COUNTING_MODE_UP,
        kUpDown = LL_HRTIM_COUNTING_MODE_UP_DOWN,
    };

    enum class HRTIMOutputSetSource : uint32_t
    {
        kNone                       = LL_HRTIM_OUTPUTSET_NONE,
        kResync                     = LL_HRTIM_OUTPUTSET_RESYNC,
        kTimerPeriod                = LL_HRTIM_OUTPUTSET_TIMPER,
        kTimerCompare1              = LL_HRTIM_OUTPUTSET_TIMCMP1,
        kTimerCompare2              = LL_HRTIM_OUTPUTSET_TIMCMP2,
        kTimerCompare3              = LL_HRTIM_OUTPUTSET_TIMCMP3,
        kTimerCompare4              = LL_HRTIM_OUTPUTSET_TIMCMP4,
        kMasterPer                  = LL_HRTIM_OUTPUTSET_MASTERPER,
        kMasterCompare1             = LL_HRTIM_OUTPUTSET_MASTERCMP1,
        kMasterCompare2             = LL_HRTIM_OUTPUTSET_MASTERCMP2,
        kMasterCompare3             = LL_HRTIM_OUTPUTSET_MASTERCMP3,
        kMasterCompare4             = LL_HRTIM_OUTPUTSET_MASTERCMP4,
        kTimerAEvent1TimerBCompare1 = LL_HRTIM_OUTPUTSET_TIMAEV1_TIMBCMP1,
        kTimerAEvent2TimerBCompare2 = LL_HRTIM_OUTPUTSET_TIMAEV2_TIMBCMP2,
        kTimerAEvent3TimerCCompare2 = LL_HRTIM_OUTPUTSET_TIMAEV3_TIMCCMP2,
        kTimerAEvent4TimerCCompare3 = LL_HRTIM_OUTPUTSET_TIMAEV4_TIMCCMP3,
        kTimerAEvent5TimerDCompare1 = LL_HRTIM_OUTPUTSET_TIMAEV5_TIMDCMP1,
        kTimerAEvent6TimerDCompare2 = LL_HRTIM_OUTPUTSET_TIMAEV6_TIMDCMP2,
        kTimerAEvent7TimerECompare3 = LL_HRTIM_OUTPUTSET_TIMAEV7_TIMECMP3,
        kTimerAEvent8TimerECompare4 = LL_HRTIM_OUTPUTSET_TIMAEV8_TIMECMP4,
        kTimerAEvent9TimerFCompare4 = LL_HRTIM_OUTPUTSET_TIMAEV9_TIMFCMP4,
        kTimerBEvent1TimerACompare1 = LL_HRTIM_OUTPUTSET_TIMBEV1_TIMACMP1,
        kTimerBEvent2TimerACompare2 = LL_HRTIM_OUTPUTSET_TIMBEV2_TIMACMP2,
        kTimerBEvent3TimerCCompare3 = LL_HRTIM_OUTPUTSET_TIMBEV3_TIMCCMP3,
        kTimerBEvent4TimerCCompare4 = LL_HRTIM_OUTPUTSET_TIMBEV4_TIMCCMP4,
        kTimerBEvent5TimerDCompare3 = LL_HRTIM_OUTPUTSET_TIMBEV5_TIMDCMP3,
        kTimerBEvent6TimerDCompare4 = LL_HRTIM_OUTPUTSET_TIMBEV6_TIMDCMP4,
        kTimerBEvent7TimerECompare1 = LL_HRTIM_OUTPUTSET_TIMBEV7_TIMECMP1,
        kTimerBEvent8TimerECompare2 = LL_HRTIM_OUTPUTSET_TIMBEV8_TIMECMP2,
        kTimerBEvent9TimerFCompare3 = LL_HRTIM_OUTPUTSET_TIMBEV9_TIMFCMP3,
        kTimerCEvent1TimerACompare2 = LL_HRTIM_OUTPUTSET_TIMCEV1_TIMACMP2,
        kTimerCEvent2TimerACompare3 = LL_HRTIM_OUTPUTSET_TIMCEV2_TIMACMP3,
        kTimerCEvent3TimerBCompare2 = LL_HRTIM_OUTPUTSET_TIMCEV3_TIMBCMP2,
        kTimerCEvent4TimerBCompare3 = LL_HRTIM_OUTPUTSET_TIMCEV4_TIMBCMP3,
        kTimerCEvent5TimerDCompare2 = LL_HRTIM_OUTPUTSET_TIMCEV5_TIMDCMP2,
        kTimerCEvent6TimerDCompare4 = LL_HRTIM_OUTPUTSET_TIMCEV6_TIMDCMP4,
        kTimerCEvent7TimerECompare3 = LL_HRTIM_OUTPUTSET_TIMCEV7_TIMECMP3,
        kTimerCEvent8TimerECompare4 = LL_HRTIM_OUTPUTSET_TIMCEV8_TIMECMP4,
        kTimerCEvent9TimerFCompare2 = LL_HRTIM_OUTPUTSET_TIMCEV9_TIMFCMP2,
        kTimerDEvent1TimerACompare1 = LL_HRTIM_OUTPUTSET_TIMDEV1_TIMACMP1,
        kTimerDEvent2TimerACompare4 = LL_HRTIM_OUTPUTSET_TIMDEV2_TIMACMP4,
        kTimerDEvent3TimerBCompare2 = LL_HRTIM_OUTPUTSET_TIMDEV3_TIMBCMP2,
        kTimerDEvent4TimerBCompare4 = LL_HRTIM_OUTPUTSET_TIMDEV4_TIMBCMP4,
        kTimerDEvent5TimerCCompare4 = LL_HRTIM_OUTPUTSET_TIMDEV5_TIMCCMP4,
        kTimerDEvent6TimerECompare1 = LL_HRTIM_OUTPUTSET_TIMDEV6_TIMECMP1,
        kTimerDEvent7TimerECompare4 = LL_HRTIM_OUTPUTSET_TIMDEV7_TIMECMP4,
        kTimerDEvent8TimerFCompare1 = LL_HRTIM_OUTPUTSET_TIMDEV8_TIMFCMP1,
        kTimerDEvent9TimerFCompare3 = LL_HRTIM_OUTPUTSET_TIMDEV9_TIMFCMP3,
        kTimerEEvent1TimerACompare4 = LL_HRTIM_OUTPUTSET_TIMEEV1_TIMACMP4,
        kTimerEEvent2TimerBCompare3 = LL_HRTIM_OUTPUTSET_TIMEEV2_TIMBCMP3,
        kTimerEEvent3TimerBCompare4 = LL_HRTIM_OUTPUTSET_TIMEEV3_TIMBCMP4,
        kTimerEEvent4TimerCCompare1 = LL_HRTIM_OUTPUTSET_TIMEEV4_TIMCCMP1,
        kTimerEEvent5TimerCCompare2 = LL_HRTIM_OUTPUTSET_TIMEEV5_TIMCCMP2,
        kTimerEEvent6TimerDCompare1 = LL_HRTIM_OUTPUTSET_TIMEEV6_TIMDCMP1,
        kTimerEEvent7TimerDCompare2 = LL_HRTIM_OUTPUTSET_TIMEEV7_TIMDCMP2,
        kTimerEEvent8TimerFCompare3 = LL_HRTIM_OUTPUTSET_TIMEEV8_TIMFCMP3,
        kTimerEEvent9TimerFCompare4 = LL_HRTIM_OUTPUTSET_TIMEEV9_TIMFCMP4,
        kTimerFEvent1TimerACompare3 = LL_HRTIM_OUTPUTSET_TIMFEV1_TIMACMP3,
        kTimerFEvent2TimerBCompare1 = LL_HRTIM_OUTPUTSET_TIMFEV2_TIMBCMP1,
        kTimerFEvent3TimerBCompare4 = LL_HRTIM_OUTPUTSET_TIMFEV3_TIMBCMP4,
        kTimerFEvent4TimerCCompare1 = LL_HRTIM_OUTPUTSET_TIMFEV4_TIMCCMP1,
        kTimerFEvent5TimerCCompare4 = LL_HRTIM_OUTPUTSET_TIMFEV5_TIMCCMP4,
        kTimerFEvent6TimerDCompare3 = LL_HRTIM_OUTPUTSET_TIMFEV6_TIMDCMP3,
        kTimerFEvent7TimerDCompare4 = LL_HRTIM_OUTPUTSET_TIMFEV7_TIMDCMP4,
        kTimerFEvent8TimerECompare2 = LL_HRTIM_OUTPUTSET_TIMFEV8_TIMECMP2,
        kTimerFEvent9TimerECompare3 = LL_HRTIM_OUTPUTSET_TIMFEV9_TIMECMP3,
        kExternalEvent1             = LL_HRTIM_OUTPUTSET_EEV_1,
        kExternalEvent2             = LL_HRTIM_OUTPUTSET_EEV_2,
        kExternalEvent3             = LL_HRTIM_OUTPUTSET_EEV_3,
        kExternalEvent4             = LL_HRTIM_OUTPUTSET_EEV_4,
        kExternalEvent5             = LL_HRTIM_OUTPUTSET_EEV_5,
        kExternalEvent6             = LL_HRTIM_OUTPUTSET_EEV_6,
        kExternalEvent7             = LL_HRTIM_OUTPUTSET_EEV_7,
        kExternalEvent8             = LL_HRTIM_OUTPUTSET_EEV_8,
        kExternalEvent9             = LL_HRTIM_OUTPUTSET_EEV_9,
        kExternalEvent10            = LL_HRTIM_OUTPUTSET_EEV_10,
        kUpdate                     = LL_HRTIM_OUTPUTSET_UPDATE,
    };

    enum class HRTIMOutputResetSource : uint32_t
    {
        kNone                       = LL_HRTIM_OUTPUTRESET_NONE,
        kResync                     = LL_HRTIM_OUTPUTRESET_RESYNC,
        kTimerPeriod                = LL_HRTIM_OUTPUTRESET_TIMPER,
        kTimerCompare1              = LL_HRTIM_OUTPUTRESET_TIMCMP1,
        kTimerCompare2              = LL_HRTIM_OUTPUTRESET_TIMCMP2,
        kTimerCompare3              = LL_HRTIM_OUTPUTRESET_TIMCMP3,
        kTimerCompare4              = LL_HRTIM_OUTPUTRESET_TIMCMP4,
        kMasterPeriod               = LL_HRTIM_OUTPUTRESET_MASTERPER,
        kMasterCompare1             = LL_HRTIM_OUTPUTRESET_MASTERCMP1,
        kMasterCompare2             = LL_HRTIM_OUTPUTRESET_MASTERCMP2,
        kMasterCompare3             = LL_HRTIM_OUTPUTRESET_MASTERCMP3,
        kMasterCompare4             = LL_HRTIM_OUTPUTRESET_MASTERCMP4,
        kTimerAEvent1TimerBCompare1 = LL_HRTIM_OUTPUTRESET_TIMAEV1_TIMBCMP1,
        kTimerAEvent2TimerBCompare2 = LL_HRTIM_OUTPUTRESET_TIMAEV2_TIMBCMP2,
        kTimerAEvent3TimerCCompare2 = LL_HRTIM_OUTPUTRESET_TIMAEV3_TIMCCMP2,
        kTimerAEvent4TimerCCompare3 = LL_HRTIM_OUTPUTRESET_TIMAEV4_TIMCCMP3,
        kTimerAEvent5TimerDCompare1 = LL_HRTIM_OUTPUTRESET_TIMAEV5_TIMDCMP1,
        kTimerAEvent6TimerDCompare2 = LL_HRTIM_OUTPUTRESET_TIMAEV6_TIMDCMP2,
        kTimerAEvent7TimerECompare3 = LL_HRTIM_OUTPUTRESET_TIMAEV7_TIMECMP3,
        kTimerAEvent8TimerECompare4 = LL_HRTIM_OUTPUTRESET_TIMAEV8_TIMECMP4,
        kTimerAEvent9TimerFCompare4 = LL_HRTIM_OUTPUTRESET_TIMAEV9_TIMFCMP4,
        kTimerBEvent1TimerACompare1 = LL_HRTIM_OUTPUTRESET_TIMBEV1_TIMACMP1,
        kTimerBEvent2TimerACompare2 = LL_HRTIM_OUTPUTRESET_TIMBEV2_TIMACMP2,
        kTimerBEvent3TimerCCompare3 = LL_HRTIM_OUTPUTRESET_TIMBEV3_TIMCCMP3,
        kTimerBEvent4TimerCCompare4 = LL_HRTIM_OUTPUTRESET_TIMBEV4_TIMCCMP4,
        kTimerBEvent5TimerDCompare3 = LL_HRTIM_OUTPUTRESET_TIMBEV5_TIMDCMP3,
        kTimerBEvent6TimerDCompare4 = LL_HRTIM_OUTPUTRESET_TIMBEV6_TIMDCMP4,
        kTimerBEvent7TimerECompare1 = LL_HRTIM_OUTPUTRESET_TIMBEV7_TIMECMP1,
        kTimerBEvent8TimerECompare2 = LL_HRTIM_OUTPUTRESET_TIMBEV8_TIMECMP2,
        kTimerBEvent9TimerFCompare3 = LL_HRTIM_OUTPUTRESET_TIMBEV9_TIMFCMP3,
        kTimerCEvent1TimerACompare2 = LL_HRTIM_OUTPUTRESET_TIMCEV1_TIMACMP2,
        kTimerCEvent2TimerACompare3 = LL_HRTIM_OUTPUTRESET_TIMCEV2_TIMACMP3,
        kTimerCEvent3TimerBCompare2 = LL_HRTIM_OUTPUTRESET_TIMCEV3_TIMBCMP2,
        kTimerCEvent4TimerBCompare3 = LL_HRTIM_OUTPUTRESET_TIMCEV4_TIMBCMP3,
        kTimerCEvent5TimerDCompare2 = LL_HRTIM_OUTPUTRESET_TIMCEV5_TIMDCMP2,
        kTimerCEvent6TimerDCompare4 = LL_HRTIM_OUTPUTRESET_TIMCEV6_TIMDCMP4,
        kTimerCEvent7TimerECompare3 = LL_HRTIM_OUTPUTRESET_TIMCEV7_TIMECMP3,
        kTimerCEvent8TimerECompare4 = LL_HRTIM_OUTPUTRESET_TIMCEV8_TIMECMP4,
        kTimerCEvent9TimerFCompare2 = LL_HRTIM_OUTPUTRESET_TIMCEV9_TIMFCMP2,
        kTimerDEvent1TimerACompare1 = LL_HRTIM_OUTPUTRESET_TIMDEV1_TIMACMP1,
        kTimerDEvent2TimerACompare4 = LL_HRTIM_OUTPUTRESET_TIMDEV2_TIMACMP4,
        kTimerDEvent3TimerBCompare2 = LL_HRTIM_OUTPUTRESET_TIMDEV3_TIMBCMP2,
        kTimerDEvent4TimerBCompare4 = LL_HRTIM_OUTPUTRESET_TIMDEV4_TIMBCMP4,
        kTimerDEvent5TimerCCompare4 = LL_HRTIM_OUTPUTRESET_TIMDEV5_TIMCCMP4,
        kTimerDEvent6TimerECompare1 = LL_HRTIM_OUTPUTRESET_TIMDEV6_TIMECMP1,
        kTimerDEvent7TimerECompare4 = LL_HRTIM_OUTPUTRESET_TIMDEV7_TIMECMP4,
        kTimerDEvent8TimerFCompare1 = LL_HRTIM_OUTPUTRESET_TIMDEV8_TIMFCMP1,
        kTimerDEvent9TimerFCompare3 = LL_HRTIM_OUTPUTRESET_TIMDEV9_TIMFCMP3,
        kTimerEEvent1TimerACompare4 = LL_HRTIM_OUTPUTRESET_TIMEEV1_TIMACMP4,
        kTimerEEvent2TimerBCompare3 = LL_HRTIM_OUTPUTRESET_TIMEEV2_TIMBCMP3,
        kTimerEEvent3TimerBCompare4 = LL_HRTIM_OUTPUTRESET_TIMEEV3_TIMBCMP4,
        kTimerEEvent4TimerCCompare1 = LL_HRTIM_OUTPUTRESET_TIMEEV4_TIMCCMP1,
        kTimerEEvent5TimerCCompare2 = LL_HRTIM_OUTPUTRESET_TIMEEV5_TIMCCMP2,
        kTimerEEvent6TimerDCompare1 = LL_HRTIM_OUTPUTRESET_TIMEEV6_TIMDCMP1,
        kTimerEEvent7TimerDCompare2 = LL_HRTIM_OUTPUTRESET_TIMEEV7_TIMDCMP2,
        kTimerEEvent8TimerFCompare3 = LL_HRTIM_OUTPUTRESET_TIMEEV8_TIMFCMP3,
        kTimerEEvent9TimerFCompare4 = LL_HRTIM_OUTPUTRESET_TIMEEV9_TIMFCMP4,
        kTimerFEvent1TimerACompare3 = LL_HRTIM_OUTPUTRESET_TIMFEV1_TIMACMP3,
        kTimerFEvent2TimerBCompare1 = LL_HRTIM_OUTPUTRESET_TIMFEV2_TIMBCMP1,
        kTimerFEvent3TimerBCompare4 = LL_HRTIM_OUTPUTRESET_TIMFEV3_TIMBCMP4,
        kTimerFEvent4TimerCCompare1 = LL_HRTIM_OUTPUTRESET_TIMFEV4_TIMCCMP1,
        kTimerFEvent5TimerCCompare4 = LL_HRTIM_OUTPUTRESET_TIMFEV5_TIMCCMP4,
        kTimerFEvent6TimerDCompare3 = LL_HRTIM_OUTPUTRESET_TIMFEV6_TIMDCMP3,
        kTimerFEvent7TimerDCompare4 = LL_HRTIM_OUTPUTRESET_TIMFEV7_TIMDCMP4,
        kTimerFEvent8TimerECompare2 = LL_HRTIM_OUTPUTRESET_TIMFEV8_TIMECMP2,
        kTimerFEvent9TimerECompare3 = LL_HRTIM_OUTPUTRESET_TIMFEV9_TIMECMP3,
        kExternalEvent1             = LL_HRTIM_OUTPUTRESET_EEV_1,
        kExternalEvent2             = LL_HRTIM_OUTPUTRESET_EEV_2,
        kExternalEvent3             = LL_HRTIM_OUTPUTRESET_EEV_3,
        kExternalEvent4             = LL_HRTIM_OUTPUTRESET_EEV_4,
        kExternalEvent5             = LL_HRTIM_OUTPUTRESET_EEV_5,
        kExternalEvent6             = LL_HRTIM_OUTPUTRESET_EEV_6,
        kExternalEvent7             = LL_HRTIM_OUTPUTRESET_EEV_7,
        kExternalEvent8             = LL_HRTIM_OUTPUTRESET_EEV_8,
        kExternalEvent9             = LL_HRTIM_OUTPUTRESET_EEV_9,
        kExternalEvent10            = LL_HRTIM_OUTPUTRESET_EEV_10,
        kUpdate                     = LL_HRTIM_OUTPUTRESET_UPDATE,
    };

    enum class HRTIMOutputPolarity : uint32_t
    {
        kPositive = LL_HRTIM_OUT_POSITIVE_POLARITY,  /// Output active high
        kNegative = LL_HRTIM_OUT_NEGATIVE_POLARITY   /// Output active low
    };

    enum class HRTIMOutputIdleMode : uint32_t
    {
        kNoIdle = LL_HRTIM_OUT_NO_IDLE,          /// Never idle
        kBurst  = LL_HRTIM_OUT_IDLE_WHEN_BURST,  /// Idle during burst mode
    };

    enum class HRTIMOutputIdleLevel : uint32_t
    {
        kInactive = LL_HRTIM_OUT_IDLELEVEL_INACTIVE,  /// Inactive when idle
        kActive   = LL_HRTIM_OUT_IDLELEVEL_ACTIVE     /// Active when idle
    };

    enum class HRTIMOutputFaultState : uint32_t
    {
        kInactive = LL_HRTIM_OUT_FAULTSTATE_INACTIVE,   /// Output inactive on fault
        kActive   = LL_HRTIM_OUT_FAULTSTATE_ACTIVE,     /// Output active on fault
        kNoAction = LL_HRTIM_OUT_FAULTSTATE_NO_ACTION,  /// No action on fault
        kHighZ    = LL_HRTIM_OUT_FAULTSTATE_HIGHZ       /// High impedance on fault
    };

    enum class HRTIMOutputChopperMode : uint32_t
    {
        kDisabled = LL_HRTIM_OUT_CHOPPERMODE_DISABLED,
        kEnabled  = LL_HRTIM_OUT_CHOPPERMODE_ENABLED,
    };

    enum class HRTIMOutputBurstModeEntryMode : uint32_t
    {
        kRegular = LL_HRTIM_OUT_BM_ENTRYMODE_REGULAR,
        kDelayed = LL_HRTIM_OUT_BM_ENTRYMODE_DELAYED,
    };

}  // namespace valle