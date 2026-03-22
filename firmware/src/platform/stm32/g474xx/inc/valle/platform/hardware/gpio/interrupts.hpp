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
    template <GPIOPortID tkPortID, GPIOPinID tkPinID>
        requires(kValidGPIOPinID<tkPortID, tkPinID>)
    struct GPIOPinInterruptTraits
    {
        static constexpr IRQn_Type skIRQn = GPIOPinTraits<tkPortID, tkPinID>::skIRQn;

        using PortTraitsT = GPIOPortTraits<tkPortID>;
        using PinTraitsT  = GPIOPinTraits<tkPortID, tkPinID>;

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
    struct GPIOPinInterruptConfig
    {
        uint32_t priority = 5;  // NVIC Priority (0 = Highest, 15 = Lowest)
    };

    template <GPIOPortID tkPortID, GPIOPinID tkPinID>
        requires(kValidGPIOPinID<tkPortID, tkPinID>)
    struct GPIOPinInterruptController
    {
    public:
        static constexpr GPIOPortID skPortID = tkPortID;
        static constexpr GPIOPinID  skPinID  = tkPinID;

        using PinTraitsT       = GPIOPinTraits<tkPortID, tkPinID>;
        using InterruptTraitsT = GPIOPinInterruptTraits<tkPortID, tkPinID>;

        /**
         * @brief Enable interrupts for this GPIO pin.
         * @param config Configuration for GPIO Interrupts.
         */
        static void enable_interrupts(const GPIOPinInterruptConfig& config)
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
    struct GPIOEXTIGlobalISRRouter
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
     * @tparam tkPortID GPIO Port ID (e.g., GPIOPortID::kA)
     * @tparam tkPinID  GPIO Pin ID (0-15)
     */
    template <GPIOPortID tkPortID, GPIOPinID tkPinID>
        requires(kValidGPIOPinID<tkPortID, tkPinID>)
    struct GPIOPinISRRouter
    {
        using UnboundIsrHandlerTag = void;
        static void handle()
        {
            // Do nothing by default (optimized away)
        }
    };

}  // namespace valle::platform