#include "app.hpp"

VALLE_DEFINE_UART_LOGGER_HANDLER(app::g_drivers.uart_logger);

namespace valle::app
{
    // Forward declaration
    float read_vca_current();

    DriverBuilderReturnT install_drivers(DriverBuilderT&& builder)
    {
        return std::move(builder)
            .template install<UARTLoggerT>()
            .template install<CurrentSensorT>()
            .template install<VCAControllerT>(delegate::Delegate<float>(&read_vca_current))
            .template install<PositionSensorT>()
            .yield();
    }

    /**
     * @brief Read current sensor helper function. Used as feedback for VCA controller.
     *
     * @return float Current in Amps.
     */
    float read_vca_current()
    {
        return g_drivers.current_sensor.read_amps();
    }

    /**
     * @brief Initialize shared devices.
     *
     */
    static void init_shared()
    {
        g_ref_registry.foreach_shared(Overloaded{
            [](ADCControllerDevice<1>& dev)
            {
                dev.init(ADCControllerConfig{
                    .resolution     = ADCResolution::k12Bit,
                    .data_alignment = ADCDataAlignment::kRight,
                    .low_power      = ADCLowPowerMode::kNone,
                    .inj =
                        ADCInjectGroupConfig{
                            .trigger_source = ADCInjectGroupTriggerSource::kSoftware,
                            .trigger_edge   = ADCInjectGroupTriggerEdge::kRising,
                        },
                    .reg          = ADCRegularGroupConfig{.trigger_source = ADCRegularGroupTriggerSource::kSoftware,
                                                          .trigger_edge   = ADCRegularGroupTriggerEdge::kRising,
                                                          .dma =
                                                     ADCRegularGroupDMAConfig{
                                                                  .priority      = DMAPriority::kHigh,
                                                                  .circular_mode = true,
                                                                  .interrupts =
                                                             DMAInterruptConfig{
                                                                          .priority  = 5,
                                                                          .enable_tc = true,
                                                                          .enable_ht = false,
                                                                          .enable_te = true,
                                                             },
                                                     },
                                                          .overrun           = ADCRegularGroupOverrunBehavior::kOverwrite,
                                                          .conversion_mode   = ADCRegularGroupConversionMode::kSingleShot,
                                                          .oversampling_mode = ADCRegularGroupOversamplingMode::kDiscontinuous},
                    .oversampling = std::nullopt  // No oversampling by default
                });
            },

            [](DMA1ControllerDevice& dev) { dev.init(); },
            [](GPIOPortADevice& dev) { dev.init(); },
            [](HRTIM1ControllerDevice& dev) { dev.init(); },
            [](I2CCommandBufferDevice<1>& dev)
            {
                dev.init(I2CCommandBufferDeviceConfig{
                    .event_int_priority = 5,
                    .error_int_priority = 5,
                });
            },
            [](DMA2ControllerDevice& dev) { dev.init(); },
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
            .baud_rate         = UARTBaudRate::kBaud115200,
            .word_length       = UARTWordLength::kBits8,
            .stop_bits         = UARTStopBits::kBits1,
            .parity            = UARTParity::kNone,
            .transfer_mode     = UARTTransferMode::kTxRx,
            .hw_flow_ctrl      = UARTHardwareFlowControl::kNone,
            .dma_priority      = DMAPriority::kHigh,
            .dma_int_priority  = 5,
            .uart_int_priority = 5,
        });

        g_drivers.vca.init();
        g_drivers.current_sensor.init(ACS724Config{.channel_config = ADCChannelConfig{
                                                       .sampling_time = ADCChannelSampleTime::k12Cycles5,
                                                       .input_mode    = ADCChannelInputMode::kSingleEnded,
                                                       .offset        = std::nullopt,
                                                   }});

        // TODO: check return value
        g_drivers.position_sensor.init(LDC161XSensorConfig<1>{
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
        });
    }

    /**
     * @brief Post-initialize shared devices.
     *
     */
    static void post_init_shared()
    {
        g_ref_registry.foreach_shared(Overloaded{
            [](ADCControllerDevice<1>& dev) { dev.post_init(true, false); },
            [](DMA1ControllerDevice& dev) { dev.post_init(); },
            [](GPIOPortADevice& dev) { dev.post_init(); },
            [](HRTIM1ControllerDevice& dev) { dev.post_init(); },
            [](I2CCommandBufferDevice<1>& dev) { dev.post_init(); },
            [](DMA2ControllerDevice& dev) { dev.post_init(); },
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

    void start_vca_controller()
    {
        g_ref_registry.template get<CurrentSensorADCControllerT>().start_inject();
        g_drivers.vca.enable();
    }

}  // namespace valle::app