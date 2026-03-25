#pragma once

#include "valle/platform/core.hpp"
#include "valle/platform/hardware/rcc/lsco.hpp"

namespace valle::platform
{
    // =============================================================================
    // CONFIGURATION
    // =============================================================================
    struct LscoConfig
    {
        bool            enabled = true;
        LscoClockSource source  = LscoClockSource::kLSI;
    };

    // =============================================================================
    // DEVICE
    // =============================================================================
    class LscoDevice
    {
    public:
        struct Descriptor : public UniqueDeviceDescriptor
        {
        };

        using InterfaceT = LscoInterface;

        using DependDevices = TypeList<>;
        using InjectDevices = TypeList<>;

        [[nodiscard]] inline bool init(const LscoConfig& config)
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