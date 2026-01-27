#include "Valle/System/hardware.hpp"

namespace valle
{

    template <>
    struct ADCIsrRouter<app::CurrentSensorT::ChannelT::skControllerID, ADCInterruptType::kEndOfInjectSequence>
    {
        static_assert(app::CurrentSensorT::ChannelT::skGroup == ADCChannelGroup::kInject,
                      "ADCIsrRouter specialization used for non-inject channel");

        static void handle()
        {
            // First read the current sensor and convert
            app::g_drivers.current_sensor.on_data_available();

            // Then run VCA control loop
            app::g_drivers.vca.run_ctrl_loop();
        }
    };

}  // namespace valle