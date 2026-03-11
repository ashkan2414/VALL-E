#pragma once

#include "app_system_config.hpp"
#include "valle/base/hardware_build/hardware_build_traits.hpp"
#include "valle/base/hardware_build/registry.hpp"

// ============================================================================
// Global Device Storage and Registry
// ============================================================================
namespace valle::hardware_build
{
    using DriversT        = typename AppHardwareBuildConfig::DriverContainerT::DriversT;
    using HardwareTraitsT = internal::BuildFromDriverListTraits<DriversT>;
    using DeviceStorageT  = typename HardwareTraitsT::DeviceStorageT;

    namespace internal
    {
        struct HardwareBuild : public HardwareBuildBase<DeviceStorageT>
        {
            using BaseT = HardwareBuildBase<DeviceStorageT>;
            using BaseT::BaseT;
        };

        extern HardwareBuild g_system_hardware;

        [[nodiscard]] inline auto boot_driver_builder()
        {
            return boot_driver_builder(g_system_hardware.storage);
        }
    }  // namespace internal

}  // namespace valle::hardware_build

namespace valle::app
{
    using DriversT             = hardware_build::DriversT;
    using DriverContainerT     = typename AppHardwareBuildConfig::DriverContainerT;
    using DeviceRefRegistryT   = typename hardware_build::HardwareTraitsT::DeviceRefRegistryT;
    using DriversTupleT        = typename TypeListToTuple<DriversT>::type;
    using DriverBuilderT       = decltype(hardware_build::internal::boot_driver_builder());
    using DriverBuilderReturnT = std::pair<DeviceRefRegistryT, DriversTupleT>;

    // Application specific driver installation, needs to be defined in app
    DriverBuilderReturnT install_drivers(DriverBuilderT&& builder);

    // Global variables that app can use
    extern DeviceRefRegistryT g_ref_registry;
    extern DriverContainerT   g_drivers;

}  // namespace valle::app