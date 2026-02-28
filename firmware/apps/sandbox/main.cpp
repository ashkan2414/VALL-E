#include <random>

#include "app.hpp"
#include "valle/core/app_bridge.hpp"

namespace valle
{
    void app::main()
    {
        app::init();

        VALLE_LOG_INFO("Starting VCA Controller Test Application");

        app::start_vca_controller();

        while (1)
        {
            app::g_drivers.vca_controller.set_target_current(0.0F);  // Set target current to 0.0 Amps
        }
    }
}  // namespace valle