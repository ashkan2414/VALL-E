#pragma once

#include "Valle/Device/registry.hpp"
#include "app_system_config.hpp"

// ============================================================================
// Global Device Storage and Registry
// ============================================================================
namespace valle
{
    namespace system
    {
        namespace internal
        {
            // Helper to get build traits from driver list
            template <typename TDriverList>
            struct BuildFromDriverListTraits
            {
            private:
                template <typename TRegistry, typename TList>
                struct DeviceRefRegistryAfterClaimsHelper;

                template <typename TRegistry, typename... TDrivers>
                struct DeviceRefRegistryAfterClaimsHelper<TRegistry, TypeList<TDrivers...>>
                {
                    using type = DeviceRefRegistryAfterClaims<TRegistry, TDrivers...>;
                };

            public:
                using DriversT       = TDriverList;
                using DeviceStorageT = DeviceStorageFromDriverList<TDriverList>;
                using DeviceRefRegistryT =
                    typename DeviceRefRegistryAfterClaimsHelper<typename DeviceStorageT::RegistryT, TDriverList>::type;
            };

            template <CHasDependDevices... TDrivers>
            using BuildFromDriversTraits = BuildFromDriverListTraits<TypeList<TDrivers...>>;
        }  // namespace internal

        using DriversT        = typename Config::DriversT;
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
        using DriverContainerT     = typename system::Config::DriverContainerT;
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
