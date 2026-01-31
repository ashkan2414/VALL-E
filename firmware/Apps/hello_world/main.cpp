#include <random>

#include "Valle/Core/app_bridge.hpp"
#include "app.hpp"

namespace valle
{
    void app::main()
    {
        app::init();

        uint64_t counter = 1;
        while (1)
        {
            VALLE_LOG_INFO("Hello World #{}!", counter++);
            delay_ms(1000);
        }
    }

}  // namespace valle