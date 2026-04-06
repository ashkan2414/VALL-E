#pragma once

#include <cstdint>

#include "stm32g4xx_ll_dma.h"
#include "valle/platform/core/id.hpp"

namespace valle::platform
{
    struct DmaControllerId : public SequentialEnumValue<DmaControllerId, uint8_t, 1, 2>
    {
        static const DmaControllerId kDma1;
        static const DmaControllerId kDma2;
    };

    inline constexpr DmaControllerId DmaControllerId::kDma1 = DmaControllerId::from_number<1>();
    inline constexpr DmaControllerId DmaControllerId::kDma2 = DmaControllerId::from_number<2>();

    inline constexpr auto kDmaChannelIdNumberMapping =
        LLDriverEnumValueSequentialNumberMapping<8>{.start_number = 1,
                                                    .ll_id_table  = {LL_DMA_CHANNEL_1,
                                                                     LL_DMA_CHANNEL_2,
                                                                     LL_DMA_CHANNEL_3,
                                                                     LL_DMA_CHANNEL_4,
                                                                     LL_DMA_CHANNEL_5,
                                                                     LL_DMA_CHANNEL_6,
                                                                     LL_DMA_CHANNEL_7,
                                                                     LL_DMA_CHANNEL_8}};

    struct DmaChannelId : public NumberedLLDriverEnumValue<DmaChannelId, kDmaChannelIdNumberMapping>
    {
        static const DmaChannelId kChannel1;
        static const DmaChannelId kChannel2;
        static const DmaChannelId kChannel3;
        static const DmaChannelId kChannel4;
        static const DmaChannelId kChannel5;
        static const DmaChannelId kChannel6;
        static const DmaChannelId kChannel7;
        static const DmaChannelId kChannel8;
    };

    inline constexpr DmaChannelId DmaChannelId::kChannel1 = DmaChannelId::from_number<1>();
    inline constexpr DmaChannelId DmaChannelId::kChannel2 = DmaChannelId::from_number<2>();
    inline constexpr DmaChannelId DmaChannelId::kChannel3 = DmaChannelId::from_number<3>();
    inline constexpr DmaChannelId DmaChannelId::kChannel4 = DmaChannelId::from_number<4>();
    inline constexpr DmaChannelId DmaChannelId::kChannel5 = DmaChannelId::from_number<5>();
    inline constexpr DmaChannelId DmaChannelId::kChannel6 = DmaChannelId::from_number<6>();
    inline constexpr DmaChannelId DmaChannelId::kChannel7 = DmaChannelId::from_number<7>();
    inline constexpr DmaChannelId DmaChannelId::kChannel8 = DmaChannelId::from_number<8>();

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