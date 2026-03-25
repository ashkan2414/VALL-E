#pragma once

#include "valle/platform/hardware/ADC/id.hpp"
#include "valle/platform/hardware/gpio.hpp"

namespace valle::platform
{
    template <AdcPeripheralId tkPeripheralId, AdcChannelId tkChannelId>
    struct AdcPinMap
    {
        using NullPinTag = void;
    };

#define DECLARE_ADC_PIN_MAP(tkPeripheralId, tkChannelId, port, pin) \
    template <>                                                     \
    struct AdcPinMap<tkPeripheralId, tkChannelId>                   \
    {                                                               \
        constexpr static GpioPortId skPort = port;                  \
        constexpr static GpioPinId  skPin  = pin;                   \
    };

    // Adc1
    DECLARE_ADC_PIN_MAP(AdcPeripheralId::kAdc1, AdcChannelId::kChannel1, GpioPortId::kPortA, GpioPinId::kPin0);  // PA0
    DECLARE_ADC_PIN_MAP(AdcPeripheralId::kAdc1, AdcChannelId::kChannel2, GpioPortId::kPortA, GpioPinId::kPin1);  // PA1
    DECLARE_ADC_PIN_MAP(AdcPeripheralId::kAdc1, AdcChannelId::kChannel3, GpioPortId::kPortA, GpioPinId::kPin2);  // PA2
    DECLARE_ADC_PIN_MAP(AdcPeripheralId::kAdc1, AdcChannelId::kChannel4, GpioPortId::kPortA, GpioPinId::kPin3);  // PA3
    DECLARE_ADC_PIN_MAP(AdcPeripheralId::kAdc1,
                        AdcChannelId::kChannel5,
                        GpioPortId::kPortB,
                        GpioPinId::kPin14);                                                                      // PB14
    DECLARE_ADC_PIN_MAP(AdcPeripheralId::kAdc1, AdcChannelId::kChannel6, GpioPortId::kPortC, GpioPinId::kPin0);  // PC0
    DECLARE_ADC_PIN_MAP(AdcPeripheralId::kAdc1, AdcChannelId::kChannel7, GpioPortId::kPortC, GpioPinId::kPin1);  // PC1
    DECLARE_ADC_PIN_MAP(AdcPeripheralId::kAdc1, AdcChannelId::kChannel8, GpioPortId::kPortC, GpioPinId::kPin2);  // PC2
    DECLARE_ADC_PIN_MAP(AdcPeripheralId::kAdc1, AdcChannelId::kChannel9, GpioPortId::kPortC, GpioPinId::kPin3);  // PC3
    DECLARE_ADC_PIN_MAP(AdcPeripheralId::kAdc1,
                        AdcChannelId::kChannel10,
                        GpioPortId::kPortF,
                        GpioPinId::kPin0);  // PF0 (OSC_IN - Check SB)
    DECLARE_ADC_PIN_MAP(AdcPeripheralId::kAdc1,
                        AdcChannelId::kChannel11,
                        GpioPortId::kPortB,
                        GpioPinId::kPin12);  // PB12
    DECLARE_ADC_PIN_MAP(AdcPeripheralId::kAdc1, AdcChannelId::kChannel12, GpioPortId::kPortB, GpioPinId::kPin1);  // PB1
    DECLARE_ADC_PIN_MAP(AdcPeripheralId::kAdc1,
                        AdcChannelId::kChannel14,
                        GpioPortId::kPortB,
                        GpioPinId::kPin11);  // PB11
    DECLARE_ADC_PIN_MAP(AdcPeripheralId::kAdc1, AdcChannelId::kChannel15, GpioPortId::kPortB, GpioPinId::kPin0);  // PB0

    // Adc2
    DECLARE_ADC_PIN_MAP(AdcPeripheralId::kAdc2, AdcChannelId::kChannel1, GpioPortId::kPortA, GpioPinId::kPin0);  // PA0
    DECLARE_ADC_PIN_MAP(AdcPeripheralId::kAdc2, AdcChannelId::kChannel2, GpioPortId::kPortA, GpioPinId::kPin1);  // PA1
    DECLARE_ADC_PIN_MAP(AdcPeripheralId::kAdc2, AdcChannelId::kChannel3, GpioPortId::kPortA, GpioPinId::kPin6);  // PA6
    DECLARE_ADC_PIN_MAP(AdcPeripheralId::kAdc2, AdcChannelId::kChannel4, GpioPortId::kPortA, GpioPinId::kPin7);  // PA7
    DECLARE_ADC_PIN_MAP(AdcPeripheralId::kAdc2, AdcChannelId::kChannel5, GpioPortId::kPortC, GpioPinId::kPin4);  // PC4
    DECLARE_ADC_PIN_MAP(AdcPeripheralId::kAdc2, AdcChannelId::kChannel6, GpioPortId::kPortC, GpioPinId::kPin0);  // PC0
    DECLARE_ADC_PIN_MAP(AdcPeripheralId::kAdc2, AdcChannelId::kChannel7, GpioPortId::kPortC, GpioPinId::kPin1);  // PC1
    DECLARE_ADC_PIN_MAP(AdcPeripheralId::kAdc2, AdcChannelId::kChannel8, GpioPortId::kPortC, GpioPinId::kPin2);  // PC2
    DECLARE_ADC_PIN_MAP(AdcPeripheralId::kAdc2, AdcChannelId::kChannel9, GpioPortId::kPortC, GpioPinId::kPin3);  // PC3
    DECLARE_ADC_PIN_MAP(AdcPeripheralId::kAdc2,
                        AdcChannelId::kChannel10,
                        GpioPortId::kPortF,
                        GpioPinId::kPin1);  // PF1 (OSC_OUT - Check SB)
    DECLARE_ADC_PIN_MAP(AdcPeripheralId::kAdc2, AdcChannelId::kChannel11, GpioPortId::kPortC, GpioPinId::kPin5);  // PC5
    DECLARE_ADC_PIN_MAP(AdcPeripheralId::kAdc2, AdcChannelId::kChannel12, GpioPortId::kPortB, GpioPinId::kPin2);  // PB2
    DECLARE_ADC_PIN_MAP(AdcPeripheralId::kAdc2, AdcChannelId::kChannel13, GpioPortId::kPortA, GpioPinId::kPin5);  // PA5
    DECLARE_ADC_PIN_MAP(AdcPeripheralId::kAdc2,
                        AdcChannelId::kChannel14,
                        GpioPortId::kPortB,
                        GpioPinId::kPin11);  // PB11
    DECLARE_ADC_PIN_MAP(AdcPeripheralId::kAdc2,
                        AdcChannelId::kChannel15,
                        GpioPortId::kPortB,
                        GpioPinId::kPin15);  // PB15
    DECLARE_ADC_PIN_MAP(AdcPeripheralId::kAdc2, AdcChannelId::kChannel17, GpioPortId::kPortA, GpioPinId::kPin4);  // PA4

    // Adc3
    DECLARE_ADC_PIN_MAP(AdcPeripheralId::kAdc3, AdcChannelId::kChannel1, GpioPortId::kPortB, GpioPinId::kPin1);  // PB1
    DECLARE_ADC_PIN_MAP(AdcPeripheralId::kAdc3, AdcChannelId::kChannel2, GpioPortId::kPortE, GpioPinId::kPin9);  // PE9
    DECLARE_ADC_PIN_MAP(AdcPeripheralId::kAdc3,
                        AdcChannelId::kChannel3,
                        GpioPortId::kPortE,
                        GpioPinId::kPin13);                                                                      // PE13
    DECLARE_ADC_PIN_MAP(AdcPeripheralId::kAdc3, AdcChannelId::kChannel4, GpioPortId::kPortE, GpioPinId::kPin7);  // PE7
    DECLARE_ADC_PIN_MAP(AdcPeripheralId::kAdc3,
                        AdcChannelId::kChannel5,
                        GpioPortId::kPortB,
                        GpioPinId::kPin13);                                                                      // PB13
    DECLARE_ADC_PIN_MAP(AdcPeripheralId::kAdc3, AdcChannelId::kChannel6, GpioPortId::kPortE, GpioPinId::kPin8);  // PE8
    DECLARE_ADC_PIN_MAP(AdcPeripheralId::kAdc3,
                        AdcChannelId::kChannel7,
                        GpioPortId::kPortD,
                        GpioPinId::kPin10);  // PD10
    DECLARE_ADC_PIN_MAP(AdcPeripheralId::kAdc3,
                        AdcChannelId::kChannel8,
                        GpioPortId::kPortD,
                        GpioPinId::kPin11);  // PD11
    DECLARE_ADC_PIN_MAP(AdcPeripheralId::kAdc3,
                        AdcChannelId::kChannel9,
                        GpioPortId::kPortD,
                        GpioPinId::kPin12);  // PD12
    DECLARE_ADC_PIN_MAP(AdcPeripheralId::kAdc3,
                        AdcChannelId::kChannel10,
                        GpioPortId::kPortD,
                        GpioPinId::kPin13);  // PD13
    DECLARE_ADC_PIN_MAP(AdcPeripheralId::kAdc3,
                        AdcChannelId::kChannel11,
                        GpioPortId::kPortD,
                        GpioPinId::kPin14);  // PD14
    DECLARE_ADC_PIN_MAP(AdcPeripheralId::kAdc3, AdcChannelId::kChannel12, GpioPortId::kPortB, GpioPinId::kPin0);  // PB0
    DECLARE_ADC_PIN_MAP(AdcPeripheralId::kAdc3,
                        AdcChannelId::kChannel14,
                        GpioPortId::kPortE,
                        GpioPinId::kPin10);  // PE10
    DECLARE_ADC_PIN_MAP(AdcPeripheralId::kAdc3,
                        AdcChannelId::kChannel15,
                        GpioPortId::kPortE,
                        GpioPinId::kPin11);  // PE11
    DECLARE_ADC_PIN_MAP(AdcPeripheralId::kAdc3,
                        AdcChannelId::kChannel16,
                        GpioPortId::kPortE,
                        GpioPinId::kPin12);  // PE12

    // Adc4
    DECLARE_ADC_PIN_MAP(AdcPeripheralId::kAdc4,
                        AdcChannelId::kChannel1,
                        GpioPortId::kPortE,
                        GpioPinId::kPin14);  // PE14
    DECLARE_ADC_PIN_MAP(AdcPeripheralId::kAdc4,
                        AdcChannelId::kChannel2,
                        GpioPortId::kPortE,
                        GpioPinId::kPin15);  // PE15
    DECLARE_ADC_PIN_MAP(AdcPeripheralId::kAdc4,
                        AdcChannelId::kChannel3,
                        GpioPortId::kPortB,
                        GpioPinId::kPin12);  // PB12
    DECLARE_ADC_PIN_MAP(AdcPeripheralId::kAdc4,
                        AdcChannelId::kChannel4,
                        GpioPortId::kPortB,
                        GpioPinId::kPin14);  // PB14
    DECLARE_ADC_PIN_MAP(AdcPeripheralId::kAdc4,
                        AdcChannelId::kChannel5,
                        GpioPortId::kPortB,
                        GpioPinId::kPin15);                                                                      // PB15
    DECLARE_ADC_PIN_MAP(AdcPeripheralId::kAdc4, AdcChannelId::kChannel6, GpioPortId::kPortE, GpioPinId::kPin8);  // PE8
    DECLARE_ADC_PIN_MAP(AdcPeripheralId::kAdc4,
                        AdcChannelId::kChannel7,
                        GpioPortId::kPortD,
                        GpioPinId::kPin10);  // PD10
    DECLARE_ADC_PIN_MAP(AdcPeripheralId::kAdc4,
                        AdcChannelId::kChannel8,
                        GpioPortId::kPortD,
                        GpioPinId::kPin11);  // PD11
    DECLARE_ADC_PIN_MAP(AdcPeripheralId::kAdc4,
                        AdcChannelId::kChannel9,
                        GpioPortId::kPortD,
                        GpioPinId::kPin12);  // PD12
    DECLARE_ADC_PIN_MAP(AdcPeripheralId::kAdc4,
                        AdcChannelId::kChannel10,
                        GpioPortId::kPortD,
                        GpioPinId::kPin13);  // PD13
    DECLARE_ADC_PIN_MAP(AdcPeripheralId::kAdc4,
                        AdcChannelId::kChannel11,
                        GpioPortId::kPortD,
                        GpioPinId::kPin14);  // PD14
    DECLARE_ADC_PIN_MAP(AdcPeripheralId::kAdc4, AdcChannelId::kChannel12, GpioPortId::kPortD, GpioPinId::kPin8);  // PD8
    DECLARE_ADC_PIN_MAP(AdcPeripheralId::kAdc4, AdcChannelId::kChannel13, GpioPortId::kPortD, GpioPinId::kPin9);  // PD9
    DECLARE_ADC_PIN_MAP(AdcPeripheralId::kAdc4,
                        AdcChannelId::kChannel14,
                        GpioPortId::kPortE,
                        GpioPinId::kPin10);  // PE10
    DECLARE_ADC_PIN_MAP(AdcPeripheralId::kAdc4,
                        AdcChannelId::kChannel15,
                        GpioPortId::kPortE,
                        GpioPinId::kPin11);  // PE11
    DECLARE_ADC_PIN_MAP(AdcPeripheralId::kAdc4,
                        AdcChannelId::kChannel16,
                        GpioPortId::kPortE,
                        GpioPinId::kPin12);  // PE12

    // Adc5
    DECLARE_ADC_PIN_MAP(AdcPeripheralId::kAdc5, AdcChannelId::kChannel1, GpioPortId::kPortA, GpioPinId::kPin8);  // PA8
    DECLARE_ADC_PIN_MAP(AdcPeripheralId::kAdc5, AdcChannelId::kChannel2, GpioPortId::kPortA, GpioPinId::kPin9);  // PA9
    DECLARE_ADC_PIN_MAP(AdcPeripheralId::kAdc5, AdcChannelId::kChannel6, GpioPortId::kPortE, GpioPinId::kPin8);  // PE8
    DECLARE_ADC_PIN_MAP(AdcPeripheralId::kAdc5,
                        AdcChannelId::kChannel7,
                        GpioPortId::kPortD,
                        GpioPinId::kPin10);  // PD10
    DECLARE_ADC_PIN_MAP(AdcPeripheralId::kAdc5,
                        AdcChannelId::kChannel8,
                        GpioPortId::kPortD,
                        GpioPinId::kPin11);  // PD11
    DECLARE_ADC_PIN_MAP(AdcPeripheralId::kAdc5,
                        AdcChannelId::kChannel9,
                        GpioPortId::kPortD,
                        GpioPinId::kPin12);  // PD12
    DECLARE_ADC_PIN_MAP(AdcPeripheralId::kAdc5,
                        AdcChannelId::kChannel10,
                        GpioPortId::kPortD,
                        GpioPinId::kPin13);  // PD13
    DECLARE_ADC_PIN_MAP(AdcPeripheralId::kAdc5,
                        AdcChannelId::kChannel11,
                        GpioPortId::kPortD,
                        GpioPinId::kPin14);  // PD14
    DECLARE_ADC_PIN_MAP(AdcPeripheralId::kAdc5, AdcChannelId::kChannel12, GpioPortId::kPortD, GpioPinId::kPin8);  // PD8
    DECLARE_ADC_PIN_MAP(AdcPeripheralId::kAdc5, AdcChannelId::kChannel13, GpioPortId::kPortD, GpioPinId::kPin9);  // PD9
    DECLARE_ADC_PIN_MAP(AdcPeripheralId::kAdc5,
                        AdcChannelId::kChannel14,
                        GpioPortId::kPortE,
                        GpioPinId::kPin10);  // PE10
    DECLARE_ADC_PIN_MAP(AdcPeripheralId::kAdc5,
                        AdcChannelId::kChannel15,
                        GpioPortId::kPortE,
                        GpioPinId::kPin11);  // PE11
    DECLARE_ADC_PIN_MAP(AdcPeripheralId::kAdc5,
                        AdcChannelId::kChannel16,
                        GpioPortId::kPortE,
                        GpioPinId::kPin12);  // PE12

#undef DECLARE_ADC_PIN_MAP

    template <typename T>
    concept CNullAdcPinMap = requires { typename T::NullPinTag; };

    template <AdcPeripheralId tkPeripheralId, AdcChannelId tkChannelId, GpioPortId tkPortId, GpioPinId tkPin>
    concept CValidAdcPin = !CNullAdcPinMap<AdcPinMap<tkPeripheralId, tkChannelId>> &&
                           (AdcPinMap<tkPeripheralId, tkChannelId>::skPort == tkPortId) &&
                           (AdcPinMap<tkPeripheralId, tkChannelId>::skPin == tkPin);

}  // namespace valle::platform