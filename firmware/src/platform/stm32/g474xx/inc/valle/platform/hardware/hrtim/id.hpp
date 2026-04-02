#pragma once

#include <cstdint>

namespace valle::platform
{
    enum class HrtimControllerId : uint8_t
    {
        kHrtim1 = 1,
    };

    enum class HrtimTimerId : uint8_t
    {
        kTimerA = 0,
        kTimerB,
        kTimerC,
        kTimerD,
        kTimerE,
        kTimerF,
    };

    enum class HrtimFaultId : uint8_t
    {
        kFault1 = 0,
        kFault2,
        kFault3,
        kFault4,
        kFault5,
        kFault6,
    };

    enum class HrtimExternalEventId : uint8_t
    {
        kExternalEvent1 = 1,
        kExternalEvent2,
        kExternalEvent3,
        kExternalEvent4,
        kExternalEvent5,
        kExternalEvent6,
        kExternalEvent7,
        kExternalEvent8,
        kExternalEvent9,
        kExternalEvent10,
    };

    template <HrtimControllerId tkControllerId>
    static constexpr uint8_t kHrtimControllerNumFromId = static_cast<uint8_t>(tkControllerId);
    static_assert(kHrtimControllerNumFromId<HrtimControllerId::kHrtim1> == 1,
                  "HRTIM Controller number from ID mapping is incorrect!");

    template <HrtimExternalEventId tkExternalEventId>
    static constexpr uint8_t kHrtimExternalEventNumFromId = static_cast<uint8_t>(tkExternalEventId);
    static_assert(kHrtimExternalEventNumFromId<HrtimExternalEventId::kExternalEvent1> == 1,
                  "HRTIM EEV number from EEV ID mapping is incorrect!");

}  // namespace valle::platform
