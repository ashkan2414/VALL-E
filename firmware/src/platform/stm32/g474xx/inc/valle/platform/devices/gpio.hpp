#pragma once

#include "valle/platform/core.hpp"
#include "valle/platform/hardware/gpio.hpp"

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
    class GpioPortDevice
    {
    public:
        struct Descriptor : public SharedDeviceDescriptor
        {
        };

        static constexpr GpioPortId skPortId = tkPortId;

        using PortTraitsT = GpioPortTraits<tkPortId>;

        [[nodiscard]] bool init()
        {
            PortTraitsT::enable_clock();
            return true;
        }
    };

    // -----------------------------------------------------------------------------
    // DEVICE ALIASES
    // -----------------------------------------------------------------------------

    using GpioPortADevice = GpioPortDevice<GpioPortId::kPortA>;
    using GpioPortBDevice = GpioPortDevice<GpioPortId::kPortB>;
    using GpioPortCDevice = GpioPortDevice<GpioPortId::kPortC>;
    using GpioPortDDevice = GpioPortDevice<GpioPortId::kPortD>;
    using GpioPortEDevice = GpioPortDevice<GpioPortId::kPortE>;
    using GpioPortFDevice = GpioPortDevice<GpioPortId::kPortF>;
    using GpioPortGDevice = GpioPortDevice<GpioPortId::kPortG>;

    // ============================================================================
    // GPIO PIN (UNIQUE DEVICE)
    // ============================================================================

    // -----------------------------------------------------------------------------
    // CONFIGURATIONS
    // -----------------------------------------------------------------------------

    /**
     * @brief Configuration structure for GPIO Pin initialization.
     *
     */
    struct GpioPinConfig
    {
        uint32_t                mode      = Gpio_MODE_INPUT;
        GpioPullMode            pull      = GpioPullMode::kNoPull;
        GpioSpeedMode           speed     = GpioSpeedMode::kLow;
        GpioAlternativeFunction alternate = GpioAlternativeFunction::kAF0;
    };

    // -----------------------------------------------------------------------------
    // DEVICE CLASS
    // -----------------------------------------------------------------------------

    /**
     * @brief Represents the Physical Pin.
     * Pure resource container. No functional API.
     */
    template <GpioPortId tkPortId, GpioPinId tkPinId>
    class GpioPinDevice
    {
    public:
        struct Descriptor : public UniqueDeviceDescriptor
        {
        };

        constexpr static GpioPortId skPortId = tkPortId;
        constexpr static GpioPinId  skPinId  = tkPinId;

        using PortTraitsT          = GpioPortTraits<tkPortId>;
        using PinTraitsT           = GpioPinTraits<tkPortId, tkPinId>;
        using InterruptControllerT = GpioPinInterruptController<tkPortId, tkPinId>;

        using DependDevices = TypeList<GpioPortDevice<tkPortId>>;

    public:
        [[nodiscard]] inline bool init(const GpioPinConfig& config)
        {
            Gpio_InitTypeDef init{};
            init.Pin       = PinTraitsT::skPinMask;
            init.Mode      = static_cast<uint32_t>(config.mode);
            init.Pull      = static_cast<uint32_t>(config.pull);
            init.Speed     = static_cast<uint32_t>(config.speed);
            init.Alternate = static_cast<uint32_t>(config.alternate);
            HAL_GPIO_Init(PortTraitsT::skInstance, &init);
            return true;
        }

        void enable_interrupts(const GpioPinInterruptConfig& config)
        {
            InterruptControllerT::enable_interrupts(config);
        }

        void disable_interrupts()
        {
            InterruptControllerT::disable_interrupts();
        }

        inline void write(bool state)
        {
            HAL_GPIO_WritePin(PortTraitsT::skInstance, PinTraitsT::skPinMask, state ? Gpio_PIN_SET : Gpio_PIN_RESET);
        }

        inline void toggle()
        {
            HAL_GPIO_TogglePin(PortTraitsT::skInstance, PinTraitsT::skPinMask);
        }

        [[nodiscard]] static inline bool read()
        {
            return (HAL_GPIO_ReadPin(PortTraitsT::skInstance, PinTraitsT::skPinMask) == Gpio_PIN_SET);
        }
    };

    class GpioNullPinDevice
    {
    public:
        struct Descriptor : public NullDeviceDescriptor
        {
        };
    };

    template <typename T>
    concept CNullGpioPinDevice = std::is_same_v<T, GpioNullPinDevice>;

    template <typename T>
    concept CGpioPinDevice = std::is_base_of_v<GpioPinDevice<T::skPortId, T::skPinId>, T>;

    // -----------------------------------------------------------------------------
    // DEVICE ALIASES
    // -----------------------------------------------------------------------------

    using GpioPinA0Device  = GpioPinDevice<GpioPortId::kPortA, GpioPinId::kPin0>;
    using GpioPinA1Device  = GpioPinDevice<GpioPortId::kPortA, GpioPinId::kPin1>;
    using GpioPinA2Device  = GpioPinDevice<GpioPortId::kPortA, GpioPinId::kPin2>;
    using GpioPinA3Device  = GpioPinDevice<GpioPortId::kPortA, GpioPinId::kPin3>;
    using GpioPinA4Device  = GpioPinDevice<GpioPortId::kPortA, GpioPinId::kPin4>;
    using GpioPinA5Device  = GpioPinDevice<GpioPortId::kPortA, GpioPinId::kPin5>;
    using GpioPinA6Device  = GpioPinDevice<GpioPortId::kPortA, GpioPinId::kPin6>;
    using GpioPinA7Device  = GpioPinDevice<GpioPortId::kPortA, GpioPinId::kPin7>;
    using GpioPinA8Device  = GpioPinDevice<GpioPortId::kPortA, GpioPinId::kPin8>;
    using GpioPinA9Device  = GpioPinDevice<GpioPortId::kPortA, GpioPinId::kPin9>;
    using GpioPinA10Device = GpioPinDevice<GpioPortId::kPortA, GpioPinId::kPin10>;
    using GpioPinA11Device = GpioPinDevice<GpioPortId::kPortA, GpioPinId::kPin11>;
    using GpioPinA12Device = GpioPinDevice<GpioPortId::kPortA, GpioPinId::kPin12>;
    using GpioPinA13Device = GpioPinDevice<GpioPortId::kPortA, GpioPinId::kPin13>;
    using GpioPinA14Device = GpioPinDevice<GpioPortId::kPortA, GpioPinId::kPin14>;
    using GpioPinA15Device = GpioPinDevice<GpioPortId::kPortA, GpioPinId::kPin15>;

    using GpioPinB0Device  = GpioPinDevice<GpioPortId::kPortB, GpioPinId::kPin0>;
    using GpioPinB1Device  = GpioPinDevice<GpioPortId::kPortB, GpioPinId::kPin1>;
    using GpioPinB2Device  = GpioPinDevice<GpioPortId::kPortB, GpioPinId::kPin2>;
    using GpioPinB3Device  = GpioPinDevice<GpioPortId::kPortB, GpioPinId::kPin3>;
    using GpioPinB4Device  = GpioPinDevice<GpioPortId::kPortB, GpioPinId::kPin4>;
    using GpioPinB5Device  = GpioPinDevice<GpioPortId::kPortB, GpioPinId::kPin5>;
    using GpioPinB6Device  = GpioPinDevice<GpioPortId::kPortB, GpioPinId::kPin6>;
    using GpioPinB7Device  = GpioPinDevice<GpioPortId::kPortB, GpioPinId::kPin7>;
    using GpioPinB8Device  = GpioPinDevice<GpioPortId::kPortB, GpioPinId::kPin8>;
    using GpioPinB9Device  = GpioPinDevice<GpioPortId::kPortB, GpioPinId::kPin9>;
    using GpioPinB10Device = GpioPinDevice<GpioPortId::kPortB, GpioPinId::kPin10>;
    using GpioPinB11Device = GpioPinDevice<GpioPortId::kPortB, GpioPinId::kPin11>;
    using GpioPinB12Device = GpioPinDevice<GpioPortId::kPortB, GpioPinId::kPin12>;
    using GpioPinB13Device = GpioPinDevice<GpioPortId::kPortB, GpioPinId::kPin13>;
    using GpioPinB14Device = GpioPinDevice<GpioPortId::kPortB, GpioPinId::kPin14>;
    using GpioPinB15Device = GpioPinDevice<GpioPortId::kPortB, GpioPinId::kPin15>;

    using GpioPinC0Device  = GpioPinDevice<GpioPortId::kPortC, GpioPinId::kPin0>;
    using GpioPinC1Device  = GpioPinDevice<GpioPortId::kPortC, GpioPinId::kPin1>;
    using GpioPinC2Device  = GpioPinDevice<GpioPortId::kPortC, GpioPinId::kPin2>;
    using GpioPinC3Device  = GpioPinDevice<GpioPortId::kPortC, GpioPinId::kPin3>;
    using GpioPinC4Device  = GpioPinDevice<GpioPortId::kPortC, GpioPinId::kPin4>;
    using GpioPinC5Device  = GpioPinDevice<GpioPortId::kPortC, GpioPinId::kPin5>;
    using GpioPinC6Device  = GpioPinDevice<GpioPortId::kPortC, GpioPinId::kPin6>;
    using GpioPinC7Device  = GpioPinDevice<GpioPortId::kPortC, GpioPinId::kPin7>;
    using GpioPinC8Device  = GpioPinDevice<GpioPortId::kPortC, GpioPinId::kPin8>;
    using GpioPinC9Device  = GpioPinDevice<GpioPortId::kPortC, GpioPinId::kPin9>;
    using GpioPinC10Device = GpioPinDevice<GpioPortId::kPortC, GpioPinId::kPin10>;
    using GpioPinC11Device = GpioPinDevice<GpioPortId::kPortC, GpioPinId::kPin11>;
    using GpioPinC12Device = GpioPinDevice<GpioPortId::kPortC, GpioPinId::kPin12>;
    using GpioPinC13Device = GpioPinDevice<GpioPortId::kPortC, GpioPinId::kPin13>;
    using GpioPinC14Device = GpioPinDevice<GpioPortId::kPortC, GpioPinId::kPin14>;
    using GpioPinC15Device = GpioPinDevice<GpioPortId::kPortC, GpioPinId::kPin15>;

    using GpioPinD0Device  = GpioPinDevice<GpioPortId::kPortD, GpioPinId::kPin0>;
    using GpioPinD1Device  = GpioPinDevice<GpioPortId::kPortD, GpioPinId::kPin1>;
    using GpioPinD2Device  = GpioPinDevice<GpioPortId::kPortD, GpioPinId::kPin2>;
    using GpioPinD3Device  = GpioPinDevice<GpioPortId::kPortD, GpioPinId::kPin3>;
    using GpioPinD4Device  = GpioPinDevice<GpioPortId::kPortD, GpioPinId::kPin4>;
    using GpioPinD5Device  = GpioPinDevice<GpioPortId::kPortD, GpioPinId::kPin5>;
    using GpioPinD6Device  = GpioPinDevice<GpioPortId::kPortD, GpioPinId::kPin6>;
    using GpioPinD7Device  = GpioPinDevice<GpioPortId::kPortD, GpioPinId::kPin7>;
    using GpioPinD8Device  = GpioPinDevice<GpioPortId::kPortD, GpioPinId::kPin8>;
    using GpioPinD9Device  = GpioPinDevice<GpioPortId::kPortD, GpioPinId::kPin9>;
    using GpioPinD10Device = GpioPinDevice<GpioPortId::kPortD, GpioPinId::kPin10>;
    using GpioPinD11Device = GpioPinDevice<GpioPortId::kPortD, GpioPinId::kPin11>;
    using GpioPinD12Device = GpioPinDevice<GpioPortId::kPortD, GpioPinId::kPin12>;
    using GpioPinD13Device = GpioPinDevice<GpioPortId::kPortD, GpioPinId::kPin13>;
    using GpioPinD14Device = GpioPinDevice<GpioPortId::kPortD, GpioPinId::kPin14>;
    using GpioPinD15Device = GpioPinDevice<GpioPortId::kPortD, GpioPinId::kPin15>;

    using GpioPinE0Device  = GpioPinDevice<GpioPortId::kPortE, GpioPinId::kPin0>;
    using GpioPinE1Device  = GpioPinDevice<GpioPortId::kPortE, GpioPinId::kPin1>;
    using GpioPinE2Device  = GpioPinDevice<GpioPortId::kPortE, GpioPinId::kPin2>;
    using GpioPinE3Device  = GpioPinDevice<GpioPortId::kPortE, GpioPinId::kPin3>;
    using GpioPinE4Device  = GpioPinDevice<GpioPortId::kPortE, GpioPinId::kPin4>;
    using GpioPinE5Device  = GpioPinDevice<GpioPortId::kPortE, GpioPinId::kPin5>;
    using GpioPinE6Device  = GpioPinDevice<GpioPortId::kPortE, GpioPinId::kPin6>;
    using GpioPinE7Device  = GpioPinDevice<GpioPortId::kPortE, GpioPinId::kPin7>;
    using GpioPinE8Device  = GpioPinDevice<GpioPortId::kPortE, GpioPinId::kPin8>;
    using GpioPinE9Device  = GpioPinDevice<GpioPortId::kPortE, GpioPinId::kPin9>;
    using GpioPinE10Device = GpioPinDevice<GpioPortId::kPortE, GpioPinId::kPin10>;
    using GpioPinE11Device = GpioPinDevice<GpioPortId::kPortE, GpioPinId::kPin11>;
    using GpioPinE12Device = GpioPinDevice<GpioPortId::kPortE, GpioPinId::kPin12>;
    using GpioPinE13Device = GpioPinDevice<GpioPortId::kPortE, GpioPinId::kPin13>;
    using GpioPinE14Device = GpioPinDevice<GpioPortId::kPortE, GpioPinId::kPin14>;
    using GpioPinE15Device = GpioPinDevice<GpioPortId::kPortE, GpioPinId::kPin15>;

    using GpioPinF0Device  = GpioPinDevice<GpioPortId::kPortF, GpioPinId::kPin0>;
    using GpioPinF1Device  = GpioPinDevice<GpioPortId::kPortF, GpioPinId::kPin1>;
    using GpioPinF2Device  = GpioPinDevice<GpioPortId::kPortF, GpioPinId::kPin2>;
    using GpioPinF3Device  = GpioPinDevice<GpioPortId::kPortF, GpioPinId::kPin3>;
    using GpioPinF4Device  = GpioPinDevice<GpioPortId::kPortF, GpioPinId::kPin4>;
    using GpioPinF5Device  = GpioPinDevice<GpioPortId::kPortF, GpioPinId::kPin5>;
    using GpioPinF6Device  = GpioPinDevice<GpioPortId::kPortF, GpioPinId::kPin6>;
    using GpioPinF7Device  = GpioPinDevice<GpioPortId::kPortF, GpioPinId::kPin7>;
    using GpioPinF8Device  = GpioPinDevice<GpioPortId::kPortF, GpioPinId::kPin8>;
    using GpioPinF9Device  = GpioPinDevice<GpioPortId::kPortF, GpioPinId::kPin9>;
    using GpioPinF10Device = GpioPinDevice<GpioPortId::kPortF, GpioPinId::kPin10>;
    using GpioPinF11Device = GpioPinDevice<GpioPortId::kPortF, GpioPinId::kPin11>;
    using GpioPinF12Device = GpioPinDevice<GpioPortId::kPortF, GpioPinId::kPin12>;
    using GpioPinF13Device = GpioPinDevice<GpioPortId::kPortF, GpioPinId::kPin13>;
    using GpioPinF14Device = GpioPinDevice<GpioPortId::kPortF, GpioPinId::kPin14>;
    using GpioPinF15Device = GpioPinDevice<GpioPortId::kPortF, GpioPinId::kPin15>;

}  // namespace valle::platform
