#pragma once

#include <stdint.h>

#include <magic_enum/magic_enum.hpp>

namespace valle
{

    static constexpr size_t kADCMaxRegChannels = 16;
    static constexpr size_t kADCMaxInjChannels = 4;

    using ADCValue = uint16_t;

    using ADCControllerID = uint8_t;

    template <ADCControllerID tkControllerID>
    constexpr bool kValidADCControllerID = (1 <= tkControllerID && tkControllerID <= 5);

    enum class ADCChannelID : uint32_t
    {
        kChannel0 = 0,
        kChannel1,
        kChannel2,
        kChannel3,
        kChannel4,
        kChannel5,
        kChannel6,
        kChannel7,
        kChannel8,
        kChannel9,
        kChannel10,
        kChannel11,
        kChannel12,
        kChannel13,
        kChannel14,
        kChannel15,
        kChannel16,
        kChannel17,
        kChannel18,
        kChannelVRefInt,
        kChannelTempSensorADC1,
        kChannelTempSensorADC5,
        kChannelVBat,
        kChannelVOPAmp1,
        kChannelVOPAmp2,
        kChannelVOPAmp3ADC2,
        kChannelVOPAmp3ADC3,
        kChannelVOPAmp4,
        kChannelVOPAmp5,
        kChannelVOPAmp6,
    };

    static constexpr size_t kNumADCChannels = magic_enum::enum_count<ADCChannelID>();

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