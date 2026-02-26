#pragma once

#include "Valle/Core/Logging/core.hpp"

namespace valle
{
    struct LoggingConfig
        : public LoggingConfigBase<LogConfig{
              .kEnabled = true, .kMaxMessageLength = 512, .kLogLevel = LogLevel::kInfo, .kPrintSourceLocation = true}>
    {
    };
}  // namespace valle
