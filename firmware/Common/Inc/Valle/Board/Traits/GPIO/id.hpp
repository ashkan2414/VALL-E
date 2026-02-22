#pragma once

#include <stdint.h>

namespace valle
{
    enum class GPIOPortID
    {
        kA,
        kB,
        kC,
        kD,
        kE,
        kF,
        kG,
    };

    using GPIOPinID = uint8_t;

    template <GPIOPortID tkPortID, GPIOPinID tkPinID>
    constexpr bool kValidGPIOPinID = (tkPinID < 16);

}  // namespace valle