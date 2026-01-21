#pragma once

#include "Valle/Device/device_core.hpp"
#include "Valle/Utils/template_utils.hpp"

namespace detail
{

    // ==========================================
    // GRAPH DISCOVERY & SORTING
    // ==========================================

    // --- Step 1: Transitive Closure (Discover all deps) ---
    // Keep expanding the list until size stops growing
    // Forward Declaration
    template <typename CurrentList>
    struct ExpandDeps;

    // Helper 1: Lazy Recurse (Only instantiated if selected)
    template <typename List>
    struct ExpandDepsRecurse
    {
        using type = typename ExpandDeps<List>::type;
    };

    // Helper 2: Lazy Stop (Just returns the list)
    template <typename List>
    struct ExpandDepsStop
    {
        using type = List;
    };

    template <typename... Ts>
    struct ExpandDeps<TypeList<Ts...>>
    {
        // Alias Current List
        using Current = TypeList<Ts...>;

        // Calculate Next List (Merge dependencies)
        using Next = typename TypeListMergeUniqueMulti<Current, typename GetInjectDevices<Ts>::type...>::type;

        // Lazy Selection
        // We select the *Struct* (Recurse vs Stop), but we do NOT access ::type yet.
        using SelectedBranch = std::conditional_t<std::is_same_v<Current, Next>,  // If List didn't grow...
                                                  ExpandDepsStop<Current>,        // Select Stop
                                                  ExpandDepsRecurse<Next>         // Select Recurse
                                                  >;

        // Evaluate Result
        // Now it's safe to access ::type, because SelectedBranch is fully defined.
        using type = typename SelectedBranch::type;
    };

    // --- Step 2: Check if Ready (All deps in Sorted) ---
    template <typename T, typename SortedList>
    struct IsReady
    {
        using Deps = typename GetInjectDevices<T>::type;
        // T is ready if all types in Deps are found in SortedList
        template <typename... Ds>
        static constexpr bool check(TypeList<Ds...>)
        {
            return (TypeListContains<Ds, SortedList>::value && ...);
        }
        static constexpr bool value = check(Deps{});
    };

    // --- Step 3: Topological Sort ---
    // Move ready items from Pending to Sorted
    template <typename Pending, typename Sorted>
    struct TopoSort;

    // Base: Pending empty -> Done
    template <typename Sorted>
    struct TopoSort<TypeList<>, Sorted>
    {
        using type = Sorted;
    };

    // Recursive Step
    template <typename... Ps, typename Sorted>
    struct TopoSort<TypeList<Ps...>, Sorted>
    {
        // Split Pending into (Ready) and (NotReady)
        // We use a simplified partitioning: Find FIRST ready item, move it, recurse.
        // Efficient enough for typical hardware lists.

        template <typename List>
        struct FindReady
        {
            using type = void;
        };

        template <typename Head, typename... Tail>
        struct FindReady<TypeList<Head, Tail...>>
        {
            using type =
                std::conditional_t<IsReady<Head, Sorted>::value, Head, typename FindReady<TypeList<Tail...>>::type>;
        };

        using ReadyType = typename FindReady<TypeList<Ps...>>::type;

        static_assert(!std::is_void_v<ReadyType>, "Cyclic Dependency or Missing Dependency detected!");

        using NextPending = typename TypeListRemove<ReadyType, TypeList<Ps...>>::type;
        using NextSorted  = typename TypeListAddUnique<ReadyType, Sorted>::type;

        using type = typename TopoSort<NextPending, NextSorted>::type;
    };

    // Logic: Should we keep 'Res' in the registry given that 'Requests' were just consumed?
    template <typename TResDeviceRef, typename TRequestTuple>
    constexpr bool should_keep()
    {
        // Extract raw hardware type T from HwRef<T>
        using DeviceT = typename TResDeviceRef::DeviceT;

        bool is_requested = TupleContains<DeviceT, TRequestTuple>::value;
        bool is_shared    = CSharedDevice<DeviceT>;

        // Drop ONLY if Requested AND Unique
        if (is_requested && !is_shared) return false;
        return true;
    }

    // ============================================
    // REFERENCE POOL
    // ============================================
    // Helper: Convert tuple<Raw...> -> tuple<Ref...>
    template <typename Tuple>
    struct MapTupleToRefs;

    template <typename... Ts>
    struct MapTupleToRefs<std::tuple<Ts...>>
    {
        using type = std::tuple<DeviceRef<Ts>...>;
    };

    template <typename T>
    struct CheckDevice
    {
        static constexpr bool value = CDevice<T>;
    };

    template <typename T>
    concept CDeviceTuple = CTupleAllElementsSatisfy<T, CheckDevice>;

    template <typename T>
    struct CheckDeviceRef
    {
        static constexpr bool value = CDeviceRef<T>;
    };

    template <typename T>
    concept CDeviceRefTuple = CTupleAllElementsSatisfy<T, CheckDeviceRef>;

    template <CDeviceRef TTargetRef, CDeviceRefTuple TRefTuple>
    struct AtomicClaimer
    {
        static auto run(TRefTuple&& registry)
        {
            // Check Policy
            using DeviceT = typename TTargetRef::DeviceT;

            // Find Item in Registry
            // (We know it exists because we check requirements before calling this)
            constexpr size_t Idx = TupleIndexOf<TTargetRef, std::remove_cvref_t<TRefTuple>>::value;

            if constexpr (CSharedDevice<DeviceT>)
            {
                // CASE A: SHARED
                // Action: Copy ref out. Keep registry exactly as is.

                TTargetRef ref = std::get<Idx>(registry);  // Copy construction

                return std::make_pair(ref,
                                      std::forward<TRefTuple>(registry)  // Return registry untouched (moved whole)
                );
            }
            else
            {
                // CASE B: UNIQUE
                // Action: Move ref out. Rebuild registry WITHOUT this type.

                // Extract (Move)
                // Note: We use get<Idx>(move(registry)) which casts the element to r-value
                TTargetRef ref = std::move(std::get<Idx>(registry));

                // 2. Rebuild Registry (Remove Type)
                // We pass the *moved-from* registry.
                // This is safe because 'remove_item_from_tuple' iterates all elements.
                // The element at 'Idx' was just moved, but 'remove_item' skips index 'Idx' (by type),
                // so we never touch the zombie element. All valid elements are moved to new registry.
                auto next_registry = remove_item_from_tuple<TTargetRef>(std::move(registry));

                return std::make_pair(std::move(ref), std::move(next_registry));
            }
        }
    };

    // Signature: run(Registry) -> pair<ExtractedTuple, FinalRegistry>
    template <typename TNeedsTuple, typename TRefTuple>
    struct RecursiveClaimer;

    // Base Case: No Needs
    template <typename TRefTuple>
    struct RecursiveClaimer<std::tuple<>, TRefTuple>
    {
        static auto run(TRefTuple&& registry)
        {
            // Return empty extracted list, pass through registry
            return std::make_pair(std::tuple<>{}, std::forward<TRefTuple>(registry));
        }
    };

    // Recursive Step
    template <typename HeadNeed, typename... TailNeeds, typename TRefTuple>
    struct RecursiveClaimer<std::tuple<HeadNeed, TailNeeds...>, TRefTuple>
    {
        static auto run(TRefTuple&& registry)
        {
            // Process Head, Returns { Ref, NewRegistryType }
            auto [head_ref, new_registry] = AtomicClaimer<HeadNeed, TRefTuple>::run(std::move(registry));

            using NextRefTupleT = decltype(new_registry);

            // Recurse for Tail (using the New Registry)
            auto [remaining_extracted, final_registry] =
                RecursiveClaimer<std::tuple<TailNeeds...>, NextRefTupleT>::run(std::move(new_registry));

            // Combine Results, Extracted = (Head, Tail...)
            auto full_extracted = std::tuple_cat(std::make_tuple(std::move(head_ref)), std::move(remaining_extracted));

            // Return { (Head, Tail...), FinalRegistry }
            return std::make_pair(std::move(full_extracted), std::move(final_registry));
        }
    };

}  // namespace detail

template <CDeviceRef... CurrentRefs>
struct DeviceRefRegistry
{
    using DeviceRefRegistryTag = void;

    std::tuple<CurrentRefs...> refs;

    constexpr explicit DeviceRefRegistry(std::tuple<CurrentRefs...>&& r) : refs(std::move(r))
    {
    }

    /**
         * @brief Adds a device reference to the registry.
         *
         * @tparam T Device type to add.
         * @param instance Device instance to reference.
         * @return New DeviceRefRegistry with added device reference.
         */
    template <CDevice T>
    constexpr auto add(T& instance) &&
    {
        // Create new tuple: (Old..., DeviceRef<T>)
        auto new_tuple = std::tuple_cat(std::move(refs), std::make_tuple(DeviceRef<T>(instance)));

        // Return new TRegistry Type
        return DeviceRefRegistry<CurrentRefs..., DeviceRef<T>>(std::move(new_tuple));
    }

    /**
         * @brief Deleted l-value overload for add to prevent adding devices from l-value DeviceRefRegistry.
         */
    template <typename T>
    constexpr auto add(T& instance) & = delete;

    /**
         * @brief Claims dependencies for 'Consumer' from the registry.
         * @note This consumes the DeviceRefRegistry (it is an r-value method). Shared resources copy ref, unique resources are consumed.
         *
         * @tparam Consumer Device type requesting dependencies.
         * @return pair<Tuple<Deps...>, DeviceRefRegistry<Remaining...>>
         */
    template <CHasInjectDevices Consumer>
    constexpr auto claim() &&
    {
        // Identify Requirements
        using RawNeeds = typename TypeListToTuple<typename GetInjectDevices<Consumer>::type>::type;
        using RefNeeds = typename detail::MapTupleToRefs<RawNeeds>::type;

        // Compile-Time Existence Check
        // (Optional but good for errors: Check if RefNeeds are in 'refs')
        // static_assert(...)

        // Run Recursive Logic
        // Input: Current Refs. Output: { Tuple<Deps...>, Tuple<Remaining...> }
        auto result = detail::RecursiveClaimer<RefNeeds, decltype(refs)>::run(std::move(refs));

        // Return
        return std::pair{
            std::move(result.first),                   // The Dependencies
            make_from_tuple(std::move(result.second))  // The New Registry
        };
    }

    /**
         * @brief Claims dependencies for 'T' from the registry when T has no dependencies.
         *
         * @tparam T Device type requesting dependencies.
         */
    template <typename T>
        requires(!CHasInjectDevices<T>)
    constexpr auto claim() &&
    {
        // No dependencies -> Return empty tuple and same registry
        return std::make_pair(std::tuple<>{}, std::move(*this));
    }

    /**
         * @brief Deleted l-value overload for claim to prevent claiming from l-value DeviceRefRegistry.
         */
    template <CHasInjectDevices Consumer>
    constexpr auto claim() & = delete;

    /**
         * @brief Deleted l-value overload for claim to prevent claiming from l-value DeviceRefRegistry.
         */
    template <typename T>
        requires(!CHasInjectDevices<T>)
    constexpr auto claim() & = delete;

    template <typename Visitor>
    void foreach_shared(Visitor&& visitor)
    {
        std::apply(
            [&](auto&&... args)
            {
                // Helper lambda to check policy and invoke visitor
                auto try_visit = [&](auto&& ref)
                {
                    using RefT = std::remove_cvref_t<decltype(ref)>;

                    // Sanity check: Is it a valid ref?
                    if constexpr (CDeviceRef<RefT>)
                    {
                        using DeviceT = typename RefT::DeviceT;

                        // Only visit SHARED devices
                        if constexpr (CSharedDevice<DeviceT>)
                        {
                            // Invoke: Pass the DEVICE REFERENCE (T&) not the wrapper
                            // Assumes SharedDeviceRef has .get() returning T&
                            visitor(ref.get());
                        }
                    }
                };

                // Fold expression: Run try_visit on every element in the tuple
                (try_visit(args), ...);
            },
            refs);
    }

private:
    /**
         * @brief Helper to create a DeviceRefRegistry from a tuple of references.
         *
         * @tparam Ts Reference types.
         * @param t Tuple of references.
         * @return constexpr auto DeviceRefRegistry with the given references.
         */
    template <CDeviceRef... Ts>
    constexpr static auto make_from_tuple(std::tuple<Ts...>&& t)
    {
        return DeviceRefRegistry<Ts...>(std::move(t));
    }
};

// Helper deduction guide for empty registry
DeviceRefRegistry() -> DeviceRefRegistry<>;

template <typename T>
concept CDeviceRefRegistry = requires { typename T::DeviceRefRegistryTag; };

namespace detail
{
    // ==========================================
    // STORAGE & CONSTRUCTION SYSTEM
    // ==========================================

    template <CDevice... Ts>
    struct DeviceStorageImpl;

    // Base Case
    template <>
    struct DeviceStorageImpl<>
    {
        template <CDeviceRefRegistry TRegistry>
        explicit DeviceStorageImpl(TRegistry&& registry)
        {
        }  // End of chain

        auto export_registry(auto registry)
        {
            return registry;
        }
    };

    // Recursive Case
    template <CDevice Head, CDevice... Tail>
    struct DeviceStorageImpl<Head, Tail...>
    {
        Head                       item;
        DeviceStorageImpl<Tail...> next;

        // --- Constructor ---
        // We take a DeviceRefRegistry, claim deps for 'Head', construct 'Head',
        // add 'Head' to registry, pass to 'Next'.
        template <CDeviceRefRegistry TRegistry>
        explicit DeviceStorageImpl(TRegistry&& registry) : DeviceStorageImpl(std::move(registry).template claim<Head>())
        {
        }

        explicit DeviceStorageImpl() : DeviceStorageImpl(DeviceRefRegistry<>(std::tuple<>{}))
        {
        }

        template <CDeviceRefRegistry TRegistry>
        auto export_registry(TRegistry&& registry)
        {
            auto [_, filtered_registry] = std::move(registry).template claim<Head>();
            auto next_registry          = std::move(filtered_registry).add(item);
            return next.export_registry(std::move(next_registry));
        }

        auto export_registry()
        {
            return export_registry(DeviceRefRegistry<>(std::tuple<>{}));
        }

    private:
        template <typename TDeps, CDeviceRefRegistry TFilteredRegistry>
        explicit DeviceStorageImpl(std::pair<TDeps, TFilteredRegistry>&& args)
            : item(std::make_from_tuple<Head>(std::move(args.first)))  // 1. Construct Item in-place
            , next(std::move(args.second).add(item))                   // 2. Add reference of MEMBER item to registry
        {
        }
    };

    template <typename T>
    struct MakeDeviceStorage;

    template <typename... Ts>
    struct MakeDeviceStorage<TypeList<Ts...>>
    {
        using type = DeviceStorageImpl<Ts...>;
    };
}  // namespace detail

template <CDevice... TDevices>
class DeviceStorage
{
private:
    using Devices     = DeviceList<TDevices...>;
    using DeviceGraph = typename Devices::Devices;
    using FullList    = typename detail::ExpandDeps<DeviceGraph>::type;
    using SortedList  = typename detail::TopoSort<FullList, TypeList<>>::type;
    using Storage     = typename detail::MakeDeviceStorage<SortedList>::type;

    Storage m_storage;

public:
    DeviceStorage() : m_storage()
    {
    }

    auto create_registry()
    {
        return m_storage.export_registry();
    }
};

template <CDeviceRefRegistry TDeviceRefRegistry, typename TDriverTuple = std::tuple<>>
class DriverBuilder
{
private:
    TDeviceRefRegistry m_registry;
    TDriverTuple       m_drivers;

public:
    DriverBuilder(TDeviceRefRegistry&& p, TDriverTuple&& d) : m_registry(std::move(p)), m_drivers(std::move(d))
    {
    }

    DriverBuilder(TDeviceRefRegistry&& p) : m_registry(std::move(p)), m_drivers()
    {
    }

    /**
     * @brief  Install a device that has dependencies.
     *
     * @tparam TDriver Driver type to install.
     * @tparam Args Constructor argument types.
     * @param args Constructor arguments.
     * @return auto New DriverBuilder with the installed device.
     */
    template <CHasInjectDevices TDriver, typename... Args>
    [[nodiscard]] auto install(Args&&... args) &&
    {
        // Ask the Registry for resources
        // This line handles ALL validation, existence checks, and unique filtering
        auto [deps, next_registry] = std::move(m_registry).template claim<TDriver>();

        // Create Driver
        auto new_driver =
            std::make_from_tuple<TDriver>(std::tuple_cat(std::move(deps), std::forward_as_tuple(args...)));

        // Return next state
        auto next_drivers = std::tuple_cat(std::move(m_drivers), std::make_tuple(std::move(new_driver)));

        return DriverBuilder<decltype(next_registry), decltype(next_drivers)>(std::move(next_registry),
                                                                              std::move(next_drivers));
    }

    /**
     * @brief Install a device that has no dependencies.
     *
     * @tparam TDriver Device type to install.
     * @tparam Args Constructor argument types.
     * @param args Constructor arguments.
     */
    template <typename TDriver, typename... Args>
        requires(!CHasInjectDevices<TDriver>)
    [[nodiscard]] auto install(Args&&... args) &&
    {
        // No dependencies -> Just create the driver
        auto new_driver = TDriver(std::forward<Args>(args)...);

        // Return next state
        auto next_drivers = std::tuple_cat(std::move(m_drivers), std::make_tuple(std::move(new_driver)));

        return DriverBuilder<TDeviceRefRegistry, decltype(next_drivers)>(std::move(m_registry),
                                                                         std::move(next_drivers));
    }

    /**
     * @brief Deleted l-value overload for install to prevent installing from l-value DriverBuilder.
     *
     * @tparam D Device type to install.
     * @tparam A Constructor argument types.
     * @param args Constructor arguments.
     */
    template <typename D, typename... A>
    void install(A&&...) & = delete;

    /**
     * @brief Yield the installed drivers.
     *
     * @return auto
     */
    auto yield() &&
    {
        return std::tuple_cat(std::make_tuple(std::move(m_registry)), std::move(m_drivers));
    }
    void yield() & = delete;
};

template <typename TDeviceStorage>
inline constexpr auto boot_driver_builder(TDeviceStorage& storage)
{
    auto registry = storage.create_registry();
    return DriverBuilder<decltype(registry)>(std::move(registry));
}