#pragma once

#include "valle/platform/devices/gpio.hpp"

namespace valle::platform
{
    template <typename TGpioPin>
    class GpioDigitalOutDriver
    {
    public:
        using InjectDevices = TypeList<TGpioPin>;

    private:
        [[no_unique_address]] DeviceRef<TGpioPin> m_pin;

    public:
        GpioDigitalOutDriver() = delete;

        /**
         * @brief Construct a new Digital Out
         * @param pin Injected Pin Resource
         */
        GpioDigitalOutDriver(DeviceRef<TGpioPin>&& pin) : m_pin(std::move(pin))
        {
        }

        [[nodiscard]] bool init(const GpioDigitalOutConfig& config)
        {
            return m_pin->init_as_digital_out(config);
        }

        // --- API ---
        void write(const bool state)
        {
            m_pin->write(state);
        }

        void set()
        {
            m_pin->set();
        }

        void reset()
        {
            m_pin->reset();
        }

        void toggle()
        {
            m_pin->toggle();
        }

        [[nodiscard]] bool read() const
        {
            return m_pin->read_output();
        }
    };

    namespace detail
    {
        template <typename TPin>
        struct ConditionalGpioDigitalOutDriver
        {
            using type = GpioDigitalOutDriver<TPin>;
        };

        template <>
        struct ConditionalGpioDigitalOutDriver<NullDevice>
        {
            using type = std::monostate;
        };

    }  // namespace detail

    template <typename TPin>
    using ConditionalGpioDigitalOutDriverT = typename detail::ConditionalGpioDigitalOutDriver<TPin>::type;

}  // namespace valle::platform