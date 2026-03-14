#include "app.hpp"

#include "valle/app/platform/core_system_config.hpp"
#include "valle/base/panic.hpp"

VALLE_DEFINE_UART_LOGGER_HANDLER(valle::app::g_drivers.uart_logger);

namespace valle::app
{
    DriverBuilderReturnT install_drivers(DriverBuilderT&& builder)
    {
        return std::move(builder)
            .template install<RootDriver>()
            .template install<platform::CoreSystemDriver>()
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
            [](platform::CoreSystemDriver& dev) { (void)dev; },
            [](platform::DMAMux1ControllerDevice& dev)
            { expect(dev.init(), "Failed to initialize DMAMux1 Controller Device"); },
            [](platform::DMA1ControllerDevice& dev)
            { expect(dev.init(), "Failed to initialize DMA1 Controller Device"); },
            [](platform::GPIOPortADevice& dev) { expect(dev.init(), "Failed to initialize GPIO Port A Device"); },
        }  // namespace valle
        );
    }

    /**
     * @brief Initialize drivers.
     *
     */
    static void init_drivers()
    {
        expect(g_drivers.uart_logger.init(platform::UARTControllerConfig{
                   .baud_rate         = platform::UARTBaudRate::kBaud230400,
                   .word_length       = platform::UARTWordLength::kBits8,
                   .stop_bits         = platform::UARTStopBits::kBits1,
                   .parity            = platform::UARTParity::kNone,
                   .transfer_mode     = platform::UARTTransferMode::kTxRx,
                   .hw_flow_ctrl      = platform::UARTHardwareFlowControl::kNone,
                   .dma_priority      = platform::DMAPriority::kHigh,
                   .dma_int_priority  = 5,
                   .uart_int_priority = 5,
               }),
               "Failed to initialize UART Logger Driver");

        expect(g_drivers.amt102.init(
                   AMT102ModuleConfigT{
                       .encoder_config =
                           platform::TIMQuadEncoderConfig{
                               .gpio_config =
                                   platform::GPIOAlternativeFunctionConfig{
                                       .mode  = platform::GPIOAlternateFunctionMode::kPushPull,
                                       .speed = platform::GPIOSpeedMode::kMedium,
                                       .pull  = platform::GPIOPullMode::kNoPull,
                                   },
                               .encoder_config =
                                   platform::TIMControllerEncoderConfig{
                                       .mode = platform::TIMControllerEncoderMode::kX4TimerInput12,
                                       .ch_config =
                                           platform::TIMChannelInputCaptureConfig{
                                               .active_input =
                                                   platform::TIMChannelInputCaptureActiveInput::kDirectTimerInput,
                                               .prescaler = platform::TIMChannelInputCapturePrescaler::kDiv1,
                                               .filter    = platform::TIMChannelInputCaptureFilter::kFreqDiv2N8Samples,
                                               .polarity  = platform::TIMChannelInputCapturePolarity::kRising,
                                           },
                                       .ch2_polarity = platform::TIMChannelInputCapturePolarity::kRising,
                                       .auto_reload  = (static_cast<uint32_t>(kEncoderPPR) * 4) - 1,
                                   }}}),
               "Failed to initialize AMT102 module");
    }

    /**
     * @brief Initialize the system hardware.
     *
     */
    void init()
    {
        // Core System Device must be initialized first since other devices may depend on it for clock configuration
        expect(g_drivers.core.init(platform::app::kDefaultCoreSystemConfig), "Failed to initialize Core System Device");

        init_root();
        init_drivers();
    }

}  // namespace valle::app