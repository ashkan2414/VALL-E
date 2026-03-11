#pragma once

#include "valle/platform/devices/gpio.hpp"

namespace valle::platform
{

    struct GPIODigitalInConfig
    {
        GPIOPullMode              pull       = GPIOPullMode::kNoPull;
        GPIOInputInterruptTrigger it_trigger = GPIOInputInterruptTrigger::kNone;
        GPIOInputInterruptAction  it_action  = GPIOInputInterruptAction::kInterrupt;
        bool                      inverted   = false;
    };

    template <typename TGpioPin>
    class GPIODigitalInDriver
    {
    public:
        using InjectDevices = TypeList<TGpioPin>;

    private:
        [[no_unique_address]] DeviceRef<TGpioPin> m_pin;
        bool                                      m_inverted;

    public:
        GPIODigitalInDriver() = delete;

        GPIODigitalInDriver(DeviceRef<TGpioPin>&& pin) : m_pin(std::move(pin)), m_inverted(false)
        {
        }

        [[nodiscard]] bool init(const GPIODigitalInConfig& config)
        {
            m_inverted = config.inverted;

            uint32_t mode = GPIO_MODE_INPUT;
            if (config.it_trigger != GPIOInputInterruptTrigger::kNone)
            {
                mode = static_cast<uint32_t>(config.it_trigger) | static_cast<uint32_t>(config.it_action);
            }

            m_pin.get().init(GPIOPinConfig{.mode      = mode,
                                           .pull      = config.pull,
                                           .speed     = GPIOSpeedMode::kLow,
                                           .alternate = GPIOAlternativeFunction::kAF0});
        }

        // --- API ---
        [[nodiscard]] inline bool read() const
        {
            const bool state = m_pin.get().read();
            return m_inverted ? !state : state;
        }
    };
}  // namespace valle::platform