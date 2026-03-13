#include "app.hpp"

#include "valle/app/system_config.hpp"
#include "valle/base/panic.hpp"
#include "valle/system/timing.hpp"

VALLE_DEFINE_UART_LOGGER_HANDLER(app::g_drivers.uart_logger);

namespace valle::app
{
    CurrentResponseCollectorT g_current_response_collector;

    DriverBuilderReturnT install_drivers(DriverBuilderT&& builder)
    {
        return std::move(builder)
            .template install<RootDriver>()
            .template install<platform::CoreSystemDriver>()
            .template install<UARTLoggerT>()
            .template install<VCAControllerT>()
            .template install<CurrentSensorT>()
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
    }

    /**
     * @brief Post-initialize shared devices.
     *
     */
    static void post_init_root()
    {
        g_drivers.root.foreach_reverse(DeviceInitOverloaded{
            [](platform::CoreSystemDriver& dev) { (void)dev; },
            [](platform::GPIOPortADevice& dev)
            { expect(dev.post_init(), "Failed to post-initialize GPIO Port A Device"); },
            [](platform::GPIOPortBDevice& dev)
            { expect(dev.post_init(), "Failed to post-initialize GPIO Port B Device"); },
            [](platform::HRTIM1ControllerDevice& dev)
            { expect(dev.post_init(), "Failed to post-initialize HRTIM1 Controller Device"); },
            [](platform::DMAMux1ControllerDevice& dev)
            { expect(dev.post_init(), "Failed to post-initialize DMAMux1 Controller Device"); },
            [](platform::DMA1ControllerDevice& dev)
            { expect(dev.post_init(), "Failed to post-initialize DMA1 Controller Device"); },
            [](platform::ADC12CommonDevice& dev)
            { expect(dev.post_init(), "Failed to post-initialize ADC 12 Common Device"); },
            [](platform::ADC1ControllerDevice& dev)
            { expect(dev.post_init(kCurrentSensorADCConfig), "Failed to post-initialize ADC 1 Controller Device"); },
        });
    }

    /**
     * @brief Initialize the system hardware.
     *
     */
    void init()
    {
        // Core System Device must be initialized first since other devices may depend on it for clock configuration
        expect(g_drivers.core.init(kDefaultCoreSystemConfig), "Failed to initialize Core System Device");

        init_root();
        init_drivers();
        post_init_root();

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