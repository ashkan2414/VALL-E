#pragma once

#include <stdint.h>

#include "stm32g4xx_ll_hrtim.h"

namespace valle
{
    using HRTIMControllerID = uint8_t;

    template <HRTIMControllerID tkControllerID>
    constexpr bool kValidHRTIMControllerID = (tkControllerID == 1);

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

}  // namespace valle