#pragma once

#include <stdint.h>

#include <source_location>
#include <string_view>


namespace valle
{
    namespace detail
    {
        static constexpr std::string_view extract_filename(const std::string_view path)
        {
            size_t pos = path.find_last_of("/\\");
            return (pos == std::string_view::npos) ? path : path.substr(pos + 1);
        }

        static constexpr std::string_view extract_function_name(std::string_view func_name)
        {
            // Find first '(' to remove parameters and return type
            size_t paren_pos = func_name.find('(');
            if (paren_pos != std::string_view::npos)
            {
                func_name = func_name.substr(0, paren_pos);
            }

            // Find first space to remove return type
            size_t space_pos = func_name.rfind(' ');
            if (space_pos != std::string_view::npos)
            {
                func_name = func_name.substr(space_pos + 1);
            }

            // Find last "::" to get only the function name
            size_t last_colon = func_name.rfind("::");
            if (last_colon != std::string_view::npos)
            {
                return func_name.substr(last_colon + 2);  // Extract only function name
            }

            return func_name;
        }
    }  // namespace detail

    /**
     * @brief Compile-time wrapper around std::source_location to store only the filename.
     */
    struct SourceLocation
    {
        std::string_view file_name{};
        std::string_view function_name{};
        uint32_t         line{};
        uint32_t         column{};

        // NOLINTNEXTLINE(hicpp-explicit-conversions)
        consteval SourceLocation(std::source_location loc = std::source_location::current())
            : file_name(detail::extract_filename(loc.file_name()))
            , function_name(detail::extract_function_name(loc.function_name()))
            , line(loc.line())
            , column(loc.column())
        {
        }

        static consteval SourceLocation current(std::source_location loc = std::source_location::current())
        {
            return SourceLocation(loc);
        }
    };
}  // namespace valle