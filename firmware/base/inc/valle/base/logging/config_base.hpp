#pragma once

#include "valle/base/logging/core.hpp"

namespace valle
{
    struct LoggingConfig
    {
        bool     enabled               = true;
        size_t   max_message_length    = 512;
        LogLevel log_level             = LogLevel::kInfo;
        bool     print_source_location = false;
    };

    template <typename T = void>
    struct LoggingConfigTraits
    {
        static constexpr LoggingConfig skConfig = LoggingConfig{};
    };

}  // namespace valle

#define VALLE_DEFINE_LOGGING_CONFIG(config)                   \
    namespace valle                                           \
    {                                                         \
        template <>                                           \
        struct LoggingConfigTraits<void>                      \
        {                                                     \
            static constexpr LoggingConfig skConfig = config; \
        };                                                    \
    }