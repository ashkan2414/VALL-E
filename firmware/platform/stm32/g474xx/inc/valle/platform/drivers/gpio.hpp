#pragma once

#include "valle/platform/devices/gpio.hpp"

namespace valle
{

    // ============================================================================
    // DIGITAL OUTPUT DRIVER
    // ============================================================================
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

    // ============================================================================
    // DIGITAL INPUT DRIVER
    // ============================================================================
    struct GPIODigitalInConfig
    {
        GPIOPullMode              pull       = GPIOPullMode::kNoPull;
        GPIOInputInterruptTrigger it_trigger = GPIOInputInterruptTrigger::kNone;
        GPIOInputInterruptAction  it_action  = GPIOInputInterruptAction::kInterrupt;
        bool                      inverted   = false;
    };

    template <typename TGpioPin>
    class GPIODigitalInDriver
    {
    public:
        using InjectDevices = TypeList<TGpioPin>;

    private:
        [[no_unique_address]] DeviceRef<TGpioPin> m_pin;
        bool                                      m_inverted;

    public:
        GPIODigitalInDriver() = delete;

        GPIODigitalInDriver(DeviceRef<TGpioPin>&& pin) : m_pin(std::move(pin)), m_inverted(false)
        {
        }

        [[nodiscard]] bool init(const GPIODigitalInConfig& config)
        {
            m_inverted = config.inverted;

            uint32_t mode = GPIO_MODE_INPUT;
            if (config.it_trigger != GPIOInputInterruptTrigger::kNone)
            {
                mode = static_cast<uint32_t>(config.it_trigger) | static_cast<uint32_t>(config.it_action);
            }

            m_pin.get().init(GPIOPinConfig{.mode      = mode,
                                           .pull      = config.pull,
                                           .speed     = GPIOSpeedMode::kLow,
                                           .alternate = GPIOAlternativeFunction::kAF0});
        }

        // --- API ---
        [[nodiscard]] inline bool read() const
        {
            const bool state = m_pin.get().read();
            return m_inverted ? !state : state;
        }
    };

    // ============================================================================
    // ANALOG PIN DRIVER (High-Z)
    // ===========================================================================

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

    // ============================================================================
    // ALTERNATE FUNCTION DRIVER
    // ============================================================================

    struct GPIOAlternativeFunctionConfig
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
    class GPIOAlternativeFunctionDriver
    {
    public:
        using InjectDevices = TypeList<TGpioPin>;

    private:
        [[no_unique_address]] DeviceRef<TGpioPin> m_pin;

    public:
        GPIOAlternativeFunctionDriver() = delete;

        GPIOAlternativeFunctionDriver(DeviceRef<TGpioPin>&& pin) : m_pin(std::move(pin))
        {
        }

        [[nodiscard]] bool init(const GPIOAlternativeFunctionConfig& config)
        {
            return m_pin.get().init(GPIOPinConfig{.mode      = static_cast<uint32_t>(config.mode),
                                                  .pull      = config.pull,
                                                  .speed     = config.speed,
                                                  .alternate = tkAfIdx});
        }
    };

}  // namespace valle