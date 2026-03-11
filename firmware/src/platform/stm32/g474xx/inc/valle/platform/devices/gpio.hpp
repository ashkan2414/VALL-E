#pragma once

#include "valle/platform/core.hpp"
#include "valle/platform/hardware/gpio.hpp"

namespace valle::platform
{
    // =============================================================================
    // Forward Declarations
    // =============================================================================

    class GPIORootDevice;

    template <GPIOPortID tkPortID>
    class GPIOPortDevice;

    template <GPIOPortID tkPortID, GPIOPinID tkPinID>
        requires(kValidGPIOPinID<tkPortID, tkPinID>)
    class GPIOPinDevice;

    // ============================================================================
    // GPIO ROOT DEVICE (INTERFACE)
    // ============================================================================
    /**
     * @brief Represents the GPIO Controller (GPIOA, GPIOB...).
     *
     */
    class GPIORootDevice
    {
    public:
        struct Descriptor : public InterfaceDeviceDescriptor
        {
            using Children = DeviceTreeList<GPIOPortDevice<GPIOPortID::kA>,
                                            GPIOPortDevice<GPIOPortID::kB>,
                                            GPIOPortDevice<GPIOPortID::kC>,
                                            GPIOPortDevice<GPIOPortID::kD>,
                                            GPIOPortDevice<GPIOPortID::kE>,
                                            GPIOPortDevice<GPIOPortID::kF>,
                                            GPIOPortDevice<GPIOPortID::kG>>;
        };
    };

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
            using Children = DeviceTreeList<GPIOPinDevice<tkPortID, 0>,
                                            GPIOPinDevice<tkPortID, 1>,
                                            GPIOPinDevice<tkPortID, 2>,
                                            GPIOPinDevice<tkPortID, 3>,
                                            GPIOPinDevice<tkPortID, 4>,
                                            GPIOPinDevice<tkPortID, 5>,
                                            GPIOPinDevice<tkPortID, 6>,
                                            GPIOPinDevice<tkPortID, 7>,
                                            GPIOPinDevice<tkPortID, 8>,
                                            GPIOPinDevice<tkPortID, 9>,
                                            GPIOPinDevice<tkPortID, 10>,
                                            GPIOPinDevice<tkPortID, 11>,
                                            GPIOPinDevice<tkPortID, 12>,
                                            GPIOPinDevice<tkPortID, 13>,
                                            GPIOPinDevice<tkPortID, 14>,
                                            GPIOPinDevice<tkPortID, 15>>;
        };

        static constexpr GPIOPortID skPortID = tkPortID;

        using DependDevices = TypeList<GPIORootDevice>;
        using PortTraitsT   = GPIOPortTraits<tkPortID>;

        [[nodiscard]] bool init()
        {
            PortTraitsT::enable_clock();
            return true;
        }
    };

    // -----------------------------------------------------------------------------
    // DEVICE ALIASES
    // -----------------------------------------------------------------------------

    using GPIOPortADevice = GPIOPortDevice<GPIOPortID::kA>;
    using GPIOPortBDevice = GPIOPortDevice<GPIOPortID::kB>;
    using GPIOPortCDevice = GPIOPortDevice<GPIOPortID::kC>;
    using GPIOPortDDevice = GPIOPortDevice<GPIOPortID::kD>;
    using GPIOPortEDevice = GPIOPortDevice<GPIOPortID::kE>;
    using GPIOPortFDevice = GPIOPortDevice<GPIOPortID::kF>;
    using GPIOPortGDevice = GPIOPortDevice<GPIOPortID::kG>;

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
        requires(kValidGPIOPinID<tkPortID, tkPinID>)
    class GPIOPinDevice
    {
    public:
        struct Descriptor : public UniqueDeviceDescriptor
        {
        };

        constexpr static GPIOPortID skPortID = tkPortID;
        constexpr static GPIOPinID  skPinID  = tkPinID;

        using DependDevices = TypeList<GPIOPortDevice<tkPortID>>;
        using PortTraitsT   = GPIOPortTraits<tkPortID>;
        using PinTraitsT    = GPIOPinTraits<tkPortID, tkPinID>;

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

    using GPIOPinA0Device  = GPIOPinDevice<GPIOPortID::kA, 0>;
    using GPIOPinA1Device  = GPIOPinDevice<GPIOPortID::kA, 1>;
    using GPIOPinA2Device  = GPIOPinDevice<GPIOPortID::kA, 2>;
    using GPIOPinA3Device  = GPIOPinDevice<GPIOPortID::kA, 3>;
    using GPIOPinA4Device  = GPIOPinDevice<GPIOPortID::kA, 4>;
    using GPIOPinA5Device  = GPIOPinDevice<GPIOPortID::kA, 5>;
    using GPIOPinA6Device  = GPIOPinDevice<GPIOPortID::kA, 6>;
    using GPIOPinA7Device  = GPIOPinDevice<GPIOPortID::kA, 7>;
    using GPIOPinA8Device  = GPIOPinDevice<GPIOPortID::kA, 8>;
    using GPIOPinA9Device  = GPIOPinDevice<GPIOPortID::kA, 9>;
    using GPIOPinA10Device = GPIOPinDevice<GPIOPortID::kA, 10>;
    using GPIOPinA11Device = GPIOPinDevice<GPIOPortID::kA, 11>;
    using GPIOPinA12Device = GPIOPinDevice<GPIOPortID::kA, 12>;
    using GPIOPinA13Device = GPIOPinDevice<GPIOPortID::kA, 13>;
    using GPIOPinA14Device = GPIOPinDevice<GPIOPortID::kA, 14>;
    using GPIOPinA15Device = GPIOPinDevice<GPIOPortID::kA, 15>;

    using GPIOPinB0Device  = GPIOPinDevice<GPIOPortID::kB, 0>;
    using GPIOPinB1Device  = GPIOPinDevice<GPIOPortID::kB, 1>;
    using GPIOPinB2Device  = GPIOPinDevice<GPIOPortID::kB, 2>;
    using GPIOPinB3Device  = GPIOPinDevice<GPIOPortID::kB, 3>;
    using GPIOPinB4Device  = GPIOPinDevice<GPIOPortID::kB, 4>;
    using GPIOPinB5Device  = GPIOPinDevice<GPIOPortID::kB, 5>;
    using GPIOPinB6Device  = GPIOPinDevice<GPIOPortID::kB, 6>;
    using GPIOPinB7Device  = GPIOPinDevice<GPIOPortID::kB, 7>;
    using GPIOPinB8Device  = GPIOPinDevice<GPIOPortID::kB, 8>;
    using GPIOPinB9Device  = GPIOPinDevice<GPIOPortID::kB, 9>;
    using GPIOPinB10Device = GPIOPinDevice<GPIOPortID::kB, 10>;
    using GPIOPinB11Device = GPIOPinDevice<GPIOPortID::kB, 11>;
    using GPIOPinB12Device = GPIOPinDevice<GPIOPortID::kB, 12>;
    using GPIOPinB13Device = GPIOPinDevice<GPIOPortID::kB, 13>;
    using GPIOPinB14Device = GPIOPinDevice<GPIOPortID::kB, 14>;
    using GPIOPinB15Device = GPIOPinDevice<GPIOPortID::kB, 15>;

    using GPIOPinC0Device  = GPIOPinDevice<GPIOPortID::kC, 0>;
    using GPIOPinC1Device  = GPIOPinDevice<GPIOPortID::kC, 1>;
    using GPIOPinC2Device  = GPIOPinDevice<GPIOPortID::kC, 2>;
    using GPIOPinC3Device  = GPIOPinDevice<GPIOPortID::kC, 3>;
    using GPIOPinC4Device  = GPIOPinDevice<GPIOPortID::kC, 4>;
    using GPIOPinC5Device  = GPIOPinDevice<GPIOPortID::kC, 5>;
    using GPIOPinC6Device  = GPIOPinDevice<GPIOPortID::kC, 6>;
    using GPIOPinC7Device  = GPIOPinDevice<GPIOPortID::kC, 7>;
    using GPIOPinC8Device  = GPIOPinDevice<GPIOPortID::kC, 8>;
    using GPIOPinC9Device  = GPIOPinDevice<GPIOPortID::kC, 9>;
    using GPIOPinC10Device = GPIOPinDevice<GPIOPortID::kC, 10>;
    using GPIOPinC11Device = GPIOPinDevice<GPIOPortID::kC, 11>;
    using GPIOPinC12Device = GPIOPinDevice<GPIOPortID::kC, 12>;
    using GPIOPinC13Device = GPIOPinDevice<GPIOPortID::kC, 13>;
    using GPIOPinC14Device = GPIOPinDevice<GPIOPortID::kC, 14>;
    using GPIOPinC15Device = GPIOPinDevice<GPIOPortID::kC, 15>;

    using GPIOPinD0Device  = GPIOPinDevice<GPIOPortID::kD, 0>;
    using GPIOPinD1Device  = GPIOPinDevice<GPIOPortID::kD, 1>;
    using GPIOPinD2Device  = GPIOPinDevice<GPIOPortID::kD, 2>;
    using GPIOPinD3Device  = GPIOPinDevice<GPIOPortID::kD, 3>;
    using GPIOPinD4Device  = GPIOPinDevice<GPIOPortID::kD, 4>;
    using GPIOPinD5Device  = GPIOPinDevice<GPIOPortID::kD, 5>;
    using GPIOPinD6Device  = GPIOPinDevice<GPIOPortID::kD, 6>;
    using GPIOPinD7Device  = GPIOPinDevice<GPIOPortID::kD, 7>;
    using GPIOPinD8Device  = GPIOPinDevice<GPIOPortID::kD, 8>;
    using GPIOPinD9Device  = GPIOPinDevice<GPIOPortID::kD, 9>;
    using GPIOPinD10Device = GPIOPinDevice<GPIOPortID::kD, 10>;
    using GPIOPinD11Device = GPIOPinDevice<GPIOPortID::kD, 11>;
    using GPIOPinD12Device = GPIOPinDevice<GPIOPortID::kD, 12>;
    using GPIOPinD13Device = GPIOPinDevice<GPIOPortID::kD, 13>;
    using GPIOPinD14Device = GPIOPinDevice<GPIOPortID::kD, 14>;
    using GPIOPinD15Device = GPIOPinDevice<GPIOPortID::kD, 15>;

    using GPIOPinE0Device  = GPIOPinDevice<GPIOPortID::kE, 0>;
    using GPIOPinE1Device  = GPIOPinDevice<GPIOPortID::kE, 1>;
    using GPIOPinE2Device  = GPIOPinDevice<GPIOPortID::kE, 2>;
    using GPIOPinE3Device  = GPIOPinDevice<GPIOPortID::kE, 3>;
    using GPIOPinE4Device  = GPIOPinDevice<GPIOPortID::kE, 4>;
    using GPIOPinE5Device  = GPIOPinDevice<GPIOPortID::kE, 5>;
    using GPIOPinE6Device  = GPIOPinDevice<GPIOPortID::kE, 6>;
    using GPIOPinE7Device  = GPIOPinDevice<GPIOPortID::kE, 7>;
    using GPIOPinE8Device  = GPIOPinDevice<GPIOPortID::kE, 8>;
    using GPIOPinE9Device  = GPIOPinDevice<GPIOPortID::kE, 9>;
    using GPIOPinE10Device = GPIOPinDevice<GPIOPortID::kE, 10>;
    using GPIOPinE11Device = GPIOPinDevice<GPIOPortID::kE, 11>;
    using GPIOPinE12Device = GPIOPinDevice<GPIOPortID::kE, 12>;
    using GPIOPinE13Device = GPIOPinDevice<GPIOPortID::kE, 13>;
    using GPIOPinE14Device = GPIOPinDevice<GPIOPortID::kE, 14>;
    using GPIOPinE15Device = GPIOPinDevice<GPIOPortID::kE, 15>;

    using GPIOPinF0Device  = GPIOPinDevice<GPIOPortID::kF, 0>;
    using GPIOPinF1Device  = GPIOPinDevice<GPIOPortID::kF, 1>;
    using GPIOPinF2Device  = GPIOPinDevice<GPIOPortID::kF, 2>;
    using GPIOPinF3Device  = GPIOPinDevice<GPIOPortID::kF, 3>;
    using GPIOPinF4Device  = GPIOPinDevice<GPIOPortID::kF, 4>;
    using GPIOPinF5Device  = GPIOPinDevice<GPIOPortID::kF, 5>;
    using GPIOPinF6Device  = GPIOPinDevice<GPIOPortID::kF, 6>;
    using GPIOPinF7Device  = GPIOPinDevice<GPIOPortID::kF, 7>;
    using GPIOPinF8Device  = GPIOPinDevice<GPIOPortID::kF, 8>;
    using GPIOPinF9Device  = GPIOPinDevice<GPIOPortID::kF, 9>;
    using GPIOPinF10Device = GPIOPinDevice<GPIOPortID::kF, 10>;
    using GPIOPinF11Device = GPIOPinDevice<GPIOPortID::kF, 11>;
    using GPIOPinF12Device = GPIOPinDevice<GPIOPortID::kF, 12>;
    using GPIOPinF13Device = GPIOPinDevice<GPIOPortID::kF, 13>;
    using GPIOPinF14Device = GPIOPinDevice<GPIOPortID::kF, 14>;
    using GPIOPinF15Device = GPIOPinDevice<GPIOPortID::kF, 15>;

}  // namespace valle::platform