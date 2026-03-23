#include <random>

#include "app.hpp"
#include "valle/app_bridge.hpp"
#include "valle/system/timing.hpp"

namespace valle::app
{
    void main()
    {
        using namespace system;

        init();
        VALLE_LOG_INFO("Initialized!");
        TimingContext::delay_ms(3000);

        start();

        while (true)
        {
            g_drivers.intake_vca_current_loop_driver.set_target_current_amps(-0.3F);
            g_drivers.exhaust_vca_current_loop_driver.set_target_current_amps(0.3F);
            TimingContext::delay(system::DelayMillis(1000));
            g_drivers.intake_vca_current_loop_driver.set_target_current_amps(0.3F);
            g_drivers.exhaust_vca_current_loop_driver.set_target_current_amps(-0.3F);
            TimingContext::delay(system::DelayMillis(1000));
        }
    }

}  // namespace valle::app