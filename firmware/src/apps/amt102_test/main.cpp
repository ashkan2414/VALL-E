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

        while (true)
        {
            VALLE_LOG_INFO("Encoder Deg: {}", app::g_drivers.amt102.get_position_deg_abs());
            system::TimingContext::delay_ms(1000);
        }
    }

}  // namespace valle