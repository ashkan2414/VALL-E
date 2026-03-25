#pragma once

#include "valle/platform/devices/gpio.hpp"

namespace valle::platform
{

    struct GpioDigitalInInterruptConfig
    {
        uint32_t                  priority;
        GpioInputInterruptTrigger trigger;
        GpioInputInterruptAction  action;
    };

    struct GpioDigitalInConfig
    {
        GpioPullMode                                pull      = GpioPullMode::kNoPull;
        std::optional<GpioDigitalInInterruptConfig> interrupt = std::nullopt;
        bool                                        inverted  = false;
    };

    template <typename TGpioPin>
    class GpioDigitalInDriver
    {
    public:
        using InjectDevices = TypeList<TGpioPin>;

    private:
        [[no_unique_address]] DeviceRef<TGpioPin> m_pin;
        bool                                      m_inverted;

    public:
        GpioDigitalInDriver() = delete;

        GpioDigitalInDriver(DeviceRef<TGpioPin>&& pin) : m_pin(std::move(pin)), m_inverted(false)
        {
        }

        [[nodiscard]] bool init(const GpioDigitalInConfig& config)
        {
            m_inverted = config.inverted;

            uint32_t mode = Gpio_MODE_INPUT;
            if (config.interrupt.has_value())
            {
                mode |=
                    static_cast<uint32_t>(config.interrupt->trigger) | static_cast<uint32_t>(config.interrupt->action);
            }

            if (!m_pin->init(GpioPinConfig{.mode      = mode,
                                           .pull      = config.pull,
                                           .speed     = GpioSpeedMode::kLow,
                                           .alternate = GpioAlternativeFunction::kAF0}))
            {
                return false;
            }

            if (config.interrupt.has_value())
            {
                m_pin->enable_interrupts(GpioPinInterruptConfig{
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
        struct ConditionalGpioDigitalInDriver
        {
            using type = GpioDigitalInDriver<TPin>;
        };

        template <>
        struct ConditionalGpioDigitalInDriver<GpioNullPinDevice>
        {
            using type = std::monostate;
        };

    }  // namespace detail

    template <typename TPin>
    using ConditionalGpioDigitalInDriverT = typename detail::ConditionalGpioDigitalInDriver<TPin>::type;

}  // namespace valle::platform