#include <random>

#include "app.hpp"
#include "valle/app_bridge.hpp"
#include "valle/system/timing.hpp"

namespace valle
{
    void app::main()
    {
        TimingContext::delay_ms(1000);

        app::init();

        if constexpr (app::kTestADCUseInject)  // NOLINT(bugprone-branch-clone)
        {
            app::g_drivers.root.adc1().enable_interrupts(ADCInterruptConfig{
                .priority = 5,
                .interrupts =
                    ADCInterruptMask{
                        .inj_eos = true,
                    },
            });
            app::g_drivers.root.adc1().start_inject();
        }
        else
        {
            app::g_drivers.root.adc1().start_regular();
        }

        while (true)
        {
            TimingContext::delay_ms(100);

            // Software trigger for Regular group
            if constexpr (!app::kTestADCUseInject)  // NOLINT(bugprone-branch-clone)
            {
                app::g_drivers.root.adc1().trigger_regular();
            }
            else
            {
                app::g_drivers.root.adc1().trigger_inject();
            }
        }
    }

}  // namespace valle