#pragma once

#include "valle/platform/devices/gpio.hpp"

namespace valle::platform
{

    /**
     * @brief Configures a pin as Analog (High Impedance).
     * Used for ADC, DAC, OpAmp, Comparator, or Low Power disconnect.
     */
    template <typename TGpioPin>
    class GpioAnalogInDriver
    {
    public:
        using InjectDevices = TypeList<TGpioPin>;

    private:
        [[no_unique_address]] DeviceRef<TGpioPin> m_pin;

    public:
        GpioAnalogInDriver() = delete;

        GpioAnalogInDriver(DeviceRef<TGpioPin>&& pin) : m_pin(std::move(pin))
        {
        }

        [[nodiscard]] bool init(const GpioAnalogInConfig& config)
        {
            (void)config;
            return m_pin->init_as_analog_in(config);
        }
    };

    namespace detail
    {
        template <typename TPin>
        struct ConditionalGpioAnalogInDriver
        {
            using type = GpioAnalogInDriver<TPin>;
        };

        template <>
        struct ConditionalGpioAnalogInDriver<NullDevice>
        {
            using type = std::monostate;
        };

    }  // namespace detail

    template <typename TPin>
    using ConditionalGpioAnalogInDriverT = typename detail::ConditionalGpioAnalogInDriver<TPin>::type;

}  // namespace valle::platform