#pragma once

#include <stdint.h>

#include <algorithm>

#include "Valle/Math/System/Blocks/feedback.hpp"
#include "Valle/Math/System/Blocks/pid.hpp"
#include "Valle/Policy/hrtim.hpp"
#include "delegate/Delegate.h"

/**
 * @brief VCA (Voice Coil Actuator) Control Modes
 *
 */
enum class VCAControlMode
{
    kOpenLoopDuty,
    kClosedLoopCurrent
};

/**
 * @brief VCA (Voice Coil Actuator) Configuration Structure
 *
 */
struct VCAConfig
{
    HRTIMHalfBridgeConfig pwm_config           = HRTIMHalfBridgeConfig::default_config();
    float                 max_current_amp      = 5.0f;
    float                 target_tolerance_amp = 0.01f;
};

/**
 * @brief VCA (Voice Coil Actuator) Controller Class
 *
 * @tparam THRTIMHalfBridgePolicy Hardware Policy class for HRTIM control.
 * @tparam tkMode  Control mode (open-loop duty or closed-loop current).
 */
template <typename THRTIMHalfBridgePolicy, VCAControlMode tkMode = VCAControlMode::kClosedLoopCurrent>
class VCA
{
public:
    static constexpr uint8_t skPolicyIndex = THRTIMHalfBridgePolicy::skIndex;

private:
    using OpenLoopControllerT = PIDSystem<float>;
    using CurrentFeedbackFn   = delegate::Delegate<float>;
    using FeedbackSystemT     = ExFeedbackSystem<OpenLoopControllerT, CurrentFeedbackFn>;

    constexpr static float skPidP = 0.5f;
    constexpr static float skPidI = 1.0f;
    constexpr static float skPidD = 0.0f;

    VCAConfig              m_config;           /// Configuration
    THRTIMHalfBridgePolicy m_hw;               /// Hardware Policy (exclusive control)
    FeedbackSystemT        m_feedback_system;  /// Feedback System
    volatile float         m_setpoint = 0.0f;  /// Current target setpoint (duty or current)

public:
    /**
     * @brief Construct a new VCA object
     *
     * @param current_feedback_fn function to read the current feedback in Amps.
     */
    VCA(const VCAConfig& config, CurrentFeedbackFn&& current_feedback_fn)
        : m_config(config)
        , m_hw()
        , m_feedback_system(OpenLoopControllerT(
                                skPidP, skPidI, skPidD, 1.0f / static_cast<float>(config.pwm_config.get_int_freq_hz())),
                            std::move(current_feedback_fn),
                            m_config.target_tolerance_amp)
    {
    }

    /**
     * @brief Construct a new VCA object with default configuration.
     *
     * @param current_feedback_fn function to read the current feedback in Amps.
     */
    VCA(CurrentFeedbackFn&& current_feedback_fn) : VCA(VCAConfig{}, std::move(current_feedback_fn))
    {
    }

    // ------------------------------------------------------------------------
    // Initialization
    // ------------------------------------------------------------------------

    /**
     * @brief Initializes the VCA hardware and feedback system.
     *
     */
    void init()
    {
        m_feedback_system.reset();
        m_hw.init(m_config.pwm_config);

        // Start at idle
        if constexpr (tkMode == VCAControlMode::kOpenLoopDuty)
        {
            set_target_duty(0.0);
        }
        else
        {
            set_target_current(0.0);
        }

        // Update PID sample time based on PWM frequency
        m_feedback_system.open_loop().set_sample_time(1.0f / static_cast<float>(get_ctrl_loop_freq_hz()));

        enable();
    }

    /**
     * @brief Enables the actuator output.
     *
     */
    void enable()
    {
        m_hw.enable_output();
    }

    /**
     * @brief Disables the actuator output (sets duty to 0% and disables PWM).
     *
     */
    void disable()
    {
        m_hw.disable_output();
    }

    // ------------------------------------------------------------------------
    // Control Interface
    // ------------------------------------------------------------------------

    /**
     * @brief Sets the target duty cycle for open-loop control.
     * @param duty 0.0 to 1.0 (0.5 = Zero Force)
     */
    void set_target_duty(const float duty)
        requires(tkMode == VCAControlMode::kOpenLoopDuty)
    {
        m_setpoint = duty;
    }

    /**
     * @brief Sets the normalized force output.
     * @param force -1.0 (Max Reverse) to 1.0 (Max Forward). 0.0 is Idle.
     */
    void set_target_current(const float amps)
        requires(tkMode == VCAControlMode::kClosedLoopCurrent)
    {
        m_setpoint = std::clamp(amps, -m_config.max_current_amp, m_config.max_current_amp);
    }

    /**
     * @brief Gets the control loop frequency in Hz.
     *
     * @return uint64_t Control loop frequency.
     */
    const uint64_t get_ctrl_loop_freq_hz() const
    {
        return m_config.pwm_config.get_int_freq_hz();
    }

    const uint64_t get_int_freq_hz() const
    {
        return m_hw.get_int_freq_hz();
    }

    /**
     * @brief Runs the control loop. Should be called at PWM frequency via a timer interrupt.
     * @note Hot Path! Keep this function efficient and ISR safe.
     */
    void run_ctrl_loop()
    {
        const float output_duty = get_controller_output();

        // Convert (-1.0 to 1.0) -> (0.05 to 0.95 Duty)
        // Locked Anti-Phase logic handled here
        const float pwm_reg_val = (output_duty + 1.0f) * 0.5f;
        m_hw.set_duty_cycle(pwm_reg_val);
    }

private:
    /**
     * @brief Gets the controller output based on the current mode.
     *
     * @return float Controller output value.
     */
    float get_controller_output()
    {
        if constexpr (tkMode == VCAControlMode::kOpenLoopDuty)
        {
            return m_setpoint;
        }
        else
        {
            return m_feedback_system.process(m_setpoint);
        }
    }
};

/**
 * @brief Binds a Hardware Timer Index to a specific C++ Object Instance.
 *
 * @param INSTANCE     The global/static instance of your VCA
 */
#define BIND_VCA_ISR(INSTANCE)                                       \
    template <>                                                      \
    void HRTIMIsrRouter<decltype(INSTANCE)::skPolicyIndex>::handle() \
    {                                                                \
        INSTANCE.run_ctrl_loop();                                    \
    }
