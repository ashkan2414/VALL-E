#pragma once

#include "stm32g4xx_hal.h"
#include "valle/platform/hardware/dma.hpp"
#include "valle/platform/hardware/uart/id.hpp"

namespace valle::platform
{

    // ============================================================================
    // ENUMERATIONS
    // ============================================================================
    enum class UartBaudRate
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

    enum class UartWordLength
    {
        kBits7 = UART_WORDLENGTH_7B,
        kBits8 = UART_WORDLENGTH_8B,
        kBits9 = UART_WORDLENGTH_9B,
    };

    enum class UartWakeUpAddressLength
    {
        kBits4 = UART_ADDRESS_DETECT_4B,
        kBits7 = UART_ADDRESS_DETECT_7B,
    };

    enum class UartStopBits
    {
        kBits1   = UART_STOPBITS_1,
        kBits0_5 = UART_STOPBITS_0_5,
        kBits2   = UART_STOPBITS_2,
        kBits1_5 = UART_STOPBITS_1_5,
    };

    enum class UartParity
    {
        kNone = UART_PARITY_NONE,
        kEven = UART_PARITY_EVEN,
        kOdd  = UART_PARITY_ODD,
    };

    enum class UartHardwareFlowControl
    {
        kNone    = UART_HWCONTROL_NONE,
        kRTS     = UART_HWCONTROL_RTS,
        kCTS     = UART_HWCONTROL_CTS,
        kRTS_CTS = UART_HWCONTROL_RTS_CTS,
    };

    enum class UartTransferMode
    {
        kTx   = UART_MODE_TX,
        kRx   = UART_MODE_RX,
        kTxRx = UART_MODE_TX_RX,
    };

    // ===========================================================================
    // HARDWARE TRAITS
    // ===========================================================================
    template <UartPeripheralId tkPeripheralId>
    struct UartControllerTraits;

    template <>
    struct UartControllerTraits<UartPeripheralId::kUSART1>
    {
        static inline USART_TypeDef* const skInstance          = USART1;
        static constexpr IRQn_Type         skIRQn              = USART1_IRQn;
        static constexpr DmaMuxRequestId   skDmaMuxRequestTx   = DmaMuxRequestId::kUSART1Tx;
        static constexpr DmaMuxRequestId   skDmaMuxRequestRx   = DmaMuxRequestId::kUSART1Rx;
        static constexpr GpioPortId        skDefaultGpioTxPort = GpioPortId::kPortA;
        static constexpr GpioPinId         skDefaultGpioTxPin  = GpioPinId::kPin9;
        static constexpr GpioPortId        skDefaultGpioRxPort = GpioPortId::kPortA;
        static constexpr GpioPinId         skDefaultGpioRxPin  = GpioPinId::kPin10;

        inline void enable_clock() const
        {
            __HAL_RCC_USART1_CLK_ENABLE();
        }
    };

    template <>
    struct UartControllerTraits<UartPeripheralId::kUSART2>
    {
        static inline USART_TypeDef* const skInstance          = USART2;
        static constexpr IRQn_Type         skIRQn              = USART2_IRQn;
        static constexpr DmaMuxRequestId   skDmaMuxRequestTx   = DmaMuxRequestId::kUSART2Tx;
        static constexpr DmaMuxRequestId   skDmaMuxRequestRx   = DmaMuxRequestId::kUSART2Rx;
        static constexpr GpioPortId        skDefaultGpioTxPort = GpioPortId::kPortA;
        static constexpr GpioPinId         skDefaultGpioTxPin  = GpioPinId::kPin2;
        static constexpr GpioPortId        skDefaultGpioRxPort = GpioPortId::kPortA;
        static constexpr GpioPinId         skDefaultGpioRxPin  = GpioPinId::kPin3;

        inline void enable_clock() const
        {
            __HAL_RCC_USART2_CLK_ENABLE();
        }
    };

    template <>
    struct UartControllerTraits<UartPeripheralId::kUSART3>
    {
        static inline USART_TypeDef* const skInstance          = USART3;
        static constexpr IRQn_Type         skIRQn              = USART3_IRQn;
        static constexpr DmaMuxRequestId   skDmaMuxRequestTx   = DmaMuxRequestId::kUSART3Tx;
        static constexpr DmaMuxRequestId   skDmaMuxRequestRx   = DmaMuxRequestId::kUSART3Rx;
        static constexpr GpioPortId        skDefaultGpioTxPort = GpioPortId::kPortB;
        static constexpr GpioPinId         skDefaultGpioTxPin  = GpioPinId::kPin10;
        static constexpr GpioPortId        skDefaultGpioRxPort = GpioPortId::kPortB;
        static constexpr GpioPinId         skDefaultGpioRxPin  = GpioPinId::kPin11;

        inline void enable_clock() const
        {
            __HAL_RCC_USART3_CLK_ENABLE();
        }
    };

    template <>
    struct UartControllerTraits<UartPeripheralId::kUart4>
    {
        static inline USART_TypeDef* const skInstance          = UART4;
        static constexpr IRQn_Type         skIRQn              = UART4_IRQn;
        static constexpr DmaMuxRequestId   skDmaMuxRequestTx   = DmaMuxRequestId::kUART4Tx;
        static constexpr DmaMuxRequestId   skDmaMuxRequestRx   = DmaMuxRequestId::kUART4Rx;
        static constexpr GpioPortId        skDefaultGpioTxPort = GpioPortId::kPortC;
        static constexpr GpioPinId         skDefaultGpioTxPin  = GpioPinId::kPin10;
        static constexpr GpioPortId        skDefaultGpioRxPort = GpioPortId::kPortC;
        static constexpr GpioPinId         skDefaultGpioRxPin  = GpioPinId::kPin11;

        inline void enable_clock() const
        {
            __HAL_RCC_UART4_CLK_ENABLE();
        }
    };

    template <>
    struct UartControllerTraits<UartPeripheralId::kUart5>
    {
        static inline USART_TypeDef* const skInstance          = UART5;
        static constexpr IRQn_Type         skIRQn              = UART5_IRQn;
        static constexpr DmaMuxRequestId   skDmaMuxRequestTx   = DmaMuxRequestId::kUART5Tx;
        static constexpr DmaMuxRequestId   skDmaMuxRequestRx   = DmaMuxRequestId::kUART5Rx;
        static constexpr GpioPortId        skDefaultGpioTxPort = GpioPortId::kPortC;
        static constexpr GpioPinId         skDefaultGpioTxPin  = GpioPinId::kPin12;
        static constexpr GpioPortId        skDefaultGpioRxPort = GpioPortId::kPortD;
        static constexpr GpioPinId         skDefaultGpioRxPin  = GpioPinId::kPin2;

        inline void enable_clock() const
        {
            __HAL_RCC_UART5_CLK_ENABLE();
        }
    };

    template <>
    struct UartControllerTraits<UartPeripheralId::kLPUart1>
    {
        static inline USART_TypeDef* const skInstance          = LPUART1;
        static constexpr IRQn_Type         skIRQn              = LPUART1_IRQn;
        static constexpr DmaMuxRequestId   skDmaMuxRequestTx   = DmaMuxRequestId::kLPUART1Tx;
        static constexpr DmaMuxRequestId   skDmaMuxRequestRx   = DmaMuxRequestId::kLPUART1Rx;
        static constexpr GpioPortId        skDefaultGpioTxPort = GpioPortId::kPortA;
        static constexpr GpioPinId         skDefaultGpioTxPin  = GpioPinId::kPin2;
        static constexpr GpioPortId        skDefaultGpioRxPort = GpioPortId::kPortA;
        static constexpr GpioPinId         skDefaultGpioRxPin  = GpioPinId::kPin3;

        inline void enable_clock() const
        {
            __HAL_RCC_LPUART1_CLK_ENABLE();
        }
    };
}  // namespace valle::platform