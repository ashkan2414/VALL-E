#pragma once

#include "stm32g4xx_ll_bus.h"
#include "valle/platform/core.hpp"
#include "valle/platform/hardware/dmamux.hpp"

namespace valle::platform
{
    // ============================================================================
    // DMAMux CONTROLLER (SHARED)
    // ============================================================================
    template <DMAMuxControllerID tkControllerID>
    class DMAMuxControllerDevice
    {
    public:
        struct Descriptor : public SharedDeviceDescriptor
        {
        };

        static constexpr DMAMuxControllerID skControllerID = tkControllerID;

        using ControllerTraitsT = DMAMuxControllerTraits<tkControllerID>;

        static inline bool init()
        {
            ControllerTraitsT::enable_clock();
            return true;
        }

        static inline void route_request(const DMAMuxChannel tkChannelID, const DMAMuxRequestID tkRequestID)
        {
            LL_DMAMUX_SetRequestID(
                ControllerTraitsT::skInstance, static_cast<uint32_t>(tkChannelID), static_cast<uint32_t>(tkRequestID));
        }
    };

    // ----------------------------------------------------------------------------
    // DEVICE ALIASES
    // ----------------------------------------------------------------------------

    using DMAMux1ControllerDevice = DMAMuxControllerDevice<DMAMuxControllerID::kDMAMux1>;

}  // namespace valle::platform
