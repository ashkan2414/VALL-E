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
            .template install<HRTIMHalfBridgeDriverT>()
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
            [](GPIOPortADevice& dev) { expect(dev.init(), "Failed to initialize GPIO Port A Device"); },
            [](HRTIM1ControllerDevice& dev) { expect(dev.init(), "Failed to initialize HRTIM1 Controller Device"); },
            [](DMAMux1ControllerDevice& dev) { expect(dev.init(), "Failed to initialize DMAMux1 Controller Device"); },

            [](DMA1ControllerDevice& dev) { expect(dev.init(), "Failed to initialize DMA1 Controller Device"); },
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

        expect(g_drivers.hb_driver.init(HRTIMHalfBridgeDriverConfig{
                   .freq_hz          = 60000,  // 60 kHz PWM Frequency
                   .repetition       = 1,
                   .rollover_mode    = HRTIMTimerRolloverMode::kPeriodReset,
                   .interrupt_config = HRTIMTimerInterruptConfig{.priority = 5,
                                                                 .interrupts =
                                                                     HRTIMTimerInterruptMask{
                                                                         .repetition = true,
                                                                     }},
                   .output_config =
                       {
                           .polarity    = HRTIMTimerOutputPolarity::kPositive,
                           .idle_mode   = HRTIMTimerOutputIdleMode::kNoIdle,
                           .idle_level  = HRTIMTimerOutputIdleLevel::kInactive,
                           .fault_state = HRTIMTimerOutputFaultState::kInactive,
                           .gpio_config =
                               HRTIMTimerOutputGPIOConfig{
                                   .speed = GPIOSpeedMode::kLow,
                                   .pull  = GPIOPullMode::kNoPull,
                               },
                       },
                   .deadtime_config = HRTIMTimerDeadTimeConfig{.rise_ns = 200.0F, .fall_ns = 200.0F},
                   .min_duty        = 0.0F,
                   .max_duty        = 0.95F,
                   .center_aligned  = true,
                   .compare_unit    = HRTIMTimerCompareUnit::kCompare1,
               }),
               "Failed to initialize HRTIM Half Bridge Driver");
    }

    /**
     * @brief Post-initialize shared devices.
     *
     */
    static void post_init_root()
    {
        g_drivers.root.foreach_reverse(DeviceInitOverloaded{
            [](CoreSystemDriver& dev) { (void)dev; },
            [](GPIOPortADevice& dev) { expect(dev.post_init(), "Failed to post-initialize GPIO Port A Device"); },
            [](HRTIM1ControllerDevice& dev)
            { expect(dev.post_init(), "Failed to post-initialize HRTIM1 Controller Device"); },
            [](DMAMux1ControllerDevice& dev)
            { expect(dev.post_init(), "Failed to post-initialize DMAMux1 Controller Device"); },

            [](DMA1ControllerDevice& dev)
            { expect(dev.post_init(), "Failed to post-initialize DMA1 Controller Device"); },
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