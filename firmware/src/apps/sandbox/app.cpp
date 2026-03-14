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
            .template install<VCAControllerT>()
            .template install<CurrentSensorT>()
            .template install<PositionSensorT>()
            .template install<TestGPIODriverT>()
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
            [](platform::GPIOPortBDevice& dev) { expect(dev.init(), "Failed to initialize GPIO Port B Device"); },
            [](platform::HRTIM1ControllerDevice& dev)
            { expect(dev.init(), "Failed to initialize HRTIM1 Controller Device"); },
            [](platform::DMAMux1ControllerDevice& dev)
            { expect(dev.init(), "Failed to initialize DMAMux1 Controller Device"); },
            [](platform::DMA1ControllerDevice& dev)
            { expect(dev.init(), "Failed to initialize DMA1 Controller Device"); },
            [](platform::ADC12CommonDevice& dev)
            { expect(dev.init(kCurrentSensorADCCommonConfig), "Failed to initialize ADC 12 Common Device"); },
            [](platform::ADC1ControllerDevice& dev)
            { expect(dev.init(), "Failed to initialize ADC1 Controller Device"); },
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

        init_vca_controller(g_drivers.vca_controller);
        init_current_sensor(g_drivers.current_sensor);

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

        expect(app::g_drivers.root.adc1().post_init(kCurrentSensorADCConfig),
               "Failed to post-initialize ADC 1 Controller Device");

        if constexpr (kCurrentSensorUseInject)
        {
            app::g_drivers.root.adc1().enable_interrupts(platform::ADCInterruptConfig{
                .priority = 5,
                .interrupts =
                    platform::ADCInterruptMask{
                        .inj_eos = true,
                    },
            });
        }
    }

    void start_vca_controller()
    {
        if constexpr (kCurrentSensorUseInject)
        {
            app::g_drivers.root.adc1().start_inject();
        }
        else
        {
            app::g_drivers.root.adc1().start_regular();
        }
        app::g_drivers.vca_controller.enable();
    }

    void stop_vca_controller()
    {
        app::g_drivers.vca_controller.disable();
    }

}  // namespace valle::app