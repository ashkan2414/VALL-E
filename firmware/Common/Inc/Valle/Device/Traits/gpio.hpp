#pragma once

#include "stm32g4xx_hal.h"
#include "stm32g4xx_hal_gpio.h"

// =============================================================================
// CONSTANTS & CHECKS
// =============================================================================

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

template <GPIOPort tkPort, uint8_t tkPinIdx>
constexpr bool kValidGPIOPin = (tkPinIdx < 16);

// ============================================================================
// HARDWARE TRAITS
// ============================================================================
template <GPIOPort tkPort>
struct GPIOPortTraits;

template <>
struct GPIOPortTraits<GPIOPort::kA>
{
    static inline GPIO_TypeDef* skInstance = GPIOA;
    static inline void          enable_clock()
    {
        __HAL_RCC_GPIOA_CLK_ENABLE();
    }
};
template <>
struct GPIOPortTraits<GPIOPort::kB>
{
    static inline GPIO_TypeDef* skInstance = GPIOB;
    static inline void          enable_clock()
    {
        __HAL_RCC_GPIOB_CLK_ENABLE();
    }
};
template <>
struct GPIOPortTraits<GPIOPort::kC>
{
    static inline GPIO_TypeDef* skInstance = GPIOC;
    static inline void          enable_clock()
    {
        __HAL_RCC_GPIOC_CLK_ENABLE();
    }
};
template <>
struct GPIOPortTraits<GPIOPort::kD>
{
    static inline GPIO_TypeDef* skInstance = GPIOD;
    static inline void          enable_clock()
    {
        __HAL_RCC_GPIOD_CLK_ENABLE();
    }
};
template <>
struct GPIOPortTraits<GPIOPort::kE>
{
    static inline GPIO_TypeDef* skInstance = GPIOE;
    static inline void          enable_clock()
    {
        __HAL_RCC_GPIOE_CLK_ENABLE();
    }
};
template <>
struct GPIOPortTraits<GPIOPort::kF>
{
    static inline GPIO_TypeDef* skInstance = GPIOF;
    static inline void          enable_clock()
    {
        __HAL_RCC_GPIOF_CLK_ENABLE();
    }
};
template <>
struct GPIOPortTraits<GPIOPort::kG>
{
    static inline GPIO_TypeDef* skInstance = GPIOG;
    static inline void          enable_clock()
    {
        __HAL_RCC_GPIOG_CLK_ENABLE();
    }
};

// =============================================================================
// ENUMERATIONS
// =============================================================================

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
