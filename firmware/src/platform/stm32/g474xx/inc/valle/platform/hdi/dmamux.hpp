#pragma once

#include "valle/platform/core.hpp"
#include "valle/platform/hardware/dmamux.hpp"

namespace valle::platform
{
    template <DmaMuxControllerId tkControllerId>
    struct DmaMuxHdi : public DmaMuxControllerInterface<tkControllerId>
    {
        struct Descriptor : public UniqueDeviceDescriptor
        {
        };

        static constexpr DmaMuxControllerId skControllerId = tkControllerId;
    };
}  // namespace valle::platform