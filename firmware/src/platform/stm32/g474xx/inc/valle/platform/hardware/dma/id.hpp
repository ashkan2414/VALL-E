#pragma once

#include <cstdint>

namespace valle::platform
{
    enum class DmaPeripheralId : uint8_t
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

    struct DmaChannelFullId
    {
        DmaPeripheralId controller_id = DmaPeripheralId::kDma1;
        DmaChannelId    channel_id    = DmaChannelId::kChannel1;
    };

}  // namespace valle::platform