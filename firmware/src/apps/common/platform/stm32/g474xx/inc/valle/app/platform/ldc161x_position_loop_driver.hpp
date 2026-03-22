#pragma once

#include "valle/app/platform/modules/ldc161x.hpp"
#include "valle/math/system/blocks/feedback.hpp"
#include "valle/math/system/blocks/pid.hpp"
#include "valle/system/sync.hpp"
#include "valle/system/timing.hpp"
#include "valle/utils/atomic.hpp"

namespace valle::platform::app
{
    // =============================================================================
    // FEEDBACK CONTROLLERS
    // =============================================================================

    template <std::floating_point T>
    struct LDC161XVCAValvePositionControllerConfig
    {
        using ValueT = T;

        std::chrono::duration<ValueT> sample_time =
            std::chrono::duration<ValueT>(0.0F);  // Control loop sample time in seconds (e.g., 1 ms)
        ValueT max_current_amp = 0.0F;            // Max current limit (for safety)
    };

    template <std::floating_point T>
    class LDC161XVCAValvePositionController : public ISystemBlock<LDC161XVCAValvePositionController<T>, T>
    {
    public:
        using ValueT  = T;
        using ConfigT = LDC161XVCAValvePositionControllerConfig<ValueT>;

    private:
        constexpr static T skPidP = static_cast<T>(5.75);  // 5.75
        constexpr static T skPidI = static_cast<T>(0.0);
        constexpr static T skPidD = static_cast<T>(0.0);

        PIDSystem<ValueT> system{};

    public:
        LDC161XVCAValvePositionController() = default;

        explicit LDC161XVCAValvePositionController(const ConfigT& config)
            : system(skPidP, skPidI, skPidD, config.sample_time, -config.max_current_amp, config.max_current_amp)
        {
        }

        ValueT process_impl(const ValueT reference)
        {
            return system.process(reference);
        }

        void reset_impl()
        {
            system.reset();
        }
    };

    template <std::floating_point T>
    struct LDC161XVCAValvePositionClosedLoopControllerConfig
    {
        using ValueT = T;

        std::chrono::duration<ValueT> sample_time =
            std::chrono::duration<ValueT>(0.0F);  // Control loop sample time in seconds (e.g., 1 ms)
        ValueT max_current_amp     = 0.0F;        // Max current limit (for safety)
        ValueT target_tolerance_mm = 0.0F;  // Target position tolerance in millimeters (used for determining when to
                                            // consider the valve "at position" for control purposes)
        ValueT min_position_mm = 0.0F;      // Minimum valve position in millimeters (fully closed)
        ValueT max_position_mm = 0.0F;      // Maximum valve position in millimeters (fully open)
    };

    template <std::floating_point T>
    class LDC161XVCAValvePositionClosedLoopController
        : public ISystemBlock<LDC161XVCAValvePositionClosedLoopController<T>, T>
    {
    public:
        using ValueT              = T;
        using ConfigT             = LDC161XVCAValvePositionClosedLoopControllerConfig<ValueT>;
        using OpenLoopControllerT = LDC161XVCAValvePositionController<ValueT>;
        using FeedbackSystemT     = ExFeedbackSystem2<OpenLoopControllerT>;

    private:
        FeedbackSystemT m_feedback_system{};  /// Feedback System
        ValueT          m_min_position_mm{};  /// Minimum valve position in millimeters
        ValueT          m_max_position_mm{};  /// Maximum valve position in millimeters

    public:
        LDC161XVCAValvePositionClosedLoopController() = default;

        explicit LDC161XVCAValvePositionClosedLoopController(const ConfigT& config)
            : m_feedback_system(OpenLoopControllerT(LDC161XVCAValvePositionControllerConfig<ValueT>{
                                    .sample_time = config.sample_time, .max_current_amp = config.max_current_amp}),
                                config.target_tolerance_mm)
            , m_min_position_mm(config.min_position_mm)
            , m_max_position_mm(config.max_position_mm)
        {
        }

        ValueT process_impl(const ValueT reference)
        {
            return m_feedback_system.process(std::clamp(reference, m_min_position_mm, m_max_position_mm));
        }

        void set_feedback(const ValueT feedback)
        {
            m_feedback_system.set_feedback(feedback);
        }

        void reset_impl()
        {
            m_feedback_system.reset();
        }
    };

    // =============================================================================
    // VCA VALVE INTERFACE
    // =============================================================================
    template <typename T>
    concept CLDC161XPositionLoopDriverInterface =
        requires(T controller, typename T::ValueT reference, typename T::ValueT feedback) {
            { controller.run(reference, feedback) } -> std::same_as<void>;
            { controller.reset() } -> std::same_as<void>;
        };

    template <typename TVCAController>
    struct LDC161XPositionLoopDriverVCAValveInterfaceConfig
    {
        using VCAControllerT = TVCAController;
        using ValueT         = typename VCAControllerT::ValueT;

        std::reference_wrapper<TVCAController>                    vca_controller;
        LDC161XVCAValvePositionClosedLoopControllerConfig<ValueT> controller_config{};
    };

    template <typename TVCAController>
    class LDC161XPositionLoopDriverVCAValveInterface
    {
    public:
        using VCAControllerT = TVCAController;
        using ValueT         = typename VCAControllerT::ValueT;
        using ControllerT    = LDC161XVCAValvePositionClosedLoopController<ValueT>;

        using ConfigT = LDC161XPositionLoopDriverVCAValveInterfaceConfig<VCAControllerT>;

    private:
        std::reference_wrapper<VCAControllerT> m_vca;
        ControllerT                            m_controller{};

    public:
        LDC161XPositionLoopDriverVCAValveInterface() = delete;

        explicit LDC161XPositionLoopDriverVCAValveInterface(const ConfigT& config)
            : m_vca(config.vca_controller), m_controller(config.controller_config)
        {
        }

        // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
        void run(const ValueT reference, const ValueT feedback)
        {
            m_controller.set_feedback(feedback);
            const float target_current_amps = m_controller.process(reference);
            m_vca.get().set_target_current_amps(target_current_amps);
        }

        void reset()
        {
            m_controller.reset();
        }
    };

    // =============================================================================
    // POSITION CONTROLLER MODULE
    // =============================================================================

    template <typename TLDC161XSensorModuleConfig, typename TConverterConfig, typename... TControllerConfigs>
    class LDC161XPositionLoopDriverConfig
    {
    public:
        using SensorModuleConfigT              = TLDC161XSensorModuleConfig;
        static constexpr uint8_t skNumChannels = SensorModuleConfigT::skNumChannels;
        static_assert(skNumChannels == sizeof...(TControllerConfigs),
                      "Number of controller configs must match the number of sensor channels");

        SensorModuleConfigT                         ldc161x_config{};
        std::array<TConverterConfig, skNumChannels> converter_configs{};
        std::tuple<TControllerConfigs...>           controller_interface_configs{};
    };

    template <typename TLDC161XSensorModule, typename TConverter, CLDC161XPositionLoopDriverInterface... TControllers>
    class LDC161XPositionLoopDriver
    {
    public:
        using SensorModuleT    = TLDC161XSensorModule;
        using ConverterT       = TConverter;
        using ControllerTupleT = std::tuple<TControllers...>;

        using SensorModuleConfigT                = typename SensorModuleT::ConfigT;
        using ReadCallbackResultT                = typename SensorModuleT::ReadCallbackResultT;
        using INTBPinT                           = typename SensorModuleT::I2CInterfaceT::INTBPinT;
        static constexpr uint8_t skNumChannels   = SensorModuleT::skNumChannels;
        static constexpr bool    skSingleChannel = skNumChannels == 1;

        using ConverterConfigT = typename ConverterT::ConfigT;

        using ConfigT =
            LDC161XPositionLoopDriverConfig<SensorModuleConfigT, ConverterConfigT, typename TControllers::ConfigT...>;

        using InjectDevices = typename GetInjectDevices<SensorModuleT>::type;
        using DependDevices = typename GetAdditionalDependDevices<SensorModuleT>::type;

    private:
        SensorModuleT                              m_sensor{};
        std::array<ConverterT, skNumChannels>      m_converters{};
        std::tuple<std::optional<TControllers>...> m_controllers{};
        Atomic<bool>                               m_active                = false;
        std::array<Atomic<float>, skNumChannels>   m_setpoints_mm          = {};
        DurationMillisF                            m_manual_read_period_ms = DurationMillisF(100.0F);

        valle::system::SynchronizedCriticalSection<valle::system::TimePointMillis> m_last_read_time{};

    public:
        template <typename... TArgs>
        explicit LDC161XPositionLoopDriver(TArgs&&... args)
            : m_sensor(std::forward<TArgs>(args)...), m_active(false), m_setpoints_mm{}
        {
        }

        [[nodiscard]] bool init(const ConfigT& config)
        {
            if (!m_sensor.init(config.ldc161x_config))
            {
                return false;
            }

            m_sensor.set_async_read_callback([this](const auto& result)
                                             { position_sensor_async_read_callback(result); });

            for (size_t i = 0; i < skNumChannels; ++i)
            {
                m_converters[i] = ConverterT(config.converter_configs[i]);
            }

            tuple_for_each_zip(m_controllers,
                               config.controller_interface_configs,
                               [](auto& controller_opt, const auto& controller_config)
                               { controller_opt.emplace(controller_config); });

            const float sample_period_ms =
                1000.0F / static_cast<float>(config.ldc161x_config.sensor_config.sample_rate_hz);
            m_manual_read_period_ms = DurationMillisF(std::ceil(sample_period_ms * 2.0F));  // 2x the sample period

            return true;
        }

        [[nodiscard]] bool start()
        {
            m_active.store(true, std::memory_order_relaxed);
            return trigger_read();
        }

        void stop()
        {
            m_active.store(false, std::memory_order_relaxed);
        }

        template <uint8_t tkChannelIndex>
        void set_target_position_mm(const float position_mm)
        {
            m_setpoints_mm[tkChannelIndex].store(position_mm, std::memory_order_relaxed);
        }

        template <uint8_t tkChannelIndex>
        [[nodiscard]] float get_target_position_mm() const
        {
            return m_setpoints_mm[tkChannelIndex].load(std::memory_order_relaxed);
        }

        [[nodiscard]] SensorModuleT& sensor()
        {
            return m_sensor;
        }

        [[nodiscard]] const SensorModuleT& sensor() const
        {
            return m_sensor;
        }

        [[nodiscard]] bool trigger_read()
        {
            if constexpr (skSingleChannel)
            {
                if (!m_sensor.read_status_frequency_mhz_async())
                {
                    return false;
                }
            }
            else
            {
                if (!m_sensor.read_status_frequency_mhz_multi_async())
                {
                    return false;
                }
            }

            m_last_read_time.with_lock([](auto& last_read_time)
                                       { last_read_time = valle::system::MillisClock::now(); });

            return true;
        }

        void monitor_read_timeout_1khz()
        {
            const auto duration_since_last_read = m_last_read_time.with_lock(
                [](const auto& last_read_time) { return valle::system::MillisClock::now() - last_read_time; });

            if (duration_since_last_read > m_manual_read_period_ms)
            {
                (void)trigger_read();
            }
        }

        // ----------------------------------------------------------------------------------------
        // ISRs and CALLBACKS
        // ----------------------------------------------------------------------------------------
        void position_sensor_async_read_callback(const ReadCallbackResultT& result)
        {
            if (!m_active.load(std::memory_order_relaxed))
            {
                return;
            }

            if constexpr (skSingleChannel)  // NOLINT(bugprone-branch-clone)
            {
                std::visit(Overloaded{[this](const SensorModuleT::ReadStatusFrequencyCallbackResultT& result)
                                      { on_channel_data<valle::app::LDC161XChannel::kChannel0>(result.data); },
                                      [](const auto&) { /* Handle other callback result types if necessary */ }},
                           result);
            }
            else
            {
                std::visit(
                    Overloaded{[this](const SensorModuleT::ReadStatusFrequencyMultiCallbackResultT& result)
                               { handle_multichannel_read_impl(result, std::make_index_sequence<skNumChannels>{}); },
                               [](const auto&) { /* Handle other callback result types if necessary */ }},
                    result);
            }
        }

        void intb_asserted_isr()
        {
            if (!m_active.load(std::memory_order_relaxed))
            {
                return;
            }

            (void)trigger_read();
        }

    private:
        template <std::size_t... I>
        void handle_multichannel_read_impl(const SensorModuleT::ReadStatusFrequencyMultiCallbackResultT& result,
                                           std::index_sequence<I...>)
        {
            (on_channel_data<valle::app::LDC161XTraits::skChannelFromChannelIndex<I>>(result.data[I]), ...);
        }

        template <valle::app::LDC161XChannel tkChannel>
        void on_channel_data(const valle::app::LDC161XDataFrequency& data)
        {
            const uint8_t channel_index  = valle::app::LDC161XTraits::skChannelIndexFromChannel<tkChannel>;
            auto&         controller_opt = std::get<channel_index>(m_controllers);

            expect(controller_opt.has_value(), "Controller for channel does not have a value");

            // Convert frequency to position
            const float reference_mm         = m_setpoints_mm[channel_index].load(std::memory_order_relaxed);
            const float feedback_position_mm = m_converters[channel_index].convert(data.frequency_mhz);

            // Update controller
            controller_opt->run(reference_mm, feedback_position_mm);
        }
    };
}  // namespace valle::platform::app

#define VALLE_BIND_LDC161X_VALVE_POSITION_CONTROLLER_INTB_ISR_ROUTER(instance)               \
    namespace valle::platform                                                                \
    {                                                                                        \
        template <>                                                                          \
        struct GPIOPinISRRouter<std::remove_cvref_t<decltype(instance)>::INTBPinT::skPortID, \
                                std::remove_cvref_t<decltype(instance)>::INTBPinT::skPinID>  \
        {                                                                                    \
            static void handle()                                                             \
            {                                                                                \
                (instance).intb_asserted_isr();                                              \
            }                                                                                \
        };                                                                                   \
    }  // namespace valle::platform

#define VALLE_BIND_LDC161X_VALVE_POSITION_CONTROLLER_ISR_ROUTERS(instance) \
    VALLE_BIND_LDC161X_VALVE_POSITION_CONTROLLER_INTB_ISR_ROUTER(instance)
