#include <random>

#include "app.hpp"
#include "valle/app_bridge.hpp"
#include "valle/system/timing.hpp"

namespace valle
{
    void app::main()
    {
        app::init();

        app::g_drivers.amt102.enable();

        uint64_t counter = 1;
        while (true)
        {
            VALLE_LOG_INFO("Hello World #{}! Encoder Deg: {}", counter++, app::g_drivers.amt102.get_position_deg_abs());
            TimingContext::delay_ms(1000);
        }
    }

}  // namespace valle