#include "app.hpp"

#include "valle/core/error.hpp"

VALLE_DEFINE_UART_LOGGER_HANDLER(app::g_drivers.uart_logger);

namespace valle::app
{

    DriverBuilderReturnT install_drivers(DriverBuilderT&& builder)
    {
        return std::move(builder)
            .template install<UARTLoggerT>()
            .template install<VCAControllerT>()
            .template install<CurrentSensorT>()
            .template install<PositionSensorT>()
            .yield();
    }

    /**
     * @brief Initialize shared devices.
     *
     */
    static void init_shared()
    {
        g_ref_registry.foreach_shared(Overloaded{
            [](DMAMux1ControllerDevice& dev)
            { valle::expect(dev.init(), "Failed to initialize DMAMux1 Controller Device"); },
            [](DMA1ControllerDevice& dev) { valle::expect(dev.init(), "Failed to initialize DMA1 Controller Device"); },
            [](DMA2ControllerDevice& dev) { valle::expect(dev.init(), "Failed to initialize DMA2 Controller Device"); },
            [](GPIOPortADevice& dev) { valle::expect(dev.init(), "Failed to initialize GPIO Port A Device"); },
            [](GPIOPortBDevice& dev) { valle::expect(dev.init(), "Failed to initialize GPIO Port B Device"); },
            [](HRTIM1ControllerDevice& dev)
            { valle::expect(dev.init(), "Failed to initialize HRTIM1 Controller Device"); },
            [](ADC345ClockDevice& dev)
            {
                valle::expect(dev.init(ADCAsyncClockConfig{.source    = ADCAsyncClockSource::kSysclk,
                                                           .prescaler = ADCAsyncClockPrescaler::kDiv8}),
                              "Failed to initialize ADC Clock Device");
            },
            [](ADC3ControllerDevice& dev)
            {
                valle::expect(dev.init(ADCControllerConfig{
                                  .resolution     = ADCResolution::k12Bit,
                                  .data_alignment = ADCDataAlignment::kRight,
                                  .low_power      = ADCLowPowerMode::kNone,
                                  .inj =
                                      ADCInjectGroupConfig{
                                          .trigger_source = ADCInjectGroupTriggerSource::kExtHrtimTRG1,
                                          .trigger_edge   = ADCInjectGroupTriggerEdge::kRising,
                                      },
                                  .reg          = ADCRegularGroupConfig{},  // Not used
                                  .oversampling = std::nullopt              // No oversampling by default
                              }),
                              "Failed to initialize ADC3 Controller Device");
            },
            [](I2C1CommandBufferDevice<>& dev)
            {
                valle::expect(dev.init(I2CCommandBufferDeviceConfig{
                                  .event_int_priority = 5,
                                  .error_int_priority = 5,
                              }),
                              "Failed to initialize I2C1 Command Buffer Device");
            }}  // namespace valle
        );
    }

    /**
     * @brief Initialize drivers.
     *
     */
    static void init_drivers()
    {
        valle::expect(g_drivers.uart_logger.init(UARTControllerConfig{
                          .baud_rate         = UARTBaudRate::kBaud115200,
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

        const uint32_t vca_pwm_freq_hz = 60000U;  // 60 kHz PWM Frequency
        valle::expect(g_drivers.vca_controller.init(
                          VCAControllerConfigT{
                              .half_bridge_config =
                                  HRTIMHalfBridgeDriverConfig{
                                      .freq_hz          = vca_pwm_freq_hz,
                                      .repetition       = 1,
                                      .rollover_mode    = HRTIMTimerRolloverMode::kPeriodReset,
                                      .interrupt_config = HRTIMTimerInterruptConfig{.priority = 5,
                                                                                    .interrupts =
                                                                                        HRTIMTimerInterruptMask{
                                                                                            .repetition = true,
                                                                                        }},
                                      .output_config =
                                          HRTIMHalfBridgeOutputConfig{
                                              .polarity    = HRTIMTimerOutputPolarity::kPositive,
                                              .idle_mode   = HRTIMTimerOutputIdleMode::kNoIdle,
                                              .idle_level  = HRTIMTimerOutputIdleLevel::kInactive,
                                              .fault_state = HRTIMTimerOutputFaultState::kInactive,
                                              .gpio_config =
                                                  HRTIMTimerOutputGPIOConfig{
                                                      .speed = GPIOSpeedMode::kLow,
                                                      .pull  = GPIOPullMode::kNoPull,
                                                  },
                                          },
                                      .deadtime_config = HRTIMTimerDeadTimeConfig{.rise_ns = 200.0F, .fall_ns = 200.0F},
                                      .min_duty        = 0.05F,
                                      .max_duty        = 0.95F,
                                      .center_aligned  = true,
                                      .compare_unit    = HRTIMTimerCompareUnit::kCompare1,
                                  },
                              .controller_config =
                                  VCAControllerSystemControllerConfigT{
                                      // Control loop at PWM frequency
                                      .sample_time_s        = 1.0F / static_cast<float>(vca_pwm_freq_hz),
                                      .max_current_amp      = 1.0F,
                                      .target_tolerance_amp = 0.001F,
                                      .feedback_fn          = delegate::Delegate<float>(
                                          []() { return g_drivers.current_sensor.read_amps(); }),
                                  }}),
                      "Failed to initialize HRTIM Half Bridge Driver");

        valle::expect(
            g_drivers.current_sensor.init(ACS724ModuleConfig{.channel_config =
                                                                 ADCChannelConfig{
                                                                     .sampling_time = ADCChannelSampleTime::k12Cycles5,
                                                                     .input_mode    = ADCChannelInputMode::kSingleEnded,
                                                                     .offset        = std::nullopt,
                                                                 }}),
            "Failed to initialize Current Sensor");

        // TODO: check return value
        valle::expect(g_drivers.position_sensor.init(LDC161XSensorConfig<1>{
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
                      }),
                      "Failed to initialize Position Sensor");
    }

    /**
     * @brief Post-initialize shared devices.
     *
     */
    static void post_init_shared()
    {
        g_ref_registry.foreach_shared(Overloaded{
            [](DMAMux1ControllerDevice& dev)
            { valle::expect(dev.post_init(), "Failed to post-initialize DMAMux1 Controller"); },
            [](DMA1ControllerDevice& dev)
            { valle::expect(dev.post_init(), "Failed to post-initialize DMA1 Controller"); },
            [](DMA2ControllerDevice& dev)
            { valle::expect(dev.post_init(), "Failed to post-initialize DMA2 Controller"); },
            [](GPIOPortADevice& dev) { valle::expect(dev.post_init(), "Failed to post-initialize GPIO Port A"); },
            [](GPIOPortBDevice& dev) { valle::expect(dev.post_init(), "Failed to post-initialize GPIO Port B"); },
            [](HRTIM1ControllerDevice& dev)
            { valle::expect(dev.post_init(), "Failed to post-initialize HRTIM1 Controller"); },
            [](ADC345ClockDevice& dev)
            { valle::expect(dev.post_init(), "Failed to post-initialize ADC Clock Device"); },
            [](ADC3ControllerDevice& dev)
            { valle::expect(dev.post_init(), "Failed to post-initialize ADC Controller"); },
            [](I2C1CommandBufferDevice<>& dev)
            { valle::expect(dev.post_init(), "Failed to post-initialize I2C Command Buffer"); },
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
        // Start current sensor ADC conversions
        g_devices.adc3->enable_interrupts(ADCInterruptConfig{
            .priority = 5,
            .interrupts =
                ADCInterruptMask{
                    .inj_eos = true,
                },
        });
        g_devices.adc3->start_inject();

        g_drivers.vca_controller.enable();
    }

}  // namespace valle::app