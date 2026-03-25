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
            .template install<UartLoggerT>()
            .template install<HrtimHalfBridgeDriverT>()
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
            [](platform::GpioPortADevice& dev) { expect(dev.init(), "Failed to initialize GPIO Port A Device"); },
            [](platform::Hrtim1ControllerDevice& dev)
            { expect(dev.init(), "Failed to initialize Hrtim1 Controller Device"); },
            [](platform::DmaMux1ControllerDevice& dev)
            { expect(dev.init(), "Failed to initialize DmaMux1 Controller Device"); },

            [](platform::Dma1ControllerDevice& dev)
            { expect(dev.init(), "Failed to initialize Dma1 Controller Device"); },
        }  // namespace valle
        );
    }

    /**
     * @brief Initialize drivers.
     *
     */
    static void init_drivers()
    {
        expect(g_drivers.uart_logger.init(platform::UartControllerConfig{
                   .baud_rate         = platform::UartBaudRate::kBaud230400,
                   .word_length       = platform::UartWordLength::kBits8,
                   .stop_bits         = platform::UartStopBits::kBits1,
                   .parity            = platform::UartParity::kNone,
                   .transfer_mode     = platform::UartTransferMode::kTxRx,
                   .hw_flow_ctrl      = platform::UartHardwareFlowControl::kNone,
                   .dma_priority      = platform::DmaPriority::kHigh,
                   .dma_int_priority  = 5,
                   .uart_int_priority = 5,
               }),
               "Failed to initialize UART Logger Driver");

        expect(g_drivers.hb_driver.init(platform::HrtimHalfBridgeDriverConfig{
                   .freq_hz          = 60000,  // 60 kHz PWM Frequency
                   .repetition       = 1,
                   .rollover_mode    = platform::HrtimTimerRolloverMode::kPeriodReset,
                   .interrupt_config = platform::HrtimTimerInterruptConfig{.priority = 5,
                                                                           .interrupts =
                                                                               platform::HrtimTimerInterruptMask{
                                                                                   .repetition = true,
                                                                               }},
                   .output_config =
                       {
                           .polarity    = platform::HrtimTimerOutputPolarity::kPositive,
                           .idle_mode   = platform::HrtimTimerOutputIdleMode::kNoIdle,
                           .idle_level  = platform::HrtimTimerOutputIdleLevel::kInactive,
                           .fault_state = platform::HrtimTimerOutputFaultState::kInactive,
                           .gpio_config =
                               platform::HrtimTimerOutputGpioConfig{
                                   .speed = platform::GpioSpeedMode::kLow,
                                   .pull  = platform::GpioPullMode::kNoPull,
                               },
                       },
                   .deadtime_config = platform::HrtimTimerDeadTimeConfig{.rise_ns = 200.0F, .fall_ns = 200.0F},
                   .min_duty        = 0.0F,
                   .max_duty        = 0.95F,
                   .center_aligned  = true,
                   .compare_unit    = platform::HrtimTimerCompareUnit::kCompare1,
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