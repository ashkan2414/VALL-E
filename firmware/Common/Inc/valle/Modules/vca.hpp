#pragma once

#include <stdint.h>

#include <algorithm>
#include <atomic>

#include "Valle/Drivers/hrtim_half_bridge.hpp"
#include "Valle/Math/System/Blocks/feedback.hpp"
#include "Valle/Math/System/Blocks/pid.hpp"
#include "delegate/Delegate.h"

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
    HRTIMHalfBridgeConfig pwm_config;            /// PWM Configuration
    float                 max_current_amp;       /// Maximum current in Amps
    float                 target_tolerance_amp;  /// Target current tolerance in Amps

    [[nodiscard]] static constexpr inline VCAModuleConfig default_config()
    {
        return VCAModuleConfig{
            .pwm_config           = HRTIMHalfBridgeConfig::default_config(),
            .max_current_amp      = 5.0f,
            .target_tolerance_amp = 0.01f,
        };
    }
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

private:
    using OpenLoopControllerT = PIDSystem<float>;
    using CurrentFeedbackFn   = delegate::Delegate<float>;
    using FeedbackSystemT     = ExFeedbackSystem<OpenLoopControllerT, CurrentFeedbackFn>;

    constexpr static float skPidP = 0.5f;
    constexpr static float skPidI = 1.0f;
    constexpr static float skPidD = 0.0f;

    HRTIMHalfBridgeDriver<THRTIMDevice> m_device;           /// Hardware
    VCAModuleConfig                     m_config;           /// Configuration
    FeedbackSystemT                     m_feedback_system;  /// Feedback System
    std::atomic<float>                  m_setpoint;         /// Current target setpoint (duty or current)

public:
    /**
     * @brief Construct a new VCAControllerModule object
     *
     * @param current_feedback_fn function to read the current feedback in Amps.
     */
    VCAControllerModule(DeviceRef<THRTIMDevice> hw,
                        const VCAModuleConfig&  config,
                        CurrentFeedbackFn       current_feedback_fn)
        : m_device(std::move(hw))
        , m_config(config)
        , m_feedback_system(OpenLoopControllerT(
                                skPidP, skPidI, skPidD, 1.0f / static_cast<float>(config.pwm_config.get_int_freq_hz())),
                            std::move(current_feedback_fn),
                            m_config.target_tolerance_amp)
        , m_setpoint(0.0f)
    {
    }

    /**
     * @brief Construct a new VCAControllerModule object with default configuration.
     *
     * @param current_feedback_fn function to read the current feedback in Amps.
     */
    VCAControllerModule(DeviceRef<THRTIMDevice> hw, CurrentFeedbackFn current_feedback_fn)
        : VCAControllerModule(std::move(hw), VCAModuleConfig{}, std::move(current_feedback_fn))
    {
    }

    // Move Constructor
    VCAControllerModule(VCAControllerModule&& other) noexcept
        : m_device(std::move(other.m_device))
        , m_config(other.m_config)
        , m_feedback_system(std::move(other.m_feedback_system))
        , m_setpoint(other.m_setpoint.load(std::memory_order_relaxed))
    {
    }

    // Move Assignment
    VCAControllerModule& operator=(VCAControllerModule&& other) noexcept
    {
        if (this != &other)
        {
            m_device          = std::move(other.m_device);
            m_config          = other.m_config;
            m_feedback_system = std::move(other.m_feedback_system);
            m_setpoint.store(other.m_setpoint.load(std::memory_order_relaxed), std::memory_order_relaxed);
        }
        return *this;
    }

    // ------------------------------------------------------------------------
    // Initialization
    // ------------------------------------------------------------------------

    /**
     * @brief Initializes the VCAControllerModule hardware and feedback system.
     *
     */
    void init()
    {
        m_feedback_system.reset();
        m_device.init(m_config.pwm_config);

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
     * @param duty 0.0 to 1.0 (0.5 = Zero Force)
     */
    void set_target_duty(const float duty)
        requires(tkMode == VCAControlMode::kOpenLoopDuty)
    {
        m_setpoint.store(duty, std::memory_order_relaxed);
    }

    /**
     * @brief Sets the normalized force output.
     * @param force -1.0 (Max Reverse) to 1.0 (Max Forward). 0.0 is Idle.
     */
    void set_target_current(const float amps)
        requires(tkMode == VCAControlMode::kClosedLoopCurrent)
    {
        m_setpoint.store(std::clamp(amps, -m_config.max_current_amp, m_config.max_current_amp),
                         std::memory_order_relaxed);
    }

    /**
     * @brief Gets the control loop frequency in Hz.
     *
     * @return uint64_t Control loop frequency.
     */
    [[nodiscard]] const uint64_t get_ctrl_loop_freq_hz() const
    {
        return m_config.pwm_config.get_int_freq_hz();
    }

    /**
     * @brief Get the interrupt frequency in Hz.
     *
     * @return const uint64_t Interrupt frequency.
     */
    [[nodiscard]] const uint64_t get_int_freq_hz() const
    {
        return m_device.get_int_freq_hz();
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
        m_device.set_duty_cycle(pwm_reg_val);
    }

private:
    /**
     * @brief Gets the controller output based on the current mode.
     *
     * @return float Controller output value.
     */
    [[nodiscard]] float get_controller_output()
    {
        if constexpr (tkMode == VCAControlMode::kOpenLoopDuty)
        {
            return m_setpoint.load(std::memory_order_relaxed);
        }
        else
        {
            return m_feedback_system.process(m_setpoint.load(std::memory_order_relaxed));
        }
    }
};

/**
 * @brief Binds a Hardware Timer Index to a specific C++ Object Instance.
 *
 * @param INSTANCE     The global/static instance of your VCAControllerModule
 */
#define BIND_VCA_ISR(INSTANCE)                                       \
    template <>                                                      \
    void HRTIMIsrRouter<decltype(INSTANCE)::skDeviceIndex>::handle() \
    {                                                                \
        INSTANCE.run_ctrl_loop();                                    \
    }
