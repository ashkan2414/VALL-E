#include "app.hpp"

#include "valle/app/engine_kinematics_config.hpp"
#include "valle/app/platform/core_system_config.hpp"
#include "valle/app/platform/grove_ldc161x_config.hpp"
#include "valle/base/panic.hpp"
#include "valle/system/timing.hpp"

VALLE_DEFINE_UART_LOGGER_HANDLER(valle::app::g_drivers.uart_logger);

namespace valle::app
{
    PositionResponseCollectorT g_position_response_collector;

    DriverBuilderReturnT install_drivers(DriverBuilderT&& builder)
    {
        return std::move(builder)
            .template install<RootDriver>()
            .template install<platform::CoreSystemDriver>()
            .template install<UARTLoggerT>()
            .template install<IntakeValveVCACurrentLoopDriverT>()
            .template install<ExhaustValveVCACurrentLoopDriverT>()
            .template install<PositionLoopDriverT>()
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
            [](platform::GPIOPortCDevice& dev) { expect(dev.init(), "Failed to initialize GPIO Port C Device"); },
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
                           .event_int_priority = 5,
                           .error_int_priority = 5,
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
        expect(
            g_drivers.position_loop_driver.init(PositionLoopDriverConfigT{
                .ldc161x_config = platform::app::get_grove_ldc161x_config<PositionSensorModuleT::skNumChannels>(
                    kPositionSampleRateHz, 5),
                .converter_configs = {PositionSensorConverterConfigT{// Exhaust
                                                                     .table = {LookupTablePoint{3.542181F, 0.0F},
                                                                               LookupTablePoint{3.50422190083F, 0.5F},
                                                                               LookupTablePoint{3.461042F, 1.19F},
                                                                               LookupTablePoint{3.372453F, 3.75F},
                                                                               LookupTablePoint{3.366891F, 4.16F},
                                                                               LookupTablePoint{3.360423F, 4.57F},
                                                                               LookupTablePoint{3.357199F, 4.97F},
                                                                               LookupTablePoint{3.352718F, 5.36F},
                                                                               LookupTablePoint{3.350428F, 5.77F}}},
                                      PositionSensorConverterConfigT{// Intake
                                                                     .table = {LookupTablePoint{3.484316F, 0.0F},
                                                                               LookupTablePoint{3.45249F, 0.5F},
                                                                               LookupTablePoint{3.407546F, 1.19F},
                                                                               LookupTablePoint{3.335775F, 3.75F},
                                                                               LookupTablePoint{3.329356F, 4.16F},
                                                                               LookupTablePoint{3.326385F, 4.57F},
                                                                               LookupTablePoint{3.323087F, 4.97F},
                                                                               LookupTablePoint{3.319487F, 5.36F},
                                                                               LookupTablePoint{3.315132F, 5.77F}}}},
                .controller_interface_configs =
                    {
                        ExhaustValvePositionVCAControllerInterfaceConfigT{
                            .vca_controller = g_drivers.exhaust_vca_current_loop_driver,
                            .controller_config =
                                platform::app::LDC161XVCAValvePositionClosedLoopControllerConfig<float>{
                                    .sample_time         = position_sensor_sample_period,
                                    .max_current_amp     = exhaust_vca_current_loop_driver_config.max_current_amp,
                                    .target_tolerance_mm = 0.5f,
                                    .min_position_mm =
                                        std::min(kExhaustValveOpenPositionMM, kExhaustValveClosePositionMM),
                                    .max_position_mm =
                                        std::max(kExhaustValveOpenPositionMM, kExhaustValveClosePositionMM),
                                },
                        },
                        IntakeValvePositionVCAControllerInterfaceConfigT{
                            .vca_controller = g_drivers.intake_vca_current_loop_driver,
                            .controller_config =
                                platform::app::LDC161XVCAValvePositionClosedLoopControllerConfig<float>{
                                    .sample_time         = position_sensor_sample_period,
                                    .max_current_amp     = intake_vca_current_loop_driver_config.max_current_amp,
                                    .target_tolerance_mm = 0.5f,
                                    .min_position_mm =
                                        std::min(kIntakeValveOpenPositionMM, kIntakeValveClosePositionMM),
                                    .max_position_mm =
                                        std::max(kIntakeValveOpenPositionMM, kIntakeValveClosePositionMM),
                                },
                        },
                    }}),
            "Failed to initialize Position Controller Driver");

        g_drivers.position_loop_driver.set_position_feedback_callback(PositionLoopDriverT::PositionFeedbackCallbackT(
            [](const std::span<const float, 2>& position_feedback_mm)
            {
                g_position_response_collector.push_data(PositionResponseData{
                    .timestamp = system::MillisClock::now(),
                    .target_position =
                        g_drivers.position_loop_driver.get_target_position_mm<kResponseTargetValveChannel>(),
                    .measured_position =
                        position_feedback_mm[LDC161XTraits::skChannelIndexFromChannel<kResponseTargetValveChannel>],
                });
            }));

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
    }

    void stop()
    {
        g_drivers.position_loop_driver.stop();
        g_drivers.intake_vca_current_loop_driver.stop();
        g_drivers.exhaust_vca_current_loop_driver.stop();
    }

}  // namespace valle::app
