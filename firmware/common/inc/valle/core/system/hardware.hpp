#pragma once

#include "app_system_config.hpp"
#include "valle/core/device/registry.hpp"

// ============================================================================
// Global Device Storage and Registry
// ============================================================================
namespace valle
{
    namespace system
    {
        using DriversT        = typename AppSystemConfig::DriverContainerT::DriversT;
        using HardwareTraitsT = internal::BuildFromDriverListTraits<DriversT>;
        using DeviceStorageT  = typename HardwareTraitsT::DeviceStorageT;

        namespace internal
        {
            struct Hardware : public HardwareBase<DeviceStorageT>
            {
                using BaseT = HardwareBase<DeviceStorageT>;
                using BaseT::BaseT;
            };

            extern Hardware g_system_hardware;

            [[nodiscard]] inline auto boot_driver_builder()
            {
                return boot_driver_builder(g_system_hardware.storage);
            }
        }  // namespace internal
    }  // namespace system

    namespace app
    {
        using DriversT             = system::DriversT;
        using DriverContainerT     = typename AppSystemConfig::DriverContainerT;
        using DeviceRefRegistryT   = typename system::HardwareTraitsT::DeviceRefRegistryT;
        using DriversTupleT        = typename TypeListToTuple<DriversT>::type;
        using DriverBuilderT       = decltype(system::internal::boot_driver_builder());
        using DriverBuilderReturnT = std::pair<DeviceRefRegistryT, DriversTupleT>;

        // Application specific driver installation, needs to be defined in app
        DriverBuilderReturnT install_drivers(DriverBuilderT&& builder);

        // Global variables that app can use
        extern DeviceRefRegistryT g_ref_registry;
        extern DriverContainerT   g_drivers;

    }  // namespace app

}  // namespace valle
