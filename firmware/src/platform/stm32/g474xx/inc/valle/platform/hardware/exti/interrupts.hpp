#pragma once

#include "stm32g4xx_ll_exti.h"
#include "valle/platform/hardware/exti/interface.hpp"

namespace valle::platform
{
    // ============================================================================
    // INTERRUPT TRAITS
    // ============================================================================
    template <ExtiLineId tkLineId>
        requires(ExtiLineTraits<tkLineId>::skIrqNum.has_value())
    struct ExtiLineInterruptSourceInterface
    {
        using LineTraitsT = ExtiLineTraits<tkLineId>;
        using InterfaceT  = ExtiLineInterface<tkLineId>;

        static constexpr auto skIRQn = ExtiLineTraits<tkLineId>::skIrqNum;

        static inline void enable()
        {
            InterfaceT{}.enable_interrupt();
        }

        static inline void disable()
        {
            InterfaceT{}.disable_interrupt();
        }

        static inline bool is_enabled()
        {
            return InterfaceT{}.is_interrupt_enabled();
        }

        static inline bool flag_active()
        {
            return InterfaceT{}.is_flag_active();
        }

        static inline bool is_pending()
        {
            return flag_active() && is_enabled();
        }

        static inline void clear()
        {
            InterfaceT{}.clear_flag();
        }
    };

    // ===========================================================================
    // INTERRUPT CONTROLLER
    // ===========================================================================

    /**
     * @brief Configuration for GPIO Interrupts.
     */
    struct ExtiLineInterruptConfig
    {
        uint32_t priority = 5;  // NVIC Priority (0 = Highest, 15 = Lowest)
    };

    template <ExtiLineId tkLineId>
    struct ExtiLineInterruptController
    {
    public:
        static constexpr ExtiLineId skLineId = tkLineId;

        using LineTraitsT               = ExtiLineTraits<tkLineId>;
        using InterruptSourceInterfaceT = ExtiLineInterruptSourceInterface<tkLineId>;

        /**
         * @brief Enable interrupts for this GPIO pin.
         * @param config Configuration for GPIO Interrupts.
         */
        static void enable_interrupts(const ExtiLineInterruptConfig& config)
        {
            InterruptSourceInterfaceT::clear();
            InterruptSourceInterfaceT::enable();

            NVIC_SetPriority(InterruptSourceInterfaceT::skIRQn, config.priority);
            NVIC_EnableIRQ(InterruptSourceInterfaceT::skIRQn);
        }

        /**
         * @brief Disable interrupts for this GPIO pin.
         */
        static void disable_interrupts()
        {
            InterruptSourceInterfaceT::disable();

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

    enum class EXTIInterruptSource
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
     * @tparam tkInterruptSource EXTI interrupt type (e.g., EXTIInterruptSource::k15_10).
     */
    template <EXTIInterruptSource tkInterruptSource>
    struct GpioEXTIIrqRouter
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
     * for a given EXTI line.
     *
     * @tparam tkLineId EXTI line ID (e.g., ExtiLineId::kLine13).
     */
    template <ExtiLineId tkLineId>

    struct ExtiLineIsrRouter
    {
        using UnboundIsrHandlerTag = void;
        static void handle()
        {
            // Do nothing by default (optimized away)
        }
    };

}  // namespace valle::platform
