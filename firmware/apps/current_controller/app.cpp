#include "app.hpp"

#include "valle/app/system_config.hpp"
#include "valle/error.hpp"
#include "valle/timing.hpp"

VALLE_DEFINE_UART_LOGGER_HANDLER(app::g_drivers.uart_logger);

namespace valle::app
{
    CurrentResponseCollectorT g_current_response_collector;

    DriverBuilderReturnT install_drivers(DriverBuilderT&& builder)
    {
        return std::move(builder)
            .template install<RootDriver>()
            .template install<CoreSystemDriver>()
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
            [](CoreSystemDriver& dev) { (void)dev; },
            [](GPIOPortADevice& dev) { expect(dev.init(), "Failed to initialize GPIO Port A Device"); },
            [](GPIOPortBDevice& dev) { expect(dev.init(), "Failed to initialize GPIO Port B Device"); },
            [](HRTIM1ControllerDevice& dev) { expect(dev.init(), "Failed to initialize HRTIM1 Controller Device"); },
            [](DMAMux1ControllerDevice& dev) { expect(dev.init(), "Failed to initialize DMAMux1 Controller Device"); },
            [](DMA1ControllerDevice& dev) { expect(dev.init(), "Failed to initialize DMA1 Controller Device"); },
            [](ADC12ClockDevice& dev)
            { expect(dev.init(kCurrentSensorADCClockConfig), "Failed to initialize ADC 12 Clock Device"); },
            [](ADC1ControllerDevice& dev) { expect(dev.init(), "Failed to initialize ADC1 Controller Device"); },
        }  // namespace valle
        );
    }

    /**
     * @brief Initialize drivers.
     *
     */
    static void init_drivers()
    {
        expect(g_drivers.uart_logger.init(UARTControllerConfig{
                   .baud_rate         = UARTBaudRate::kBaud230400,
                   .word_length       = UARTWordLength::kBits8,
                   .stop_bits         = UARTStopBits::kBits1,
                   .parity            = UARTParity::kNone,
                   .transfer_mode     = UARTTransferMode::kTxRx,
                   .hw_flow_ctrl      = UARTHardwareFlowControl::kNone,
                   .dma_priority      = DMAPriority::kHigh,
                   .dma_int_priority  = 5,
                   .uart_int_priority = 5,
               }),
               "Failed to initialize UART Logger Driver");

        init_vca_controller(g_drivers.vca_controller);
        init_current_sensor(g_drivers.current_sensor);

        expect(g_drivers.test_gpio.init(GPIODigitalOutConfig{
                   .mode  = GPIOOutputMode::kPushPull,
                   .speed = GPIOSpeedMode::kLow,
                   .pull  = GPIOPullMode::kNoPull,
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
            [](CoreSystemDriver& dev) { (void)dev; },
            [](GPIOPortADevice& dev) { expect(dev.post_init(), "Failed to post-initialize GPIO Port A Device"); },
            [](GPIOPortBDevice& dev) { expect(dev.post_init(), "Failed to post-initialize GPIO Port B Device"); },
            [](HRTIM1ControllerDevice& dev)
            { expect(dev.post_init(), "Failed to post-initialize HRTIM1 Controller Device"); },
            [](DMAMux1ControllerDevice& dev)
            { expect(dev.post_init(), "Failed to post-initialize DMAMux1 Controller Device"); },
            [](DMA1ControllerDevice& dev)
            { expect(dev.post_init(), "Failed to post-initialize DMA1 Controller Device"); },
            [](ADC12ClockDevice& dev) { expect(dev.post_init(), "Failed to post-initialize ADC 12 Clock Device"); },
            [](ADC1ControllerDevice& dev)
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
            app::g_drivers.root.adc1().enable_interrupts(ADCInterruptConfig{
                .priority = 5,
                .interrupts =
                    ADCInterruptMask{
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