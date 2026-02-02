#pragma once

#include <array>
#include <magic_enum/magic_enum.hpp>
#include <optional>
#include <string_view>

namespace detail
{
    // --- [Same helpers as before] ---
    consteval bool is_upper(char c)
    {
        return c >= 'A' && c <= 'Z';
    }
    consteval char to_upper(char c)
    {
        return (c >= 'a' && c <= 'z') ? static_cast<char>(c - 'a' + 'A') : c;
    }

    template <size_t N>
    struct FixedString
    {
        static constexpr size_t skSize = N;
        char                    data[N + 1]{};
    };

    consteval size_t calculate_macro_size(std::string_view name)
    {
        size_t size  = 0;
        bool   first = true;
        for (size_t i = 0; i < name.size(); ++i)
        {
            if (i == 0 && name[i] == 'k')
            {
                continue;
            }

            if (is_upper(name[i]) && !first)
            {
                size++;
            }

            size++;
            first = false;
        }
        return size;
    }

    template <size_t N>
    consteval auto to_macro_case(std::string_view name)
    {
        FixedString<N> out{};
        size_t         j     = 0;
        bool           first = true;
        for (size_t i = 0; i < name.size(); ++i)
        {
            if (i == 0 && name[i] == 'k')
            {
                continue;
            }

            if (is_upper(name[i]) && !first)
            {
                out.data[j++] = '_';
            }

            out.data[j++] = to_upper(name[i]);
            first         = false;
        }
        return out;
    }

    // --- [The Magic: Static Storage per Value] ---

    template <auto V>
    struct EnumValueStringStorage
    {
        static constexpr auto             skOriginalName = magic_enum::enum_name(V);
        static constexpr size_t           skSize         = calculate_macro_size(skOriginalName);
        static constexpr auto             skBuffer       = to_macro_case<skSize>(skOriginalName);
        static constexpr std::string_view value{skBuffer.data, skSize};
    };

    // --- [The Table: Static Storage per Enum Type] ---

    template <typename T, size_t... Is>
    consteval auto make_table(std::index_sequence<Is...> indices)
    {
        (void)indices;
        // This creates an array of string_views pointing to the unique static buffers
        return std::array<std::string_view, sizeof...(Is)>{
            EnumValueStringStorage<magic_enum::enum_values<T>()[Is]>::value...};
    }

    template <typename T>
    struct EnumTable
    {
        static constexpr auto skValues = magic_enum::enum_values<T>();
        static constexpr auto skTable  = make_table<T>(std::make_index_sequence<skValues.size()>{});

        [[nodiscard]] static constexpr std::string_view get(T value) noexcept
        {
            auto index = magic_enum::enum_index(value);
            if (index.has_value())
            {
                return skTable[index.value()];
            }
            return "UNKNOWN";
        }
    };
}  // namespace detail

/**
 * @brief Get the macro-style name of an enum value.
 * Works with both compile-time and runtime values!
 */
template <typename TEnum>
constexpr std::string_view enum_name(TEnum value)
{
    return detail::EnumTable<TEnum>::get(value);
}