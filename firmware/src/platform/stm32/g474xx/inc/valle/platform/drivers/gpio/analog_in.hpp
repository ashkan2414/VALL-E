#pragma once

#include "valle/platform/devices/gpio.hpp"

namespace valle::platform
{
    struct GPIOAnalogInConfig
    {
    };

    /**
     * @brief Configures a pin as Analog (High Impedance).
     * Used for ADC, DAC, OpAmp, Comparator, or Low Power disconnect.
     */
    template <typename TGpioPin>
    class GPIOAnalogInDriver
    {
    public:
        using InjectDevices = TypeList<TGpioPin>;

    private:
        [[no_unique_address]] DeviceRef<TGpioPin> m_pin;

    public:
        GPIOAnalogInDriver() = delete;

        GPIOAnalogInDriver(DeviceRef<TGpioPin>&& pin) : m_pin(std::move(pin))
        {
        }

        [[nodiscard]] bool init(const GPIOAnalogInConfig& config)
        {
            (void)config;
            return m_pin.get().init(GPIOPinConfig{.mode      = GPIO_MODE_ANALOG,
                                                  .pull      = GPIOPullMode::kNoPull,
                                                  .speed     = GPIOSpeedMode::kLow,
                                                  .alternate = GPIOAlternativeFunction::kAF0});
        }
    };

    namespace detail
    {
        template <typename TPin>
        struct ConditionalGPIOAnalogInDriver
        {
            using type = GPIOAnalogInDriver<TPin>;
        };

        template <>
        struct ConditionalGPIOAnalogInDriver<GPIONullPinDevice>
        {
            using type = std::monostate;
        };

    }  // namespace detail

    template <typename TPin>
    using ConditionalGPIOAnalogInDriverT = typename detail::ConditionalGPIOAnalogInDriver<TPin>::type;

}  // namespace valle::platform