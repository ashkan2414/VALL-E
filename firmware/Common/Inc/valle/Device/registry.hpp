#pragma once

#include "Valle/Device/device_core.hpp"
#include "Valle/Utils/template_utils.hpp"

namespace valle
{

    namespace detail
    {

        // ==========================================
        // GRAPH DISCOVERY & SORTING
        // ==========================================

        template <typename T>
        using DependT = typename GetDependDevices<T>::type;

        template <typename TList>
        struct DirectDeps
        {
            // Flatten all direct dependencies of current items into one giant list
            //    (e.g., [ADC1, ADC1, ADC1, GPIOA, GPIOB, ADC1...])
            //    We use a wrapper to unpack the list into Concat.
            template <typename L>
            struct FlattenHelper;
            template <typename... Ts>
            struct FlattenHelper<TypeList<Ts...>>
            {
                using type = typename TypeListMergeUniqueMulti<DependT<Ts>...>::type;
            };

            using type = typename FlattenHelper<TList>::type;
        };

        // Helper to wrap the result to stop recursion
        template <typename T>
        struct Identity
        {
            using type = T;
        };

        template <typename TCurrentList>
        struct ExpandDepsImpl
        {
            using AllDirectDeps = typename DirectDeps<TCurrentList>::type;

            // Combine + Deduplicate
            using CombinedRaw = typename TypeListConcat<TCurrentList, AllDirectDeps>::type;
            using NextUnique  = typename TypeListUniqueLowCard<CombinedRaw, TypeList<>>::type;

            // Check for Steady State
            static constexpr bool skChanged = !std::is_same_v<TCurrentList, NextUnique>;

            // LAZY Selection
            // If Changed: Select ExpandDepsImpl<NextUnique> (Struct, not ::type yet!)
            // If Same:    Select Identity<TCurrentList>
            using SelectedStruct = std::conditional_t<skChanged, ExpandDepsImpl<NextUnique>, Identity<TCurrentList>>;

            // Access ::type safely
            using type = typename SelectedStruct::type;
        };

        template <>
        struct ExpandDepsImpl<TypeList<>>
        {
            using type = TypeList<>;
        };

        template <typename T>
        struct ExpandDeps
        {
            using type = typename ExpandDepsImpl<T>::type;
        };

        template <typename T, typename SortedList>
        struct IsReady
        {
            using Deps = typename GetDependDevices<T>::type;

            template <typename... Ds>
            static constexpr bool check(TypeList<Ds...> deps)
            {
                return (TypeListContains<Ds, SortedList>::value && ...);
            }
            static constexpr bool value = check(Deps{});
        };

        // Forward Declaration
        template <typename TPending, typename TSorted>
        struct TopoSort;

        // BASE CASE: Pending is empty -> Return Sorted
        template <typename TSorted>
        struct TopoSort<TypeList<>, TSorted>
        {
            using type = TSorted;
        };

        template <typename... Ps, typename TSorted>
        struct TopoSort<TypeList<Ps...>, TSorted>
        {
            using PendingT = TypeList<Ps...>;

            // Helper: Find first item in Pending that is ready
            template <typename List>
            struct FindReady;

            // Internal Search
            template <typename Head, typename... Tail>
            struct FindReady<TypeList<Head, Tail...>>
            {
                using type = std::
                    conditional_t<IsReady<Head, TSorted>::value, Head, typename FindReady<TypeList<Tail...>>::type>;
            };
            // Not Found Case (End of recursion)
            template <typename... Empty>
            struct FindReady<TypeList<Empty...>>
            {
                using type = void;
            };

            using ReadyT = typename FindReady<PendingT>::type;

            // Assertion: If Pending is not empty, we MUST find a ReadyT.
            // If ReadyT is void, it means we have a cycle or missing dependency.
            static_assert(!std::is_void_v<ReadyT>,
                          "Cyclic Dependency or Missing Dependency detected! Graph cannot be resolved.");

            // Move ReadyT from Pending to Sorted
            using NextPendingT = typename TypeListRemove<ReadyT, PendingT>::type;
            using NextSortedT  = typename TypeListAddUnique<ReadyT, TSorted>::type;

            // Recurse or Finish
            using type = typename TopoSort<NextPendingT, NextSortedT>::type;
        };

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

        template <typename TRegistryTuple, typename TNeedsList>
        struct BatchClaimer
        {
            static constexpr size_t skRegSize = std::tuple_size_v<TRegistryTuple>;

            // Identify INDICES of required devices
            template <typename... TNeeds>
            static constexpr auto get_indices(TypeList<TNeeds...> needs)
            {
                return std::array<size_t, sizeof...(TNeeds)>{TupleIndex<DeviceRef<TNeeds>, TRegistryTuple>::value...};
            }

            static constexpr auto skIndices = get_indices(TNeedsList{});

            // Logic: Should we KEEP the item at tkRegIndex in the new registry?
            // We keep it if:
            //   A. It is NOT in the requested list.
            //   B. It IS in the requested list, BUT it is a SharedDevice.
            template <size_t tkRegIndex>
            static constexpr bool should_keep()
            {
                // Check if tkRegIndex is in skIndices
                bool is_requested = false;
                for (auto idx : skIndices)
                {
                    if (idx == tkRegIndex)
                    {
                        is_requested = true;
                    }
                }

                if (!is_requested)
                {
                    return true;
                }

                // It is requested. Check if shared.
                using RefType = std::tuple_element_t<tkRegIndex, TRegistryTuple>;
                using DeviceT = typename RefType::DeviceT;

                return CSharedDevice<DeviceT>;
            }

            // Logic: Extracting the dependencies
            // Returns a tuple of references (copies or moves)
            template <typename... TNeeds>
            static auto extract_deps(TRegistryTuple& reg, TypeList<TNeeds...> needs)
            {
                return std::make_tuple(extract_one<TNeeds>(reg)...);
            }

            template <typename Need, typename Reg>
            static DeviceRef<Need> extract_one(Reg& reg)
            {
                constexpr size_t idx = TupleIndex<DeviceRef<Need>, Reg>::value;
                // Move if Unique, Copy if Shared
                if constexpr (CSharedDevice<Need>)  // NOLINT(bugprone-branch-clone)
                {
                    return std::get<idx>(reg);  // Copy Ref
                }
                else
                {
                    return std::move(std::get<idx>(reg));  // Move Ref
                }
            }

            template <size_t Index>
            static auto get_if_kept(TRegistryTuple& reg)
            {
                if constexpr (should_keep<Index>())  // NOLINT(bugprone-branch-clone)
                {
                    // Keep: Return a tuple containing the element
                    return std::make_tuple(std::move(std::get<Index>(reg)));
                }
                else
                {
                    // Drop: Return an empty tuple
                    return std::tuple<>{};
                }
            }

            // Logic: Building the new Registry
            // We iterate 0..N, check should_keep(), and build a new tuple.
            template <size_t... Is>
            static auto build_new_registry(TRegistryTuple&& reg, std::index_sequence<Is...> indices)
            {
                // Expand using the helper and concat results
                return std::tuple_cat(get_if_kept<Is>(reg)...);
            }

            static auto run(TRegistryTuple&& reg)
            {
                // Extract Dependencies
                auto deps = extract_deps(reg, TNeedsList{});

                // Build Remaining Registry
                // Note: We pass 'reg' as r-value.
                // Unique items extracted in step A are moved-from.
                // Unique items NOT filtered in B would be an error (but logical impossibility).
                // Items kept in B are moved to the new tuple.
                auto new_reg = build_new_registry(std::move(reg), std::make_index_sequence<skRegSize>{});

                return std::make_pair(std::move(deps), std::move(new_reg));
            }
        };

    }  // namespace detail

    template <CDeviceRef... TCurrentRefs>
    struct DeviceRefRegistry
    {
        using DeviceRefRegistryTag = void;
        using TRegistryTuple       = std::tuple<TCurrentRefs...>;

        template <CHasInjectDevices TConsumer>
        using DeviceRefRegistryAfterClaim =
            decltype(std::declval<DeviceRefRegistry<TCurrentRefs...>>().template claim<TConsumer>().second);

        TRegistryTuple refs;

        constexpr explicit DeviceRefRegistry(TRegistryTuple&& ref_tuple) : refs(std::move(ref_tuple))
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
            return DeviceRefRegistry<TCurrentRefs..., DeviceRef<T>>(std::move(new_tuple));
        }

        /**
     * @brief Deleted l-value overload for add to prevent adding devices from l-value DeviceRefRegistry.
     */
        template <typename T>
        constexpr auto add(T& instance) & = delete;

        /**
         * @brief Claims dependencies for 'TConsumer' from the registry.
         * @note This consumes the DeviceRefRegistry (it is an r-value method). Shared resources copy ref, unique resources are consumed.
         *
         * @tparam TConsumer Device type requesting dependencies.
         * @return pair<Tuple<Deps...>, DeviceRefRegistry<Remaining...>>
         */
        template <CHasInjectDevices TConsumer>
        constexpr auto claim() &&
        {
            using NeedsT = typename GetInjectDevices<TConsumer>::type;

            // Use the BatchClaimer to do everything in one pass
            auto result = detail::BatchClaimer<TRegistryTuple, NeedsT>::run(std::move(refs));

            return std::pair{std::move(result.first), make_from_tuple(std::move(result.second))};
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
            return std::make_pair(std::tuple<>{}, std::move(*this));
        }

        /**
         * @brief Deleted l-value overload for claim to prevent claiming from l-value DeviceRefRegistry.
         */
        template <CHasInjectDevices TConsumer>
        constexpr auto claim() & = delete;

        /**
         * @brief Deleted l-value overload for claim to prevent claiming from l-value DeviceRefRegistry.
         */
        template <typename T>
            requires(!CHasInjectDevices<T>)
        constexpr auto claim() & = delete;

        template <CSharedDevice TDevice>
        [[nodiscard]] TDevice& get()
        {
            constexpr size_t   idx = TupleIndex<DeviceRef<TDevice>, TRegistryTuple>::value;
            DeviceRef<TDevice> ref = std::get<idx>(refs);
            return ref.get();
        }

        template <typename Visitor>
        void foreach_shared(Visitor&& visitor)
        {
            std::apply(
                [&](auto&&... args)
                {
                    auto try_visit = [&](auto&& ref)
                    {
                        using RefT = std::remove_cvref_t<decltype(ref)>;
                        if constexpr (CDeviceRef<RefT> && CSharedDevice<typename RefT::DeviceT>)
                        {
                            visitor(ref.get());
                        }
                    };
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
        constexpr static auto make_from_tuple(std::tuple<Ts...>&& ref_tuple)
        {
            return DeviceRefRegistry<Ts...>(std::move(ref_tuple));
        }
    };

    // Helper deduction guide for empty registry
    DeviceRefRegistry() -> DeviceRefRegistry<>;

    template <typename T>
    concept CDeviceRefRegistry = requires { typename T::DeviceRefRegistryTag; };

    namespace detail
    {

        template <CDeviceRefRegistry TRegistry, CHasInjectDevices... TConsumers>
        struct DeviceRefRegistryAfterClaimsHelper;

        template <CDeviceRefRegistry TRegistry, CHasInjectDevices TFirst, CHasInjectDevices... TRest>
        struct DeviceRefRegistryAfterClaimsHelper<TRegistry, TFirst, TRest...>
        {
            using NextRegistry = typename TRegistry::template DeviceRefRegistryAfterClaim<TFirst>;
            using type         = typename DeviceRefRegistryAfterClaimsHelper<NextRegistry, TRest...>::type;
        };

        template <CDeviceRefRegistry TRegistry>
        struct DeviceRefRegistryAfterClaimsHelper<TRegistry>
        {
            using type = TRegistry;
        };

    }  // namespace detail

    template <CDeviceRefRegistry TRegistry, CHasInjectDevices... TConsumers>
    using DeviceRefRegistryAfterClaims =
        typename detail::DeviceRefRegistryAfterClaimsHelper<TRegistry, TConsumers...>::type;

    // ============================================================================
    // STORAGE & CONSTRUCTION SYSTEM
    // ============================================================================

    namespace detail
    {
        template <CDevice... Ts>
        struct DeviceStorageImpl;
        template <>
        struct DeviceStorageImpl<>
        {
            template <CDeviceRefRegistry TRegistry>
            explicit DeviceStorageImpl(TRegistry&&)
            {
            }
            auto export_registry(auto registry)
            {
                return registry;
            }
        };

        template <CDevice Head, CDevice... Tail>
        struct DeviceStorageImpl<Head, Tail...>
        {
            Head                       item;
            DeviceStorageImpl<Tail...> next;

            /**
         * @brief Constructor that builds the device storage by claiming devices from the registry.
         *
         * @tparam TRegistry Type of the device reference registry.
         * @param registry Device reference registry to claim dependencies from.
         */
            template <CDeviceRefRegistry TRegistry>
            explicit DeviceStorageImpl(TRegistry&& registry)
                : DeviceStorageImpl(std::move(registry).template claim<Head>())
            {
            }

            /**
         * @brief Default constructor that initializes the device storage with an empty registry.
         * @note This constructor is only valid if all devices can be constructed without dependencies.
         */
            explicit DeviceStorageImpl()  // NOLINT(cppcoreguidelines-pro-type-member-init)
                : DeviceStorageImpl(DeviceRefRegistry<>(std::tuple<>{}))
            {
            }

            /**
         * @brief Accessor to get a device by type.
         *
         * @tparam TDevice Type of the device to retrieve.
         * @return Reference to the requested device.
         */
            template <typename TDevice>
            [[nodiscard]] TDevice& get()
            {
                if constexpr (std::is_same_v<TDevice, Head>)
                {
                    return item;
                }
                else
                {
                    return next.template get<TDevice>();
                }
            }

            /**
         * @brief Exports the device reference registry after adding the current device.
         *
         * @tparam TRegistry Type of the device reference registry.
         * @param registry Device reference registry to add the current device to.
         * @return New DeviceRefRegistry with the current device added.
         */
            template <CDeviceRefRegistry TRegistry>
            auto export_registry(TRegistry&& registry)
            {
                auto [_, filtered] = std::move(registry).template claim<Head>();
                return next.export_registry(std::move(filtered).add(item));
            }

            /**
         * @brief Exports the device reference registry starting from an empty registry.
         *
         * @return auto New DeviceRefRegistry with all devices added.
         */
            auto export_registry()
            {
                return export_registry(DeviceRefRegistry<>(std::tuple<>{}));
            }

        private:
            /**
         * @brief Construct a new Device Storage Impl object
         *
         * @tparam TDeps Type of the dependencies tuple.
         * @tparam TFiltered Type of the filtered device reference registry.
         * @param args Pair of dependencies tuple and filtered device reference registry.
         */
            template <typename TDeps, CDeviceRefRegistry TFiltered>
            // NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init)
            explicit DeviceStorageImpl(std::pair<TDeps, TFiltered>&& args)
                : item(std::make_from_tuple<Head>(std::move(args.first))), next(std::move(args.second).add(item))
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
        using Devices    = TypeList<TDevices...>;
        using FullList   = typename detail::ExpandDeps<Devices>::type;
        using SortedList = typename detail::TopoSort<FullList, TypeList<>>::type;
        using Storage    = typename detail::MakeDeviceStorage<SortedList>::type;

        friend system::internal::GlobalDeviceStorageAccessor;  // Allow access to get()

        Storage m_storage;

    public:
        using RegistryT = decltype(std::declval<Storage>().export_registry());

        DeviceStorage() : m_storage()
        {
        }

        [[nodiscard]] auto create_registry()
        {
            return m_storage.export_registry();
        }

    private:
        template <CDevice TDevice>
        [[nodiscard]] TDevice& get()
        {
            return m_storage.template get<TDevice>();
        }
    };

    namespace detail
    {
        template <typename TDeviceList>
        struct DeviceStorageBuilderHelper;

        template <typename... TDevices>
        struct DeviceStorageBuilderHelper<TypeList<TDevices...>>
        {
            using type = DeviceStorage<TDevices...>;
        };

        template <CDevice... TDevices>
        [[nodiscard]] inline DeviceStorage<TDevices...> device_storage_builder_helper(TypeList<TDevices...> devices)
        {
            return DeviceStorage<TDevices...>{};
        }
    }  // namespace detail

    template <typename TDriverList>
    using DeviceStorageFromDriverList =
        typename detail::DeviceStorageBuilderHelper<typename detail::DirectDeps<TDriverList>::type>::type;

    template <typename TDriverList>
    [[nodiscard]] inline DeviceStorageFromDriverList<TDriverList> build_device_storage_from_driver_list()
    {
        using DirectDepsT = typename detail::DirectDeps<TDriverList>::type;
        return detail::device_storage_builder_helper(DirectDepsT{});
    }

    template <CHasDependDevices... TDrivers>
    using DeviceStorageFromDrivers = DeviceStorageFromDriverList<TypeList<TDrivers...>>;

    template <CHasDependDevices... TDrivers>
    [[nodiscard]] inline DeviceStorageFromDrivers<TDrivers...> build_device_storage_from_drivers()
    {
        return build_device_storage_from_driver_list<TypeList<TDrivers...>>();
    }

    template <CDevice... TDevices>
    using DeviceStorageFromDeviceTree =
        typename detail::DeviceStorageBuilderHelper<typename DeviceList<TDevices...>::Devices>::type;

    template <CDevice... TDevices>
    [[nodiscard]] inline DeviceStorageFromDeviceTree<TDevices...> build_device_storage_from_device_tree()
    {
        using DeviceGraph = typename DeviceList<TDevices...>::Devices;
        return detail::device_storage_builder_helper(DeviceGraph{});
    }

    template <CDeviceRefRegistry TDeviceRefRegistry, typename TDriverTuple = std::tuple<>>
    class DriverBuilder
    {
    private:
        TDeviceRefRegistry m_registry;
        TDriverTuple       m_drivers;

    public:
        DriverBuilder(TDeviceRefRegistry&& registry, TDriverTuple&& drivers)
            : m_registry(std::move(registry)), m_drivers(std::move(drivers))
        {
        }

        explicit DriverBuilder(TDeviceRefRegistry&& registry) : m_registry(std::move(registry)), m_drivers()
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
        [[nodiscard]] auto yield() &&
        {
            return std::make_pair(std::move(m_registry), std::move(m_drivers));
        }
        void yield() & = delete;
    };

    template <typename TDeviceStorage>
    [[nodiscard]] constexpr auto boot_driver_builder(TDeviceStorage& storage)
    {
        auto registry = storage.create_registry();
        return DriverBuilder<decltype(registry)>(std::move(registry));
    }

}  // namespace valle