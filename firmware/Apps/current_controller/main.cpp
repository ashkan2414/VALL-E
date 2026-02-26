#include <random>

#include "Valle/Core/app_bridge.hpp"
#include "app.hpp"

namespace valle
{
    void app::main()
    {
        delay_ms(1000);

        app::init();

        app::g_devices.adc3->start_inject();
        app::g_drivers.vca_controller.enable();

        uint32_t counter = 0;

        while (true)
        {
            VALLE_LOG_INFO("Hello World #{}! Current: {} A", ++counter, app::g_drivers.current_sensor.read_amps());
            delay_ms(1000);
        }
    }

}  // namespace valle