#pragma once

#include "Valle/Utils/source_location.hpp"

namespace valle
{
    /**
     * @brief This function logs a panic message and halts the system.
     * @param message The panic message to log.
     * @param location The source location where the panic occurred.
     */
    void panic(std::string_view message, SourceLocation location);

    /**
     * @brief This function checks a condition and panics if the condition is false.
     * @param condition The condition to check.
     * @param message The panic message to log if the condition is false.
     * @param location The source location where the panic occurred.
     */
    inline void expect(const bool             condition,
                       const std::string_view message,
                       SourceLocation         location = SourceLocation::current())
    {
        if (!condition)
        {
            panic(message, location);
        }
    }

}  // namespace valle