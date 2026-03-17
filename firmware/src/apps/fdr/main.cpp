#include <random>

#include "app.hpp"
#include "valle/app_bridge.hpp"

namespace valle::app
{
    static constexpr float kExtendCurrent  = 0.3F;  // Max current during extend (e.g. power stroke)
    static constexpr float kRetractCurrent = 0.3F;  // Max current during retract (e.g. exhaust stroke)

    std::pair<EngineStroke, float> get_stroke_and_pos_norm()
    {
        // return g_drivers.crank_encoder.get_stroke_pos_norm_approx();

        constexpr float kRPM                = 100.0F;
        constexpr float kRevsPerSecond      = kRPM / 60.0F;
        constexpr float kCycleDegrees       = 720.0F;
        constexpr float kRevDegrees         = 360.0F;
        constexpr float kPi                 = std::numbers::pi_v<float>;

        static system::TimerMillis s_timer;

        const float time_seconds = static_cast<float>(s_timer.elapsed_as<system::DurationMillis>().count()) / 1000.0F;  // Start the timer on first call

        // Total crank angle advanced since t = 0
        const float total_crank_deg = time_seconds * kRevsPerSecond * kRevDegrees;

        // 4-stroke cycle angle: [0, 720)
        float cycle_deg = std::fmod(total_crank_deg, kCycleDegrees);
        if (cycle_deg < 0.0F)
        {
            cycle_deg += kCycleDegrees;
        }

        // Determine current stroke
        EngineStroke stroke;
        if (cycle_deg < 180.0F)
        {
            stroke = EngineStroke::kIntake;
        }
        else if (cycle_deg < 360.0F)
        {
            stroke = EngineStroke::kCompression;
        }
        else if (cycle_deg < 540.0F)
        {
            stroke = EngineStroke::kPower;
        }
        else
        {
            stroke = EngineStroke::kExhaust;
        }

        // Piston position repeats every 360 degrees
        float rev_deg = std::fmod(total_crank_deg, kRevDegrees);
        if (rev_deg < 0.0F)
        {
            rev_deg += kRevDegrees;
        }

        const float theta_rad = rev_deg * kPi / 180.0F;

        // Normalized piston position:
        // 0.0 at TDC, 1.0 at BDC
        const float normalized_position = 0.5F * (1.0F - std::cos(theta_rad));

        return {stroke, normalized_position};
    }   

    float compute_intake_valve_current(const EngineStroke stroke, float pos_norm)
    {
        if (stroke == EngineStroke::kIntake)
        {
            constexpr float open_start = 0.1f;
            constexpr float open_end   = 0.8f;

            if (pos_norm >= open_start && pos_norm <= open_end)
            {
                return kExtendCurrent;
            }
        }

        return kRetractCurrent;
    }

    float compute_exhaust_valve_current(const EngineStroke stroke, float pos_norm)
    {
        if (stroke == EngineStroke::kExhaust)
        {
            constexpr float open_start = 0.1f;
            constexpr float open_end   = 0.9f;

            if ((pos_norm >= open_start) && (pos_norm <= open_end))
            {
                return kExtendCurrent;
            }
        }

        return kRetractCurrent;
    }

    // Sets the current controller based on a normalized angle input [0, 1] (maps to 0 to 2pi along the motor cycle).
    void run_control_loop()
    {
        const auto [stroke, cylinder_pos_norm] = get_stroke_and_pos_norm();
        const float command_current_amps       = compute_intake_valve_current(stroke, cylinder_pos_norm);
        // const float command_current_amps       = compute_exhaust_valve_current(stroke, cylinder_pos_norm);

        g_drivers.vca_current_loop_driver.set_target_current_amps(command_current_amps);
    }

    void main()
    {
        using namespace system;

        init();
        TimingContext::delay_ms(3000);

        g_drivers.crank_encoder.enable();

        //VALLE_LOG_INFO("Initialized! Waiting for homing index...");
        //expect(g_drivers.crank_encoder.run_homing_sequence(TimeoutMillis(1000000)),
        //       "Homing failed: Index pulse not detected within timeout");

        g_drivers.vca_current_loop_driver.start();

        VALLE_LOG_INFO("Initialized!");
        TimingContext::delay_ms(1000);

        while (true)
        {
            TimingContext::run_within_period_us([]() { run_control_loop(); }, 3333);  // ~300Hz control loop
        }
    }
}  // namespace valle::app