#pragma once

#include <stdint.h>

namespace valle
{
    using HRTIMControllerID = uint8_t;

    template <HRTIMControllerID tkControllerID>
    constexpr bool kValidHRTIMControllerID = (tkControllerID == 1);

    enum class HRTIMTimerID : uint32_t
    {
        kA = 0,
        kB,
        kC,
        kD,
        kE,
        kF,
    };

    enum class HRTIMFaultID : uint32_t
    {
        kFault1 = 0,
        kFault2,
        kFault3,
        kFault4,
        kFault5,
        kFault6,
    };

    enum class HRTIMEEVID : uint32_t
    {
        kEEV1 = 0,
        kEEV2,
        kEEV3,
        kEEV4,
        kEEV5,
        kEEV6,
        kEEV7,
        kEEV8,
        kEEV9,
        kEEV10,
    };

}  // namespace valle
