#pragma once

#include <cstdint>

namespace valle
{
    struct PlatformTimingCTDefaultConfig
    {
        uint32_t core_clock_freq_hz = 170'000'000U;
    };

    struct PlatformCTDefaultConfig
    {
        PlatformTimingCTDefaultConfig timing_config{};
    };

    template <typename T>
    struct PlatformCTConfigTraits
    {
        static constexpr auto skConfig = PlatformCTDefaultConfig{};
        using ConfigT                  = decltype(skConfig);
        using TimingConfigT            = decltype(skConfig.timing_config);
    };

#define VALLE_DEFINE_PLATFORM_CT_CONFIG(config)                            \
    template <>                                                            \
    struct valle::PlatformCTConfigTraits<void>                             \
    {                                                                      \
        static constexpr auto skConfig = config;                           \
        using ConfigT                  = decltype(skConfig);               \
        using TimingConfigT            = decltype(skConfig.timing_config); \
    };

}  // namespace valle