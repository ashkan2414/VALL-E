#pragma once

#include "valle/platform/hardware/gpio.hpp"
#include "valle/platform/hardware/uart/id.hpp"

namespace valle::platform
{
    enum class UartPinType : uint8_t
    {
        kTx,
        kRx,
        kCK,
        kCTS,
        kRTS,
        kDE,
    };

    // Define a helper to store valid AF mappings
    template <UartControllerId tkControllerId, GpioPortId port, GpioPinId pin, UartPinType tkType>

    struct UartGpioPinAF
    {
        using NoAFDefinedTag = void;
    };

    template <UartControllerId tkControllerId, GpioPortId port, GpioPinId pin, UartPinType tkType>
    concept CValidUartPin =
        requires { typename UartGpioPinAF<tkControllerId, port, pin, tkType>::NoAFDefinedTag; } == false;

    // Register valid pins from the Datasheet (Example for USART1)
#define REGISTER_UART_PIN(uart_id, port_id, pin_id, pin_type, af_id) \
    template <>                                                      \
    struct UartGpioPinAF<uart_id, port_id, pin_id, pin_type>         \
    {                                                                \
        static constexpr GpioAlternativeFunction skAF = af_id;       \
    };

    // USART1
    REGISTER_UART_PIN(UartControllerId::kUSART1,
                      GpioPortId::kPortA,
                      GpioPinId::kPin8,
                      UartPinType::kCK,
                      GpioAlternativeFunction::kAF7);
    REGISTER_UART_PIN(UartControllerId::kUSART1,
                      GpioPortId::kPortA,
                      GpioPinId::kPin9,
                      UartPinType::kTx,
                      GpioAlternativeFunction::kAF7);
    REGISTER_UART_PIN(UartControllerId::kUSART1,
                      GpioPortId::kPortA,
                      GpioPinId::kPin10,
                      UartPinType::kRx,
                      GpioAlternativeFunction::kAF7);
    REGISTER_UART_PIN(UartControllerId::kUSART1,
                      GpioPortId::kPortA,
                      GpioPinId::kPin11,
                      UartPinType::kCTS,
                      GpioAlternativeFunction::kAF7);
    REGISTER_UART_PIN(UartControllerId::kUSART1,
                      GpioPortId::kPortA,
                      GpioPinId::kPin12,
                      UartPinType::kRTS,
                      GpioAlternativeFunction::kAF7);
    REGISTER_UART_PIN(UartControllerId::kUSART1,
                      GpioPortId::kPortA,
                      GpioPinId::kPin12,
                      UartPinType::kDE,
                      GpioAlternativeFunction::kAF7);

    REGISTER_UART_PIN(UartControllerId::kUSART1,
                      GpioPortId::kPortB,
                      GpioPinId::kPin6,
                      UartPinType::kTx,
                      GpioAlternativeFunction::kAF7);
    REGISTER_UART_PIN(UartControllerId::kUSART1,
                      GpioPortId::kPortB,
                      GpioPinId::kPin7,
                      UartPinType::kRx,
                      GpioAlternativeFunction::kAF7);

    REGISTER_UART_PIN(UartControllerId::kUSART1,
                      GpioPortId::kPortC,
                      GpioPinId::kPin4,
                      UartPinType::kTx,
                      GpioAlternativeFunction::kAF7);
    REGISTER_UART_PIN(UartControllerId::kUSART1,
                      GpioPortId::kPortC,
                      GpioPinId::kPin5,
                      UartPinType::kRx,
                      GpioAlternativeFunction::kAF7);

    REGISTER_UART_PIN(UartControllerId::kUSART1,
                      GpioPortId::kPortE,
                      GpioPinId::kPin0,
                      UartPinType::kTx,
                      GpioAlternativeFunction::kAF7);
    REGISTER_UART_PIN(UartControllerId::kUSART1,
                      GpioPortId::kPortE,
                      GpioPinId::kPin1,
                      UartPinType::kRx,
                      GpioAlternativeFunction::kAF7);

    REGISTER_UART_PIN(UartControllerId::kUSART1,
                      GpioPortId::kPortG,
                      GpioPinId::kPin9,
                      UartPinType::kTx,
                      GpioAlternativeFunction::kAF7);

    // USART2
    REGISTER_UART_PIN(UartControllerId::kUSART2,
                      GpioPortId::kPortA,
                      GpioPinId::kPin0,
                      UartPinType::kCTS,
                      GpioAlternativeFunction::kAF7);
    REGISTER_UART_PIN(UartControllerId::kUSART2,
                      GpioPortId::kPortA,
                      GpioPinId::kPin1,
                      UartPinType::kRTS,
                      GpioAlternativeFunction::kAF7);
    REGISTER_UART_PIN(UartControllerId::kUSART2,
                      GpioPortId::kPortA,
                      GpioPinId::kPin1,
                      UartPinType::kDE,
                      GpioAlternativeFunction::kAF7);
    REGISTER_UART_PIN(UartControllerId::kUSART2,
                      GpioPortId::kPortA,
                      GpioPinId::kPin2,
                      UartPinType::kTx,
                      GpioAlternativeFunction::kAF7);
    REGISTER_UART_PIN(UartControllerId::kUSART2,
                      GpioPortId::kPortA,
                      GpioPinId::kPin3,
                      UartPinType::kRx,
                      GpioAlternativeFunction::kAF7);
    REGISTER_UART_PIN(UartControllerId::kUSART2,
                      GpioPortId::kPortA,
                      GpioPinId::kPin4,
                      UartPinType::kCK,
                      GpioAlternativeFunction::kAF7);
    REGISTER_UART_PIN(UartControllerId::kUSART2,
                      GpioPortId::kPortA,
                      GpioPinId::kPin14,
                      UartPinType::kTx,
                      GpioAlternativeFunction::kAF7);
    REGISTER_UART_PIN(UartControllerId::kUSART2,
                      GpioPortId::kPortA,
                      GpioPinId::kPin15,
                      UartPinType::kRx,
                      GpioAlternativeFunction::kAF7);

    REGISTER_UART_PIN(UartControllerId::kUSART2,
                      GpioPortId::kPortB,
                      GpioPinId::kPin3,
                      UartPinType::kTx,
                      GpioAlternativeFunction::kAF7);
    REGISTER_UART_PIN(UartControllerId::kUSART2,
                      GpioPortId::kPortB,
                      GpioPinId::kPin4,
                      UartPinType::kRx,
                      GpioAlternativeFunction::kAF7);
    REGISTER_UART_PIN(UartControllerId::kUSART2,
                      GpioPortId::kPortB,
                      GpioPinId::kPin5,
                      UartPinType::kCK,
                      GpioAlternativeFunction::kAF7);

    REGISTER_UART_PIN(UartControllerId::kUSART2,
                      GpioPortId::kPortD,
                      GpioPinId::kPin3,
                      UartPinType::kCTS,
                      GpioAlternativeFunction::kAF7);
    REGISTER_UART_PIN(UartControllerId::kUSART2,
                      GpioPortId::kPortD,
                      GpioPinId::kPin4,
                      UartPinType::kRTS,
                      GpioAlternativeFunction::kAF7);
    REGISTER_UART_PIN(UartControllerId::kUSART2,
                      GpioPortId::kPortD,
                      GpioPinId::kPin4,
                      UartPinType::kDE,
                      GpioAlternativeFunction::kAF7);
    REGISTER_UART_PIN(UartControllerId::kUSART2,
                      GpioPortId::kPortD,
                      GpioPinId::kPin5,
                      UartPinType::kTx,
                      GpioAlternativeFunction::kAF7);
    REGISTER_UART_PIN(UartControllerId::kUSART2,
                      GpioPortId::kPortD,
                      GpioPinId::kPin6,
                      UartPinType::kRx,
                      GpioAlternativeFunction::kAF7);
    REGISTER_UART_PIN(UartControllerId::kUSART2,
                      GpioPortId::kPortD,
                      GpioPinId::kPin7,
                      UartPinType::kCK,
                      GpioAlternativeFunction::kAF7);

    // USART3
    REGISTER_UART_PIN(UartControllerId::kUSART3,
                      GpioPortId::kPortA,
                      GpioPinId::kPin13,
                      UartPinType::kCTS,
                      GpioAlternativeFunction::kAF7);

    REGISTER_UART_PIN(UartControllerId::kUSART3,
                      GpioPortId::kPortB,
                      GpioPinId::kPin8,
                      UartPinType::kRx,
                      GpioAlternativeFunction::kAF7);
    REGISTER_UART_PIN(UartControllerId::kUSART3,
                      GpioPortId::kPortB,
                      GpioPinId::kPin9,
                      UartPinType::kTx,
                      GpioAlternativeFunction::kAF7);
    REGISTER_UART_PIN(UartControllerId::kUSART3,
                      GpioPortId::kPortB,
                      GpioPinId::kPin10,
                      UartPinType::kTx,
                      GpioAlternativeFunction::kAF7);
    REGISTER_UART_PIN(UartControllerId::kUSART3,
                      GpioPortId::kPortB,
                      GpioPinId::kPin11,
                      UartPinType::kRx,
                      GpioAlternativeFunction::kAF7);
    REGISTER_UART_PIN(UartControllerId::kUSART3,
                      GpioPortId::kPortB,
                      GpioPinId::kPin12,
                      UartPinType::kCK,
                      GpioAlternativeFunction::kAF7);
    REGISTER_UART_PIN(UartControllerId::kUSART3,
                      GpioPortId::kPortB,
                      GpioPinId::kPin13,
                      UartPinType::kCTS,
                      GpioAlternativeFunction::kAF7);
    REGISTER_UART_PIN(UartControllerId::kUSART3,
                      GpioPortId::kPortB,
                      GpioPinId::kPin14,
                      UartPinType::kRTS,
                      GpioAlternativeFunction::kAF7);
    REGISTER_UART_PIN(UartControllerId::kUSART3,
                      GpioPortId::kPortB,
                      GpioPinId::kPin14,
                      UartPinType::kDE,
                      GpioAlternativeFunction::kAF7);

    REGISTER_UART_PIN(UartControllerId::kUSART3,
                      GpioPortId::kPortC,
                      GpioPinId::kPin10,
                      UartPinType::kTx,
                      GpioAlternativeFunction::kAF7);
    REGISTER_UART_PIN(UartControllerId::kUSART3,
                      GpioPortId::kPortC,
                      GpioPinId::kPin11,
                      UartPinType::kRx,
                      GpioAlternativeFunction::kAF7);
    REGISTER_UART_PIN(UartControllerId::kUSART3,
                      GpioPortId::kPortC,
                      GpioPinId::kPin12,
                      UartPinType::kCK,
                      GpioAlternativeFunction::kAF7);

    REGISTER_UART_PIN(UartControllerId::kUSART3,
                      GpioPortId::kPortD,
                      GpioPinId::kPin8,
                      UartPinType::kTx,
                      GpioAlternativeFunction::kAF7);
    REGISTER_UART_PIN(UartControllerId::kUSART3,
                      GpioPortId::kPortD,
                      GpioPinId::kPin9,
                      UartPinType::kRx,
                      GpioAlternativeFunction::kAF7);
    REGISTER_UART_PIN(UartControllerId::kUSART3,
                      GpioPortId::kPortD,
                      GpioPinId::kPin10,
                      UartPinType::kCK,
                      GpioAlternativeFunction::kAF7);
    REGISTER_UART_PIN(UartControllerId::kUSART3,
                      GpioPortId::kPortD,
                      GpioPinId::kPin11,
                      UartPinType::kCTS,
                      GpioAlternativeFunction::kAF7);
    REGISTER_UART_PIN(UartControllerId::kUSART3,
                      GpioPortId::kPortD,
                      GpioPinId::kPin12,
                      UartPinType::kRTS,
                      GpioAlternativeFunction::kAF7);
    REGISTER_UART_PIN(UartControllerId::kUSART3,
                      GpioPortId::kPortD,
                      GpioPinId::kPin12,
                      UartPinType::kDE,
                      GpioAlternativeFunction::kAF7);

    REGISTER_UART_PIN(UartControllerId::kUSART3,
                      GpioPortId::kPortE,
                      GpioPinId::kPin15,
                      UartPinType::kRx,
                      GpioAlternativeFunction::kAF7);

    REGISTER_UART_PIN(UartControllerId::kUSART3,
                      GpioPortId::kPortF,
                      GpioPinId::kPin6,
                      UartPinType::kRTS,
                      GpioAlternativeFunction::kAF7);

    // Uart4
    REGISTER_UART_PIN(UartControllerId::kUart4,
                      GpioPortId::kPortA,
                      GpioPinId::kPin15,
                      UartPinType::kRTS,
                      GpioAlternativeFunction::kAF8);
    REGISTER_UART_PIN(UartControllerId::kUart4,
                      GpioPortId::kPortA,
                      GpioPinId::kPin15,
                      UartPinType::kDE,
                      GpioAlternativeFunction::kAF8);

    REGISTER_UART_PIN(UartControllerId::kUart4,
                      GpioPortId::kPortB,
                      GpioPinId::kPin7,
                      UartPinType::kCTS,
                      GpioAlternativeFunction::kAF14);

    REGISTER_UART_PIN(UartControllerId::kUart4,
                      GpioPortId::kPortC,
                      GpioPinId::kPin10,
                      UartPinType::kTx,
                      GpioAlternativeFunction::kAF5);
    REGISTER_UART_PIN(UartControllerId::kUart4,
                      GpioPortId::kPortC,
                      GpioPinId::kPin11,
                      UartPinType::kRx,
                      GpioAlternativeFunction::kAF5);

    // Uart5
    REGISTER_UART_PIN(UartControllerId::kUart5,
                      GpioPortId::kPortB,
                      GpioPinId::kPin4,
                      UartPinType::kRTS,
                      GpioAlternativeFunction::kAF8);
    REGISTER_UART_PIN(UartControllerId::kUart5,
                      GpioPortId::kPortB,
                      GpioPinId::kPin4,
                      UartPinType::kDE,
                      GpioAlternativeFunction::kAF8);
    REGISTER_UART_PIN(UartControllerId::kUart5,
                      GpioPortId::kPortB,
                      GpioPinId::kPin5,
                      UartPinType::kCTS,
                      GpioAlternativeFunction::kAF14);

    REGISTER_UART_PIN(UartControllerId::kUart5,
                      GpioPortId::kPortC,
                      GpioPinId::kPin12,
                      UartPinType::kTx,
                      GpioAlternativeFunction::kAF5);

    REGISTER_UART_PIN(UartControllerId::kUart5,
                      GpioPortId::kPortD,
                      GpioPinId::kPin2,
                      UartPinType::kRx,
                      GpioAlternativeFunction::kAF5);

    // LPUart1
    REGISTER_UART_PIN(UartControllerId::kLPUart1,
                      GpioPortId::kPortA,
                      GpioPinId::kPin2,
                      UartPinType::kTx,
                      GpioAlternativeFunction::kAF12);
    REGISTER_UART_PIN(UartControllerId::kLPUart1,
                      GpioPortId::kPortA,
                      GpioPinId::kPin3,
                      UartPinType::kRx,
                      GpioAlternativeFunction::kAF12);
    REGISTER_UART_PIN(UartControllerId::kLPUart1,
                      GpioPortId::kPortA,
                      GpioPinId::kPin6,
                      UartPinType::kCTS,
                      GpioAlternativeFunction::kAF12);

    REGISTER_UART_PIN(UartControllerId::kLPUart1,
                      GpioPortId::kPortB,
                      GpioPinId::kPin1,
                      UartPinType::kRTS,
                      GpioAlternativeFunction::kAF12);
    REGISTER_UART_PIN(UartControllerId::kLPUart1,
                      GpioPortId::kPortB,
                      GpioPinId::kPin1,
                      UartPinType::kDE,
                      GpioAlternativeFunction::kAF12);
    REGISTER_UART_PIN(UartControllerId::kLPUart1,
                      GpioPortId::kPortB,
                      GpioPinId::kPin10,
                      UartPinType::kRx,
                      GpioAlternativeFunction::kAF8);
    REGISTER_UART_PIN(UartControllerId::kLPUart1,
                      GpioPortId::kPortB,
                      GpioPinId::kPin11,
                      UartPinType::kTx,
                      GpioAlternativeFunction::kAF8);
    REGISTER_UART_PIN(UartControllerId::kLPUart1,
                      GpioPortId::kPortB,
                      GpioPinId::kPin12,
                      UartPinType::kRTS,
                      GpioAlternativeFunction::kAF8);
    REGISTER_UART_PIN(UartControllerId::kLPUart1,
                      GpioPortId::kPortB,
                      GpioPinId::kPin12,
                      UartPinType::kDE,
                      GpioAlternativeFunction::kAF8);
    REGISTER_UART_PIN(UartControllerId::kLPUart1,
                      GpioPortId::kPortB,
                      GpioPinId::kPin13,
                      UartPinType::kCTS,
                      GpioAlternativeFunction::kAF8);

    REGISTER_UART_PIN(UartControllerId::kLPUart1,
                      GpioPortId::kPortC,
                      GpioPinId::kPin0,
                      UartPinType::kRx,
                      GpioAlternativeFunction::kAF8);
    REGISTER_UART_PIN(UartControllerId::kLPUart1,
                      GpioPortId::kPortC,
                      GpioPinId::kPin1,
                      UartPinType::kTx,
                      GpioAlternativeFunction::kAF8);

    REGISTER_UART_PIN(UartControllerId::kLPUart1,
                      GpioPortId::kPortG,
                      GpioPinId::kPin5,
                      UartPinType::kCTS,
                      GpioAlternativeFunction::kAF8);
    REGISTER_UART_PIN(UartControllerId::kLPUart1,
                      GpioPortId::kPortG,
                      GpioPinId::kPin6,
                      UartPinType::kRTS,
                      GpioAlternativeFunction::kAF8);
    REGISTER_UART_PIN(UartControllerId::kLPUart1,
                      GpioPortId::kPortG,
                      GpioPinId::kPin6,
                      UartPinType::kDE,
                      GpioAlternativeFunction::kAF8);
    REGISTER_UART_PIN(UartControllerId::kLPUart1,
                      GpioPortId::kPortG,
                      GpioPinId::kPin7,
                      UartPinType::kTx,
                      GpioAlternativeFunction::kAF8);
    REGISTER_UART_PIN(UartControllerId::kLPUart1,
                      GpioPortId::kPortG,
                      GpioPinId::kPin8,
                      UartPinType::kRx,
                      GpioAlternativeFunction::kAF8);

}  // namespace valle::platform
