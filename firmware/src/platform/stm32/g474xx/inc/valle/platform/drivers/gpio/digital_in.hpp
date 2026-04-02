#pragma once

#include "valle/platform/devices/gpio.hpp"

namespace valle::platform
{

    template <typename TGpioPin>
    class GpioDigitalInDriver
    {
    public:
        using InjectDevices = TypeList<TGpioPin>;

    private:
        [[no_unique_address]] DeviceRef<TGpioPin> m_pin;

    public:
        GpioDigitalInDriver() = delete;

        GpioDigitalInDriver(DeviceRef<TGpioPin>&& pin) : m_pin(std::move(pin))
        {
        }

        [[nodiscard]] bool init(const GpioDigitalInConfig& config)
        {
            uint32_t mode = Gpio_MODE_INPUT;
            if (config.interrupt.has_value())
            {
                mode |=
                    static_cast<uint32_t>(config.interrupt->trigger) | static_cast<uint32_t>(config.interrupt->action);
            }

            if (!m_pin->init(GpioPinConfig{.mode      = mode,
                                           .pull      = config.pull,
                                           .speed     = GpioPinSpeedMode::kLow,
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
            return m_pin->read_input();
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
        struct ConditionalGpioDigitalInDriver<NullDevice>
        {
            using type = std::monostate;
        };

    }  // namespace detail

    template <typename TPin>
    using ConditionalGpioDigitalInDriverT = typename detail::ConditionalGpioDigitalInDriver<TPin>::type;

}  // namespace valle::platform