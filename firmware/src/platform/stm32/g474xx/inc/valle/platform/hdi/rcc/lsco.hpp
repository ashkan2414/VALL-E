#pragma once

#include "valle/platform/hardware/rcc/lsco.hpp"

namespace valle::platform
{
    template <typename T = void>
    struct LscoHdi : public LscoInterface
    {
        struct Descriptor : public UniqueDeviceDescriptor
        {
        };
    };

}  // namespace valle::platform