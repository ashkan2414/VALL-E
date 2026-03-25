#include "app.hpp"

#include "valle/app/platform/core_system_config.hpp"
#include "valle/app/platform/grove_ldc161x_config.hpp"
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
            .template install<UartLoggerT>()
            .template install<PositionSensorModuleT>()
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
            [](platform::DmaMux1ControllerDevice& dev)
            { expect(dev.init(), "Failed to initialize DmaMux1 Controller Device"); },
            [](platform::Dma1ControllerDevice& dev)
            { expect(dev.init(), "Failed to initialize Dma1 Controller Device"); },
            [](platform::GpioPortADevice& dev) { expect(dev.init(), "Failed to initialize GPIO Port A Device"); },
            [](platform::GpioPortBDevice& dev) { expect(dev.init(), "Failed to initialize GPIO Port B Device"); },
            [](platform::GpioPortCDevice& dev) { expect(dev.init(), "Failed to initialize GPIO Port C Device"); },
            [](platform::I2c1CommandBufferDevice<>& dev)
            {
                expect(dev.init(platform::I2cCommandBufferDeviceConfig{
                           .controller_config =
                               platform::I2cControllerConfig{
                                   .timing_reg = 0x40621236,  // 400kHz @ 170MHz APB1 clock (values from STM32CubeMX)
                                   .enable_analog_filter = true,
                                   .dma_priority         = platform::DmaPriority::kHigh,
                               },
                           .event_int_priority = 5,
                           .error_int_priority = 5,
                       }),
                       "Failed to initialize I2c1 Command Buffer Device");
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

        expect(g_drivers.position_sensor.init(
                   platform::app::get_grove_ldc161x_config<PositionSensorModuleT::skNumChannels>(300, 5)),
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

    void position_sensor_async_read_callback(const PositionSensorModuleT::ReadCallbackResultT& result)
    {
        if (std::holds_alternative<PositionSensorModuleT::ErrorCallbackResultT>(result))
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
