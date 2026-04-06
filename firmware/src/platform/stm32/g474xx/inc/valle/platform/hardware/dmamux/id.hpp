#pragma once

#include <cstdint>

#include "stm32g4xx_ll_dmamux.h"
#include "valle/platform/core/id.hpp"

namespace valle::platform
{
    struct DmaMuxControllerId : public SequentialEnumValue<DmaMuxControllerId, uint8_t, 1, 1>
    {
        static const DmaMuxControllerId kDmaMux1;
    };

    inline constexpr DmaMuxControllerId DmaMuxControllerId::kDmaMux1 = DmaMuxControllerId::from_number<1>();

    inline constexpr auto kDmaMuxChannelIdNumberMapping =
        LLDriverEnumValueSequentialNumberMapping<16>{.start_number = 0,
                                                     .ll_id_table  = {LL_DMAMUX_CHANNEL_0,
                                                                      LL_DMAMUX_CHANNEL_1,
                                                                      LL_DMAMUX_CHANNEL_2,
                                                                      LL_DMAMUX_CHANNEL_3,
                                                                      LL_DMAMUX_CHANNEL_4,
                                                                      LL_DMAMUX_CHANNEL_5,
                                                                      LL_DMAMUX_CHANNEL_6,
                                                                      LL_DMAMUX_CHANNEL_7,
                                                                      LL_DMAMUX_CHANNEL_8,
                                                                      LL_DMAMUX_CHANNEL_9,
                                                                      LL_DMAMUX_CHANNEL_10,
                                                                      LL_DMAMUX_CHANNEL_11,
                                                                      LL_DMAMUX_CHANNEL_12,
                                                                      LL_DMAMUX_CHANNEL_13,
                                                                      LL_DMAMUX_CHANNEL_14,
                                                                      LL_DMAMUX_CHANNEL_15}};

    struct DmaMuxChannelId : public NumberedLLDriverEnumValue<DmaMuxChannelId, kDmaMuxChannelIdNumberMapping>
    {
        static const DmaMuxChannelId kChannel0;
        static const DmaMuxChannelId kChannel1;
        static const DmaMuxChannelId kChannel2;
        static const DmaMuxChannelId kChannel3;
        static const DmaMuxChannelId kChannel4;
        static const DmaMuxChannelId kChannel5;
        static const DmaMuxChannelId kChannel6;
        static const DmaMuxChannelId kChannel7;
        static const DmaMuxChannelId kChannel8;
        static const DmaMuxChannelId kChannel9;
        static const DmaMuxChannelId kChannel10;
        static const DmaMuxChannelId kChannel11;
        static const DmaMuxChannelId kChannel12;
        static const DmaMuxChannelId kChannel13;
        static const DmaMuxChannelId kChannel14;
        static const DmaMuxChannelId kChannel15;
    };

    inline constexpr DmaMuxChannelId DmaMuxChannelId::kChannel0  = DmaMuxChannelId::from_number<0>();
    inline constexpr DmaMuxChannelId DmaMuxChannelId::kChannel1  = DmaMuxChannelId::from_number<1>();
    inline constexpr DmaMuxChannelId DmaMuxChannelId::kChannel2  = DmaMuxChannelId::from_number<2>();
    inline constexpr DmaMuxChannelId DmaMuxChannelId::kChannel3  = DmaMuxChannelId::from_number<3>();
    inline constexpr DmaMuxChannelId DmaMuxChannelId::kChannel4  = DmaMuxChannelId::from_number<4>();
    inline constexpr DmaMuxChannelId DmaMuxChannelId::kChannel5  = DmaMuxChannelId::from_number<5>();
    inline constexpr DmaMuxChannelId DmaMuxChannelId::kChannel6  = DmaMuxChannelId::from_number<6>();
    inline constexpr DmaMuxChannelId DmaMuxChannelId::kChannel7  = DmaMuxChannelId::from_number<7>();
    inline constexpr DmaMuxChannelId DmaMuxChannelId::kChannel8  = DmaMuxChannelId::from_number<8>();
    inline constexpr DmaMuxChannelId DmaMuxChannelId::kChannel9  = DmaMuxChannelId::from_number<9>();
    inline constexpr DmaMuxChannelId DmaMuxChannelId::kChannel10 = DmaMuxChannelId::from_number<10>();
    inline constexpr DmaMuxChannelId DmaMuxChannelId::kChannel11 = DmaMuxChannelId::from_number<11>();
    inline constexpr DmaMuxChannelId DmaMuxChannelId::kChannel12 = DmaMuxChannelId::from_number<12>();
    inline constexpr DmaMuxChannelId DmaMuxChannelId::kChannel13 = DmaMuxChannelId::from_number<13>();
    inline constexpr DmaMuxChannelId DmaMuxChannelId::kChannel14 = DmaMuxChannelId::from_number<14>();
    inline constexpr DmaMuxChannelId DmaMuxChannelId::kChannel15 = DmaMuxChannelId::from_number<15>();

    inline constexpr auto kDmaMuxRequestGenChannelIdNumberMapping = LLDriverEnumValueSequentialNumberMapping<4>{
        .start_number = 0,
        .ll_id_table  = {LL_DMAMUX_REQ_GEN_0, LL_DMAMUX_REQ_GEN_1, LL_DMAMUX_REQ_GEN_2, LL_DMAMUX_REQ_GEN_3}};

    struct DmaMuxRequestGenChannelId
        : public NumberedLLDriverEnumValue<DmaMuxRequestGenChannelId, kDmaMuxRequestGenChannelIdNumberMapping>
    {
        static const DmaMuxRequestGenChannelId kGenerator0;
        static const DmaMuxRequestGenChannelId kGenerator1;
        static const DmaMuxRequestGenChannelId kGenerator2;
        static const DmaMuxRequestGenChannelId kGenerator3;
    };

    inline constexpr DmaMuxRequestGenChannelId DmaMuxRequestGenChannelId::kGenerator0 =
        DmaMuxRequestGenChannelId::from_number<0>();
    inline constexpr DmaMuxRequestGenChannelId DmaMuxRequestGenChannelId::kGenerator1 =
        DmaMuxRequestGenChannelId::from_number<1>();
    inline constexpr DmaMuxRequestGenChannelId DmaMuxRequestGenChannelId::kGenerator2 =
        DmaMuxRequestGenChannelId::from_number<2>();
    inline constexpr DmaMuxRequestGenChannelId DmaMuxRequestGenChannelId::kGenerator3 =
        DmaMuxRequestGenChannelId::from_number<3>();

    // ============================================================================
    // SPECIFIERS
    // ============================================================================
    struct DmaMuxControllerSpec
    {
        DmaMuxControllerId controller_id;
    };

    inline constexpr auto kDmaMux1ControllerSpec = DmaMuxControllerSpec{DmaMuxControllerId::kDmaMux1};

    struct DmaMuxChannelSpec
    {
        DmaMuxControllerId controller_id;
        DmaMuxChannelId    channel_id;

        [[nodiscard]] constexpr DmaMuxControllerSpec controller_spec() const
        {
            return DmaMuxControllerSpec{controller_id};
        }
    };

    inline constexpr auto kDmaMux1Channel0Spec =
        DmaMuxChannelSpec{DmaMuxControllerId::kDmaMux1, DmaMuxChannelId::kChannel0};
    inline constexpr auto kDmaMux1Channel1Spec =
        DmaMuxChannelSpec{DmaMuxControllerId::kDmaMux1, DmaMuxChannelId::kChannel1};
    inline constexpr auto kDmaMux1Channel2Spec =
        DmaMuxChannelSpec{DmaMuxControllerId::kDmaMux1, DmaMuxChannelId::kChannel2};
    inline constexpr auto kDmaMux1Channel3Spec =
        DmaMuxChannelSpec{DmaMuxControllerId::kDmaMux1, DmaMuxChannelId::kChannel3};
    inline constexpr auto kDmaMux1Channel4Spec =
        DmaMuxChannelSpec{DmaMuxControllerId::kDmaMux1, DmaMuxChannelId::kChannel4};
    inline constexpr auto kDmaMux1Channel5Spec =
        DmaMuxChannelSpec{DmaMuxControllerId::kDmaMux1, DmaMuxChannelId::kChannel5};
    inline constexpr auto kDmaMux1Channel6Spec =
        DmaMuxChannelSpec{DmaMuxControllerId::kDmaMux1, DmaMuxChannelId::kChannel6};
    inline constexpr auto kDmaMux1Channel7Spec =
        DmaMuxChannelSpec{DmaMuxControllerId::kDmaMux1, DmaMuxChannelId::kChannel7};
    inline constexpr auto kDmaMux1Channel8Spec =
        DmaMuxChannelSpec{DmaMuxControllerId::kDmaMux1, DmaMuxChannelId::kChannel8};
    inline constexpr auto kDmaMux1Channel9Spec =
        DmaMuxChannelSpec{DmaMuxControllerId::kDmaMux1, DmaMuxChannelId::kChannel9};
    inline constexpr auto kDmaMux1Channel10Spec =
        DmaMuxChannelSpec{DmaMuxControllerId::kDmaMux1, DmaMuxChannelId::kChannel10};
    inline constexpr auto kDmaMux1Channel11Spec =
        DmaMuxChannelSpec{DmaMuxControllerId::kDmaMux1, DmaMuxChannelId::kChannel11};
    inline constexpr auto kDmaMux1Channel12Spec =
        DmaMuxChannelSpec{DmaMuxControllerId::kDmaMux1, DmaMuxChannelId::kChannel12};
    inline constexpr auto kDmaMux1Channel13Spec =
        DmaMuxChannelSpec{DmaMuxControllerId::kDmaMux1, DmaMuxChannelId::kChannel13};
    inline constexpr auto kDmaMux1Channel14Spec =
        DmaMuxChannelSpec{DmaMuxControllerId::kDmaMux1, DmaMuxChannelId::kChannel14};
    inline constexpr auto kDmaMux1Channel15Spec =
        DmaMuxChannelSpec{DmaMuxControllerId::kDmaMux1, DmaMuxChannelId::kChannel15};

    struct DmaMuxRequestGenChannelSpec
    {
        DmaMuxControllerId        controller_id;
        DmaMuxRequestGenChannelId generator_id;

        [[nodiscard]] constexpr DmaMuxControllerSpec controller_spec() const
        {
            return DmaMuxControllerSpec{controller_id};
        }
    };

    inline constexpr auto kDmaMux1RequestGenChannel0Spec =
        DmaMuxRequestGenChannelSpec{DmaMuxControllerId::kDmaMux1, DmaMuxRequestGenChannelId::kGenerator0};
    inline constexpr auto kDmaMux1RequestGenChannel1Spec =
        DmaMuxRequestGenChannelSpec{DmaMuxControllerId::kDmaMux1, DmaMuxRequestGenChannelId::kGenerator1};
    inline constexpr auto kDmaMux1RequestGenChannel2Spec =
        DmaMuxRequestGenChannelSpec{DmaMuxControllerId::kDmaMux1, DmaMuxRequestGenChannelId::kGenerator2};
    inline constexpr auto kDmaMux1RequestGenChannel3Spec =
        DmaMuxRequestGenChannelSpec{DmaMuxControllerId::kDmaMux1, DmaMuxRequestGenChannelId::kGenerator3};

}  // namespace valle::platform