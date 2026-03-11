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
        requires(kValidGPIOPinID<port, pin>)
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
    REGISTER_UART_PIN(UARTControllerID::kUSART1, GPIOPortID::kA, 8, UARTPinType::kCK, GPIOAlternativeFunction::kAF7);
    REGISTER_UART_PIN(UARTControllerID::kUSART1, GPIOPortID::kA, 9, UARTPinType::kTx, GPIOAlternativeFunction::kAF7);
    REGISTER_UART_PIN(UARTControllerID::kUSART1, GPIOPortID::kA, 10, UARTPinType::kRx, GPIOAlternativeFunction::kAF7);
    REGISTER_UART_PIN(UARTControllerID::kUSART1, GPIOPortID::kA, 11, UARTPinType::kCTS, GPIOAlternativeFunction::kAF7);
    REGISTER_UART_PIN(UARTControllerID::kUSART1, GPIOPortID::kA, 12, UARTPinType::kRTS, GPIOAlternativeFunction::kAF7);
    REGISTER_UART_PIN(UARTControllerID::kUSART1, GPIOPortID::kA, 12, UARTPinType::kDE, GPIOAlternativeFunction::kAF7);

    REGISTER_UART_PIN(UARTControllerID::kUSART1, GPIOPortID::kB, 6, UARTPinType::kTx, GPIOAlternativeFunction::kAF7);
    REGISTER_UART_PIN(UARTControllerID::kUSART1, GPIOPortID::kB, 7, UARTPinType::kRx, GPIOAlternativeFunction::kAF7);

    REGISTER_UART_PIN(UARTControllerID::kUSART1, GPIOPortID::kC, 4, UARTPinType::kTx, GPIOAlternativeFunction::kAF7);
    REGISTER_UART_PIN(UARTControllerID::kUSART1, GPIOPortID::kC, 5, UARTPinType::kRx, GPIOAlternativeFunction::kAF7);

    REGISTER_UART_PIN(UARTControllerID::kUSART1, GPIOPortID::kE, 0, UARTPinType::kTx, GPIOAlternativeFunction::kAF7);
    REGISTER_UART_PIN(UARTControllerID::kUSART1, GPIOPortID::kE, 1, UARTPinType::kRx, GPIOAlternativeFunction::kAF7);

    REGISTER_UART_PIN(UARTControllerID::kUSART1, GPIOPortID::kG, 9, UARTPinType::kTx, GPIOAlternativeFunction::kAF7);

    // USART2
    REGISTER_UART_PIN(UARTControllerID::kUSART2, GPIOPortID::kA, 0, UARTPinType::kCTS, GPIOAlternativeFunction::kAF7);
    REGISTER_UART_PIN(UARTControllerID::kUSART2, GPIOPortID::kA, 1, UARTPinType::kRTS, GPIOAlternativeFunction::kAF7);
    REGISTER_UART_PIN(UARTControllerID::kUSART2, GPIOPortID::kA, 1, UARTPinType::kDE, GPIOAlternativeFunction::kAF7);
    REGISTER_UART_PIN(UARTControllerID::kUSART2, GPIOPortID::kA, 2, UARTPinType::kTx, GPIOAlternativeFunction::kAF7);
    REGISTER_UART_PIN(UARTControllerID::kUSART2, GPIOPortID::kA, 3, UARTPinType::kRx, GPIOAlternativeFunction::kAF7);
    REGISTER_UART_PIN(UARTControllerID::kUSART2, GPIOPortID::kA, 4, UARTPinType::kCK, GPIOAlternativeFunction::kAF7);
    REGISTER_UART_PIN(UARTControllerID::kUSART2, GPIOPortID::kA, 14, UARTPinType::kTx, GPIOAlternativeFunction::kAF7);
    REGISTER_UART_PIN(UARTControllerID::kUSART2, GPIOPortID::kA, 15, UARTPinType::kRx, GPIOAlternativeFunction::kAF7);

    REGISTER_UART_PIN(UARTControllerID::kUSART2, GPIOPortID::kB, 3, UARTPinType::kTx, GPIOAlternativeFunction::kAF7);
    REGISTER_UART_PIN(UARTControllerID::kUSART2, GPIOPortID::kB, 4, UARTPinType::kRx, GPIOAlternativeFunction::kAF7);
    REGISTER_UART_PIN(UARTControllerID::kUSART2, GPIOPortID::kB, 5, UARTPinType::kCK, GPIOAlternativeFunction::kAF7);

    REGISTER_UART_PIN(UARTControllerID::kUSART2, GPIOPortID::kD, 3, UARTPinType::kCTS, GPIOAlternativeFunction::kAF7);
    REGISTER_UART_PIN(UARTControllerID::kUSART2, GPIOPortID::kD, 4, UARTPinType::kRTS, GPIOAlternativeFunction::kAF7);
    REGISTER_UART_PIN(UARTControllerID::kUSART2, GPIOPortID::kD, 4, UARTPinType::kDE, GPIOAlternativeFunction::kAF7);
    REGISTER_UART_PIN(UARTControllerID::kUSART2, GPIOPortID::kD, 5, UARTPinType::kTx, GPIOAlternativeFunction::kAF7);
    REGISTER_UART_PIN(UARTControllerID::kUSART2, GPIOPortID::kD, 6, UARTPinType::kRx, GPIOAlternativeFunction::kAF7);
    REGISTER_UART_PIN(UARTControllerID::kUSART2, GPIOPortID::kD, 7, UARTPinType::kCK, GPIOAlternativeFunction::kAF7);

    // USART3
    REGISTER_UART_PIN(UARTControllerID::kUSART3, GPIOPortID::kA, 13, UARTPinType::kCTS, GPIOAlternativeFunction::kAF7);

    REGISTER_UART_PIN(UARTControllerID::kUSART3, GPIOPortID::kB, 8, UARTPinType::kRx, GPIOAlternativeFunction::kAF7);
    REGISTER_UART_PIN(UARTControllerID::kUSART3, GPIOPortID::kB, 9, UARTPinType::kTx, GPIOAlternativeFunction::kAF7);
    REGISTER_UART_PIN(UARTControllerID::kUSART3, GPIOPortID::kB, 10, UARTPinType::kTx, GPIOAlternativeFunction::kAF7);
    REGISTER_UART_PIN(UARTControllerID::kUSART3, GPIOPortID::kB, 11, UARTPinType::kRx, GPIOAlternativeFunction::kAF7);
    REGISTER_UART_PIN(UARTControllerID::kUSART3, GPIOPortID::kB, 12, UARTPinType::kCK, GPIOAlternativeFunction::kAF7);
    REGISTER_UART_PIN(UARTControllerID::kUSART3, GPIOPortID::kB, 13, UARTPinType::kCTS, GPIOAlternativeFunction::kAF7);
    REGISTER_UART_PIN(UARTControllerID::kUSART3, GPIOPortID::kB, 14, UARTPinType::kRTS, GPIOAlternativeFunction::kAF7);
    REGISTER_UART_PIN(UARTControllerID::kUSART3, GPIOPortID::kB, 14, UARTPinType::kDE, GPIOAlternativeFunction::kAF7);

    REGISTER_UART_PIN(UARTControllerID::kUSART3, GPIOPortID::kC, 10, UARTPinType::kTx, GPIOAlternativeFunction::kAF7);
    REGISTER_UART_PIN(UARTControllerID::kUSART3, GPIOPortID::kC, 11, UARTPinType::kRx, GPIOAlternativeFunction::kAF7);
    REGISTER_UART_PIN(UARTControllerID::kUSART3, GPIOPortID::kC, 12, UARTPinType::kCK, GPIOAlternativeFunction::kAF7);

    REGISTER_UART_PIN(UARTControllerID::kUSART3, GPIOPortID::kD, 8, UARTPinType::kTx, GPIOAlternativeFunction::kAF7);
    REGISTER_UART_PIN(UARTControllerID::kUSART3, GPIOPortID::kD, 9, UARTPinType::kRx, GPIOAlternativeFunction::kAF7);
    REGISTER_UART_PIN(UARTControllerID::kUSART3, GPIOPortID::kD, 10, UARTPinType::kCK, GPIOAlternativeFunction::kAF7);
    REGISTER_UART_PIN(UARTControllerID::kUSART3, GPIOPortID::kD, 11, UARTPinType::kCTS, GPIOAlternativeFunction::kAF7);
    REGISTER_UART_PIN(UARTControllerID::kUSART3, GPIOPortID::kD, 12, UARTPinType::kRTS, GPIOAlternativeFunction::kAF7);
    REGISTER_UART_PIN(UARTControllerID::kUSART3, GPIOPortID::kD, 12, UARTPinType::kDE, GPIOAlternativeFunction::kAF7);

    REGISTER_UART_PIN(UARTControllerID::kUSART3, GPIOPortID::kE, 15, UARTPinType::kRx, GPIOAlternativeFunction::kAF7);

    REGISTER_UART_PIN(UARTControllerID::kUSART3, GPIOPortID::kF, 6, UARTPinType::kRTS, GPIOAlternativeFunction::kAF7);

    // UART4
    REGISTER_UART_PIN(UARTControllerID::kUART4, GPIOPortID::kA, 15, UARTPinType::kRTS, GPIOAlternativeFunction::kAF8);
    REGISTER_UART_PIN(UARTControllerID::kUART4, GPIOPortID::kA, 15, UARTPinType::kDE, GPIOAlternativeFunction::kAF8);

    REGISTER_UART_PIN(UARTControllerID::kUART4, GPIOPortID::kB, 7, UARTPinType::kCTS, GPIOAlternativeFunction::kAF14);

    REGISTER_UART_PIN(UARTControllerID::kUART4, GPIOPortID::kC, 10, UARTPinType::kTx, GPIOAlternativeFunction::kAF5);
    REGISTER_UART_PIN(UARTControllerID::kUART4, GPIOPortID::kC, 11, UARTPinType::kRx, GPIOAlternativeFunction::kAF5);

    // UART5
    REGISTER_UART_PIN(UARTControllerID::kUART5, GPIOPortID::kB, 4, UARTPinType::kRTS, GPIOAlternativeFunction::kAF8);
    REGISTER_UART_PIN(UARTControllerID::kUART5, GPIOPortID::kB, 4, UARTPinType::kDE, GPIOAlternativeFunction::kAF8);
    REGISTER_UART_PIN(UARTControllerID::kUART5, GPIOPortID::kB, 5, UARTPinType::kCTS, GPIOAlternativeFunction::kAF14);

    REGISTER_UART_PIN(UARTControllerID::kUART5, GPIOPortID::kC, 12, UARTPinType::kTx, GPIOAlternativeFunction::kAF5);

    REGISTER_UART_PIN(UARTControllerID::kUART5, GPIOPortID::kD, 2, UARTPinType::kRx, GPIOAlternativeFunction::kAF5);

    // LPUART1
    REGISTER_UART_PIN(UARTControllerID::kLPUART1, GPIOPortID::kA, 2, UARTPinType::kTx, GPIOAlternativeFunction::kAF12);
    REGISTER_UART_PIN(UARTControllerID::kLPUART1, GPIOPortID::kA, 3, UARTPinType::kRx, GPIOAlternativeFunction::kAF12);
    REGISTER_UART_PIN(UARTControllerID::kLPUART1, GPIOPortID::kA, 6, UARTPinType::kCTS, GPIOAlternativeFunction::kAF12);

    REGISTER_UART_PIN(UARTControllerID::kLPUART1, GPIOPortID::kB, 1, UARTPinType::kRTS, GPIOAlternativeFunction::kAF12);
    REGISTER_UART_PIN(UARTControllerID::kLPUART1, GPIOPortID::kB, 1, UARTPinType::kDE, GPIOAlternativeFunction::kAF12);
    REGISTER_UART_PIN(UARTControllerID::kLPUART1, GPIOPortID::kB, 10, UARTPinType::kRx, GPIOAlternativeFunction::kAF8);
    REGISTER_UART_PIN(UARTControllerID::kLPUART1, GPIOPortID::kB, 11, UARTPinType::kTx, GPIOAlternativeFunction::kAF8);
    REGISTER_UART_PIN(UARTControllerID::kLPUART1, GPIOPortID::kB, 12, UARTPinType::kRTS, GPIOAlternativeFunction::kAF8);
    REGISTER_UART_PIN(UARTControllerID::kLPUART1, GPIOPortID::kB, 12, UARTPinType::kDE, GPIOAlternativeFunction::kAF8);
    REGISTER_UART_PIN(UARTControllerID::kLPUART1, GPIOPortID::kB, 13, UARTPinType::kCTS, GPIOAlternativeFunction::kAF8);

    REGISTER_UART_PIN(UARTControllerID::kLPUART1, GPIOPortID::kC, 0, UARTPinType::kRx, GPIOAlternativeFunction::kAF8);
    REGISTER_UART_PIN(UARTControllerID::kLPUART1, GPIOPortID::kC, 1, UARTPinType::kTx, GPIOAlternativeFunction::kAF8);

    REGISTER_UART_PIN(UARTControllerID::kLPUART1, GPIOPortID::kG, 5, UARTPinType::kCTS, GPIOAlternativeFunction::kAF8);
    REGISTER_UART_PIN(UARTControllerID::kLPUART1, GPIOPortID::kG, 6, UARTPinType::kRTS, GPIOAlternativeFunction::kAF8);
    REGISTER_UART_PIN(UARTControllerID::kLPUART1, GPIOPortID::kG, 6, UARTPinType::kDE, GPIOAlternativeFunction::kAF8);
    REGISTER_UART_PIN(UARTControllerID::kLPUART1, GPIOPortID::kG, 7, UARTPinType::kTx, GPIOAlternativeFunction::kAF8);
    REGISTER_UART_PIN(UARTControllerID::kLPUART1, GPIOPortID::kG, 8, UARTPinType::kRx, GPIOAlternativeFunction::kAF8);

}  // namespace valle::platform