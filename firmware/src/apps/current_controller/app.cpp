#include "app.hpp"

#include "valle/app/platform/core_system_config.hpp"
#include "valle/base/panic.hpp"
#include "valle/system/timing.hpp"

VALLE_DEFINE_UART_LOGGER_HANDLER(valle::app::g_drivers.uart_logger);

namespace valle::app
{
    CurrentResponseCollectorT g_current_response_collector;

    DriverBuilderReturnT install_drivers(DriverBuilderT&& builder)
    {
        return std::move(builder)
            .template install<RootDriver>()
            .template install<platform::CoreSystemDriver>()
            .template install<UARTLoggerT>()
            .template install<VCACurrentLoopDriverT>()
            .template install<TestGPIODriverT>()
            .yield();
    }

    /**
     * @brief Initialize shared devices.
     *
     */
    static void init_root()
    {
        // Some devices not intialized here are handled by the current loop driver.
        // TODO: Implement a "owned shared device" concept to allow drivers to allow a clear owner.
        g_drivers.root.foreach (DeviceInitOverloaded{
            [](platform::CoreSystemDriver& dev) { (void)dev; },
            [](platform::GPIOPortADevice& dev) { expect(dev.init(), "Failed to initialize GPIO Port A Device"); },
            [](platform::GPIOPortBDevice& dev) { expect(dev.init(), "Failed to initialize GPIO Port B Device"); },
            [](platform::HRTIM1ControllerDevice& dev) { (void)dev; },  // Initialized by VCA Current Loop Driver
            [](platform::DMAMux1ControllerDevice& dev)
            { expect(dev.init(), "Failed to initialize DMAMux1 Controller Device"); },
            [](platform::DMA1ControllerDevice& dev)
            { expect(dev.init(), "Failed to initialize DMA1 Controller Device"); },
            [](platform::ADC12CommonDevice& dev) { (void)dev; },     // Initialized by VCA Current Loop Driver
            [](platform::ADC1ControllerDevice& dev) { (void)dev; },  // Initialized by VCA Current Loop Driver
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

        constexpr auto vca_current_loop_driver_config =
            VCACurrentLoopDriverConfigT{
                .pwm_freq_hz          = kVCAPWMFreqHz,
                .max_current_amp      = 0.3F,
                .target_tolerance_amp = 0.001F,
                .interrupt_priority   = 5,
                .current_sensor_calibration =
                    LinearConverterConfig{
                        .scale  = 1.0F,
                        .offset = 0.0F,
                    },
            }
                .to_raw();
        static_assert(
            !vca_current_loop_driver_config.validate(platform::app::kDefaultCoreSystemConfig.rcc_config).has_value(),
            "VCA Current Loop Driver configuration is invalid");

        expect(g_drivers.vca_current_loop_driver.init(vca_current_loop_driver_config),
               "Failed to initialize VCA Current Loop Driver");

        expect(g_drivers.test_gpio.init(platform::GPIODigitalOutConfig{
                   .mode  = platform::GPIOOutputMode::kPushPull,
                   .speed = platform::GPIOSpeedMode::kLow,
                   .pull  = platform::GPIOPullMode::kNoPull,
               }),
               "Failed to initialize Test GPIO Driver");
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