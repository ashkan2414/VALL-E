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
    DECLARE_ADC_PIN_MAP(ADCControllerID::kADC1, ADCChannelID::kChannel1, GPIOPortID::kPortA, GPIOPinID::kPin0);  // PA0
    DECLARE_ADC_PIN_MAP(ADCControllerID::kADC1, ADCChannelID::kChannel2, GPIOPortID::kPortA, GPIOPinID::kPin1);  // PA1
    DECLARE_ADC_PIN_MAP(ADCControllerID::kADC1, ADCChannelID::kChannel3, GPIOPortID::kPortA, GPIOPinID::kPin2);  // PA2
    DECLARE_ADC_PIN_MAP(ADCControllerID::kADC1, ADCChannelID::kChannel4, GPIOPortID::kPortA, GPIOPinID::kPin3);  // PA3
    DECLARE_ADC_PIN_MAP(ADCControllerID::kADC1,
                        ADCChannelID::kChannel5,
                        GPIOPortID::kPortB,
                        GPIOPinID::kPin14);                                                                      // PB14
    DECLARE_ADC_PIN_MAP(ADCControllerID::kADC1, ADCChannelID::kChannel6, GPIOPortID::kPortC, GPIOPinID::kPin0);  // PC0
    DECLARE_ADC_PIN_MAP(ADCControllerID::kADC1, ADCChannelID::kChannel7, GPIOPortID::kPortC, GPIOPinID::kPin1);  // PC1
    DECLARE_ADC_PIN_MAP(ADCControllerID::kADC1, ADCChannelID::kChannel8, GPIOPortID::kPortC, GPIOPinID::kPin2);  // PC2
    DECLARE_ADC_PIN_MAP(ADCControllerID::kADC1, ADCChannelID::kChannel9, GPIOPortID::kPortC, GPIOPinID::kPin3);  // PC3
    DECLARE_ADC_PIN_MAP(ADCControllerID::kADC1,
                        ADCChannelID::kChannel10,
                        GPIOPortID::kPortF,
                        GPIOPinID::kPin0);  // PF0 (OSC_IN - Check SB)
    DECLARE_ADC_PIN_MAP(ADCControllerID::kADC1,
                        ADCChannelID::kChannel11,
                        GPIOPortID::kPortB,
                        GPIOPinID::kPin12);  // PB12
    DECLARE_ADC_PIN_MAP(ADCControllerID::kADC1, ADCChannelID::kChannel12, GPIOPortID::kPortB, GPIOPinID::kPin1);  // PB1
    DECLARE_ADC_PIN_MAP(ADCControllerID::kADC1,
                        ADCChannelID::kChannel14,
                        GPIOPortID::kPortB,
                        GPIOPinID::kPin11);  // PB11
    DECLARE_ADC_PIN_MAP(ADCControllerID::kADC1, ADCChannelID::kChannel15, GPIOPortID::kPortB, GPIOPinID::kPin0);  // PB0

    // ADC2
    DECLARE_ADC_PIN_MAP(ADCControllerID::kADC2, ADCChannelID::kChannel1, GPIOPortID::kPortA, GPIOPinID::kPin0);  // PA0
    DECLARE_ADC_PIN_MAP(ADCControllerID::kADC2, ADCChannelID::kChannel2, GPIOPortID::kPortA, GPIOPinID::kPin1);  // PA1
    DECLARE_ADC_PIN_MAP(ADCControllerID::kADC2, ADCChannelID::kChannel3, GPIOPortID::kPortA, GPIOPinID::kPin6);  // PA6
    DECLARE_ADC_PIN_MAP(ADCControllerID::kADC2, ADCChannelID::kChannel4, GPIOPortID::kPortA, GPIOPinID::kPin7);  // PA7
    DECLARE_ADC_PIN_MAP(ADCControllerID::kADC2, ADCChannelID::kChannel5, GPIOPortID::kPortC, GPIOPinID::kPin4);  // PC4
    DECLARE_ADC_PIN_MAP(ADCControllerID::kADC2, ADCChannelID::kChannel6, GPIOPortID::kPortC, GPIOPinID::kPin0);  // PC0
    DECLARE_ADC_PIN_MAP(ADCControllerID::kADC2, ADCChannelID::kChannel7, GPIOPortID::kPortC, GPIOPinID::kPin1);  // PC1
    DECLARE_ADC_PIN_MAP(ADCControllerID::kADC2, ADCChannelID::kChannel8, GPIOPortID::kPortC, GPIOPinID::kPin2);  // PC2
    DECLARE_ADC_PIN_MAP(ADCControllerID::kADC2, ADCChannelID::kChannel9, GPIOPortID::kPortC, GPIOPinID::kPin3);  // PC3
    DECLARE_ADC_PIN_MAP(ADCControllerID::kADC2,
                        ADCChannelID::kChannel10,
                        GPIOPortID::kPortF,
                        GPIOPinID::kPin1);  // PF1 (OSC_OUT - Check SB)
    DECLARE_ADC_PIN_MAP(ADCControllerID::kADC2, ADCChannelID::kChannel11, GPIOPortID::kPortC, GPIOPinID::kPin5);  // PC5
    DECLARE_ADC_PIN_MAP(ADCControllerID::kADC2, ADCChannelID::kChannel12, GPIOPortID::kPortB, GPIOPinID::kPin2);  // PB2
    DECLARE_ADC_PIN_MAP(ADCControllerID::kADC2, ADCChannelID::kChannel13, GPIOPortID::kPortA, GPIOPinID::kPin5);  // PA5
    DECLARE_ADC_PIN_MAP(ADCControllerID::kADC2,
                        ADCChannelID::kChannel14,
                        GPIOPortID::kPortB,
                        GPIOPinID::kPin11);  // PB11
    DECLARE_ADC_PIN_MAP(ADCControllerID::kADC2,
                        ADCChannelID::kChannel15,
                        GPIOPortID::kPortB,
                        GPIOPinID::kPin15);  // PB15
    DECLARE_ADC_PIN_MAP(ADCControllerID::kADC2, ADCChannelID::kChannel17, GPIOPortID::kPortA, GPIOPinID::kPin4);  // PA4

    // ADC3
    DECLARE_ADC_PIN_MAP(ADCControllerID::kADC3, ADCChannelID::kChannel1, GPIOPortID::kPortB, GPIOPinID::kPin1);  // PB1
    DECLARE_ADC_PIN_MAP(ADCControllerID::kADC3, ADCChannelID::kChannel2, GPIOPortID::kPortE, GPIOPinID::kPin9);  // PE9
    DECLARE_ADC_PIN_MAP(ADCControllerID::kADC3,
                        ADCChannelID::kChannel3,
                        GPIOPortID::kPortE,
                        GPIOPinID::kPin13);                                                                      // PE13
    DECLARE_ADC_PIN_MAP(ADCControllerID::kADC3, ADCChannelID::kChannel4, GPIOPortID::kPortE, GPIOPinID::kPin7);  // PE7
    DECLARE_ADC_PIN_MAP(ADCControllerID::kADC3,
                        ADCChannelID::kChannel5,
                        GPIOPortID::kPortB,
                        GPIOPinID::kPin13);                                                                      // PB13
    DECLARE_ADC_PIN_MAP(ADCControllerID::kADC3, ADCChannelID::kChannel6, GPIOPortID::kPortE, GPIOPinID::kPin8);  // PE8
    DECLARE_ADC_PIN_MAP(ADCControllerID::kADC3,
                        ADCChannelID::kChannel7,
                        GPIOPortID::kPortD,
                        GPIOPinID::kPin10);  // PD10
    DECLARE_ADC_PIN_MAP(ADCControllerID::kADC3,
                        ADCChannelID::kChannel8,
                        GPIOPortID::kPortD,
                        GPIOPinID::kPin11);  // PD11
    DECLARE_ADC_PIN_MAP(ADCControllerID::kADC3,
                        ADCChannelID::kChannel9,
                        GPIOPortID::kPortD,
                        GPIOPinID::kPin12);  // PD12
    DECLARE_ADC_PIN_MAP(ADCControllerID::kADC3,
                        ADCChannelID::kChannel10,
                        GPIOPortID::kPortD,
                        GPIOPinID::kPin13);  // PD13
    DECLARE_ADC_PIN_MAP(ADCControllerID::kADC3,
                        ADCChannelID::kChannel11,
                        GPIOPortID::kPortD,
                        GPIOPinID::kPin14);  // PD14
    DECLARE_ADC_PIN_MAP(ADCControllerID::kADC3, ADCChannelID::kChannel12, GPIOPortID::kPortB, GPIOPinID::kPin0);  // PB0
    DECLARE_ADC_PIN_MAP(ADCControllerID::kADC3,
                        ADCChannelID::kChannel14,
                        GPIOPortID::kPortE,
                        GPIOPinID::kPin10);  // PE10
    DECLARE_ADC_PIN_MAP(ADCControllerID::kADC3,
                        ADCChannelID::kChannel15,
                        GPIOPortID::kPortE,
                        GPIOPinID::kPin11);  // PE11
    DECLARE_ADC_PIN_MAP(ADCControllerID::kADC3,
                        ADCChannelID::kChannel16,
                        GPIOPortID::kPortE,
                        GPIOPinID::kPin12);  // PE12

    // ADC4
    DECLARE_ADC_PIN_MAP(ADCControllerID::kADC4,
                        ADCChannelID::kChannel1,
                        GPIOPortID::kPortE,
                        GPIOPinID::kPin14);  // PE14
    DECLARE_ADC_PIN_MAP(ADCControllerID::kADC4,
                        ADCChannelID::kChannel2,
                        GPIOPortID::kPortE,
                        GPIOPinID::kPin15);  // PE15
    DECLARE_ADC_PIN_MAP(ADCControllerID::kADC4,
                        ADCChannelID::kChannel3,
                        GPIOPortID::kPortB,
                        GPIOPinID::kPin12);  // PB12
    DECLARE_ADC_PIN_MAP(ADCControllerID::kADC4,
                        ADCChannelID::kChannel4,
                        GPIOPortID::kPortB,
                        GPIOPinID::kPin14);  // PB14
    DECLARE_ADC_PIN_MAP(ADCControllerID::kADC4,
                        ADCChannelID::kChannel5,
                        GPIOPortID::kPortB,
                        GPIOPinID::kPin15);                                                                      // PB15
    DECLARE_ADC_PIN_MAP(ADCControllerID::kADC4, ADCChannelID::kChannel6, GPIOPortID::kPortE, GPIOPinID::kPin8);  // PE8
    DECLARE_ADC_PIN_MAP(ADCControllerID::kADC4,
                        ADCChannelID::kChannel7,
                        GPIOPortID::kPortD,
                        GPIOPinID::kPin10);  // PD10
    DECLARE_ADC_PIN_MAP(ADCControllerID::kADC4,
                        ADCChannelID::kChannel8,
                        GPIOPortID::kPortD,
                        GPIOPinID::kPin11);  // PD11
    DECLARE_ADC_PIN_MAP(ADCControllerID::kADC4,
                        ADCChannelID::kChannel9,
                        GPIOPortID::kPortD,
                        GPIOPinID::kPin12);  // PD12
    DECLARE_ADC_PIN_MAP(ADCControllerID::kADC4,
                        ADCChannelID::kChannel10,
                        GPIOPortID::kPortD,
                        GPIOPinID::kPin13);  // PD13
    DECLARE_ADC_PIN_MAP(ADCControllerID::kADC4,
                        ADCChannelID::kChannel11,
                        GPIOPortID::kPortD,
                        GPIOPinID::kPin14);  // PD14
    DECLARE_ADC_PIN_MAP(ADCControllerID::kADC4, ADCChannelID::kChannel12, GPIOPortID::kPortD, GPIOPinID::kPin8);  // PD8
    DECLARE_ADC_PIN_MAP(ADCControllerID::kADC4, ADCChannelID::kChannel13, GPIOPortID::kPortD, GPIOPinID::kPin9);  // PD9
    DECLARE_ADC_PIN_MAP(ADCControllerID::kADC4,
                        ADCChannelID::kChannel14,
                        GPIOPortID::kPortE,
                        GPIOPinID::kPin10);  // PE10
    DECLARE_ADC_PIN_MAP(ADCControllerID::kADC4,
                        ADCChannelID::kChannel15,
                        GPIOPortID::kPortE,
                        GPIOPinID::kPin11);  // PE11
    DECLARE_ADC_PIN_MAP(ADCControllerID::kADC4,
                        ADCChannelID::kChannel16,
                        GPIOPortID::kPortE,
                        GPIOPinID::kPin12);  // PE12

    // ADC5
    DECLARE_ADC_PIN_MAP(ADCControllerID::kADC5, ADCChannelID::kChannel1, GPIOPortID::kPortA, GPIOPinID::kPin8);  // PA8
    DECLARE_ADC_PIN_MAP(ADCControllerID::kADC5, ADCChannelID::kChannel2, GPIOPortID::kPortA, GPIOPinID::kPin9);  // PA9
    DECLARE_ADC_PIN_MAP(ADCControllerID::kADC5, ADCChannelID::kChannel6, GPIOPortID::kPortE, GPIOPinID::kPin8);  // PE8
    DECLARE_ADC_PIN_MAP(ADCControllerID::kADC5,
                        ADCChannelID::kChannel7,
                        GPIOPortID::kPortD,
                        GPIOPinID::kPin10);  // PD10
    DECLARE_ADC_PIN_MAP(ADCControllerID::kADC5,
                        ADCChannelID::kChannel8,
                        GPIOPortID::kPortD,
                        GPIOPinID::kPin11);  // PD11
    DECLARE_ADC_PIN_MAP(ADCControllerID::kADC5,
                        ADCChannelID::kChannel9,
                        GPIOPortID::kPortD,
                        GPIOPinID::kPin12);  // PD12
    DECLARE_ADC_PIN_MAP(ADCControllerID::kADC5,
                        ADCChannelID::kChannel10,
                        GPIOPortID::kPortD,
                        GPIOPinID::kPin13);  // PD13
    DECLARE_ADC_PIN_MAP(ADCControllerID::kADC5,
                        ADCChannelID::kChannel11,
                        GPIOPortID::kPortD,
                        GPIOPinID::kPin14);  // PD14
    DECLARE_ADC_PIN_MAP(ADCControllerID::kADC5, ADCChannelID::kChannel12, GPIOPortID::kPortD, GPIOPinID::kPin8);  // PD8
    DECLARE_ADC_PIN_MAP(ADCControllerID::kADC5, ADCChannelID::kChannel13, GPIOPortID::kPortD, GPIOPinID::kPin9);  // PD9
    DECLARE_ADC_PIN_MAP(ADCControllerID::kADC5,
                        ADCChannelID::kChannel14,
                        GPIOPortID::kPortE,
                        GPIOPinID::kPin10);  // PE10
    DECLARE_ADC_PIN_MAP(ADCControllerID::kADC5,
                        ADCChannelID::kChannel15,
                        GPIOPortID::kPortE,
                        GPIOPinID::kPin11);  // PE11
    DECLARE_ADC_PIN_MAP(ADCControllerID::kADC5,
                        ADCChannelID::kChannel16,
                        GPIOPortID::kPortE,
                        GPIOPinID::kPin12);  // PE12

#undef DECLARE_ADC_PIN_MAP

    template <typename T>
    concept CNullADCPinMap = requires { typename T::NullPinTag; };

    template <ADCControllerID tkControllerID, ADCChannelID tkChannelID, GPIOPortID tkPortID, GPIOPinID tkPin>
    concept CValidADCPin = !CNullADCPinMap<ADCPinMap<tkControllerID, tkChannelID>> &&
                           (ADCPinMap<tkControllerID, tkChannelID>::skPort == tkPortID) &&
                           (ADCPinMap<tkControllerID, tkChannelID>::skPin == tkPin);

}  // namespace valle::platform