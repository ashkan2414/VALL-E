#include <random>

#include "app.hpp"
#include "valle/app_bridge.hpp"
#include "valle/system/timing.hpp"

namespace valle
{
    void app::main()
    {
        app::init();

        uint64_t counter = 1;
        while (true)
        {
            VALLE_LOG_INFO("Hello World #{}!", counter++);
            TimingContext::delay_ms(1000);
        }
    }

}  // namespace valle