#pragma once
#include <stdint.h>

namespace valle
{
    enum class TIMControllerID : uint32_t
    {
        kTim1 = 0,
        kTim2,
        kTim3,
        kTim4,
        kTim5,
        kTim6,
        kTim7,
        kTim8,
        kTim15,
        kTim16,
        kTim17,
        kTim20
    };

    enum class TIMChannelID : uint32_t
    {
        kCh1 = 0,
        kCh2,
        kCh3,
        kCh4,
        kCh5,
        kCh6,
    };

}  // namespace valle