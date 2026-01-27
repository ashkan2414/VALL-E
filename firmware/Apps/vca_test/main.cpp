#include <random>

#include "app.hpp"
#include "app_bridge.h"


using namespace valle;

void app_entry(void)
{
    app::init();
    app::start_vca_controller();

    while (1)
    {
        app::g_drivers.vca.set_target_current(static_cast<float>(rand()) / RAND_MAX * 0.6F -
                                              0.3F);  // Set random target between -0.3 to 0.3 Amps
    }
}