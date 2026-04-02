#pragma once

#include "valle/platform/core.hpp"
#include "valle/platform/hardware/uart.hpp"

namespace valle::platform
{
    template <UartControllerId tkControllerId>
    struct UartHdi : public UartControllerInterface<tkControllerId>
    {
        struct Descriptor : public UniqueDeviceDescriptor
        {
        };

        static constexpr UartControllerId skControllerId = tkControllerId;
    };
}  // namespace valle::platform