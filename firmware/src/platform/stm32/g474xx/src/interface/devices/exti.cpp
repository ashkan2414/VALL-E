#include "valle/platform/hardware/exti/interrupts.hpp"

#if __has_include("app_isr_bindings.hpp")
#include "app_isr_bindings.hpp"
#endif

namespace valle::platform
{
    /**
     * @brief Check if any granular ISR handler is bound for a specific EXTI line across all ports.
     *
     * @tparam tkLineId EXTI Line ID.
     */
    template <ExtiLineId tkLineId>
    [[nodiscard]] consteval static bool exti_pin_has_granular_handler()
    {
        return CBoundIsrRouter<ExtiLineIsrRouter<tkLineId>>;
    }

    /**
     * @brief Check if any granular ISR handler is bound for a specific NVIC IRQ.
     *
     * @tparam tkInterruptSource EXTI interrupt type (e.g., EXTIInterruptSource::k15_10).
     */
    template <EXTIInterruptSource tkInterruptSource>
    [[nodiscard]] consteval static bool exti_has_any_granular_isr_handler()
    {
        if constexpr (tkInterruptSource == EXTIInterruptSource::k0)
        {
            return exti_pin_has_granular_handler<ExtiLineId::kLine0>();
        }
        else if constexpr (tkInterruptSource == EXTIInterruptSource::k1)
        {
            return exti_pin_has_granular_handler<ExtiLineId::kLine1>();
        }
        else if constexpr (tkInterruptSource == EXTIInterruptSource::k2)
        {
            return exti_pin_has_granular_handler<ExtiLineId::kLine2>();
        }
        else if constexpr (tkInterruptSource == EXTIInterruptSource::k3)
        {
            return exti_pin_has_granular_handler<ExtiLineId::kLine3>();
        }
        else if constexpr (tkInterruptSource == EXTIInterruptSource::k4)
        {
            return exti_pin_has_granular_handler<ExtiLineId::kLine4>();
        }
        else if constexpr (tkInterruptSource == EXTIInterruptSource::k9_5)
        {
            return exti_pin_has_granular_handler<ExtiLineId::kLine5>() ||
                   exti_pin_has_granular_handler<ExtiLineId::kLine6>() ||
                   exti_pin_has_granular_handler<ExtiLineId::kLine7>() ||
                   exti_pin_has_granular_handler<ExtiLineId::kLine8>() ||
                   exti_pin_has_granular_handler<ExtiLineId::kLine9>();
        }
        else if constexpr (tkInterruptSource == EXTIInterruptSource::k15_10)
        {
            return exti_pin_has_granular_handler<ExtiLineId::kLine10>() ||
                   exti_pin_has_granular_handler<ExtiLineId::kLine11>() ||
                   exti_pin_has_granular_handler<ExtiLineId::kLine12>() ||
                   exti_pin_has_granular_handler<ExtiLineId::kLine13>() ||
                   exti_pin_has_granular_handler<ExtiLineId::kLine14>() ||
                   exti_pin_has_granular_handler<ExtiLineId::kLine15>();
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
     * @tparam tkLineId EXTI Line ID (e.g., ExtiLineId::kLine13).
     */
    template <ExtiLineId tkLineId>
    static void exti_line_handler()
    {
        using RouterT = ExtiLineIsrRouter<tkLineId>;
        using TraitsT = ExtiLineInterruptSourceInterface<tkLineId>;
        if constexpr (CBoundIsrRouter<RouterT>)
        {
            if (TraitsT::is_pending())
            {
                if constexpr (kIsrRouterConfigAck<RouterT>)
                {
                    TraitsT::clear();
                }
                RouterT::handle();
            }
        }
        else
        {
            if (TraitsT::is_pending())
            {
                TraitsT::clear();
            }
        }
    }

    /**
     * @brief EXTI NVIC Interrupt Handler Router
     *
     * @tparam tkInterruptSource EXTI interrupt type (e.g., EXTIInterruptSource::k15_10).
     */
    template <EXTIInterruptSource tkInterruptSource>
    static void exti_irq_handler()
    {
        using IrqRouterT                   = ExtiEXTIIrqRouter<tkInterruptSource>;
        constexpr bool has_global_router   = CBoundIsrRouter<IrqRouterT>;
        constexpr bool has_granular_router = exti_has_any_granular_isr_handler<tkInterruptSource>();

        static_assert(!(has_global_router && has_granular_router),
                      "VALLE CONFLICT: IRQ and Source ISR handlers detected for the same EXTI IRQ Group.");

        if constexpr (has_global_router)
        {
            IrqRouterT::handle();
            return;
        }

        // Route to the appropriate line demultiplexers based on the shared IRQ Group
        if constexpr (tkInterruptSource == EXTIInterruptSource::k0)
        {
            exti_line_handler<ExtiLineId::kLine0>();
        }
        else if constexpr (tkInterruptSource == EXTIInterruptSource::k1)
        {
            exti_line_handler<ExtiLineId::kLine1>();
        }
        else if constexpr (tkInterruptSource == EXTIInterruptSource::k2)
        {
            exti_line_handler<ExtiLineId::kLine2>();
        }
        else if constexpr (tkInterruptSource == EXTIInterruptSource::k3)
        {
            exti_line_handler<ExtiLineId::kLine3>();
        }
        else if constexpr (tkInterruptSource == EXTIInterruptSource::k4)
        {
            exti_line_handler<ExtiLineId::kLine4>();
        }
        else if constexpr (tkInterruptSource == EXTIInterruptSource::k9_5)
        {
            exti_line_handler<ExtiLineId::kLine5>();
            exti_line_handler<ExtiLineId::kLine6>();
            exti_line_handler<ExtiLineId::kLine7>();
            exti_line_handler<ExtiLineId::kLine8>();
            exti_line_handler<ExtiLineId::kLine9>();
        }
        else if constexpr (tkInterruptSource == EXTIInterruptSource::k15_10)
        {
            exti_line_handler<ExtiLineId::kLine10>();
            exti_line_handler<ExtiLineId::kLine11>();
            exti_line_handler<ExtiLineId::kLine12>();
            exti_line_handler<ExtiLineId::kLine13>();
            exti_line_handler<ExtiLineId::kLine14>();
            exti_line_handler<ExtiLineId::kLine15>();
        }
    }

    // ============================================================================
    // STM32 VECTOR TABLE BINDINGS (External C)
    // ============================================================================
    // On STM32G4, lines 0-4 have dedicated vectors, while 5-9 and 10-15 are shared.

    extern "C"
    {
        void EXTI0_IRQHandler(void)  // NOLINT(readability-identifier-naming)
        {
            exti_irq_handler<EXTIInterruptSource::k0>();
        }

        void EXTI1_IRQHandler(void)  // NOLINT(readability-identifier-naming)
        {
            exti_irq_handler<EXTIInterruptSource::k1>();
        }

        void EXTI2_IRQHandler(void)  // NOLINT(readability-identifier-naming)
        {
            exti_irq_handler<EXTIInterruptSource::k2>();
        }

        void EXTI3_IRQHandler(void)  // NOLINT(readability-identifier-naming)
        {
            exti_irq_handler<EXTIInterruptSource::k3>();
        }

        void EXTI4_IRQHandler(void)  // NOLINT(readability-identifier-naming)
        {
            exti_irq_handler<EXTIInterruptSource::k4>();
        }

        void EXTI9_5_IRQHandler(void)  // NOLINT(readability-identifier-naming)
        {
            exti_irq_handler<EXTIInterruptSource::k9_5>();
        }

        void EXTI15_10_IRQHandler(void)  // NOLINT(readability-identifier-naming)
        {
            exti_irq_handler<EXTIInterruptSource::k15_10>();
        }
    }

}  // namespace valle::platform