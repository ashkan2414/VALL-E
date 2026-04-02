#pragma once

#include "valle/platform/core.hpp"
#include "valle/platform/hardware/adc_common.hpp"

namespace valle::platform
{
    template <AdcCommonControllerId tkControllerId>
    struct AdcCommonControllerHdi : public AdcCommonControllerInterface<tkControllerId>
    {
        struct Descriptor : public UniqueDeviceDescriptor
        {
        };

        static constexpr AdcCommonControllerId skCommonId = tkControllerId;
    };
}  // namespace valle::platform