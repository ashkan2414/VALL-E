#pragma once

#include "valle/platform/core.hpp"
#include "valle/platform/hardware/rcc/lsco.hpp"

namespace valle::platform
{
    // =============================================================================
    // CONFIGURATION
    // =============================================================================
    struct LSCOConfig
    {
        bool            enabled = true;
        LSCOClockSource source  = LSCOClockSource::kLSI;
    };

    // =============================================================================
    // DEVICE
    // =============================================================================
    class LSCODevice
    {
    public:
        struct Descriptor : public UniqueDeviceDescriptor
        {
        };

        using InterfaceT = LSCOInterface;

        using DependDevices = TypeList<>;
        using InjectDevices = TypeList<>;

        [[nodiscard]] inline bool init(const LSCOConfig& config)
        {
            InterfaceT::set_source(config.source);

            if (config.enabled)
            {
                InterfaceT::enable();
            }
            else
            {
                InterfaceT::disable();
            }

            return true;
        }
    };

}  // namespace valle::platform