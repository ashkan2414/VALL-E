#pragma once

#include "Valle/Device/devices/hrtim.hpp"
#include "Valle/Device/registry.hpp"

using SystemDeviceStorage = DeviceStorage<HRTIMDevice>;

namespace dont_use_me
{
    extern SystemDeviceStorage g_device_storage;
}

inline auto boot_driver_builder()
{
    return boot_driver_builder(dont_use_me::g_device_storage);
}
