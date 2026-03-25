#pragma once

#include "valle/platform/devices/gpio.hpp"

namespace valle::platform
{
    struct GpioAlternateFunctionConfig
    {
        GpioAlternateFunctionMode mode  = GpioAlternateFunctionMode::kPushPull;
        GpioSpeedMode             speed = GpioSpeedMode::kLow;
        GpioPullMode              pull  = GpioPullMode::kNoPull;
    };

    /**
     * @brief Connects a pin to an internal peripheral (Timer, UART, SPI).
     * @tparam tkAfIdx The Alternate Function Index (0-15). See Datasheet.
     */
    template <typename TGpioPin, GpioAlternativeFunction tkAfIdx>
    class GpioAlternateFunctionDriver
    {
    public:
        using InjectDevices = TypeList<TGpioPin>;

    private:
        [[no_unique_address]] DeviceRef<TGpioPin> m_pin;

    public:
        GpioAlternateFunctionDriver() = delete;

        GpioAlternateFunctionDriver(DeviceRef<TGpioPin>&& pin) : m_pin(std::move(pin))
        {
        }

        [[nodiscard]] bool init(const GpioAlternateFunctionConfig& config)
        {
            return m_pin.get().init(GpioPinConfig{.mode      = static_cast<uint32_t>(config.mode),
                                                  .pull      = config.pull,
                                                  .speed     = config.speed,
                                                  .alternate = tkAfIdx});
        }
    };

    namespace detail
    {
        template <typename TPin, GpioAlternativeFunction tkAfIdx>
        struct ConditionalGpioAlternateFunctionDriver
        {
            using type = GpioAlternateFunctionDriver<TPin, tkAfIdx>;
        };

        template <GpioAlternativeFunction tkAfIdx>
        struct ConditionalGpioAlternateFunctionDriver<GpioNullPinDevice, tkAfIdx>
        {
            using type = std::monostate;
        };

    }  // namespace detail

    template <typename TPin, GpioAlternativeFunction tkAfIdx>
    using ConditionalGpioAlternateFunctionDriverT =
        typename detail::ConditionalGpioAlternateFunctionDriver<TPin, tkAfIdx>::type;

}  // namespace valle::platform