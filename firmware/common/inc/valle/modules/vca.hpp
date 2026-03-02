#pragma once

#include <stdint.h>

#include <algorithm>
#include <atomic>

#include "delegate/Delegate.h"
#include "valle/core/device/device.hpp"
#include "valle/math/system/blocks/feedback.hpp"
#include "valle/math/system/blocks/pid.hpp"

namespace valle
{
    // =============================================================================
    // Half Bridge Interface
    // =============================================================================
    template <typename TDerived, typename TConfig>
    class VCAControllerModuleHalfBridgeInterface
    {
    public:
        using ConfigT = TConfig;

        [[nodiscard]] bool init(const ConfigT& config)
        {
            return static_cast<TDerived*>(this)->init_impl(config);
        }

        void enable()
        {
            static_cast<TDerived*>(this)->enable_impl();
        }

        void disable()
        {
            static_cast<TDerived*>(this)->disable_impl();
        }

        void set_duty_norm(const float normalized_duty)
        {
            static_cast<TDerived*>(this)->set_duty_norm_impl(normalized_duty);
        }
    };

    // =============================================================================
    // System Controller
    // =============================================================================

    /**
     * @brief VCAControllerModule (Voice Coil Actuator) Control Modes
     *
     */
    enum class VCAControlMode
    {
        kOpenLoopDuty,
        kClosedLoopCurrent
    };

    template <std::floating_point T>
    struct VCAClosedLoopControllerConfig
    {
        using ValueT = T;

        ValueT sample_time_s = 0.0F;  // Control loop sample time in seconds (e.g., 1 ms)
    };

    template <std::floating_point T>
    class VCACurrentController : public ISystemBlock<VCACurrentController<T>, T>
    {
    public:
        using ValueT                           = T;
        using ConfigT                          = VCAClosedLoopControllerConfig<ValueT>;
        static constexpr VCAControlMode skMode = VCAControlMode::kClosedLoopCurrent;

    private:
        constexpr static T skPidP = static_cast<T>(0.5);
        constexpr static T skPidI = static_cast<T>(1.0);
        constexpr static T skPidD = static_cast<T>(0.0);

        PIDSystem<ValueT> system{};

    public:
        VCACurrentController() = default;

        explicit VCACurrentController(const ConfigT& config) : system(skPidP, skPidI, skPidD, config.sample_time_s)
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
    struct VCAClosedLoopCurrentFeedbackControllerConfig
    {
        using ValueT            = T;
        using CurrentFeedbackFn = delegate::Delegate<ValueT>;

        ValueT sample_time_s   = 0.0F;  // Control loop sample time in seconds (e.g., 1 ms)
        ValueT max_current_amp = 0.0F;  // Max current limit (for safety)
        ValueT target_tolerance_amp =
            0.0F;  // Target current tolerance in Amps (used to determine when to disable the output for safety)
        CurrentFeedbackFn feedback_fn{};  // Function to read current feedback (e.g., from a sensor)
    };

    template <std::floating_point T>
    class VCAClosedLoopCurrentFeedbackController : public ISystemBlock<VCAClosedLoopCurrentFeedbackController<T>, T>
    {
    public:
        using ValueT                           = T;
        using ConfigT                          = VCAClosedLoopCurrentFeedbackControllerConfig<ValueT>;
        using CurrentFeedbackFn                = typename ConfigT::CurrentFeedbackFn;
        using OpenLoopControllerT              = VCACurrentController<ValueT>;
        using FeedbackSystemT                  = ExFeedbackSystem<OpenLoopControllerT, CurrentFeedbackFn>;
        static constexpr VCAControlMode skMode = VCAControlMode::kClosedLoopCurrent;

    private:
        FeedbackSystemT m_feedback_system{};  /// Feedback System
        ValueT          m_max_current_amp{};  /// Max current limit (for safety)

    public:
        VCAClosedLoopCurrentFeedbackController() = default;

        explicit VCAClosedLoopCurrentFeedbackController(const ConfigT& config)
            : m_feedback_system(OpenLoopControllerT(VCAClosedLoopControllerConfig<ValueT>{config.sample_time_s}),
                                std::move(config.feedback_fn),
                                config.target_tolerance_amp)
            , m_max_current_amp(std::abs(config.max_current_amp))
        {
        }

        ValueT process_impl(const ValueT reference)
        {
            return m_feedback_system.process(std::clamp(reference, -m_max_current_amp, m_max_current_amp));
        }

        void reset_impl()
        {
            m_feedback_system.reset();
        }
    };

    template <std::floating_point T>
    struct VCAOpenLoopDutyControllerConfig
    {
        using ValueT = T;
    };

    template <std::floating_point T>
    class VCAOpenLoopDutyController : public ISystemBlock<VCAOpenLoopDutyController<T>, T>
    {
    public:
        using ValueT                           = T;
        using ConfigT                          = VCAOpenLoopDutyControllerConfig<ValueT>;
        static constexpr VCAControlMode skMode = VCAControlMode::kOpenLoopDuty;

        explicit VCAOpenLoopDutyController(const ConfigT& config = {})
        {
            (void)config;  // No configuration needed for open-loop, but we accept it for interface consistency
        }

        static ValueT process_impl(const ValueT reference)
        {
            return std::clamp(reference, -1.0F, 1.0F);  // Clamp duty cycle to valid range
        }

        static void reset_impl()
        {
        }
    };

    // =============================================================================
    // Module
    // =============================================================================

    /**
     * @brief VCA Controller Module (Voice Coil Actuator) Configuration Structure
     *
     */
    template <typename TPWMConfig, typename TControllerConfig>
    struct VCAControllerModuleConfigX
    {
        // Half-Bridge Driver Configuration
        TPWMConfig half_bridge_config{};

        /// Controller Configuration (e.g., max current, control loop sample time, etc.)
        TControllerConfig controller_config{};
    };

    /**
     * @brief VCA Controller Module (Voice Coil Actuator) Controller Class
     *
     * @tparam THRTIMTimerDevice Hardware Device class for HRTIM control.
     * @tparam skMode  Control mode (open-loop duty or closed-loop current).
     */
    template <typename THalfBridgeInterface, typename TController>
    class VCAControllerModuleX
    {
    public:
        using HalfBridgeInterfaceT = THalfBridgeInterface;
        using ControllerT          = TController;

        using InjectDevices = typename GetInjectDevices<THalfBridgeInterface>::type;
        using DependDevices = typename GetAdditionalDependDevices<THalfBridgeInterface>::type;

        using ConfigT =
            VCAControllerModuleConfigX<typename THalfBridgeInterface::ConfigT, typename TController::ConfigT>;

        static constexpr VCAControlMode skMode = ControllerT::skMode;

    private:
        float                m_max_current_amp{};  /// Max current limit (for safety)
        Atomic<float>        m_setpoint{};         /// Current target setpoint (duty or current)
        ControllerT          m_controller{};       /// Control algorithm (either open-loop or closed-loop)
        THalfBridgeInterface m_half_bridge{};

    public:
        template <typename... TArgs>
        explicit VCAControllerModuleX(TArgs&&... args) : m_half_bridge(std::forward<TArgs>(args)...)

        {
        }

        // ------------------------------------------------------------------------
        // Initialization
        // ------------------------------------------------------------------------

        /**
         * @brief Initializes the VCA Controller Module hardware with open loop duty control.
         *
         * @param config The configuration for the VCA Controller Module, including PWM settings and max current
         * limit.
         * @return true if initialization was successful, false otherwise (e.g., hardware init failure, invalid
         * config, etc.).
         */
        [[nodiscard]] bool init(const ConfigT& config)
        {
            m_controller = ControllerT(config.controller_config);

            if (!m_half_bridge.init(config.half_bridge_config))
            {
                return false;
            }

            // Start at idle
            set_zero();

            return true;
        }

        /**
         * @brief Enables the actuator output.
         *
         */
        void enable()
        {
            m_half_bridge.enable();
        }

        /**
         * @brief Disables the actuator output (sets duty to 0% and disables PWM).
         *
         */
        void disable()
        {
            m_half_bridge.disable();
        }

        // ------------------------------------------------------------------------
        // Control Interface
        // ------------------------------------------------------------------------

        THalfBridgeInterface& get_half_bridge()
        {
            return m_half_bridge;
        }

        void set_zero()
        {
            if constexpr (skMode == VCAControlMode::kOpenLoopDuty)
            {
                set_target_duty(0.0F);
            }
            else if constexpr (skMode == VCAControlMode::kClosedLoopCurrent)
            {
                set_target_current(0.0F);
            }
        }

        /**
         * @brief Sets the target duty cycle for open-loop control.
         * @param duty -1.0 to 1.0 (0.0 = Zero Force)
         */
        void set_target_duty(const float duty)
            requires(skMode == VCAControlMode::kOpenLoopDuty)
        {
            m_setpoint.store(duty, std::memory_order_relaxed);
        }

        /**
         * @brief Sets the target current for closed-loop control. The controller will adjust the duty cycle to try
         * to achieve this current.
         * @param amps Target current in Amps.
         */
        void set_target_current(const float amps)
            requires(skMode == VCAControlMode::kClosedLoopCurrent)
        {
            m_setpoint.store(amps, std::memory_order_relaxed);
        }

        /**
         * @brief Runs the control loop. Should be called at PWM frequency via a timer interrupt.
         * @note Hot Path! Keep this function efficient and ISR safe.
         */
        float run_ctrl_loop()
        {
            const float output_duty = m_controller.process(m_setpoint.load(std::memory_order_relaxed));

            // Clamp the output duty cycle to valid range before sending to the half-bridge interface
            const float clamped_duty = std::clamp(output_duty, -1.0F, 1.0F);

            // Convert (-1.0 to 1.0) -> (0.0 to 1.0 Duty)
            const float normalized_duty = (clamped_duty + 1.0F) * 0.5F;

            m_half_bridge.set_duty_norm(normalized_duty);

            return normalized_duty;
        }
    };

}  // namespace valle