#pragma once

#include "valle/platform/devices/gpio.hpp"

namespace valle::platform
{

    struct GPIODigitalInInterruptConfig
    {
        uint32_t                  priority;
        GPIOInputInterruptTrigger trigger;
        GPIOInputInterruptAction  action;
    };

    struct GPIODigitalInConfig
    {
        GPIOPullMode                                pull      = GPIOPullMode::kNoPull;
        std::optional<GPIODigitalInInterruptConfig> interrupt = std::nullopt;
        bool                                        inverted  = false;
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
            if (config.interrupt.has_value())
            {
                mode |=
                    static_cast<uint32_t>(config.interrupt->trigger) | static_cast<uint32_t>(config.interrupt->action);
            }

            if (!m_pin->init(GPIOPinConfig{.mode      = mode,
                                           .pull      = config.pull,
                                           .speed     = GPIOSpeedMode::kLow,
                                           .alternate = GPIOAlternativeFunction::kAF0}))
            {
                return false;
            }

            if (config.interrupt.has_value())
            {
                m_pin->enable_interrupts(GPIOPinInterruptConfig{
                    .priority = config.interrupt->priority,
                });
            }

            return true;
        }

        // --- API ---
        [[nodiscard]] inline bool read() const
        {
            const bool state = m_pin->read();
            return m_inverted ? !state : state;
        }
    };

    namespace detail
    {
        template <typename TPin>
        struct ConditionalGPIODigitalInDriver
        {
            using type = GPIODigitalInDriver<TPin>;
        };

        template <>
        struct ConditionalGPIODigitalInDriver<GPIONullPinDevice>
        {
            using type = std::monostate;
        };

    }  // namespace detail

    template <typename TPin>
    using ConditionalGPIODigitalInDriverT = typename detail::ConditionalGPIODigitalInDriver<TPin>::type;

}  // namespace valle::platform