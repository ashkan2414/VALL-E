#pragma once

#include <cstdint>

namespace valle::platform
{
    struct PlatformTimingCTDefaultConfig
    {
        uint32_t core_clock_freq_hz = 170'000'000U;
    };

    struct CTDefaultConfig
    {
        PlatformTimingCTDefaultConfig timing_config{};
    };

    template <typename T>
    struct CTConfigTraits
    {
        static constexpr auto skConfig = CTDefaultConfig{};
        using ConfigT                  = decltype(skConfig);
        using TimingConfigT            = decltype(skConfig.timing_config);
    };

#define VALLE_DEFINE_PLATFORM_CT_CONFIG(config)                                \
    namespace valle::platform                                                  \
    {                                                                          \
        template <>                                                            \
        struct CTConfigTraits<void>                                            \
        {                                                                      \
            static constexpr auto skConfig = config;                           \
            using ConfigT                  = decltype(skConfig);               \
            using TimingConfigT            = decltype(skConfig.timing_config); \
        };                                                                     \
    }

}  // namespace valle::platform