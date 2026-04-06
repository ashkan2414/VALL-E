#pragma once

#include <cstdint>

#include "stm32g4xx_ll_hrtim.h"
#include "valle/platform/core/id.hpp"

namespace valle::platform
{
    struct HrtimControllerId : public SequentialEnumValue<HrtimControllerId, uint8_t, 1, 1>
    {
        static const HrtimControllerId kHrtim1;
    };

    inline constexpr HrtimControllerId HrtimControllerId::kHrtim1 = HrtimControllerId::from_number<1>();

    struct HrtimTimerId : public LLDriverEnumValue<HrtimTimerId>
    {
        static const HrtimTimerId kTimerA;
        static const HrtimTimerId kTimerB;
        static const HrtimTimerId kTimerC;
        static const HrtimTimerId kTimerD;
        static const HrtimTimerId kTimerE;
        static const HrtimTimerId kTimerF;
    };

    inline constexpr HrtimTimerId HrtimTimerId::kTimerA = HrtimTimerId::from_ll<LL_HRTIM_TIMER_A>();
    inline constexpr HrtimTimerId HrtimTimerId::kTimerB = HrtimTimerId::from_ll<LL_HRTIM_TIMER_B>();
    inline constexpr HrtimTimerId HrtimTimerId::kTimerC = HrtimTimerId::from_ll<LL_HRTIM_TIMER_C>();
    inline constexpr HrtimTimerId HrtimTimerId::kTimerD = HrtimTimerId::from_ll<LL_HRTIM_TIMER_D>();
    inline constexpr HrtimTimerId HrtimTimerId::kTimerE = HrtimTimerId::from_ll<LL_HRTIM_TIMER_E>();
    inline constexpr HrtimTimerId HrtimTimerId::kTimerF = HrtimTimerId::from_ll<LL_HRTIM_TIMER_F>();

    inline constexpr auto kHrtimFaultIdNumberMapping =
        LLDriverEnumValueSequentialNumberMapping<6>{.start_number = 1,
                                                    .ll_id_table  = {LL_HRTIM_FAULT_1,
                                                                     LL_HRTIM_FAULT_2,
                                                                     LL_HRTIM_FAULT_3,
                                                                     LL_HRTIM_FAULT_4,
                                                                     LL_HRTIM_FAULT_5,
                                                                     LL_HRTIM_FAULT_6}};

    struct HrtimFaultId : public NumberedLLDriverEnumValue<HrtimFaultId, kHrtimFaultIdNumberMapping>
    {
        static const HrtimFaultId kFault1;
        static const HrtimFaultId kFault2;
        static const HrtimFaultId kFault3;
        static const HrtimFaultId kFault4;
        static const HrtimFaultId kFault5;
        static const HrtimFaultId kFault6;
    };

    inline constexpr HrtimFaultId HrtimFaultId::kFault1 = HrtimFaultId::from_number<1>();
    inline constexpr HrtimFaultId HrtimFaultId::kFault2 = HrtimFaultId::from_number<2>();
    inline constexpr HrtimFaultId HrtimFaultId::kFault3 = HrtimFaultId::from_number<3>();
    inline constexpr HrtimFaultId HrtimFaultId::kFault4 = HrtimFaultId::from_number<4>();
    inline constexpr HrtimFaultId HrtimFaultId::kFault5 = HrtimFaultId::from_number<5>();
    inline constexpr HrtimFaultId HrtimFaultId::kFault6 = HrtimFaultId::from_number<6>();

    inline constexpr auto kHrtimExternalEventIdNumberMapping =
        LLDriverEnumValueSequentialNumberMapping<10>{.start_number = 1,
                                                     .ll_id_table  = {LL_HRTIM_EVENT_1,
                                                                      LL_HRTIM_EVENT_2,
                                                                      LL_HRTIM_EVENT_3,
                                                                      LL_HRTIM_EVENT_4,
                                                                      LL_HRTIM_EVENT_5,
                                                                      LL_HRTIM_EVENT_6,
                                                                      LL_HRTIM_EVENT_7,
                                                                      LL_HRTIM_EVENT_8,
                                                                      LL_HRTIM_EVENT_9,
                                                                      LL_HRTIM_EVENT_10}};

    struct HrtimExternalEventId
        : public NumberedLLDriverEnumValue<HrtimExternalEventId, kHrtimExternalEventIdNumberMapping>
    {
        static const HrtimExternalEventId kExternalEvent1;
        static const HrtimExternalEventId kExternalEvent2;
        static const HrtimExternalEventId kExternalEvent3;
        static const HrtimExternalEventId kExternalEvent4;
        static const HrtimExternalEventId kExternalEvent5;
        static const HrtimExternalEventId kExternalEvent6;
        static const HrtimExternalEventId kExternalEvent7;
        static const HrtimExternalEventId kExternalEvent8;
        static const HrtimExternalEventId kExternalEvent9;
        static const HrtimExternalEventId kExternalEvent10;
    };

    inline constexpr HrtimExternalEventId HrtimExternalEventId::kExternalEvent1 =
        HrtimExternalEventId::from_number<1>();
    inline constexpr HrtimExternalEventId HrtimExternalEventId::kExternalEvent2 =
        HrtimExternalEventId::from_number<2>();
    inline constexpr HrtimExternalEventId HrtimExternalEventId::kExternalEvent3 =
        HrtimExternalEventId::from_number<3>();
    inline constexpr HrtimExternalEventId HrtimExternalEventId::kExternalEvent4 =
        HrtimExternalEventId::from_number<4>();
    inline constexpr HrtimExternalEventId HrtimExternalEventId::kExternalEvent5 =
        HrtimExternalEventId::from_number<5>();
    inline constexpr HrtimExternalEventId HrtimExternalEventId::kExternalEvent6 =
        HrtimExternalEventId::from_number<6>();
    inline constexpr HrtimExternalEventId HrtimExternalEventId::kExternalEvent7 =
        HrtimExternalEventId::from_number<7>();
    inline constexpr HrtimExternalEventId HrtimExternalEventId::kExternalEvent8 =
        HrtimExternalEventId::from_number<8>();
    inline constexpr HrtimExternalEventId HrtimExternalEventId::kExternalEvent9 =
        HrtimExternalEventId::from_number<9>();
    inline constexpr HrtimExternalEventId HrtimExternalEventId::kExternalEvent10 =
        HrtimExternalEventId::from_number<10>();

}  // namespace valle::platform
