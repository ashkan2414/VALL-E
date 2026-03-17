#include "app.hpp"

#include "valle/app/engine_kinematics_config.hpp"
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
            .template install<VCACurrentLoopDriverT>()
            .template install<PositionSensorT>()
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
            [](platform::ADC12CommonDevice& dev) { (void)dev; },     // Initialized by vca current loop driver
            [](platform::ADC1ControllerDevice& dev) { (void)dev; },  // Initialized by vca current loop driver
            [](platform::I2C1CommandBufferDevice<>& dev)
            {
                expect(dev.init(platform::I2CCommandBufferDeviceConfig{
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

        constexpr auto vca_current_loop_driver_config = platform::app::kDefaultVCACurrentLoopDriverConfig<>.to_raw();
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

        expect(g_drivers.position_sensor.init(PositionSensorModuleConfigT{
                   .i2c_config{},
                   .sensor_config =
                       PositionSensorConfigT{
                           .clock_source           = LDC161XClockSourceExternalClock{.fclk_mhz = 40.0f},
                           .sample_rate_hz         = 1000,
                           .deglitch_bandwidth     = LDC161XDeglitchBandwidth::kBand10MHz,
                           .interrupt_config       = std::nullopt,
                           .sensor_activation_mode = LDC161XSensorActivationMode::kFullCurrentMode,
                           .enable_rp_override     = true,
                           .auto_amplitude_en      = false,
                           .high_current_drive_en  = true,
                           .channels               = {LDC161XChannelConfig{
                                             .coil_config =
                                   LDC161XCoilConfig{
                                                     .inductance_uh  = 18.147f,
                                                     .capacitance_pf = 100.0f,
                                                     .rp_kohm        = 15.727f,
                                                     .q_factor       = 35.97f,
                                   },
                                             .drive_current = LDC161XIDriveCurrent::from_coil_rp(15.727f, 1.5f),
                                             .offset_config = LDC161XOffsetConfigFrequency{.offset_mhz = 0.0f},
                           }},
                       }}),
               "Failed to initialize Position Sensor");

        expect(g_drivers.crank_encoder.init(
                   CrankEncoderModuleConfigT{.engine_kinematics = kPowerfistEngineKinematicsConfig,
                                             .home_rad          = 0.73247560718F,
                                             .reverse_direction = false}),
               "Failed to initialize AMT102 Crank Encoder module");
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