#include "app.hpp"

#include "valle/app/platform/core_system_config.hpp"
#include "valle/base/panic.hpp"

VALLE_DEFINE_UART_LOGGER_HANDLER(valle::app::g_drivers.uart_logger);

namespace valle::app
{
    system::SynchronizedCriticalSection<std::optional<PositionSensorAsyncReadResultT>> g_position_sensor_read_result;

    DriverBuilderReturnT install_drivers(DriverBuilderT&& builder)
    {
        return std::move(builder)
            .template install<RootDriver>()
            .template install<platform::CoreSystemDriver>()
            .template install<UARTLoggerT>()
            .template install<PositionSensorT>()
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
            [](platform::GPIOPortBDevice& dev) { expect(dev.init(), "Failed to initialize GPIO Port B Device"); },
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

        constexpr auto grove_coil_config = LDC161XCoilConfig{
            .inductance_uh  = 18.147F,
            .capacitance_pf = 100.0F,
            .rp_kohm        = 15.727F,
            .q_factor       = 35.97F,
        };
        constexpr auto channel_config = LDC161XChannelConfig{
            .coil_config   = grove_coil_config,
            .drive_current = LDC161XIDriveCurrent::from_coil_rp(grove_coil_config.rp_kohm),
            .offset_config = LDC161XOffsetConfigFrequency{.offset_mhz = 2.5F},
        };

        expect(g_drivers.position_sensor.init(PositionSensorModuleConfigT{
                   .i2c_config =
                       valle::platform::app::LDC161XSensorModuleI2CInterfaceConfig{
                           .intb_interrupt_priority = 5,
                       },
                   .sensor_config =
                       PositionSensorConfigT{
                           .clock_source       = LDC161XClockSourceExternalClock{.fclk_mhz = 40.0F},
                           .sample_rate_hz     = 300,
                           .deglitch_bandwidth = LDC161XDeglitchBandwidth::kBand10MHz,
                           .interrupt_config =
                               LDC161XInterruptConfig{
                                   .int_on_data_ready = true,
                               },
                           .sensor_activation_mode = LDC161XSensorActivationMode::kFullCurrentMode,
                           .enable_rp_override     = true,
                           .auto_amplitude_en      = false,
                           // .high_current_drive_en = false,
                           .channels = {channel_config, channel_config},
                       }}),
               "Failed to initialize Position Sensor");

        g_drivers.position_sensor.set_async_read_callback([](const auto& result)
                                                          { position_sensor_async_read_callback(result); });
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

    void position_sensor_async_read_callback(const PositionSensorT::ReadCallbackResultT& result)
    {
        if (std::holds_alternative<PositionSensorT::ErrorCallbackResultT>(result))
        {
            VALLE_LOG_ERROR("Position sensor error callback invoked");
            return;
        }

        g_position_sensor_read_result.with_lock(
            [&](auto& read_result)
            {
                read_result = result;  // Store the result for synchronous access from the main loop
            });
    }

    void intb_asserted_isr()
    {
        (void)valle::app::g_drivers.position_sensor.read_status_data_raw_multi_async();
    }

}  // namespace valle::app