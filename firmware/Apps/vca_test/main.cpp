#include <random>

#include "app_bridge.h"
#include "system_hardware.hpp"

using namespace valle;

void app_entry(void)
{
    system::init();
    system::start_vca_controller();

    while (1)
    {
        system::g_vca.set_target_current(static_cast<float>(rand()) / RAND_MAX * 0.6F -
                                         0.3F);  // Set random target between -0.3 to 0.3 Amps
    }
}