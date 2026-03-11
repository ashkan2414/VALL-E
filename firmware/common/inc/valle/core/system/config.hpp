#pragma once

#include "valle/core/device/registry.hpp"

namespace valle
{
    template <typename TDriverContainer>
    struct SystemConfigBase
    {
        using DriverContainerT = TDriverContainer;
    };

    namespace system::internal
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
    }  // namespace system::internal

    template <typename TDriversList>
    using RootDevicesFromDrivers = ToDeviceTypeList<typename TupleToTypeList<
        typename system::internal::BuildFromDriverListTraits<TDriversList>::DeviceRefRegistryT::RegistryTupleT>::type>;

}  // namespace valle