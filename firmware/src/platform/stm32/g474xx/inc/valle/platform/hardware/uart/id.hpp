#pragma once

#include <cstdint>

namespace valle::platform
{
    enum UARTControllerID : uint8_t
    {
        kUSART1  = 1,
        kUSART2  = 2,
        kUSART3  = 3,
        kUART4   = 4,
        kUART5   = 5,
        kLPUART1 = 6,
    };

}  // namespace valle::platform