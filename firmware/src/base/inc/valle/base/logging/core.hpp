#pragma once

#include <cstdint>
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

    void log_output_handler(std::string_view) __attribute__((weak));
}  // namespace valle