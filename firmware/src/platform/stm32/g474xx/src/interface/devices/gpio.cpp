#include "valle/platform/hardware/gpio/interrupts.hpp"

#if __has_include("app_isr_bindings.hpp")
#include "app_isr_bindings.hpp"
#endif

namespace valle::platform
{
    /**
     * @brief Check if any granular ISR handler is bound for a specific EXTI line across all ports.
     *
     * @tparam tkPinId EXTI Line / Pin ID.
     */
    template <GpioPinId tkPinId>
    [[nodiscard]] consteval static inline bool exti_pin_has_granular_handler()
    {
        return CBoundIsrRouter<GpioPinIsrRouter<GpioPortId::kPortA, tkPinId>> ||
               CBoundIsrRouter<GpioPinIsrRouter<GpioPortId::kPortB, tkPinId>> ||
               CBoundIsrRouter<GpioPinIsrRouter<GpioPortId::kPortC, tkPinId>> ||
               CBoundIsrRouter<GpioPinIsrRouter<GpioPortId::kPortD, tkPinId>> ||
               CBoundIsrRouter<GpioPinIsrRouter<GpioPortId::kPortE, tkPinId>> ||
               CBoundIsrRouter<GpioPinIsrRouter<GpioPortId::kPortF, tkPinId>> ||
               CBoundIsrRouter<GpioPinIsrRouter<GpioPortId::kPortG, tkPinId>>;
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
            return exti_pin_has_granular_handler<GpioPinId::kPin0>();
        }
        else if constexpr (tkInterruptType == EXTIInterruptType::k1)
        {
            return exti_pin_has_granular_handler<GpioPinId::kPin1>();
        }
        else if constexpr (tkInterruptType == EXTIInterruptType::k2)
        {
            return exti_pin_has_granular_handler<GpioPinId::kPin2>();
        }
        else if constexpr (tkInterruptType == EXTIInterruptType::k3)
        {
            return exti_pin_has_granular_handler<GpioPinId::kPin3>();
        }
        else if constexpr (tkInterruptType == EXTIInterruptType::k4)
        {
            return exti_pin_has_granular_handler<GpioPinId::kPin4>();
        }
        else if constexpr (tkInterruptType == EXTIInterruptType::k9_5)
        {
            return exti_pin_has_granular_handler<GpioPinId::kPin5>() ||
                   exti_pin_has_granular_handler<GpioPinId::kPin6>() ||
                   exti_pin_has_granular_handler<GpioPinId::kPin7>() ||
                   exti_pin_has_granular_handler<GpioPinId::kPin8>() ||
                   exti_pin_has_granular_handler<GpioPinId::kPin9>();
        }
        else if constexpr (tkInterruptType == EXTIInterruptType::k15_10)
        {
            return exti_pin_has_granular_handler<GpioPinId::kPin10>() ||
                   exti_pin_has_granular_handler<GpioPinId::kPin11>() ||
                   exti_pin_has_granular_handler<GpioPinId::kPin12>() ||
                   exti_pin_has_granular_handler<GpioPinId::kPin13>() ||
                   exti_pin_has_granular_handler<GpioPinId::kPin14>() ||
                   exti_pin_has_granular_handler<GpioPinId::kPin15>();
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
     * @tparam tkPinId GPIO Pin/EXTI Line ID (0-15)
     */
    template <GpioPinId tkPinId>
    static inline void exti_line_handler()
    {
        // The EXTI pending flag and line mask only depend on the pin ID, not the port.
        // We can safely use Port A's traits to check if this line fired at all.
        using PinTraitsT = GpioPinTraits<GpioPortId::kPortA, tkPinId>;

        if (LL_EXTI_IsActiveFlag_0_31(PinTraitsT::skLLEXTILine) != 0)
        {
            bool handled = false;

#define CHECK_PORT_HANDLER(tkPortId)                                    \
    if constexpr (CBoundIsrRouter<GpioPinIsrRouter<tkPortId, tkPinId>>) \
    {                                                                   \
        using TraitsT = GpioPinInterruptTraits<tkPortId, tkPinId>;      \
        using RouterT = GpioPinIsrRouter<tkPortId, tkPinId>;            \
        if (TraitsT::is_pending())                                      \
        {                                                               \
            if constexpr (kIsrRouterConfigAck<RouterT>)                 \
            {                                                           \
                TraitsT::ack();                                         \
            }                                                           \
            RouterT::handle();                                          \
            handled = true;                                             \
        }                                                               \
    }

            CHECK_PORT_HANDLER(GpioPortId::kPortA);
            CHECK_PORT_HANDLER(GpioPortId::kPortB);
            CHECK_PORT_HANDLER(GpioPortId::kPortC);
            CHECK_PORT_HANDLER(GpioPortId::kPortD);
            CHECK_PORT_HANDLER(GpioPortId::kPortE);
            CHECK_PORT_HANDLER(GpioPortId::kPortF);
            CHECK_PORT_HANDLER(GpioPortId::kPortG);

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
        using GlobalRouterT               = GpioEXTIGlobalIsrRouter<tkInterruptType>;
        constexpr bool kHasGlobalRouter   = CBoundIsrRouter<GlobalRouterT>;
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
            exti_line_handler<GpioPinId::kPin0>();
        }
        else if constexpr (tkInterruptType == EXTIInterruptType::k1)
        {
            exti_line_handler<GpioPinId::kPin1>();
        }
        else if constexpr (tkInterruptType == EXTIInterruptType::k2)
        {
            exti_line_handler<GpioPinId::kPin2>();
        }
        else if constexpr (tkInterruptType == EXTIInterruptType::k3)
        {
            exti_line_handler<GpioPinId::kPin3>();
        }
        else if constexpr (tkInterruptType == EXTIInterruptType::k4)
        {
            exti_line_handler<GpioPinId::kPin4>();
        }
        else if constexpr (tkInterruptType == EXTIInterruptType::k9_5)
        {
            exti_line_handler<GpioPinId::kPin5>();
            exti_line_handler<GpioPinId::kPin6>();
            exti_line_handler<GpioPinId::kPin7>();
            exti_line_handler<GpioPinId::kPin8>();
            exti_line_handler<GpioPinId::kPin9>();
        }
        else if constexpr (tkInterruptType == EXTIInterruptType::k15_10)
        {
            exti_line_handler<GpioPinId::kPin10>();
            exti_line_handler<GpioPinId::kPin11>();
            exti_line_handler<GpioPinId::kPin12>();
            exti_line_handler<GpioPinId::kPin13>();
            exti_line_handler<GpioPinId::kPin14>();
            exti_line_handler<GpioPinId::kPin15>();
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