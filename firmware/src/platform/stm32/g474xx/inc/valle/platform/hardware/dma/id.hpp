#pragma once

#include <cstdint>

namespace valle::platform
{
    enum class DMAControllerID : uint8_t
    {
        kDMA1 = 1,
        kDMA2 = 2,
    };

    enum class DMAChannelID : uint8_t
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

    struct DMAChannelFullID
    {
        DMAControllerID controller_id = DMAControllerID::kDMA1;
        DMAChannelID    channel_id    = DMAChannelID::kChannel1;
    };

}  // namespace valle::platform