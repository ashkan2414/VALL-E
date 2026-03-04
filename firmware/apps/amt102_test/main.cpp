#include <random>

#include "app.hpp"
#include "valle/core/app_bridge.hpp"

namespace valle
{
    void app::main()
    {
        app::init();

        app::g_drivers.amt102.enable();

        uint64_t counter = 1;
        while (1)
        {
            VALLE_LOG_INFO("Hello World #{}! Encoder Deg: {}", counter++, app::g_drivers.amt102.get_position_deg_abs());
            delay_ms(1000);
        }
    }

}  // namespace valle