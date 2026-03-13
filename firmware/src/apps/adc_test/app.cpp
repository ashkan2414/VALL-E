#include "app.hpp"

#include "valle/app/system_config.hpp"
#include "valle/base/panic.hpp"

VALLE_DEFINE_UART_LOGGER_HANDLER(app::g_drivers.uart_logger);

namespace valle::app
{
    DriverBuilderReturnT install_drivers(DriverBuilderT&& builder)
    {
        return std::move(builder)
            .template install<RootDriver>()
            .template install<CoreSystemDriver>()
            .template install<UARTLoggerT>()
            .template install<TestADCDriverT>()
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
            [](ADC12CommonDevice& dev)
            {
                expect(dev.init(ADCCommonAsyncClockConfig{.source    = ADCCommonAsyncClockSource::kSysclk,
                                                          .prescaler = ADCCommonAsyncClockPrescaler::kDiv8}),
                       "Failed to initialize ADC12 Clock Device");
            },
            [](ADC1ControllerDevice& dev) { expect(dev.init(), "Failed to initialize Test ADC Controller"); },
            [](DMAMux1ControllerDevice& dev) { expect(dev.init(), "Failed to initialize DMAMux1 Controller Device"); },

            [](DMA1ControllerDevice& dev) { expect(dev.init(), "Failed to initialize DMA1 Controller Device"); },

            [](DMA2ControllerDevice& dev) { expect(dev.init(), "Failed to initialize DMA2 Controller Device"); },

            [](GPIOPortADevice& dev) { expect(dev.init(), "Failed to initialize GPIO Port A Device"); },
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

        expect(g_drivers.test_adc.init(ADCAnalogSensorDriverConfig<TestADCConverterT>{
                   .channel_config = ADCChannelConfig{.sampling_time = ADCChannelSampleTime::k12Cycles5,
                                                      .input_mode    = ADCChannelInputMode::kSingleEnded,
                                                      .offset        = std::nullopt},
                   .converter_config{}}),
               "Failed to initialize Test ADC Driver");
    }

    /**
     * @brief Post-initialize shared devices.
     *
     */
    static void post_init_root()
    {
        g_drivers.root.foreach_reverse(DeviceInitOverloaded{
            [](CoreSystemDriver& dev) { (void)dev; },
            [](ADC12CommonDevice& dev) { expect(dev.post_init(), "Failed to post-initialize ADC12 Clock Device"); },

            [](ADC1ControllerDevice& dev)
            {
                expect(
                    dev.post_init(ADCControllerConfig{
                        .resolution     = ADCResolution::k12Bit,
                        .data_alignment = ADCDataAlignment::kRight,
                        .low_power      = ADCLowPowerMode::kNone,
                        .inj =
                            ADCInjectGroupConfig{
                                .trigger_source = ADCInjectGroupTriggerSource::kSoftware,
                                .trigger_edge   = ADCInjectGroupTriggerEdge::kRisingFalling,
                            },
                        .reg          = ADCRegularGroupConfig{.trigger_source = ADCRegularGroupTriggerSource::kSoftware,
                                                              .trigger_edge   = ADCRegularGroupTriggerEdge::kRisingFalling,
                                                              .dma =
                                                         ADCRegularGroupDMAConfig{
                                                                      .priority      = DMAPriority::kHigh,
                                                                      .circular_mode = true,
                                                                      .interrupts =
                                                                 DMAChannelInterruptConfig{
                                                                              .priority = 10,
                                                                              .interrupts =
                                                                         DMAChannelInterruptMask{
                                                                                      .transfer_complete = true,
                                                                                      .half_transfer     = false,
                                                                                      .transfer_error    = false,
                                                                         },
                                                                 },
                                                         },
                                                              .overrun           = ADCRegularGroupOverrunBehavior::kOverwrite,
                                                              .conversion_mode   = ADCRegularGroupConversionMode::kSingleShot,
                                                              .oversampling_mode = ADCRegularGroupOversamplingMode::kContinuous},
                        .oversampling = std::nullopt  // No oversampling by default
                    }),
                    "Failed to post-initialize ADC1 Controller Device");
            },

            [](DMAMux1ControllerDevice& dev)
            { expect(dev.post_init(), "Failed to post-initialize DMAMux1 Controller Device"); },

            [](DMA1ControllerDevice& dev)
            { expect(dev.post_init(), "Failed to post-initialize DMA1 Controller Device"); },

            [](DMA2ControllerDevice& dev)
            { expect(dev.post_init(), "Failed to post-initialize DMA2 Controller Device"); },

            [](GPIOPortADevice& dev) { expect(dev.post_init(), "Failed to post-initialize GPIO Port A Device"); },
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
    }

}  // namespace valle::app