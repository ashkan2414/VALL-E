#pragma once

#include "Valle/Device/device.hpp"

template <CHasDependDevices... TDrivers>
struct SystemBuildFromDriversTraits
{
    using DeviceStorageT     = DeviceStorageFromDrivers<TDrivers...>;
    using DeviceRefRegistryT = DeviceRefRegistryAfterClaims<typename DeviceStorageT::RegistryT, TDrivers...>;
};
