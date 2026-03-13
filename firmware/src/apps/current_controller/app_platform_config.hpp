#pragma once

#include "valle/platform/core/config_base.hpp"

namespace valle::app
{
    constexpr platform::CTDefaultConfig skPlatformConfig =
        platform::CTDefaultConfig{.timing_config = {
                                      .core_clock_freq_hz = 170'000'000U,
                                  }};

}  // namespace valle::app

VALLE_DEFINE_PLATFORM_CT_CONFIG(valle::app::skPlatformConfig);