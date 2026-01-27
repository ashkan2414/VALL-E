#pragma once

#include "Valle/Device/Devices/gpio.hpp"

namespace valle
{

    // ============================================================================
    // DIGITAL OUTPUT DRIVER
    // ============================================================================
    struct GPIODigitalOutConfig
    {
        GPIOOutputMode mode;
        GPIOSpeedMode  speed;
        GPIOPullMode   pull;

        [[nodiscard]] static GPIODigitalOutConfig default_config()
        {
            return GPIODigitalOutConfig{
                .mode = GPIOOutputMode::kPushPull, .speed = GPIOSpeedMode::kLow, .pull = GPIOPullMode::kNoPull};
        }
    };

    template <typename TGpioPin>
    class GPIODigitalOutDriver
    {
    public:
        using InjectDevices = TypeList<TGpioPin>;

    private:
        DeviceRef<TGpioPin> m_pin;
        bool                m_inverted;

    public:
        /**
     * @brief Construct a new Digital Out
     * @param pin Injected Pin Resource
     */
        GPIODigitalOutDriver(DeviceRef<TGpioPin>&& pin) : m_pin(std::move(pin)), m_inverted(false)
        {
        }

        void init(const GPIODigitalOutConfig& config = GPIODigitalOutConfig::default_config())
        {
            m_pin.get().init(GPIOPinConfig{.mode      = static_cast<uint32_t>(config.mode),
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
        GPIOPullMode              pull;
        GPIOInputInterruptTrigger it_trigger;
        GPIOInputInterruptAction  it_action;
        bool                      inverted;

        [[nodiscard]] static inline constexpr GPIODigitalInConfig default_config()
        {
            return GPIODigitalInConfig{.pull       = GPIOPullMode::kNoPull,
                                       .it_trigger = GPIOInputInterruptTrigger::kNone,
                                       .it_action  = GPIOInputInterruptAction::kInterrupt,
                                       .inverted   = false};
        }
    };

    template <typename TGpioPin>
    class GPIODigitalInDriver
    {
    public:
        using InjectDevices = TypeList<TGpioPin>;

    private:
        DeviceRef<TGpioPin> m_pin;
        bool                m_inverted;

    public:
        GPIODigitalInDriver(DeviceRef<TGpioPin>&& pin) : m_pin(std::move(pin)), m_inverted(false)
        {
        }

        void init(const GPIODigitalInConfig& config = GPIODigitalInConfig::default_config())
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
        [[nodiscard]] static inline constexpr GPIOAnalogInConfig default_config()
        {
            return GPIOAnalogInConfig{};
        }
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
        DeviceRef<TGpioPin> m_pin;

    public:
        GPIOAnalogInDriver(DeviceRef<TGpioPin>&& pin) : m_pin(std::move(pin))
        {
        }

        void init(const GPIOAnalogInConfig& config = GPIOAnalogInConfig::default_config())
        {
            m_pin.get().init(GPIOPinConfig{.mode      = GPIO_MODE_ANALOG,
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
        GPIOAlternateFunctionMode mode;
        GPIOSpeedMode             speed;
        GPIOPullMode              pull;

        [[nodiscard]] static inline constexpr GPIOAlternativeFunctionConfig default_config()
        {
            return GPIOAlternativeFunctionConfig{.mode  = GPIOAlternateFunctionMode::kPushPull,
                                                 .speed = GPIOSpeedMode::kLow,
                                                 .pull  = GPIOPullMode::kNoPull};
        };
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
        DeviceRef<TGpioPin> m_pin;

    public:
        GPIOAlternativeFunctionDriver(DeviceRef<TGpioPin>&& pin) : m_pin(std::move(pin))
        {
        }

        void init(const GPIOAlternativeFunctionConfig& config = GPIOAlternativeFunctionConfig::default_config())
        {
            m_pin.get().init(GPIOPinConfig{.mode      = static_cast<uint32_t>(config.mode),
                                           .pull      = config.pull,
                                           .speed     = config.speed,
                                           .alternate = tkAfIdx});
        }
    };

}  // namespace valle