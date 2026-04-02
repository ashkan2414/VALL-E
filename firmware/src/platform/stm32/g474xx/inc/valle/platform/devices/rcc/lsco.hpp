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
    template <typename T = void>
    class Lsco
    {
    public:
        struct Descriptor : public UniqueDeviceDescriptor
        {
        };

        using HdiT          = LscoHdi<T>;
        using InjectDevices = TypeList<HdiT>;

    private:
        [[no_unique_address]] DeviceRef<HdiT> m_hw;

    public:
        explicit Lsco(DeviceRef<HdiT>&& hardware_key) : m_hw(std::move(hardware_key))
        {
        }

        [[nodiscard]] inline bool init(const LscoConfig& config)
        {
            m_hw->set_source(config.source);

            if (config.enabled)
            {
                m_hw->enable();
            }
            else
            {
                m_hw->disable();
            }

            return true;
        }
    };

}  // namespace valle::platform