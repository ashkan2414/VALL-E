#pragma once
#include <cstdint>

#include "stm32g4xx_ll_tim.h"
#include "valle/platform/core/id.hpp"

namespace valle::platform
{
    struct TimControllerId : public EnumValue<TimControllerId, uint8_t>
    {
        static const TimControllerId kTim1;
        static const TimControllerId kTim2;
        static const TimControllerId kTim3;
        static const TimControllerId kTim4;
        static const TimControllerId kTim5;
        static const TimControllerId kTim6;
        static const TimControllerId kTim7;
        static const TimControllerId kTim8;
        static const TimControllerId kTim15;
        static const TimControllerId kTim16;
        static const TimControllerId kTim17;
        static const TimControllerId kTim20;
    };

    inline constexpr TimControllerId TimControllerId::kTim1  = TimControllerId::from_number<1>();
    inline constexpr TimControllerId TimControllerId::kTim2  = TimControllerId::from_number<2>();
    inline constexpr TimControllerId TimControllerId::kTim3  = TimControllerId::from_number<3>();
    inline constexpr TimControllerId TimControllerId::kTim4  = TimControllerId::from_number<4>();
    inline constexpr TimControllerId TimControllerId::kTim5  = TimControllerId::from_number<5>();
    inline constexpr TimControllerId TimControllerId::kTim6  = TimControllerId::from_number<6>();
    inline constexpr TimControllerId TimControllerId::kTim7  = TimControllerId::from_number<7>();
    inline constexpr TimControllerId TimControllerId::kTim8  = TimControllerId::from_number<8>();
    inline constexpr TimControllerId TimControllerId::kTim15 = TimControllerId::from_number<15>();
    inline constexpr TimControllerId TimControllerId::kTim16 = TimControllerId::from_number<16>();
    inline constexpr TimControllerId TimControllerId::kTim17 = TimControllerId::from_number<17>();
    inline constexpr TimControllerId TimControllerId::kTim20 = TimControllerId::from_number<20>();

    inline constexpr auto kTimChannelIdNumberMapping =
        LLDriverEnumValueSequentialNumberMapping<6>{.start_number = 0,
                                                    .ll_id_table  = {LL_TIM_CHANNEL_CH1,
                                                                     LL_TIM_CHANNEL_CH2,
                                                                     LL_TIM_CHANNEL_CH3,
                                                                     LL_TIM_CHANNEL_CH4,
                                                                     LL_TIM_CHANNEL_CH5,
                                                                     LL_TIM_CHANNEL_CH6}};

    struct TimChannelId : public NumberedLLDriverEnumValue<TimChannelId, kTimChannelIdNumberMapping>
    {
        static const TimChannelId kCh1;
        static const TimChannelId kCh2;
        static const TimChannelId kCh3;
        static const TimChannelId kCh4;
        static const TimChannelId kCh5;
        static const TimChannelId kCh6;
    };

    inline constexpr TimChannelId TimChannelId::kCh1 = TimChannelId::from_number<1>();
    inline constexpr TimChannelId TimChannelId::kCh2 = TimChannelId::from_number<2>();
    inline constexpr TimChannelId TimChannelId::kCh3 = TimChannelId::from_number<3>();
    inline constexpr TimChannelId TimChannelId::kCh4 = TimChannelId::from_number<4>();
    inline constexpr TimChannelId TimChannelId::kCh5 = TimChannelId::from_number<5>();
    inline constexpr TimChannelId TimChannelId::kCh6 = TimChannelId::from_number<6>();

}  // namespace valle::platform