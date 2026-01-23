#include "system_hardware.hpp"

using namespace valle;

template <>
struct ADCIsrRouter<CurrentSensorT::ChannelT::skADCIdx, ADCInterruptType::kEndOfInjectSequence>
{
    static_assert(CurrentSensorT::ChannelT::skGroup == ADCChannelGroup::kInject,
                  "ADCIsrRouter specialization used for non-inject channel");

    static void handle()
    {
        // First read the current sensor and convert
        system::g_current_sensor.on_data_available();

        // Then run VCA control loop
        system::g_vca.run_ctrl_loop();
    }
};
