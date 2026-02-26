#pragma once

#include <stdint.h>

#include <algorithm>
#include <atomic>

#include "Valle/Board/Drivers/hrtim_half_bridge.hpp"
#include "Valle/Math/System/Blocks/feedback.hpp"
#include "Valle/Math/System/Blocks/pid.hpp"
#include "delegate/Delegate.h"

namespace valle
{
    template <std::floating_point T>
    class VCAClosedLoopController : public ISystemBlock<VCAClosedLoopController<T>, T>
    {
    public:
        using ValueT = T;

    private:
        constexpr static T skPidP = static_cast<T>(0.5);
        constexpr static T skPidI = static_cast<T>(1.0);
        constexpr static T skPidD = static_cast<T>(0.0);

        PIDSystem<ValueT> system{};

    public:
        VCAClosedLoopController() = default;

        explicit VCAClosedLoopController(const ValueT sample_time_s) : system(skPidP, skPidI, skPidD, sample_time_s)
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
    class VCAClosedLoopFeedbackController : public ISystemBlock<VCAClosedLoopFeedbackController<T>, T>
    {
    public:
        using ValueT              = T;
        using CurrentFeedbackFn   = delegate::Delegate<ValueT>;
        using OpenLoopControllerT = VCAClosedLoopController<ValueT>;
        using FeedbackSystemT     = ExFeedbackSystem<OpenLoopControllerT, CurrentFeedbackFn>;

    private:
        FeedbackSystemT m_feedback_system{};  /// Feedback System

    public:
        VCAClosedLoopFeedbackController() = default;

        VCAClosedLoopFeedbackController(ValueT sample_time_s, CurrentFeedbackFn&& feedback_fn, ValueT min_tolerance)
            : m_feedback_system(OpenLoopControllerT(sample_time_s), std::move(feedback_fn), min_tolerance)
        {
        }

        ValueT process_impl(const ValueT reference)
        {
            return m_feedback_system.process(reference);
        }

        void reset_impl()
        {
            m_feedback_system.reset();
        }
    };

    template <std::floating_point T>
    class VCAOpenLoopController : public ISystemBlock<VCAOpenLoopController<T>, T>
    {
    public:
        using ValueT = T;

        static ValueT process_impl(const ValueT reference)
        {
            return reference;
        }

        static void reset_impl()
        {
        }
    };

    /**
     * @brief VCAControllerModule (Voice Coil Actuator) Control Modes
     *
     */
    enum class VCAControlMode
    {
        kOpenLoopDuty,
        kClosedLoopCurrent
    };

    /**
     * @brief VCAControllerModule (Voice Coil Actuator) Configuration Structure
     *
     */
    struct VCAModuleConfig
    {
        // PWM Configuration
        HRTIMHalfBridgeDriverConfig half_bridge_config{};

        /// Maximum current in Amps
        float max_current_amp = 5.0F;  // NOLINT(readability-magic-numbers)

        /// Target current tolerance in Amps
        float target_tolerance_amp = 0.01F;  // NOLINT(readability-magic-numbers)
    };

    /**
     * @brief VCAControllerModule (Voice Coil Actuator) Controller Class
     *
     * @tparam THRTIMTimerDevice Hardware Device class for HRTIM control.
     * @tparam tkMode  Control mode (open-loop duty or closed-loop current).
     */
    template <typename THRTIMDevice, VCAControlMode tkMode = VCAControlMode::kClosedLoopCurrent>
    class VCAControllerModule
    {
    public:
        using InjectDevices                    = TypeList<THRTIMDevice>;
        static constexpr uint8_t skDeviceIndex = THRTIMDevice::skIndex;

        using ControllerT = std::conditional_t<tkMode == VCAControlMode::kClosedLoopCurrent,
                                               VCAClosedLoopFeedbackController<float>,
                                               VCAOpenLoopController<float>>;

    private:
        HRTIMHalfBridgeDriver<THRTIMDevice> m_device;           /// Hardware
        float                               m_max_current_amp;  /// Max current limit (for safety)
        Atomic<float>                       m_setpoint;         /// Current target setpoint (duty or current)
        ControllerT                         m_controller;       /// Control algorithm (either open-loop or closed-loop)

    public:
        VCAControllerModule() = delete;

        /**
         * @brief Construct a new VCAControllerModule object
         *
         * @param current_feedback_fn function to read the current feedback in Amps.
         */
        VCAControllerModule(DeviceRef<THRTIMDevice> hw) : m_device(std::move(hw))
        {
        }

        // ------------------------------------------------------------------------
        // Initialization
        // ------------------------------------------------------------------------

        /**
         * @brief Initializes the VCAControllerModule hardware with open loop duty control.
         *
         * @param config The configuration for the VCAControllerModule, including PWM settings and max current limit.
         * @return true if initialization was successful, false otherwise (e.g., hardware init failure, invalid config,
         * etc.).
         */
        [[nodiscard]] bool init(const VCAModuleConfig& config)
            requires(tkMode == VCAControlMode::kOpenLoopDuty)
        {
            m_max_current_amp = config.max_current_amp;

            if (!m_device.init(config.half_bridge_config))
            {
                return false;
            }

            // Start at idle
            set_target_duty(0.0F);

            return true;
        }

        /**
         * @brief Initializes the VCAControllerModule hardware with closed loop controller.
         *
         * @param config The configuration for the VCAControllerModule, including PWM settings and max current limit.
         * @param current_feedback_fn A delegate function that returns the current feedback in Amps. This function will
         * be called at PWM frequency from the control loop, so it should be efficient and ISR safe.
         */
        [[nodiscard]] bool init(const VCAModuleConfig&                                             config,
                                typename VCAClosedLoopFeedbackController<float>::CurrentFeedbackFn current_feedback_fn)
            requires(tkMode == VCAControlMode::kClosedLoopCurrent)
        {
            m_max_current_amp = config.max_current_amp;
            m_controller      = ControllerT(1.0F / static_cast<float>(config.half_bridge_config.get_int_freq_hz()),
                                       std::move(current_feedback_fn),
                                       config.target_tolerance_amp);

            if (!m_device.init(config.half_bridge_config))
            {
                return false;
            }

            // Start at idle
            set_target_current(0.0F);

            return true;
        }

        /**
         * @brief Initializes an ADC trigger for the timer. This allows the timer to trigger ADC conversions at specific
         * points in the PWM cycle.
         *
         * @tparam tkTriggerID The ID of the ADC trigger to configure (e.g., kTrig1, kTrig2, etc.). Each timer has
         * multiple trigger outputs that can be independently configured.
         * @param config The configuration for the ADC trigger, including the source event (e.g., timer period, compare
         * match, etc.) and the rollover mode (when the trigger should occur in up-down counting).
         * @return true if the ADC trigger was successfully initialized, false otherwise (e.g., invalid trigger ID,
         * incompatible configuration, etc.).
         * @return false if the trigger ID is invalid or if the configuration is incompatible with the timer's counting
         * mode. For example, certain trigger sources may not be valid in center-aligned mode, and the function should
         * validate this before applying the configuration.
         */
        template <HRTIMTimerADCTriggerID tkTriggerID>
        [[nodiscard]] inline bool init_adc_trigger(
            const HRTIMTimerADCTriggerConfig<THRTIMDevice::skTimerID, tkTriggerID>& config)
        {
            return m_device.init_adc_trigger(config);
        }

        /**
         * @brief Enables the actuator output.
         *
         */
        void enable()
        {
            m_device.enable_output();
        }

        /**
         * @brief Disables the actuator output (sets duty to 0% and disables PWM).
         *
         */
        void disable()
        {
            m_device.disable_output();
        }

        // ------------------------------------------------------------------------
        // Control Interface
        // ------------------------------------------------------------------------

        /**
         * @brief Sets the target duty cycle for open-loop control.
         * @param duty -1.0 to 1.0 (0.0 = Zero Force)
         */
        void set_target_duty(const float duty)
            requires(tkMode == VCAControlMode::kOpenLoopDuty)
        {
            m_setpoint.store(std::clamp(duty, -1.0F, 1.0F), std::memory_order_relaxed);
        }

        /**
         * @brief Sets the target current for closed-loop control. The controller will adjust the duty cycle to try to
         * achieve this current.
         * @param amps Target current in Amps.
         */
        void set_target_current(const float amps)
            requires(tkMode == VCAControlMode::kClosedLoopCurrent)
        {
            m_setpoint.store(std::clamp(amps, -m_max_current_amp, m_max_current_amp), std::memory_order_relaxed);
        }

        /**
         * @brief Runs the control loop. Should be called at PWM frequency via a timer interrupt.
         * @note Hot Path! Keep this function efficient and ISR safe.
         */
        float run_ctrl_loop()
        {
            const float output_duty     = m_controller.process(m_setpoint.load(std::memory_order_relaxed));
            const float normalized_duty = (output_duty + 1.0F) * 0.5F;

            // Convert (-1.0 to 1.0) -> (0.0 to 1.0 Duty)
            m_device.set_duty_cycle(normalized_duty);

            return normalized_duty;
        }
    };

}  // namespace valle