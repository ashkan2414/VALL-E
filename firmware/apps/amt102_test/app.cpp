#include "app.hpp"

#include "valle/app/system_config.hpp"
#include "valle/error.hpp"

VALLE_DEFINE_UART_LOGGER_HANDLER(app::g_drivers.uart_logger);

namespace valle::app
{
    DriverBuilderReturnT install_drivers(DriverBuilderT&& builder)
    {
        return std::move(builder)
            .template install<RootDriver>()
            .template install<CoreSystemDriver>()
            .template install<UARTLoggerT>()
            .template install<AMT102ModuleT>()
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

        expect(g_drivers.amt102.init(AMT102ModuleConfigT{
                   .encoder_config =
                       TIMQuadEncoderConfig{
                           .gpio_config =
                               GPIOAlternativeFunctionConfig{
                                   .mode  = GPIOAlternateFunctionMode::kPushPull,
                                   .speed = GPIOSpeedMode::kMedium,
                                   .pull  = GPIOPullMode::kNoPull,
                               },
                           .encoder_config =
                               TIMControllerEncoderConfig{
                                   .mode = TIMControllerEncoderMode::kX4TimerInput12,
                                   .ch_config =
                                       TIMChannelInputCaptureConfig{
                                           .active_input = TIMChannelInputCaptureActiveInput::kDirectTimerInput,
                                           .prescaler    = TIMChannelInputCapturePrescaler::kDiv1,
                                           .filter       = TIMChannelInputCaptureFilter::kFreqDiv2N8Samples,
                                           .polarity     = TIMChannelInputCapturePolarity::kRising,
                                       },
                                   .ch2_polarity = TIMChannelInputCapturePolarity::kRising,
                                   .auto_reload  = (static_cast<uint32_t>(kEncoderPPR) * 4) - 1,
                               }}}),
               "Failed to initialize AMT102 module");
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
        });
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