#include "app.hpp"

#include "valle/app/engine_kinematics_config.hpp"
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
            .template install<UartLoggerT>()
            .template install<VCACurrentLoopDriverT>()
            .template install<CrankEncoderModuleT>()
            .template install<TestGpioDriverT>()
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
            [](platform::GpioPortADevice& dev) { expect(dev.init(), "Failed to initialize GPIO Port A Device"); },
            [](platform::GpioPortBDevice& dev) { expect(dev.init(), "Failed to initialize GPIO Port B Device"); },
            [](platform::Hrtim1ControllerDevice& dev) { (void)dev; },  // Initialized by VCA Current Loop Driver
            [](platform::DmaMux1ControllerDevice& dev)
            { expect(dev.init(), "Failed to initialize DmaMux1 Controller Device"); },
            [](platform::Dma1ControllerDevice& dev)
            { expect(dev.init(), "Failed to initialize Dma1 Controller Device"); },
            [](platform::Adc12CommonDevice& dev)
            {
                expect(dev.init(platform::AdcCommonConfig{
                           .clock_config =
                               platform::AdcCommonAsyncClockConfig{.prescaler =
                                                                       platform::AdcCommonAsyncClockPrescaler::kDiv4}}),
                       "Failed to initialize Adc12 Common Device");
            },
            [](platform::Adc1ControllerDevice& dev) { (void)dev; },  // Initialized by VCA Current Loop Driver
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

        constexpr auto vca_current_loop_driver_config =
            platform::app::kDefaultVcaCurrentLoopDriverConfig<kVcaCurrentLoopDriverId>.to_raw();
        static_assert(
            !vca_current_loop_driver_config.validate(platform::app::kDefaultCoreSystemConfig.rcc_config).has_value(),
            "VCA Current Loop Driver configuration is invalid");

        expect(g_drivers.vca_current_loop_driver.init(vca_current_loop_driver_config),
               "Failed to initialize VCA Current Loop Driver");

        expect(g_drivers.crank_encoder.init(
                   CrankEncoderModuleConfigT{.engine_kinematics = kPowerfistEngineKinematicsConfig,
                                             .home_rad          = 0.73247560718F,
                                             .reverse_direction = false}),
               "Failed to initialize AMT102 Crank Encoder module");

        expect(g_drivers.test_gpio.init(platform::GpioDigitalOutConfig{
                   .mode  = platform::GpioOutputMode::kPushPull,
                   .speed = platform::GpioSpeedMode::kLow,
                   .pull  = platform::GpioPullMode::kNoPull,
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
