#pragma once

#include "valle/platform/hardware/gpio.hpp"
#include "valle/platform/hardware/uart/id.hpp"

namespace valle::platform
{
    enum class UARTPinType : uint8_t
    {
        kTx,
        kRx,
        kCK,
        kCTS,
        kRTS,
        kDE,
    };

    // Define a helper to store valid AF mappings
    template <UARTControllerID tkID, GPIOPortID port, GPIOPinID pin, UARTPinType tkType>

    struct UARTGPIOPinAF
    {
        using NoAFDefinedTag = void;
    };

    template <UARTControllerID tkID, GPIOPortID port, GPIOPinID pin, UARTPinType tkType>
    concept CValidUARTPin = requires { typename UARTGPIOPinAF<tkID, port, pin, tkType>::NoAFDefinedTag; } == false;

    // Register valid pins from the Datasheet (Example for USART1)
#define REGISTER_UART_PIN(uart_id, port_id, pin_id, pin_type, af_id) \
    template <>                                                      \
    struct UARTGPIOPinAF<uart_id, port_id, pin_id, pin_type>         \
    {                                                                \
        static constexpr GPIOAlternativeFunction skAF = af_id;       \
    };

    // USART1
    REGISTER_UART_PIN(UARTControllerID::kUSART1,
                      GPIOPortID::kPortA,
                      GPIOPinID::kPin8,
                      UARTPinType::kCK,
                      GPIOAlternativeFunction::kAF7);
    REGISTER_UART_PIN(UARTControllerID::kUSART1,
                      GPIOPortID::kPortA,
                      GPIOPinID::kPin9,
                      UARTPinType::kTx,
                      GPIOAlternativeFunction::kAF7);
    REGISTER_UART_PIN(UARTControllerID::kUSART1,
                      GPIOPortID::kPortA,
                      GPIOPinID::kPin10,
                      UARTPinType::kRx,
                      GPIOAlternativeFunction::kAF7);
    REGISTER_UART_PIN(UARTControllerID::kUSART1,
                      GPIOPortID::kPortA,
                      GPIOPinID::kPin11,
                      UARTPinType::kCTS,
                      GPIOAlternativeFunction::kAF7);
    REGISTER_UART_PIN(UARTControllerID::kUSART1,
                      GPIOPortID::kPortA,
                      GPIOPinID::kPin12,
                      UARTPinType::kRTS,
                      GPIOAlternativeFunction::kAF7);
    REGISTER_UART_PIN(UARTControllerID::kUSART1,
                      GPIOPortID::kPortA,
                      GPIOPinID::kPin12,
                      UARTPinType::kDE,
                      GPIOAlternativeFunction::kAF7);

    REGISTER_UART_PIN(UARTControllerID::kUSART1,
                      GPIOPortID::kPortB,
                      GPIOPinID::kPin6,
                      UARTPinType::kTx,
                      GPIOAlternativeFunction::kAF7);
    REGISTER_UART_PIN(UARTControllerID::kUSART1,
                      GPIOPortID::kPortB,
                      GPIOPinID::kPin7,
                      UARTPinType::kRx,
                      GPIOAlternativeFunction::kAF7);

    REGISTER_UART_PIN(UARTControllerID::kUSART1,
                      GPIOPortID::kPortC,
                      GPIOPinID::kPin4,
                      UARTPinType::kTx,
                      GPIOAlternativeFunction::kAF7);
    REGISTER_UART_PIN(UARTControllerID::kUSART1,
                      GPIOPortID::kPortC,
                      GPIOPinID::kPin5,
                      UARTPinType::kRx,
                      GPIOAlternativeFunction::kAF7);

    REGISTER_UART_PIN(UARTControllerID::kUSART1,
                      GPIOPortID::kPortE,
                      GPIOPinID::kPin0,
                      UARTPinType::kTx,
                      GPIOAlternativeFunction::kAF7);
    REGISTER_UART_PIN(UARTControllerID::kUSART1,
                      GPIOPortID::kPortE,
                      GPIOPinID::kPin1,
                      UARTPinType::kRx,
                      GPIOAlternativeFunction::kAF7);

    REGISTER_UART_PIN(UARTControllerID::kUSART1,
                      GPIOPortID::kPortG,
                      GPIOPinID::kPin9,
                      UARTPinType::kTx,
                      GPIOAlternativeFunction::kAF7);

    // USART2
    REGISTER_UART_PIN(UARTControllerID::kUSART2,
                      GPIOPortID::kPortA,
                      GPIOPinID::kPin0,
                      UARTPinType::kCTS,
                      GPIOAlternativeFunction::kAF7);
    REGISTER_UART_PIN(UARTControllerID::kUSART2,
                      GPIOPortID::kPortA,
                      GPIOPinID::kPin1,
                      UARTPinType::kRTS,
                      GPIOAlternativeFunction::kAF7);
    REGISTER_UART_PIN(UARTControllerID::kUSART2,
                      GPIOPortID::kPortA,
                      GPIOPinID::kPin1,
                      UARTPinType::kDE,
                      GPIOAlternativeFunction::kAF7);
    REGISTER_UART_PIN(UARTControllerID::kUSART2,
                      GPIOPortID::kPortA,
                      GPIOPinID::kPin2,
                      UARTPinType::kTx,
                      GPIOAlternativeFunction::kAF7);
    REGISTER_UART_PIN(UARTControllerID::kUSART2,
                      GPIOPortID::kPortA,
                      GPIOPinID::kPin3,
                      UARTPinType::kRx,
                      GPIOAlternativeFunction::kAF7);
    REGISTER_UART_PIN(UARTControllerID::kUSART2,
                      GPIOPortID::kPortA,
                      GPIOPinID::kPin4,
                      UARTPinType::kCK,
                      GPIOAlternativeFunction::kAF7);
    REGISTER_UART_PIN(UARTControllerID::kUSART2,
                      GPIOPortID::kPortA,
                      GPIOPinID::kPin14,
                      UARTPinType::kTx,
                      GPIOAlternativeFunction::kAF7);
    REGISTER_UART_PIN(UARTControllerID::kUSART2,
                      GPIOPortID::kPortA,
                      GPIOPinID::kPin15,
                      UARTPinType::kRx,
                      GPIOAlternativeFunction::kAF7);

    REGISTER_UART_PIN(UARTControllerID::kUSART2,
                      GPIOPortID::kPortB,
                      GPIOPinID::kPin3,
                      UARTPinType::kTx,
                      GPIOAlternativeFunction::kAF7);
    REGISTER_UART_PIN(UARTControllerID::kUSART2,
                      GPIOPortID::kPortB,
                      GPIOPinID::kPin4,
                      UARTPinType::kRx,
                      GPIOAlternativeFunction::kAF7);
    REGISTER_UART_PIN(UARTControllerID::kUSART2,
                      GPIOPortID::kPortB,
                      GPIOPinID::kPin5,
                      UARTPinType::kCK,
                      GPIOAlternativeFunction::kAF7);

    REGISTER_UART_PIN(UARTControllerID::kUSART2,
                      GPIOPortID::kPortD,
                      GPIOPinID::kPin3,
                      UARTPinType::kCTS,
                      GPIOAlternativeFunction::kAF7);
    REGISTER_UART_PIN(UARTControllerID::kUSART2,
                      GPIOPortID::kPortD,
                      GPIOPinID::kPin4,
                      UARTPinType::kRTS,
                      GPIOAlternativeFunction::kAF7);
    REGISTER_UART_PIN(UARTControllerID::kUSART2,
                      GPIOPortID::kPortD,
                      GPIOPinID::kPin4,
                      UARTPinType::kDE,
                      GPIOAlternativeFunction::kAF7);
    REGISTER_UART_PIN(UARTControllerID::kUSART2,
                      GPIOPortID::kPortD,
                      GPIOPinID::kPin5,
                      UARTPinType::kTx,
                      GPIOAlternativeFunction::kAF7);
    REGISTER_UART_PIN(UARTControllerID::kUSART2,
                      GPIOPortID::kPortD,
                      GPIOPinID::kPin6,
                      UARTPinType::kRx,
                      GPIOAlternativeFunction::kAF7);
    REGISTER_UART_PIN(UARTControllerID::kUSART2,
                      GPIOPortID::kPortD,
                      GPIOPinID::kPin7,
                      UARTPinType::kCK,
                      GPIOAlternativeFunction::kAF7);

    // USART3
    REGISTER_UART_PIN(UARTControllerID::kUSART3,
                      GPIOPortID::kPortA,
                      GPIOPinID::kPin13,
                      UARTPinType::kCTS,
                      GPIOAlternativeFunction::kAF7);

    REGISTER_UART_PIN(UARTControllerID::kUSART3,
                      GPIOPortID::kPortB,
                      GPIOPinID::kPin8,
                      UARTPinType::kRx,
                      GPIOAlternativeFunction::kAF7);
    REGISTER_UART_PIN(UARTControllerID::kUSART3,
                      GPIOPortID::kPortB,
                      GPIOPinID::kPin9,
                      UARTPinType::kTx,
                      GPIOAlternativeFunction::kAF7);
    REGISTER_UART_PIN(UARTControllerID::kUSART3,
                      GPIOPortID::kPortB,
                      GPIOPinID::kPin10,
                      UARTPinType::kTx,
                      GPIOAlternativeFunction::kAF7);
    REGISTER_UART_PIN(UARTControllerID::kUSART3,
                      GPIOPortID::kPortB,
                      GPIOPinID::kPin11,
                      UARTPinType::kRx,
                      GPIOAlternativeFunction::kAF7);
    REGISTER_UART_PIN(UARTControllerID::kUSART3,
                      GPIOPortID::kPortB,
                      GPIOPinID::kPin12,
                      UARTPinType::kCK,
                      GPIOAlternativeFunction::kAF7);
    REGISTER_UART_PIN(UARTControllerID::kUSART3,
                      GPIOPortID::kPortB,
                      GPIOPinID::kPin13,
                      UARTPinType::kCTS,
                      GPIOAlternativeFunction::kAF7);
    REGISTER_UART_PIN(UARTControllerID::kUSART3,
                      GPIOPortID::kPortB,
                      GPIOPinID::kPin14,
                      UARTPinType::kRTS,
                      GPIOAlternativeFunction::kAF7);
    REGISTER_UART_PIN(UARTControllerID::kUSART3,
                      GPIOPortID::kPortB,
                      GPIOPinID::kPin14,
                      UARTPinType::kDE,
                      GPIOAlternativeFunction::kAF7);

    REGISTER_UART_PIN(UARTControllerID::kUSART3,
                      GPIOPortID::kPortC,
                      GPIOPinID::kPin10,
                      UARTPinType::kTx,
                      GPIOAlternativeFunction::kAF7);
    REGISTER_UART_PIN(UARTControllerID::kUSART3,
                      GPIOPortID::kPortC,
                      GPIOPinID::kPin11,
                      UARTPinType::kRx,
                      GPIOAlternativeFunction::kAF7);
    REGISTER_UART_PIN(UARTControllerID::kUSART3,
                      GPIOPortID::kPortC,
                      GPIOPinID::kPin12,
                      UARTPinType::kCK,
                      GPIOAlternativeFunction::kAF7);

    REGISTER_UART_PIN(UARTControllerID::kUSART3,
                      GPIOPortID::kPortD,
                      GPIOPinID::kPin8,
                      UARTPinType::kTx,
                      GPIOAlternativeFunction::kAF7);
    REGISTER_UART_PIN(UARTControllerID::kUSART3,
                      GPIOPortID::kPortD,
                      GPIOPinID::kPin9,
                      UARTPinType::kRx,
                      GPIOAlternativeFunction::kAF7);
    REGISTER_UART_PIN(UARTControllerID::kUSART3,
                      GPIOPortID::kPortD,
                      GPIOPinID::kPin10,
                      UARTPinType::kCK,
                      GPIOAlternativeFunction::kAF7);
    REGISTER_UART_PIN(UARTControllerID::kUSART3,
                      GPIOPortID::kPortD,
                      GPIOPinID::kPin11,
                      UARTPinType::kCTS,
                      GPIOAlternativeFunction::kAF7);
    REGISTER_UART_PIN(UARTControllerID::kUSART3,
                      GPIOPortID::kPortD,
                      GPIOPinID::kPin12,
                      UARTPinType::kRTS,
                      GPIOAlternativeFunction::kAF7);
    REGISTER_UART_PIN(UARTControllerID::kUSART3,
                      GPIOPortID::kPortD,
                      GPIOPinID::kPin12,
                      UARTPinType::kDE,
                      GPIOAlternativeFunction::kAF7);

    REGISTER_UART_PIN(UARTControllerID::kUSART3,
                      GPIOPortID::kPortE,
                      GPIOPinID::kPin15,
                      UARTPinType::kRx,
                      GPIOAlternativeFunction::kAF7);

    REGISTER_UART_PIN(UARTControllerID::kUSART3,
                      GPIOPortID::kPortF,
                      GPIOPinID::kPin6,
                      UARTPinType::kRTS,
                      GPIOAlternativeFunction::kAF7);

    // UART4
    REGISTER_UART_PIN(UARTControllerID::kUART4,
                      GPIOPortID::kPortA,
                      GPIOPinID::kPin15,
                      UARTPinType::kRTS,
                      GPIOAlternativeFunction::kAF8);
    REGISTER_UART_PIN(UARTControllerID::kUART4,
                      GPIOPortID::kPortA,
                      GPIOPinID::kPin15,
                      UARTPinType::kDE,
                      GPIOAlternativeFunction::kAF8);

    REGISTER_UART_PIN(UARTControllerID::kUART4,
                      GPIOPortID::kPortB,
                      GPIOPinID::kPin7,
                      UARTPinType::kCTS,
                      GPIOAlternativeFunction::kAF14);

    REGISTER_UART_PIN(UARTControllerID::kUART4,
                      GPIOPortID::kPortC,
                      GPIOPinID::kPin10,
                      UARTPinType::kTx,
                      GPIOAlternativeFunction::kAF5);
    REGISTER_UART_PIN(UARTControllerID::kUART4,
                      GPIOPortID::kPortC,
                      GPIOPinID::kPin11,
                      UARTPinType::kRx,
                      GPIOAlternativeFunction::kAF5);

    // UART5
    REGISTER_UART_PIN(UARTControllerID::kUART5,
                      GPIOPortID::kPortB,
                      GPIOPinID::kPin4,
                      UARTPinType::kRTS,
                      GPIOAlternativeFunction::kAF8);
    REGISTER_UART_PIN(UARTControllerID::kUART5,
                      GPIOPortID::kPortB,
                      GPIOPinID::kPin4,
                      UARTPinType::kDE,
                      GPIOAlternativeFunction::kAF8);
    REGISTER_UART_PIN(UARTControllerID::kUART5,
                      GPIOPortID::kPortB,
                      GPIOPinID::kPin5,
                      UARTPinType::kCTS,
                      GPIOAlternativeFunction::kAF14);

    REGISTER_UART_PIN(UARTControllerID::kUART5,
                      GPIOPortID::kPortC,
                      GPIOPinID::kPin12,
                      UARTPinType::kTx,
                      GPIOAlternativeFunction::kAF5);

    REGISTER_UART_PIN(UARTControllerID::kUART5,
                      GPIOPortID::kPortD,
                      GPIOPinID::kPin2,
                      UARTPinType::kRx,
                      GPIOAlternativeFunction::kAF5);

    // LPUART1
    REGISTER_UART_PIN(UARTControllerID::kLPUART1,
                      GPIOPortID::kPortA,
                      GPIOPinID::kPin2,
                      UARTPinType::kTx,
                      GPIOAlternativeFunction::kAF12);
    REGISTER_UART_PIN(UARTControllerID::kLPUART1,
                      GPIOPortID::kPortA,
                      GPIOPinID::kPin3,
                      UARTPinType::kRx,
                      GPIOAlternativeFunction::kAF12);
    REGISTER_UART_PIN(UARTControllerID::kLPUART1,
                      GPIOPortID::kPortA,
                      GPIOPinID::kPin6,
                      UARTPinType::kCTS,
                      GPIOAlternativeFunction::kAF12);

    REGISTER_UART_PIN(UARTControllerID::kLPUART1,
                      GPIOPortID::kPortB,
                      GPIOPinID::kPin1,
                      UARTPinType::kRTS,
                      GPIOAlternativeFunction::kAF12);
    REGISTER_UART_PIN(UARTControllerID::kLPUART1,
                      GPIOPortID::kPortB,
                      GPIOPinID::kPin1,
                      UARTPinType::kDE,
                      GPIOAlternativeFunction::kAF12);
    REGISTER_UART_PIN(UARTControllerID::kLPUART1,
                      GPIOPortID::kPortB,
                      GPIOPinID::kPin10,
                      UARTPinType::kRx,
                      GPIOAlternativeFunction::kAF8);
    REGISTER_UART_PIN(UARTControllerID::kLPUART1,
                      GPIOPortID::kPortB,
                      GPIOPinID::kPin11,
                      UARTPinType::kTx,
                      GPIOAlternativeFunction::kAF8);
    REGISTER_UART_PIN(UARTControllerID::kLPUART1,
                      GPIOPortID::kPortB,
                      GPIOPinID::kPin12,
                      UARTPinType::kRTS,
                      GPIOAlternativeFunction::kAF8);
    REGISTER_UART_PIN(UARTControllerID::kLPUART1,
                      GPIOPortID::kPortB,
                      GPIOPinID::kPin12,
                      UARTPinType::kDE,
                      GPIOAlternativeFunction::kAF8);
    REGISTER_UART_PIN(UARTControllerID::kLPUART1,
                      GPIOPortID::kPortB,
                      GPIOPinID::kPin13,
                      UARTPinType::kCTS,
                      GPIOAlternativeFunction::kAF8);

    REGISTER_UART_PIN(UARTControllerID::kLPUART1,
                      GPIOPortID::kPortC,
                      GPIOPinID::kPin0,
                      UARTPinType::kRx,
                      GPIOAlternativeFunction::kAF8);
    REGISTER_UART_PIN(UARTControllerID::kLPUART1,
                      GPIOPortID::kPortC,
                      GPIOPinID::kPin1,
                      UARTPinType::kTx,
                      GPIOAlternativeFunction::kAF8);

    REGISTER_UART_PIN(UARTControllerID::kLPUART1,
                      GPIOPortID::kPortG,
                      GPIOPinID::kPin5,
                      UARTPinType::kCTS,
                      GPIOAlternativeFunction::kAF8);
    REGISTER_UART_PIN(UARTControllerID::kLPUART1,
                      GPIOPortID::kPortG,
                      GPIOPinID::kPin6,
                      UARTPinType::kRTS,
                      GPIOAlternativeFunction::kAF8);
    REGISTER_UART_PIN(UARTControllerID::kLPUART1,
                      GPIOPortID::kPortG,
                      GPIOPinID::kPin6,
                      UARTPinType::kDE,
                      GPIOAlternativeFunction::kAF8);
    REGISTER_UART_PIN(UARTControllerID::kLPUART1,
                      GPIOPortID::kPortG,
                      GPIOPinID::kPin7,
                      UARTPinType::kTx,
                      GPIOAlternativeFunction::kAF8);
    REGISTER_UART_PIN(UARTControllerID::kLPUART1,
                      GPIOPortID::kPortG,
                      GPIOPinID::kPin8,
                      UARTPinType::kRx,
                      GPIOAlternativeFunction::kAF8);

}  // namespace valle::platform
