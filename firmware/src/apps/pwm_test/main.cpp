#include <random>

#include "app.hpp"
#include "valle/app_bridge.hpp"
#include "valle/system/timing.hpp"

namespace valle
{
    void app::main()
    {
        system::TimingContext::delay_ms(1000);

        app::init();
        app::g_drivers.hb_driver.enable_output();

        uint32_t step = 0;

        while (true)
        {
            system::TimingContext::delay_ms(1000);

            step                 = (step + 10U) % 180U;
            uint32_t    triangle = (step <= 90U) ? step : (180U - step);
            const float duty     = static_cast<float>(triangle + 5U) / 100.0F;

            app::g_drivers.hb_driver.set_duty_cycle(duty);

            VALLE_LOG_INFO("Current Duty Cycle: {:.2f}", duty);
        }
    }

}  // namespace valle