#pragma once

#include <cstdint>

namespace valle::platform
{
    enum class DmaControllerId : uint8_t
    {
        kDma1 = 1,
        kDma2 = 2,
    };

    enum class DmaChannelId : uint8_t
    {
        kChannel1 = 1,
        kChannel2,
        kChannel3,
        kChannel4,
        kChannel5,
        kChannel6,
        kChannel7,
        kChannel8,
    };

    // ===========================================================================
    // SPECS
    // ==========================================================================
    struct DmaControllerSpec
    {
        DmaControllerId controller_id;
    };

    inline constexpr auto kDma1ControllerSpec = DmaControllerSpec{DmaControllerId::kDma1};
    inline constexpr auto kDma2ControllerSpec = DmaControllerSpec{DmaControllerId::kDma2};

    struct DmaChannelSpec
    {
        DmaControllerId controller_id;
        DmaChannelId    channel_id;

        [[nodiscard]] constexpr DmaControllerSpec controller_spec() const
        {
            return DmaControllerSpec{controller_id};
        }
    };

    inline constexpr auto kDma1Channel1Spec = DmaChannelSpec{DmaControllerId::kDma1, DmaChannelId::kChannel1};
    inline constexpr auto kDma1Channel2Spec = DmaChannelSpec{DmaControllerId::kDma1, DmaChannelId::kChannel2};
    inline constexpr auto kDma1Channel3Spec = DmaChannelSpec{DmaControllerId::kDma1, DmaChannelId::kChannel3};
    inline constexpr auto kDma1Channel4Spec = DmaChannelSpec{DmaControllerId::kDma1, DmaChannelId::kChannel4};
    inline constexpr auto kDma1Channel5Spec = DmaChannelSpec{DmaControllerId::kDma1, DmaChannelId::kChannel5};
    inline constexpr auto kDma1Channel6Spec = DmaChannelSpec{DmaControllerId::kDma1, DmaChannelId::kChannel6};
    inline constexpr auto kDma1Channel7Spec = DmaChannelSpec{DmaControllerId::kDma1, DmaChannelId::kChannel7};
    inline constexpr auto kDma1Channel8Spec = DmaChannelSpec{DmaControllerId::kDma1, DmaChannelId::kChannel8};

    inline constexpr auto kDma2Channel1Spec = DmaChannelSpec{DmaControllerId::kDma2, DmaChannelId::kChannel1};
    inline constexpr auto kDma2Channel2Spec = DmaChannelSpec{DmaControllerId::kDma2, DmaChannelId::kChannel2};
    inline constexpr auto kDma2Channel3Spec = DmaChannelSpec{DmaControllerId::kDma2, DmaChannelId::kChannel3};
    inline constexpr auto kDma2Channel4Spec = DmaChannelSpec{DmaControllerId::kDma2, DmaChannelId::kChannel4};
    inline constexpr auto kDma2Channel5Spec = DmaChannelSpec{DmaControllerId::kDma2, DmaChannelId::kChannel5};
    inline constexpr auto kDma2Channel6Spec = DmaChannelSpec{DmaControllerId::kDma2, DmaChannelId::kChannel6};
    inline constexpr auto kDma2Channel7Spec = DmaChannelSpec{DmaControllerId::kDma2, DmaChannelId::kChannel7};
    inline constexpr auto kDma2Channel8Spec = DmaChannelSpec{DmaControllerId::kDma2, DmaChannelId::kChannel8};

}  // namespace valle::platform