#pragma once

#include "stm32g4xx_hal.h"
#include "stm32g4xx_hal_gpio.h"
#include "stm32g4xx_ll_exti.h"
#include "valle/platform/hardware/gpio/interface.hpp"

namespace valle::platform
{
    // ============================================================================
    // INTERRUPT TRAITS
    // ============================================================================
    template <GpioPortId tkPortId, GpioPinId tkPinId>

    struct GpioPinInterruptTraits
    {
        static constexpr IRQn_Type skIRQn = GpioPinTraits<tkPortId, tkPinId>::skIRQn;

        using PortTraitsT = GpioPortTraits<tkPortId>;
        using PinTraitsT  = GpioPinTraits<tkPortId, tkPinId>;

        static inline bool is_exti_source()
        {
            return LL_SYSCFG_GetEXTISource(PinTraitsT::skLLSyscfgEXTILine) == PortTraitsT::skLLSyscfgEXTIPort;
        }

        static inline void enable()
        {
            LL_EXTI_EnableIT_0_31(PinTraitsT::skLLEXTILine);
        }

        static inline void disable()
        {
            LL_EXTI_DisableIT_0_31(PinTraitsT::skLLEXTILine);
        }

        static inline bool is_enabled()
        {
            return LL_EXTI_IsEnabledIT_0_31(PinTraitsT::skLLEXTILine) != 0 && is_exti_source();
        }

        static inline bool flag_active()
        {
            return LL_EXTI_IsActiveFlag_0_31(PinTraitsT::skLLEXTILine) != 0 && is_exti_source();
        }

        static inline bool is_pending()
        {
            return flag_active() && is_enabled();
        }

        static inline void ack()
        {
            if (is_exti_source())
            {
                LL_EXTI_ClearFlag_0_31(PinTraitsT::skLLEXTILine);
            }
        }
    };

    // ===========================================================================
    // INTERRUPT CONTROLLER
    // ===========================================================================

    /**
     * @brief Configuration for GPIO Interrupts.
     */
    struct GpioPinInterruptConfig
    {
        uint32_t priority = 5;  // NVIC Priority (0 = Highest, 15 = Lowest)
    };

    template <GpioPortId tkPortId, GpioPinId tkPinId>

    struct GpioPinInterruptController
    {
    public:
        static constexpr GpioPortId skPortId = tkPortId;
        static constexpr GpioPinId  skPinId  = tkPinId;

        using PinTraitsT       = GpioPinTraits<tkPortId, tkPinId>;
        using InterruptTraitsT = GpioPinInterruptTraits<tkPortId, tkPinId>;

        /**
         * @brief Enable interrupts for this GPIO pin.
         * @param config Configuration for GPIO Interrupts.
         */
        static void enable_interrupts(const GpioPinInterruptConfig& config)
        {
            InterruptTraitsT::ack();
            InterruptTraitsT::enable();

            NVIC_SetPriority(InterruptTraitsT::skIRQn, config.priority);
            NVIC_EnableIRQ(InterruptTraitsT::skIRQn);
        }

        /**
         * @brief Disable interrupts for this GPIO pin.
         */
        static void disable_interrupts()
        {
            InterruptTraitsT::disable();

            // Note: We intentionally do NOT disable the NVIC IRQ here.
            // EXTI lines 5-9 and 10-15 share NVIC interrupts. If we called
            // NVIC_DisableIRQ() on pin 5, it would break pins 6, 7, 8, and 9!
            // Disabling the specific EXTI line mask above is perfectly sufficient.
        }
    };

    // ===========================================================================
    // INTERRUPT ROUTERS
    // ===========================================================================

    // ---------------------------------------------------------------------------
    // GLOBAL ISR ROUTER
    // ---------------------------------------------------------------------------

    enum class EXTIInterruptType
    {
        k0,
        k1,
        k2,
        k3,
        k4,
        k9_5,
        k15_10,
    };

    /**
     * @brief Global EXTI ISR Router for shared or specific EXTI lines.
     * Specializing this allows you to handle the entire ISR in one function
     * (e.g., EXTI15_10_IRQHandler).
     *
     * @tparam tkInterruptType EXTI interrupt type (e.g., EXTIInterruptType::k15_10).
     */
    template <EXTIInterruptType tkInterruptType>
    struct GpioEXTIGlobalIsrRouter
    {
        using UnboundIsrHandlerTag = void;

        static void handle()
        {
        }
    };

    // ---------------------------------------------------------------------------
    // GRANULAR ISR ROUTER
    // ---------------------------------------------------------------------------

    /**
     * @brief GPIO ISR Router
     *
     * Specialize this template to handle specific GPIO EXTI interrupts
     * for a given port and pin.
     *
     * @tparam tkPortId GPIO Port ID (e.g., GpioPortId::kPortA)
     * @tparam tkPinId  GPIO Pin ID (0-15)
     */
    template <GpioPortId tkPortId, GpioPinId tkPinId>

    struct GpioPinIsrRouter
    {
        using UnboundIsrHandlerTag = void;
        static void handle()
        {
            // Do nothing by default (optimized away)
        }
    };

}  // namespace valle::platform
