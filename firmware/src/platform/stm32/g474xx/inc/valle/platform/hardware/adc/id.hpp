#pragma once

#include <cstdint>
#include <magic_enum/magic_enum.hpp>

namespace valle::platform
{

    enum class AdcPeripheralId : uint8_t
    {
        kAdc1 = 1,
        kAdc2 = 2,
        kAdc3 = 3,
        kAdc4 = 4,
        kAdc5 = 5,
    };

    enum class AdcChannelId : uint32_t
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
        kChannelTempSensorAdc1,
        kChannelTempSensorAdc5,
        kChannelVBat,
        kChannelVOPAmp1,
        kChannelVOPAmp2,
        kChannelVOPAmp3Adc2,
        kChannelVOPAmp3Adc3,
        kChannelVOPAmp4,
        kChannelVOPAmp5,
        kChannelVOPAmp6,
    };

    enum class AdcInjectChannelRank : uint8_t
    {
        kRank1 = 1,
        kRank2,
        kRank3,
        kRank4,
    };

    enum class AdcRegularChannelRank : uint8_t
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

    static constexpr size_t kNumAdcChannels    = magic_enum::enum_count<AdcChannelId>();
    static constexpr size_t kAdcMaxRegChannels = 16;
    static constexpr size_t kAdcMaxInjChannels = 4;

    [[nodiscard]] constexpr uint8_t get_adc_controller_number(const AdcPeripheralId controller_id)
    {
        return static_cast<uint8_t>(controller_id);
    }

    template <AdcPeripheralId tkPeripheralId>
    static constexpr uint8_t kAdcControllerNumber = get_adc_controller_number(tkPeripheralId);

    [[nodiscard]] constexpr std::optional<uint8_t> get_adc_channel_number(const AdcChannelId channel_id)
    {
        switch (channel_id)
        {
            case AdcChannelId::kChannel0:
            case AdcChannelId::kChannel1:
            case AdcChannelId::kChannel2:
            case AdcChannelId::kChannel3:
            case AdcChannelId::kChannel4:
            case AdcChannelId::kChannel5:
            case AdcChannelId::kChannel6:
            case AdcChannelId::kChannel7:
            case AdcChannelId::kChannel8:
            case AdcChannelId::kChannel9:
            case AdcChannelId::kChannel10:
            case AdcChannelId::kChannel11:
            case AdcChannelId::kChannel12:
            case AdcChannelId::kChannel13:
            case AdcChannelId::kChannel14:
            case AdcChannelId::kChannel15:
            case AdcChannelId::kChannel16:
            case AdcChannelId::kChannel17:
            case AdcChannelId::kChannel18:
                return static_cast<uint8_t>(static_cast<uint32_t>(channel_id) & 0xFF);
            default:
                return std::nullopt;  // Invalid channel number
        }
    }

    template <AdcChannelId tkChannelId>
    static constexpr uint8_t kAdcChannelNumber = []()
    {
        constexpr auto channel_number = get_adc_channel_number(tkChannelId);
        static_assert(channel_number.has_value(),
                      "Invalid ADC channel ID: does not correspond to a valid channel number");
        return channel_number.value();
    }();

    [[nodiscard]] constexpr uint8_t get_inject_channel_rank_number(const AdcInjectChannelRank rank)
    {
        return static_cast<uint8_t>(rank);
    }

    template <AdcInjectChannelRank tkRank>
    static constexpr uint8_t kAdcInjectChannelRankNumber = get_inject_channel_rank_number(tkRank);

    [[nodiscard]] constexpr uint8_t get_regular_channel_rank_number(const AdcRegularChannelRank rank)
    {
        return static_cast<uint8_t>(rank);
    }

    template <AdcRegularChannelRank tkRank>
    static constexpr uint8_t kAdcRegularChannelRankNumber = get_regular_channel_rank_number(tkRank);

}  // namespace valle::platform