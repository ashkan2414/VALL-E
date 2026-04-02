#pragma once

#include "valle/platform/hardware/uart/interface.hpp"

namespace valle::platform
{
    // ============================================================================
    // INTERRUPT TRAITS
    // ============================================================================

    enum class UartInterruptSource : uint8_t
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

    template <UartControllerId tkControllerId, UartInterruptSource tkIntSource>
    struct UartInterruptSourceInterface;

#define DEFINE_UART_INT_TRAIT(tkIntSource, ll_name)                                                   \
    template <UartControllerId tkControllerId>                                                        \
    struct UartInterruptSourceInterface<tkControllerId, (tkIntSource)>                                \
    {                                                                                                 \
        static inline void enable()                                                                   \
        {                                                                                             \
            LL_USART_EnableIT_##ll_name(UartControllerTraits<tkControllerId>::skInstance);            \
        }                                                                                             \
        static inline void disable()                                                                  \
        {                                                                                             \
            LL_USART_DisableIT_##ll_name(UartControllerTraits<tkControllerId>::skInstance);           \
        }                                                                                             \
        static inline bool is_enabled()                                                               \
        {                                                                                             \
            return LL_USART_IsEnabledIT_##ll_name(UartControllerTraits<tkControllerId>::skInstance);  \
        }                                                                                             \
        static inline bool flag_active()                                                              \
        {                                                                                             \
            return LL_USART_IsActiveFlag_##ll_name(UartControllerTraits<tkControllerId>::skInstance); \
        }                                                                                             \
        static inline bool is_pending()                                                               \
        {                                                                                             \
            return flag_active() && is_enabled();                                                     \
        }                                                                                             \
        static inline void clear()                                                                    \
        {                                                                                             \
            /* Note: Some flags (TXE, RXNE) have no clear register; */                                \
            /* they are cleared by RDR/TDR access. LL handles this. */                                \
            if constexpr ((tkIntSource) != UartInterruptSource::kTxEmpty &&                           \
                          (tkIntSource) != UartInterruptSource::kRxNotEmpty)                          \
            {                                                                                         \
                LL_USART_ClearFlag_##ll_name(UartControllerTraits<tkControllerId>::skInstance);       \
            }                                                                                         \
        }                                                                                             \
    };

    // Basic
    DEFINE_UART_INT_TRAIT(UartInterruptSource::kTxEmpty, TXE_TXFNF);
    DEFINE_UART_INT_TRAIT(UartInterruptSource::kTxComplete, TC);
    DEFINE_UART_INT_TRAIT(UartInterruptSource::kTxCompleteBeforeGuard, TCBGT);
    DEFINE_UART_INT_TRAIT(UartInterruptSource::kRxNotEmpty, RXNE_RXFNE);
    DEFINE_UART_INT_TRAIT(UartInterruptSource::kIdle, IDLE);

    // Errors
    DEFINE_UART_INT_TRAIT(UartInterruptSource::kOverrun, ORE);
    DEFINE_UART_INT_TRAIT(UartInterruptSource::kParityError, PE);
    DEFINE_UART_INT_TRAIT(UartInterruptSource::kFramingError, FE);
    DEFINE_UART_INT_TRAIT(UartInterruptSource::kNoiseError, NE);
    DEFINE_UART_INT_TRAIT(UartInterruptSource::kSpiSlaveUnderrun, UDR);

    // Flow Control
    DEFINE_UART_INT_TRAIT(UartInterruptSource::kCTS, CTS);  // LL uses 'CTS' for the CTSIF trait

    // FIFO
    DEFINE_UART_INT_TRAIT(UartInterruptSource::kTxFifoEmpty, TXFE);
    DEFINE_UART_INT_TRAIT(UartInterruptSource::kRxFifoFull, RXFF);
    DEFINE_UART_INT_TRAIT(UartInterruptSource::kTxFifoThreshold, TXFT);
    DEFINE_UART_INT_TRAIT(UartInterruptSource::kRxFifoThreshold, RXFT);

    // Protocol
    DEFINE_UART_INT_TRAIT(UartInterruptSource::kCharMatch, CM);
    DEFINE_UART_INT_TRAIT(UartInterruptSource::kReceiverTimeout, RTO);
    DEFINE_UART_INT_TRAIT(UartInterruptSource::kEndOfBlock, EOB);
    DEFINE_UART_INT_TRAIT(UartInterruptSource::kLinBreak, LBD);
    DEFINE_UART_INT_TRAIT(UartInterruptSource::kWakeup, WUF);

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

    template <UartControllerId tkControllerId>
    class UartInterruptController
    {
    public:
        static constexpr UartControllerId skControllerId = tkControllerId;
        using ControllerTraitsT                          = UartControllerTraits<tkControllerId>;

        static void enable_interrupts(const UartControllerInterruptConfig& config)
        {
            if (config.interrupts.tx_empty)
            {
                UartInterruptSourceInterface<tkControllerId, UartInterruptSource::kTxEmpty>::enable();
            }
            if (config.interrupts.tx_complete)
            {
                UartInterruptSourceInterface<tkControllerId, UartInterruptSource::kTxComplete>::enable();
            }

            if (config.interrupts.rx_not_empty)
            {
                UartInterruptSourceInterface<tkControllerId, UartInterruptSource::kRxNotEmpty>::enable();
            }
            if (config.interrupts.idle)
            {
                UartInterruptSourceInterface<tkControllerId, UartInterruptSource::kIdle>::enable();
            }
            if (config.interrupts.tx_complete_before_guard)
            {
                UartInterruptSourceInterface<tkControllerId, UartInterruptSource::kTxCompleteBeforeGuard>::enable();
            }
            if (config.interrupts.overrun_error)
            {
                UartInterruptSourceInterface<tkControllerId, UartInterruptSource::kOverrun>::enable();
            }
            if (config.interrupts.parity_error)
            {
                UartInterruptSourceInterface<tkControllerId, UartInterruptSource::kParityError>::enable();
            }
            if (config.interrupts.framing_error)
            {
                UartInterruptSourceInterface<tkControllerId, UartInterruptSource::kFramingError>::enable();
            }
            if (config.interrupts.noise_error)
            {
                UartInterruptSourceInterface<tkControllerId, UartInterruptSource::kNoiseError>::enable();
            }
            if (config.interrupts.spi_slave_underrun)
            {
                UartInterruptSourceInterface<tkControllerId, UartInterruptSource::kSpiSlaveUnderrun>::enable();
            }
            if (config.interrupts.cts)
            {
                UartInterruptSourceInterface<tkControllerId, UartInterruptSource::kCTS>::enable();
            }
            if (config.interrupts.tx_fifo_empty)
            {
                UartInterruptSourceInterface<tkControllerId, UartInterruptSource::kTxFifoEmpty>::enable();
            }
            if (config.interrupts.rx_fifo_full)
            {
                UartInterruptSourceInterface<tkControllerId, UartInterruptSource::kRxFifoFull>::enable();
            }
            if (config.interrupts.tx_fifo_threshold)
            {
                UartInterruptSourceInterface<tkControllerId, UartInterruptSource::kTxFifoThreshold>::enable();
            }
            if (config.interrupts.rx_fifo_threshold)
            {
                UartInterruptSourceInterface<tkControllerId, UartInterruptSource::kRxFifoThreshold>::enable();
            }
            if (config.interrupts.char_match)
            {
                UartInterruptSourceInterface<tkControllerId, UartInterruptSource::kCharMatch>::enable();
            }
            if (config.interrupts.receiver_timeout)
            {
                UartInterruptSourceInterface<tkControllerId, UartInterruptSource::kReceiverTimeout>::enable();
            }
            if (config.interrupts.end_of_block)
            {
                UartInterruptSourceInterface<tkControllerId, UartInterruptSource::kEndOfBlock>::enable();
            }
            if (config.interrupts.lin_break)
            {
                UartInterruptSourceInterface<tkControllerId, UartInterruptSource::kLinBreak>::enable();
            }
            if (config.interrupts.wakeup)
            {
                UartInterruptSourceInterface<tkControllerId, UartInterruptSource::kWakeup>::enable();
            }

            // Set NVIC Priority and Enable IRQ
            NVIC_SetPriority(ControllerTraitsT::skIRQn, config.priority);
            NVIC_EnableIRQ(ControllerTraitsT::skIRQn);
        }

        static void disable_interrupts()
        {
            UartInterruptSourceInterface<tkControllerId, UartInterruptSource::kTxEmpty>::disable();
            UartInterruptSourceInterface<tkControllerId, UartInterruptSource::kTxComplete>::disable();
            UartInterruptSourceInterface<tkControllerId, UartInterruptSource::kRxNotEmpty>::disable();
            UartInterruptSourceInterface<tkControllerId, UartInterruptSource::kIdle>::disable();
            UartInterruptSourceInterface<tkControllerId, UartInterruptSource::kTxCompleteBeforeGuard>::disable();
            UartInterruptSourceInterface<tkControllerId, UartInterruptSource::kOverrun>::disable();
            UartInterruptSourceInterface<tkControllerId, UartInterruptSource::kParityError>::disable();
            UartInterruptSourceInterface<tkControllerId, UartInterruptSource::kFramingError>::disable();
            UartInterruptSourceInterface<tkControllerId, UartInterruptSource::kNoiseError>::disable();
            UartInterruptSourceInterface<tkControllerId, UartInterruptSource::kSpiSlaveUnderrun>::disable();
            UartInterruptSourceInterface<tkControllerId, UartInterruptSource::kCTS>::disable();
            UartInterruptSourceInterface<tkControllerId, UartInterruptSource::kTxFifoEmpty>::disable();
            UartInterruptSourceInterface<tkControllerId, UartInterruptSource::kRxFifoFull>::disable();
            UartInterruptSourceInterface<tkControllerId, UartInterruptSource::kTxFifoThreshold>::disable();
            UartInterruptSourceInterface<tkControllerId, UartInterruptSource::kRxFifoThreshold>::disable();
            UartInterruptSourceInterface<tkControllerId, UartInterruptSource::kCharMatch>::disable();
            UartInterruptSourceInterface<tkControllerId, UartInterruptSource::kReceiverTimeout>::disable();
            UartInterruptSourceInterface<tkControllerId, UartInterruptSource::kEndOfBlock>::disable();
            UartInterruptSourceInterface<tkControllerId, UartInterruptSource::kLinBreak>::disable();
            UartInterruptSourceInterface<tkControllerId, UartInterruptSource::kWakeup>::disable();

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
     * @tparam tkControllerId UART Controller ID.
     */
    template <UartControllerId tkControllerId>
    struct UartIrqRouter
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
     * @tparam tkControllerId UART Controller ID.
     * @tparam tkIntSource      UART Interrupt Type.
     */
    template <UartControllerId tkControllerId, UartInterruptSource tkIntSource>
    struct UartIsrRouter
    {
        using UnboundIsrHandlerTag = void;

        static void handle()
        {
        }
    };

}  // namespace valle::platform