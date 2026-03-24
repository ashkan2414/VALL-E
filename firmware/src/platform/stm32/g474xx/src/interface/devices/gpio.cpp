#include "valle/platform/hardware/gpio/interrupts.hpp"

#if __has_include("app_isr_bindings.hpp")
#include "app_isr_bindings.hpp"
#endif

namespace valle::platform
{
    /**
     * @brief Check if any granular ISR handler is bound for a specific EXTI line across all ports.
     *
     * @tparam tkPinID EXTI Line / Pin ID.
     */
    template <GPIOPinID tkPinID>
    [[nodiscard]] consteval static inline bool exti_pin_has_granular_handler()
    {
        return CBoundISRRouter<GPIOPinISRRouter<GPIOPortID::kPortA, tkPinID>> ||
               CBoundISRRouter<GPIOPinISRRouter<GPIOPortID::kPortB, tkPinID>> ||
               CBoundISRRouter<GPIOPinISRRouter<GPIOPortID::kPortC, tkPinID>> ||
               CBoundISRRouter<GPIOPinISRRouter<GPIOPortID::kPortD, tkPinID>> ||
               CBoundISRRouter<GPIOPinISRRouter<GPIOPortID::kPortE, tkPinID>> ||
               CBoundISRRouter<GPIOPinISRRouter<GPIOPortID::kPortF, tkPinID>> ||
               CBoundISRRouter<GPIOPinISRRouter<GPIOPortID::kPortG, tkPinID>>;
    }

    /**
     * @brief Check if any granular ISR handler is bound for a specific NVIC IRQ.
     *
     * @tparam tkInterruptType EXTI interrupt type (e.g., EXTIInterruptType::k15_10).
     */
    template <EXTIInterruptType tkInterruptType>
    [[nodiscard]] consteval static inline bool exti_has_any_granular_isr_handler()
    {
        if constexpr (tkInterruptType == EXTIInterruptType::k0)
        {
            return exti_pin_has_granular_handler<GPIOPinID::kPin0>();
        }
        else if constexpr (tkInterruptType == EXTIInterruptType::k1)
        {
            return exti_pin_has_granular_handler<GPIOPinID::kPin1>();
        }
        else if constexpr (tkInterruptType == EXTIInterruptType::k2)
        {
            return exti_pin_has_granular_handler<GPIOPinID::kPin2>();
        }
        else if constexpr (tkInterruptType == EXTIInterruptType::k3)
        {
            return exti_pin_has_granular_handler<GPIOPinID::kPin3>();
        }
        else if constexpr (tkInterruptType == EXTIInterruptType::k4)
        {
            return exti_pin_has_granular_handler<GPIOPinID::kPin4>();
        }
        else if constexpr (tkInterruptType == EXTIInterruptType::k9_5)
        {
            return exti_pin_has_granular_handler<GPIOPinID::kPin5>() ||
                   exti_pin_has_granular_handler<GPIOPinID::kPin6>() ||
                   exti_pin_has_granular_handler<GPIOPinID::kPin7>() ||
                   exti_pin_has_granular_handler<GPIOPinID::kPin8>() ||
                   exti_pin_has_granular_handler<GPIOPinID::kPin9>();
        }
        else if constexpr (tkInterruptType == EXTIInterruptType::k15_10)
        {
            return exti_pin_has_granular_handler<GPIOPinID::kPin10>() ||
                   exti_pin_has_granular_handler<GPIOPinID::kPin11>() ||
                   exti_pin_has_granular_handler<GPIOPinID::kPin12>() ||
                   exti_pin_has_granular_handler<GPIOPinID::kPin13>() ||
                   exti_pin_has_granular_handler<GPIOPinID::kPin14>() ||
                   exti_pin_has_granular_handler<GPIOPinID::kPin15>();
        }
        else
        {
            return false;
        }
    }

    /**
     * @brief EXTI Line Dispatcher
     *
     * Checks if a specific EXTI line is pending at the hardware level. If it is,
     * it dynamically routes to the active port's granular handler.
     *
     * @tparam tkPinID GPIO Pin/EXTI Line ID (0-15)
     */
    template <GPIOPinID tkPinID>
    static inline void exti_line_handler()
    {
        // The EXTI pending flag and line mask only depend on the pin ID, not the port.
        // We can safely use Port A's traits to check if this line fired at all.
        using PinTraitsT = GPIOPinTraits<GPIOPortID::kPortA, tkPinID>;

        if (LL_EXTI_IsActiveFlag_0_31(PinTraitsT::skLLEXTILine) != 0)
        {
            bool handled = false;

#define CHECK_PORT_HANDLER(tkPortID)                                    \
    if constexpr (CBoundISRRouter<GPIOPinISRRouter<tkPortID, tkPinID>>) \
    {                                                                   \
        using TraitsT = GPIOPinInterruptTraits<tkPortID, tkPinID>;      \
        using RouterT = GPIOPinISRRouter<tkPortID, tkPinID>;            \
        if (TraitsT::is_pending())                                      \
        {                                                               \
            if constexpr (kISRRouterConfigAck<RouterT>)                 \
            {                                                           \
                TraitsT::ack();                                         \
            }                                                           \
            RouterT::handle();                                          \
            handled = true;                                             \
        }                                                               \
    }

            CHECK_PORT_HANDLER(GPIOPortID::kPortA);
            CHECK_PORT_HANDLER(GPIOPortID::kPortB);
            CHECK_PORT_HANDLER(GPIOPortID::kPortC);
            CHECK_PORT_HANDLER(GPIOPortID::kPortD);
            CHECK_PORT_HANDLER(GPIOPortID::kPortE);
            CHECK_PORT_HANDLER(GPIOPortID::kPortF);
            CHECK_PORT_HANDLER(GPIOPortID::kPortG);

#undef CHECK_PORT_HANDLER

            // Fallback: If an interrupt fired but no matching user router was bound
            // (or if the hardware triggered spuriously), we MUST clear the EXTI flag!
            // Failing to do this causes the CPU to lock up in an infinite ISR loop.
            if (!handled)
            {
                LL_EXTI_ClearFlag_0_31(PinTraitsT::skLLEXTILine);
            }
        }
    }

    /**
     * @brief EXTI NVIC Interrupt Handler Router
     *
     * @tparam tkInterruptType EXTI interrupt type (e.g., EXTIInterruptType::k15_10).
     */
    template <EXTIInterruptType tkInterruptType>
    static inline void exti_irq_handler()
    {
        using GlobalRouterT               = GPIOEXTIGlobalISRRouter<tkInterruptType>;
        constexpr bool kHasGlobalRouter   = CBoundISRRouter<GlobalRouterT>;
        constexpr bool kHasGranularRouter = exti_has_any_granular_isr_handler<tkInterruptType>();

        static_assert(!(kHasGlobalRouter && kHasGranularRouter),
                      "VALLE CONFLICT: Global and Granular ISRs detected for the same EXTI IRQ Group.");

        if constexpr (kHasGlobalRouter)
        {
            GlobalRouterT::handle();
            return;
        }

        // Route to the appropriate line demultiplexers based on the shared IRQ Group
        if constexpr (tkInterruptType == EXTIInterruptType::k0)
        {
            exti_line_handler<GPIOPinID::kPin0>();
        }
        else if constexpr (tkInterruptType == EXTIInterruptType::k1)
        {
            exti_line_handler<GPIOPinID::kPin1>();
        }
        else if constexpr (tkInterruptType == EXTIInterruptType::k2)
        {
            exti_line_handler<GPIOPinID::kPin2>();
        }
        else if constexpr (tkInterruptType == EXTIInterruptType::k3)
        {
            exti_line_handler<GPIOPinID::kPin3>();
        }
        else if constexpr (tkInterruptType == EXTIInterruptType::k4)
        {
            exti_line_handler<GPIOPinID::kPin4>();
        }
        else if constexpr (tkInterruptType == EXTIInterruptType::k9_5)
        {
            exti_line_handler<GPIOPinID::kPin5>();
            exti_line_handler<GPIOPinID::kPin6>();
            exti_line_handler<GPIOPinID::kPin7>();
            exti_line_handler<GPIOPinID::kPin8>();
            exti_line_handler<GPIOPinID::kPin9>();
        }
        else if constexpr (tkInterruptType == EXTIInterruptType::k15_10)
        {
            exti_line_handler<GPIOPinID::kPin10>();
            exti_line_handler<GPIOPinID::kPin11>();
            exti_line_handler<GPIOPinID::kPin12>();
            exti_line_handler<GPIOPinID::kPin13>();
            exti_line_handler<GPIOPinID::kPin14>();
            exti_line_handler<GPIOPinID::kPin15>();
        }
    }

    // ============================================================================
    // STM32 VECTOR TABLE BINDINGS (External C)
    // ============================================================================
    // On STM32G4, lines 0-4 have dedicated vectors, while 5-9 and 10-15 are shared.

    extern "C"
    {
        void EXTI0_IRQHandler(void)
        {
            exti_irq_handler<EXTIInterruptType::k0>();
        }

        void EXTI1_IRQHandler(void)
        {
            exti_irq_handler<EXTIInterruptType::k1>();
        }

        void EXTI2_IRQHandler(void)
        {
            exti_irq_handler<EXTIInterruptType::k2>();
        }

        void EXTI3_IRQHandler(void)
        {
            exti_irq_handler<EXTIInterruptType::k3>();
        }

        void EXTI4_IRQHandler(void)
        {
            exti_irq_handler<EXTIInterruptType::k4>();
        }

        void EXTI9_5_IRQHandler(void)
        {
            exti_irq_handler<EXTIInterruptType::k9_5>();
        }

        void EXTI15_10_IRQHandler(void)
        {
            exti_irq_handler<EXTIInterruptType::k15_10>();
        }
    }

}  // namespace valle::platform