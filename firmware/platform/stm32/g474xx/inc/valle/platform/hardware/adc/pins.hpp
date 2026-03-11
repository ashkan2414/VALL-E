#pragma once

#include "valle/platform/hardware/ADC/id.hpp"
#include "valle/platform/hardware/gpio.hpp"

namespace valle::platform
{
    template <ADCControllerID tkControllerID, ADCChannelID tkChannelID>
    struct ADCPinMap
    {
        using NullPinTag = void;
    };

#define DECLARE_ADC_PIN_MAP(tkControllerID, tkChannelID, port, pin) \
    template <>                                                     \
    struct ADCPinMap<tkControllerID, tkChannelID>                   \
    {                                                               \
        constexpr static GPIOPortID skPort = port;                  \
        constexpr static GPIOPinID  skPin  = pin;                   \
    };

    // ADC1
    DECLARE_ADC_PIN_MAP(1, ADCChannelID::kChannel1, GPIOPortID::kA, 0);    // PA0
    DECLARE_ADC_PIN_MAP(1, ADCChannelID::kChannel2, GPIOPortID::kA, 1);    // PA1
    DECLARE_ADC_PIN_MAP(1, ADCChannelID::kChannel3, GPIOPortID::kA, 2);    // PA2
    DECLARE_ADC_PIN_MAP(1, ADCChannelID::kChannel4, GPIOPortID::kA, 3);    // PA3
    DECLARE_ADC_PIN_MAP(1, ADCChannelID::kChannel5, GPIOPortID::kB, 14);   // PB14
    DECLARE_ADC_PIN_MAP(1, ADCChannelID::kChannel6, GPIOPortID::kC, 0);    // PC0
    DECLARE_ADC_PIN_MAP(1, ADCChannelID::kChannel7, GPIOPortID::kC, 1);    // PC1
    DECLARE_ADC_PIN_MAP(1, ADCChannelID::kChannel8, GPIOPortID::kC, 2);    // PC2
    DECLARE_ADC_PIN_MAP(1, ADCChannelID::kChannel9, GPIOPortID::kC, 3);    // PC3
    DECLARE_ADC_PIN_MAP(1, ADCChannelID::kChannel10, GPIOPortID::kF, 0);   // PF0 (OSC_IN - Check SB)
    DECLARE_ADC_PIN_MAP(1, ADCChannelID::kChannel11, GPIOPortID::kB, 12);  // PB12
    DECLARE_ADC_PIN_MAP(1, ADCChannelID::kChannel12, GPIOPortID::kB, 1);   // PB1
    DECLARE_ADC_PIN_MAP(1, ADCChannelID::kChannel14, GPIOPortID::kB, 11);  // PB11
    DECLARE_ADC_PIN_MAP(1, ADCChannelID::kChannel15, GPIOPortID::kB, 0);   // PB0

    // ADC2
    DECLARE_ADC_PIN_MAP(2, ADCChannelID::kChannel1, GPIOPortID::kA, 0);    // PA0
    DECLARE_ADC_PIN_MAP(2, ADCChannelID::kChannel2, GPIOPortID::kA, 1);    // PA1
    DECLARE_ADC_PIN_MAP(2, ADCChannelID::kChannel3, GPIOPortID::kA, 6);    // PA6
    DECLARE_ADC_PIN_MAP(2, ADCChannelID::kChannel4, GPIOPortID::kA, 7);    // PA7
    DECLARE_ADC_PIN_MAP(2, ADCChannelID::kChannel5, GPIOPortID::kC, 4);    // PC4
    DECLARE_ADC_PIN_MAP(2, ADCChannelID::kChannel6, GPIOPortID::kC, 0);    // PC0
    DECLARE_ADC_PIN_MAP(2, ADCChannelID::kChannel7, GPIOPortID::kC, 1);    // PC1
    DECLARE_ADC_PIN_MAP(2, ADCChannelID::kChannel8, GPIOPortID::kC, 2);    // PC2
    DECLARE_ADC_PIN_MAP(2, ADCChannelID::kChannel9, GPIOPortID::kC, 3);    // PC3
    DECLARE_ADC_PIN_MAP(2, ADCChannelID::kChannel10, GPIOPortID::kF, 1);   // PF1 (OSC_OUT - Check SB)
    DECLARE_ADC_PIN_MAP(2, ADCChannelID::kChannel11, GPIOPortID::kC, 5);   // PC5
    DECLARE_ADC_PIN_MAP(2, ADCChannelID::kChannel12, GPIOPortID::kB, 2);   // PB2
    DECLARE_ADC_PIN_MAP(2, ADCChannelID::kChannel13, GPIOPortID::kA, 5);   // PA5
    DECLARE_ADC_PIN_MAP(2, ADCChannelID::kChannel14, GPIOPortID::kB, 11);  // PB11
    DECLARE_ADC_PIN_MAP(2, ADCChannelID::kChannel15, GPIOPortID::kB, 15);  // PB15
    DECLARE_ADC_PIN_MAP(2, ADCChannelID::kChannel17, GPIOPortID::kA, 4);   // PA4

    // ADC3
    DECLARE_ADC_PIN_MAP(3, ADCChannelID::kChannel1, GPIOPortID::kB, 1);    // PB1
    DECLARE_ADC_PIN_MAP(3, ADCChannelID::kChannel2, GPIOPortID::kE, 9);    // PE9
    DECLARE_ADC_PIN_MAP(3, ADCChannelID::kChannel3, GPIOPortID::kE, 13);   // PE13
    DECLARE_ADC_PIN_MAP(3, ADCChannelID::kChannel4, GPIOPortID::kE, 7);    // PE7
    DECLARE_ADC_PIN_MAP(3, ADCChannelID::kChannel5, GPIOPortID::kB, 13);   // PB13
    DECLARE_ADC_PIN_MAP(3, ADCChannelID::kChannel6, GPIOPortID::kE, 8);    // PE8
    DECLARE_ADC_PIN_MAP(3, ADCChannelID::kChannel7, GPIOPortID::kD, 10);   // PD10
    DECLARE_ADC_PIN_MAP(3, ADCChannelID::kChannel8, GPIOPortID::kD, 11);   // PD11
    DECLARE_ADC_PIN_MAP(3, ADCChannelID::kChannel9, GPIOPortID::kD, 12);   // PD12
    DECLARE_ADC_PIN_MAP(3, ADCChannelID::kChannel10, GPIOPortID::kD, 13);  // PD13
    DECLARE_ADC_PIN_MAP(3, ADCChannelID::kChannel11, GPIOPortID::kD, 14);  // PD14
    DECLARE_ADC_PIN_MAP(3, ADCChannelID::kChannel12, GPIOPortID::kB, 0);   // PB0
    DECLARE_ADC_PIN_MAP(3, ADCChannelID::kChannel14, GPIOPortID::kE, 10);  // PE10
    DECLARE_ADC_PIN_MAP(3, ADCChannelID::kChannel15, GPIOPortID::kE, 11);  // PE11
    DECLARE_ADC_PIN_MAP(3, ADCChannelID::kChannel16, GPIOPortID::kE, 12);  // PE12

    // ADC4
    DECLARE_ADC_PIN_MAP(4, ADCChannelID::kChannel1, GPIOPortID::kE, 14);   // PE14
    DECLARE_ADC_PIN_MAP(4, ADCChannelID::kChannel2, GPIOPortID::kE, 15);   // PE15
    DECLARE_ADC_PIN_MAP(4, ADCChannelID::kChannel3, GPIOPortID::kB, 12);   // PB12
    DECLARE_ADC_PIN_MAP(4, ADCChannelID::kChannel4, GPIOPortID::kB, 14);   // PB14
    DECLARE_ADC_PIN_MAP(4, ADCChannelID::kChannel5, GPIOPortID::kB, 15);   // PB15
    DECLARE_ADC_PIN_MAP(4, ADCChannelID::kChannel6, GPIOPortID::kE, 8);    // PE8
    DECLARE_ADC_PIN_MAP(4, ADCChannelID::kChannel7, GPIOPortID::kD, 10);   // PD10
    DECLARE_ADC_PIN_MAP(4, ADCChannelID::kChannel8, GPIOPortID::kD, 11);   // PD11
    DECLARE_ADC_PIN_MAP(4, ADCChannelID::kChannel9, GPIOPortID::kD, 12);   // PD12
    DECLARE_ADC_PIN_MAP(4, ADCChannelID::kChannel10, GPIOPortID::kD, 13);  // PD13
    DECLARE_ADC_PIN_MAP(4, ADCChannelID::kChannel11, GPIOPortID::kD, 14);  // PD14
    DECLARE_ADC_PIN_MAP(4, ADCChannelID::kChannel12, GPIOPortID::kD, 8);   // PD8
    DECLARE_ADC_PIN_MAP(4, ADCChannelID::kChannel13, GPIOPortID::kD, 9);   // PD9
    DECLARE_ADC_PIN_MAP(4, ADCChannelID::kChannel14, GPIOPortID::kE, 10);  // PE10
    DECLARE_ADC_PIN_MAP(4, ADCChannelID::kChannel15, GPIOPortID::kE, 11);  // PE11
    DECLARE_ADC_PIN_MAP(4, ADCChannelID::kChannel16, GPIOPortID::kE, 12);  // PE12

    // ADC5
    DECLARE_ADC_PIN_MAP(5, ADCChannelID::kChannel1, GPIOPortID::kA, 8);    // PA8
    DECLARE_ADC_PIN_MAP(5, ADCChannelID::kChannel2, GPIOPortID::kA, 9);    // PA9
    DECLARE_ADC_PIN_MAP(5, ADCChannelID::kChannel6, GPIOPortID::kE, 8);    // PE8
    DECLARE_ADC_PIN_MAP(5, ADCChannelID::kChannel7, GPIOPortID::kD, 10);   // PD10
    DECLARE_ADC_PIN_MAP(5, ADCChannelID::kChannel8, GPIOPortID::kD, 11);   // PD11
    DECLARE_ADC_PIN_MAP(5, ADCChannelID::kChannel9, GPIOPortID::kD, 12);   // PD12
    DECLARE_ADC_PIN_MAP(5, ADCChannelID::kChannel10, GPIOPortID::kD, 13);  // PD13
    DECLARE_ADC_PIN_MAP(5, ADCChannelID::kChannel11, GPIOPortID::kD, 14);  // PD14
    DECLARE_ADC_PIN_MAP(5, ADCChannelID::kChannel12, GPIOPortID::kD, 8);   // PD8
    DECLARE_ADC_PIN_MAP(5, ADCChannelID::kChannel13, GPIOPortID::kD, 9);   // PD9
    DECLARE_ADC_PIN_MAP(5, ADCChannelID::kChannel14, GPIOPortID::kE, 10);  // PE10
    DECLARE_ADC_PIN_MAP(5, ADCChannelID::kChannel15, GPIOPortID::kE, 11);  // PE11
    DECLARE_ADC_PIN_MAP(5, ADCChannelID::kChannel16, GPIOPortID::kE, 12);  // PE12

#undef DECLARE_ADC_PIN_MAP

    template <typename T>
    concept CNullADCPinMap = requires { typename T::NullPinTag; };

    template <ADCControllerID tkControllerID, ADCChannelID tkChannelID, GPIOPortID tkPortID, GPIOPinID tkPin>
    concept CValidADCPin = !CNullADCPinMap<ADCPinMap<tkControllerID, tkChannelID>> &&
                           (ADCPinMap<tkControllerID, tkChannelID>::skPort == tkPortID) &&
                           (ADCPinMap<tkControllerID, tkChannelID>::skPin == tkPin);

}  // namespace valle::platform