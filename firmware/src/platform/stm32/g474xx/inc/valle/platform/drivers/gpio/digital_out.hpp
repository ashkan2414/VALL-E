#pragma once

#include "valle/platform/devices/gpio.hpp"

namespace valle::platform
{
    struct GpioDigitalOutConfig
    {
        GpioOutputMode mode  = GpioOutputMode::kPushPull;
        GpioSpeedMode  speed = GpioSpeedMode::kLow;
        GpioPullMode   pull  = GpioPullMode::kNoPull;
    };

    template <typename TGpioPin>
    class GpioDigitalOutDriver
    {
    public:
        using InjectDevices = TypeList<TGpioPin>;

    private:
        [[no_unique_address]] DeviceRef<TGpioPin> m_pin;
        bool                                      m_inverted = false;

    public:
        GpioDigitalOutDriver() = delete;

        /**
         * @brief Construct a new Digital Out
         * @param pin Injected Pin Resource
         */
        GpioDigitalOutDriver(DeviceRef<TGpioPin>&& pin) : m_pin(std::move(pin)), m_inverted(false)
        {
        }

        [[nodiscard]] bool init(const GpioDigitalOutConfig& config)
        {
            return m_pin.get().init(GpioPinConfig{.mode      = static_cast<uint32_t>(config.mode),
                                                  .pull      = config.pull,
                                                  .speed     = config.speed,
                                                  .alternate = GpioAlternativeFunction::kAF0});
        }

        // --- API ---
        inline void write(const bool state)
        {
            m_pin.get().write(state ^ m_inverted);
        }

        inline void toggle()
        {
            m_pin.get().toggle();
        }

        [[nodiscard]] inline bool read() const
        {
            // Read ODR, not IDR
            return (TGpioPin::skPort->ODR & TGpioPin::skPinMask) != 0;
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
        struct ConditionalGpioDigitalOutDriver<GpioNullPinDevice>
        {
            using type = std::monostate;
        };

    }  // namespace detail

    template <typename TPin>
    using ConditionalGpioDigitalOutDriverT = typename detail::ConditionalGpioDigitalOutDriver<TPin>::type;

}  // namespace valle::platform