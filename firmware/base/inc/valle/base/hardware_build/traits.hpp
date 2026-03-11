#pragma once

#include "valle/base/hardware_build/registry.hpp"

namespace valle
{
    namespace hardware_build
    {
        // Helper to get build traits from driver list
        template <typename TDriverList>
        struct TraitsFromDriverList
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

            using RootDevicesT =
                ToDeviceTypeList<typename TupleToTypeList<typename DeviceRefRegistryT::RegistryTupleT>::type>;
        };

        template <CHasDependDevices... TDrivers>
        using TraitsFromDrivers = TraitsFromDriverList<TypeList<TDrivers...>>;

        template <typename TDriversList>
        using RootDevicesFromDriverList = typename TraitsFromDriverList<TDriversList>::RootDevicesT;

    }  // namespace hardware_build

}  // namespace valle
