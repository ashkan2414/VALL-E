#include <random>

#include "app.hpp"
#include "valle/app_bridge.hpp"

namespace valle
{
    void app::main()
    {
        using namespace system;

        TimingContext::delay_ms(3000);

        app::init();
        app::g_drivers.vca_current_loop_driver.start();

        VALLE_LOG_INFO("Initialized!");
        TimingContext::delay_ms(1000);

        while (true)
        {
            app::g_drivers.vca_current_loop_driver.set_target_current_amps(0.0F);  // Set target current to 0.0 Amps
        }
    }
}  // namespace valle