#pragma once

#include <stdint.h>

namespace valle
{
    using DMAControllerID = uint8_t;

    template <DMAControllerID tkControllerID>
    constexpr bool kValidDMAControllerID = (tkControllerID == 1 || tkControllerID == 2);

    using DMAChannelID = uint8_t;

    template <DMAChannelID tkChannelID>
    constexpr bool kValidDMAChannel = (tkChannelID >= 1 && tkChannelID <= 8);

    struct DMAChannelFullID
    {
        DMAControllerID controller_id = 0;
        DMAChannelID    channel_id    = 0;
    };

}  // namespace valle