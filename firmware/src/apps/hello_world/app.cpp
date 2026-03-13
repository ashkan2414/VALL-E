#include "app.hpp"

#include "valle/app/system_config.hpp"
#include "valle/base/panic.hpp"

VALLE_DEFINE_UART_LOGGER_HANDLER(app::g_drivers.uart_logger);

namespace valle::app
{
    DriverBuilderReturnT install_drivers(DriverBuilderT&& builder)
    {
        return std::move(builder)
            .template install<RootDriver>()
            .template install<CoreSystemDriver>()
            .template install<UARTLoggerT>()
            .yield();
    }

    /**
     * @brief Initialize shared devices.
     *
     */
    static void init_root()
    {
        g_drivers.root.foreach (DeviceInitOverloaded{
            [](CoreSystemDriver& dev) { (void)dev; },
            [](DMAMux1ControllerDevice& dev) { expect(dev.init(), "Failed to initialize DMAMux1 Controller Device"); },
            [](DMA1ControllerDevice& dev) { expect(dev.init(), "Failed to initialize DMA1 Controller Device"); },
            [](GPIOPortADevice& dev) { expect(dev.init(), "Failed to initialize GPIO Port A Device"); },
        }  // namespace valle
        );
    }

    /**
     * @brief Initialize drivers.
     *
     */
    static void init_drivers()
    {
        expect(g_drivers.uart_logger.init(UARTControllerConfig{
                   .baud_rate         = UARTBaudRate::kBaud230400,
                   .word_length       = UARTWordLength::kBits8,
                   .stop_bits         = UARTStopBits::kBits1,
                   .parity            = UARTParity::kNone,
                   .transfer_mode     = UARTTransferMode::kTxRx,
                   .hw_flow_ctrl      = UARTHardwareFlowControl::kNone,
                   .dma_priority      = DMAPriority::kHigh,
                   .dma_int_priority  = 5,
                   .uart_int_priority = 5,
               }),
               "Failed to initialize UART Logger Driver");
    }

    /**
     * @brief Post-initialize shared devices.
     *
     */
    static void post_init_root()
    {
        g_drivers.root.foreach_reverse(DeviceInitOverloaded{
            [](CoreSystemDriver& dev) { (void)dev; },
            [](DMAMux1ControllerDevice& dev)
            { expect(dev.post_init(), "Failed to post-initialize DMAMux1 Controller Device"); },
            [](DMA1ControllerDevice& dev)
            { expect(dev.post_init(), "Failed to post-initialize DMA1 Controller Device"); },
            [](GPIOPortADevice& dev) { expect(dev.post_init(), "Failed to post-initialize GPIO Port A Device"); },
        }  // namespace valle::app
        );
    }

    /**
     * @brief Initialize the system hardware.
     *
     */
    void init()
    {
        // Core System Device must be initialized first since other devices may depend on it for clock configuration
        expect(g_drivers.core.init(kDefaultCoreSystemConfig), "Failed to initialize Core System Device");

        init_root();
        init_drivers();
        post_init_root();
    }

}  // namespace valle::app