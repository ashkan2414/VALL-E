#pragma once

// ==========================================
// TYPE LIST
// ==========================================

// Basic Type TList
template <typename... Ts>
struct TypeList
{
};

// --- Set Operations (Add, TypeListContains) ---
template <typename T, typename TList>
struct TypeListContains;

template <typename T, typename... Ts>
struct TypeListContains<T, TypeList<Ts...>> : std::disjunction<std::is_same<T, Ts>...>
{
};

// Append T to TList
template <typename T, typename TList>
struct TypeListAdd;

template <typename T, typename... Ts>
struct TypeListAdd<T, TypeList<Ts...>>
{
    using type = TypeList<Ts..., T>;
};

// Append T to TList if not present
template <typename T, typename TList>
struct TypeListAddUnique;
template <typename T, typename... Ts>
struct TypeListAddUnique<T, TypeList<Ts...>>
{
    using type = std::conditional_t<TypeListContains<T, TypeList<Ts...>>::value, TypeList<Ts...>, TypeList<Ts..., T>>;
};

template <typename... Lists>
struct TypeListConcat;

// Base: Empty
template <>
struct TypeListConcat<>
{
    using type = TypeList<>;
};

// Base: Single
template <typename TList>
struct TypeListConcat<TList>
{
    using type = TList;
};

// Merge Two
template <typename... As, typename... Bs>
struct TypeListConcat<TypeList<As...>, TypeList<Bs...>>
{
    using type = TypeList<As..., Bs...>;
};

// Merge Three (Optimization)
template <typename... As, typename... Bs, typename... Cs>
struct TypeListConcat<TypeList<As...>, TypeList<Bs...>, TypeList<Cs...>>
{
    using type = TypeList<As..., Bs..., Cs...>;
};

// Recursive Split: Process head, merge with result of tail
// This reduces recursion depth effectively for large packs.
template <typename L1, typename L2, typename L3, typename... Rest>
struct TypeListConcat<L1, L2, L3, Rest...>
{
    // Merge 3 at a time to reduce depth quickly
    using Left  = typename TypeListConcat<L1, L2, L3>::type;
    using Right = typename TypeListConcat<Rest...>::type;
    using type  = typename TypeListConcat<Left, Right>::type;
};

// Merge two lists
template <typename Current, typename New>
struct TypeListMergeUnique;

template <typename... Cs, typename... Ns>
struct TypeListMergeUnique<TypeList<Cs...>, TypeList<Ns...>>
{
    template <typename Acc, typename TList>
    struct Helper;

    template <typename Acc>
    struct Helper<Acc, TypeList<>>
    {
        using type = Acc;
    };

    template <typename Acc, typename H, typename... T>
    struct Helper<Acc, TypeList<H, T...>>
    {
        using NextAcc =
            std::conditional_t<TypeListContains<H, Acc>::value, Acc, typename TypeListAddUnique<H, Acc>::type>;
        using type = typename Helper<NextAcc, TypeList<T...>>::type;
    };

    using type = typename Helper<TypeList<Cs...>, TypeList<Ns...>>::type;
};

template <typename... Lists>
struct TypeListMergeUniqueMulti;

// Base Case: 0 Lists
template <>
struct TypeListMergeUniqueMulti<>
{
    using type = TypeList<>;
};

// Base Case: 1 TList
template <typename TList>
struct TypeListMergeUniqueMulti<TList>
{
    using type = TList;
};

// Recursive Step: Merge First+Second, then Merge Result+Rest
template <typename List1, typename List2, typename... Rest>
struct TypeListMergeUniqueMulti<List1, List2, Rest...>
{
    using Combined = typename TypeListMergeUnique<List1, List2>::type;
    using type     = typename TypeListMergeUniqueMulti<Combined, Rest...>::type;
};

// Remove T from TList
template <typename Target, typename TList>
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

// Deduplicate TList (Low Cardinality Optimization)
template <typename InList, typename OutList = TypeList<>>
struct TypeListUniqueLowCard;

// Base: Done
template <typename Out>
struct TypeListUniqueLowCard<TypeList<>, Out>
{
    using type = Out;
};

// Recursive Step
template <typename Head, typename... Tail, typename Out>
struct TypeListUniqueLowCard<TypeList<Head, Tail...>, Out>
{
    // Check if seen (Fast lookup in small Out list)
    static constexpr bool kSeen = TypeListContains<Head, Out>::value;

    // Add if new
    using NextOut = std::conditional_t<kSeen, Out, typename TypeListAdd<Head, Out>::type>;

    using type = typename TypeListUniqueLowCard<TypeList<Tail...>, NextOut>::type;
};

template <typename TList>
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
struct TupleIndex;

template <typename T, typename... Ts>
struct TupleIndex<T, std::tuple<Ts...>>
{
    // Simple linear scan using inheritance for speed
    template <typename U, typename... Us>
    static constexpr size_t find()
    {
        if constexpr (std::is_same_v<T, typename U::DeviceT>)
            return 0;
        else
            return 1 + TupleIndex<T, std::tuple<Us...>>::template find<Us...>();
    }
    static constexpr size_t value = find<Ts...>();
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
