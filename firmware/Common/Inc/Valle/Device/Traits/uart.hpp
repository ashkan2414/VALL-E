#pragma once

#include "Valle/Device/Traits/dma.hpp"
#include "Valle/Device/Traits/gpio.hpp"
#include "stm32g4xx_hal.h"

namespace valle
{
    enum UARTControllerID : uint8_t
    {
        kUSART1  = 1,
        kUSART2  = 2,
        kUSART3  = 3,
        kUART4   = 4,
        kUART5   = 5,
        kLPUART1 = 6,
    };

    // ===========================================================================
    // HARDWARE TRAITS
    // ===========================================================================
    template <UARTControllerID tkID>
    struct UARTTraits;

    template <>
    struct UARTTraits<UARTControllerID::kUSART1>
    {
        static inline USART_TypeDef* const skInstance          = USART1;
        static constexpr IRQn_Type         skIRQn              = USART1_IRQn;
        static constexpr DMARequestID      skTxRequest         = DMARequestID::kUSART1Tx;
        static constexpr DMARequestID      skRxRequest         = DMARequestID::kUSART1Rx;
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
    struct UARTTraits<UARTControllerID::kUSART2>
    {
        static inline USART_TypeDef* const skInstance          = USART2;
        static constexpr IRQn_Type         skIRQn              = USART2_IRQn;
        static constexpr DMARequestID      skTxRequest         = DMARequestID::kUSART2Tx;
        static constexpr DMARequestID      skRxRequest         = DMARequestID::kUSART2Rx;
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
    struct UARTTraits<UARTControllerID::kUSART3>
    {
        static inline USART_TypeDef* const skInstance          = USART3;
        static constexpr IRQn_Type         skIRQn              = USART3_IRQn;
        static constexpr DMARequestID      skTxRequest         = DMARequestID::kUSART3Tx;
        static constexpr DMARequestID      skRxRequest         = DMARequestID::kUSART3Rx;
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
    struct UARTTraits<UARTControllerID::kUART4>
    {
        static inline USART_TypeDef* const skInstance          = UART4;
        static constexpr IRQn_Type         skIRQn              = UART4_IRQn;
        static constexpr DMARequestID      skTxRequest         = DMARequestID::kUART4Tx;
        static constexpr DMARequestID      skRxRequest         = DMARequestID::kUART4Rx;
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
    struct UARTTraits<UARTControllerID::kUART5>
    {
        static inline USART_TypeDef* const skInstance          = UART5;
        static constexpr IRQn_Type         skIRQn              = UART5_IRQn;
        static constexpr DMARequestID      skTxRequest         = DMARequestID::kUART5Tx;
        static constexpr DMARequestID      skRxRequest         = DMARequestID::kUART5Rx;
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
    struct UARTTraits<UARTControllerID::kLPUART1>
    {
        static inline USART_TypeDef* const skInstance          = LPUART1;
        static constexpr IRQn_Type         skIRQn              = LPUART1_IRQn;
        static constexpr DMARequestID      skTxRequest         = DMARequestID::kLPUART1Tx;
        static constexpr DMARequestID      skRxRequest         = DMARequestID::kLPUART1Rx;
        static constexpr GPIOPortID        skDefaultGPIOTxPort = GPIOPortID::kA;
        static constexpr GPIOPinID         skDefaultGPIOTxPin  = 2;
        static constexpr GPIOPortID        skDefaultGPIORxPort = GPIOPortID::kA;
        static constexpr GPIOPinID         skDefaultGPIORxPin  = 3;

        static inline void enable_clock()
        {
            __HAL_RCC_LPUART1_CLK_ENABLE();
        }
    };

    // ============================================================================
    // UART PIN ALTERNATE FUNCTION MAPPINGS (Datasheet Table 13)
    // ============================================================================
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

}  // namespace valle