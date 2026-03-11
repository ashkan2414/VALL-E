#pragma once

#include <optional>
#include <string_view>

#include "valle/platform/core.hpp"
#include "valle/platform/devices/power.hpp"
#include "valle/platform/devices/rcc.hpp"

namespace valle::platform
{

    struct CoreSystemConfig
    {
        PowerConfig power_config{};
        RCCConfig   rcc_config{};

        [[nodiscard]] constexpr std::optional<std::string_view> validate() const
        {
            return rcc_config.validate(power_config.regulation.get_voltage_range_mode());
        }
    };

    class CoreSystemDriver
    {
    public:
        using InjectDevices = TypeList<PowerDevice<>, RCCDevice<>>;

    private:
        [[no_unique_address]] DeviceRef<PowerDevice<>> m_power_device;
        [[no_unique_address]] DeviceRef<RCCDevice<>>   m_rcc_device;

    public:
        CoreSystemDriver() = delete;

        CoreSystemDriver(DeviceRef<PowerDevice<>>&& power_device, DeviceRef<RCCDevice<>>&& rcc_device)
            : m_power_device(std::move(power_device)), m_rcc_device(std::move(rcc_device))
        {
        }

        [[nodiscard]] bool init(const CoreSystemConfig& config)
        {
            auto validation_result = config.validate();
            expect(!validation_result.has_value(), validation_result.value_or("Unknown validation error"));

            // Initialize power first since RCC initialization may depend on power state
            if (!m_power_device->init(config.power_config))
            {
                return false;
            }

            if (!m_rcc_device->init(config.rcc_config))
            {
                return false;
            }

            CycleClock::init();
            SystemClock::init();

            return true;
        }
    };
}  // namespace valle::platform