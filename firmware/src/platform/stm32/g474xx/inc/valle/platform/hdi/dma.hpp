#pragma once

#include "valle/platform/core.hpp"
#include "valle/platform/hardware/dma.hpp"

namespace valle::platform
{
    template <DmaControllerId tkControllerId>
    struct DmaHdi : public DmaControllerInterface<tkControllerId>
    {
        struct Descriptor : public UniqueDeviceDescriptor
        {
        };

        static constexpr DmaControllerId skControllerId = tkControllerId;
    };

    template <DmaControllerId tkControllerId, DmaChannelId tkChannelId>
    struct DmaChannelHdi : public DmaChannelInterface<tkControllerId, tkChannelId>
    {
        struct Descriptor : public UniqueDeviceDescriptor
        {
        };

        static constexpr DmaControllerId skControllerId = tkControllerId;
        static constexpr DmaChannelId    skChannelId    = tkChannelId;
    };

}  // namespace valle::platform