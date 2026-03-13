#pragma once

namespace valle::app
{
    constexpr CTDefaultConfig skPlatformConfig = CTDefaultConfig{.timing_config = {
                                                                     .core_clock_freq_hz = 170'000'000U,
                                                                 }};

}  // namespace valle::app

VALLE_DEFINE_PLATFORM_CT_CONFIG(valle::app::skPlatformConfig);