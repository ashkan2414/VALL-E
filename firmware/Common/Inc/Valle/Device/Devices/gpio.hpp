#pragma once

#include "Valle/Device/device_core.hpp"
#include "stm32g4xx_hal.h"
#include "stm32g4xx_hal_gpio.h"

enum class GPIOPort
{
    kA,
    kB,
    kC,
    kD,
    kE,
    kF,
    kG,
};

enum class GPIOPullMode
{
    kNoPull   = GPIO_NOPULL,
    kPullUp   = GPIO_PULLUP,
    kPullDown = GPIO_PULLDOWN,
};

enum class GPIOSpeedMode
{
    kLow      = GPIO_SPEED_FREQ_LOW,
    kMedium   = GPIO_SPEED_FREQ_MEDIUM,
    kHigh     = GPIO_SPEED_FREQ_HIGH,
    kVeryHigh = GPIO_SPEED_FREQ_VERY_HIGH,
};

enum class GPIOOutputMode
{
    kPushPull  = GPIO_MODE_OUTPUT_PP,
    kOpenDrain = GPIO_MODE_OUTPUT_OD,
};

enum class GPIOInputInterruptTrigger
{
    kNone           = 0,
    kIntRisingEdge  = TRIGGER_RISING,
    kIntFallingEdge = TRIGGER_FALLING,
    kIntBothEdges   = TRIGGER_RISING | TRIGGER_FALLING,
};

enum class GPIOInputInterruptAction
{
    kInterrupt = EXTI_IT,
    kEvent     = EXTI_EVT,
};

enum class GPIOAlternateFunctionMode
{
    kPushPull  = GPIO_MODE_AF_PP,
    kOpenDrain = GPIO_MODE_AF_OD,
};

enum class GPIOAlternativeFunction
{
    kAF0  = 0,
    kAF1  = 1,
    kAF2  = 2,
    kAF3  = 3,
    kAF4  = 4,
    kAF5  = 5,
    kAF6  = 6,
    kAF7  = 7,
    kAF8  = 8,
    kAF9  = 9,
    kAF10 = 10,
    kAF11 = 11,
    kAF12 = 12,
    kAF13 = 13,
    kAF14 = 14,
    kAF15 = 15,
};

template <GPIOPort tkPort, uint8_t tkPinIdx>
constexpr bool kValidGPIOPin = (tkPinIdx < 16);

// ============================================================================
// HAL TRAITS (Map 'A' -> GPIOA + Clock Enable)
// ============================================================================
template <GPIOPort tkPort>
struct GPIOPortTraits;

template <>
struct GPIOPortTraits<GPIOPort::kA>
{
    static inline const GPIO_TypeDef* skInstance = GPIOA;
    static inline void                enable_clock()
    {
        __HAL_RCC_GPIOA_CLK_ENABLE();
    }
};
template <>
struct GPIOPortTraits<GPIOPort::kB>
{
    static inline const GPIO_TypeDef* skInstance = GPIOB;
    static inline void                enable_clock()
    {
        __HAL_RCC_GPIOB_CLK_ENABLE();
    }
};
template <>
struct GPIOPortTraits<GPIOPort::kC>
{
    static inline const GPIO_TypeDef* skInstance = GPIOC;
    static inline void                enable_clock()
    {
        __HAL_RCC_GPIOC_CLK_ENABLE();
    }
};
template <>
struct GPIOPortTraits<GPIOPort::kD>
{
    static inline const GPIO_TypeDef* skInstance = GPIOD;
    static inline void                enable_clock()
    {
        __HAL_RCC_GPIOD_CLK_ENABLE();
    }
};
template <>
struct GPIOPortTraits<GPIOPort::kE>
{
    static inline const GPIO_TypeDef* skInstance = GPIOE;
    static inline void                enable_clock()
    {
        __HAL_RCC_GPIOE_CLK_ENABLE();
    }
};
template <>
struct GPIOPortTraits<GPIOPort::kF>
{
    static inline const GPIO_TypeDef* skInstance = GPIOF;
    static inline void                enable_clock()
    {
        __HAL_RCC_GPIOF_CLK_ENABLE();
    }
};
template <>
struct GPIOPortTraits<GPIOPort::kG>
{
    static inline const GPIO_TypeDef* skInstance = GPIOG;
    static inline void                enable_clock()
    {
        __HAL_RCC_GPIOG_CLK_ENABLE();
    }
};

// =============================================================================
// Forward Declarations
// =============================================================================

class GPIODevice;

template <GPIOPort tkPort>
class GPIOPortDevice;

template <GPIOPort tkPort, uint8_t tkPinIdx>
    requires(kValidGPIOPin<tkPort, tkPinIdx>)
class GPIOPinDevice;

// ============================================================================
// THE GPIO CONTROLLER (interface device)
// ============================================================================
/**
 * @brief Represents the GPIO Controller (GPIOA, GPIOB...).
 *
 */
class GPIODevice
{
public:
    struct Descriptor : public InterfaceDeviceDescriptor
    {
        using Children = DeviceList<GPIOPortDevice<GPIOPort::kA>,
                                    GPIOPortDevice<GPIOPort::kB>,
                                    GPIOPortDevice<GPIOPort::kC>,
                                    GPIOPortDevice<GPIOPort::kD>,
                                    GPIOPortDevice<GPIOPort::kE>,
                                    GPIOPortDevice<GPIOPort::kF>,
                                    GPIOPortDevice<GPIOPort::kG>>;
    };
};

// ============================================================================
// THE PORT (Shared Device)
// ============================================================================
/**
 * @brief Represents the GPIO Port Hardware (GPIOA, GPIOB...).
 * Responsibility: Managing the AHB Clock.
 */
template <GPIOPort tkPort>
class GPIOPortDevice
{
public:
    struct Descriptor : public SharedDeviceDescriptor
    {
        using Children = DeviceList<GPIOPinDevice<tkPort, 0>,
                                    GPIOPinDevice<tkPort, 1>,
                                    GPIOPinDevice<tkPort, 2>,
                                    GPIOPinDevice<tkPort, 3>,
                                    GPIOPinDevice<tkPort, 4>,
                                    GPIOPinDevice<tkPort, 5>,
                                    GPIOPinDevice<tkPort, 6>,
                                    GPIOPinDevice<tkPort, 7>,
                                    GPIOPinDevice<tkPort, 8>,
                                    GPIOPinDevice<tkPort, 9>,
                                    GPIOPinDevice<tkPort, 10>,
                                    GPIOPinDevice<tkPort, 11>,
                                    GPIOPinDevice<tkPort, 12>,
                                    GPIOPinDevice<tkPort, 13>,
                                    GPIOPinDevice<tkPort, 14>,
                                    GPIOPinDevice<tkPort, 15>>;
    };
    using DependDevices = TypeList<GPIODevice>;
    using Traits        = GPIOPortTraits<tkPort>;

    void init()
    {
        Traits::enable_clock();
    }

    void post_init()
    {
        // Nothing for now
    }
};

// ============================================================================
// THE PIN (Unique Device)
// ============================================================================
struct GPIOPinConfig
{
    uint32_t                mode;
    GPIOPullMode            pull;
    GPIOSpeedMode           speed;
    GPIOAlternativeFunction alternate;

    [[nodiscard]] static inline constexpr GPIOPinConfig default_config()
    {
        return GPIOPinConfig{.mode      = GPIO_MODE_INPUT,
                             .pull      = GPIOPullMode::kNoPull,
                             .speed     = GPIOSpeedMode::kLow,
                             .alternate = GPIOAlternativeFunction::kAF0};
    }
};

/**
 * @brief Represents the Physical Pin.
 * Pure resource container. No functional API.
 */
template <GPIOPort tkPort, uint8_t tkPinIdx>
    requires(kValidGPIOPin<tkPort, tkPinIdx>)
class GPIOPinDevice
{
public:
    struct Descriptor : public UniqueDeviceDescriptor
    {
    };

    using DependDevices = TypeList<GPIOPortDevice<tkPort>>;
    using Traits        = GPIOPortTraits<tkPort>;

    // Hardware Constants exposed for Drivers
    static constexpr GPIO_TypeDef* skPort    = Traits::skInstance;
    static constexpr uint16_t      skPinMask = (1UL << tkPinIdx);

public:
    static inline void init(const GPIOPinConfig& config = GPIOPinConfig::default_config())
    {
        GPIO_InitTypeDef init = {0};
        init.Pin              = skPinMask;
        init.Mode             = static_cast<uint32_t>(config.mode);
        init.Pull             = static_cast<uint32_t>(config.pull);
        init.Speed            = static_cast<uint32_t>(config.speed);
        init.Alternate        = static_cast<uint32_t>(config.alternate);
        HAL_GPIO_Init(skPort, &init);
    }

    static inline void write(bool state)
    {
        HAL_GPIO_WritePin(skPort, skPinMask, state ? GPIO_PIN_SET : GPIO_PIN_RESET);
    }

    static inline void toggle()
    {
        HAL_GPIO_TogglePin(skPort, skPinMask);
    }

    [[nodiscard]] static inline bool read()
    {
        return (HAL_GPIO_ReadPin(skPort, skPinMask) == GPIO_PIN_SET);
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
concept CNullPin = std::is_same_v<T, GPIONullPinDevice>;

// ============================================================================
// GPIO Pin Aliases
// ============================================================================

using GPIOPinA0  = GPIOPinDevice<GPIOPort::kA, 0>;
using GPIOPinA1  = GPIOPinDevice<GPIOPort::kA, 1>;
using GPIOPinA2  = GPIOPinDevice<GPIOPort::kA, 2>;
using GPIOPinA3  = GPIOPinDevice<GPIOPort::kA, 3>;
using GPIOPinA4  = GPIOPinDevice<GPIOPort::kA, 4>;
using GPIOPinA5  = GPIOPinDevice<GPIOPort::kA, 5>;
using GPIOPinA6  = GPIOPinDevice<GPIOPort::kA, 6>;
using GPIOPinA7  = GPIOPinDevice<GPIOPort::kA, 7>;
using GPIOPinA8  = GPIOPinDevice<GPIOPort::kA, 8>;
using GPIOPinA9  = GPIOPinDevice<GPIOPort::kA, 9>;
using GPIOPinA10 = GPIOPinDevice<GPIOPort::kA, 10>;
using GPIOPinA11 = GPIOPinDevice<GPIOPort::kA, 11>;
using GPIOPinA12 = GPIOPinDevice<GPIOPort::kA, 12>;
using GPIOPinA13 = GPIOPinDevice<GPIOPort::kA, 13>;
using GPIOPinA14 = GPIOPinDevice<GPIOPort::kA, 14>;
using GPIOPinA15 = GPIOPinDevice<GPIOPort::kA, 15>;

using GPIOPinB0  = GPIOPinDevice<GPIOPort::kB, 0>;
using GPIOPinB1  = GPIOPinDevice<GPIOPort::kB, 1>;
using GPIOPinB2  = GPIOPinDevice<GPIOPort::kB, 2>;
using GPIOPinB3  = GPIOPinDevice<GPIOPort::kB, 3>;
using GPIOPinB4  = GPIOPinDevice<GPIOPort::kB, 4>;
using GPIOPinB5  = GPIOPinDevice<GPIOPort::kB, 5>;
using GPIOPinB6  = GPIOPinDevice<GPIOPort::kB, 6>;
using GPIOPinB7  = GPIOPinDevice<GPIOPort::kB, 7>;
using GPIOPinB8  = GPIOPinDevice<GPIOPort::kB, 8>;
using GPIOPinB9  = GPIOPinDevice<GPIOPort::kB, 9>;
using GPIOPinB10 = GPIOPinDevice<GPIOPort::kB, 10>;
using GPIOPinB11 = GPIOPinDevice<GPIOPort::kB, 11>;
using GPIOPinB12 = GPIOPinDevice<GPIOPort::kB, 12>;
using GPIOPinB13 = GPIOPinDevice<GPIOPort::kB, 13>;
using GPIOPinB14 = GPIOPinDevice<GPIOPort::kB, 14>;
using GPIOPinB15 = GPIOPinDevice<GPIOPort::kB, 15>;

using GPIOPinC0  = GPIOPinDevice<GPIOPort::kC, 0>;
using GPIOPinC1  = GPIOPinDevice<GPIOPort::kC, 1>;
using GPIOPinC2  = GPIOPinDevice<GPIOPort::kC, 2>;
using GPIOPinC3  = GPIOPinDevice<GPIOPort::kC, 3>;
using GPIOPinC4  = GPIOPinDevice<GPIOPort::kC, 4>;
using GPIOPinC5  = GPIOPinDevice<GPIOPort::kC, 5>;
using GPIOPinC6  = GPIOPinDevice<GPIOPort::kC, 6>;
using GPIOPinC7  = GPIOPinDevice<GPIOPort::kC, 7>;
using GPIOPinC8  = GPIOPinDevice<GPIOPort::kC, 8>;
using GPIOPinC9  = GPIOPinDevice<GPIOPort::kC, 9>;
using GPIOPinC10 = GPIOPinDevice<GPIOPort::kC, 10>;
using GPIOPinC11 = GPIOPinDevice<GPIOPort::kC, 11>;
using GPIOPinC12 = GPIOPinDevice<GPIOPort::kC, 12>;
using GPIOPinC13 = GPIOPinDevice<GPIOPort::kC, 13>;
using GPIOPinC14 = GPIOPinDevice<GPIOPort::kC, 14>;
using GPIOPinC15 = GPIOPinDevice<GPIOPort::kC, 15>;

using GPIOPinD0  = GPIOPinDevice<GPIOPort::kD, 0>;
using GPIOPinD1  = GPIOPinDevice<GPIOPort::kD, 1>;
using GPIOPinD2  = GPIOPinDevice<GPIOPort::kD, 2>;
using GPIOPinD3  = GPIOPinDevice<GPIOPort::kD, 3>;
using GPIOPinD4  = GPIOPinDevice<GPIOPort::kD, 4>;
using GPIOPinD5  = GPIOPinDevice<GPIOPort::kD, 5>;
using GPIOPinD6  = GPIOPinDevice<GPIOPort::kD, 6>;
using GPIOPinD7  = GPIOPinDevice<GPIOPort::kD, 7>;
using GPIOPinD8  = GPIOPinDevice<GPIOPort::kD, 8>;
using GPIOPinD9  = GPIOPinDevice<GPIOPort::kD, 9>;
using GPIOPinD10 = GPIOPinDevice<GPIOPort::kD, 10>;
using GPIOPinD11 = GPIOPinDevice<GPIOPort::kD, 11>;
using GPIOPinD12 = GPIOPinDevice<GPIOPort::kD, 12>;
using GPIOPinD13 = GPIOPinDevice<GPIOPort::kD, 13>;
using GPIOPinD14 = GPIOPinDevice<GPIOPort::kD, 14>;
using GPIOPinD15 = GPIOPinDevice<GPIOPort::kD, 15>;

using GPIOPinE0  = GPIOPinDevice<GPIOPort::kE, 0>;
using GPIOPinE1  = GPIOPinDevice<GPIOPort::kE, 1>;
using GPIOPinE2  = GPIOPinDevice<GPIOPort::kE, 2>;
using GPIOPinE3  = GPIOPinDevice<GPIOPort::kE, 3>;
using GPIOPinE4  = GPIOPinDevice<GPIOPort::kE, 4>;
using GPIOPinE5  = GPIOPinDevice<GPIOPort::kE, 5>;
using GPIOPinE6  = GPIOPinDevice<GPIOPort::kE, 6>;
using GPIOPinE7  = GPIOPinDevice<GPIOPort::kE, 7>;
using GPIOPinE8  = GPIOPinDevice<GPIOPort::kE, 8>;
using GPIOPinE9  = GPIOPinDevice<GPIOPort::kE, 9>;
using GPIOPinE10 = GPIOPinDevice<GPIOPort::kE, 10>;
using GPIOPinE11 = GPIOPinDevice<GPIOPort::kE, 11>;
using GPIOPinE12 = GPIOPinDevice<GPIOPort::kE, 12>;
using GPIOPinE13 = GPIOPinDevice<GPIOPort::kE, 13>;
using GPIOPinE14 = GPIOPinDevice<GPIOPort::kE, 14>;
using GPIOPinE15 = GPIOPinDevice<GPIOPort::kE, 15>;

using GPIOPinF0  = GPIOPinDevice<GPIOPort::kF, 0>;
using GPIOPinF1  = GPIOPinDevice<GPIOPort::kF, 1>;
using GPIOPinF2  = GPIOPinDevice<GPIOPort::kF, 2>;
using GPIOPinF3  = GPIOPinDevice<GPIOPort::kF, 3>;
using GPIOPinF4  = GPIOPinDevice<GPIOPort::kF, 4>;
using GPIOPinF5  = GPIOPinDevice<GPIOPort::kF, 5>;
using GPIOPinF6  = GPIOPinDevice<GPIOPort::kF, 6>;
using GPIOPinF7  = GPIOPinDevice<GPIOPort::kF, 7>;
using GPIOPinF8  = GPIOPinDevice<GPIOPort::kF, 8>;
using GPIOPinF9  = GPIOPinDevice<GPIOPort::kF, 9>;
using GPIOPinF10 = GPIOPinDevice<GPIOPort::kF, 10>;
using GPIOPinF11 = GPIOPinDevice<GPIOPort::kF, 11>;
using GPIOPinF12 = GPIOPinDevice<GPIOPort::kF, 12>;
using GPIOPinF13 = GPIOPinDevice<GPIOPort::kF, 13>;
using GPIOPinF14 = GPIOPinDevice<GPIOPort::kF, 14>;
using GPIOPinF15 = GPIOPinDevice<GPIOPort::kF, 15>;
