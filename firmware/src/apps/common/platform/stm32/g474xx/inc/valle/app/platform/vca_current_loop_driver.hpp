#pragma once

#include "valle/app/platform/core_system_config.hpp"
#include "valle/app/platform/modules/acs724.hpp"
#include "valle/app/platform/modules/vca.hpp"
#include "valle/base/panic.hpp"
#include "valle/platform/hardware/common.hpp"
#include "valle/system/timing.hpp"
#include "valle/utils/atomic.hpp"

namespace valle::platform::app
{
    struct VCACurrentLoopDriverDefaultCTConfig
    {
        using PWMOutput1PinT              = GPIOPinA8Device;
        using PWMOutput2PinT              = GPIOPinA9Device;
        using CurrentSensorADCDMAChannelT = DMA1Channel2Device;

        static constexpr HRTIMControllerID skVCAHRTIMPWMControllerID = 1;
        static constexpr HRTIMTimerID      skVCAHRTIMPWMTimerID      = HRTIMTimerID::kA;

        static constexpr ADCControllerID skCurrentSensorADCControllerID = 1;
        static constexpr ADCChannelID    skCurrentSensorADCChannelId    = ADCChannelID::kChannel1;

        static constexpr auto skCurrentSensorModel                 = valle::app::ACS724Model::k2P5ABi;
        static constexpr auto skCurrentSensorADCHRTIMTriggerSource = ADCInjectGroupTriggerSource::kExtHrtimTRG2;
    };

    template <uint8_t tkID>
    struct VCACurrentLoopDriverCTConfigRegistry
    {
        static constexpr auto skConfig = VCACurrentLoopDriverDefaultCTConfig{};
    };

    template <uint8_t tkID>
    using VCACurrentLoopDriverCTConfigT = decltype(VCACurrentLoopDriverCTConfigRegistry<tkID>::skConfig);

    template <uint8_t tkID>
    struct VCACurrentLoopDriverCTConfigTraits : public VCACurrentLoopDriverCTConfigT<tkID>
    {
        using BaseT = VCACurrentLoopDriverCTConfigT<tkID>;

        using ValueT                               = float;
        using VCAControllerSystemControllerT       = valle::app::VCAClosedLoopCurrentFeedbackController<ValueT>;
        using VCAControllerSystemControllerConfigT = typename VCAControllerSystemControllerT::ConfigT;
        using VCAControllerConfigT = VCAControllerHRTIMModuleConfig<VCAControllerSystemControllerConfigT>;

        using VCAHRTIMPWMControllerDeviceT = HRTIMControllerDevice<BaseT::skVCAHRTIMPWMControllerID>;
        using VCAHRTIMPWMTimerDeviceT = HRTIMTimerDevice<BaseT::skVCAHRTIMPWMControllerID, BaseT::skVCAHRTIMPWMTimerID>;
        using VCAControllerT = VCAControllerHRTIMModule<VCAHRTIMPWMTimerDeviceT, VCAControllerSystemControllerT>;

        static constexpr auto skCurrentSensorADCCommonID =
            BaseT::skCurrentSensorADCControllerID <= 2 ? ADCCommonID::kADC12 : ADCCommonID::kADC345;

        using CurrentSensorADCControllerDeviceT = ADCControllerDevice<BaseT::skCurrentSensorADCControllerID>;
        using CurrentSensorADCChannelDeviceT =
            ADCInjectChannelRank1Device<BaseT::skCurrentSensorADCControllerID, BaseT::skCurrentSensorADCChannelId>;
        using CurrentSensorT = ACS724Module<CurrentSensorADCChannelDeviceT, BaseT::skCurrentSensorModel>;

        static constexpr auto skCurrentSensorHRTIMADCTriggerID =
            adc_trigger_to_hrtim_trigger(BaseT::skCurrentSensorADCHRTIMTriggerSource);

        using HRTIMTimerADCTriggerConfigT =
            HRTIMTimerADCTriggerConfig<BaseT::skVCAHRTIMPWMTimerID, skCurrentSensorHRTIMADCTriggerID>;
    };

#define VALLE_DEFINE_VCA_CURRENT_LOOP_DRIVER_CT_CONFIG(name, id, config)                                 \
    namespace valle::platform::app                                                                       \
    {                                                                                                    \
        template <>                                                                                      \
        struct VCACurrentLoopDriverCTConfigRegistry<id>                                                  \
        {                                                                                                \
            static constexpr auto skConfig = config;                                                     \
        };                                                                                               \
        struct VCACurrentLoopDriver##name##HRTIMTimerCTConfig : HRTIMTimerCTDefaultConfig                \
        {                                                                                                \
            using Output1PinT = typename VCACurrentLoopDriverCTConfigT<id>::PWMOutput1PinT;              \
            using Output2PinT = typename VCACurrentLoopDriverCTConfigT<id>::PWMOutput2PinT;              \
        };                                                                                               \
        struct VCACurrentLoopDriver##name##ADCControllerCTConfig : ADCControllerCTDefaultConfig          \
        {                                                                                                \
            using DMAChannelT = typename VCACurrentLoopDriverCTConfigT<id>::CurrentSensorADCDMAChannelT; \
        };                                                                                               \
    }                                                                                                    \
    VALLE_DEFINE_HRTIM_TIMER_CT_CONFIG(                                                                  \
        valle::platform::app::VCACurrentLoopDriverCTConfigT<id>::skVCAHRTIMPWMControllerID,              \
        valle::platform::app::VCACurrentLoopDriverCTConfigT<id>::skVCAHRTIMPWMTimerID,                   \
        valle::platform::app::VCACurrentLoopDriver##name##HRTIMTimerCTConfig{});                         \
    VALLE_DEFINE_ADC_CONTROLLER_CT_CONFIG(                                                               \
        valle::platform::app::VCACurrentLoopDriverCTConfigT<id>::skCurrentSensorADCControllerID,         \
        valle::platform::app::VCACurrentLoopDriver##name##ADCControllerCTConfig{});

    template <uint8_t tkID>
    struct VCACurrentLoopDriverConfigRaw
    {
        using CTConfigTraitsT             = VCACurrentLoopDriverCTConfigTraits<tkID>;
        using VCAControllerConfigT        = typename CTConfigTraitsT::VCAControllerConfigT;
        using HRTIMTimerADCTriggerConfigT = typename CTConfigTraitsT::HRTIMTimerADCTriggerConfigT;

        VCAControllerConfigT        vca_controller_config{};
        ADCControllerConfig         current_sensor_adc_controller_config{};
        ACS724ModuleConfig          current_sensor_config{};
        HRTIMTimerADCTriggerConfigT hrtim_adc_trigger_config{};
        ADCInterruptConfig          current_sensor_adc_interrupt_config{};

        [[nodiscard]] constexpr std::optional<std::string_view> validate(const RCCConfig& rcc_config) const
        {
            const auto current_sensor_channel_sample_time = ADCRootInterface::calculate_channel_sample_time(
                ADCCommonConfigTraits<CTConfigTraitsT::skCurrentSensorADCCommonID>::get_source_clock_freq_hz(
                    rcc_config),
                current_sensor_config.channel_config.sampling_time,
                current_sensor_adc_controller_config.oversampling.has_value()
                    ? std::optional<ADCOversamplingRatio>(current_sensor_adc_controller_config.oversampling->ratio)
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

    template <uint8_t tkID>
    struct VCACurrentLoopDriverConfig
    {
        using CTConfigTraitsT = VCACurrentLoopDriverCTConfigTraits<tkID>;

        using ValueT                               = typename CTConfigTraitsT::ValueT;
        using VCAControllerSystemControllerConfigT = typename CTConfigTraitsT::VCAControllerSystemControllerConfigT;
        using VCAControllerConfigT                 = typename CTConfigTraitsT::VCAControllerConfigT;
        using HRTIMTimerADCTriggerConfigT          = typename CTConfigTraitsT::HRTIMTimerADCTriggerConfigT;

        uint32_t pwm_freq_hz          = 60000U;  // 60 kHz PWM Frequency
        ValueT   max_current_amp      = 1.0F;    // Maximum current in Amps for control loop scaling
        ValueT   target_tolerance_amp = 0.001F;  // Target current tolerance in Amps
        uint32_t interrupt_priority   = 5;  // Interrupt priority for the HRTIM timer interrupt (0 = highest priority)
        LinearConverterConfig current_sensor_calibration{
            .scale  = 1.0F,  // Default to 1:1 scaling (no calibration)
            .offset = 0.0F,  // Default to zero offset (no calibration)
        };

        [[nodiscard]] constexpr VCACurrentLoopDriverConfigRaw<tkID> to_raw() const
        {
            return VCACurrentLoopDriverConfigRaw<tkID>{
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
                    HRTIMHalfBridgeDriverConfig{
                        .freq_hz          = pwm_freq_hz,
                        .repetition       = 1,
                        .rollover_mode    = HRTIMTimerRolloverMode::kPeriodReset,
                        .interrupt_config = HRTIMTimerInterruptConfig{.priority = interrupt_priority,
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
                .controller_config = VCAControllerSystemControllerConfigT{
                    // Control loop at PWM frequency
                    .sample_time          = DurationSecondsF(1.0F / static_cast<float>(pwm_freq_hz)),
                    .max_current_amp      = max_current_amp,
                    .target_tolerance_amp = target_tolerance_amp}};
        }

        [[nodiscard]] constexpr ADCControllerConfig get_adc_controller_config() const
        {
            return ADCControllerConfig{
                .resolution     = ADCResolution::k12Bit,
                .data_alignment = ADCDataAlignment::kRight,
                .low_power      = ADCLowPowerMode::kNone,
                .inj =
                    ADCInjectGroupConfig{
                        .trigger_source            = CTConfigTraitsT::skCurrentSensorADCHRTIMTriggerSource,
                        .trigger_edge              = ADCInjectGroupTriggerEdge::kRising,
                        .auto_trigger_from_regular = false,
                    },
                .reg = {},
                .oversampling =
                    ADCOversamplingConfig{
                        .ratio = ADCOversamplingRatio::k8x,
                        .shift = ADCOversamplingShift::kDiv2,
                        .scope = ADCOversamplingScope::kInject,
                    },
            };
        }

        [[nodiscard]] constexpr ADCChannelConfig get_adc_channel_config() const
        {
            return ADCChannelConfig{
                .sampling_time = ADCChannelSampleTime::k24Cycles5,
                .input_mode    = ADCChannelInputMode::kSingleEnded,
                .offset        = std::nullopt,
            };
        }

        [[nodiscard]] constexpr ACS724ModuleConfig get_current_sensor_config() const
        {
            return ACS724ModuleConfig{.channel_config = get_adc_channel_config(),
                                      .calibration    = current_sensor_calibration};
        }

        [[nodiscard]] constexpr HRTIMTimerADCTriggerConfigT get_hrtim_adc_trigger_config() const
        {
            return HRTIMTimerADCTriggerConfigT{
                .source        = HRTIMTimerADCTriggerSourceID<CTConfigTraitsT::skVCAHRTIMPWMTimerID,
                                                              CTConfigTraitsT::skCurrentSensorHRTIMADCTriggerID>::kPeriod,
                .rollover_mode = HRTIMTimerADCRolloverMode::kPeriodReset,
                .post_scaler   = 0,
            };
        }

        [[nodiscard]] constexpr ADCInterruptConfig get_adc_interrupt_config() const
        {
            return ADCInterruptConfig{
                .priority = interrupt_priority,
                .interrupts =
                    platform::ADCInterruptMask{
                        .inj_eos = true,
                    },
            };
        }
    };

    template <uint8_t tkID>
    class VCACurrentLoopDriver
    {
    public:
        using CTConfigTraitsT = VCACurrentLoopDriverCTConfigTraits<tkID>;

        using ValueT                            = typename CTConfigTraitsT::ValueT;
        using VCAHRTIMPWMControllerDeviceT      = typename CTConfigTraitsT::VCAHRTIMPWMControllerDeviceT;
        using VCAHRTIMPWMTimerDeviceT           = typename CTConfigTraitsT::VCAHRTIMPWMTimerDeviceT;
        using VCAControllerT                    = typename CTConfigTraitsT::VCAControllerT;
        using CurrentSensorADCControllerDeviceT = typename CTConfigTraitsT::CurrentSensorADCControllerDeviceT;
        using CurrentSensorADCChannelDeviceT    = typename CTConfigTraitsT::CurrentSensorADCChannelDeviceT;
        using CurrentSensorT                    = typename CTConfigTraitsT::CurrentSensorT;

        static constexpr auto skVCAHRTIMPWMControllerID      = CTConfigTraitsT::skVCAHRTIMPWMControllerID;
        static constexpr auto skVCAHRTIMPWMTimerID           = CTConfigTraitsT::skVCAHRTIMPWMTimerID;
        static constexpr auto skCurrentSensorADCControllerID = CTConfigTraitsT::skCurrentSensorADCControllerID;
        static constexpr auto skCurrentSensorADCChannelId    = CTConfigTraitsT::skCurrentSensorADCChannelId;

        using ConfigT    = VCACurrentLoopDriverConfig<tkID>;
        using RawConfigT = VCACurrentLoopDriverConfigRaw<tkID>;

        using InjectDevices = TypeList<VCAHRTIMPWMControllerDeviceT,
                                       VCAHRTIMPWMTimerDeviceT,
                                       CurrentSensorADCControllerDeviceT,
                                       CurrentSensorADCChannelDeviceT>;

    private:
        [[no_unique_address]] DeviceRef<VCAHRTIMPWMControllerDeviceT>      m_hrtim_controller;
        VCAControllerT                                                     m_vca_controller;
        [[no_unique_address]] DeviceRef<CurrentSensorADCControllerDeviceT> m_adc_controller;
        CurrentSensorT                                                     m_current_sensor;

        static constexpr float skSampleSwitchLower = 0.4F;  // switch to PERIOD only if duty < 0.4
        static constexpr float skSampleSwitchUpper = 0.6F;  // switch to RESET only if duty > 0.6

        Atomic<float> m_last_duty_cycle = 0.5F;
        Atomic<bool>  m_reset_triggered = false;
        bool          m_sample_on_reset = false;  // true = sample on RESET, false = PERIOD

    public:
        VCACurrentLoopDriver() = delete;

        VCACurrentLoopDriver(DeviceRef<VCAHRTIMPWMControllerDeviceT>&&      hrtim_controller_device,
                             DeviceRef<VCAHRTIMPWMTimerDeviceT>&&           hrtim_timer_device,
                             DeviceRef<CurrentSensorADCControllerDeviceT>&& adc_controller_device,
                             DeviceRef<CurrentSensorADCChannelDeviceT>&&    adc_channel_device)
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

    template <uint8_t tkID>
    constexpr auto kDefaultVCACurrentLoopDriverConfig = VCACurrentLoopDriverConfig<tkID>{
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
        struct ADCISRRouter<std::remove_cvref_t<decltype(instance)>::skCurrentSensorADCControllerID,   \
                            ADCInterruptType::kInjectEndOfSequence>                                    \
        {                                                                                              \
            static void handle()                                                                       \
            {                                                                                          \
                if (instance.on_current_sensor_adc_reading_isr())                                      \
                {                                                                                      \
                }                                                                                      \
            }                                                                                          \
        };                                                                                             \
        template <>                                                                                    \
        struct HRTIMTimerISRRouter<std::remove_cvref_t<decltype(instance)>::skVCAHRTIMPWMControllerID, \
                                   std::remove_cvref_t<decltype(instance)>::skVCAHRTIMPWMTimerID,      \
                                   HRTIMTimerInterruptType::kRepetition>                               \
        {                                                                                              \
            static void handle()                                                                       \
            {                                                                                          \
                instance.on_pwm_repetition_isr();                                                      \
            }                                                                                          \
        };                                                                                             \
    }  // namespace valle::platform
