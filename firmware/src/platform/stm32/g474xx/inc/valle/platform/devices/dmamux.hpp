#pragma once

#include "stm32g4xx_ll_bus.h"
#include "valle/platform/core.hpp"
#include "valle/platform/hardware/dmamux.hpp"

namespace valle::platform
{
    // ============================================================================
    // DmaMux CONTROLLER (SHARED)
    // ============================================================================
    template <DmaMuxPeripheralId tkPeripheralId>
    class DmaMuxControllerDevice
    {
    public:
        struct Descriptor : public SharedDeviceDescriptor
        {
        };

        static constexpr DmaMuxPeripheralId skPeripheralId = tkPeripheralId;

        using ControllerTraitsT = DmaMuxControllerTraits<tkPeripheralId>;

        static inline bool init()
        {
            ControllerTraitsT::enable_clock();
            return true;
        }

        static inline void route_request(const DmaMuxChannel tkChannelId, const DmaMuxRequestId tkRequestId)
        {
            LL_DMAMUX_SetRequestId(
                ControllerTraitsT::skInstance, static_cast<uint32_t>(tkChannelId), static_cast<uint32_t>(tkRequestId));
        }
    };

    // ----------------------------------------------------------------------------
    // DEVICE ALIASES
    // ----------------------------------------------------------------------------

    using DmaMux1ControllerDevice = DmaMuxControllerDevice<DmaMuxPeripheralId::kDmaMux1>;

}  // namespace valle::platform
