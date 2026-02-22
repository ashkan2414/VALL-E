#pragma once

#include <stdint.h>

namespace valle
{

    static constexpr size_t kADCMaxRegChannels = 16;
    static constexpr size_t kADCMaxInjChannels = 4;
    static constexpr size_t kADCMaxChannelId   = 20;

    using ADCValue = uint16_t;

    using ADCControllerID = uint8_t;

    template <ADCControllerID tkControllerID>
    constexpr bool kValidADCControllerID = (1 <= tkControllerID && tkControllerID <= 5);

    enum class ADCChannelID : uint32_t
    {
        kChannel0              = LL_ADC_CHANNEL_0,
        kChannel1              = LL_ADC_CHANNEL_1,
        kChannel2              = LL_ADC_CHANNEL_2,
        kChannel3              = LL_ADC_CHANNEL_3,
        kChannel4              = LL_ADC_CHANNEL_4,
        kChannel5              = LL_ADC_CHANNEL_5,
        kChannel6              = LL_ADC_CHANNEL_6,
        kChannel7              = LL_ADC_CHANNEL_7,
        kChannel8              = LL_ADC_CHANNEL_8,
        kChannel9              = LL_ADC_CHANNEL_9,
        kChannel10             = LL_ADC_CHANNEL_10,
        kChannel11             = LL_ADC_CHANNEL_11,
        kChannel12             = LL_ADC_CHANNEL_12,
        kChannel13             = LL_ADC_CHANNEL_13,
        kChannel14             = LL_ADC_CHANNEL_14,
        kChannel15             = LL_ADC_CHANNEL_15,
        kChannel16             = LL_ADC_CHANNEL_16,
        kChannel17             = LL_ADC_CHANNEL_17,
        kChannel18             = LL_ADC_CHANNEL_18,
        kChannelVRefInt        = LL_ADC_CHANNEL_VREFINT,
        kChannelTempSensorADC1 = LL_ADC_CHANNEL_TEMPSENSOR_ADC1,
        kChannelTempSensorADC5 = LL_ADC_CHANNEL_TEMPSENSOR_ADC5,
        kChannelVBat           = LL_ADC_CHANNEL_VBAT,
        kChannelVOPAmp1        = LL_ADC_CHANNEL_VOPAMP1,
        kChannelVOPAmp2        = LL_ADC_CHANNEL_VOPAMP2,
        kChannelVOPAmp3ADC2    = LL_ADC_CHANNEL_VOPAMP3_ADC2,
        kChannelVOPAmp3ADC3    = LL_ADC_CHANNEL_VOPAMP3_ADC3,
        kChannelVOPAmp4        = LL_ADC_CHANNEL_VOPAMP4,
        kChannelVOPAmp5        = LL_ADC_CHANNEL_VOPAMP5,
        kChannelVOPAmp6        = LL_ADC_CHANNEL_VOPAMP6,
    };

    template <ADCControllerID tkControllerID, ADCChannelID tkChannelID>
    constexpr bool kValidADCChannelID =
        kValidADCControllerID<tkControllerID>;  // TODO: Add channel validation per ADC if needed

    using ADCInjectChannelRank = uint8_t;

    template <ADCInjectChannelRank tkRank>
    constexpr bool kValidADCInjectRank = (tkRank >= 1 && tkRank <= 4);

    using ADCRegularChannelRank = uint8_t;

    template <ADCRegularChannelRank tkRank>
    constexpr bool kValidADCRegularRank = (tkRank >= 1 && tkRank <= 16);

}  // namespace valle