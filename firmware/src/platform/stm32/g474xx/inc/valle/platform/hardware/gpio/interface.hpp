#pragma once

#include "stm32g4xx_hal.h"
#include "stm32g4xx_hal_gpio.h"
#include "stm32g4xx_ll_exti.h"
#include "stm32g4xx_ll_system.h"
#include "valle/platform/hardware/gpio/id.hpp"

namespace valle::platform
{
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
        kLow      = GPIO_SPEED_FREQ_LOW,        // range up to 5 MHz
        kMedium   = GPIO_SPEED_FREQ_MEDIUM,     // range  5 MHz to 25 MHz
        kHigh     = GPIO_SPEED_FREQ_HIGH,       // range 25 MHz to 50 MHz
        kVeryHigh = GPIO_SPEED_FREQ_VERY_HIGH,  // range 50 MHz to 120 MHz
    };

    enum class GPIOOutputMode
    {
        kPushPull  = GPIO_MODE_OUTPUT_PP,
        kOpenDrain = GPIO_MODE_OUTPUT_OD,
    };

    enum class GPIOInputInterruptTrigger
    {
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

    // ============================================================================
    // HARDWARE TRAITS
    // ============================================================================

    // ---------------------------------------------------------------------------
    // GPIO PORT
    // ---------------------------------------------------------------------------
    template <GPIOPortID tkPortID>
    struct GPIOPortTraits;

    template <>
    struct GPIOPortTraits<GPIOPortID::kPortA>
    {
        static inline GPIO_TypeDef* const skInstance         = GPIOA;
        static constexpr uint32_t         skLLSyscfgEXTIPort = LL_SYSCFG_EXTI_PORTA;

        static void enable_clock()
        {
            __HAL_RCC_GPIOA_CLK_ENABLE();
        }
    };

    template <>
    struct GPIOPortTraits<GPIOPortID::kPortB>
    {
        static inline GPIO_TypeDef* const skInstance         = GPIOB;
        static constexpr uint32_t         skLLSyscfgEXTIPort = LL_SYSCFG_EXTI_PORTB;

        static void enable_clock()
        {
            __HAL_RCC_GPIOB_CLK_ENABLE();
        }
    };
    template <>
    struct GPIOPortTraits<GPIOPortID::kPortC>
    {
        static inline GPIO_TypeDef* const skInstance         = GPIOC;
        static constexpr uint32_t         skLLSyscfgEXTIPort = LL_SYSCFG_EXTI_PORTC;

        static void enable_clock()
        {
            __HAL_RCC_GPIOC_CLK_ENABLE();
        }
    };
    template <>
    struct GPIOPortTraits<GPIOPortID::kPortD>
    {
        static inline GPIO_TypeDef* const skInstance         = GPIOD;
        static constexpr uint32_t         skLLSyscfgEXTIPort = LL_SYSCFG_EXTI_PORTD;

        static void enable_clock()
        {
            __HAL_RCC_GPIOD_CLK_ENABLE();
        }
    };
    template <>
    struct GPIOPortTraits<GPIOPortID::kPortE>
    {
        static inline GPIO_TypeDef* const skInstance         = GPIOE;
        static constexpr uint32_t         skLLSyscfgEXTIPort = LL_SYSCFG_EXTI_PORTE;

        static void enable_clock()
        {
            __HAL_RCC_GPIOE_CLK_ENABLE();
        }
    };
    template <>
    struct GPIOPortTraits<GPIOPortID::kPortF>
    {
        static inline GPIO_TypeDef* const skInstance         = GPIOF;
        static constexpr uint32_t         skLLSyscfgEXTIPort = LL_SYSCFG_EXTI_PORTF;

        static void enable_clock()
        {
            __HAL_RCC_GPIOF_CLK_ENABLE();
        }
    };
    template <>
    struct GPIOPortTraits<GPIOPortID::kPortG>
    {
        static inline GPIO_TypeDef* const skInstance         = GPIOG;
        static constexpr uint32_t         skLLSyscfgEXTIPort = LL_SYSCFG_EXTI_PORTG;

        static void enable_clock()
        {
            __HAL_RCC_GPIOG_CLK_ENABLE();
        }
    };

    // ---------------------------------------------------------------------------
    // GPIO PIN
    // ---------------------------------------------------------------------------

    template <GPIOPortID tkPortID, GPIOPinID tkPinID>
    struct GPIOPinTraits
    {
    private:
        static constexpr uint8_t skPinIdx = static_cast<uint8_t>(tkPinID);

        static consteval IRQn_Type get_irq_n()
        {
            if constexpr (skPinIdx == 0)
            {
                return EXTI0_IRQn;
            }
            else if constexpr (skPinIdx == 1)
            {
                return EXTI1_IRQn;
            }
            else if constexpr (skPinIdx == 2)
            {
                return EXTI2_IRQn;
            }
            else if constexpr (skPinIdx == 3)
            {
                return EXTI3_IRQn;
            }
            else if constexpr (skPinIdx == 4)
            {
                return EXTI4_IRQn;
            }
            else if constexpr (skPinIdx >= 5 && skPinIdx <= 9)
            {
                return EXTI9_5_IRQn;
            }
            else if constexpr (skPinIdx >= 10 && skPinIdx <= 15)
            {
                return EXTI15_10_IRQn;
            }
        }

        static consteval uint32_t get_ll_exti_line()
        {
            if constexpr (skPinIdx == 0)
            {
                return LL_EXTI_LINE_0;
            }
            else if constexpr (skPinIdx == 1)
            {
                return LL_EXTI_LINE_1;
            }
            else if constexpr (skPinIdx == 2)
            {
                return LL_EXTI_LINE_2;
            }
            else if constexpr (skPinIdx == 3)
            {
                return LL_EXTI_LINE_3;
            }
            else if constexpr (skPinIdx == 4)
            {
                return LL_EXTI_LINE_4;
            }
            else if constexpr (skPinIdx == 5)
            {
                return LL_EXTI_LINE_5;
            }
            else if constexpr (skPinIdx == 6)
            {
                return LL_EXTI_LINE_6;
            }
            else if constexpr (skPinIdx == 7)
            {
                return LL_EXTI_LINE_7;
            }
            else if constexpr (skPinIdx == 8)
            {
                return LL_EXTI_LINE_8;
            }
            else if constexpr (skPinIdx == 9)
            {
                return LL_EXTI_LINE_9;
            }
            else if constexpr (skPinIdx == 10)
            {
                return LL_EXTI_LINE_10;
            }
            else if constexpr (skPinIdx == 11)
            {
                return LL_EXTI_LINE_11;
            }
            else if constexpr (skPinIdx == 12)
            {
                return LL_EXTI_LINE_12;
            }
            else if constexpr (skPinIdx == 13)
            {
                return LL_EXTI_LINE_13;
            }
            else if constexpr (skPinIdx == 14)
            {
                return LL_EXTI_LINE_14;
            }
            else if constexpr (skPinIdx == 15)
            {
                return LL_EXTI_LINE_15;
            }
        }

        static consteval uint32_t get_ll_syscfg_exti_line()
        {
            if constexpr (skPinIdx == 0)
            {
                return LL_SYSCFG_EXTI_LINE0;
            }
            else if constexpr (skPinIdx == 1)
            {
                return LL_SYSCFG_EXTI_LINE1;
            }
            else if constexpr (skPinIdx == 2)
            {
                return LL_SYSCFG_EXTI_LINE2;
            }
            else if constexpr (skPinIdx == 3)
            {
                return LL_SYSCFG_EXTI_LINE3;
            }
            else if constexpr (skPinIdx == 4)
            {
                return LL_SYSCFG_EXTI_LINE4;
            }
            else if constexpr (skPinIdx == 5)
            {
                return LL_SYSCFG_EXTI_LINE5;
            }
            else if constexpr (skPinIdx == 6)
            {
                return LL_SYSCFG_EXTI_LINE6;
            }
            else if constexpr (skPinIdx == 7)
            {
                return LL_SYSCFG_EXTI_LINE7;
            }
            else if constexpr (skPinIdx == 8)
            {
                return LL_SYSCFG_EXTI_LINE8;
            }
            else if constexpr (skPinIdx == 9)
            {
                return LL_SYSCFG_EXTI_LINE9;
            }
            else if constexpr (skPinIdx == 10)
            {
                return LL_SYSCFG_EXTI_LINE10;
            }
            else if constexpr (skPinIdx == 11)
            {
                return LL_SYSCFG_EXTI_LINE11;
            }
            else if constexpr (skPinIdx == 12)
            {
                return LL_SYSCFG_EXTI_LINE12;
            }
            else if constexpr (skPinIdx == 13)
            {
                return LL_SYSCFG_EXTI_LINE13;
            }
            else if constexpr (skPinIdx == 14)
            {
                return LL_SYSCFG_EXTI_LINE14;
            }
            else if constexpr (skPinIdx == 15)
            {
                return LL_SYSCFG_EXTI_LINE15;
            }
        }

    public:
        static constexpr uint16_t  skPinMask          = (1UL << skPinIdx);  // NOLINT(hicpp-signed-bitwise)
        static constexpr IRQn_Type skIRQn             = get_irq_n();
        static constexpr uint32_t  skLLEXTILine       = get_ll_exti_line();
        static constexpr uint32_t  skLLSyscfgEXTILine = get_ll_syscfg_exti_line();
    };

}  // namespace valle::platform