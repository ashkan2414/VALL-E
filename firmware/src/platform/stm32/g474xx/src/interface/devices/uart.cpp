#include "valle/platform/devices/uart.hpp"

#include "magic_enum/magic_enum.hpp"

#if __has_include("app_isr_bindings.hpp")
#include "app_isr_bindings.hpp"
#endif

namespace valle::platform
{
    /**
     * @brief Check if any granular ISR handler is bound for this UART.
     *
     * @tparam tkControllerId UART Controller ID.
     */
    template <UartControllerId tkControllerId>
    [[nodiscard]] consteval static inline bool uart_has_any_granular_isr_handler()
    {
        constexpr auto values = magic_enum::enum_values<UartInterruptSource>();

        return [values]<std::size_t... Is>(std::index_sequence<Is...>)
        {
            return (CBoundIsrRouter<UartIsrRouter<tkControllerId, values[Is]>> || ...);
        }(std::make_index_sequence<values.size()>{});
    }

    /**
     * @brief UART Interrupt Handler Router
     *
     * @tparam tkControllerId UART Index (1-5 for USART1-Uart5, 6 for LPUart1)
     */
    template <UartControllerId tkControllerId>
    static inline void uart_irq_handler()
    {
        using IrqRouterT                   = UartIrqRouter<tkControllerId>;
        constexpr bool has_irq_handler     = CBoundIsrRouter<IrqRouterT>;
        constexpr bool has_source_handlers = uart_has_any_granular_isr_handler<tkControllerId>();
        static_assert(!(has_irq_handler && has_source_handlers),
                      "VALLE CONFLICT: IRQ and Source ISR handlers detected.");

        if constexpr (has_irq_handler)
        {
            IrqRouterT::handle();
            return;
        }

#define HANDLE_UART_INT(tkIntSource)                                               \
    {                                                                              \
        using RouterT = UartIsrRouter<tkControllerId, (tkIntSource)>;              \
        using TraitsT = UartInterruptSourceInterface<tkControllerId, tkIntSource>; \
        if constexpr (CBoundIsrRouter<RouterT>)                                    \
        {                                                                          \
            if (TraitsT::is_pending())                                             \
            {                                                                      \
                if constexpr (kIsrRouterConfigAck<RouterT>)                        \
                {                                                                  \
                    TraitsT::clear();                                              \
                }                                                                  \
                RouterT::handle();                                                 \
            }                                                                      \
        }                                                                          \
    }

        // --- Core Status ---
        HANDLE_UART_INT(UartInterruptSource::kTxEmpty);
        HANDLE_UART_INT(UartInterruptSource::kTxComplete);
        HANDLE_UART_INT(UartInterruptSource::kTxCompleteBeforeGuard);
        HANDLE_UART_INT(UartInterruptSource::kRxNotEmpty);
        HANDLE_UART_INT(UartInterruptSource::kIdle);

        // --- Hardware Flow Control ---
        HANDLE_UART_INT(UartInterruptSource::kCTS);

        // --- FIFO Management ---
        HANDLE_UART_INT(UartInterruptSource::kTxFifoEmpty);
        HANDLE_UART_INT(UartInterruptSource::kRxFifoFull);
        HANDLE_UART_INT(UartInterruptSource::kTxFifoThreshold);
        HANDLE_UART_INT(UartInterruptSource::kRxFifoThreshold);

        // --- Errors ---
        HANDLE_UART_INT(UartInterruptSource::kOverrun);
        HANDLE_UART_INT(UartInterruptSource::kParityError);
        HANDLE_UART_INT(UartInterruptSource::kFramingError);
        HANDLE_UART_INT(UartInterruptSource::kNoiseError);
        HANDLE_UART_INT(UartInterruptSource::kSpiSlaveUnderrun);

        // --- Advanced Protocols ---
        HANDLE_UART_INT(UartInterruptSource::kCharMatch);
        HANDLE_UART_INT(UartInterruptSource::kReceiverTimeout);
        HANDLE_UART_INT(UartInterruptSource::kEndOfBlock);
        HANDLE_UART_INT(UartInterruptSource::kLinBreak);
        HANDLE_UART_INT(UartInterruptSource::kWakeup);

        // --- Auto Baud Rate ---
        HANDLE_UART_INT(UartInterruptSource::kAutoBaudError);
        HANDLE_UART_INT(UartInterruptSource::kAutoBaudFinished);

#undef HANDLE_UART_INT
    }

    extern "C"
    {
        void USART1_IRQHandler(void)  // NOLINT(readability-identifier-naming)
        {
            uart_irq_handler<UartControllerId::kUSART1>();
        }

        void USART2_IRQHandler(void)  // NOLINT(readability-identifier-naming)
        {
            uart_irq_handler<UartControllerId::kUSART2>();
        }

        void USART3_IRQHandler(void)  // NOLINT(readability-identifier-naming)
        {
            uart_irq_handler<UartControllerId::kUSART3>();
        }

        void UART4_IRQHandler(void)  // NOLINT(readability-identifier-naming)
        {
            uart_irq_handler<UartControllerId::kUart4>();
        }

        void UART5_IRQHandler(void)  // NOLINT(readability-identifier-naming)
        {
            uart_irq_handler<UartControllerId::kUart5>();
        }

        void LPUART1_IRQHandler(void)  // NOLINT(readability-identifier-naming)
        {
            uart_irq_handler<UartControllerId::kLPUart1>();
        }
    }
}  // namespace valle::platform