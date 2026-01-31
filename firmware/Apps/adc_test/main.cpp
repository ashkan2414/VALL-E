#include <random>

#include "Valle/Core/app_bridge.hpp"
#include "app.hpp"

namespace valle
{
    void app::main()
    {
        delay_ms(1000);

        app::init();

        if constexpr (app::kTestADCUseInject)  // NOLINT(bugprone-branch-clone)
        {
            app::g_devices.adc1->enable_interrupts(ADCInterruptConfig{
                .priority           = 5,
                .enable_inj_eos_int = true,
            });
            app::g_devices.adc1->start_inject();
        }
        else
        {
            app::g_devices.adc1->start_regular();
        }

        uint32_t counter = 1;
        while (1)
        {
            delay_ms(500);

            VALLE_LOG_INFO("Hello World #{}!", counter++);

            delay_ms(500);

            // Software trigger for Regular group
            if constexpr (!app::kTestADCUseInject)  // NOLINT(bugprone-branch-clone)
            {
                app::g_devices.adc1->trigger_regular();
            }
            else
            {
                app::g_devices.adc1->trigger_inject();
            }
        }
    }

}  // namespace valle