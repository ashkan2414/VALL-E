#include "valle/base/system_build/build.hpp"

namespace valle::platform
{
    static void on_data_available()
    {
        // First read the current sensor and convert
        valle::app::g_drivers.test_adc.on_data_available();

        VALLE_LOG_INFO("ADC Voltage: {} V", valle::app::g_drivers.test_adc.read());
    }

    template <>
    struct ADCISRRouter<valle::app::TestADCChannelT::skControllerID, ADCInterruptType::kInjectEndOfSequence>
    {
        static void handle()
        {
            on_data_available();
        }
    };

    template <>
    struct DMAISRRouter<valle::app::TestADCDMAChannelT::skControllerID,
                        valle::app::TestADCDMAChannelT::skChannelID,
                        DMAChannelInterruptType::kTransferComplete>
    {
        static void handle()
        {
            on_data_available();
        }
    };

}  // namespace valle::platform

// Bind UART Logger ISR
VALLE_BIND_UART_LOGGER_ISR_ROUTERS(valle::app::g_drivers.uart_logger);
