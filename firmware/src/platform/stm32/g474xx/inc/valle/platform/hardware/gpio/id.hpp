#pragma once

#include <cstdint>

namespace valle::platform
{
    enum class GPIOPortID
    {
        kPortA,
        kPortB,
        kPortC,
        kPortD,
        kPortE,
        kPortF,
        kPortG,
    };

    enum class GPIOPinID : uint8_t
    {
        kPin0 = 0,
        kPin1,
        kPin2,
        kPin3,
        kPin4,
        kPin5,
        kPin6,
        kPin7,
        kPin8,
        kPin9,
        kPin10,
        kPin11,
        kPin12,
        kPin13,
        kPin14,
        kPin15,
    };

}  // namespace valle::platform