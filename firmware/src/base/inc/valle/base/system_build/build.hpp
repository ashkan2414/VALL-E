#pragma once

#include "app_system_config.hpp"
#include "valle/base/system_build/registry.hpp"
#include "valle/base/system_build/traits.hpp"

// ============================================================================
// Global Device Storage and Registry
// ============================================================================
namespace valle::system_build
{
    using DriversT        = typename AppSystemBuildConfig::DriverContainerT::DriversT;
    using HardwareTraitsT = TraitsFromDriverList<DriversT>;
    using DeviceStorageT  = typename HardwareTraitsT::DeviceStorageT;

    namespace internal
    {
        struct SystemBuild : public SystemBuildBase<DeviceStorageT>
        {
            using BaseT = SystemBuildBase<DeviceStorageT>;
            using BaseT::BaseT;
        };

        extern SystemBuild g_system_build;

        [[nodiscard]] inline auto boot_driver_builder()
        {
            return boot_driver_builder(g_system_build.storage);
        }
    }  // namespace internal

}  // namespace valle::system_build

namespace valle::app
{
    using DriversT             = system_build::DriversT;
    using DriverContainerT     = typename AppSystemBuildConfig::DriverContainerT;
    using DeviceRefRegistryT   = typename system_build::HardwareTraitsT::DeviceRefRegistryT;
    using DriversTupleT        = typename TypeListToTuple<DriversT>::type;
    using DriverBuilderT       = decltype(system_build::internal::boot_driver_builder());
    using DriverBuilderReturnT = std::pair<DeviceRefRegistryT, DriversTupleT>;

    // Application specific driver installation, needs to be defined in app
    DriverBuilderReturnT install_drivers(DriverBuilderT&& builder);

    // Global variables that app can use
    extern DeviceRefRegistryT g_ref_registry;
    extern DriverContainerT   g_drivers;

}  // namespace valle::app