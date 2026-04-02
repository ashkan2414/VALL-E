#pragma once

#include "valle/platform/core.hpp"
#include "valle/platform/devices/exti.hpp"
#include "valle/platform/hdi/gpio.hpp"

namespace valle::platform
{
    // ============================================================================
    // GPIO PORT (SHARED DEVICE)
    // ============================================================================
    /**
     * @brief Represents the GPIO Port Hardware (GpioA, GpioB...).
     * Responsibility: Managing the AHB Clock.
     */
    template <GpioPortId tkPortId>
    class GpioPort
    {
    public:
        struct Descriptor : public SharedDeviceDescriptor
        {
        };

        static constexpr GpioPortId skPortId = tkPortId;

        using PortTraitsT   = GpioPortTraits<tkPortId>;
        using PortHdiT      = GpioPortHdi<tkPortId>;
        using InjectDevices = TypeList<PortHdiT>;

    private:
        [[no_unique_address]] DeviceRef<PortHdiT> m_port_hw;

    public:
        GpioPort() = delete;

        explicit GpioPort(DeviceRef<PortHdiT>&& hardware_key) : m_port_hw(std::move(hardware_key))
        {
        }

        [[nodiscard]] bool init()
        {
            m_port_hw->enable_clock();
            return true;
        }
    };

    // -----------------------------------------------------------------------------
    // DEVICE ALIASES
    // -----------------------------------------------------------------------------

    using GpioPortA = GpioPort<GpioPortId::kPortA>;
    using GpioPortB = GpioPort<GpioPortId::kPortB>;
    using GpioPortC = GpioPort<GpioPortId::kPortC>;
    using GpioPortD = GpioPort<GpioPortId::kPortD>;
    using GpioPortE = GpioPort<GpioPortId::kPortE>;
    using GpioPortF = GpioPort<GpioPortId::kPortF>;
    using GpioPortG = GpioPort<GpioPortId::kPortG>;

    // ============================================================================
    // GPIO PIN (UNIQUE DEVICE)
    // ============================================================================

    // -----------------------------------------------------------------------------
    // COMPILE TIME CONFIGURATIONS
    // -----------------------------------------------------------------------------
    struct GpioPinDefaultCTConfig
    {
        static constexpr bool skUseExti = false;
    };

    // -----------------------------------------------------------------------------
    // CONFIGURATIONS
    // -----------------------------------------------------------------------------

    struct GpioInterruptConfig
    {
        uint32_t              priority;
        ExtiLineTrigger       trigger        = ExtiLineTrigger::kRising;
        ExtiLineTriggerAction trigger_action = ExtiLineTriggerAction::kInterrupt;
    };

    struct GpioDigitalInConfig
    {
        GpioPinPullMode                    pull      = GpioPinPullMode::kNoPull;
        std::optional<GpioInterruptConfig> interrupt = std::nullopt;
    };

    struct GpioDigitalOutConfig
    {
        GpioPinOutputMode mode  = GpioPinOutputMode::kPushPull;
        GpioPinSpeedMode  speed = GpioPinSpeedMode::kLow;
        GpioPinPullMode   pull  = GpioPinPullMode::kNoPull;
    };

    struct GpioAlternateFunctionConfig
    {
        GpioPinOutputMode       mode               = GpioPinOutputMode::kPushPull;
        GpioPinSpeedMode        speed              = GpioPinSpeedMode::kLow;
        GpioPinPullMode         pull               = GpioPinPullMode::kNoPull;
        GpioAlternativeFunction alternate_function = GpioAlternativeFunction::kAF0;
    };

    struct GpioAnalogInConfig
    {
    };

    using GpioPinConfig =
        std::variant<GpioDigitalInConfig, GpioDigitalOutConfig, GpioAlternateFunctionConfig, GpioAnalogInConfig>;

    // -----------------------------------------------------------------------------
    // DEVICE CLASS
    // -----------------------------------------------------------------------------

    /**
     * @brief Represents the Physical Pin.
     * Pure resource container. No functional API.
     */
    template <GpioPortId tkPortId, GpioPinId tkPinId, typename TCTConfig = GpioPinDefaultCTConfig>
    class GpioPin
    {
    public:
        struct Descriptor : public UniqueDeviceDescriptor
        {
        };

        constexpr static GpioPortId skPortId = tkPortId;
        constexpr static GpioPinId  skPinId  = tkPinId;

        using PortTraitsT = GpioPortTraits<tkPortId>;
        using PinTraitsT  = GpioPinTraits<tkPortId, tkPinId>;

        using CTConfigT                 = TCTConfig;
        static constexpr bool skUseExti = CTConfigT::skUseExti;

        using PinHdiT              = GpioPinHdi<tkPortId, tkPinId>;
        using InterruptControllerT = GpioPinInterruptController<tkPortId, tkPinId>;
        using ExtiLineT            = std::conditional_t<skUseExti, ExtiLine<PinTraitsT::skExtiLine>, NullDevice>;

        using DependDevices = TypeList<GpioPort<tkPortId>>;
        using InjectDevices = typename FilterNullDevices<TypeList<PinHdiT, ExtiLineT>>::type;

    private:
        [[no_unique_address]] DeviceRef<PinHdiT>                                    m_pin_hw;
        [[no_unique_address]] ConditionalDeviceRef<skUseExti, DeviceRef<ExtiLineT>> m_exti_line{};

    public:
        template <typename... TArgs>
        explicit GpioPin(TArgs&&... args)
            : m_pin_hw(extract_device_ref<true, PinHdiT>(std::forward<TArgs>(args)...))
            , m_exti_line(extract_device_ref<skUseExti, ExtiLineT>(std::forward<TArgs>(args)...))
        {
        }

        [[nodiscard]] bool init_as_digital_in(const GpioDigitalInConfig& config)
        {
            m_pin_hw->set_mode(GpioPinMode::kInput);
            m_pin_hw->set_pull(config.pull);

            if constexpr (skUseExti)
            {
                if (config.interrupt.has_value())
                {
                    enable_interrupt(config.interrupt.value());
                }
            }

            return true;
        }

        [[nodiscard]] bool init_as_digital_out(const GpioDigitalOutConfig& config)
        {
            m_pin_hw->set_mode(GpioPinMode::kOutput);
            m_pin_hw->set_pull(config.pull);
            m_pin_hw->set_speed(config.speed);
            m_pin_hw->set_output_mode(config.mode);
            return true;
        }

        [[nodiscard]] bool init_as_alternate_function(const GpioAlternateFunctionConfig& config)
        {
            m_pin_hw->set_mode(GpioPinMode::kAlternateFunction);
            m_pin_hw->set_pull(config.pull);
            m_pin_hw->set_speed(config.speed);
            m_pin_hw->set_alternate_function(config.alternate);
            return true;
        }

        [[nodiscard]] bool init_as_analog_in(const GpioAnalogInConfig& config)
        {
            (void)config;  // No configurable parameters for analog mode
            m_pin_hw->set_mode(GpioPinMode::kAnalog);
            return true;
        }

        [[nodiscard]] bool init(const GpioPinConfig& config)
        {
            return std::visit(
                Overloaded{
                    [this](const GpioDigitalInConfig& digital_in_config)
                    { return init_as_digital_in(digital_in_config); },
                    [this](const GpioDigitalOutConfig& digital_out_config)
                    { return init_as_digital_out(digital_out_config); },
                    [this](const GpioAlternateFunctionConfig& af_config)
                    { return init_as_alternate_function(af_config); },
                    [this](const GpioAnalogInConfig& analog_in_config) { return init_as_analog_in(analog_in_config); },
                },
                config);
        }

        [[nodiscard]] bool enable_interrupt(const GpioInterruptConfig& config)
            requires(skUseExti)
        {
            m_exti_line->init(ExtiLineInterruptConfig{
                .source         = PinTraitsT::skExtiLineSource,
                .trigger        = config.trigger,
                .trigger_action = config.trigger_action,
            });
            m_exti_line->enable_interrupts(ExtiLineInterruptControllerConfig{.priority = config.priority});
        }

        void enable_interrupts(const GpioPinInterruptConfig& config)
        {
            InterruptControllerT::enable_interrupts(config);
        }

        void disable_interrupts()
        {
            InterruptControllerT::disable_interrupts();
        }

        void lock() const
        {
            m_pin_hw->lock();
        }

        [[nodiscard]] bool is_locked() const
        {
            return m_pin_hw->is_locked();
        }

        [[nodiscard]] bool read_input() const
        {
            return m_pin_hw->is_input_set();
        }

        [[nodiscard]] bool read_output() const
        {
            return m_pin_hw->is_output_set();
        }

        void set() const
        {
            m_pin_hw->set_output();
        }

        void reset() const
        {
            m_pin_hw->reset_output();
        }

        void write(bool state)
        {
            if (state)
            {
                set();
            }
            else
            {
                reset();
            }
        }

        void toggle() const
        {
            m_pin_hw->toggle();
        }
    };

    template <typename T>
    concept CGpioPin = std::is_base_of_v<GpioPin<T::skPortId, T::skPinId>, T>;

    // -----------------------------------------------------------------------------
    // DEVICE ALIASES
    // -----------------------------------------------------------------------------

    using GpioPinA0  = GpioPin<GpioPortId::kPortA, GpioPinId::kPin0>;
    using GpioPinA1  = GpioPin<GpioPortId::kPortA, GpioPinId::kPin1>;
    using GpioPinA2  = GpioPin<GpioPortId::kPortA, GpioPinId::kPin2>;
    using GpioPinA3  = GpioPin<GpioPortId::kPortA, GpioPinId::kPin3>;
    using GpioPinA4  = GpioPin<GpioPortId::kPortA, GpioPinId::kPin4>;
    using GpioPinA5  = GpioPin<GpioPortId::kPortA, GpioPinId::kPin5>;
    using GpioPinA6  = GpioPin<GpioPortId::kPortA, GpioPinId::kPin6>;
    using GpioPinA7  = GpioPin<GpioPortId::kPortA, GpioPinId::kPin7>;
    using GpioPinA8  = GpioPin<GpioPortId::kPortA, GpioPinId::kPin8>;
    using GpioPinA9  = GpioPin<GpioPortId::kPortA, GpioPinId::kPin9>;
    using GpioPinA10 = GpioPin<GpioPortId::kPortA, GpioPinId::kPin10>;
    using GpioPinA11 = GpioPin<GpioPortId::kPortA, GpioPinId::kPin11>;
    using GpioPinA12 = GpioPin<GpioPortId::kPortA, GpioPinId::kPin12>;
    using GpioPinA13 = GpioPin<GpioPortId::kPortA, GpioPinId::kPin13>;
    using GpioPinA14 = GpioPin<GpioPortId::kPortA, GpioPinId::kPin14>;
    using GpioPinA15 = GpioPin<GpioPortId::kPortA, GpioPinId::kPin15>;

    using GpioPinB0  = GpioPin<GpioPortId::kPortB, GpioPinId::kPin0>;
    using GpioPinB1  = GpioPin<GpioPortId::kPortB, GpioPinId::kPin1>;
    using GpioPinB2  = GpioPin<GpioPortId::kPortB, GpioPinId::kPin2>;
    using GpioPinB3  = GpioPin<GpioPortId::kPortB, GpioPinId::kPin3>;
    using GpioPinB4  = GpioPin<GpioPortId::kPortB, GpioPinId::kPin4>;
    using GpioPinB5  = GpioPin<GpioPortId::kPortB, GpioPinId::kPin5>;
    using GpioPinB6  = GpioPin<GpioPortId::kPortB, GpioPinId::kPin6>;
    using GpioPinB7  = GpioPin<GpioPortId::kPortB, GpioPinId::kPin7>;
    using GpioPinB8  = GpioPin<GpioPortId::kPortB, GpioPinId::kPin8>;
    using GpioPinB9  = GpioPin<GpioPortId::kPortB, GpioPinId::kPin9>;
    using GpioPinB10 = GpioPin<GpioPortId::kPortB, GpioPinId::kPin10>;
    using GpioPinB11 = GpioPin<GpioPortId::kPortB, GpioPinId::kPin11>;
    using GpioPinB12 = GpioPin<GpioPortId::kPortB, GpioPinId::kPin12>;
    using GpioPinB13 = GpioPin<GpioPortId::kPortB, GpioPinId::kPin13>;
    using GpioPinB14 = GpioPin<GpioPortId::kPortB, GpioPinId::kPin14>;
    using GpioPinB15 = GpioPin<GpioPortId::kPortB, GpioPinId::kPin15>;

    using GpioPinC0  = GpioPin<GpioPortId::kPortC, GpioPinId::kPin0>;
    using GpioPinC1  = GpioPin<GpioPortId::kPortC, GpioPinId::kPin1>;
    using GpioPinC2  = GpioPin<GpioPortId::kPortC, GpioPinId::kPin2>;
    using GpioPinC3  = GpioPin<GpioPortId::kPortC, GpioPinId::kPin3>;
    using GpioPinC4  = GpioPin<GpioPortId::kPortC, GpioPinId::kPin4>;
    using GpioPinC5  = GpioPin<GpioPortId::kPortC, GpioPinId::kPin5>;
    using GpioPinC6  = GpioPin<GpioPortId::kPortC, GpioPinId::kPin6>;
    using GpioPinC7  = GpioPin<GpioPortId::kPortC, GpioPinId::kPin7>;
    using GpioPinC8  = GpioPin<GpioPortId::kPortC, GpioPinId::kPin8>;
    using GpioPinC9  = GpioPin<GpioPortId::kPortC, GpioPinId::kPin9>;
    using GpioPinC10 = GpioPin<GpioPortId::kPortC, GpioPinId::kPin10>;
    using GpioPinC11 = GpioPin<GpioPortId::kPortC, GpioPinId::kPin11>;
    using GpioPinC12 = GpioPin<GpioPortId::kPortC, GpioPinId::kPin12>;
    using GpioPinC13 = GpioPin<GpioPortId::kPortC, GpioPinId::kPin13>;
    using GpioPinC14 = GpioPin<GpioPortId::kPortC, GpioPinId::kPin14>;
    using GpioPinC15 = GpioPin<GpioPortId::kPortC, GpioPinId::kPin15>;

    using GpioPinD0  = GpioPin<GpioPortId::kPortD, GpioPinId::kPin0>;
    using GpioPinD1  = GpioPin<GpioPortId::kPortD, GpioPinId::kPin1>;
    using GpioPinD2  = GpioPin<GpioPortId::kPortD, GpioPinId::kPin2>;
    using GpioPinD3  = GpioPin<GpioPortId::kPortD, GpioPinId::kPin3>;
    using GpioPinD4  = GpioPin<GpioPortId::kPortD, GpioPinId::kPin4>;
    using GpioPinD5  = GpioPin<GpioPortId::kPortD, GpioPinId::kPin5>;
    using GpioPinD6  = GpioPin<GpioPortId::kPortD, GpioPinId::kPin6>;
    using GpioPinD7  = GpioPin<GpioPortId::kPortD, GpioPinId::kPin7>;
    using GpioPinD8  = GpioPin<GpioPortId::kPortD, GpioPinId::kPin8>;
    using GpioPinD9  = GpioPin<GpioPortId::kPortD, GpioPinId::kPin9>;
    using GpioPinD10 = GpioPin<GpioPortId::kPortD, GpioPinId::kPin10>;
    using GpioPinD11 = GpioPin<GpioPortId::kPortD, GpioPinId::kPin11>;
    using GpioPinD12 = GpioPin<GpioPortId::kPortD, GpioPinId::kPin12>;
    using GpioPinD13 = GpioPin<GpioPortId::kPortD, GpioPinId::kPin13>;
    using GpioPinD14 = GpioPin<GpioPortId::kPortD, GpioPinId::kPin14>;
    using GpioPinD15 = GpioPin<GpioPortId::kPortD, GpioPinId::kPin15>;

    using GpioPinE0  = GpioPin<GpioPortId::kPortE, GpioPinId::kPin0>;
    using GpioPinE1  = GpioPin<GpioPortId::kPortE, GpioPinId::kPin1>;
    using GpioPinE2  = GpioPin<GpioPortId::kPortE, GpioPinId::kPin2>;
    using GpioPinE3  = GpioPin<GpioPortId::kPortE, GpioPinId::kPin3>;
    using GpioPinE4  = GpioPin<GpioPortId::kPortE, GpioPinId::kPin4>;
    using GpioPinE5  = GpioPin<GpioPortId::kPortE, GpioPinId::kPin5>;
    using GpioPinE6  = GpioPin<GpioPortId::kPortE, GpioPinId::kPin6>;
    using GpioPinE7  = GpioPin<GpioPortId::kPortE, GpioPinId::kPin7>;
    using GpioPinE8  = GpioPin<GpioPortId::kPortE, GpioPinId::kPin8>;
    using GpioPinE9  = GpioPin<GpioPortId::kPortE, GpioPinId::kPin9>;
    using GpioPinE10 = GpioPin<GpioPortId::kPortE, GpioPinId::kPin10>;
    using GpioPinE11 = GpioPin<GpioPortId::kPortE, GpioPinId::kPin11>;
    using GpioPinE12 = GpioPin<GpioPortId::kPortE, GpioPinId::kPin12>;
    using GpioPinE13 = GpioPin<GpioPortId::kPortE, GpioPinId::kPin13>;
    using GpioPinE14 = GpioPin<GpioPortId::kPortE, GpioPinId::kPin14>;
    using GpioPinE15 = GpioPin<GpioPortId::kPortE, GpioPinId::kPin15>;

    using GpioPinF0  = GpioPin<GpioPortId::kPortF, GpioPinId::kPin0>;
    using GpioPinF1  = GpioPin<GpioPortId::kPortF, GpioPinId::kPin1>;
    using GpioPinF2  = GpioPin<GpioPortId::kPortF, GpioPinId::kPin2>;
    using GpioPinF3  = GpioPin<GpioPortId::kPortF, GpioPinId::kPin3>;
    using GpioPinF4  = GpioPin<GpioPortId::kPortF, GpioPinId::kPin4>;
    using GpioPinF5  = GpioPin<GpioPortId::kPortF, GpioPinId::kPin5>;
    using GpioPinF6  = GpioPin<GpioPortId::kPortF, GpioPinId::kPin6>;
    using GpioPinF7  = GpioPin<GpioPortId::kPortF, GpioPinId::kPin7>;
    using GpioPinF8  = GpioPin<GpioPortId::kPortF, GpioPinId::kPin8>;
    using GpioPinF9  = GpioPin<GpioPortId::kPortF, GpioPinId::kPin9>;
    using GpioPinF10 = GpioPin<GpioPortId::kPortF, GpioPinId::kPin10>;
    using GpioPinF11 = GpioPin<GpioPortId::kPortF, GpioPinId::kPin11>;
    using GpioPinF12 = GpioPin<GpioPortId::kPortF, GpioPinId::kPin12>;
    using GpioPinF13 = GpioPin<GpioPortId::kPortF, GpioPinId::kPin13>;
    using GpioPinF14 = GpioPin<GpioPortId::kPortF, GpioPinId::kPin14>;
    using GpioPinF15 = GpioPin<GpioPortId::kPortF, GpioPinId::kPin15>;

}  // namespace valle::platform
