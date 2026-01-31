#include "app.hpp"

VALLE_DEFINE_UART_LOGGER_HANDLER(app::g_drivers.uart_logger);

namespace valle::app
{
    DriverBuilderReturnT install_drivers(DriverBuilderT&& builder)
    {
        return std::move(builder).template install<UARTLoggerT>().template install<TestADCDriverT>().yield();
    }

    /**
     * @brief Initialize shared devices.
     *
     */
    static void init_shared()
    {
        g_ref_registry.foreach_shared(Overloaded{
            [](ADC12ClockDevice& dev)
            {
                dev.init(ADCAsyncClockConfig{.source    = ADCAsyncClockSource::kSysclk,
                                             .prescaler = ADCAsyncClockPrescaler::kDiv8});
            },
            [](TestADCControllerT& dev)
            {
                dev.init(ADCControllerConfig{
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
                                                             DMAInterruptConfig{
                                                                          .priority  = 10,
                                                                          .enable_tc = true,
                                                                          .enable_ht = false,
                                                                          .enable_te = false,
                                                             },
                                                     },
                                                          .overrun           = ADCRegularGroupOverrunBehavior::kOverwrite,
                                                          .conversion_mode   = ADCRegularGroupConversionMode::kSingleShot,
                                                          .oversampling_mode = ADCRegularGroupOversamplingMode::kDiscontinuous},
                    .oversampling = std::nullopt  // No oversampling by default
                });
            },
            [](DMAMux1ControllerDevice& dev) { dev.init(); },
            [](DMA1ControllerDevice& dev) { dev.init(); },
            [](DMA2ControllerDevice& dev) { dev.init(); },
            [](GPIOPortADevice& dev) { dev.init(); },
        }  // namespace valle
        );
    }

    /**
     * @brief Initialize drivers.
     *
     */
    static void init_drivers()
    {
        g_drivers.uart_logger.init(UARTControllerConfig{
            .baud_rate         = UARTBaudRate::kBaud230400,
            .word_length       = UARTWordLength::kBits8,
            .stop_bits         = UARTStopBits::kBits1,
            .parity            = UARTParity::kNone,
            .transfer_mode     = UARTTransferMode::kTxRx,
            .hw_flow_ctrl      = UARTHardwareFlowControl::kNone,
            .dma_priority      = DMAPriority::kHigh,
            .dma_int_priority  = 5,
            .uart_int_priority = 5,
        });

        g_drivers.test_adc.init(ADCSensorDriverConfig<TestADCConverterT>{
            .channel_config   = ADCChannelConfig{.sampling_time = ADCChannelSampleTime::k12Cycles5,
                                                 .input_mode    = ADCChannelInputMode::kSingleEnded,
                                                 .offset        = std::nullopt},
            .converter_config = {}});
    }

    /**
     * @brief Post-initialize shared devices.
     *
     */
    static void post_init_shared()
    {
        g_ref_registry.foreach_shared(Overloaded{
            [](ADC12ClockDevice& dev) { dev.post_init(); },
            [](TestADCControllerT& dev) { dev.post_init(); },
            [](DMAMux1ControllerDevice& dev) { dev.post_init(); },
            [](DMA1ControllerDevice& dev) { dev.post_init(); },
            [](DMA2ControllerDevice& dev) { dev.post_init(); },
            [](GPIOPortADevice& dev) { dev.post_init(); },
        });
    }

    /**
     * @brief Initialize the system hardware.
     *
     */
    void init()
    {
        init_shared();
        init_drivers();
        post_init_shared();
    }

}  // namespace valle::app