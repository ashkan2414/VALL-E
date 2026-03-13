#include "app.hpp"

#include "valle/core/error.hpp"
#include "valle/platform/hardware/common.hpp"

VALLE_DEFINE_UART_LOGGER_HANDLER(app::g_drivers.uart_logger);

namespace valle::app
{

    static constexpr ADCInjectGroupTriggerSource kADCInjectGroupHRTIMTrigger =
        ADCInjectGroupTriggerSource::kExtHrtimTRG2;
    static constexpr ADCRegularGroupTriggerSource kADCRegularGroupHRTIMTrigger =
        ADCRegularGroupTriggerSource::kExtHrtimTRG1;

    constexpr auto kVCAPWMPeriod          = DurationSecondsF(1.0F / static_cast<float>(kVCAPWMFreqHz));
    constexpr auto kCurrentSamplingPeriod = DurationSecondsF(1.0F / static_cast<float>(kCurrentSamplingFreqHz));

    constexpr auto kMaxCurrentSensorSampleTime =
        kCurrentSamplingPeriod - DurationMicros(12);  // Leave at least 12 µs margin for ADC conversion and processing

    constexpr auto kCurrentSensorADCClockConfig =
        ADCAsyncClockConfig{.source = ADCAsyncClockSource::kSysclk, .prescaler = ADCAsyncClockPrescaler::kDiv4};

    constexpr auto kCurrentSensorADCChannelConfig = ADCChannelConfig{
        .sampling_time = ADCChannelSampleTime::k24Cycles5,
        .input_mode    = ADCChannelInputMode::kSingleEnded,
        .offset        = std::nullopt,
    };

    constexpr auto kCurrentSensorADCConfig = ADCControllerConfig{
        .resolution     = ADCResolution::k12Bit,
        .data_alignment = ADCDataAlignment::kRight,
        .low_power      = ADCLowPowerMode::kNone,
        .inj =
            ADCInjectGroupConfig{
                .trigger_source            = kADCInjectGroupHRTIMTrigger,
                .trigger_edge              = ADCInjectGroupTriggerEdge::kRising,
                .auto_trigger_from_regular = false,
            },
        .reg = ADCRegularGroupConfig{.trigger_source = kADCRegularGroupHRTIMTrigger,
                                     .trigger_edge   = ADCRegularGroupTriggerEdge::kRising,
                                     .dma =
                                         ADCRegularGroupDMAConfig{
                                             .priority      = DMAPriority::kHigh,
                                             .circular_mode = true,
                                             .interrupts =
                                                 DMAChannelInterruptConfig{
                                                     .priority = 5,
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
        .oversampling =
            ADCOversamplingConfig{
                .ratio = ADCOversamplingRatio::k8x,
                .shift = ADCOversamplingShift::kDiv2,
                .scope =
                    (kCurrentSensorUseInject ? ADCOversamplingScope::kInject : ADCOversamplingScope::kRegularContinued),
            },
    };

    constexpr auto kCurrentSensorChannelSampleTime = ADCRootInterface::calculate_channel_sample_time_s(
        ADCRootInterface::calculate_clock_freq_hz(kSystemClockFreqHz, kCurrentSensorADCClockConfig.prescaler),
        kCurrentSensorADCChannelConfig.sampling_time,
        kCurrentSensorADCConfig.oversampling.has_value()
            ? std::optional<ADCOversamplingRatio>(kCurrentSensorADCConfig.oversampling->ratio)
            : std::nullopt);

    static_assert(kCurrentSensorChannelSampleTime < kMaxCurrentSensorSampleTime,
                  "ADC sampling time must be less than PWM period for proper synchronization");

    CurrentResponseCollectorT g_current_response_collector;

    DriverBuilderReturnT install_drivers(DriverBuilderT&& builder)
    {
        return std::move(builder)
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
    static void init_shared()
    {
        g_ref_registry.foreach_shared(Overloaded{
            [](GPIOPortADevice& dev) { valle::expect(dev.init(), "Failed to initialize GPIO Port A Device"); },
            [](GPIOPortBDevice& dev) { valle::expect(dev.init(), "Failed to initialize GPIO Port B Device"); },
            [](HRTIM1ControllerDevice& dev)
            { valle::expect(dev.init(), "Failed to initialize HRTIM1 Controller Device"); },
            [](DMAMux1ControllerDevice& dev)
            { valle::expect(dev.init(), "Failed to initialize DMAMux1 Controller Device"); },
            [](DMA1ControllerDevice& dev) { valle::expect(dev.init(), "Failed to initialize DMA1 Controller Device"); },
            [](ADC12ClockDevice& dev)
            { valle::expect(dev.init(kCurrentSensorADCClockConfig), "Failed to initialize ADC 12 Clock Device"); },
            [](ADC1ControllerDevice& dev) { valle::expect(dev.init(), "Failed to initialize ADC1 Controller Device"); },
        }  // namespace valle
        );
    }

    /**
     * @brief Initialize drivers.
     *
     */
    static void init_drivers()
    {
        valle::expect(g_drivers.uart_logger.init(UARTControllerConfig{
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

        valle::expect(g_drivers.vca_controller.init(
                          VCAControllerConfigT{
                              .half_bridge_config =
                                  HRTIMHalfBridgeDriverConfig{
                                      .freq_hz          = kVCAPWMFreqHz,
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
                                      .sample_time          = kVCAPWMPeriod,
                                      .max_current_amp      = 1.0F,
                                      .target_tolerance_amp = 0.001F,
                                      .feedback_fn          = delegate::Delegate<float>(
                                          []() { return g_drivers.current_sensor.read_amps(); }),
                                  }}),
                      "Failed to initialize HRTIM Half Bridge Driver");

        constexpr HRTIMTimerADCTriggerID adc_trigger_id =
            kCurrentSensorUseInject ? adc_trigger_to_hrtim_trigger(kADCInjectGroupHRTIMTrigger)
                                    : adc_trigger_to_hrtim_trigger(kADCRegularGroupHRTIMTrigger);

        valle::expect(g_drivers.vca_controller.get_half_bridge().init_adc_trigger(
                          HRTIMTimerADCTriggerConfig<kVCAPWMHRTIMTimerID, adc_trigger_id>{
                              .source = HRTIMTimerADCTriggerSourceID<kVCAPWMHRTIMTimerID, adc_trigger_id>::kPeriod,
                              .rollover_mode = HRTIMTimerADCRolloverMode::kPeriodReset,
                              .post_scaler   = 0,
                          }),
                      "Failed to initialize HRTIM ADC Trigger");

        valle::expect(
            g_drivers.current_sensor.init(ACS724ModuleConfig{
                .channel_config = kCurrentSensorADCChannelConfig, 
                .calibration_config = ACS724CalibrationConfig{
                                              .scale = 1.13026F,//0.993680193966F,  
                                              .offset = -0.266701F,//-0.0156816646131F,  
                                          }}),
            "Failed to initialize ACS724 Current Sensor Driver");

        valle::expect(g_drivers.test_gpio.init(GPIODigitalOutConfig{
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
    static void post_init_shared()
    {
        g_ref_registry.foreach_shared(Overloaded{
            [](GPIOPortADevice& dev)
            { valle::expect(dev.post_init(), "Failed to post-initialize GPIO Port A Device"); },
            [](GPIOPortBDevice& dev)
            { valle::expect(dev.post_init(), "Failed to post-initialize GPIO Port B Device"); },
            [](HRTIM1ControllerDevice& dev)
            { valle::expect(dev.post_init(), "Failed to post-initialize HRTIM1 Controller Device"); },
            [](DMAMux1ControllerDevice& dev)
            { valle::expect(dev.post_init(), "Failed to post-initialize DMAMux1 Controller Device"); },
            [](DMA1ControllerDevice& dev)
            { valle::expect(dev.post_init(), "Failed to post-initialize DMA1 Controller Device"); },
            [](ADC12ClockDevice& dev)
            { valle::expect(dev.post_init(), "Failed to post-initialize ADC 12 Clock Device"); },
            [](ADC1ControllerDevice& dev)
            {
                valle::expect(dev.post_init(kCurrentSensorADCConfig),
                              "Failed to post-initialize ADC 1 Controller Device");
            },
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

        if constexpr (kCurrentSensorUseInject)
        {
            app::g_devices.adc1->enable_interrupts(ADCInterruptConfig{
                .priority = 5,
                .interrupts =
                    ADCInterruptMask{
                        .inj_eos = true,
                    },
            });
        }
    }

}  // namespace valle::app