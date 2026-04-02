#pragma once

#include "valle/platform/hardware/ADC/id.hpp"
#include "valle/platform/hardware/gpio.hpp"

namespace valle::platform
{
    template <AdcChannelSpec tkChannelSpec>
    struct AdcPinMap
    {
        using NullPinTag = void;
    };

#define DECLARE_ADC_PIN_MAP(tkChannelSpec, tkGpioPinSpec)           \
    template <>                                                     \
    struct AdcPinMap<tkChannelSpec>                                 \
    {                                                               \
        constexpr static GpioPinSpec skGpioPinSpec = tkGpioPinSpec; \
    };

    // Adc1
    DECLARE_ADC_PIN_MAP(kAdc1Channel1Spec, kGpioPinA0Spec);
    DECLARE_ADC_PIN_MAP(kAdc1Channel2Spec, kGpioPinA1Spec);
    DECLARE_ADC_PIN_MAP(kAdc1Channel3Spec, kGpioPinA2Spec);
    DECLARE_ADC_PIN_MAP(kAdc1Channel4Spec, kGpioPinA3Spec);
    DECLARE_ADC_PIN_MAP(kAdc1Channel5Spec, kGpioPinB14Spec);
    DECLARE_ADC_PIN_MAP(kAdc1Channel6Spec, kGpioPinC0Spec);
    DECLARE_ADC_PIN_MAP(kAdc1Channel7Spec, kGpioPinC1Spec);
    DECLARE_ADC_PIN_MAP(kAdc1Channel8Spec, kGpioPinC2Spec);
    DECLARE_ADC_PIN_MAP(kAdc1Channel9Spec, kGpioPinC3Spec);
    DECLARE_ADC_PIN_MAP(kAdc1Channel10Spec, kGpioPinF0Spec);  // (OSC_IN - Check SB)
    DECLARE_ADC_PIN_MAP(kAdc1Channel11Spec, kGpioPinB12Spec);
    DECLARE_ADC_PIN_MAP(kAdc1Channel12Spec, kGpioPinB1Spec);
    DECLARE_ADC_PIN_MAP(kAdc1Channel14Spec, kGpioPinB11Spec);
    DECLARE_ADC_PIN_MAP(kAdc1Channel15Spec, kGpioPinB0Spec);

    // Adc2
    DECLARE_ADC_PIN_MAP(kAdc2Channel1Spec, kGpioPinA0Spec);
    DECLARE_ADC_PIN_MAP(kAdc2Channel2Spec, kGpioPinA1Spec);
    DECLARE_ADC_PIN_MAP(kAdc2Channel3Spec, kGpioPinA6Spec);
    DECLARE_ADC_PIN_MAP(kAdc2Channel4Spec, kGpioPinA7Spec);
    DECLARE_ADC_PIN_MAP(kAdc2Channel5Spec, kGpioPinC4Spec);
    DECLARE_ADC_PIN_MAP(kAdc2Channel6Spec, kGpioPinC0Spec);
    DECLARE_ADC_PIN_MAP(kAdc2Channel7Spec, kGpioPinC1Spec);
    DECLARE_ADC_PIN_MAP(kAdc2Channel8Spec, kGpioPinC2Spec);
    DECLARE_ADC_PIN_MAP(kAdc2Channel9Spec, kGpioPinC3Spec);
    DECLARE_ADC_PIN_MAP(kAdc2Channel10Spec, kGpioPinF1Spec);  // (OSC_OUT - Check SB)
    DECLARE_ADC_PIN_MAP(kAdc2Channel11Spec, kGpioPinC5Spec);
    DECLARE_ADC_PIN_MAP(kAdc2Channel12Spec, kGpioPinB2Spec);
    DECLARE_ADC_PIN_MAP(kAdc2Channel13Spec, kGpioPinA5Spec);
    DECLARE_ADC_PIN_MAP(kAdc2Channel14Spec, kGpioPinB11Spec);
    DECLARE_ADC_PIN_MAP(kAdc2Channel15Spec, kGpioPinB15Spec);
    DECLARE_ADC_PIN_MAP(kAdc2Channel17Spec, kGpioPinA4Spec);

    // Adc3
    DECLARE_ADC_PIN_MAP(kAdc3Channel1Spec, kGpioPinB1Spec);
    DECLARE_ADC_PIN_MAP(kAdc3Channel2Spec, kGpioPinE9Spec);
    DECLARE_ADC_PIN_MAP(kAdc3Channel3Spec, kGpioPinE13Spec);
    DECLARE_ADC_PIN_MAP(kAdc3Channel4Spec, kGpioPinE7Spec);
    DECLARE_ADC_PIN_MAP(kAdc3Channel5Spec, kGpioPinB13Spec);
    DECLARE_ADC_PIN_MAP(kAdc3Channel6Spec, kGpioPinE8Spec);
    DECLARE_ADC_PIN_MAP(kAdc3Channel7Spec, kGpioPinD10Spec);
    DECLARE_ADC_PIN_MAP(kAdc3Channel8Spec, kGpioPinD11Spec);
    DECLARE_ADC_PIN_MAP(kAdc3Channel9Spec, kGpioPinD12Spec);
    DECLARE_ADC_PIN_MAP(kAdc3Channel10Spec, kGpioPinD13Spec);
    DECLARE_ADC_PIN_MAP(kAdc3Channel11Spec, kGpioPinD14Spec);
    DECLARE_ADC_PIN_MAP(kAdc3Channel12Spec, kGpioPinB0Spec);
    DECLARE_ADC_PIN_MAP(kAdc3Channel14Spec, kGpioPinE10Spec);
    DECLARE_ADC_PIN_MAP(kAdc3Channel15Spec, kGpioPinE11Spec);
    DECLARE_ADC_PIN_MAP(kAdc3Channel16Spec, kGpioPinE12Spec);

    // Adc4
    DECLARE_ADC_PIN_MAP(kAdc4Channel1Spec, kGpioPinE14Spec);
    DECLARE_ADC_PIN_MAP(kAdc4Channel2Spec, kGpioPinE15Spec);
    DECLARE_ADC_PIN_MAP(kAdc4Channel3Spec, kGpioPinB12Spec);
    DECLARE_ADC_PIN_MAP(kAdc4Channel4Spec, kGpioPinB14Spec);
    DECLARE_ADC_PIN_MAP(kAdc4Channel5Spec, kGpioPinB15Spec);
    DECLARE_ADC_PIN_MAP(kAdc4Channel6Spec, kGpioPinE8Spec);
    DECLARE_ADC_PIN_MAP(kAdc4Channel7Spec, kGpioPinD10Spec);
    DECLARE_ADC_PIN_MAP(kAdc4Channel8Spec, kGpioPinD11Spec);
    DECLARE_ADC_PIN_MAP(kAdc4Channel9Spec, kGpioPinD12Spec);
    DECLARE_ADC_PIN_MAP(kAdc4Channel10Spec, kGpioPinD13Spec);
    DECLARE_ADC_PIN_MAP(kAdc4Channel11Spec, kGpioPinD14Spec);
    DECLARE_ADC_PIN_MAP(kAdc4Channel12Spec, kGpioPinD8Spec);
    DECLARE_ADC_PIN_MAP(kAdc4Channel13Spec, kGpioPinD9Spec);
    DECLARE_ADC_PIN_MAP(kAdc4Channel14Spec, kGpioPinE10Spec);
    DECLARE_ADC_PIN_MAP(kAdc4Channel15Spec, kGpioPinE11Spec);
    DECLARE_ADC_PIN_MAP(kAdc4Channel16Spec, kGpioPinE12Spec);

    // Adc5
    DECLARE_ADC_PIN_MAP(kAdc5Channel1Spec, kGpioPinA8Spec);
    DECLARE_ADC_PIN_MAP(kAdc5Channel2Spec, kGpioPinA9Spec);
    DECLARE_ADC_PIN_MAP(kAdc5Channel6Spec, kGpioPinE8Spec);
    DECLARE_ADC_PIN_MAP(kAdc5Channel7Spec, kGpioPinD10Spec);
    DECLARE_ADC_PIN_MAP(kAdc5Channel8Spec, kGpioPinD11Spec);
    DECLARE_ADC_PIN_MAP(kAdc5Channel9Spec, kGpioPinD12Spec);
    DECLARE_ADC_PIN_MAP(kAdc5Channel10Spec, kGpioPinD13Spec);
    DECLARE_ADC_PIN_MAP(kAdc5Channel11Spec, kGpioPinD14Spec);
    DECLARE_ADC_PIN_MAP(kAdc5Channel12Spec, kGpioPinD8Spec);
    DECLARE_ADC_PIN_MAP(kAdc5Channel13Spec, kGpioPinD9Spec);
    DECLARE_ADC_PIN_MAP(kAdc5Channel14Spec, kGpioPinE10Spec);
    DECLARE_ADC_PIN_MAP(kAdc5Channel15Spec, kGpioPinE11Spec);
    DECLARE_ADC_PIN_MAP(kAdc5Channel16Spec, kGpioPinE12Spec);

#undef DECLARE_ADC_PIN_MAP

    template <typename T>
    concept CNullAdcPinMap = requires { typename T::NullPinTag; };

    template <AdcControllerId tkControllerId, AdcChannelId tkChannelId, GpioPortId tkPortId, GpioPinId tkPin>
    concept CValidAdcPin = !CNullAdcPinMap<AdcPinMap<tkControllerId, tkChannelId>> &&
                           (AdcPinMap<tkControllerId, tkChannelId>::skPort == tkPortId) &&
                           (AdcPinMap<tkControllerId, tkChannelId>::skPin == tkPin);

}  // namespace valle::platform