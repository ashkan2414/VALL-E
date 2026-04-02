#pragma once

#include <cstdint>

#include "valle/base/panic.hpp"
#include "valle/utils/compiler.hpp"

namespace valle::platform
{
    // ============================================================================
    // ENUM VALUE
    // ============================================================================

    template <typename TDerived, typename TRep>
    class EnumValue
    {
    public:
        using RepT = TRep;

    private:
        RepT m_value;

    protected:
        explicit constexpr EnumValue(const RepT value) : m_value(value)
        {
        }

    public:
        [[nodiscard]] static constexpr VALLE_FORCE_INLINE TDerived from_value(const uint32_t value)
        {
            return make_derived(value);
        }

        template <uint32_t tkValue>
        [[nodiscard]] static constexpr VALLE_FORCE_INLINE TDerived from_value()
        {
            return make_derived(tkValue);
        }

        [[nodiscard]] constexpr uint32_t to_value() const
        {
            return m_value;
        }

        [[nodiscard]] constexpr bool VALLE_FORCE_INLINE operator==(const EnumValue&) const = default;

    private:
        [[nodiscard]] static constexpr VALLE_FORCE_INLINE TDerived make_derived(const RepT value)
        {
            struct Raw final : EnumValue
            {
                explicit constexpr Raw(const RepT v) : EnumValue(v)
                {
                }
            };

            return static_cast<TDerived>(Raw{value});
        }
    };

    // ============================================================================
    // SEQUENTIAL ENUM VALUE
    // ============================================================================

    template <typename TDerived, typename TRep, TRep tkStartNum, TRep tkNumIds>
    struct SequentialEnumValue
    {
    public:
        using RepT                       = TRep;
        static constexpr RepT skStartNum = tkStartNum;
        static constexpr RepT skNumIds   = tkNumIds;
        static constexpr RepT skEndNum   = tkStartNum + tkNumIds - 1;

    private:
        RepT m_value;

    protected:
        explicit constexpr VALLE_FORCE_INLINE SequentialEnumValue(const RepT value) : m_value(value)
        {
        }

    public:
        [[nodiscard]] static constexpr VALLE_FORCE_INLINE std::optional<TDerived> from_number(const RepT number)
        {
            if (number < skStartNum || number > skEndNum)
            {
                return std::nullopt;
            }
            return make_derived(number);
        }

        template <RepT tkNumber>
            requires(tkNumber >= tkStartNum && tkNumber <= skEndNum)
        [[nodiscard]] static constexpr VALLE_FORCE_INLINE TDerived from_number()
        {
            return make_derived(tkNumber);
        }

        [[nodiscard]] constexpr RepT to_number() const
        {
            return m_value;
        }

        [[nodiscard]] static constexpr VALLE_FORCE_INLINE std::optional<TDerived> from_index(const RepT index)
        {
            if (index >= skNumIds)
            {
                return std::nullopt;
            }
            return make_derived(index + skStartNum);
        }

        template <RepT tkIndex>
            requires(tkIndex < skNumIds)
        [[nodiscard]] static constexpr VALLE_FORCE_INLINE TDerived from_index()
        {
            return make_derived(tkIndex + skStartNum);
        }

        [[nodiscard]] constexpr RepT to_index() const
        {
            return m_value - skStartNum;
        }

        [[nodiscard]] constexpr bool VALLE_FORCE_INLINE operator==(const SequentialEnumValue&) const = default;

    private:
        [[nodiscard]] static constexpr VALLE_FORCE_INLINE TDerived make_derived(const RepT value)
        {
            struct Raw final : SequentialEnumValue
            {
                explicit constexpr Raw(const RepT v) : SequentialEnumValue(v)
                {
                }
            };

            return static_cast<TDerived>(Raw{value});
        }
    };

    // ============================================================================
    // LL DRIVER ENUM VALUE
    // ============================================================================
    template <typename TDerived>
    class LLDriverEnumValue
    {
    private:
        uint32_t m_id;

    protected:
        explicit constexpr VALLE_FORCE_INLINE LLDriverEnumValue(const uint32_t ll_id) : m_id(ll_id)
        {
        }

    public:
        [[nodiscard]] static constexpr VALLE_FORCE_INLINE TDerived from_ll_id(const uint32_t ll_id)
        {
            return make_derived(ll_id);
        }

        template <uint32_t tkLLId>
        [[nodiscard]] static constexpr VALLE_FORCE_INLINE TDerived from_ll_id()
        {
            return make_derived(tkLLId);
        }

        [[nodiscard]] constexpr VALLE_FORCE_INLINE uint32_t to_ll_id() const
        {
            return m_id;
        }

        [[nodiscard]] constexpr VALLE_FORCE_INLINE bool operator==(const LLDriverEnumValue&) const = default;

    private:
        [[nodiscard]] static constexpr VALLE_FORCE_INLINE TDerived make_derived(const uint32_t ll_id)
        {
            struct Raw final : LLDriverEnumValue
            {
                explicit constexpr VALLE_FORCE_INLINE Raw(const uint32_t v) : LLDriverEnumValue(v)
                {
                }
            };

            return static_cast<TDerived>(Raw{ll_id});
        }
    };

    // ============================================================================
    // NUMBERED LL DRIVER ENUM VALUE
    // ============================================================================

    template <size_t tkNumIds>
    struct LLDriverEnumValueNumberMapping
    {
        std::array<std::pair<uint32_t, uint32_t>, tkNumIds> mapping;

        [[nodiscard]] constexpr VALLE_FORCE_INLINE bool contains_number(const uint32_t number) const
        {
            for (const auto& pair : mapping)
            {
                if (pair.second == number)
                {
                    return true;
                }
            }
            return false;
        }

        [[nodiscard]] constexpr VALLE_FORCE_INLINE std::optional<uint32_t> ll_id_to_number(const uint32_t ll_id) const
        {
            for (const auto& pair : mapping)
            {
                if (pair.first == ll_id)
                {
                    return pair.second;
                }
            }
            return std::nullopt;
        }

        [[nodiscard]] constexpr VALLE_FORCE_INLINE std::optional<uint32_t> number_to_ll_id(const uint32_t number) const
        {
            for (const auto& pair : mapping)
            {
                if (pair.second == number)
                {
                    return pair.first;
                }
            }
            return std::nullopt;
        }
    };

    template <size_t tkNumIds>
    struct LLDriverEnumValueSequentialNumberMapping
    {
        uint32_t                       start_number;
        std::array<uint32_t, tkNumIds> ll_id_table;

        [[nodiscard]] constexpr VALLE_FORCE_INLINE bool contains_number(const uint32_t number) const
        {
            return number >= start_number && number < start_number + ll_id_table.size();
        }

        [[nodiscard]] constexpr VALLE_FORCE_INLINE std::optional<uint32_t> ll_id_to_number(const uint32_t ll_id) const
        {
            for (size_t i = 0; i < ll_id_table.size(); ++i)
            {
                if (ll_id_table[i] == ll_id)
                {
                    return start_number + i;
                }
            }
            return std::nullopt;
        }

        [[nodiscard]] constexpr VALLE_FORCE_INLINE std::optional<uint32_t> number_to_ll_id(const uint32_t number) const
        {
            if (number < start_number || number >= start_number + ll_id_table.size())
            {
                return std::nullopt;
            }
            return ll_id_table[number - start_number];
        }
    };

    template <typename T>
    concept CLLDriverNumberMapping = requires(const T t, const uint32_t ll_id, const uint32_t number) {
        { t.contains_number(number) } -> std::convertible_to<bool>;
        { t.ll_id_to_number(ll_id) } -> std::convertible_to<std::optional<uint32_t>>;
        { t.number_to_ll_id(number) } -> std::convertible_to<std::optional<uint32_t>>;
    };

    template <typename TDerived, auto tkMapping>
        requires(CLLDriverNumberMapping<decltype(tkMapping)>)
    class NumberedLLDriverEnumValue
    {
    private:
        uint32_t m_id;

    protected:
        explicit constexpr VALLE_FORCE_INLINE NumberedLLDriverEnumValue(const uint32_t ll_id) : m_id(ll_id)
        {
        }

    public:
        [[nodiscard]] static constexpr VALLE_FORCE_INLINE TDerived from_ll_id(const uint32_t ll_id)
        {
            return make_derived(ll_id);
        }

        template <uint32_t tkLLId>
        [[nodiscard]] static constexpr VALLE_FORCE_INLINE TDerived from_ll_id()
        {
            return make_derived(tkLLId);
        }

        [[nodiscard]] constexpr VALLE_FORCE_INLINE uint32_t to_ll_id() const
        {
            return m_id;
        }

        [[nodiscard]] static constexpr VALLE_FORCE_INLINE std::optional<TDerived> from_number(const uint32_t number)
        {
            constexpr auto ll_id_opt = tkMapping.number_to_ll_id(number);
            if (!ll_id_opt.has_value())
            {
                return std::nullopt;
            }
            return make_derived(ll_id_opt.value());
        }

        template <uint32_t tkNumber>
            requires(tkMapping.contains_number(tkNumber))
        [[nodiscard]] static constexpr VALLE_FORCE_INLINE TDerived from_number()
        {
            constexpr auto ll_id_opt = tkMapping.number_to_ll_id(tkNumber);
            if constexpr (!tkMapping.contains_number(tkNumber))
            {
                static_assert(kAlwaysFalseV<tkNumber>, "Invalid number for this mapping");
            }

            return make_derived(ll_id_opt.value());
        }

        [[nodiscard]] constexpr VALLE_FORCE_INLINE std::optional<uint32_t> to_number() const
        {
            return tkMapping.ll_id_to_number(to_ll_id());
        }

        [[nodiscard]] constexpr VALLE_FORCE_INLINE bool operator==(const NumberedLLDriverEnumValue&) const = default;

    private:
        [[nodiscard]] static constexpr VALLE_FORCE_INLINE TDerived make_derived(const uint32_t ll_id)
        {
            struct Raw final : NumberedLLDriverEnumValue
            {
                explicit constexpr VALLE_FORCE_INLINE Raw(const uint32_t v) : NumberedLLDriverEnumValue(v)
                {
                }
            };

            return static_cast<TDerived>(Raw{ll_id});
        }
    };

}  // namespace valle::platform