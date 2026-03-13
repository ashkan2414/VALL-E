#pragma once

#include "valle/platform/core/config_base.hpp"

#if __has_include("app_platform_config.hpp")
#include "app_platform_config.hpp"
#endif

namespace valle::platform
{
    using CTConfigTraitsT            = CTConfigTraits<void>;
    using PlatformCTConfigT          = typename CTConfigTraits<void>::ConfigT;
    constexpr auto kPlatformCTConfig = CTConfigTraits<void>::skConfig;
}  // namespace valle::platform