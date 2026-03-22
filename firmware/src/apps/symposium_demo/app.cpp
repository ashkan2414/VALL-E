#include "app.hpp"

#include "valle/app/engine_kinematics_config.hpp"
#include "valle/app/platform/core_system_config.hpp"
#include "valle/app/platform/grove_ldc161x_config.hpp"
#include "valle/base/panic.hpp"
#include "valle/system/timing.hpp"

VALLE_DEFINE_UART_LOGGER_HANDLER(valle::app::g_drivers.uart_logger);

namespace valle::app
{
    DriverBuilderReturnT install_drivers(DriverBuilderT&& builder)
    {
        return std::move(builder)
            .template install<RootDriver>()
            .template install<platform::CoreSystemDriver>()
            .template install<UARTLoggerT>()
            .template install<IntakeValveVCACurrentLoopDriverT>()
            .template install<ExhaustValveVCACurrentLoopDriverT>()
            .template install<PositionLoopDriverT>()
            .template install<CrankEncoderModuleT>()
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
            [](platform::ADC12CommonDevice& dev)
            {
                expect(dev.init(platform::ADCCommonConfig{
                           .clock_config =
                               platform::ADCCommonAsyncClockConfig{.prescaler =
                                                                       platform::ADCCommonAsyncClockPrescaler::kDiv4}}),
                       "Failed to initialize ADC12 Common Device");
            },
            [](platform::ADC1ControllerDevice& dev) { (void)dev; },  // Initialized by VCA Current Loop Driver
            [](platform::ADC2ControllerDevice& dev) { (void)dev; },  // Initialized by VCA Current Loop Driver
            [](platform::I2C1CommandBufferDevice<>& dev)
            {
                expect(dev.init(platform::I2CCommandBufferDeviceConfig{
                           .controller_config =
                               platform::I2CControllerConfig{
                                   .timing_reg = 0x40621236,  // 400kHz @ 170MHz APB1 clock (values from STM32CubeMX)
                                   .enable_analog_filter = true,
                                   .dma_priority         = platform::DMAPriority::kHigh,
                               },
                           .event_int_priority = 6,
                           .error_int_priority = 4,
                       }),
                       "Failed to initialize I2C1 Command Buffer Device");
            },
        }  // namespace valle
        );
    }

    /**
     * @brief Initialize drivers.
     *
     */
    static void init_drivers()
    {
        // UART Logger Driver Configuration
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

        // Intake VCA Current Loop Driver Configuration
        constexpr auto intake_vca_current_loop_driver_config =
            platform::app::kDefaultVCACurrentLoopDriverConfig<kIntakeValveVCACurrentLoopDriverID>;
        constexpr auto intake_vca_current_loop_driver_config_raw = intake_vca_current_loop_driver_config.to_raw();
        static_assert(
            !intake_vca_current_loop_driver_config_raw.validate(platform::app::kDefaultCoreSystemConfig.rcc_config)
                 .has_value(),
            "VCA Current Loop Driver configuration is invalid");

        expect(g_drivers.intake_vca_current_loop_driver.init(intake_vca_current_loop_driver_config_raw),
               "Failed to initialize VCA Current Loop Driver");

        // Exhaust VCA Current Loop Driver Configuration
        constexpr auto exhaust_vca_current_loop_driver_config =
            platform::app::kDefaultVCACurrentLoopDriverConfig<kExhaustValveVCACurrentLoopDriverID>;
        constexpr auto exhaust_vca_current_loop_driver_config_raw = exhaust_vca_current_loop_driver_config.to_raw();
        static_assert(
            !exhaust_vca_current_loop_driver_config_raw.validate(platform::app::kDefaultCoreSystemConfig.rcc_config)
                 .has_value(),
            "VCA Current Loop Driver configuration is invalid");
        expect(g_drivers.exhaust_vca_current_loop_driver.init(exhaust_vca_current_loop_driver_config_raw),
               "Failed to initialize VCA Current Loop Driver");

        // Position Controller Configuration
        static constexpr auto position_sensor_sample_period = DurationSecondsF(1.0f / kPositionSampleRateHz);
        expect(g_drivers.position_loop_driver.init(
                   PositionLoopDriverConfigT{
                       .ldc161x_config = platform::app::get_grove_ldc161x_config<PositionSensorModuleT::skNumChannels>(
                           kPositionSampleRateHz, 6),
                       .converter_configs = {PositionSensorConverterConfigT{.table =
                                                                                {
                                                                                    LookupTablePoint{3.0f, 0.0f},
                                                                                    LookupTablePoint{3.1f, 0.5f},
                                                                                    LookupTablePoint{3.2f, 1.0f},
                                                                                    LookupTablePoint{3.3f, 1.5f},
                                                                                    LookupTablePoint{3.4f, 2.0f},
                                                                                    LookupTablePoint{3.5f, 2.5f},
                                                                                    LookupTablePoint{3.6f, 3.0f},
                                                                                    LookupTablePoint{3.7f, 3.5f},
                                                                                    LookupTablePoint{3.8f, 4.0f},
                                                                                    LookupTablePoint{3.9f, 4.5f},
                                                                                    LookupTablePoint{4.0f, 5.0f},
                                                                                    LookupTablePoint{4.1f, 5.5f},
                                                                                    LookupTablePoint{4.2f, 6.0f},
                                                                                    LookupTablePoint{4.3f, 6.5f},
                                                                                    LookupTablePoint{4.4f, 7.0f},
                                                                                }},
                                             PositionSensorConverterConfigT{.table =
                                                                                {
                                                                                    LookupTablePoint{3.0f, 0.0f},
                                                                                    LookupTablePoint{3.1f, 0.5f},
                                                                                    LookupTablePoint{3.2f, 1.0f},
                                                                                    LookupTablePoint{3.3f, 1.5f},
                                                                                    LookupTablePoint{3.4f, 2.0f},
                                                                                    LookupTablePoint{3.5f, 2.5f},
                                                                                    LookupTablePoint{3.6f, 3.0f},
                                                                                    LookupTablePoint{3.7f, 3.5f},
                                                                                    LookupTablePoint{3.8f, 4.0f},
                                                                                    LookupTablePoint{3.9f, 4.5f},
                                                                                    LookupTablePoint{4.0f, 5.0f},
                                                                                    LookupTablePoint{4.1f, 5.5f},
                                                                                    LookupTablePoint{4.2f, 6.0f},
                                                                                    LookupTablePoint{4.3f, 6.5f},
                                                                                    LookupTablePoint{4.4f, 7.0f},
                                                                                }}},
                       .controller_interface_configs =
                           {
                               IntakeValvePositionVCAControllerInterfaceConfigT{
                                   .vca_controller = g_drivers.intake_vca_current_loop_driver,
                                   .controller_config =
                                       platform::app::LDC161XVCAValvePositionClosedLoopControllerConfig<float>{
                                           .sample_time         = position_sensor_sample_period,
                                           .max_current_amp     = intake_vca_current_loop_driver_config.max_current_amp,
                                           .target_tolerance_mm = 0.01f,
                                           .min_position_mm =
                                               std::min(kIntakeValveOpenPositionMM, kIntakeValveClosePositionMM),
                                           .max_position_mm =
                                               std::max(kIntakeValveOpenPositionMM, kIntakeValveClosePositionMM),
                                       },
                               },
                               ExhaustValvePositionVCAControllerInterfaceConfigT{
                                   .vca_controller = g_drivers.exhaust_vca_current_loop_driver,
                                   .controller_config =
                                       platform::app::LDC161XVCAValvePositionClosedLoopControllerConfig<float>{
                                           .sample_time     = position_sensor_sample_period,
                                           .max_current_amp = exhaust_vca_current_loop_driver_config.max_current_amp,
                                           .target_tolerance_mm = 0.01f,
                                           .min_position_mm =
                                               std::min(kExhaustValveOpenPositionMM, kExhaustValveClosePositionMM),
                                           .max_position_mm =
                                               std::max(kExhaustValveOpenPositionMM, kExhaustValveClosePositionMM),
                                       },
                               },
                           }}),
               "Failed to initialize Position Controller Driver");

        // AMT10x Crank Encoder Module Configuration
        expect(g_drivers.crank_encoder.init(
                   CrankEncoderModuleConfigT{.engine_kinematics = kPowerfistEngineKinematicsConfig,
                                             .home_rad          = 0.73247560718F,
                                             .reverse_direction = false}),
               "Failed to initialize AMT102 Crank Encoder module");

        // Test GPIO Driver Configuration
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

    void start()
    {
        g_drivers.intake_vca_current_loop_driver.start();
        g_drivers.exhaust_vca_current_loop_driver.start();
        expect(g_drivers.position_loop_driver.start(), "Failed to start Position Loop Driver");

        if constexpr (!kSimulatedEncoder)
        {
            g_drivers.crank_encoder.enable();
        }
    }

    void stop()
    {
        if constexpr (!kSimulatedEncoder)
        {
            g_drivers.crank_encoder.disable();
        }

        g_drivers.position_loop_driver.stop();
        g_drivers.intake_vca_current_loop_driver.stop();
        g_drivers.exhaust_vca_current_loop_driver.stop();
    }

}  // namespace valle::app