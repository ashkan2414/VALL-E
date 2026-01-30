#pragma once

#include <string_view>

namespace valle
{
    /**
     * @enum LogLevel
     * @brief Defines different log levels for filtering log messages.
     */
    enum class LogLevel : uint8_t
    {
        kDebug = 0,
        kInfo,
        kWarn,
        kError,
        kFatal,
    };

    struct LogConfig
    {
        const bool     kEnabled             = true;
        const size_t   kMaxMessageLength    = 512;
        const LogLevel kLogLevel            = LogLevel::kInfo;
        const bool     kPrintSourceLocation = true;
    };

    template <LogConfig config = LogConfig{}>
    struct LoggingConfigBase
    {
        static constexpr LogConfig skConfig = config;
    };

    void log_output_handler(std::string_view) __attribute__((weak));
}  // namespace valle