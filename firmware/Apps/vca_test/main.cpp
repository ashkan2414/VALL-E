#include <random>

#include "Valle/Modules/vca.hpp"
#include "app_bridge.h"

float current_sensor_reading()
{
    return 0.0f;
}

auto g_vca = VCA<HRTIMHalfBridgePolicyA>(delegate::Delegate<float>(&current_sensor_reading));
BIND_VCA_ISR(g_vca);

void app_entry(void)
{
    g_vca.init();

    while (1)
    {
        g_vca.set_target_current(random() % 5);  // Set random target between 0-5A
    }
}