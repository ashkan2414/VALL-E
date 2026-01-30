#pragma once

#include "Valle/Core/Logging/core.hpp"
#include "Valle/Drivers/uart_logger.hpp"
#include "Valle/System/hardware.hpp"

namespace valle
{
    struct LoggingConfig
        : public LoggingConfigBase<LogConfig{
              .kEnabled = true, .kMaxMessageLength = 512, .kLogLevel = LogLevel::kInfo, .kPrintSourceLocation = true}>
    {
    };
}  // namespace valle

VALLE_BIND_UART_LOGGER_HANDLER(app::g_drivers.uart_logger);