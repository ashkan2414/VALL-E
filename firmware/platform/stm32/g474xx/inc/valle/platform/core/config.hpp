#pragma once

#include "valle/platform/core/config_base.hpp"

#if __has_include("app_platform_config.hpp")
#include "app_platform_config.hpp"
#endif

namespace valle
{
    using PlatformCTConfigTraitsT    = PlatformCTConfigTraits<void>;
    using PlatformCTConfigT          = typename PlatformCTConfigTraits<void>::ConfigT;
    constexpr auto kPlatformCTConfig = PlatformCTConfigTraits<void>::skConfig;
}  // namespace valle