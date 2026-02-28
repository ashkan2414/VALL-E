#include <random>

#include "app.hpp"
#include "valle/core/app_bridge.hpp"


namespace valle
{
    void app::main()
    {
        delay_ms(1000);

        app::init();
        app::g_drivers.hb_driver.enable_output();

        uint32_t step = 0;

        while (1)
        {
            delay_ms(1000);

            step                 = (step + 10U) % 180U;
            uint32_t    triangle = (step <= 90U) ? step : (180U - step);
            const float duty     = static_cast<float>(triangle + 5U) / 100.0F;

            app::g_drivers.hb_driver.set_duty_cycle(duty);

            VALLE_LOG_INFO(
                "Hello World! Rep #{}, Duty Cycle: {:.2f}", app::g_rep_counter.load(std::memory_order_relaxed), duty);
        }
    }

}  // namespace valle