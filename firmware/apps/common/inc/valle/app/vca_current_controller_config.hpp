#pragma once

#include "valle/platform/hardware/common.hpp"
#include "valle/platform/modules/acs724.hpp"
#include "valle/platform/modules/vca.hpp"

namespace valle::app
{
    // -----------------------------------------------------------------------------
    // DEVICE CONFIG
    // -----------------------------------------------------------------------------
    constexpr HRTIMControllerID kVCAPWMHRTIMControllerID = 1;
    constexpr HRTIMTimerID      kVCAPWMHRTIMTimerID      = HRTIMTimerID::kA;

    // HRTIM Controller
    struct HRTIMControllerCTConfig : HRTIMControllerCTDefaultConfig
    {
    };

    // HRTIM Timer
    struct HRTIMTimerCTConfig : HRTIMTimerCTDefaultConfig
    {
        using Output1PinT = GPIOPinA8Device;
        using Output2PinT = GPIOPinA9Device;
    };

    // Current Sensor ADC Channel (pin PA0, A0)
    constexpr ADCControllerID kCurrentSensorADCID        = 1;
    constexpr ADCChannelID    kCurrentSensorADCChannelId = ADCChannelID::kChannel1;

    using CurrentSensorADCDMAChannelT = DMA1Channel2Device;
    struct ADCControllerCTConfig : ADCControllerCTDefaultConfig
    {
        using DMAChannelT = CurrentSensorADCDMAChannelT;
    };

    // -----------------------------------------------------------------------------
    // PARAMETERS
    // -----------------------------------------------------------------------------

    constexpr uint32_t kVCAPWMFreqHz          = 60000U;         // 60 kHz PWM Frequency
    constexpr uint32_t kCurrentSamplingFreqHz = kVCAPWMFreqHz;  // Sample at PWM frequency for synchronization

    // Set to false to use regular channel instead (not recommended, less accurate timing)
    static constexpr bool kCurrentSensorUseInject = true;

    constexpr auto kVCAPWMPeriod          = DurationSecondsF(1.0F / static_cast<float>(kVCAPWMFreqHz));
    constexpr auto kCurrentSamplingPeriod = DurationSecondsF(1.0F / static_cast<float>(kCurrentSamplingFreqHz));
    constexpr auto kMaxCurrentSensorSampleTime =
        kCurrentSamplingPeriod - DurationMicros(12);  // Leave at least 12 µs margin for ADC conversion and processing

    // -----------------------------------------------------------------------------
    // VCA CONFIG
    // -----------------------------------------------------------------------------

    using VCAControllerSystemControllerT       = VCAClosedLoopCurrentFeedbackController<float>;
    using VCAControllerSystemControllerConfigT = typename VCAControllerSystemControllerT::ConfigT;

    using VCAControllerConfigT = VCAControllerHRTIMModuleConfig<VCAControllerSystemControllerConfigT>;

    constexpr auto kVCAControllerConfig =
        VCAControllerConfigT{.half_bridge_config =
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
                             .controller_config = VCAControllerSystemControllerConfigT{// Control loop at PWM frequency
                                                                                       .sample_time     = kVCAPWMPeriod,
                                                                                       .max_current_amp = 1.0F,
                                                                                       .target_tolerance_amp = 0.001F}};

    // -----------------------------------------------------------------------------
    // ADC CONFIG
    // ------------------------------------------------------------------------------
    constexpr auto kCurrentSensorADCClockConfig =
        ADCAsyncClockConfig{.source = ADCAsyncClockSource::kSysclk, .prescaler = ADCAsyncClockPrescaler::kDiv4};

    static constexpr ADCInjectGroupTriggerSource kCurrentSensorADCInjectGroupHRTIMTrigger =
        ADCInjectGroupTriggerSource::kExtHrtimTRG2;
    static constexpr ADCRegularGroupTriggerSource kCurrentSensorADCRegularGroupHRTIMTrigger =
        ADCRegularGroupTriggerSource::kExtHrtimTRG1;

    constexpr auto kCurrentSensorADCConfig = ADCControllerConfig{
        .resolution     = ADCResolution::k12Bit,
        .data_alignment = ADCDataAlignment::kRight,
        .low_power      = ADCLowPowerMode::kNone,
        .inj =
            ADCInjectGroupConfig{
                .trigger_source            = kCurrentSensorADCInjectGroupHRTIMTrigger,
                .trigger_edge              = ADCInjectGroupTriggerEdge::kRising,
                .auto_trigger_from_regular = false,
            },
        .reg = ADCRegularGroupConfig{.trigger_source = kCurrentSensorADCRegularGroupHRTIMTrigger,
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

    constexpr auto kCurrentSensorADCChannelConfig = ADCChannelConfig{
        .sampling_time = ADCChannelSampleTime::k24Cycles5,
        .input_mode    = ADCChannelInputMode::kSingleEnded,
        .offset        = std::nullopt,
    };

    constexpr auto kCurrentSensorConfig = ACS724ModuleConfig{.channel_config = kCurrentSensorADCChannelConfig};

    constexpr HRTIMTimerADCTriggerID kCurrentSensorHRTIMADCTriggerID =
        kCurrentSensorUseInject ? adc_trigger_to_hrtim_trigger(kCurrentSensorADCInjectGroupHRTIMTrigger)
                                : adc_trigger_to_hrtim_trigger(kCurrentSensorADCRegularGroupHRTIMTrigger);
    constexpr auto kCurrentSensorHRTIMADCTriggerConfig =
        HRTIMTimerADCTriggerConfig<kVCAPWMHRTIMTimerID, kCurrentSensorHRTIMADCTriggerID>{
            .source = HRTIMTimerADCTriggerSourceID<kVCAPWMHRTIMTimerID, kCurrentSensorHRTIMADCTriggerID>::kPeriod,
            .rollover_mode = HRTIMTimerADCRolloverMode::kPeriodReset,
            .post_scaler   = 0,
        };

    // -----------------------------------------------------------------------------
    // VALIDATE
    // ------------------------------------------------------------------------------
    constexpr auto kCurrentSensorChannelSampleTime = ADCRootInterface::calculate_channel_sample_time_s(
        ADCRootInterface::calculate_clock_freq_hz(kSystemClockFreqHz, kCurrentSensorADCClockConfig.prescaler),
        kCurrentSensorADCChannelConfig.sampling_time,
        kCurrentSensorADCConfig.oversampling.has_value()
            ? std::optional<ADCOversamplingRatio>(kCurrentSensorADCConfig.oversampling->ratio)
            : std::nullopt);

    static_assert(kCurrentSensorChannelSampleTime < kMaxCurrentSensorSampleTime,
                  "ADC sampling time must be less than PWM period for proper synchronization");
}  // namespace valle::app

VALLE_DEFINE_HRTIM_CONTROLLER_CT_CONFIG(app::kVCAPWMHRTIMControllerID, app::HRTIMControllerCTConfig{});
VALLE_DEFINE_HRTIM_TIMER_CT_CONFIG(app::kVCAPWMHRTIMControllerID, app::kVCAPWMHRTIMTimerID, app::HRTIMTimerCTConfig{});
VALLE_DEFINE_ADC_CONTROLLER_CT_CONFIG(app::kCurrentSensorADCID, app::ADCControllerCTConfig{});

namespace valle::app
{
    using VCAPWMHRTIMControllerDeviceT = HRTIMControllerDevice<kVCAPWMHRTIMControllerID>;
    using VCAPWMHRTIMTimerDeviceT      = HRTIMTimerDevice<kVCAPWMHRTIMControllerID, kVCAPWMHRTIMTimerID>;
    using VCAControllerT = VCAControllerHRTIMModule<VCAPWMHRTIMTimerDeviceT, VCAControllerSystemControllerT>;

    using CurrentSensorADCChannelT =
        std::conditional_t<kCurrentSensorUseInject,
                           ADCInjectChannelRank1Device<kCurrentSensorADCID, kCurrentSensorADCChannelId>,
                           ADCRegularChannelRank1Device<kCurrentSensorADCID, kCurrentSensorADCChannelId>>;
    using CurrentSensorADCControllerT = CurrentSensorADCChannelT::ChannelT::ControllerT;
    using CurrentSensorT              = ACS724Module<CurrentSensorADCChannelT, ACS724Model::k2P5ABi>;

    inline void init_vca_controller(VCAControllerT& vca_controller)
    {
        expect(vca_controller.init(kVCAControllerConfig), "Failed to initialize HRTIM Half Bridge Driver");

        expect(vca_controller.get_half_bridge().init_adc_trigger(kCurrentSensorHRTIMADCTriggerConfig),
               "Failed to initialize HRTIM ADC Trigger");
    }

    inline void init_current_sensor(CurrentSensorT& current_sensor)
    {
        expect(current_sensor.init(kCurrentSensorConfig), "Failed to initialize ACS724 Current Sensor Driver");
    }

}  // namespace valle::app