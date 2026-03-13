#pragma once

#include <functional>
#include <variant>

#include "valle/utils/tmp.hpp"

namespace valle
{
    // =============================================================================
    // DEVICE DESCRIPTORS
    // =============================================================================

    enum class DeviceType
    {
        kUnique,
        kShared,
        kInterface,
        kNull
    };

    template <DeviceType tkDeviceType>
    struct DeviceDescriptor
    {
        constexpr static DeviceType skDeviceType = tkDeviceType;
        constexpr static bool       skNeedsInit  = true;
    };

    using UniqueDeviceDescriptor    = DeviceDescriptor<DeviceType::kUnique>;
    using SharedDeviceDescriptor    = DeviceDescriptor<DeviceType::kShared>;
    using ProxyDeviceDescriptor     = SharedDeviceDescriptor;
    using InterfaceDeviceDescriptor = DeviceDescriptor<DeviceType::kInterface>;
    using NullDeviceDescriptor      = DeviceDescriptor<DeviceType::kNull>;

    template <typename TDevice>
    static constexpr DeviceType kDeviceType = TDevice::Descriptor::skDeviceType;

    template <typename T>
    concept CDevice = requires { typename T::Descriptor; };

    template <typename T, DeviceType tkDeviceType>
    concept CDeviceOfType = CDevice<T> && kDeviceType<T> == tkDeviceType;

    template <typename T>
    concept CUniqueDevice = CDeviceOfType<T, DeviceType::kUnique>;
    template <typename T>
    concept CSharedDevice = CDeviceOfType<T, DeviceType::kShared>;
    template <typename T>
    concept CInterfaceDevice = CDeviceOfType<T, DeviceType::kInterface>;
    template <typename T>
    concept CNullDevice = CDeviceOfType<T, DeviceType::kNull>;

    template <typename T>
    concept CHasChildrenDevices = CDevice<T> && requires { typename T::Descriptor::Children; };

    // ============================================================================
    // Initialization Traits
    // ============================================================================

    template <CDevice TDevice>
    inline constexpr bool kDeviceNeedsInit = requires {
        { TDevice::Descriptor::skNeedsInit } -> std::convertible_to<bool>;
    } ? static_cast<bool>(TDevice::Descriptor::skNeedsInit) : true;

    template <class... Ts>
    struct DeviceInitOverloaded : Ts...
    {
        using Ts::operator()...;

        void operator()(auto& dev) const
        {
            using DeviceT = std::remove_cvref_t<decltype(dev)>;
            static_assert(CDevice<DeviceT>, "default_init_device can only be used with valid devices");
            constexpr bool needs_init = kDeviceNeedsInit<DeviceT>;
            static_assert(!needs_init || kAlwaysFalseT<DeviceT>, "Device needs explicit initialization");
        }
    };
    template <class... Ts>
    DeviceInitOverloaded(Ts...) -> DeviceInitOverloaded<Ts...>;

    // =============================================================================
    // SYSTEM HARDWARE SINGLETON ACCESS
    // =============================================================================
    namespace system_build::internal
    {
        template <typename TStorage>
        struct SystemBuildBase
        {
            TStorage storage;

            explicit SystemBuildBase(TStorage&& storage_init) : storage(std::move(storage_init))
            {
            }
        };

        struct SystemBuild;
        extern void* gp_system_build;

        struct GlobalDeviceStorageAccessor
        {
            template <CDevice TDevice>
            TDevice& get()
            {
                return static_cast<SystemBuild*>(gp_system_build)->storage.template get<TDevice>();
            }
        };

    }  // namespace system_build::internal

    // =============================================================================
    // DEVICE REFERENCE WRAPPERS
    // =============================================================================

    template <CUniqueDevice TDevice>
    class UniqueDeviceRef
    {
    public:
        using UniqueDeviceRefTag = void;
        using DeviceT            = TDevice;

        UniqueDeviceRef() = delete;

        explicit UniqueDeviceRef(DeviceT& device) noexcept
        {
            (void)device;  // Silence unused parameter warning
        }

        // Move Constructor
        UniqueDeviceRef(UniqueDeviceRef&& other) noexcept
        {
            (void)other;  // Silence unused parameter warning
        }

        // Move Assignment
        UniqueDeviceRef& operator=(UniqueDeviceRef&& other) noexcept
        {
            return *this;
        }

        // Access the underlying Device
        DeviceT& get() noexcept
        {
            return system_build::internal::GlobalDeviceStorageAccessor{}.get<DeviceT>();
        }

        const DeviceT& get() const noexcept
        {
            return system_build::internal::GlobalDeviceStorageAccessor{}.get<DeviceT>();
        }

        // Operator overloads for easier access
        DeviceT* operator->() noexcept
        {
            return &get();
        }

        const DeviceT* operator->() const noexcept
        {
            return &get();
        }

        DeviceT& operator*() noexcept
        {
            return get();
        }

        const DeviceT& operator*() const noexcept
        {
            return get();
        }

    private:
        // Disable Copy Constructor and Copy Assignment
        UniqueDeviceRef(const UniqueDeviceRef&)            = delete;
        UniqueDeviceRef& operator=(const UniqueDeviceRef&) = delete;
    };

    template <CSharedDevice TDevice>
    class SharedDeviceRef
    {
    public:
        using SharedDeviceRefTag = void;
        using DeviceT            = TDevice;

        SharedDeviceRef() = delete;

        explicit SharedDeviceRef(const DeviceT& device) noexcept
        {
            (void)device;  // Silence unused parameter warning
        }

        // Access the underlying Device
        DeviceT& get() noexcept
        {
            return system_build::internal::GlobalDeviceStorageAccessor{}.get<DeviceT>();
        }

        const DeviceT& get() const noexcept
        {
            return system_build::internal::GlobalDeviceStorageAccessor{}.get<DeviceT>();
        }

        // Operator overloads for easier access
        DeviceT* operator->() noexcept
        {
            return &get();
        }

        const DeviceT* operator->() const noexcept
        {
            return &get();
        }

        DeviceT& operator*() noexcept
        {
            return get();
        }

        const DeviceT& operator*() const noexcept
        {
            return get();
        }
    };

    template <CNullDevice TDevice>
    class NullDeviceRef
    {
    public:
        NullDeviceRef() = delete;

        explicit NullDeviceRef(TDevice& device)
        {
        }
    };

    namespace detail
    {
        template <CDevice TDevice, DeviceType tkDeviceType>
        struct DeviceRefSelector;

        // unique device
        template <CDevice TDevice>
            requires(kDeviceType<TDevice> == DeviceType::kUnique)
        struct DeviceRefSelector<TDevice, DeviceType::kUnique>
        {
            using type = UniqueDeviceRef<TDevice>;
        };

        // shared device
        template <CDevice TDevice>
            requires(kDeviceType<TDevice> == DeviceType::kShared)
        struct DeviceRefSelector<TDevice, DeviceType::kShared>
        {
            using type = SharedDeviceRef<TDevice>;
        };

        // null device
        template <CDevice TDevice>
            requires(kDeviceType<TDevice> == DeviceType::kNull)
        struct DeviceRefSelector<TDevice, DeviceType::kNull>
        {
            using type = NullDeviceRef<TDevice>;
        };

    }  // namespace detail

    template <CDevice TDevice>
        requires(!CInterfaceDevice<TDevice>)
    using DeviceRef = typename detail::DeviceRefSelector<TDevice, kDeviceType<TDevice>>::type;

    template <bool tkCondition, typename TDevice, typename TFalseRef = std::monostate>
    using ConditionalDeviceRef = std::conditional_t<tkCondition, DeviceRef<TDevice>, TFalseRef>;

    template <typename T>
    concept CSharedDeviceRef = requires { typename T::SharedDeviceRefTag; };

    template <typename T>
    concept CUniqueDeviceRef = requires { typename T::UniqueDeviceRefTag; };

    template <typename T>
    concept CNullDeviceRef = requires { typename T::NullDeviceRefTag; };

    template <typename T>
    concept CDeviceRef = CSharedDeviceRef<T> || CUniqueDeviceRef<T> || CNullDeviceRef<T>;

    template <typename... Ts>
    class DeviceTreeList;

    namespace detail
    {
        // The Recursive Flattener Struct
        // This struct accepts a Type T and exports 'type' which is a std::tuple of the leaf devices.
        template <typename T>
        struct DeviceFlattener
        {
            // Base Case: T is a leaf resource (no Children Devices, not a Registry)
            // Exclude interface devices
            using type = std::conditional_t<CInterfaceDevice<T>, TypeList<>, TypeList<T>>;
        };

        // Case 2: T is a DeviceTreeList<Ts...>
        // We must expand the pack and recursively flatten each element.
        template <typename... InnerTs>
        struct DeviceFlattener<DeviceTreeList<InnerTs...>>
        {
            // tuple_cat joins the tuples resulting from flattening each InnerT
            using type = typename TypeListMergeUniqueMulti<typename DeviceFlattener<InnerTs>::type...>::type;
        };

        // Case 3: T has a Children Devices alias
        // We ignore T itself and recursively flatten the Children Devices type.
        template <CHasChildrenDevices T>
        struct DeviceFlattener<T>
        {
            // Flatten the Children
            using ChildrenRaw  = typename T::Descriptor::Children;  // This is a DeviceTreeList<...>
            using ChildrenFlat = typename DeviceFlattener<ChildrenRaw>::type;

            // Interface device: take only children
            // Non-interface device: take T + children
            using type = std::conditional_t<CInterfaceDevice<T>,
                                            ChildrenFlat,
                                            typename TypeListMergeUniqueMulti<TypeList<T>, ChildrenFlat>::type>;
        };

    }  // namespace detail

    template <typename... Ts>
    class DeviceTreeList
    {
    public:
        // This expands the incoming Ts... into one flat tuple of leaf devices
        using Devices = typename TypeListMergeUniqueMulti<typename detail::DeviceFlattener<Ts>::type...>::type;
    };

    // =============================================================================
    // Device Dependency Introspection Helpers
    // =============================================================================
    template <typename T>
    struct ToDeviceRef
    {
        using type = DeviceRef<T>;
    };

    template <typename TDeviceList>
    using ToDeviceRefTuple = typename TypeListToTuple<typename TransformTypeList<TDeviceList, ToDeviceRef>::type>::type;

    template <typename T>
    struct ToDevice
    {
        using type = typename T::DeviceT;
    };

    template <typename TDeviceRefList>
    using ToDeviceTuple = typename TypeListToTuple<typename TransformTypeList<TDeviceRefList, ToDevice>::type>::type;

    template <typename TDeviceRefList>
    using ToDeviceTypeList = typename TupleToTypeList<ToDeviceTuple<TDeviceRefList>>::type;

    // =============================================================================
    // DEPENDENCY INSPECTION HELPERS
    // =============================================================================

    template <typename T>
    struct IsNotInterfaceDevice : std::bool_constant<!CInterfaceDevice<T>>
    {
    };

    template <typename TDeviceList>
    using FilterInterfaceDevices = typename FilterTypeList<IsNotInterfaceDevice, TDeviceList>::type;

    template <typename T>
    struct IsNotNullDevice : std::bool_constant<!CNullDevice<T>>
    {
    };

    template <typename TDeviceList>
    using FilterNullDevices = typename FilterTypeList<IsNotNullDevice, TDeviceList>::type;

    template <typename T>
    concept CHasInjectDevices =
        requires { typename T::InjectDevices; } && TypeListSize<typename T::InjectDevices>::value > 0;

    // Helper to get injection dependencies as a tuple type
    template <typename T>
    struct GetInjectDevices
    {
        using type = TypeList<>;
    };

    template <CHasInjectDevices T>
    struct GetInjectDevices<T>
    {
        using type = typename T::InjectDevices;
        static_assert(TypeListAll<IsNotInterfaceDevice, type>::value,
                      "InjectDevices list cannot contain Interface Devices!");
    };

    // Helper concept to check for additional dependencies
    template <typename T>
    concept CHasAdditionalDependDevices =
        requires { typename T::DependDevices; } && TypeListSize<typename T::DependDevices>::value > 0;

    // Helper to get dependencies as a TypeList type
    template <typename T>
    struct GetAdditionalDependDevices
    {
        using type = TypeList<>;
    };

    template <CHasAdditionalDependDevices T>
    struct GetAdditionalDependDevices<T>
    {
        using type = FilterInterfaceDevices<typename T::DependDevices>;
    };

    // Helper concept to check for dependencies
    template <typename T>
    concept CHasDependDevices = CHasAdditionalDependDevices<T> || CHasInjectDevices<T>;

    // Helper to get dependencies as a TypeList type
    template <typename T>
    struct GetDependDevices
    {
        using type = TypeList<>;
    };

    template <CHasDependDevices T>
    struct GetDependDevices<T>
    {
        using type = typename TypeListMergeUnique<typename GetAdditionalDependDevices<T>::type,
                                                  typename GetInjectDevices<T>::type>::type;
    };

    // ISR Router

    template <typename T>
    concept CUnboundISRRouter = requires { typename T::UnboundIsrHandlerTag; };

    template <typename T>
    concept CBoundISRRouter = !CUnboundISRRouter<T>;

    template <bool tkAck = true>
    struct ISRRouterConfig
    {
        static constexpr bool skAck = tkAck;  // Whether to acknowledge the interrupt in the router before handling
    };

    template <typename T>
    concept CISRRouterHasConfig = requires { typename T::skConfig; };

    template <typename T>
    static constexpr auto kISRRouterConfig = []
    {
        if constexpr (CISRRouterHasConfig<T>)
        {
            return T::skConfig;
        }
        else
        {
            return ISRRouterConfig<>{};
        }
    }();

    template <typename T>
    static constexpr bool kISRRouterConfigAck = kISRRouterConfig<T>.skAck;

    // Constructor helper

    namespace detail
    {
        // Helper to find the index of a DeviceRef in a parameter pack by its DeviceT
        template <typename TTargetDevice, typename... TArgs>
        constexpr size_t find_device_index()
        {
            // Creates a boolean array at compile time checking each argument
            bool matches[] = {std::is_same_v<TTargetDevice, typename std::remove_cvref_t<TArgs>::DeviceT>...};
            for (size_t i = 0; i < sizeof...(TArgs); ++i)
            {
                if (matches[i]) return i;
            }
            return sizeof...(TArgs);  // Return out of bounds if not found
        }
    }  // namespace detail

    // Helper to find a specific type in a parameter pack
    template <bool tkCondition, typename TDevice, typename... TArgs>
    constexpr auto extract_device_ref(TArgs&&... args)
    {
        // Case 1: The device is optional/conditional and we don't want it (monostate)
        if constexpr (!tkCondition)
        {
            return std::monostate{};
        }
        else
        {
            // Case 2: We need the device.
            // Find the index of the argument that wraps the device type we need.
            constexpr size_t index = detail::find_device_index<TDevice, TArgs...>();

            // GATEKEEPER: Hard compile error if the type isn't in the pack
            static_assert(index < sizeof...(TArgs),
                          "\n\n[VALLE ERROR] Dependency Injection Failure!\n"
                          "A constructor requested a device dependency that was not provided in the arguments pack.\n"
                          "Check the DeviceT and TArgs types in the compiler error below.\n");

            // Use a tuple to safely access the specific argument by index.
            // std::tie/forward_as_tuple ensures we aren't copying the actual devices.
            auto pack = std::forward_as_tuple(args...);

            // No pointers, no reinterpret_cast.
            // We get the specific DeviceRef, call .get() to get the actual hardware,
            // and wrap it in a fresh DeviceRef for the caller.
            return DeviceRef<TDevice>(std::get<index>(pack).get());
        }
    }

    // =============================================================================
    // PACKED DRIVER BASE
    // ============================================================================
    template <typename TDeviceList>
    class PackedDriverBase
    {
    public:
        using InjectDevices = TDeviceList;
        using DeviceTuple   = ToDeviceRefTuple<InjectDevices>;

        [[no_unique_address]] DeviceTuple devices;

        PackedDriverBase() = delete;

        template <typename... TArgs>
            requires(std::constructible_from<DeviceTuple, TArgs...>)
        explicit constexpr PackedDriverBase(TArgs&&... args) : devices(std::forward<TArgs>(args)...)
        {
        }

        template <typename TDevice>
        constexpr auto& get()
        {
            return std::get<DeviceRef<TDevice>>(devices).get();
        }

        template <typename TDevice>
        constexpr const auto& get() const
        {
            return std::get<DeviceRef<TDevice>>(devices).get();
        }

        template <typename TVisitor>
        constexpr decltype(auto) visit(TVisitor&& visitor)
        {
            return std::apply([&visitor](auto&... device_refs) -> decltype(auto)
                              { return std::forward<TVisitor>(visitor)(device_refs.get()...); },
                              devices);
        }

        template <typename TVisitor>
        constexpr decltype(auto) visit(TVisitor&& visitor) const
        {
            return std::apply([&visitor](const auto&... device_refs) -> decltype(auto)
                              { return std::forward<TVisitor>(visitor)(device_refs.get()...); },
                              devices);
        }

        template <typename TVisitor>
        constexpr void foreach (TVisitor&& visitor)
        {
            std::apply(
                [&visitor](auto&... device_refs)
                {
                    // Call the visitor as an lvalue (no std::forward here)
                    (visitor(device_refs.get()), ...);
                },
                devices);
        }

        template <typename TVisitor>
        constexpr void foreach_reverse(TVisitor&& visitor)
        {
            constexpr auto size = std::tuple_size_v<DeviceTuple>;

            if constexpr (size > 0)
            {
                // Create a sequence 0, 1, 2, ... N-1
                [&]<std::size_t... Is>(std::index_sequence<Is...>)
                {
                    // Use the comma fold expression, but subtract the index from (size - 1)
                    // This results in indices: (N-1), (N-2), ... 0
                    (visitor(std::get<size - 1 - Is>(devices).get()), ...);
                }(std::make_index_sequence<size>{});
            }
        }

        template <typename TVisitor>
        constexpr void foreach (TVisitor&& visitor) const
        {
            std::apply([&visitor](const auto&... device_refs) { (visitor(device_refs.get()), ...); }, devices);
        }

        template <typename TVisitor>
        constexpr void foreach_reverse(TVisitor&& visitor) const
        {
            constexpr auto size = std::tuple_size_v<DeviceTuple>;

            if constexpr (size > 0)
            {
                // Create a sequence 0, 1, 2, ... N-1
                [&]<std::size_t... Is>(std::index_sequence<Is...>)
                {
                    // Use the comma fold expression, but subtract the index from (size - 1)
                    // This results in indices: (N-1), (N-2), ... 0
                    (visitor(std::get<size - 1 - Is>(devices).get()), ...);
                }(std::make_index_sequence<size>{});
            }
        }

        template <typename TDevice, typename TVisitor>
        constexpr decltype(auto) visit_one(TVisitor&& visitor)
        {
            return std::forward<TVisitor>(visitor)(get<TDevice>());
        }

        template <typename TDevice, typename TVisitor>
        constexpr decltype(auto) visit_one(TVisitor&& visitor) const
        {
            return std::forward<TVisitor>(visitor)(get<TDevice>());
        }
    };

}  // namespace valle

#define VALLE_DEFINE_PACKED_DEVICE_DRIVER_ACCESSOR(member_name, device_type) \
    constexpr auto& member_name()                                            \
    {                                                                        \
        return this->template get<device_type>();                            \
    }                                                                        \
                                                                             \
    constexpr const auto& member_name() const                                \
    {                                                                        \
        return this->template get<device_type>();                            \
    }