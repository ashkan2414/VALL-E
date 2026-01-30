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
        kError
    };

    inline constexpr std::string_view log_level_name(const LogLevel level)
    {
        switch (level)
        {
            case LogLevel::kDebug:
                return "DEBUG";
            case LogLevel::kInfo:
                return "INFO";
            case LogLevel::kWarn:
                return "WARN";
            case LogLevel::kError:
                return "ERROR";
            default:
                return "UNKNOWN";
        }
    }

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

    template <typename T = void>
    struct LoggingOutputHandler
    {
        static void output(const std::string_view message)
        {
        }
    };

}  // namespace valle