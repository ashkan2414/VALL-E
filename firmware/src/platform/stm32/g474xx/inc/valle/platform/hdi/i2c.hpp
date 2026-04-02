#pragma once

#include "valle/platform/core.hpp"
#include "valle/platform/hardware/i2c.hpp"

namespace valle::platform
{
    template <I2cControllerId tkControllerId>
    struct I2cHdi : public I2cControllerInterface<tkControllerId>
    {
        struct Descriptor : public UniqueDeviceDescriptor
        {
        };

        static constexpr I2cControllerId skControllerId = tkControllerId;
    };
}  // namespace valle::platform