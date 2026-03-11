#pragma once

#include "valle/platform/hardware/uart/interface.hpp"

namespace valle::platform
{
    // ============================================================================
    // INTERRUPT TRAITS
    // ============================================================================

    enum class UARTInterruptType : uint8_t
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

    template <UARTControllerID tkControllerID, UARTInterruptType tkIntType>
    struct UARTInterruptTraits;

#define DEFINE_UART_INT_TRAIT(tkIntType, ll_name)                                                                      \
    template <UARTControllerID tkControllerID>                                                                         \
    struct UARTInterruptTraits<tkControllerID, (tkIntType)>                                                            \
    {                                                                                                                  \
        static inline void enable()                                                                                    \
        {                                                                                                              \
            LL_USART_EnableIT_##ll_name(UARTControllerTraits<tkControllerID>::skInstance);                             \
        }                                                                                                              \
        static inline void disable()                                                                                   \
        {                                                                                                              \
            LL_USART_DisableIT_##ll_name(UARTControllerTraits<tkControllerID>::skInstance);                            \
        }                                                                                                              \
        static inline bool is_enabled()                                                                                \
        {                                                                                                              \
            return LL_USART_IsEnabledIT_##ll_name(UARTControllerTraits<tkControllerID>::skInstance);                   \
        }                                                                                                              \
        static inline bool flag_active()                                                                               \
        {                                                                                                              \
            return LL_USART_IsActiveFlag_##ll_name(UARTControllerTraits<tkControllerID>::skInstance);                  \
        }                                                                                                              \
        static inline bool is_pending()                                                                                \
        {                                                                                                              \
            return flag_active() && is_enabled();                                                                      \
        }                                                                                                              \
        static inline void ack()                                                                                       \
        {                                                                                                              \
            /* Note: Some flags (TXE, RXNE) have no clear register; */                                                 \
            /* they are cleared by RDR/TDR access. LL handles this. */                                                 \
            if constexpr ((tkIntType) != UARTInterruptType::kTxEmpty && (tkIntType) != UARTInterruptType::kRxNotEmpty) \
            {                                                                                                          \
                LL_USART_ClearFlag_##ll_name(UARTControllerTraits<tkControllerID>::skInstance);                        \
            }                                                                                                          \
        }                                                                                                              \
    };

    // Basic
    DEFINE_UART_INT_TRAIT(UARTInterruptType::kTxEmpty, TXE_TXFNF);
    DEFINE_UART_INT_TRAIT(UARTInterruptType::kTxComplete, TC);
    DEFINE_UART_INT_TRAIT(UARTInterruptType::kTxCompleteBeforeGuard, TCBGT);
    DEFINE_UART_INT_TRAIT(UARTInterruptType::kRxNotEmpty, RXNE_RXFNE);
    DEFINE_UART_INT_TRAIT(UARTInterruptType::kIdle, IDLE);

    // Errors
    DEFINE_UART_INT_TRAIT(UARTInterruptType::kOverrun, ORE);
    DEFINE_UART_INT_TRAIT(UARTInterruptType::kParityError, PE);
    DEFINE_UART_INT_TRAIT(UARTInterruptType::kFramingError, FE);
    DEFINE_UART_INT_TRAIT(UARTInterruptType::kNoiseError, NE);
    DEFINE_UART_INT_TRAIT(UARTInterruptType::kSpiSlaveUnderrun, UDR);

    // Flow Control
    DEFINE_UART_INT_TRAIT(UARTInterruptType::kCTS, CTS);  // LL uses 'CTS' for the CTSIF trait

    // FIFO
    DEFINE_UART_INT_TRAIT(UARTInterruptType::kTxFifoEmpty, TXFE);
    DEFINE_UART_INT_TRAIT(UARTInterruptType::kRxFifoFull, RXFF);
    DEFINE_UART_INT_TRAIT(UARTInterruptType::kTxFifoThreshold, TXFT);
    DEFINE_UART_INT_TRAIT(UARTInterruptType::kRxFifoThreshold, RXFT);

    // Protocol
    DEFINE_UART_INT_TRAIT(UARTInterruptType::kCharMatch, CM);
    DEFINE_UART_INT_TRAIT(UARTInterruptType::kReceiverTimeout, RTO);
    DEFINE_UART_INT_TRAIT(UARTInterruptType::kEndOfBlock, EOB);
    DEFINE_UART_INT_TRAIT(UARTInterruptType::kLinBreak, LBD);
    DEFINE_UART_INT_TRAIT(UARTInterruptType::kWakeup, WUF);

#undef DEFINE_UART_INT_TRAIT

    // ============================================================================
    // INTERRUPT CONTROLLER
    // ============================================================================

    struct UARTInterruptMask
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

    struct UARTControllerInterruptConfig
    {
        uint8_t           priority = 5;  // NVIC Priority (0-15)
        UARTInterruptMask interrupts{};
    };

    template <UARTControllerID tkControllerID>
    class UARTInterruptController
    {
    public:
        static constexpr UARTControllerID skControllerID = tkControllerID;
        using ControllerTraitsT                          = UARTControllerTraits<tkControllerID>;

        static void enable_interrupts(const UARTControllerInterruptConfig& config)
        {
            if (config.interrupts.tx_empty)
            {
                UARTInterruptTraits<tkControllerID, UARTInterruptType::kTxEmpty>::enable();
            }
            if (config.interrupts.tx_complete)
            {
                UARTInterruptTraits<tkControllerID, UARTInterruptType::kTxComplete>::enable();
            }

            if (config.interrupts.rx_not_empty)
            {
                UARTInterruptTraits<tkControllerID, UARTInterruptType::kRxNotEmpty>::enable();
            }
            if (config.interrupts.idle)
            {
                UARTInterruptTraits<tkControllerID, UARTInterruptType::kIdle>::enable();
            }
            if (config.interrupts.tx_complete_before_guard)
            {
                UARTInterruptTraits<tkControllerID, UARTInterruptType::kTxCompleteBeforeGuard>::enable();
            }
            if (config.interrupts.overrun_error)
            {
                UARTInterruptTraits<tkControllerID, UARTInterruptType::kOverrun>::enable();
            }
            if (config.interrupts.parity_error)
            {
                UARTInterruptTraits<tkControllerID, UARTInterruptType::kParityError>::enable();
            }
            if (config.interrupts.framing_error)
            {
                UARTInterruptTraits<tkControllerID, UARTInterruptType::kFramingError>::enable();
            }
            if (config.interrupts.noise_error)
            {
                UARTInterruptTraits<tkControllerID, UARTInterruptType::kNoiseError>::enable();
            }
            if (config.interrupts.spi_slave_underrun)
            {
                UARTInterruptTraits<tkControllerID, UARTInterruptType::kSpiSlaveUnderrun>::enable();
            }
            if (config.interrupts.cts)
            {
                UARTInterruptTraits<tkControllerID, UARTInterruptType::kCTS>::enable();
            }
            if (config.interrupts.tx_fifo_empty)
            {
                UARTInterruptTraits<tkControllerID, UARTInterruptType::kTxFifoEmpty>::enable();
            }
            if (config.interrupts.rx_fifo_full)
            {
                UARTInterruptTraits<tkControllerID, UARTInterruptType::kRxFifoFull>::enable();
            }
            if (config.interrupts.tx_fifo_threshold)
            {
                UARTInterruptTraits<tkControllerID, UARTInterruptType::kTxFifoThreshold>::enable();
            }
            if (config.interrupts.rx_fifo_threshold)
            {
                UARTInterruptTraits<tkControllerID, UARTInterruptType::kRxFifoThreshold>::enable();
            }
            if (config.interrupts.char_match)
            {
                UARTInterruptTraits<tkControllerID, UARTInterruptType::kCharMatch>::enable();
            }
            if (config.interrupts.receiver_timeout)
            {
                UARTInterruptTraits<tkControllerID, UARTInterruptType::kReceiverTimeout>::enable();
            }
            if (config.interrupts.end_of_block)
            {
                UARTInterruptTraits<tkControllerID, UARTInterruptType::kEndOfBlock>::enable();
            }
            if (config.interrupts.lin_break)
            {
                UARTInterruptTraits<tkControllerID, UARTInterruptType::kLinBreak>::enable();
            }
            if (config.interrupts.wakeup)
            {
                UARTInterruptTraits<tkControllerID, UARTInterruptType::kWakeup>::enable();
            }

            // Set NVIC Priority and Enable IRQ
            NVIC_SetPriority(ControllerTraitsT::skIRQn, config.priority);
            NVIC_EnableIRQ(ControllerTraitsT::skIRQn);
        }

        static void disable_interrupts()
        {
            UARTInterruptTraits<tkControllerID, UARTInterruptType::kTxEmpty>::disable();
            UARTInterruptTraits<tkControllerID, UARTInterruptType::kTxComplete>::disable();
            UARTInterruptTraits<tkControllerID, UARTInterruptType::kRxNotEmpty>::disable();
            UARTInterruptTraits<tkControllerID, UARTInterruptType::kIdle>::disable();
            UARTInterruptTraits<tkControllerID, UARTInterruptType::kTxCompleteBeforeGuard>::disable();
            UARTInterruptTraits<tkControllerID, UARTInterruptType::kOverrun>::disable();
            UARTInterruptTraits<tkControllerID, UARTInterruptType::kParityError>::disable();
            UARTInterruptTraits<tkControllerID, UARTInterruptType::kFramingError>::disable();
            UARTInterruptTraits<tkControllerID, UARTInterruptType::kNoiseError>::disable();
            UARTInterruptTraits<tkControllerID, UARTInterruptType::kSpiSlaveUnderrun>::disable();
            UARTInterruptTraits<tkControllerID, UARTInterruptType::kCTS>::disable();
            UARTInterruptTraits<tkControllerID, UARTInterruptType::kTxFifoEmpty>::disable();
            UARTInterruptTraits<tkControllerID, UARTInterruptType::kRxFifoFull>::disable();
            UARTInterruptTraits<tkControllerID, UARTInterruptType::kTxFifoThreshold>::disable();
            UARTInterruptTraits<tkControllerID, UARTInterruptType::kRxFifoThreshold>::disable();
            UARTInterruptTraits<tkControllerID, UARTInterruptType::kCharMatch>::disable();
            UARTInterruptTraits<tkControllerID, UARTInterruptType::kReceiverTimeout>::disable();
            UARTInterruptTraits<tkControllerID, UARTInterruptType::kEndOfBlock>::disable();
            UARTInterruptTraits<tkControllerID, UARTInterruptType::kLinBreak>::disable();
            UARTInterruptTraits<tkControllerID, UARTInterruptType::kWakeup>::disable();

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
     * @tparam tkControllerID UART Controller ID.
     */
    template <UARTControllerID tkControllerID>
    struct UARTGlobalISRRouter
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
     * @tparam tkControllerID UART Controller ID.
     * @tparam tkIntType      UART Interrupt Type.
     */
    template <UARTControllerID tkControllerID, UARTInterruptType tkIntType>
    struct UARTISRRouter
    {
        using UnboundIsrHandlerTag = void;

        static void handle()
        {
        }
    };

}  // namespace valle::platform