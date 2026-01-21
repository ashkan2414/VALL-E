#pragma once

#include <functional>

#include "Valle/Utils/template_utils.hpp"

template <bool tkShared = false>
class DeviceDescriptor
{
public:
    constexpr static bool skIsShared = tkShared;
};

using UniqueDeviceDescriptor = DeviceDescriptor<false>;
using SharedDeviceDescriptor = DeviceDescriptor<true>;

template <typename T>
concept CDevice = requires {
    typename T::Descriptor;
    { T::Descriptor::skIsShared } -> std::convertible_to<bool>;
};

template <typename T>
concept CUniqueDevice = CDevice<T> && !T::Descriptor::skIsShared;
template <typename T>
concept CSharedDevice = CDevice<T> && T::Descriptor::skIsShared;

template <typename T>
concept CHasChildrenDevices = CDevice<T> && requires { typename T::Descriptor::Children; };

template <typename TDevice>
class UniqueDeviceRef
{
public:
    using UniqueDeviceRefTag = void;
    using DeviceT            = TDevice;

private:
    std::reference_wrapper<DeviceT> m_Device;

public:
    explicit UniqueDeviceRef(DeviceT& Device) : m_Device(Device)
    {
    }

    // Move Constructor
    UniqueDeviceRef(UniqueDeviceRef&& other) noexcept : m_Device(other.m_Device)
    {
    }

    // Move Assignment
    UniqueDeviceRef& operator=(UniqueDeviceRef&& other) noexcept
    {
        m_Device = other.m_Device;
        return *this;
    }

    // Access the underlying Device
    DeviceT& get() noexcept
    {
        return m_Device.get();
    }

    const DeviceT& get() const noexcept
    {
        return m_Device.get();
    }

    // Operator overloads for easier access
    DeviceT* operator->() noexcept
    {
        return &m_Device.get();
    }

    const DeviceT* operator->() const noexcept
    {
        return &m_Device.get();
    }

    DeviceT& operator*() noexcept
    {
        return m_Device.get();
    }

    const DeviceT& operator*() const noexcept
    {
        return m_Device.get();
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
    std::reference_wrapper<DeviceT> m_Device;

public:
    explicit SharedDeviceRef(DeviceT& Device) : m_Device(Device)
    {
    }

    // Copy Constructor
    SharedDeviceRef(const SharedDeviceRef& other) noexcept : m_Device(other.m_Device)
    {
    }

    // Move Constructor
    SharedDeviceRef(SharedDeviceRef&& other) noexcept : m_Device(other.m_Device)
    {
    }

    // Copy Assignment
    SharedDeviceRef& operator=(const SharedDeviceRef& other) noexcept
    {
        m_Device = other.m_Device;
        return *this;
    }

    // Move Assignment
    SharedDeviceRef& operator=(SharedDeviceRef&& other) noexcept
    {
        m_Device = other.m_Device;
        return *this;
    }

    // Access the underlying Device
    DeviceT& get() noexcept
    {
        return m_Device.get();
    }

    const DeviceT& get() const noexcept
    {
        return m_Device.get();
    }

    // Operator overloads for easier access
    DeviceT* operator->() noexcept
    {
        return &m_Device.get();
    }

    const DeviceT* operator->() const noexcept
    {
        return &m_Device.get();
    }

    DeviceT& operator*() noexcept
    {
        return m_Device.get();
    }

    const DeviceT& operator*() const noexcept
    {
        return m_Device.get();
    }
};

template <CDevice TDevice>
using DeviceRef =
    std::conditional_t<TDevice::Descriptor::skIsShared, SharedDeviceRef<TDevice>, UniqueDeviceRef<TDevice>>;

template <typename T>
concept CSharedDeviceRef = requires { typename T::SharedDeviceRefTag; };

template <typename T>
concept CUniqueDeviceRef = requires { typename T::UniqueDeviceRefTag; };

template <typename T>
concept CDeviceRef = CSharedDeviceRef<T> || CUniqueDeviceRef<T>;

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
        using type = TypeList<T>;
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

        // Merge Parent + Children
        using type = typename TypeListMergeUniqueMulti<TypeList<T>, ChildrenFlat>::type;
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
concept CHasInjectDevices = requires { typename T::InjectDevices; };

// Helper to get dependencies as a tuple type
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
