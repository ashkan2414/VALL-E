#pragma once

#include <functional>

#include "Valle/Utils/template_utils.hpp"

namespace valle
{

    // =============================================================================
    // DEVICE DESCRIPTORS
    // =============================================================================

    template <bool tkShared = false>
    struct DeviceDescriptor
    {
        constexpr static bool skIsShared = tkShared;
    };

    using UniqueDeviceDescriptor = DeviceDescriptor<false>;
    using SharedDeviceDescriptor = DeviceDescriptor<true>;

    struct InterfaceDeviceDescriptor
    {
        using InterfaceDeviceDescriptorTag = void;
    };

    struct NullDeviceDescriptor
    {
        using NullDeviceDescriptorTag = void;
    };

    template <typename T>
    concept CDevice = requires { typename T::Descriptor; };

    template <typename T>
    concept CUniqueDevice = CDevice<T> && !T::Descriptor::skIsShared;
    template <typename T>
    concept CSharedDevice = CDevice<T> && T::Descriptor::skIsShared;
    template <typename T>
    concept CInterfaceDevice = CDevice<T> && requires { typename T::Descriptor::InterfaceDeviceDescriptorTag; };
    template <typename T>
    concept CNullDevice = CDevice<T> && requires { typename T::Descriptor::NullDeviceDescriptorTag; };

    template <typename T>
    concept CHasChildrenDevices = CDevice<T> && requires { typename T::Descriptor::Children; };

    // =============================================================================
    // SYSTEM HARDWARE SINGLETON ACCESS
    // =============================================================================
    namespace system::internal
    {
        template <typename TStorage>
        struct HardwareBase
        {
            TStorage storage;

            explicit HardwareBase(TStorage&& storage_init) : storage(std::move(storage_init))
            {
            }
        };

        struct Hardware;
        extern void* gp_system_hardware;

        struct GlobalDeviceStorageAccessor
        {
            template <CDevice TDevice>
            TDevice& get()
            {
                return static_cast<Hardware*>(gp_system_hardware)->storage.template get<TDevice>();
            }
        };

    }  // namespace system::internal

    // =============================================================================
    // DEVICE REFERENCE WRAPPERS
    // =============================================================================

    template <typename TDevice>
    class UniqueDeviceRef
    {
    public:
        using UniqueDeviceRefTag = void;
        using DeviceT            = TDevice;

    private:
    public:
        UniqueDeviceRef() = delete;

        explicit UniqueDeviceRef(DeviceT& device) noexcept
        {
        }

        // Move Constructor
        UniqueDeviceRef(UniqueDeviceRef&& other) noexcept
        {
        }

        // Move Assignment
        UniqueDeviceRef& operator=(UniqueDeviceRef&& other) noexcept
        {
            return *this;
        }

        // Access the underlying Device
        DeviceT& get() noexcept
        {
            return system::internal::GlobalDeviceStorageAccessor{}.get<DeviceT>();
        }

        const DeviceT& get() const noexcept
        {
            return system::internal::GlobalDeviceStorageAccessor{}.get<DeviceT>();
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

    template <typename TDevice>
    class SharedDeviceRef
    {
    public:
        using SharedDeviceRefTag = void;
        using DeviceT            = TDevice;

    private:
    public:
        SharedDeviceRef() = delete;

        explicit SharedDeviceRef(const DeviceT& device) noexcept
        {
        }

        // Copy Constructor
        SharedDeviceRef(const SharedDeviceRef& other) noexcept
        {
        }

        // Move Constructor
        SharedDeviceRef(SharedDeviceRef&& other) noexcept
        {
        }

        // Copy Assignment
        SharedDeviceRef& operator=(const SharedDeviceRef& other) noexcept
        {
            return *this;
        }

        // Move Assignment
        SharedDeviceRef& operator=(SharedDeviceRef&& other) noexcept
        {
            return *this;
        }

        // Access the underlying Device
        DeviceT& get() noexcept
        {
            return system::internal::GlobalDeviceStorageAccessor{}.get<DeviceT>();
        }

        const DeviceT& get() const noexcept
        {
            return system::internal::GlobalDeviceStorageAccessor{}.get<DeviceT>();
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

    template <typename TDevice>
    class NullDeviceRef
    {
    public:
        explicit NullDeviceRef(TDevice& device)
        {
        }
    };

    template <CDevice TDevice>
    using DeviceRef = std::conditional_t<
        CNullDevice<TDevice>,
        NullDeviceRef<TDevice>,
        std::conditional_t<CSharedDevice<TDevice>, SharedDeviceRef<TDevice>, UniqueDeviceRef<TDevice>>>;

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
    class DeviceList;

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

        // Case 2: T is a DeviceList<Ts...>
        // We must expand the pack and recursively flatten each element.
        template <typename... InnerTs>
        struct DeviceFlattener<DeviceList<InnerTs...>>
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
            using ChildrenRaw  = typename T::Descriptor::Children;  // This is a DeviceList<...>
            using ChildrenFlat = typename DeviceFlattener<ChildrenRaw>::type;

            // Interface device: take only children
            // Non-interface device: take T + children
            using type = std::conditional_t<CInterfaceDevice<T>,
                                            ChildrenFlat,
                                            typename TypeListMergeUniqueMulti<TypeList<T>, ChildrenFlat>::type>;
        };

    }  // namespace detail

    template <typename... Ts>
    class DeviceList
    {
    public:
        // This expands the incoming Ts... into one flat tuple of leaf devices
        using Devices = typename TypeListMergeUniqueMulti<typename detail::DeviceFlattener<Ts>::type...>::type;
    };

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
    };

    // Helper concept to check for additional dependencies
    template <typename T>
    concept CHasAdditionalDependDevices =
        requires { typename T::DependDevices; } && TypeListSize<typename T::DependDevices>::value > 0;

    // Helper to get dependencies as a tuple type
    template <typename T>
    struct GetAdditionalDependDevices
    {
        using type = TypeList<>;
    };

    template <CHasAdditionalDependDevices T>
    struct GetAdditionalDependDevices<T>
    {
        using type = typename T::DependDevices;
    };

    // Helper concept to check for dependencies
    template <typename T>
    concept CHasDependDevices = CHasAdditionalDependDevices<T> || CHasInjectDevices<T>;

    // Helper to get dependencies as a tuple type
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
    concept CUnboundIsrHandler = requires { typename T::UnboundIsrHandlerTag; };

    template <typename T>
    concept CBoundIsrHandler = !CUnboundIsrHandler<T>;

}  // namespace valle