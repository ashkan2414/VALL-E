#pragma once

#include "valle/app/system_config.hpp"
#include "valle/base/panic.hpp"
#include "valle/platform/hardware/common.hpp"
#include "valle/platform/modules/acs724.hpp"
#include "valle/platform/modules/vca.hpp"
#include "valle/system/timing.hpp"


namespace valle::app
{
    // -----------------------------------------------------------------------------
    // DEVICE CONFIG
    // -----------------------------------------------------------------------------
    constexpr platform::HRTIMControllerID kVCAPWMHRTIMControllerID = 1;
    constexpr platform::HRTIMTimerID      kVCAPWMHRTIMTimerID      = platform::HRTIMTimerID::kA;

    // HRTIM Controller
    struct HRTIMControllerCTConfig : platform::HRTIMControllerCTDefaultConfig
    {
    };

    // HRTIM Timer
    struct HRTIMTimerCTConfig : platform::HRTIMTimerCTDefaultConfig
    {
        using Output1PinT = platform::GPIOPinA8Device;
        using Output2PinT = platform::GPIOPinA9Device;
    };

    // Current Sensor ADC Channel (pin PA0, A0)
    constexpr platform::ADCControllerID kCurrentSensorADCID        = 1;
    constexpr platform::ADCChannelID    kCurrentSensorADCChannelId = platform::ADCChannelID::kChannel1;

    using CurrentSensorADCDMAChannelT = platform::DMA1Channel2Device;
    struct ADCControllerCTConfig : platform::ADCControllerCTDefaultConfig
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
        kCurrentSamplingPeriod -
        system::DurationMicros(12);  // Leave at least 12 µs margin for ADC conversion and processing

    // -----------------------------------------------------------------------------
    // VCA CONFIG
    // -----------------------------------------------------------------------------

    using VCAControllerSystemControllerT       = VCAClosedLoopCurrentFeedbackController<float>;
    using VCAControllerSystemControllerConfigT = typename VCAControllerSystemControllerT::ConfigT;

    using VCAControllerConfigT = platform::VCAControllerHRTIMModuleConfig<VCAControllerSystemControllerConfigT>;

    constexpr auto kVCAControllerConfig = VCAControllerConfigT{
        .half_bridge_config =
            platform::HRTIMHalfBridgeDriverConfig{
                .freq_hz          = kVCAPWMFreqHz,
                .repetition       = 1,
                .rollover_mode    = platform::HRTIMTimerRolloverMode::kPeriodReset,
                .interrupt_config = platform::HRTIMTimerInterruptConfig{.priority = 5,
                                                                        .interrupts =
                                                                            platform::HRTIMTimerInterruptMask{
                                                                                .repetition = true,
                                                                            }},
                .output_config =
                    platform::HRTIMHalfBridgeOutputConfig{
                        .polarity    = platform::HRTIMTimerOutputPolarity::kPositive,
                        .idle_mode   = platform::HRTIMTimerOutputIdleMode::kNoIdle,
                        .idle_level  = platform::HRTIMTimerOutputIdleLevel::kInactive,
                        .fault_state = platform::HRTIMTimerOutputFaultState::kInactive,
                        .gpio_config =
                            platform::HRTIMTimerOutputGPIOConfig{
                                .speed = platform::GPIOSpeedMode::kLow,
                                .pull  = platform::GPIOPullMode::kNoPull,
                            },
                    },
                .deadtime_config = platform::HRTIMTimerDeadTimeConfig{.rise_ns = 200.0F, .fall_ns = 200.0F},
                .min_duty        = 0.05F,
                .max_duty        = 0.95F,
                .center_aligned  = true,
                .compare_unit    = platform::HRTIMTimerCompareUnit::kCompare1,
            },
        .controller_config = VCAControllerSystemControllerConfigT{// Control loop at PWM frequency
                                                                  .sample_time          = kVCAPWMPeriod,
                                                                  .max_current_amp      = 1.0F,
                                                                  .target_tolerance_amp = 0.001F}};

    // -----------------------------------------------------------------------------
    // ADC CONFIG
    // ------------------------------------------------------------------------------
    constexpr auto kCurrentSensorADCCommonConfig = platform::ADCCommonConfig<platform::ADCCommonID::kADC12>{
        .clock_config =
            platform::ADCCommonAsyncClockConfig{.prescaler = platform::ADCCommonAsyncClockPrescaler::kDiv4}};
    static_assert(!kCurrentSensorADCCommonConfig.validate(kDefaultCoreSystemConfig.rcc_config).has_value(),
                  "ADC Common config is invalid");

    static constexpr platform::ADCInjectGroupTriggerSource kCurrentSensorADCInjectGroupHRTIMTrigger =
        platform::ADCInjectGroupTriggerSource::kExtHrtimTRG2;
    static constexpr platform::ADCRegularGroupTriggerSource kCurrentSensorADCRegularGroupHRTIMTrigger =
        platform::ADCRegularGroupTriggerSource::kExtHrtimTRG1;

    constexpr auto kCurrentSensorADCConfig = platform::ADCControllerConfig{
        .resolution     = platform::ADCResolution::k12Bit,
        .data_alignment = platform::ADCDataAlignment::kRight,
        .low_power      = platform::ADCLowPowerMode::kNone,
        .inj =
            platform::ADCInjectGroupConfig{
                .trigger_source            = kCurrentSensorADCInjectGroupHRTIMTrigger,
                .trigger_edge              = platform::ADCInjectGroupTriggerEdge::kRising,
                .auto_trigger_from_regular = false,
            },
        .reg = platform::ADCRegularGroupConfig{.trigger_source = kCurrentSensorADCRegularGroupHRTIMTrigger,
                                               .trigger_edge   = platform::ADCRegularGroupTriggerEdge::kRising,
                                               .dma =
                                                   platform::ADCRegularGroupDMAConfig{
                                                       .priority      = platform::DMAPriority::kHigh,
                                                       .circular_mode = true,
                                                       .interrupts =
                                                           platform::DMAChannelInterruptConfig{
                                                               .priority = 5,
                                                               .interrupts =
                                                                   platform::DMAChannelInterruptMask{
                                                                       .transfer_complete = true,
                                                                       .half_transfer     = false,
                                                                       .transfer_error    = false,
                                                                   },
                                                           },
                                                   },
                                               .overrun         = platform::ADCRegularGroupOverrunBehavior::kOverwrite,
                                               .conversion_mode = platform::ADCRegularGroupConversionMode::kSingleShot,
                                               .oversampling_mode =
                                                   platform::ADCRegularGroupOversamplingMode::kContinuous},
        .oversampling =
            platform::ADCOversamplingConfig{
                .ratio = platform::ADCOversamplingRatio::k8x,
                .shift = platform::ADCOversamplingShift::kDiv2,
                .scope = (kCurrentSensorUseInject ? platform::ADCOversamplingScope::kInject
                                                  : platform::ADCOversamplingScope::kRegularContinued),
            },
    };

    constexpr auto kCurrentSensorADCChannelConfig = platform::ADCChannelConfig{
        .sampling_time = platform::ADCChannelSampleTime::k24Cycles5,
        .input_mode    = platform::ADCChannelInputMode::kSingleEnded,
        .offset        = std::nullopt,
    };

    constexpr auto kCurrentSensorConfig =
        platform::ACS724ModuleConfig{.channel_config = kCurrentSensorADCChannelConfig};

    constexpr platform::HRTIMTimerADCTriggerID kCurrentSensorHRTIMADCTriggerID =
        kCurrentSensorUseInject ? adc_trigger_to_hrtim_trigger(kCurrentSensorADCInjectGroupHRTIMTrigger)
                                : adc_trigger_to_hrtim_trigger(kCurrentSensorADCRegularGroupHRTIMTrigger);
    constexpr auto kCurrentSensorHRTIMADCTriggerConfig =
        platform::HRTIMTimerADCTriggerConfig<kVCAPWMHRTIMTimerID, kCurrentSensorHRTIMADCTriggerID>{
            .source =
                platform::HRTIMTimerADCTriggerSourceID<kVCAPWMHRTIMTimerID, kCurrentSensorHRTIMADCTriggerID>::kPeriod,
            .rollover_mode = platform::HRTIMTimerADCRolloverMode::kPeriodReset,
            .post_scaler   = 0,
        };

    // -----------------------------------------------------------------------------
    // VALIDATE
    // ------------------------------------------------------------------------------
    constexpr auto kCurrentSensorChannelSampleTime = platform::ADCRootInterface::calculate_channel_sample_time_s(
        kCurrentSensorADCCommonConfig.get_source_clock_freq_hz(kDefaultCoreSystemConfig.rcc_config),
        kCurrentSensorADCChannelConfig.sampling_time,
        kCurrentSensorADCConfig.oversampling.has_value()
            ? std::optional<platform::ADCOversamplingRatio>(kCurrentSensorADCConfig.oversampling->ratio)
            : std::nullopt);

    static_assert(kCurrentSensorChannelSampleTime < kMaxCurrentSensorSampleTime,
                  "ADC sampling time must be less than PWM period for proper synchronization");
}  // namespace valle::app

VALLE_DEFINE_HRTIM_CONTROLLER_CT_CONFIG(app::kVCAPWMHRTIMControllerID, app::HRTIMControllerCTConfig{});
VALLE_DEFINE_HRTIM_TIMER_CT_CONFIG(app::kVCAPWMHRTIMControllerID, app::kVCAPWMHRTIMTimerID, app::HRTIMTimerCTConfig{});
VALLE_DEFINE_ADC_CONTROLLER_CT_CONFIG(app::kCurrentSensorADCID, app::ADCControllerCTConfig{});

namespace valle::app
{
    using VCAPWMHRTIMControllerDeviceT = platform::HRTIMControllerDevice<kVCAPWMHRTIMControllerID>;
    using VCAPWMHRTIMTimerDeviceT      = platform::HRTIMTimerDevice<kVCAPWMHRTIMControllerID, kVCAPWMHRTIMTimerID>;
    using VCAControllerT = platform::VCAControllerHRTIMModule<VCAPWMHRTIMTimerDeviceT, VCAControllerSystemControllerT>;

    using CurrentSensorADCChannelT =
        std::conditional_t<kCurrentSensorUseInject,
                           platform::ADCInjectChannelRank1Device<kCurrentSensorADCID, kCurrentSensorADCChannelId>,
                           platform::ADCRegularChannelRank1Device<kCurrentSensorADCID, kCurrentSensorADCChannelId>>;
    using CurrentSensorADCControllerT = CurrentSensorADCChannelT::ChannelT::ControllerT;
    using CurrentSensorT              = platform::ACS724Module<CurrentSensorADCChannelT, ACS724Model::k2P5ABi>;

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