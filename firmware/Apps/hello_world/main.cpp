#include <random>

#include "Valle/Core/Logging/logger.hpp"
#include "app.hpp"
#include "app_bridge.h"

using namespace valle;

void app_entry(void)
{
    app::init();

    uint64_t counter = 1;
    while (1)
    {
        VALLE_LOG_INFO("Hello World #{}!", counter++);
        delay_ms(1000);
    }
}