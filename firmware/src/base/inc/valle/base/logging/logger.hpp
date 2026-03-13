#pragma once

#include <fmt/core.h>
#include <fmt/format.h>

#include <array>
#include <cstdint>
#include <cstdio>
#include <functional>
#include <source_location>
#include <span>
#include <string_view>
#include <tuple>
#include <type_traits>

#include "valle/base/logging/config_base.hpp"
#include "valle/utils/enum.hpp"
#include "valle/utils/source_location.hpp"

#if __has_include("app_log_config.hpp")
#include "app_log_config.hpp"
#endif

namespace valle
{
    constexpr LoggingConfig kLoggingConfig = LoggingConfigTraits<void>::skConfig;

    template <LogLevel tkLevel>
    constexpr bool kShouldLog = static_cast<uint8_t>(tkLevel) >= static_cast<uint8_t>(kLoggingConfig.log_level);

    using LogSourceLocationWithData = SourceLocation;

    struct LogEmptySourceLocation
    {
        // NOLINTNEXTLINE(hicpp-explicit-conversions)
        consteval LogEmptySourceLocation(std::source_location loc = std::source_location::current())
        {
            (void)loc;
        }
    };

    using LogSourceLocation =
        std::conditional_t<kLoggingConfig.print_source_location, LogSourceLocationWithData, LogEmptySourceLocation>;

    namespace detail
    {

        template <typename... TArgs>
        struct FormatString
        {
            fmt::format_string<TArgs...> format{};
            LogSourceLocation            loc{};

            /**
             * @brief Constructs a log message that contains message and source information.
             * @param fmt The log message (format string).
             * @param l The source location of the log.
             */
            template <class T>
                requires(std::constructible_from<fmt::format_string<TArgs...>, const T&>)
            inline consteval FormatString(const T& fmt, const LogSourceLocation& l = {}) : format(fmt), loc(l)
            {
            }
        };

        template <typename... TArgs>
        struct EmptyFormatString
        {
            /**
             * @brief Constructs a log message that contains message and source information.
             * @param fmt The log message (format string).
             * @param l The source location of the log.
             */
            template <class T>
                requires(std::constructible_from<fmt::format_string<TArgs...>, T const&>)
            inline consteval EmptyFormatString(const T& fmt, const LogSourceLocation& l = {})
            {
            }

            inline constexpr EmptyFormatString(const FormatString<TArgs...>& fmt)
            {
            }
        };

        /**
         * @brief Helper struct to capture log message and context information.
         * @tparam tkLevel The log level of the message.
         * @tparam TArgs The types of the arguments for formatting.
         */
        template <LogLevel tkLevel, typename... TArgs>
        struct FormatStringWithLevel
            : public std::conditional_t<kShouldLog<tkLevel>, FormatString<TArgs...>, EmptyFormatString<TArgs...>>
        {
            using BaseT = std::conditional_t<kShouldLog<tkLevel>, FormatString<TArgs...>, EmptyFormatString<TArgs...>>;
            using BaseT::BaseT;

            // NOLINTNEXTLINE(hicpp-explicit-conversions)
            constexpr FormatStringWithLevel(const FormatString<TArgs...>& other) : BaseT(other)
            {
            }
        };

    }  // namespace detail

    template <typename... TArgs>
    using FormatString = detail::FormatString<std::type_identity_t<TArgs>...>;

    template <LogLevel tkLevel, typename... TArgs>
    using FormatStringWithLevel = detail::FormatStringWithLevel<tkLevel, std::type_identity_t<TArgs>...>;

    /**
     * @class Logger
     * @brief Lightweight logging system for embedded environments.
     */
    class Logger
    {
    public:
        /**
         * @brief Logs a message with the given log level and contextual information.
         * @param format The log message with level and context.
         */
        template <LogLevel tkLevel, typename... TArgs>
        void log(const FormatStringWithLevel<tkLevel, TArgs...>& format, TArgs&&... args)
        {
            if constexpr (kLoggingConfig.enabled && kShouldLog<tkLevel>)
            {
                // Fixed-size buffer to avoid dynamic allocation
                std::array<char, kLoggingConfig.max_message_length> buffer{};
                auto                                               current_buffer =
                    std::span<char>(buffer.data(), buffer.size() - 1);  // Reserve space for null terminator

                // Format location
                if constexpr (kLoggingConfig.print_source_location)
                {
                    if (current_buffer.size() > 1)
                    {
                        auto location_format_result = fmt::format_to_n(current_buffer.data(),
                                                                       current_buffer.size() - 1,
                                                                       "[{} in {}:{}:{}]",
                                                                       format.loc.function_name,
                                                                       format.loc.file_name,
                                                                       format.loc.line,
                                                                       format.loc.column);

                        current_buffer = current_buffer.subspan(location_format_result.size);
                    }
                }

                // Format level
                if (current_buffer.size() > 1)
                {
                    auto level_format_result = fmt::format_to_n(
                        current_buffer.data(), current_buffer.size() - 1, "[{}]: ", enum_name(tkLevel));
                    current_buffer = current_buffer.subspan(level_format_result.size);
                }

                // Format main message
                if (current_buffer.size() > 1)
                {
                    auto message_format_result = fmt::format_to_n(
                        current_buffer.data(), current_buffer.size() - 1, format.format, std::forward<TArgs>(args)...);

                    current_buffer = current_buffer.subspan(message_format_result.size);
                }

                // Append newline (there should always be space for this)
                current_buffer[0] = '\n';
                current_buffer    = current_buffer.subspan(1);

                log_output_handler(std::string_view(buffer.data(), buffer.size() - current_buffer.size()));
            }
        }

        template <LogLevel tkLevel, typename... TArgs>
        void log(const FormatString<TArgs...>& format, TArgs&&... args)
        {
            log(FormatStringWithLevel<tkLevel, TArgs...>(format), std::forward<TArgs>(args)...);
        }

        template <typename... TArgs>
        void debug(const FormatString<TArgs...>& format, TArgs&&... args)
        {
            log(FormatStringWithLevel<LogLevel::kDebug, TArgs...>(format), std::forward<TArgs>(args)...);
        }

        template <typename... TArgs>
        void info(const FormatString<TArgs...>& format, TArgs&&... args)
        {
            log(FormatStringWithLevel<LogLevel::kInfo, TArgs...>(format), std::forward<TArgs>(args)...);
        }

        template <typename... TArgs>
        void warn(const FormatString<TArgs...>& format, TArgs&&... args)
        {
            log(FormatStringWithLevel<LogLevel::kWarn, TArgs...>(format), std::forward<TArgs>(args)...);
        }

        template <typename... TArgs>
        void error(const FormatString<TArgs...>& format, TArgs&&... args)
        {
            log(FormatStringWithLevel<LogLevel::kError, TArgs...>(format), std::forward<TArgs>(args)...);
        }

        template <typename... TArgs>
        void fatal(const FormatString<TArgs...>& format, TArgs&&... args)
        {
            log(FormatStringWithLevel<LogLevel::kFatal, TArgs...>(format), std::forward<TArgs>(args)...);
        }
    };

    extern Logger g_logger;

}  // namespace valle

#define VALLE_LOG(level, fmt, ...) valle::g_logger.log<(level)>((fmt), ##__VA_ARGS__)
#define VALLE_LOG_DEBUG(fmt, ...)  valle::g_logger.debug((fmt), ##__VA_ARGS__)
#define VALLE_LOG_INFO(fmt, ...)   valle::g_logger.info((fmt), ##__VA_ARGS__)
#define VALLE_LOG_WARN(fmt, ...)   valle::g_logger.warn((fmt), ##__VA_ARGS__)
#define VALLE_LOG_ERROR(fmt, ...)  valle::g_logger.error((fmt), ##__VA_ARGS__)
#define VALLE_LOG_FATAL(fmt, ...)  valle::g_logger.fatal((fmt), ##__VA_ARGS__)