#pragma once

#include "valle/platform/hardware/exti.hpp"

namespace valle::platform
{
    template <ExtiLineId tkLineId>
    struct ExtiLineHdi : public ExtiLineInterface<tkLineId>
    {
        struct Descriptor : public UniqueDeviceDescriptor
        {
        };

        static constexpr ExtiLineId skLineId = tkLineId;
    };

}  // namespace valle::platform