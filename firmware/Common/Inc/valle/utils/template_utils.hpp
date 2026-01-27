#pragma once

#include <tuple>
#include <type_traits>

namespace valle
{

    // =============================================================================
    // TYPE LIST
    // =============================================================================

    // Basic Type TList
    template <typename... Ts>
    struct TypeList
    {
    };

    // ----------------------------------------------------------------------------
    // TypeListContains: check if T is in TList
    // ----------------------------------------------------------------------------
    template <typename T, typename TList>
    struct TypeListContains;

    template <typename T, typename... Ts>
    struct TypeListContains<T, TypeList<Ts...>> : std::disjunction<std::is_same<T, Ts>...>
    {
    };

    // ----------------------------------------------------------------------------
    // TypeListAdd: add T to TList
    // ----------------------------------------------------------------------------
    template <typename T, typename TList>
    struct TypeListAdd;

    template <typename T, typename... Ts>
    struct TypeListAdd<T, TypeList<Ts...>>
    {
        using type = TypeList<Ts..., T>;
    };

    // ----------------------------------------------------------------------------
    // TypeListAddUnique: add T to TList if not present
    // ----------------------------------------------------------------------------
    template <typename T, typename TList>
    struct TypeListAddUnique;
    template <typename T, typename... Ts>
    struct TypeListAddUnique<T, TypeList<Ts...>>
    {
        using type =
            std::conditional_t<TypeListContains<T, TypeList<Ts...>>::value, TypeList<Ts...>, TypeList<Ts..., T>>;
    };

    // ----------------------------------------------------------------------------
    // TypeListConcat: concatenate multiple TypeLists
    // ----------------------------------------------------------------------------

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

    // ----------------------------------------------------------------------------
    // TypeListMergeUnique: merge two lists uniquely
    // ----------------------------------------------------------------------------

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

    // ----------------------------------------------------------------------------
    // TypeListMergeUniqueMulti: merge multiple lists uniquely
    // ----------------------------------------------------------------------------

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

    // ----------------------------------------------------------------------------
    // TypeListRemove: remove T from TList
    // ----------------------------------------------------------------------------

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

    // ----------------------------------------------------------------------------
    // TypeListUniqueLowCard: deduplicate TList (low cardinality optimization)
    // ----------------------------------------------------------------------------

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
        static constexpr bool skSeen = TypeListContains<Head, Out>::value;

        // Add if new
        using NextOut = std::conditional_t<skSeen, Out, typename TypeListAdd<Head, Out>::type>;

        using type = typename TypeListUniqueLowCard<TypeList<Tail...>, NextOut>::type;
    };

    // ----------------------------------------------------------------------------
    // TypeListSize: get size of TList
    // ----------------------------------------------------------------------------

    template <typename TList>
    struct TypeListSize;

    template <typename... Ts>
    struct TypeListSize<TypeList<Ts...>>
    {
        static constexpr std::size_t value = sizeof...(Ts);
    };

    template <>
    struct TypeListSize<TypeList<>>
    {
        static constexpr std::size_t value = 0;
    };

    // ----------------------------------------------------------------------------
    // Is Tuple
    // ----------------------------------------------------------------------------

    template <typename TList>
    struct TypeListToTuple;

    template <typename... Ts>
    struct TypeListToTuple<TypeList<Ts...>>
    {
        using type = std::tuple<Ts...>;
    };

    // ============================================================================
    // Tuple Utilities
    // ============================================================================

    // ----------------------------------------------------------------------------
    // IsTuple: check if T is a specialization of std::tuple
    // ----------------------------------------------------------------------------

    namespace detail
    {
        // Trait: Check if T is a specialization of std::tuple
        template <typename T>
        struct IsTupleImpl : std::false_type
        {
        };

        template <typename... Ts>
        struct IsTupleImpl<std::tuple<Ts...>> : std::true_type
        {
        };
    }  // namespace detail

    template <typename T>
    concept CIsTuple = detail::IsTupleImpl<std::remove_cvref_t<T>>::value;

    // ----------------------------------------------------------------------------
    // TupleContains: check if T exists in TTuple<Ts...>
    // ----------------------------------------------------------------------------

    template <typename T, typename TTuple>
    struct TupleContains;

    template <typename T, typename... Ts>
    struct TupleContains<T, std::tuple<Ts...>>
    {
        static constexpr bool value = (std::same_as<T, Ts> || ...);
    };

    // ----------------------------------------------------------------------------
    // TupleAllElementsSatisfy: check if all elements in TTuple satisfy TypeTrait
    // ----------------------------------------------------------------------------

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

    // ----------------------------------------------------------------------------
    // TupleIndex: get index of T in TTuple<Ts...>
    // ----------------------------------------------------------------------------
    template <typename T, typename Tuple>
        requires(TupleContains<T, Tuple>::value)
    struct TupleIndex;

    template <typename T, typename... Ts>
        requires(TupleContains<T, std::tuple<Ts...>>::value)
    struct TupleIndex<T, std::tuple<Ts...>>
    {
        // Simple linear scan using inheritance for speed
        template <typename U, typename... Us>
        static constexpr std::size_t find()
        {
            if constexpr (std::is_same_v<T, U>)
            {
                return 0;
            }
            else
            {
                return 1 + TupleIndex<T, std::tuple<Us...>>::template find<Us...>();
            }
        }
        static constexpr std::size_t value = find<Ts...>();
    };

    // ----------------------------------------------------------------------------
    // RemoveItemFromTuple: remove TRemove from TTuple
    // ----------------------------------------------------------------------------
    template <typename TRemove, typename TTuple>
    static constexpr auto remove_item_from_tuple(TTuple&& tuple)
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
            std::forward<TTuple>(tuple));
    }

    // ============================================================================
    // Type inspection helpers
    // ============================================================================

    // ----------------------------------------------------------------------------
    // IsInstantiationOf: check if T is an instantiation of Template<...>
    // ----------------------------------------------------------------------------

    // Template taking Types (e.g. std::vector<int>)
    template <template <typename...> class TTemplate, typename T>
    struct IsInstantiationOfTypes : std::false_type
    {
    };

    template <template <typename...> class TTemplate, typename... TArgs>
    struct IsInstantiationOfTypes<TTemplate, TTemplate<TArgs...>> : std::true_type
    {
    };

    template <typename T, template <typename...> class TTemplate>
    concept CIsInstantiationOfTypes = IsInstantiationOfTypes<TTemplate, T>::value;

    // Template taking Values (Non-Type Template Parameters) (e.g. ADCPinMap<0, 1>)
    template <template <auto...> class TTemplate, typename T>
    struct IsInstantiationOfValues : std::false_type
    {
    };

    template <template <auto...> class TTemplate, auto... VArgs>
    struct IsInstantiationOfValues<TTemplate, TTemplate<VArgs...>> : std::true_type
    {
    };

    template <typename T, template <auto...> class TTemplate>
    concept CIsInstantiationOfValues = IsInstantiationOfValues<TTemplate, T>::value;

    template <typename T>
    concept CVoid = std::is_same_v<T, void>;

    // ============================================================================
    // Overloaded Helper
    // ============================================================================

    template <class... Ts>
    struct Overloaded : Ts...
    {
        using Ts::operator()...;
    };
    template <class... Ts>
    Overloaded(Ts...) -> Overloaded<Ts...>;

    // ============================================================================
    // Debugging
    // ============================================================================

    /**
 * @brief Triggers a compiler error that prints the deduced type T.
 * Usage: InspectType<MyComplexType>{};
 */
    template <typename... TInspect>
    struct InspectType;  // No definition!

    template <std::size_t N>
    struct InspectValue
    {
        static_assert(N < 0, "Inspect Value: Triggered intentional compile-time error to inspect value.");
    };

}  // namespace valle