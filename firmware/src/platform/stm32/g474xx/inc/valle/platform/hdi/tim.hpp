#pragma once

#include "valle/platform/core.hpp"
#include "valle/platform/hardware/tim.hpp"

namespace valle::platform
{
    template <TimControllerId tkControllerId>
    struct TimHdi : public TimControllerInterface<tkControllerId>
    {
        struct Descriptor : public UniqueDeviceDescriptor
        {
        };

        static constexpr TimControllerId skControllerId = tkControllerId;
    };

    template <TimControllerId tkControllerId, TimTimerId tkTimerId>
    struct TimTimerHdi : public TimTimerInterface<tkControllerId, tkTimerId>
    {
        struct Descriptor : public UniqueDeviceDescriptor
        {
        };

        static constexpr TimControllerId skControllerId = tkControllerId;
        static constexpr TimTimerId      skTimerId      = tkTimerId;
    };

}  // namespace valle::platform