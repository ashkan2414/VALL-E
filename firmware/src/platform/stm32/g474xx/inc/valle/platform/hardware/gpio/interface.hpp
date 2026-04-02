#pragma once

#include "stm32g4xx_ll_gpio.h"
#include "stm32g4xx_ll_system.h"
#include "valle/platform/hardware/exti.hpp"
#include "valle/platform/hardware/gpio/id.hpp"

namespace valle::platform
{
    // =============================================================================
    // ENUMERATIONS
    // =============================================================================

    enum class GpioPinMode
    {
        kInput             = LL_GPIO_MODE_INPUT,
        kOutput            = LL_GPIO_MODE_OUTPUT,
        kAlternateFunction = LL_GPIO_MODE_ALTERNATE,
        kAnalog            = LL_GPIO_MODE_ANALOG,
    };

    enum class GpioPinPullMode
    {
        kNoPull   = LL_GPIO_PULL_NO,
        kPullUp   = LL_GPIO_PULL_UP,
        kPullDown = LL_GPIO_PULL_DOWN,
    };

    enum class GpioPinSpeedMode
    {
        kLow      = LL_GPIO_SPEED_FREQ_LOW,        // range up to 5 MHz
        kMedium   = LL_GPIO_SPEED_FREQ_MEDIUM,     // range  5 MHz to 25 MHz
        kHigh     = LL_GPIO_SPEED_FREQ_HIGH,       // range 25 MHz to 50 MHz
        kVeryHigh = LL_GPIO_SPEED_FREQ_VERY_HIGH,  // range 50 MHz to 120 MHz
    };

    enum class GpioPinOutputMode
    {
        kPushPull  = LL_GPIO_OUTPUT_PUSHPULL,
        kOpenDrain = LL_GPIO_OUTPUT_OPENDRAIN,
    };

    enum class GpioAlternativeFunction
    {
        kAF0  = LL_GPIO_AF_0,
        kAF1  = LL_GPIO_AF_1,
        kAF2  = LL_GPIO_AF_2,
        kAF3  = LL_GPIO_AF_3,
        kAF4  = LL_GPIO_AF_4,
        kAF5  = LL_GPIO_AF_5,
        kAF6  = LL_GPIO_AF_6,
        kAF7  = LL_GPIO_AF_7,
        kAF8  = LL_GPIO_AF_8,
        kAF9  = LL_GPIO_AF_9,
        kAF10 = LL_GPIO_AF_10,
        kAF11 = LL_GPIO_AF_11,
        kAF12 = LL_GPIO_AF_12,
        kAF13 = LL_GPIO_AF_13,
        kAF14 = LL_GPIO_AF_14,
        kAF15 = LL_GPIO_AF_15,
    };

    // ============================================================================
    // HARDWARE TRAITS
    // ============================================================================

    // ---------------------------------------------------------------------------
    // GPIO PORT
    // ---------------------------------------------------------------------------
    template <GpioPortId tkPortId>
    struct GpioPortTraits;

    template <>
    struct GpioPortTraits<GpioPortId::kPortA>
    {
        static inline GPIO_TypeDef* const skInstance       = GPIOA;
        static constexpr ExtiLineSource   skExtiLineSource = ExtiLineSource::kPortA;
    };

    template <>
    struct GpioPortTraits<GpioPortId::kPortB>
    {
        static inline GPIO_TypeDef* const skInstance       = GPIOB;
        static constexpr ExtiLineSource   skExtiLineSource = ExtiLineSource::kPortB;
    };
    template <>
    struct GpioPortTraits<GpioPortId::kPortC>
    {
        static inline GPIO_TypeDef* const skInstance       = GPIOC;
        static constexpr ExtiLineSource   skExtiLineSource = ExtiLineSource::kPortC;
    };
    template <>
    struct GpioPortTraits<GpioPortId::kPortD>
    {
        static inline GPIO_TypeDef* const skInstance       = GPIOD;
        static constexpr ExtiLineSource   skExtiLineSource = ExtiLineSource::kPortD;
    };
    template <>
    struct GpioPortTraits<GpioPortId::kPortE>
    {
        static inline GPIO_TypeDef* const skInstance       = GPIOE;
        static constexpr ExtiLineSource   skExtiLineSource = ExtiLineSource::kPortE;
    };
    template <>
    struct GpioPortTraits<GpioPortId::kPortF>
    {
        static inline GPIO_TypeDef* const skInstance       = GPIOF;
        static constexpr ExtiLineSource   skExtiLineSource = ExtiLineSource::kPortF;
    };
    template <>
    struct GpioPortTraits<GpioPortId::kPortG>
    {
        static inline GPIO_TypeDef* const skInstance       = GPIOG;
        static constexpr ExtiLineSource   skExtiLineSource = ExtiLineSource::kPortG;
    };

    // ---------------------------------------------------------------------------
    // GPIO PIN
    // ---------------------------------------------------------------------------

    template <GpioPortId tkPortId, GpioPinId tkPinId>
    struct GpioPinTraits
    {
        static constexpr uint32_t skLLIdTable[] = {LL_GPIO_PIN_0,
                                                   LL_GPIO_PIN_1,
                                                   LL_GPIO_PIN_2,
                                                   LL_GPIO_PIN_3,
                                                   LL_GPIO_PIN_4,
                                                   LL_GPIO_PIN_5,
                                                   LL_GPIO_PIN_6,
                                                   LL_GPIO_PIN_7,
                                                   LL_GPIO_PIN_8,
                                                   LL_GPIO_PIN_9,
                                                   LL_GPIO_PIN_10,
                                                   LL_GPIO_PIN_11,
                                                   LL_GPIO_PIN_12,
                                                   LL_GPIO_PIN_13,
                                                   LL_GPIO_PIN_14,
                                                   LL_GPIO_PIN_15};

        static constexpr ExtiLineId skExtiLineTable[] = {ExtiLineId::kLine0,
                                                         ExtiLineId::kLine1,
                                                         ExtiLineId::kLine2,
                                                         ExtiLineId::kLine3,
                                                         ExtiLineId::kLine4,
                                                         ExtiLineId::kLine5,
                                                         ExtiLineId::kLine6,
                                                         ExtiLineId::kLine7,
                                                         ExtiLineId::kLine8,
                                                         ExtiLineId::kLine9,
                                                         ExtiLineId::kLine10,
                                                         ExtiLineId::kLine11,
                                                         ExtiLineId::kLine12,
                                                         ExtiLineId::kLine13,
                                                         ExtiLineId::kLine14,
                                                         ExtiLineId::kLine15};

        static constexpr uint8_t        skPinIdx         = static_cast<uint8_t>(tkPinId);
        static constexpr uint16_t       skPinMask        = (1UL << skPinIdx);  // NOLINT(hicpp-signed-bitwise)
        static constexpr uint32_t       skPinLLId        = skLLIdTable[skPinIdx];
        static constexpr ExtiLineId     skExtiLine       = skExtiLineTable[skPinIdx];
        static constexpr ExtiLineSource skExtiLineSource = GpioPortTraits<tkPortId>::skExtiLineSource;
    };

    // ============================================================================
    // INTERFACE
    // ============================================================================

    template <GpioPortId tkPortId>
    struct GpioPortInterface
    {
        using PortTraitsT = GpioPortTraits<tkPortId>;

        void enable_clock() const
        {
            if constexpr (tkPortId == GpioPortId::kPortA)
            {
                LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA);
            }
            else if constexpr (tkPortId == GpioPortId::kPortB)
            {
                LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOB);
            }
            else if constexpr (tkPortId == GpioPortId::kPortC)
            {
                LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOC);
            }
            else if constexpr (tkPortId == GpioPortId::kPortD)
            {
                LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOD);
            }
            else if constexpr (tkPortId == GpioPortId::kPortE)
            {
                LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOE);
            }
            else if constexpr (tkPortId == GpioPortId::kPortF)
            {
                LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOF);
            }
            else if constexpr (tkPortId == GpioPortId::kPortG)
            {
                LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOG);
            }
            else
            {
                static_assert(kAlwaysFalseV<GpioPortId>, "Invalid GpioPortId");
            }
        }

        void disable_clock() const
        {
            if constexpr (tkPortId == GpioPortId::kPortA)
            {
                LL_AHB2_GRP1_DisableClock(LL_AHB2_GRP1_PERIPH_GPIOA);
            }
            else if constexpr (tkPortId == GpioPortId::kPortB)
            {
                LL_AHB2_GRP1_DisableClock(LL_AHB2_GRP1_PERIPH_GPIOB);
            }
            else if constexpr (tkPortId == GpioPortId::kPortC)
            {
                LL_AHB2_GRP1_DisableClock(LL_AHB2_GRP1_PERIPH_GPIOC);
            }
            else if constexpr (tkPortId == GpioPortId::kPortD)
            {
                LL_AHB2_GRP1_DisableClock(LL_AHB2_GRP1_PERIPH_GPIOD);
            }
            else if constexpr (tkPortId == GpioPortId::kPortE)
            {
                LL_AHB2_GRP1_DisableClock(LL_AHB2_GRP1_PERIPH_GPIOE);
            }
            else if constexpr (tkPortId == GpioPortId::kPortF)
            {
                LL_AHB2_GRP1_DisableClock(LL_AHB2_GRP1_PERIPH_GPIOF);
            }
            else if constexpr (tkPortId == GpioPortId::kPortG)
            {
                LL_AHB2_GRP1_DisableClock(LL_AHB2_GRP1_PERIPH_GPIOG);
            }
            else
            {
                static_assert(kAlwaysFalseV<GpioPortId>, "Invalid GpioPortId");
            }
        }

        void lock_pin(const uint32_t pin_mask) const
        {
            LL_GPIO_LockPin(PortTraitsT::skInstance, pin_mask);
        }

        [[nodiscard]] bool is_pin_locked(const uint32_t pin_mask) const
        {
            return static_cast<bool>(LL_GPIO_IsPinLocked(PortTraitsT::skInstance, pin_mask) != 0U);
        }

        [[nodiscard]] bool is_any_pin_locked() const
        {
            return static_cast<bool>(LL_GPIO_IsAnyPinLocked(PortTraitsT::skInstance) != 0U);
        }

        [[nodiscard]] uint32_t read_input_port() const
        {
            return static_cast<uint32_t>(LL_GPIO_ReadInputPort(PortTraitsT::skInstance));
        }

        [[nodiscard]] bool is_input_pin_set(const uint32_t pin_mask) const
        {
            return static_cast<bool>(LL_GPIO_IsInputPinSet(PortTraitsT::skInstance, pin_mask) != 0U);
        }

        void write_output_port(const uint32_t port_value) const
        {
            LL_GPIO_WriteOutputPort(PortTraitsT::skInstance, port_value);
        }

        [[nodiscard]] uint32_t read_output_port() const
        {
            return static_cast<uint32_t>(LL_GPIO_ReadOutputPort(PortTraitsT::skInstance));
        }

        [[nodiscard]] bool is_output_pin_set(const uint32_t pin_mask) const
        {
            return static_cast<bool>(LL_GPIO_IsOutputPinSet(PortTraitsT::skInstance, pin_mask) != 0U);
        }

        void set_output_pin(const uint32_t pin_mask) const
        {
            LL_GPIO_SetOutputPin(PortTraitsT::skInstance, pin_mask);
        }

        void reset_output_pin(const uint32_t pin_mask) const
        {
            LL_GPIO_ResetOutputPin(PortTraitsT::skInstance, pin_mask);
        }

        void toggle_pin(const uint32_t pin_mask) const
        {
            LL_GPIO_TogglePin(PortTraitsT::skInstance, pin_mask);
        }
    };

    template <GpioPortId tkPortId, GpioPinId tkPinId>
    struct GpioPinInterface
    {
        using PortTraitsT = GpioPortTraits<tkPortId>;
        using PinTraitsT  = GpioPinTraits<tkPortId, tkPinId>;

        void set_mode(const GpioPinMode mode) const
        {
            LL_GPIO_SetPinMode(PortTraitsT::skInstance, PinTraitsT::skPinLLId, static_cast<uint32_t>(mode));
        }

        [[nodiscard]] GpioPinMode get_mode() const
        {
            return static_cast<GpioPinMode>(LL_GPIO_GetPinMode(PortTraitsT::skInstance, PinTraitsT::skPinLLId));
        }

        void set_output_mode(const GpioPinOutputMode output_mode) const
        {
            LL_GPIO_SetPinOutputType(
                PortTraitsT::skInstance, PinTraitsT::skPinLLIdMask, static_cast<uint32_t>(output_mode));
        }

        [[nodiscard]] GpioPinOutputMode get_output_mode() const
        {
            return static_cast<GpioPinOutputMode>(
                LL_GPIO_GetPinOutputType(PortTraitsT::skInstance, PinTraitsT::skPinLLId));
        }

        void set_speed(const GpioPinSpeedMode speed) const
        {
            LL_GPIO_SetPinSpeed(PortTraitsT::skInstance, PinTraitsT::skPinLLId, static_cast<uint32_t>(speed));
        }

        [[nodiscard]] GpioPinSpeedMode get_speed() const
        {
            return static_cast<GpioPinSpeedMode>(LL_GPIO_GetPinSpeed(PortTraitsT::skInstance, PinTraitsT::skPinLLId));
        }

        void set_pull(const GpioPinPullMode pull) const
        {
            LL_GPIO_SetPinPull(PortTraitsT::skInstance, PinTraitsT::skPinLLId, static_cast<uint32_t>(pull));
        }

        [[nodiscard]] GpioPinPullMode get_pull() const
        {
            return static_cast<GpioPinPullMode>(LL_GPIO_GetPinPull(PortTraitsT::skInstance, PinTraitsT::skPinLLId));
        }

        void set_alternate_function(const GpioAlternativeFunction alternate) const
        {
            if constexpr (PinTraitsT::skPinIdx >= 0 && PinTraitsT::skPinIdx <= 7)
            {
                LL_GPIO_SetAFPin_0_7(PortTraitsT::skInstance, PinTraitsT::skPinLLId, static_cast<uint32_t>(alternate));
            }
            else if constexpr (PinTraitsT::skPinIdx >= 8 && PinTraitsT::skPinIdx <= 15)
            {
                LL_GPIO_SetAFPin_8_15(PortTraitsT::skInstance, PinTraitsT::skPinLLId, static_cast<uint32_t>(alternate));
            }
            else
            {
                static_assert(kAlwaysFalseV<PinTraitsT::skPinIdx>, "Invalid Pin Id");
            }
        }

        [[nodiscard]] GpioAlternativeFunction get_alternate_function() const
        {
            if constexpr (PinTraitsT::skPinIdx >= 0 && PinTraitsT::skPinIdx <= 7)
            {
                return static_cast<GpioAlternativeFunction>(
                    LL_GPIO_GetAFPin_0_7(PortTraitsT::skInstance, PinTraitsT::skPinLLId));
            }
            else if constexpr (PinTraitsT::skPinIdx >= 8 && PinTraitsT::skPinIdx <= 15)
            {
                return static_cast<GpioAlternativeFunction>(
                    LL_GPIO_GetAFPin_8_15(PortTraitsT::skInstance, PinTraitsT::skPinLLId));
            }
            else
            {
                static_assert(kAlwaysFalseV<PinTraitsT::skPinIdx>, "Invalid Pin Id");
            }
        }

        void lock() const
        {
            LL_GPIO_LockPin(PortTraitsT::skInstance, PinTraitsT::skPinMask);
        }

        [[nodiscard]] bool is_locked() const
        {
            return static_cast<bool>(LL_GPIO_IsPinLocked(PortTraitsT::skInstance, PinTraitsT::skPinMask) != 0U);
        }

        [[nodiscard]] bool is_input_set() const
        {
            return static_cast<bool>(LL_GPIO_IsInputPinSet(PortTraitsT::skInstance, PinTraitsT::skPinMask) != 0U);
        }

        [[nodiscard]] bool is_output_set() const
        {
            return static_cast<bool>(LL_GPIO_IsOutputPinSet(PortTraitsT::skInstance, PinTraitsT::skPinMask) != 0U);
        }

        void set_output() const
        {
            LL_GPIO_SetOutputPin(PortTraitsT::skInstance, PinTraitsT::skPinMask);
        }

        void reset_output() const
        {
            LL_GPIO_ResetOutputPin(PortTraitsT::skInstance, PinTraitsT::skPinMask);
        }

        void toggle() const
        {
            LL_GPIO_TogglePin(PortTraitsT::skInstance, PinTraitsT::skPinMask);
        }
    };

}  // namespace valle::platform
