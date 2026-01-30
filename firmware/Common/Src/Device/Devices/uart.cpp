#include "Valle/Device/Devices/uart.hpp"

#include "magic_enum/magic_enum.hpp"

#if __has_include("app_isr_bindings.hpp")
#include "app_isr_bindings.hpp"
#endif

namespace valle
{
    /**
     * @brief Check if any granular ISR handler is bound for this UART.
     *
     * @tparam tkControllerID UART Controller ID.
     */
    template <UARTControllerID tkID>
    [[nodiscard]] consteval static inline bool uart_has_any_granular_isr_handler()
    {
        constexpr auto values = magic_enum::enum_values<UARTInterruptType>();

        return [values]<std::size_t... Is>(std::index_sequence<Is...>)
        {
            return (CBoundISRRouter<UARTISRRouter<tkID, values[Is]>> || ...);
        }(std::make_index_sequence<values.size()>{});
    }

    /**
     * @brief UART Interrupt Handler Router
     *
     * @tparam tkControllerID UART Index (1-5 for USART1-UART5, 6 for LPUART1)
     */
    template <UARTControllerID tkControllerID>
    static inline void uart_irq_handler()
    {
        using GlobalRouterT               = UARTGlobalISRRouter<tkControllerID>;
        constexpr bool kHasGlobalRouter   = CBoundISRRouter<GlobalRouterT>;
        constexpr bool kHasGranularRouter = uart_has_any_granular_isr_handler<tkControllerID>();
        static_assert(!(kHasGlobalRouter && kHasGranularRouter), "VALLE CONFLICT: Global and Granular ISRs detected.");

        if constexpr (kHasGlobalRouter)
        {
            GlobalRouterT::handle();
            return;
        }

#define HANDLE_UART_INT(tkIntType)                                      \
    {                                                                   \
        using RouterT = UARTISRRouter<tkControllerID, (tkIntType)>;     \
        using TraitsT = UARTInterruptTraits<tkControllerID, tkIntType>; \
        if constexpr (CBoundISRRouter<RouterT>)                         \
        {                                                               \
            if (TraitsT::is_pending())                                  \
            {                                                           \
                if constexpr (kISRRouterConfigAck<RouterT>)             \
                {                                                       \
                    TraitsT::ack();                                     \
                }                                                       \
                RouterT::handle();                                      \
            }                                                           \
        }                                                               \
    }

        // --- Core Status ---
        HANDLE_UART_INT(UARTInterruptType::kTxEmpty);
        HANDLE_UART_INT(UARTInterruptType::kTxComplete);
        HANDLE_UART_INT(UARTInterruptType::kTxCompleteBeforeGuard);
        HANDLE_UART_INT(UARTInterruptType::kRxNotEmpty);
        HANDLE_UART_INT(UARTInterruptType::kIdle);

        // --- Hardware Flow Control ---
        HANDLE_UART_INT(UARTInterruptType::kCTS);

        // --- FIFO Management ---
        HANDLE_UART_INT(UARTInterruptType::kTxFifoEmpty);
        HANDLE_UART_INT(UARTInterruptType::kRxFifoFull);
        HANDLE_UART_INT(UARTInterruptType::kTxFifoThreshold);
        HANDLE_UART_INT(UARTInterruptType::kRxFifoThreshold);

        // --- Errors ---
        HANDLE_UART_INT(UARTInterruptType::kOverrun);
        HANDLE_UART_INT(UARTInterruptType::kParityError);
        HANDLE_UART_INT(UARTInterruptType::kFramingError);
        HANDLE_UART_INT(UARTInterruptType::kNoiseError);
        HANDLE_UART_INT(UARTInterruptType::kSpiSlaveUnderrun);

        // --- Advanced Protocols ---
        HANDLE_UART_INT(UARTInterruptType::kCharMatch);
        HANDLE_UART_INT(UARTInterruptType::kReceiverTimeout);
        HANDLE_UART_INT(UARTInterruptType::kEndOfBlock);
        HANDLE_UART_INT(UARTInterruptType::kLinBreak);
        HANDLE_UART_INT(UARTInterruptType::kWakeup);

        // --- Auto Baud Rate ---
        HANDLE_UART_INT(UARTInterruptType::kAutoBaudError);
        HANDLE_UART_INT(UARTInterruptType::kAutoBaudFinished);

#undef HANDLE_UART_INT
    }

    extern "C"
    {
        void USART1_IRQHandler(void)  // NOLINT(readability-identifier-naming)
        {
            uart_irq_handler<UARTControllerID::kUSART1>();
        }

        void USART2_IRQHandler(void)  // NOLINT(readability-identifier-naming)
        {
            uart_irq_handler<UARTControllerID::kUSART2>();
        }

        void USART3_IRQHandler(void)  // NOLINT(readability-identifier-naming)
        {
            uart_irq_handler<UARTControllerID::kUSART3>();
        }

        void UART4_IRQHandler(void)  // NOLINT(readability-identifier-naming)
        {
            uart_irq_handler<UARTControllerID::kUART4>();
        }

        void UART5_IRQHandler(void)  // NOLINT(readability-identifier-naming)
        {
            uart_irq_handler<UARTControllerID::kUART5>();
        }

        void LPUART1_IRQHandler(void)  // NOLINT(readability-identifier-naming)
        {
            uart_irq_handler<UARTControllerID::kLPUART1>();
        }
    }
}  // namespace valle