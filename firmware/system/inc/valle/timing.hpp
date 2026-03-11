#pragma once

#include "valle/core/timing.hpp"

#if __has_include("valle/platform/system/timing.hpp")
#include "valle/platform/system/timing.hpp"
#endif

namespace valle
{
    using Timing = PlatformTimingUtils;

}  // namespace valle