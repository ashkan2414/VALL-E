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
            [](platform::CoreSystemDriver& dev) { (void)dev; },
            [](platform::GPIOPortADevice& dev) { expect(dev.init(), "Failed to initialize GPIO Port A Device"); },
            [](platform::HRTIM1ControllerDevice& dev)
            { expect(dev.init(), "Failed to initialize HRTIM1 Controller Device"); },
            [](platform::DMAMux1ControllerDevice& dev)
            { expect(dev.init(), "Failed to initialize DMAMux1 Controller Device"); },

            [](platform::DMA1ControllerDevice& dev)
            { expect(dev.init(), "Failed to initialize DMA1 Controller Device"); },
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

        expect(g_drivers.hb_driver.init(platform::HRTIMHalfBridgeDriverConfig{
                   .freq_hz          = 60000,  // 60 kHz PWM Frequency
                   .repetition       = 1,
                   .rollover_mode    = platform::HRTIMTimerRolloverMode::kPeriodReset,
                   .interrupt_config = platform::HRTIMTimerInterruptConfig{.priority = 5,
                                                                           .interrupts =
                                                                               platform::HRTIMTimerInterruptMask{
                                                                                   .repetition = true,
                                                                               }},
                   .output_config =
                       {
                           .polarity    = platform::HRTIMTimerOutputPolarity::kPositive,
                           .idle_mode   = platform::HRTIMTimerOutputIdleMode::kNoIdle,
                           .idle_level  = platform::HRTIMTimerOutputIdleLevel::kInactive,
                           .fault_state = platform::HRTIMTimerOutputFaultState::kInactive,
                           .gpio_config =
                               platform::HRTIMTimerOutputGPIOConfig{
                                   .speed = platform::GPIOSpeedMode::kLow,
                                   .pull  = platform::GPIOPullMode::kNoPull,
                               },
                       },
                   .deadtime_config = platform::HRTIMTimerDeadTimeConfig{.rise_ns = 200.0F, .fall_ns = 200.0F},
                   .min_duty        = 0.0F,
                   .max_duty        = 0.95F,
                   .center_aligned  = true,
                   .compare_unit    = platform::HRTIMTimerCompareUnit::kCompare1,
               }),
               "Failed to initialize HRTIM Half Bridge Driver");
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