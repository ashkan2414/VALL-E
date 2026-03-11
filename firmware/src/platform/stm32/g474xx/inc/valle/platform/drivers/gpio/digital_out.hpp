#pragma once

#include "valle/platform/devices/gpio.hpp"

namespace valle::platform
{
    struct GPIODigitalOutConfig
    {
        GPIOOutputMode mode  = GPIOOutputMode::kPushPull;
        GPIOSpeedMode  speed = GPIOSpeedMode::kLow;
        GPIOPullMode   pull  = GPIOPullMode::kNoPull;
    };

    template <typename TGpioPin>
    class GPIODigitalOutDriver
    {
    public:
        using InjectDevices = TypeList<TGpioPin>;

    private:
        [[no_unique_address]] DeviceRef<TGpioPin> m_pin;
        bool                                      m_inverted = false;

    public:
        GPIODigitalOutDriver() = delete;

        /**
         * @brief Construct a new Digital Out
         * @param pin Injected Pin Resource
         */
        GPIODigitalOutDriver(DeviceRef<TGpioPin>&& pin) : m_pin(std::move(pin)), m_inverted(false)
        {
        }

        [[nodiscard]] bool init(const GPIODigitalOutConfig& config)
        {
            return m_pin.get().init(GPIOPinConfig{.mode      = static_cast<uint32_t>(config.mode),
                                                  .pull      = config.pull,
                                                  .speed     = config.speed,
                                                  .alternate = GPIOAlternativeFunction::kAF0});
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
}  // namespace valle::platform