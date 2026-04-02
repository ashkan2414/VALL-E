#pragma once

#include "valle/platform/core.hpp"
#include "valle/platform/hardware/gpio.hpp"

namespace valle::platform
{
    template <GpioPortId tkPortId>
    struct GpioPortHdi : public GpioPortInterface<tkPortId>
    {
        struct Descriptor : public UniqueDeviceDescriptor
        {
        };

        static constexpr GpioPortId skPortId = tkPortId;
    };

    template <GpioPortId tkPortId, GpioPinId tkPinId>
    struct GpioPinHdi : public GpioPinInterface<tkPortId, tkPinId>
    {
        struct Descriptor : public UniqueDeviceDescriptor
        {
        };

        static constexpr GpioPortId skPortId = tkPortId;
        static constexpr GpioPinId  skPinId  = tkPinId;
    };

}  // namespace valle::platform