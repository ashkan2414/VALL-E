#pragma once

#include <string_view>

#include "valle/utils/source_location.hpp"

namespace valle
{
    struct PanicSourceInfo
    {
        std::string_view message{};
        SourceLocation   location{};
    };

    void panic_handler(const PanicSourceInfo& info);

    /**
     * @brief This function logs a panic message and halts the system.
     * @param message The panic message to log.
     * @param location The source location where the panic occurred.
     */
    // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
    constexpr void panic(std::string_view message, SourceLocation location = {})
    {
        if (std::is_constant_evaluated())
        {
            __builtin_trap();
        }
        else
        {
            panic_handler(PanicSourceInfo{.message = message, .location = location});
        }
    }

    /**
     * @brief This function checks a condition and panics if the condition is false.
     * @param condition The condition to check.
     * @param message The panic message to log if the condition is false.
     * @param location The source location where the panic occurred.
     */
    constexpr void expect(const bool             condition,
                          const std::string_view message,
                          SourceLocation         location = SourceLocation::current())
    {
        if (!condition)
        {
            panic(message, location);
        }
    }
}  // namespace valle