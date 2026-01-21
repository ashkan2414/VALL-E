#pragma once

// ==========================================
// TYPE LIST
// ==========================================

// Basic Type List
template <typename... Ts>
struct TypeList
{
};

// --- Set Operations (Add, TypeListContains) ---

template <typename T, typename List>
struct TypeListContains;
template <typename T, typename... Ts>
struct TypeListContains<T, TypeList<Ts...>> : std::bool_constant<(std::is_same_v<T, Ts> || ...)>
{
};

// Append T to List if not present
template <typename T, typename List>
struct TypeListAddUnique;
template <typename T, typename... Ts>
struct TypeListAddUnique<T, TypeList<Ts...>>
{
    using type = std::conditional_t<TypeListContains<T, TypeList<Ts...>>::value, TypeList<Ts...>, TypeList<Ts..., T>>;
};

// Merge two lists
template <typename DestList, typename SourceList>
struct TypeListMergeUnique;

// Base Case: Source is empty -> Return Dest
template <typename DestList, template <typename...> class List>
struct TypeListMergeUnique<DestList, List<>>
{
    using type = DestList;
};

// Recursive Step: Take Head of Source, AddUnique to Dest, Recurse with Tail
template <typename DestList, template <typename...> class List, typename Head, typename... Tail>
struct TypeListMergeUnique<DestList, List<Head, Tail...>>
{
    // Add Head to Dest (if unique)
    using NextDest = typename TypeListAddUnique<Head, DestList>::type;

    // Recurse
    using type = typename TypeListMergeUnique<NextDest, List<Tail...>>::type;
};

template <typename... Lists>
struct TypeListMergeUniqueMulti;

// Base Case: 0 Lists
template <>
struct TypeListMergeUniqueMulti<>
{
    using type = TypeList<>;
};

// Base Case: 1 List
template <typename List>
struct TypeListMergeUniqueMulti<List>
{
    using type = List;
};

// Recursive Step: Merge First+Second, then Merge Result+Rest
template <typename List1, typename List2, typename... Rest>
struct TypeListMergeUniqueMulti<List1, List2, Rest...>
{
    using Combined = typename TypeListMergeUnique<List1, List2>::type;
    using type     = typename TypeListMergeUniqueMulti<Combined, Rest...>::type;
};

// Remove T from List
template <typename Target, typename List>
struct TypeListRemove;

template <typename Target, typename... Ts>
struct TypeListRemove<Target, TypeList<Ts...>>
{
    // Map: If U == Target, return empty list. Else return list of U.
    template <typename U>
    using Wrap = std::conditional_t<std::is_same_v<U, Target>, TypeList<>, TypeList<U>>;

    // Reduce: Merge all resulting lists
    using type = typename TypeListMergeUniqueMulti<Wrap<Ts>...>::type;
};

template <typename List>
struct TypeListToTuple;

template <typename... Ts>
struct TypeListToTuple<TypeList<Ts...>>
{
    using type = std::tuple<Ts...>;
};

// ==========================================
// Tuple Utilities
// ==========================================

namespace detail
{
    // Trait: Check if T is a specialization of std::tuple
    template <typename T>
    struct is_tuple_impl : std::false_type
    {
    };

    template <typename... Ts>
    struct is_tuple_impl<std::tuple<Ts...>> : std::true_type
    {
    };
}  // namespace detail

template <typename T>
concept CIsTuple = detail::is_tuple_impl<std::remove_cvref_t<T>>::value;

// Check if Type T exists in TTuple<Ts...>
template <typename T, typename TTuple>
struct TupleContains;

template <typename T, typename... Ts>
struct TupleContains<T, std::tuple<Ts...>>
{
    static constexpr bool value = (std::same_as<T, Ts> || ...);
};

namespace detail
{
    // Primary template: Returns false if T is not a tuple
    template <typename T, template <typename> class Trait>
    struct TupleCheckImpl : std::false_type
    {
    };

    // Specialization: Unpacks the tuple and checks the Trait against all types
    template <typename... Ts, template <typename> class Trait>
    struct TupleCheckImpl<std::tuple<Ts...>, Trait> : std::bool_constant<(Trait<Ts>::value && ...)>
    {
    };
}  // namespace detail

// Check if every element in 'TTuple' satisfies 'TypeTrait'
template <typename T, template <typename> class TypeTrait>
concept CTupleAllElementsSatisfy = CIsTuple<T> && detail::TupleCheckImpl<T, TypeTrait>::value;

// --- Helper: Find Index of Type T in TTuple ---
template <typename T, typename Tuple>
struct TupleIndexOf;

template <typename T, typename... Ts>
struct TupleIndexOf<T, std::tuple<Ts...>>
{
private:
    template <size_t I, typename Cur, typename... Rest>
    struct Impl;

    // 1. Recursive Case (At least 2 elements remaining)
    template <size_t I, typename Cur, typename Next, typename... Rest>
    struct Impl<I, Cur, Next, Rest...>
    {
        static constexpr size_t value = std::is_same_v<T, Cur> ? I : Impl<I + 1, Next, Rest...>::value;
    };

    // 2. Base Case (Last element remaining)
    template <size_t I, typename Cur>
    struct Impl<I, Cur>
    {
        // If match, return I. If not match, return -1 (huge number) to trigger index-out-of-bounds error in std::get
        static constexpr size_t value = std::is_same_v<T, Cur> ? I : static_cast<size_t>(-1);
    };

public:
    static_assert(sizeof...(Ts) > 0, "TupleIndexOf called on empty tuple!");
    static constexpr size_t value = Impl<0, Ts...>::value;
};

// --- Helper: Remove Type T from TTuple (Return new TTuple) ---
// This moves all elements EXCEPT T into a new tuple.
template <typename TRemove, typename TTuple>
static constexpr inline auto remove_item_from_tuple(TTuple&& t)
{
    return std::apply(
        [&](auto&&... args)
        {
            return std::tuple_cat(
                [&](auto&& arg)
                {
                    using ArgT = std::remove_cvref_t<decltype(arg)>;
                    if constexpr (std::is_same_v<ArgT, TRemove>)
                    {
                        return std::tuple<>{};  // Drop it
                    }
                    else
                    {
                        return std::make_tuple(std::forward<decltype(arg)>(arg));  // Keep/Move it
                    }
                }(std::forward<decltype(args)>(args))...);
        },
        std::forward<TTuple>(t));
}

// ==========================================
// Overloaded Helper
// ==========================================

template <class... Ts>
struct Overloaded : Ts...
{
    using Ts::operator()...;
};
template <class... Ts>
Overloaded(Ts...) -> Overloaded<Ts...>;
