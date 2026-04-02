#pragma once

#include <algorithm>
#include <cstdint>

#include "valle/platform/core.hpp"
#include "valle/platform/hardware/hrtim.hpp"

namespace valle::platform
{
    template <HrtimControllerId tkControllerId>
    struct HrtimHdi : public HrtimControllerInterface<tkControllerId>
    {
        struct Descriptor : public UniqueDeviceDescriptor
        {
        };

        static constexpr HrtimControllerId skControllerId = tkControllerId;
    };

    template <HrtimControllerId tkControllerId, HrtimFaultId tkFaultId>
    struct HrtimFaultHdi : public HrtimFaultInterface<tkControllerId, tkFaultId>
    {
        struct Descriptor : public UniqueDeviceDescriptor
        {
        };

        static constexpr HrtimControllerId skControllerId = tkControllerId;
        static constexpr HrtimFaultId      skFaultId      = tkFaultId;
    };

    template <HrtimControllerId tkControllerId, HrtimExternalEventId tkExternalEventId>
    struct HrtimExternalEventHdi : public HrtimExternalEventInterface<tkControllerId, tkExternalEventId>
    {
        struct Descriptor : public UniqueDeviceDescriptor
        {
        };

        static constexpr HrtimControllerId    skControllerId    = tkControllerId;
        static constexpr HrtimExternalEventId skExternalEventId = tkExternalEventId;
    };

    template <HrtimControllerId tkControllerId, HrtimTimerId tkTimerId>
    struct HrtimTimerHdi : public HrtimTimerInterface<tkControllerId, tkTimerId>
    {
        struct Descriptor : public UniqueDeviceDescriptor
        {
        };

        static constexpr HrtimControllerId skControllerId = tkControllerId;
        static constexpr HrtimTimerId      skTimerId      = tkTimerId;
    };

}  // namespace valle::platform
