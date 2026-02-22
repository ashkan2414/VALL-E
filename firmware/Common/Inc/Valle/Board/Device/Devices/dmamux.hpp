#pragma once

#include "Valle/Board/Device/device_core.hpp"
#include "Valle/Board/Traits/dmamux.hpp"
#include "stm32g4xx_ll_bus.h"

namespace valle
{
    // ============================================================================
    // FORWARD DECLARATIONS
    // ============================================================================

    class DMAMuxRootDevice;

    template <DMAMuxControllerID tkControllerID>
        requires(kValidDMAMuxControllerID<tkControllerID>)
    class DMAMuxControllerDevice;

    // ============================================================================
    // DMAMux ROOT DEVICE (INTERFACE)
    // ============================================================================
    class DMAMuxRootDevice
    {
    public:
        struct Descriptor : public InterfaceDeviceDescriptor
        {
            using Children = DeviceTreeList<DMAMuxControllerDevice<1>>;
        };
    };

    // ============================================================================
    // DMAMux CONTROLLER (SHARED)
    // ============================================================================
    template <DMAMuxControllerID tkControllerID>
        requires(kValidDMAMuxControllerID<tkControllerID>)
    class DMAMuxControllerDevice
    {
    public:
        struct Descriptor : public SharedDeviceDescriptor
        {
        };

        static constexpr DMAMuxControllerID skControllerID = tkControllerID;

        using DependDevices     = TypeList<DMAMuxRootDevice>;
        using ControllerTraitsT = DMAMuxControllerTraits<tkControllerID>;

        static inline bool init()
        {
            ControllerTraitsT::enable_clock();
            return true;
        }

        static inline bool post_init()
        {
            // Additional post-initialization if needed
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

    using DMAMux1ControllerDevice = DMAMuxControllerDevice<1>;

}  // namespace valle