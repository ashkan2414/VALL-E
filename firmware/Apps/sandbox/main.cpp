#include <random>

#include "Valle/Core/app_bridge.hpp"
#include "app.hpp"

namespace valle
{
    void app::main()
    {
        app::init();

        VALLE_LOG_INFO("Starting VCA Controller Test Application");

        app::start_vca_controller();

        while (1)
        {
            app::g_drivers.vca.set_target_current(static_cast<float>(rand()) / RAND_MAX * 0.6F -
                                                  0.3F);  // Set random target between -0.3 to 0.3 Amps
        }
    }
}  // namespace valle