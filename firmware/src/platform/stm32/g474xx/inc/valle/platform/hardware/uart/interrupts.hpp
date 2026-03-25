#pragma once

#include "valle/platform/hardware/uart/interface.hpp"

namespace valle::platform
{
    // ============================================================================
    // INTERRUPT TRAITS
    // ============================================================================

    enum class UartInterruptType : uint8_t
    {
        // --- Basic ---
        kTxEmpty,                // TXE / TXFNF: Transmit data register empty (or FIFO not full)
        kTxComplete,             // TC: Transmission complete
        kRxNotEmpty,             // RXNE / RXFNE: Read data register not empty (or FIFO not empty)
        kIdle,                   // IDLE: Idle line detected
        kTxCompleteBeforeGuard,  // TCBGT (G4 specific, used in Smartcard/RS485)

        // --- Error ---
        kOverrun,           // ORE: Overrun error
        kParityError,       // PE: Parity error
        kFramingError,      // FE: Framing error
        kNoiseError,        // NE: Noise error
        kSpiSlaveUnderrun,  // UDR (Specific to SPI slave mode)

        // --- Hardware Flow Control ---
        kCTS,  // CTSIF (CTS Interrupt Flag)

        // --- FIFO Management (G4 Specific) ---
        kTxFifoEmpty,      // TXFE: TX FIFO is completely empty
        kRxFifoFull,       // RXFF: RX FIFO is completely full
        kTxFifoThreshold,  // TXFT: TX FIFO reached threshold (set in CR3)
        kRxFifoThreshold,  // RXFT: RX FIFO reached threshold (set in CR3)

        // --- Management / Protocol ---
        kCharMatch,        // CMF: Character match
        kReceiverTimeout,  // RTO: Receiver timeout (no data for specific duration)
        kEndOfBlock,       // EOB: End of Block (Smartcard mode)
        kLinBreak,         // LBD: LIN break detection
        kWakeup,           // WUF: Wakeup from Stop mode

        // --- Auto Baud Rate ---
        kAutoBaudError,    // ABRE: Auto-baud rate error
        kAutoBaudFinished  // ABRF: Auto-baud rate finished
    };

    template <UartPeripheralId tkPeripheralId, UartInterruptType tkIntType>
    struct UartInterruptTraits;

#define DEFINE_UART_INT_TRAIT(tkIntType, ll_name)                                                                      \
    template <UartPeripheralId tkPeripheralId>                                                                         \
    struct UartInterruptTraits<tkPeripheralId, (tkIntType)>                                                            \
    {                                                                                                                  \
        static inline void enable()                                                                                    \
        {                                                                                                              \
            LL_USART_EnableIT_##ll_name(UartControllerTraits<tkPeripheralId>::skInstance);                             \
        }                                                                                                              \
        static inline void disable()                                                                                   \
        {                                                                                                              \
            LL_USART_DisableIT_##ll_name(UartControllerTraits<tkPeripheralId>::skInstance);                            \
        }                                                                                                              \
        static inline bool is_enabled()                                                                                \
        {                                                                                                              \
            return LL_USART_IsEnabledIT_##ll_name(UartControllerTraits<tkPeripheralId>::skInstance);                   \
        }                                                                                                              \
        static inline bool flag_active()                                                                               \
        {                                                                                                              \
            return LL_USART_IsActiveFlag_##ll_name(UartControllerTraits<tkPeripheralId>::skInstance);                  \
        }                                                                                                              \
        static inline bool is_pending()                                                                                \
        {                                                                                                              \
            return flag_active() && is_enabled();                                                                      \
        }                                                                                                              \
        static inline void ack()                                                                                       \
        {                                                                                                              \
            /* Note: Some flags (TXE, RXNE) have no clear register; */                                                 \
            /* they are cleared by RDR/TDR access. LL handles this. */                                                 \
            if constexpr ((tkIntType) != UartInterruptType::kTxEmpty && (tkIntType) != UartInterruptType::kRxNotEmpty) \
            {                                                                                                          \
                LL_USART_ClearFlag_##ll_name(UartControllerTraits<tkPeripheralId>::skInstance);                        \
            }                                                                                                          \
        }                                                                                                              \
    };

    // Basic
    DEFINE_UART_INT_TRAIT(UartInterruptType::kTxEmpty, TXE_TXFNF);
    DEFINE_UART_INT_TRAIT(UartInterruptType::kTxComplete, TC);
    DEFINE_UART_INT_TRAIT(UartInterruptType::kTxCompleteBeforeGuard, TCBGT);
    DEFINE_UART_INT_TRAIT(UartInterruptType::kRxNotEmpty, RXNE_RXFNE);
    DEFINE_UART_INT_TRAIT(UartInterruptType::kIdle, IDLE);

    // Errors
    DEFINE_UART_INT_TRAIT(UartInterruptType::kOverrun, ORE);
    DEFINE_UART_INT_TRAIT(UartInterruptType::kParityError, PE);
    DEFINE_UART_INT_TRAIT(UartInterruptType::kFramingError, FE);
    DEFINE_UART_INT_TRAIT(UartInterruptType::kNoiseError, NE);
    DEFINE_UART_INT_TRAIT(UartInterruptType::kSpiSlaveUnderrun, UDR);

    // Flow Control
    DEFINE_UART_INT_TRAIT(UartInterruptType::kCTS, CTS);  // LL uses 'CTS' for the CTSIF trait

    // FIFO
    DEFINE_UART_INT_TRAIT(UartInterruptType::kTxFifoEmpty, TXFE);
    DEFINE_UART_INT_TRAIT(UartInterruptType::kRxFifoFull, RXFF);
    DEFINE_UART_INT_TRAIT(UartInterruptType::kTxFifoThreshold, TXFT);
    DEFINE_UART_INT_TRAIT(UartInterruptType::kRxFifoThreshold, RXFT);

    // Protocol
    DEFINE_UART_INT_TRAIT(UartInterruptType::kCharMatch, CM);
    DEFINE_UART_INT_TRAIT(UartInterruptType::kReceiverTimeout, RTO);
    DEFINE_UART_INT_TRAIT(UartInterruptType::kEndOfBlock, EOB);
    DEFINE_UART_INT_TRAIT(UartInterruptType::kLinBreak, LBD);
    DEFINE_UART_INT_TRAIT(UartInterruptType::kWakeup, WUF);

#undef DEFINE_UART_INT_TRAIT

    // ============================================================================
    // INTERRUPT CONTROLLER
    // ============================================================================

    struct UartInterruptMask
    {
        union
        {
            uint32_t mask;
            struct
            {
                // Basic
                bool tx_empty : 1                 = false;  // TXE / TXFNF
                bool tx_complete : 1              = false;  // TC
                bool rx_not_empty : 1             = false;  // RXNE / RXFNE
                bool idle : 1                     = false;  // IDLE
                bool tx_complete_before_guard : 1 = false;  // TCBGT

                // Errors
                bool overrun_error : 1      = false;  // ORE
                bool parity_error : 1       = false;  // PE
                bool framing_error : 1      = false;  // FE
                bool noise_error : 1        = false;  // NE
                bool spi_slave_underrun : 1 = false;  // UDR

                // Hardware Flow Control
                bool cts : 1 = false;  // CTSIF

                // FIFO
                bool tx_fifo_empty : 1     = false;  // TXFE
                bool rx_fifo_full : 1      = false;  // RXFF
                bool tx_fifo_threshold : 1 = false;  // TXFT
                bool rx_fifo_threshold : 1 = false;  // RXFT

                // Protocol-specific
                bool char_match : 1       = false;  // CM
                bool receiver_timeout : 1 = false;  // RTO
                bool end_of_block : 1     = false;  // EOB
                bool lin_break : 1        = false;  // LBD
                bool wakeup : 1           = false;  // WUF
            };
        };
    };

    struct UartControllerInterruptConfig
    {
        uint8_t           priority = 5;  // NVIC Priority (0-15)
        UartInterruptMask interrupts{};
    };

    template <UartPeripheralId tkPeripheralId>
    class UartInterruptController
    {
    public:
        static constexpr UartPeripheralId skPeripheralId = tkPeripheralId;
        using ControllerTraitsT                          = UartControllerTraits<tkPeripheralId>;

        static void enable_interrupts(const UartControllerInterruptConfig& config)
        {
            if (config.interrupts.tx_empty)
            {
                UartInterruptTraits<tkPeripheralId, UartInterruptType::kTxEmpty>::enable();
            }
            if (config.interrupts.tx_complete)
            {
                UartInterruptTraits<tkPeripheralId, UartInterruptType::kTxComplete>::enable();
            }

            if (config.interrupts.rx_not_empty)
            {
                UartInterruptTraits<tkPeripheralId, UartInterruptType::kRxNotEmpty>::enable();
            }
            if (config.interrupts.idle)
            {
                UartInterruptTraits<tkPeripheralId, UartInterruptType::kIdle>::enable();
            }
            if (config.interrupts.tx_complete_before_guard)
            {
                UartInterruptTraits<tkPeripheralId, UartInterruptType::kTxCompleteBeforeGuard>::enable();
            }
            if (config.interrupts.overrun_error)
            {
                UartInterruptTraits<tkPeripheralId, UartInterruptType::kOverrun>::enable();
            }
            if (config.interrupts.parity_error)
            {
                UartInterruptTraits<tkPeripheralId, UartInterruptType::kParityError>::enable();
            }
            if (config.interrupts.framing_error)
            {
                UartInterruptTraits<tkPeripheralId, UartInterruptType::kFramingError>::enable();
            }
            if (config.interrupts.noise_error)
            {
                UartInterruptTraits<tkPeripheralId, UartInterruptType::kNoiseError>::enable();
            }
            if (config.interrupts.spi_slave_underrun)
            {
                UartInterruptTraits<tkPeripheralId, UartInterruptType::kSpiSlaveUnderrun>::enable();
            }
            if (config.interrupts.cts)
            {
                UartInterruptTraits<tkPeripheralId, UartInterruptType::kCTS>::enable();
            }
            if (config.interrupts.tx_fifo_empty)
            {
                UartInterruptTraits<tkPeripheralId, UartInterruptType::kTxFifoEmpty>::enable();
            }
            if (config.interrupts.rx_fifo_full)
            {
                UartInterruptTraits<tkPeripheralId, UartInterruptType::kRxFifoFull>::enable();
            }
            if (config.interrupts.tx_fifo_threshold)
            {
                UartInterruptTraits<tkPeripheralId, UartInterruptType::kTxFifoThreshold>::enable();
            }
            if (config.interrupts.rx_fifo_threshold)
            {
                UartInterruptTraits<tkPeripheralId, UartInterruptType::kRxFifoThreshold>::enable();
            }
            if (config.interrupts.char_match)
            {
                UartInterruptTraits<tkPeripheralId, UartInterruptType::kCharMatch>::enable();
            }
            if (config.interrupts.receiver_timeout)
            {
                UartInterruptTraits<tkPeripheralId, UartInterruptType::kReceiverTimeout>::enable();
            }
            if (config.interrupts.end_of_block)
            {
                UartInterruptTraits<tkPeripheralId, UartInterruptType::kEndOfBlock>::enable();
            }
            if (config.interrupts.lin_break)
            {
                UartInterruptTraits<tkPeripheralId, UartInterruptType::kLinBreak>::enable();
            }
            if (config.interrupts.wakeup)
            {
                UartInterruptTraits<tkPeripheralId, UartInterruptType::kWakeup>::enable();
            }

            // Set NVIC Priority and Enable IRQ
            NVIC_SetPriority(ControllerTraitsT::skIRQn, config.priority);
            NVIC_EnableIRQ(ControllerTraitsT::skIRQn);
        }

        static void disable_interrupts()
        {
            UartInterruptTraits<tkPeripheralId, UartInterruptType::kTxEmpty>::disable();
            UartInterruptTraits<tkPeripheralId, UartInterruptType::kTxComplete>::disable();
            UartInterruptTraits<tkPeripheralId, UartInterruptType::kRxNotEmpty>::disable();
            UartInterruptTraits<tkPeripheralId, UartInterruptType::kIdle>::disable();
            UartInterruptTraits<tkPeripheralId, UartInterruptType::kTxCompleteBeforeGuard>::disable();
            UartInterruptTraits<tkPeripheralId, UartInterruptType::kOverrun>::disable();
            UartInterruptTraits<tkPeripheralId, UartInterruptType::kParityError>::disable();
            UartInterruptTraits<tkPeripheralId, UartInterruptType::kFramingError>::disable();
            UartInterruptTraits<tkPeripheralId, UartInterruptType::kNoiseError>::disable();
            UartInterruptTraits<tkPeripheralId, UartInterruptType::kSpiSlaveUnderrun>::disable();
            UartInterruptTraits<tkPeripheralId, UartInterruptType::kCTS>::disable();
            UartInterruptTraits<tkPeripheralId, UartInterruptType::kTxFifoEmpty>::disable();
            UartInterruptTraits<tkPeripheralId, UartInterruptType::kRxFifoFull>::disable();
            UartInterruptTraits<tkPeripheralId, UartInterruptType::kTxFifoThreshold>::disable();
            UartInterruptTraits<tkPeripheralId, UartInterruptType::kRxFifoThreshold>::disable();
            UartInterruptTraits<tkPeripheralId, UartInterruptType::kCharMatch>::disable();
            UartInterruptTraits<tkPeripheralId, UartInterruptType::kReceiverTimeout>::disable();
            UartInterruptTraits<tkPeripheralId, UartInterruptType::kEndOfBlock>::disable();
            UartInterruptTraits<tkPeripheralId, UartInterruptType::kLinBreak>::disable();
            UartInterruptTraits<tkPeripheralId, UartInterruptType::kWakeup>::disable();

            NVIC_DisableIRQ(ControllerTraitsT::skIRQn);
        }
    };

    // ============================================================================
    // ISR ROUTERS
    // ============================================================================

    // ---------------------------------------------------------------------------
    // GLOBAL ISR ROUTER
    // ---------------------------------------------------------------------------

    /**
     * @brief Global ISR Router for a specific UART.
     * Specializing this allows you to handle the entire ISR in one function
     * (e.g., when delegating to the ST HAL).
     *
     * @tparam tkPeripheralId UART Peripheral ID.
     */
    template <UartPeripheralId tkPeripheralId>
    struct UartGlobalIsrRouter
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
     * @brief UART ISR Router
     *
     * Specialize this template in your application or driver to bind
     * logic to specific UART interrupts.
     *
     * @tparam tkPeripheralId UART Peripheral ID.
     * @tparam tkIntType      UART Interrupt Type.
     */
    template <UartPeripheralId tkPeripheralId, UartInterruptType tkIntType>
    struct UartIsrRouter
    {
        using UnboundIsrHandlerTag = void;

        static void handle()
        {
        }
    };

}  // namespace valle::platform