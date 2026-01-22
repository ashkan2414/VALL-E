#include <random>

#include "Valle/Device/device.hpp"
#include "Valle/Modules/vca.hpp"
#include "app_bridge.h"

float current_sensor_reading()
{
    return 0.0f;
}

auto g_storage = build_device_storage_from_drivers<VCAControllerModule<HRTIMTimerDeviceA>>();

auto [g_device_ref_pool, g_vca] =
    boot_driver_builder(g_storage)
        .install<VCAControllerModule<HRTIMTimerDeviceA>>(delegate::Delegate<float>(&current_sensor_reading))
        .yield();

BIND_VCA_ISR(g_vca);

void app_entry(void)
{
    g_device_ref_pool.foreach_shared(Overloaded{
        [](HRTIMDevice& dev) { dev.init(); },
    });

    g_vca.init();

    while (1)
    {
        g_vca.set_target_current(random() % 5);  // Set random target between 0-5A
    }
}