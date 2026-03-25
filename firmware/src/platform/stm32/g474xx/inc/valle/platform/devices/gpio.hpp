#pragma once

#include "valle/platform/core.hpp"
#include "valle/platform/hardware/gpio.hpp"

namespace valle::platform
{
    // ============================================================================
    // GPIO PORT (SHARED DEVICE)
    // ============================================================================
    /**
     * @brief Represents the GPIO Port Hardware (GPIOA, GPIOB...).
     * Responsibility: Managing the AHB Clock.
     */
    template <GPIOPortID tkPortID>
    class GPIOPortDevice
    {
    public:
        struct Descriptor : public SharedDeviceDescriptor
        {
        };

        static constexpr GPIOPortID skPortID = tkPortID;

        using PortTraitsT = GPIOPortTraits<tkPortID>;

        [[nodiscard]] bool init()
        {
            PortTraitsT::enable_clock();
            return true;
        }
    };

    // -----------------------------------------------------------------------------
    // DEVICE ALIASES
    // -----------------------------------------------------------------------------

    using GPIOPortADevice = GPIOPortDevice<GPIOPortID::kPortA>;
    using GPIOPortBDevice = GPIOPortDevice<GPIOPortID::kPortB>;
    using GPIOPortCDevice = GPIOPortDevice<GPIOPortID::kPortC>;
    using GPIOPortDDevice = GPIOPortDevice<GPIOPortID::kPortD>;
    using GPIOPortEDevice = GPIOPortDevice<GPIOPortID::kPortE>;
    using GPIOPortFDevice = GPIOPortDevice<GPIOPortID::kPortF>;
    using GPIOPortGDevice = GPIOPortDevice<GPIOPortID::kPortG>;

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
    struct GPIOPinConfig
    {
        uint32_t                mode      = GPIO_MODE_INPUT;
        GPIOPullMode            pull      = GPIOPullMode::kNoPull;
        GPIOSpeedMode           speed     = GPIOSpeedMode::kLow;
        GPIOAlternativeFunction alternate = GPIOAlternativeFunction::kAF0;
    };

    // -----------------------------------------------------------------------------
    // DEVICE CLASS
    // -----------------------------------------------------------------------------

    /**
     * @brief Represents the Physical Pin.
     * Pure resource container. No functional API.
     */
    template <GPIOPortID tkPortID, GPIOPinID tkPinID>
    class GPIOPinDevice
    {
    public:
        struct Descriptor : public UniqueDeviceDescriptor
        {
        };

        constexpr static GPIOPortID skPortID = tkPortID;
        constexpr static GPIOPinID  skPinID  = tkPinID;

        using PortTraitsT          = GPIOPortTraits<tkPortID>;
        using PinTraitsT           = GPIOPinTraits<tkPortID, tkPinID>;
        using InterruptControllerT = GPIOPinInterruptController<tkPortID, tkPinID>;

        using DependDevices = TypeList<GPIOPortDevice<tkPortID>>;

    public:
        [[nodiscard]] inline bool init(const GPIOPinConfig& config)
        {
            GPIO_InitTypeDef init{};
            init.Pin       = PinTraitsT::skPinMask;
            init.Mode      = static_cast<uint32_t>(config.mode);
            init.Pull      = static_cast<uint32_t>(config.pull);
            init.Speed     = static_cast<uint32_t>(config.speed);
            init.Alternate = static_cast<uint32_t>(config.alternate);
            HAL_GPIO_Init(PortTraitsT::skInstance, &init);
            return true;
        }

        void enable_interrupts(const GPIOPinInterruptConfig& config)
        {
            InterruptControllerT::enable_interrupts(config);
        }

        void disable_interrupts()
        {
            InterruptControllerT::disable_interrupts();
        }

        inline void write(bool state)
        {
            HAL_GPIO_WritePin(PortTraitsT::skInstance, PinTraitsT::skPinMask, state ? GPIO_PIN_SET : GPIO_PIN_RESET);
        }

        inline void toggle()
        {
            HAL_GPIO_TogglePin(PortTraitsT::skInstance, PinTraitsT::skPinMask);
        }

        [[nodiscard]] static inline bool read()
        {
            return (HAL_GPIO_ReadPin(PortTraitsT::skInstance, PinTraitsT::skPinMask) == GPIO_PIN_SET);
        }
    };

    class GPIONullPinDevice
    {
    public:
        struct Descriptor : public NullDeviceDescriptor
        {
        };
    };

    template <typename T>
    concept CNullGPIOPinDevice = std::is_same_v<T, GPIONullPinDevice>;

    template <typename T>
    concept CGPIOPinDevice = std::is_base_of_v<GPIOPinDevice<T::skPortID, T::skPinID>, T>;

    // -----------------------------------------------------------------------------
    // DEVICE ALIASES
    // -----------------------------------------------------------------------------

    using GPIOPinA0Device  = GPIOPinDevice<GPIOPortID::kPortA, GPIOPinID::kPin0>;
    using GPIOPinA1Device  = GPIOPinDevice<GPIOPortID::kPortA, GPIOPinID::kPin1>;
    using GPIOPinA2Device  = GPIOPinDevice<GPIOPortID::kPortA, GPIOPinID::kPin2>;
    using GPIOPinA3Device  = GPIOPinDevice<GPIOPortID::kPortA, GPIOPinID::kPin3>;
    using GPIOPinA4Device  = GPIOPinDevice<GPIOPortID::kPortA, GPIOPinID::kPin4>;
    using GPIOPinA5Device  = GPIOPinDevice<GPIOPortID::kPortA, GPIOPinID::kPin5>;
    using GPIOPinA6Device  = GPIOPinDevice<GPIOPortID::kPortA, GPIOPinID::kPin6>;
    using GPIOPinA7Device  = GPIOPinDevice<GPIOPortID::kPortA, GPIOPinID::kPin7>;
    using GPIOPinA8Device  = GPIOPinDevice<GPIOPortID::kPortA, GPIOPinID::kPin8>;
    using GPIOPinA9Device  = GPIOPinDevice<GPIOPortID::kPortA, GPIOPinID::kPin9>;
    using GPIOPinA10Device = GPIOPinDevice<GPIOPortID::kPortA, GPIOPinID::kPin10>;
    using GPIOPinA11Device = GPIOPinDevice<GPIOPortID::kPortA, GPIOPinID::kPin11>;
    using GPIOPinA12Device = GPIOPinDevice<GPIOPortID::kPortA, GPIOPinID::kPin12>;
    using GPIOPinA13Device = GPIOPinDevice<GPIOPortID::kPortA, GPIOPinID::kPin13>;
    using GPIOPinA14Device = GPIOPinDevice<GPIOPortID::kPortA, GPIOPinID::kPin14>;
    using GPIOPinA15Device = GPIOPinDevice<GPIOPortID::kPortA, GPIOPinID::kPin15>;

    using GPIOPinB0Device  = GPIOPinDevice<GPIOPortID::kPortB, GPIOPinID::kPin0>;
    using GPIOPinB1Device  = GPIOPinDevice<GPIOPortID::kPortB, GPIOPinID::kPin1>;
    using GPIOPinB2Device  = GPIOPinDevice<GPIOPortID::kPortB, GPIOPinID::kPin2>;
    using GPIOPinB3Device  = GPIOPinDevice<GPIOPortID::kPortB, GPIOPinID::kPin3>;
    using GPIOPinB4Device  = GPIOPinDevice<GPIOPortID::kPortB, GPIOPinID::kPin4>;
    using GPIOPinB5Device  = GPIOPinDevice<GPIOPortID::kPortB, GPIOPinID::kPin5>;
    using GPIOPinB6Device  = GPIOPinDevice<GPIOPortID::kPortB, GPIOPinID::kPin6>;
    using GPIOPinB7Device  = GPIOPinDevice<GPIOPortID::kPortB, GPIOPinID::kPin7>;
    using GPIOPinB8Device  = GPIOPinDevice<GPIOPortID::kPortB, GPIOPinID::kPin8>;
    using GPIOPinB9Device  = GPIOPinDevice<GPIOPortID::kPortB, GPIOPinID::kPin9>;
    using GPIOPinB10Device = GPIOPinDevice<GPIOPortID::kPortB, GPIOPinID::kPin10>;
    using GPIOPinB11Device = GPIOPinDevice<GPIOPortID::kPortB, GPIOPinID::kPin11>;
    using GPIOPinB12Device = GPIOPinDevice<GPIOPortID::kPortB, GPIOPinID::kPin12>;
    using GPIOPinB13Device = GPIOPinDevice<GPIOPortID::kPortB, GPIOPinID::kPin13>;
    using GPIOPinB14Device = GPIOPinDevice<GPIOPortID::kPortB, GPIOPinID::kPin14>;
    using GPIOPinB15Device = GPIOPinDevice<GPIOPortID::kPortB, GPIOPinID::kPin15>;

    using GPIOPinC0Device  = GPIOPinDevice<GPIOPortID::kPortC, GPIOPinID::kPin0>;
    using GPIOPinC1Device  = GPIOPinDevice<GPIOPortID::kPortC, GPIOPinID::kPin1>;
    using GPIOPinC2Device  = GPIOPinDevice<GPIOPortID::kPortC, GPIOPinID::kPin2>;
    using GPIOPinC3Device  = GPIOPinDevice<GPIOPortID::kPortC, GPIOPinID::kPin3>;
    using GPIOPinC4Device  = GPIOPinDevice<GPIOPortID::kPortC, GPIOPinID::kPin4>;
    using GPIOPinC5Device  = GPIOPinDevice<GPIOPortID::kPortC, GPIOPinID::kPin5>;
    using GPIOPinC6Device  = GPIOPinDevice<GPIOPortID::kPortC, GPIOPinID::kPin6>;
    using GPIOPinC7Device  = GPIOPinDevice<GPIOPortID::kPortC, GPIOPinID::kPin7>;
    using GPIOPinC8Device  = GPIOPinDevice<GPIOPortID::kPortC, GPIOPinID::kPin8>;
    using GPIOPinC9Device  = GPIOPinDevice<GPIOPortID::kPortC, GPIOPinID::kPin9>;
    using GPIOPinC10Device = GPIOPinDevice<GPIOPortID::kPortC, GPIOPinID::kPin10>;
    using GPIOPinC11Device = GPIOPinDevice<GPIOPortID::kPortC, GPIOPinID::kPin11>;
    using GPIOPinC12Device = GPIOPinDevice<GPIOPortID::kPortC, GPIOPinID::kPin12>;
    using GPIOPinC13Device = GPIOPinDevice<GPIOPortID::kPortC, GPIOPinID::kPin13>;
    using GPIOPinC14Device = GPIOPinDevice<GPIOPortID::kPortC, GPIOPinID::kPin14>;
    using GPIOPinC15Device = GPIOPinDevice<GPIOPortID::kPortC, GPIOPinID::kPin15>;

    using GPIOPinD0Device  = GPIOPinDevice<GPIOPortID::kPortD, GPIOPinID::kPin0>;
    using GPIOPinD1Device  = GPIOPinDevice<GPIOPortID::kPortD, GPIOPinID::kPin1>;
    using GPIOPinD2Device  = GPIOPinDevice<GPIOPortID::kPortD, GPIOPinID::kPin2>;
    using GPIOPinD3Device  = GPIOPinDevice<GPIOPortID::kPortD, GPIOPinID::kPin3>;
    using GPIOPinD4Device  = GPIOPinDevice<GPIOPortID::kPortD, GPIOPinID::kPin4>;
    using GPIOPinD5Device  = GPIOPinDevice<GPIOPortID::kPortD, GPIOPinID::kPin5>;
    using GPIOPinD6Device  = GPIOPinDevice<GPIOPortID::kPortD, GPIOPinID::kPin6>;
    using GPIOPinD7Device  = GPIOPinDevice<GPIOPortID::kPortD, GPIOPinID::kPin7>;
    using GPIOPinD8Device  = GPIOPinDevice<GPIOPortID::kPortD, GPIOPinID::kPin8>;
    using GPIOPinD9Device  = GPIOPinDevice<GPIOPortID::kPortD, GPIOPinID::kPin9>;
    using GPIOPinD10Device = GPIOPinDevice<GPIOPortID::kPortD, GPIOPinID::kPin10>;
    using GPIOPinD11Device = GPIOPinDevice<GPIOPortID::kPortD, GPIOPinID::kPin11>;
    using GPIOPinD12Device = GPIOPinDevice<GPIOPortID::kPortD, GPIOPinID::kPin12>;
    using GPIOPinD13Device = GPIOPinDevice<GPIOPortID::kPortD, GPIOPinID::kPin13>;
    using GPIOPinD14Device = GPIOPinDevice<GPIOPortID::kPortD, GPIOPinID::kPin14>;
    using GPIOPinD15Device = GPIOPinDevice<GPIOPortID::kPortD, GPIOPinID::kPin15>;

    using GPIOPinE0Device  = GPIOPinDevice<GPIOPortID::kPortE, GPIOPinID::kPin0>;
    using GPIOPinE1Device  = GPIOPinDevice<GPIOPortID::kPortE, GPIOPinID::kPin1>;
    using GPIOPinE2Device  = GPIOPinDevice<GPIOPortID::kPortE, GPIOPinID::kPin2>;
    using GPIOPinE3Device  = GPIOPinDevice<GPIOPortID::kPortE, GPIOPinID::kPin3>;
    using GPIOPinE4Device  = GPIOPinDevice<GPIOPortID::kPortE, GPIOPinID::kPin4>;
    using GPIOPinE5Device  = GPIOPinDevice<GPIOPortID::kPortE, GPIOPinID::kPin5>;
    using GPIOPinE6Device  = GPIOPinDevice<GPIOPortID::kPortE, GPIOPinID::kPin6>;
    using GPIOPinE7Device  = GPIOPinDevice<GPIOPortID::kPortE, GPIOPinID::kPin7>;
    using GPIOPinE8Device  = GPIOPinDevice<GPIOPortID::kPortE, GPIOPinID::kPin8>;
    using GPIOPinE9Device  = GPIOPinDevice<GPIOPortID::kPortE, GPIOPinID::kPin9>;
    using GPIOPinE10Device = GPIOPinDevice<GPIOPortID::kPortE, GPIOPinID::kPin10>;
    using GPIOPinE11Device = GPIOPinDevice<GPIOPortID::kPortE, GPIOPinID::kPin11>;
    using GPIOPinE12Device = GPIOPinDevice<GPIOPortID::kPortE, GPIOPinID::kPin12>;
    using GPIOPinE13Device = GPIOPinDevice<GPIOPortID::kPortE, GPIOPinID::kPin13>;
    using GPIOPinE14Device = GPIOPinDevice<GPIOPortID::kPortE, GPIOPinID::kPin14>;
    using GPIOPinE15Device = GPIOPinDevice<GPIOPortID::kPortE, GPIOPinID::kPin15>;

    using GPIOPinF0Device  = GPIOPinDevice<GPIOPortID::kPortF, GPIOPinID::kPin0>;
    using GPIOPinF1Device  = GPIOPinDevice<GPIOPortID::kPortF, GPIOPinID::kPin1>;
    using GPIOPinF2Device  = GPIOPinDevice<GPIOPortID::kPortF, GPIOPinID::kPin2>;
    using GPIOPinF3Device  = GPIOPinDevice<GPIOPortID::kPortF, GPIOPinID::kPin3>;
    using GPIOPinF4Device  = GPIOPinDevice<GPIOPortID::kPortF, GPIOPinID::kPin4>;
    using GPIOPinF5Device  = GPIOPinDevice<GPIOPortID::kPortF, GPIOPinID::kPin5>;
    using GPIOPinF6Device  = GPIOPinDevice<GPIOPortID::kPortF, GPIOPinID::kPin6>;
    using GPIOPinF7Device  = GPIOPinDevice<GPIOPortID::kPortF, GPIOPinID::kPin7>;
    using GPIOPinF8Device  = GPIOPinDevice<GPIOPortID::kPortF, GPIOPinID::kPin8>;
    using GPIOPinF9Device  = GPIOPinDevice<GPIOPortID::kPortF, GPIOPinID::kPin9>;
    using GPIOPinF10Device = GPIOPinDevice<GPIOPortID::kPortF, GPIOPinID::kPin10>;
    using GPIOPinF11Device = GPIOPinDevice<GPIOPortID::kPortF, GPIOPinID::kPin11>;
    using GPIOPinF12Device = GPIOPinDevice<GPIOPortID::kPortF, GPIOPinID::kPin12>;
    using GPIOPinF13Device = GPIOPinDevice<GPIOPortID::kPortF, GPIOPinID::kPin13>;
    using GPIOPinF14Device = GPIOPinDevice<GPIOPortID::kPortF, GPIOPinID::kPin14>;
    using GPIOPinF15Device = GPIOPinDevice<GPIOPortID::kPortF, GPIOPinID::kPin15>;

}  // namespace valle::platform
