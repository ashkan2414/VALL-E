#pragma once

#include "valle/platform/devices/gpio.hpp"

namespace valle::platform
{
    struct GPIOAlternateFunctionConfig
    {
        GPIOAlternateFunctionMode mode  = GPIOAlternateFunctionMode::kPushPull;
        GPIOSpeedMode             speed = GPIOSpeedMode::kLow;
        GPIOPullMode              pull  = GPIOPullMode::kNoPull;
    };

    /**
     * @brief Connects a pin to an internal peripheral (Timer, UART, SPI).
     * @tparam tkAfIdx The Alternate Function Index (0-15). See Datasheet.
     */
    template <typename TGpioPin, GPIOAlternativeFunction tkAfIdx>
    class GPIOAlternateFunctionDriver
    {
    public:
        using InjectDevices = TypeList<TGpioPin>;

    private:
        [[no_unique_address]] DeviceRef<TGpioPin> m_pin;

    public:
        GPIOAlternateFunctionDriver() = delete;

        GPIOAlternateFunctionDriver(DeviceRef<TGpioPin>&& pin) : m_pin(std::move(pin))
        {
        }

        [[nodiscard]] bool init(const GPIOAlternateFunctionConfig& config)
        {
            return m_pin.get().init(GPIOPinConfig{.mode      = static_cast<uint32_t>(config.mode),
                                                  .pull      = config.pull,
                                                  .speed     = config.speed,
                                                  .alternate = tkAfIdx});
        }
    };

    namespace detail
    {
        template <typename TPin, GPIOAlternativeFunction tkAfIdx>
        struct ConditionalGPIOAlternateFunctionDriver
        {
            using type = GPIOAlternateFunctionDriver<TPin, tkAfIdx>;
        };

        template <GPIOAlternativeFunction tkAfIdx>
        struct ConditionalGPIOAlternateFunctionDriver<GPIONullPinDevice, tkAfIdx>
        {
            using type = std::monostate;
        };

    }  // namespace detail

    template <typename TPin, GPIOAlternativeFunction tkAfIdx>
    using ConditionalGPIOAlternateFunctionDriverT =
        typename detail::ConditionalGPIOAlternateFunctionDriver<TPin, tkAfIdx>::type;

}  // namespace valle::platform