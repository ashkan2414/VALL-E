#pragma once

#include "valle/platform/devices/gpio.hpp"

namespace valle::platform
{
    /**
     * @brief Connects a pin to an internal peripheral (Timer, UART, SPI).
     * @tparam tkAfIdx The Alternate Function Index (0-15). See Datasheet.
     */
    template <typename TGpioPin>
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
            return m_pin->init_as_alternate_function(config);
        }
    };

    namespace detail
    {
        template <typename TPin>
        struct ConditionalGpioAlternateFunctionDriver
        {
            using type = GpioAlternateFunctionDriver<TPin>;
        };

        template <>
        struct ConditionalGpioAlternateFunctionDriver<NullDevice>
        {
            using type = std::monostate;
        };

    }  // namespace detail

    template <typename TPin>
    using ConditionalGpioAlternateFunctionDriverT = typename detail::ConditionalGpioAlternateFunctionDriver<TPin>::type;

}  // namespace valle::platform