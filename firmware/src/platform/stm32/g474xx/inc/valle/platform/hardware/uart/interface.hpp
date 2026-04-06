#pragma once

#include "stm32g4xx_hal.h"
#include "valle/platform/hardware/dma.hpp"
#include "valle/platform/hardware/uart/id.hpp"

namespace valle::platform
{

    // ============================================================================
    // ENUMERATIONS
    // ============================================================================
    struct UartBaudRate : public EnumValue<UartBaudRate, uint32_t>
    {
        static const UartBaudRate kBaud1200;
        static const UartBaudRate kBaud2400;
        static const UartBaudRate kBaud4800;
        static const UartBaudRate kBaud9600;
        static const UartBaudRate kBaud14400;
        static const UartBaudRate kBaud19200;
        static const UartBaudRate kBaud38400;
        static const UartBaudRate kBaud57600;
        static const UartBaudRate kBaud115200;
        static const UartBaudRate kBaud230400;
        static const UartBaudRate kBaud460800;
        static const UartBaudRate kBaud921600;
        static const UartBaudRate kBaud1000000;
        static const UartBaudRate kBaud2000000;
    };

    inline constexpr UartBaudRate UartBaudRate::kBaud1200 = UartBaudRate::from_number<1200>();
    inline constexpr UartBaudRate UartBaudRate::kBaud2400 = UartBaudRate::from_number<2400>();
    inline constexpr UartBaudRate UartBaudRate::kBaud4800 = UartBaudRate::from_number<4800>();
    inline constexpr UartBaudRate UartBaudRate::kBaud9600 = UartBaudRate::from_number<9600>();
    inline constexpr UartBaudRate UartBaudRate::kBaud14400 = UartBaudRate::from_number<14400>();
    inline constexpr UartBaudRate UartBaudRate::kBaud19200 = UartBaudRate::from_number<19200>();
    inline constexpr UartBaudRate UartBaudRate::kBaud38400 = UartBaudRate::from_number<38400>();
    inline constexpr UartBaudRate UartBaudRate::kBaud57600 = UartBaudRate::from_number<57600>();
    inline constexpr UartBaudRate UartBaudRate::kBaud115200 = UartBaudRate::from_number<115200>();
    inline constexpr UartBaudRate UartBaudRate::kBaud230400 = UartBaudRate::from_number<230400>();
    inline constexpr UartBaudRate UartBaudRate::kBaud460800 = UartBaudRate::from_number<460800>();
    inline constexpr UartBaudRate UartBaudRate::kBaud921600 = UartBaudRate::from_number<921600>();
    inline constexpr UartBaudRate UartBaudRate::kBaud1000000 = UartBaudRate::from_number<1000000>();
    inline constexpr UartBaudRate UartBaudRate::kBaud2000000 = UartBaudRate::from_number<2000000>();


    struct UartWordLength : public LLDriverEnumValue<UartWordLength>
    {
        static const UartWordLength kBits7;
        static const UartWordLength kBits8;
        static const UartWordLength kBits9;
    };

    inline constexpr UartWordLength UartWordLength::kBits7 = UartWordLength::from_ll<UART_WORDLENGTH_7B>();
    inline constexpr UartWordLength UartWordLength::kBits8 = UartWordLength::from_ll<UART_WORDLENGTH_8B>();
    inline constexpr UartWordLength UartWordLength::kBits9 = UartWordLength::from_ll<UART_WORDLENGTH_9B>();


    struct UartWakeUpAddressLength : public LLDriverEnumValue<UartWakeUpAddressLength>
    {
        static const UartWakeUpAddressLength kBits4;
        static const UartWakeUpAddressLength kBits7;
    };

    inline constexpr UartWakeUpAddressLength UartWakeUpAddressLength::kBits4 = UartWakeUpAddressLength::from_ll<UART_ADDRESS_DETECT_4B>();
    inline constexpr UartWakeUpAddressLength UartWakeUpAddressLength::kBits7 = UartWakeUpAddressLength::from_ll<UART_ADDRESS_DETECT_7B>();


    struct UartStopBits : public LLDriverEnumValue<UartStopBits>
    {
        static const UartStopBits kBits1;
        static const UartStopBits kBits0_5;
        static const UartStopBits kBits2;
        static const UartStopBits kBits1_5;
    };

    inline constexpr UartStopBits UartStopBits::kBits1 = UartStopBits::from_ll<UART_STOPBITS_1>();
    inline constexpr UartStopBits UartStopBits::kBits0_5 = UartStopBits::from_ll<UART_STOPBITS_0_5>();
    inline constexpr UartStopBits UartStopBits::kBits2 = UartStopBits::from_ll<UART_STOPBITS_2>();
    inline constexpr UartStopBits UartStopBits::kBits1_5 = UartStopBits::from_ll<UART_STOPBITS_1_5>();


    struct UartParity : public LLDriverEnumValue<UartParity>
    {
        static const UartParity kNone;
        static const UartParity kEven;
        static const UartParity kOdd;
    };

    inline constexpr UartParity UartParity::kNone = UartParity::from_ll<UART_PARITY_NONE>();
    inline constexpr UartParity UartParity::kEven = UartParity::from_ll<UART_PARITY_EVEN>();
    inline constexpr UartParity UartParity::kOdd = UartParity::from_ll<UART_PARITY_ODD>();


    struct UartHardwareFlowControl : public LLDriverEnumValue<UartHardwareFlowControl>
    {
        static const UartHardwareFlowControl kNone;
        static const UartHardwareFlowControl kRTS;
        static const UartHardwareFlowControl kCTS;
        static const UartHardwareFlowControl kRTS_CTS;
    };

    inline constexpr UartHardwareFlowControl UartHardwareFlowControl::kNone = UartHardwareFlowControl::from_ll<UART_HWCONTROL_NONE>();
    inline constexpr UartHardwareFlowControl UartHardwareFlowControl::kRTS = UartHardwareFlowControl::from_ll<UART_HWCONTROL_RTS>();
    inline constexpr UartHardwareFlowControl UartHardwareFlowControl::kCTS = UartHardwareFlowControl::from_ll<UART_HWCONTROL_CTS>();
    inline constexpr UartHardwareFlowControl UartHardwareFlowControl::kRTS_CTS = UartHardwareFlowControl::from_ll<UART_HWCONTROL_RTS_CTS>();


    struct UartTransferMode : public LLDriverEnumValue<UartTransferMode>
    {
        static const UartTransferMode kTx;
        static const UartTransferMode kRx;
        static const UartTransferMode kTxRx;
    };

    inline constexpr UartTransferMode UartTransferMode::kTx = UartTransferMode::from_ll<UART_MODE_TX>();
    inline constexpr UartTransferMode UartTransferMode::kRx = UartTransferMode::from_ll<UART_MODE_RX>();
    inline constexpr UartTransferMode UartTransferMode::kTxRx = UartTransferMode::from_ll<UART_MODE_TX_RX>();


    // ===========================================================================
    // HARDWARE TRAITS
    // ===========================================================================
    template <UartControllerId tkControllerId>
    struct UartControllerTraits;

    template <>
    struct UartControllerTraits<UartControllerId::kUSART1>
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
    struct UartControllerTraits<UartControllerId::kUSART2>
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
    struct UartControllerTraits<UartControllerId::kUSART3>
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
    struct UartControllerTraits<UartControllerId::kUart4>
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
    struct UartControllerTraits<UartControllerId::kUart5>
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
    struct UartControllerTraits<UartControllerId::kLPUart1>
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