#pragma once

#include <cstdint>

namespace valle
{
    using DMAMuxControllerID = uint8_t;

    template <DMAMuxControllerID tkControllerID>
    constexpr bool kValidDMAMuxControllerID = (tkControllerID == 1);
}  // namespace valle