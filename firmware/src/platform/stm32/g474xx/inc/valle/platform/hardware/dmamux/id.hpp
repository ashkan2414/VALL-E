#pragma once

#include <cstdint>

namespace valle::platform
{
    enum class DmaMuxControllerId : uint8_t
    {
        kDmaMux1 = 1,
    };

    enum class DmaMuxChannelId : uint8_t
    {
        kChannel0  = 0,
        kChannel1  = 1,
        kChannel2  = 2,
        kChannel3  = 3,
        kChannel4  = 4,
        kChannel5  = 5,
        kChannel6  = 6,
        kChannel7  = 7,
        kChannel8  = 8,
        kChannel9  = 9,
        kChannel10 = 10,
        kChannel11 = 11,
        kChannel12 = 12,
        kChannel13 = 13,
        kChannel14 = 14,
        kChannel15 = 15,
    };

    enum class DmaMuxRequestGenChannelId : uint8_t
    {
        kGenerator0 = 0,
        kGenerator1,
        kGenerator2,
        kGenerator3,
    };

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