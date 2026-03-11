#pragma once

#include "valle/core/device/device.hpp"
#include "valle/platform/core.hpp"
#include "valle/platform/hardware/rcc/lsco.hpp"

namespace valle
{
    // =============================================================================
    // FORWARD DECLARATIONS
    // =============================================================================
    class LSCORootDevice;
    class LSCODevice;

    // =============================================================================
    // ROOT LSCO DEVICE
    // =============================================================================
    class LSCORootDevice
    {
    public:
        struct Descriptor : public InterfaceDeviceDescriptor
        {
            using Children = DeviceTreeList<LSCODevice>;
        };
    };

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

        using DependDevices = TypeList<LSCORootDevice>;
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

}  // namespace valle