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
     * @tparam tkPeripheralId UART Peripheral ID.
     */
    template <UartPeripheralId tkPeripheralId>
    [[nodiscard]] consteval static inline bool uart_has_any_granular_isr_handler()
    {
        constexpr auto values = magic_enum::enum_values<UartInterruptType>();

        return [values]<std::size_t... Is>(std::index_sequence<Is...>)
        {
            return (CBoundIsrRouter<UartIsrRouter<tkPeripheralId, values[Is]>> || ...);
        }(std::make_index_sequence<values.size()>{});
    }

    /**
     * @brief UART Interrupt Handler Router
     *
     * @tparam tkPeripheralId UART Index (1-5 for USART1-Uart5, 6 for LPUart1)
     */
    template <UartPeripheralId tkPeripheralId>
    static inline void uart_irq_handler()
    {
        using GlobalRouterT               = UartGlobalIsrRouter<tkPeripheralId>;
        constexpr bool kHasGlobalRouter   = CBoundIsrRouter<GlobalRouterT>;
        constexpr bool kHasGranularRouter = uart_has_any_granular_isr_handler<tkPeripheralId>();
        static_assert(!(kHasGlobalRouter && kHasGranularRouter), "VALLE CONFLICT: Global and Granular ISRs detected.");

        if constexpr (kHasGlobalRouter)
        {
            GlobalRouterT::handle();
            return;
        }

#define HANDLE_UART_INT(tkIntType)                                      \
    {                                                                   \
        using RouterT = UartIsrRouter<tkPeripheralId, (tkIntType)>;     \
        using TraitsT = UartInterruptTraits<tkPeripheralId, tkIntType>; \
        if constexpr (CBoundIsrRouter<RouterT>)                         \
        {                                                               \
            if (TraitsT::is_pending())                                  \
            {                                                           \
                if constexpr (kIsrRouterConfigAck<RouterT>)             \
                {                                                       \
                    TraitsT::ack();                                     \
                }                                                       \
                RouterT::handle();                                      \
            }                                                           \
        }                                                               \
    }

        // --- Core Status ---
        HANDLE_UART_INT(UartInterruptType::kTxEmpty);
        HANDLE_UART_INT(UartInterruptType::kTxComplete);
        HANDLE_UART_INT(UartInterruptType::kTxCompleteBeforeGuard);
        HANDLE_UART_INT(UartInterruptType::kRxNotEmpty);
        HANDLE_UART_INT(UartInterruptType::kIdle);

        // --- Hardware Flow Control ---
        HANDLE_UART_INT(UartInterruptType::kCTS);

        // --- FIFO Management ---
        HANDLE_UART_INT(UartInterruptType::kTxFifoEmpty);
        HANDLE_UART_INT(UartInterruptType::kRxFifoFull);
        HANDLE_UART_INT(UartInterruptType::kTxFifoThreshold);
        HANDLE_UART_INT(UartInterruptType::kRxFifoThreshold);

        // --- Errors ---
        HANDLE_UART_INT(UartInterruptType::kOverrun);
        HANDLE_UART_INT(UartInterruptType::kParityError);
        HANDLE_UART_INT(UartInterruptType::kFramingError);
        HANDLE_UART_INT(UartInterruptType::kNoiseError);
        HANDLE_UART_INT(UartInterruptType::kSpiSlaveUnderrun);

        // --- Advanced Protocols ---
        HANDLE_UART_INT(UartInterruptType::kCharMatch);
        HANDLE_UART_INT(UartInterruptType::kReceiverTimeout);
        HANDLE_UART_INT(UartInterruptType::kEndOfBlock);
        HANDLE_UART_INT(UartInterruptType::kLinBreak);
        HANDLE_UART_INT(UartInterruptType::kWakeup);

        // --- Auto Baud Rate ---
        HANDLE_UART_INT(UartInterruptType::kAutoBaudError);
        HANDLE_UART_INT(UartInterruptType::kAutoBaudFinished);

#undef HANDLE_UART_INT
    }

    extern "C"
    {
        void USART1_IRQHandler(void)  // NOLINT(readability-identifier-naming)
        {
            uart_irq_handler<UartPeripheralId::kUSART1>();
        }

        void USART2_IRQHandler(void)  // NOLINT(readability-identifier-naming)
        {
            uart_irq_handler<UartPeripheralId::kUSART2>();
        }

        void USART3_IRQHandler(void)  // NOLINT(readability-identifier-naming)
        {
            uart_irq_handler<UartPeripheralId::kUSART3>();
        }

        void UART4_IRQHandler(void)  // NOLINT(readability-identifier-naming)
        {
            uart_irq_handler<UartPeripheralId::kUart4>();
        }

        void UART5_IRQHandler(void)  // NOLINT(readability-identifier-naming)
        {
            uart_irq_handler<UartPeripheralId::kUart5>();
        }

        void LPUART1_IRQHandler(void)  // NOLINT(readability-identifier-naming)
        {
            uart_irq_handler<UartPeripheralId::kLPUart1>();
        }
    }
}  // namespace valle::platform