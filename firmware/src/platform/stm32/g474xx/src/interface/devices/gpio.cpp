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
        requires(tkPinID < 16)
    [[nodiscard]] consteval static inline bool exti_pin_has_granular_handler()
    {
        return CBoundISRRouter<GPIOPinISRRouter<GPIOPortID::kA, tkPinID>> ||
               CBoundISRRouter<GPIOPinISRRouter<GPIOPortID::kB, tkPinID>> ||
               CBoundISRRouter<GPIOPinISRRouter<GPIOPortID::kC, tkPinID>> ||
               CBoundISRRouter<GPIOPinISRRouter<GPIOPortID::kD, tkPinID>> ||
               CBoundISRRouter<GPIOPinISRRouter<GPIOPortID::kE, tkPinID>> ||
               CBoundISRRouter<GPIOPinISRRouter<GPIOPortID::kF, tkPinID>> ||
               CBoundISRRouter<GPIOPinISRRouter<GPIOPortID::kG, tkPinID>>;
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
            return exti_pin_has_granular_handler<0>();
        }
        else if constexpr (tkInterruptType == EXTIInterruptType::k1)
        {
            return exti_pin_has_granular_handler<1>();
        }
        else if constexpr (tkInterruptType == EXTIInterruptType::k2)
        {
            return exti_pin_has_granular_handler<2>();
        }
        else if constexpr (tkInterruptType == EXTIInterruptType::k3)
        {
            return exti_pin_has_granular_handler<3>();
        }
        else if constexpr (tkInterruptType == EXTIInterruptType::k4)
        {
            return exti_pin_has_granular_handler<4>();
        }
        else if constexpr (tkInterruptType == EXTIInterruptType::k9_5)
        {
            return exti_pin_has_granular_handler<5>() || exti_pin_has_granular_handler<6>() ||
                   exti_pin_has_granular_handler<7>() || exti_pin_has_granular_handler<8>() ||
                   exti_pin_has_granular_handler<9>();
        }
        else if constexpr (tkInterruptType == EXTIInterruptType::k15_10)
        {
            return exti_pin_has_granular_handler<10>() || exti_pin_has_granular_handler<11>() ||
                   exti_pin_has_granular_handler<12>() || exti_pin_has_granular_handler<13>() ||
                   exti_pin_has_granular_handler<14>() || exti_pin_has_granular_handler<15>();
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
        requires(tkPinID < 16)
    static inline void exti_line_handler()
    {
        // The EXTI pending flag and line mask only depend on the pin ID, not the port.
        // We can safely use Port A's traits to check if this line fired at all.
        using PinTraitsT = GPIOPinTraits<GPIOPortID::kA, tkPinID>;

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

            CHECK_PORT_HANDLER(GPIOPortID::kA);
            CHECK_PORT_HANDLER(GPIOPortID::kB);
            CHECK_PORT_HANDLER(GPIOPortID::kC);
            CHECK_PORT_HANDLER(GPIOPortID::kD);
            CHECK_PORT_HANDLER(GPIOPortID::kE);
            CHECK_PORT_HANDLER(GPIOPortID::kF);
            CHECK_PORT_HANDLER(GPIOPortID::kG);

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
            exti_line_handler<0>();
        }
        else if constexpr (tkInterruptType == EXTIInterruptType::k1)
        {
            exti_line_handler<1>();
        }
        else if constexpr (tkInterruptType == EXTIInterruptType::k2)
        {
            exti_line_handler<2>();
        }
        else if constexpr (tkInterruptType == EXTIInterruptType::k3)
        {
            exti_line_handler<3>();
        }
        else if constexpr (tkInterruptType == EXTIInterruptType::k4)
        {
            exti_line_handler<4>();
        }
        else if constexpr (tkInterruptType == EXTIInterruptType::k9_5)
        {
            exti_line_handler<5>();
            exti_line_handler<6>();
            exti_line_handler<7>();
            exti_line_handler<8>();
            exti_line_handler<9>();
        }
        else if constexpr (tkInterruptType == EXTIInterruptType::k15_10)
        {
            exti_line_handler<10>();
            exti_line_handler<11>();
            exti_line_handler<12>();
            exti_line_handler<13>();
            exti_line_handler<14>();
            exti_line_handler<15>();
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