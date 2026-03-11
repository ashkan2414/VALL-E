#pragma once

#include "valle/core/logging/core.hpp"

namespace valle
{
    struct LoggingConfig
        : public LoggingConfigBase<LogConfig{
              .kEnabled = true, .kMaxMessageLength = 512, .kLogLevel = LogLevel::kInfo, .kPrintSourceLocation = false}>
    {
    };
}  // namespace valle
