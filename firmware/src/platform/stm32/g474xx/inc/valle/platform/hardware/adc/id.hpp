#pragma once

#include <cstdint>
#include <magic_enum/magic_enum.hpp>

namespace valle::platform
{

    enum class ADCControllerID : uint8_t
    {
        kADC1 = 1,
        kADC2 = 2,
        kADC3 = 3,
        kADC4 = 4,
        kADC5 = 5,
    };

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

    enum class ADCInjectChannelRank : uint8_t
    {
        kRank1 = 1,
        kRank2,
        kRank3,
        kRank4,
    };

    enum class ADCRegularChannelRank : uint8_t
    {
        kRank1 = 1,
        kRank2,
        kRank3,
        kRank4,
        kRank5,
        kRank6,
        kRank7,
        kRank8,
        kRank9,
        kRank10,
        kRank11,
        kRank12,
        kRank13,
        kRank14,
        kRank15,
        kRank16,
    };

    static constexpr size_t kNumADCChannels    = magic_enum::enum_count<ADCChannelID>();
    static constexpr size_t kADCMaxRegChannels = 16;
    static constexpr size_t kADCMaxInjChannels = 4;

    [[nodiscard]] constexpr uint8_t get_adc_controller_number(const ADCControllerID controller_id)
    {
        return static_cast<uint8_t>(controller_id);
    }

    template <ADCControllerID tkControllerID>
    static constexpr uint8_t kADCControllerNumber = get_adc_controller_number(tkControllerID);

    [[nodiscard]] constexpr std::optional<uint8_t> get_adc_channel_number(const ADCChannelID channel_id)
    {
        switch (channel_id)
        {
            case ADCChannelID::kChannel0:
            case ADCChannelID::kChannel1:
            case ADCChannelID::kChannel2:
            case ADCChannelID::kChannel3:
            case ADCChannelID::kChannel4:
            case ADCChannelID::kChannel5:
            case ADCChannelID::kChannel6:
            case ADCChannelID::kChannel7:
            case ADCChannelID::kChannel8:
            case ADCChannelID::kChannel9:
            case ADCChannelID::kChannel10:
            case ADCChannelID::kChannel11:
            case ADCChannelID::kChannel12:
            case ADCChannelID::kChannel13:
            case ADCChannelID::kChannel14:
            case ADCChannelID::kChannel15:
            case ADCChannelID::kChannel16:
            case ADCChannelID::kChannel17:
            case ADCChannelID::kChannel18:
                return static_cast<uint8_t>(static_cast<uint32_t>(channel_id) & 0xFF);
            default:
                return std::nullopt;  // Invalid channel number
        }
    }

    template <ADCChannelID tkChannelID>
    static constexpr uint8_t kADCChannelNumber = []()
    {
        constexpr auto channel_number = get_adc_channel_number(tkChannelID);
        static_assert(channel_number.has_value(),
                      "Invalid ADC channel ID: does not correspond to a valid channel number");
        return channel_number.value();
    }();

    [[nodiscard]] constexpr uint8_t get_inject_channel_rank_number(const ADCInjectChannelRank rank)
    {
        return static_cast<uint8_t>(rank);
    }

    template <ADCInjectChannelRank tkRank>
    static constexpr uint8_t kADCInjectChannelRankNumber = get_inject_channel_rank_number(tkRank);

    [[nodiscard]] constexpr uint8_t get_regular_channel_rank_number(const ADCRegularChannelRank rank)
    {
        return static_cast<uint8_t>(rank);
    }

    template <ADCRegularChannelRank tkRank>
    static constexpr uint8_t kADCRegularChannelRankNumber = get_regular_channel_rank_number(tkRank);

}  // namespace valle::platform