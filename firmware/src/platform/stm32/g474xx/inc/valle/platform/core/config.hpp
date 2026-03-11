#pragma once

#include "valle/platform/core/config_base.hpp"

#if __has_include("app_platform_config.hpp")
#include "app_platform_config.hpp"
#endif

namespace valle::platform
{
    using CTConfigRegistryT          = CTConfigRegistry<void>;
    using PlatformCTConfigT          = typename CTConfigRegistry<void>::ConfigT;
    constexpr auto kPlatformCTConfig = CTConfigRegistry<void>::skConfig;
}  // namespace valle::platform