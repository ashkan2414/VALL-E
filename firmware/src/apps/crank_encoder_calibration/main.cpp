#include <random>

#include "app.hpp"
#include "valle/app_bridge.hpp"
#include "valle/math/engine_kinematics.hpp"
#include "valle/system/timing.hpp"

namespace valle::app
{
    void main()
    {
        using namespace valle::system;

        init();
        TimingContext::delay_ms(3000);

        g_drivers.crank_encoder.enable();

        VALLE_LOG_INFO("Initialized! Waiting for homing index...");
        expect(g_drivers.crank_encoder.run_homing_sequence(TimeoutMillis(1000000)),
               "Homing failed: Index pulse not detected within timeout");
        VALLE_LOG_INFO("Homing successful! Starting main loop...");

        while (true)
        {
            const float encoder_pos_norm_cycle  = g_drivers.crank_encoder.get_encoder_pos_norm_cycle();
            const float encoder_pos_norm_rev    = g_drivers.crank_encoder.get_encoder_pos_norm_rev();
            const float encoder_pos_norm_stroke = g_drivers.crank_encoder.get_encoder_pos_norm_stroke();

            const float encoder_pos_rad_cycle  = g_drivers.crank_encoder.get_encoder_pos_rad_cycle();
            const float encoder_pos_rad_rev    = g_drivers.crank_encoder.get_encoder_pos_rad_rev();
            const float encoder_pos_rad_stroke = g_drivers.crank_encoder.get_encoder_pos_rad_stroke();

            const float crank_angle_rad_cycle  = g_drivers.crank_encoder.get_crank_angle_rad_cycle();
            const float crank_angle_rad_rev    = g_drivers.crank_encoder.get_crank_angle_rad_rev();
            const float crank_angle_rad_stroke = g_drivers.crank_encoder.get_crank_angle_rad_stroke();

            const float cylinder_angle_rad       = g_drivers.crank_encoder.get_cylinder_angle_rad();
            const float cylinder_pos_norm_approx = g_drivers.crank_encoder.get_cylinder_pos_norm_approx();
            const float cylinder_pos_norm        = g_drivers.crank_encoder.get_cylinder_pos_norm();
            const float cylinder_displacement    = g_drivers.crank_encoder.get_cylinder_displacement();

            const float home_calibration_compensation_rad = 2 * std::numbers::pi_v<float> - encoder_pos_rad_rev;

            VALLE_LOG_INFO(
                "\nEncoder Pos [Norm] [Cycle / Rev / Stroke]: {:.2f} / {:.2f} / {:.2f}"
                "\nEncoder Pos [Rad] [Cycle / Rev / Stroke]: {:.2f} / {:.2f} / {:.2f}"
                "\nCrank Angle [Rad] [Cycle / Rev / Stroke]: {:.2f} / {:.2f} / {:.2f}"
                "\nCylinder Angle [Rad]: {:.2f}"
                "\nCylinder Pos [Norm] [Approx (Exact)]: {:.2f} ({:.2f})"
                "\nCylinder Displacement [in]: {:.2f}"
                "\nEncoder Pos Home Compensation [Rad]: {}",
                encoder_pos_norm_cycle,
                encoder_pos_norm_rev,
                encoder_pos_norm_stroke,
                encoder_pos_rad_cycle,
                encoder_pos_rad_rev,
                encoder_pos_rad_stroke,
                crank_angle_rad_cycle,
                crank_angle_rad_rev,
                crank_angle_rad_stroke,
                cylinder_angle_rad,
                cylinder_pos_norm_approx,
                cylinder_pos_norm,
                cylinder_displacement,
                home_calibration_compensation_rad);
            TimingContext::delay_ms(100);
        }
    }
}  // namespace valle::app
