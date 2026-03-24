#pragma once

#include <cstdint>

namespace valle::platform
{
    enum class HRTIMControllerID : uint8_t
    {
        kHRTIM1 = 1,
    };

    enum class HRTIMTimerID : uint8_t
    {
        kA = 0,
        kB,
        kC,
        kD,
        kE,
        kF,
    };

    enum class HRTIMFaultID : uint8_t
    {
        kFault1 = 0,
        kFault2,
        kFault3,
        kFault4,
        kFault5,
        kFault6,
    };

    enum class HRTIMEEVID : uint8_t
    {
        kEEV1 = 1,
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

    template <HRTIMControllerID tkControllerID>
    static constexpr uint8_t kHRTIMControllerNumFromID = static_cast<uint8_t>(tkControllerID);
    static_assert(kHRTIMControllerNumFromID<HRTIMControllerID::kHRTIM1> == 1,
                  "HRTIM Controller number from ID mapping is incorrect!");

    template <HRTIMEEVID tkEEVID>
    static constexpr uint8_t kHRTIMEEVNumFromID = static_cast<uint8_t>(tkEEVID);
    static_assert(kHRTIMEEVNumFromID<HRTIMEEVID::kEEV1> == 1, "HRTIM EEV number from EEV ID mapping is incorrect!");

}  // namespace valle::platform
