#pragma once

#include "valle/app/platform/core_system_config.hpp"
#include "valle/app/platform/modules/acs724.hpp"
#include "valle/app/platform/modules/vca.hpp"
#include "valle/base/panic.hpp"
#include "valle/platform/hardware/common.hpp"
#include "valle/system/sync.hpp"
#include "valle/system/timing.hpp"
#include "valle/utils/atomic.hpp"

namespace valle::platform::app
{
    struct VCACurrentLoopDriverDefaultCTConfig
    {
        using PWMOutput1PinT              = GpioPinA8;
        using PWMOutput2PinT              = GpioPinA9;
        using CurrentSensorAdcDmaChannelT = Dma1Channel2;

        static constexpr HrtimControllerId skVcaHrtimPwmControllerId = platform::HrtimControllerId::kHrtim1;
        static constexpr HrtimTimerId      skVcaHrtimPwmTimerId      = HrtimTimerId::kTimerA;

        static constexpr AdcControllerId skCurrentSensorAdcControllerId = platform::AdcControllerId::kAdc1;
        static constexpr AdcChannelId    skCurrentSensorAdcChannelId    = AdcChannelId::kChannel1;

        static constexpr auto skCurrentSensorModel                 = valle::app::ACS724Model::k2P5ABi;
        static constexpr auto skCurrentSensorAdcHrtimTriggerSource = AdcInjectGroupTriggerSource::kExtHrtimTRG2;
    };

    template <uint8_t tkControllerId>
    struct VCACurrentLoopDriverCTConfigRegistry
    {
        static constexpr auto skConfig = VCACurrentLoopDriverDefaultCTConfig{};
    };

    template <uint8_t tkControllerId>
    using VCACurrentLoopDriverCTConfigT = decltype(VCACurrentLoopDriverCTConfigRegistry<tkControllerId>::skConfig);

    template <uint8_t tkControllerId>
    struct VCACurrentLoopDriverCTConfigTraits : public VCACurrentLoopDriverCTConfigT<tkControllerId>
    {
        using BaseT = VCACurrentLoopDriverCTConfigT<tkControllerId>;

        using ValueT                               = float;
        using VCAControllerSystemControllerT       = valle::app::VCAClosedLoopCurrentFeedbackController<ValueT>;
        using VCAControllerSystemControllerConfigT = typename VCAControllerSystemControllerT::ConfigT;
        using VCAControllerConfigT = VCAControllerHrtimModuleConfig<VCAControllerSystemControllerConfigT>;

        using VCAHrtimPwmControllerDeviceT = HrtimController<BaseT::skVcaHrtimPwmControllerId>;
        using VCAHrtimPwmTimerDeviceT      = HrtimTimer<BaseT::skVcaHrtimPwmControllerId, BaseT::skVcaHrtimPwmTimerId>;
        using VCAControllerT = VCAControllerHrtimModule<VCAHrtimPwmTimerDeviceT, VCAControllerSystemControllerT>;

        static constexpr auto skCurrentSensorAdcCommonId =
            AdcControllerTraits<BaseT::skCurrentSensorAdcControllerId>::skCommonId;

        using CurrentSensorAdcControllerDeviceT = AdcController<BaseT::skCurrentSensorAdcControllerId>;
        using CurrentSensorAdcChannelDeviceT =
            AdcInjectChannelRank1<BaseT::skCurrentSensorAdcControllerId, BaseT::skCurrentSensorAdcChannelId>;
        using CurrentSensorT = ACS724Module<CurrentSensorAdcChannelDeviceT, BaseT::skCurrentSensorModel>;

        static constexpr auto skCurrentSensorHrtimAdcTriggerId =
            adc_trigger_to_hrtim_trigger(BaseT::skCurrentSensorAdcHrtimTriggerSource);

        using HrtimTimerAdcTriggerConfigT =
            HrtimTimerAdcTriggerConfig<BaseT::skVcaHrtimPwmTimerId, skCurrentSensorHrtimAdcTriggerId>;
    };

#define VALLE_DEFINE_VCA_CURRENT_LOOP_DRIVER_CT_CONFIG(name, id, config)                                 \
    namespace valle::platform::app                                                                       \
    {                                                                                                    \
        template <>                                                                                      \
        struct VCACurrentLoopDriverCTConfigRegistry<id>                                                  \
        {                                                                                                \
            static constexpr auto skConfig = config;                                                     \
        };                                                                                               \
        struct VCACurrentLoopDriver##name##HrtimTimerCTConfig : HrtimTimerCTDefaultConfig                \
        {                                                                                                \
            using Output1PinT = typename VCACurrentLoopDriverCTConfigT<id>::PWMOutput1PinT;              \
            using Output2PinT = typename VCACurrentLoopDriverCTConfigT<id>::PWMOutput2PinT;              \
        };                                                                                               \
        struct VCACurrentLoopDriver##name##AdcControllerCTConfig : AdcControllerCTDefaultConfig          \
        {                                                                                                \
            using DmaChannelT = typename VCACurrentLoopDriverCTConfigT<id>::CurrentSensorAdcDmaChannelT; \
        };                                                                                               \
    }                                                                                                    \
    VALLE_DEFINE_HRTIM_TIMER_CT_CONFIG(                                                                  \
        valle::platform::app::VCACurrentLoopDriverCTConfigT<id>::skVcaHrtimPwmControllerId,              \
        valle::platform::app::VCACurrentLoopDriverCTConfigT<id>::skVcaHrtimPwmTimerId,                   \
        valle::platform::app::VCACurrentLoopDriver##name##HrtimTimerCTConfig{});                         \
    VALLE_DEFINE_ADC_CONTROLLER_CT_CONFIG(                                                               \
        valle::platform::app::VCACurrentLoopDriverCTConfigT<id>::skCurrentSensorAdcControllerId,         \
        valle::platform::app::VCACurrentLoopDriver##name##AdcControllerCTConfig{});

    template <uint8_t tkControllerId>
    struct VCACurrentLoopDriverConfigRaw
    {
        using CTConfigTraitsT             = VCACurrentLoopDriverCTConfigTraits<tkControllerId>;
        using VCAControllerConfigT        = typename CTConfigTraitsT::VCAControllerConfigT;
        using HrtimTimerAdcTriggerConfigT = typename CTConfigTraitsT::HrtimTimerAdcTriggerConfigT;

        VCAControllerConfigT        vca_controller_config{};
        AdcControllerConfig         current_sensor_adc_controller_config{};
        ACS724ModuleConfig          current_sensor_config{};
        HrtimTimerAdcTriggerConfigT hrtim_adc_trigger_config{};
        AdcInterruptConfig          current_sensor_adc_interrupt_config{};

        [[nodiscard]] constexpr std::optional<std::string_view> validate(const RccConfig& rcc_config) const
        {
            const auto current_sensor_channel_sample_time = AdcTraits::calculate_channel_sample_time(
                AdcCommonConfig::get_source_clock_freq_hz<CTConfigTraitsT::skCurrentSensorAdcCommonId>(rcc_config),
                current_sensor_config.channel_config.sampling_time,
                current_sensor_adc_controller_config.oversampling.has_value()
                    ? std::optional<AdcOversamplingRatio>(current_sensor_adc_controller_config.oversampling->ratio)
                    : std::nullopt);

            const auto current_sample_period =
                DurationSecondsF(1.0F / static_cast<float>(vca_controller_config.half_bridge_config.freq_hz));
            const auto max_current_sensor_sample_time =
                current_sample_period -
                system::DurationMicros(12);  // Leave at least 12 µs margin for ADC conversion and processing

            if (current_sensor_channel_sample_time >= max_current_sensor_sample_time)
            {
                return "ADC sampling time must be less than PWM period for proper synchronization";
            }

            return std::nullopt;
        }
    };

    template <uint8_t tkControllerId>
    struct VCACurrentLoopDriverConfig
    {
        using CTConfigTraitsT = VCACurrentLoopDriverCTConfigTraits<tkControllerId>;

        using ValueT                               = typename CTConfigTraitsT::ValueT;
        using VCAControllerSystemControllerConfigT = typename CTConfigTraitsT::VCAControllerSystemControllerConfigT;
        using VCAControllerConfigT                 = typename CTConfigTraitsT::VCAControllerConfigT;
        using HrtimTimerAdcTriggerConfigT          = typename CTConfigTraitsT::HrtimTimerAdcTriggerConfigT;

        uint32_t pwm_freq_hz          = 60000U;  // 60 kHz PWM Frequency
        ValueT   max_current_amp      = 1.0F;    // Maximum current in Amps for control loop scaling
        ValueT   target_tolerance_amp = 0.001F;  // Target current tolerance in Amps
        uint32_t interrupt_priority   = 5;  // Interrupt priority for the HRTIM timer interrupt (0 = highest priority)
        LinearConverterConfig current_sensor_calibration{
            .scale  = 1.0F,  // Default to 1:1 scaling (no calibration)
            .offset = 0.0F,  // Default to zero offset (no calibration)
        };

        [[nodiscard]] constexpr VCACurrentLoopDriverConfigRaw<tkControllerId> to_raw() const
        {
            return VCACurrentLoopDriverConfigRaw<tkControllerId>{
                .vca_controller_config                = get_vca_controller_config(),
                .current_sensor_adc_controller_config = get_adc_controller_config(),
                .current_sensor_config                = get_current_sensor_config(),
                .hrtim_adc_trigger_config             = get_hrtim_adc_trigger_config(),
                .current_sensor_adc_interrupt_config  = get_adc_interrupt_config()};
        }

        [[nodiscard]] constexpr VCAControllerConfigT get_vca_controller_config() const
        {
            return VCAControllerConfigT{
                .half_bridge_config =
                    HrtimHalfBridgeDriverConfig{
                        .freq_hz          = pwm_freq_hz,
                        .repetition       = 1,
                        .rollover_mode    = HrtimTimerRolloverMode::kPeriodReset,
                        .interrupt_config = HrtimTimerInterruptConfig{.priority = interrupt_priority,
                                                                      .interrupts =
                                                                          HrtimTimerInterruptMask{
                                                                              .repetition = true,
                                                                          }},
                        .output_config =
                            HrtimHalfBridgeOutputConfig{
                                .polarity    = HrtimTimerOutputPolarity::kPositive,
                                .idle_mode   = HrtimTimerOutputIdleMode::kNoIdle,
                                .idle_level  = HrtimTimerOutputIdleLevel::kInactive,
                                .fault_state = HrtimTimerOutputFaultState::kInactive,
                                .gpio_config =
                                    HrtimTimerOutputGpioConfig{
                                        .speed = GpioPinSpeedMode::kLow,
                                        .pull  = GpioPinPullMode::kNoPull,
                                    },
                            },
                        .deadtime_config = HrtimTimerDeadTimeConfig{.rise_ns = 200.0F, .fall_ns = 200.0F},
                        .min_duty        = 0.05F,
                        .max_duty        = 0.95F,
                        .center_aligned  = true,
                        .compare_unit    = HrtimTimerCompareUnit::kCompare1,
                    },
                .controller_config = VCAControllerSystemControllerConfigT{
                    // Control loop at PWM frequency
                    .sample_time          = DurationSecondsF(1.0F / static_cast<float>(pwm_freq_hz)),
                    .max_current_amp      = max_current_amp,
                    .target_tolerance_amp = target_tolerance_amp}};
        }

        [[nodiscard]] constexpr AdcControllerConfig get_adc_controller_config() const
        {
            return AdcControllerConfig{
                .resolution     = AdcResolution::k12Bit,
                .data_alignment = AdcDataAlignment::kRight,
                .low_power      = AdcLowPowerMode::kNone,
                .inj =
                    AdcInjectGroupConfig{
                        .trigger_source            = CTConfigTraitsT::skCurrentSensorAdcHrtimTriggerSource,
                        .trigger_edge              = AdcInjectGroupTriggerEdge::kRising,
                        .auto_trigger_from_regular = false,
                    },
                .reg = {},
                .oversampling =
                    AdcOversamplingConfig{
                        .ratio = AdcOversamplingRatio::k8x,
                        .shift = AdcOversamplingShift::kDiv2,
                        .scope = AdcOversamplingScope::kInject,
                    },
            };
        }

        [[nodiscard]] constexpr AdcChannelConfig get_adc_channel_config() const
        {
            return AdcChannelConfig{
                .sampling_time = AdcChannelSampleTime::k24Cycles5,
                .input_mode    = AdcChannelInputMode::kSingleEnded,
                .offset        = std::nullopt,
            };
        }

        [[nodiscard]] constexpr ACS724ModuleConfig get_current_sensor_config() const
        {
            return ACS724ModuleConfig{.channel_config = get_adc_channel_config(),
                                      .calibration    = current_sensor_calibration};
        }

        [[nodiscard]] constexpr HrtimTimerAdcTriggerConfigT get_hrtim_adc_trigger_config() const
        {
            return HrtimTimerAdcTriggerConfigT{
                .source        = HrtimTimerAdcTriggerSourceId<CTConfigTraitsT::skVcaHrtimPwmTimerId,
                                                              CTConfigTraitsT::skCurrentSensorHrtimAdcTriggerId>::kPeriod,
                .rollover_mode = HrtimTimerAdcRolloverMode::kPeriodReset,
                .post_scaler   = 0,
            };
        }

        [[nodiscard]] constexpr AdcInterruptConfig get_adc_interrupt_config() const
        {
            return AdcInterruptConfig{
                .priority = interrupt_priority,
                .interrupts =
                    platform::AdcInterruptMask{
                        .inj_eos = true,
                    },
            };
        }
    };

    template <uint8_t tkControllerId>
    class VCACurrentLoopDriver
    {
    public:
        using CTConfigTraitsT = VCACurrentLoopDriverCTConfigTraits<tkControllerId>;

        using ValueT                            = typename CTConfigTraitsT::ValueT;
        using VCAHrtimPwmControllerDeviceT      = typename CTConfigTraitsT::VCAHrtimPwmControllerDeviceT;
        using VCAHrtimPwmTimerDeviceT           = typename CTConfigTraitsT::VCAHrtimPwmTimerDeviceT;
        using VCAControllerT                    = typename CTConfigTraitsT::VCAControllerT;
        using CurrentSensorAdcControllerDeviceT = typename CTConfigTraitsT::CurrentSensorAdcControllerDeviceT;
        using CurrentSensorAdcChannelDeviceT    = typename CTConfigTraitsT::CurrentSensorAdcChannelDeviceT;
        using CurrentSensorT                    = typename CTConfigTraitsT::CurrentSensorT;

        static constexpr auto skVcaHrtimPwmControllerId      = CTConfigTraitsT::skVcaHrtimPwmControllerId;
        static constexpr auto skVcaHrtimPwmTimerId           = CTConfigTraitsT::skVcaHrtimPwmTimerId;
        static constexpr auto skCurrentSensorAdcControllerId = CTConfigTraitsT::skCurrentSensorAdcControllerId;
        static constexpr auto skCurrentSensorAdcChannelId    = CTConfigTraitsT::skCurrentSensorAdcChannelId;

        using ConfigT    = VCACurrentLoopDriverConfig<tkControllerId>;
        using RawConfigT = VCACurrentLoopDriverConfigRaw<tkControllerId>;

        using InjectDevices = TypeList<VCAHrtimPwmControllerDeviceT,
                                       VCAHrtimPwmTimerDeviceT,
                                       CurrentSensorAdcControllerDeviceT,
                                       CurrentSensorAdcChannelDeviceT>;

    private:
        [[no_unique_address]] DeviceRef<VCAHrtimPwmControllerDeviceT>      m_hrtim_controller;
        VCAControllerT                                                     m_vca_controller;
        [[no_unique_address]] DeviceRef<CurrentSensorAdcControllerDeviceT> m_adc_controller;
        CurrentSensorT                                                     m_current_sensor;

        static constexpr float skSampleSwitchLower = 0.4F;  // switch to PERIOD only if duty < 0.4
        static constexpr float skSampleSwitchUpper = 0.6F;  // switch to RESET only if duty > 0.6

        Atomic<float> m_last_duty_cycle = 0.5F;
        Atomic<bool>  m_reset_triggered = false;
        bool          m_sample_on_reset = false;  // true = sample on RESET, false = PERIOD

    public:
        VCACurrentLoopDriver() = delete;

        VCACurrentLoopDriver(DeviceRef<VCAHrtimPwmControllerDeviceT>&&      hrtim_controller_device,
                             DeviceRef<VCAHrtimPwmTimerDeviceT>&&           hrtim_timer_device,
                             DeviceRef<CurrentSensorAdcControllerDeviceT>&& adc_controller_device,
                             DeviceRef<CurrentSensorAdcChannelDeviceT>&&    adc_channel_device)
            : m_hrtim_controller(std::move(hrtim_controller_device))
            , m_vca_controller(std::move(hrtim_timer_device))
            , m_adc_controller(std::move(adc_controller_device))
            , m_current_sensor(std::move(adc_channel_device))
        {
        }

        [[nodiscard]] bool init(const ConfigT& config)
        {
            return init(config.to_raw());
        }

        [[nodiscard]] bool init(const RawConfigT& config)
        {
            expect(m_hrtim_controller->init(), "Failed to initialize HRTIM Controller");
            expect(m_vca_controller.init(config.vca_controller_config), "Failed to initialize VCA controller");

            expect(m_adc_controller->init(), "Failed to initialize Current Sensor ADC Controller");
            expect(m_current_sensor.init(config.current_sensor_config),
                   "Failed to initialize ACS724 Current Sensor Driver");

            expect(m_vca_controller.get_half_bridge().init_adc_trigger(config.hrtim_adc_trigger_config),
                   "Failed to initialize HRTIM ADC Trigger");

            expect(m_adc_controller->post_init(config.current_sensor_adc_controller_config),
                   "Failed to post initialize Current Sensor ADC Controller");

            m_adc_controller->enable_interrupts(config.current_sensor_adc_interrupt_config);

            return true;
        }

        VCAControllerT& get_vca_controller()
        {
            return m_vca_controller;
        }

        CurrentSensorT& get_current_sensor()
        {
            return m_current_sensor;
        }

        // ---------------------------------------------------------------------
        // CONTROL
        // ---------------------------------------------------------------------

        void start()
        {
            m_adc_controller->start_inject();
            m_vca_controller.enable();
        }

        void stop()
        {
            m_vca_controller.disable();
        }

        void set_target_current_amps(const float target_current_amps)
        {
            m_vca_controller.set_target_current_amps(target_current_amps);
        }

        [[nodiscard]] float get_feedback_current_amps()
        {
            return m_current_sensor.read_amps();
        }

        [[nodiscard]] float get_target_current_amps()
        {
            return m_vca_controller.get_target_current_amps();
        }

        // ---------------------------------------------------------------------
        // ISR HANDLERS
        // ---------------------------------------------------------------------

        bool on_current_sensor_adc_reading_isr()
        {
            const bool current_event_reset = m_reset_triggered.load(std::memory_order_acquire);
            if (current_event_reset)
            {
                m_reset_triggered.store(false, std::memory_order_release);
            }

            const float last_duty = m_last_duty_cycle.load(std::memory_order_relaxed);
            // --- update edge choice with hysteresis ---
            if (last_duty > skSampleSwitchUpper)
            {
                m_sample_on_reset = true;
            }
            else if (last_duty < skSampleSwitchLower)
            {
                m_sample_on_reset = false;
            }

            // only process sample if event matches chosen edge
            if (m_sample_on_reset != current_event_reset)
            {
                return false;
            }

            // First read the sampled current sensor and convert
            m_current_sensor.get_adc().on_data_available();

            // Then run VCA control loop
            m_last_duty_cycle.store(m_vca_controller.run_ctrl_loop(get_feedback_current_amps()),
                                    std::memory_order_relaxed);
            return true;
        }

        void on_pwm_repetition_isr()
        {
            m_reset_triggered.store(true, std::memory_order_release);
        }

        float get_last_duty_cycle() const
        {
            return m_last_duty_cycle.load(std::memory_order_relaxed);
        }
    };

    template <uint8_t tkControllerId>
    constexpr auto kDefaultVcaCurrentLoopDriverConfig = VCACurrentLoopDriverConfig<tkControllerId>{
        .pwm_freq_hz          = 60000U,
        .max_current_amp      = 0.3F,
        .target_tolerance_amp = 0.001F,
        .interrupt_priority   = 5,
        .current_sensor_calibration =
            LinearConverterConfig{
                .scale  = 1.13026F,
                .offset = -0.266701F,
            },
    };

}  // namespace valle::platform::app

#define VALLE_BIND_VCA_CURRENT_LOOP_DRIVER_ISR_ROUTERS(instance)                                       \
    namespace valle::platform                                                                          \
    {                                                                                                  \
        template <>                                                                                    \
        struct AdcIsrRouter<std::remove_cvref_t<decltype(instance)>::skCurrentSensorAdcControllerId,   \
                            AdcInterruptSource::kInjectEndOfSequence>                                  \
        {                                                                                              \
            static void handle()                                                                       \
            {                                                                                          \
                if (instance.on_current_sensor_adc_reading_isr())                                      \
                {                                                                                      \
                }                                                                                      \
            }                                                                                          \
        };                                                                                             \
        template <>                                                                                    \
        struct HrtimTimerIsrRouter<std::remove_cvref_t<decltype(instance)>::skVcaHrtimPwmControllerId, \
                                   std::remove_cvref_t<decltype(instance)>::skVcaHrtimPwmTimerId,      \
                                   HrtimTimerInterruptSource::kRepetition>                             \
        {                                                                                              \
            static void handle()                                                                       \
            {                                                                                          \
                instance.on_pwm_repetition_isr();                                                      \
            }                                                                                          \
        };                                                                                             \
    }  // namespace valle::platform
