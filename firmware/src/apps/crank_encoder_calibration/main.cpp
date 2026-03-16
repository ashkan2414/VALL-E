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
        expect(g_drivers.crank_encoder.run_homing_sequence(TimeoutMillis(std::numeric_limits<TimeoutMillisRep>::max())),
               "Homing failed: Index pulse not detected within timeout");
        VALLE_LOG_INFO("Homing successful! Starting main loop...");

        while (true)
        {
            const float angle_rad_rel = g_drivers.crank_encoder.get_pos_rad_rel();
            const float angle_rad_abs = g_drivers.crank_encoder.get_pos_rad_abs();

            const float norm_count_rel = g_drivers.crank_encoder.get_pos_norm_rel();
            const float norm_count_abs = g_drivers.crank_encoder.get_pos_norm_abs();

            const float rotor_angle_rad = g_engine_kinematics_calculator.norm_count_to_rotor_angle_rad(norm_count_rel);
            const float cylinder_angle_rad =
                g_engine_kinematics_calculator.rotor_angle_rad_to_cylinder_angle_rad(rotor_angle_rad);
            const float cylinder_pos_norm_approx =
                g_engine_kinematics_calculator.cylinder_angle_rad_to_cylinder_pos_norm_approx(cylinder_angle_rad);
            const float cylinder_displacement =
                g_engine_kinematics_calculator.cylinder_angle_rad_to_cylinder_displacement(cylinder_angle_rad);
            const float cylinder_pos_norm_exact =
                g_engine_kinematics_calculator.cylinder_displacement_to_cylinder_pos_norm(cylinder_displacement);

            const float home_calibration_compensation_rad = 2 * std::numbers::pi_v<float> - angle_rad_rel;

            VALLE_LOG_INFO(
                "\nAngle Rad [Rel (Abs)]: {:.2f} ({:.2f})\nNorm Count [Rel (Abs)]: {:.2f} ({:.2f})"
                "\nRotor Angle [Rad]: {:.2f}\nCylinder Angle [Rad]: {:.2f}"
                "\nCylinder Pos [Norm] [Approx (Exact)]: {:.2f} ({:.2f})"
                "\nCylinder Displacement [in]: {:.2f}\nHome Compensation [Rad]: {}",
                angle_rad_rel,
                angle_rad_abs,
                norm_count_rel,
                norm_count_abs,
                rotor_angle_rad,
                cylinder_angle_rad,
                cylinder_pos_norm_approx,
                cylinder_pos_norm_exact,
                cylinder_displacement,
                home_calibration_compensation_rad);
            TimingContext::delay_ms(100);
        }
    }

}  // namespace valle::app