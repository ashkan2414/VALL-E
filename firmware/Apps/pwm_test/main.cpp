#include <random>

#include "Valle/Core/app_bridge.hpp"
#include "app.hpp"

namespace valle
{
    void app::main()
    {
        delay_ms(1000);

        app::init();
        app::g_drivers.hb_driver.enable_output();

        while (1)
        {
            VALLE_LOG_INFO("Hello World #{}!", app::g_rep_counter.load(std::memory_order_relaxed));
            delay_ms(1000);
        }
    }

}  // namespace valle