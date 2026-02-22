#pragma once

#include "Valle/Board/Traits/UART/id.hpp"
#include "Valle/Board/Traits/dma.hpp"
#include "stm32g4xx_hal.h"

namespace valle
{

    // ============================================================================
    // ENUMERATIONS
    // ============================================================================
    enum class UARTBaudRate
    {
        kBaud1200    = 1200,
        kBaud2400    = 2400,
        kBaud4800    = 4800,
        kBaud9600    = 9600,
        kBaud14400   = 14400,
        kBaud19200   = 19200,
        kBaud38400   = 38400,
        kBaud57600   = 57600,
        kBaud115200  = 115200,
        kBaud230400  = 230400,
        kBaud460800  = 460800,
        kBaud921600  = 921600,
        kBaud1000000 = 1000000,
        kBaud2000000 = 2000000,
    };

    enum class UARTWordLength
    {
        kBits7 = UART_WORDLENGTH_7B,
        kBits8 = UART_WORDLENGTH_8B,
        kBits9 = UART_WORDLENGTH_9B,
    };

    enum class UARTWakeUpAddressLength
    {
        kBits4 = UART_ADDRESS_DETECT_4B,
        kBits7 = UART_ADDRESS_DETECT_7B,
    };

    enum class UARTStopBits
    {
        kBits1   = UART_STOPBITS_1,
        kBits0_5 = UART_STOPBITS_0_5,
        kBits2   = UART_STOPBITS_2,
        kBits1_5 = UART_STOPBITS_1_5,
    };

    enum class UARTParity
    {
        kNone = UART_PARITY_NONE,
        kEven = UART_PARITY_EVEN,
        kOdd  = UART_PARITY_ODD,
    };

    enum class UARTHardwareFlowControl
    {
        kNone    = UART_HWCONTROL_NONE,
        kRTS     = UART_HWCONTROL_RTS,
        kCTS     = UART_HWCONTROL_CTS,
        kRTS_CTS = UART_HWCONTROL_RTS_CTS,
    };

    enum class UARTTransferMode
    {
        kTx   = UART_MODE_TX,
        kRx   = UART_MODE_RX,
        kTxRx = UART_MODE_TX_RX,
    };

    // ===========================================================================
    // HARDWARE TRAITS
    // ===========================================================================
    template <UARTControllerID tkID>
    struct UARTControllerTraits;

    template <>
    struct UARTControllerTraits<UARTControllerID::kUSART1>
    {
        static inline USART_TypeDef* const skInstance          = USART1;
        static constexpr IRQn_Type         skIRQn              = USART1_IRQn;
        static constexpr DMAMuxRequestID   skDMAMuxRequestTx   = DMAMuxRequestID::kUSART1Tx;
        static constexpr DMAMuxRequestID   skDMAMuxRequestRx   = DMAMuxRequestID::kUSART1Rx;
        static constexpr GPIOPortID        skDefaultGPIOTxPort = GPIOPortID::kA;
        static constexpr GPIOPinID         skDefaultGPIOTxPin  = 9;
        static constexpr GPIOPortID        skDefaultGPIORxPort = GPIOPortID::kA;
        static constexpr GPIOPinID         skDefaultGPIORxPin  = 10;

        static inline void enable_clock()
        {
            __HAL_RCC_USART1_CLK_ENABLE();
        }
    };

    template <>
    struct UARTControllerTraits<UARTControllerID::kUSART2>
    {
        static inline USART_TypeDef* const skInstance          = USART2;
        static constexpr IRQn_Type         skIRQn              = USART2_IRQn;
        static constexpr DMAMuxRequestID   skDMAMuxRequestTx   = DMAMuxRequestID::kUSART2Tx;
        static constexpr DMAMuxRequestID   skDMAMuxRequestRx   = DMAMuxRequestID::kUSART2Rx;
        static constexpr GPIOPortID        skDefaultGPIOTxPort = GPIOPortID::kA;
        static constexpr GPIOPinID         skDefaultGPIOTxPin  = 2;
        static constexpr GPIOPortID        skDefaultGPIORxPort = GPIOPortID::kA;
        static constexpr GPIOPinID         skDefaultGPIORxPin  = 3;

        static inline void enable_clock()
        {
            __HAL_RCC_USART2_CLK_ENABLE();
        }
    };

    template <>
    struct UARTControllerTraits<UARTControllerID::kUSART3>
    {
        static inline USART_TypeDef* const skInstance          = USART3;
        static constexpr IRQn_Type         skIRQn              = USART3_IRQn;
        static constexpr DMAMuxRequestID   skDMAMuxRequestTx   = DMAMuxRequestID::kUSART3Tx;
        static constexpr DMAMuxRequestID   skDMAMuxRequestRx   = DMAMuxRequestID::kUSART3Rx;
        static constexpr GPIOPortID        skDefaultGPIOTxPort = GPIOPortID::kB;
        static constexpr GPIOPinID         skDefaultGPIOTxPin  = 10;
        static constexpr GPIOPortID        skDefaultGPIORxPort = GPIOPortID::kB;
        static constexpr GPIOPinID         skDefaultGPIORxPin  = 11;

        static inline void enable_clock()
        {
            __HAL_RCC_USART3_CLK_ENABLE();
        }
    };

    template <>
    struct UARTControllerTraits<UARTControllerID::kUART4>
    {
        static inline USART_TypeDef* const skInstance          = UART4;
        static constexpr IRQn_Type         skIRQn              = UART4_IRQn;
        static constexpr DMAMuxRequestID   skDMAMuxRequestTx   = DMAMuxRequestID::kUART4Tx;
        static constexpr DMAMuxRequestID   skDMAMuxRequestRx   = DMAMuxRequestID::kUART4Rx;
        static constexpr GPIOPortID        skDefaultGPIOTxPort = GPIOPortID::kC;
        static constexpr GPIOPinID         skDefaultGPIOTxPin  = 10;
        static constexpr GPIOPortID        skDefaultGPIORxPort = GPIOPortID::kC;
        static constexpr GPIOPinID         skDefaultGPIORxPin  = 11;

        static inline void enable_clock()
        {
            __HAL_RCC_UART4_CLK_ENABLE();
        }
    };

    template <>
    struct UARTControllerTraits<UARTControllerID::kUART5>
    {
        static inline USART_TypeDef* const skInstance          = UART5;
        static constexpr IRQn_Type         skIRQn              = UART5_IRQn;
        static constexpr DMAMuxRequestID   skDMAMuxRequestTx   = DMAMuxRequestID::kUART5Tx;
        static constexpr DMAMuxRequestID   skDMAMuxRequestRx   = DMAMuxRequestID::kUART5Rx;
        static constexpr GPIOPortID        skDefaultGPIOTxPort = GPIOPortID::kC;
        static constexpr GPIOPinID         skDefaultGPIOTxPin  = 12;
        static constexpr GPIOPortID        skDefaultGPIORxPort = GPIOPortID::kD;
        static constexpr GPIOPinID         skDefaultGPIORxPin  = 2;

        static inline void enable_clock()
        {
            __HAL_RCC_UART5_CLK_ENABLE();
        }
    };

    template <>
    struct UARTControllerTraits<UARTControllerID::kLPUART1>
    {
        static inline USART_TypeDef* const skInstance          = LPUART1;
        static constexpr IRQn_Type         skIRQn              = LPUART1_IRQn;
        static constexpr DMAMuxRequestID   skDMAMuxRequestTx   = DMAMuxRequestID::kLPUART1Tx;
        static constexpr DMAMuxRequestID   skDMAMuxRequestRx   = DMAMuxRequestID::kLPUART1Rx;
        static constexpr GPIOPortID        skDefaultGPIOTxPort = GPIOPortID::kA;
        static constexpr GPIOPinID         skDefaultGPIOTxPin  = 2;
        static constexpr GPIOPortID        skDefaultGPIORxPort = GPIOPortID::kA;
        static constexpr GPIOPinID         skDefaultGPIORxPin  = 3;

        static inline void enable_clock()
        {
            __HAL_RCC_LPUART1_CLK_ENABLE();
        }
    };
}  // namespace valle