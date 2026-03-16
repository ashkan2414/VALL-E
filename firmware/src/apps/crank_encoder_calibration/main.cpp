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
        g_drivers.crank_encoder.enable();

        VALLE_LOG_INFO("Initialized! Waiting for homing sequence...");
        // Wait for a few seconds to allow the user to set the crank to the desired position for homing (e.g. TDC or a
        // known offset)
        expect(g_drivers.crank_encoder.run_homing_sequence(TimeoutMillis(60'000)),
               "Homing sequence failed! Index pulse not detected within timeout.");

        VALLE_LOG_INFO("Homing complete! Starting main loop in 3 seconds...");
        TimingContext::delay_ms(3000);

        VALLE_LOG_INFO(
            "norm_count,rotor_angle_rad,cylinder_angle_rad,cylinder_pos_norm_approx,cylinder_pos_norm_exact,cylinder_"
            "displacement_in");

        while (true)
        {
            const float norm_count      = g_drivers.crank_encoder.get_pos_norm_rel();
            const float rotor_angle_rad = g_engine_kinematics_calculator.norm_count_to_rotor_angle_rad(norm_count);
            const float cylinder_angle_rad =
                g_engine_kinematics_calculator.rotor_angle_rad_to_cylinder_angle_rad(rotor_angle_rad);
            const float cylinder_pos_norm_approx =
                g_engine_kinematics_calculator.cylinder_angle_rad_to_cylinder_pos_norm_approx(cylinder_angle_rad);
            const float cylinder_displacement =
                g_engine_kinematics_calculator.cylinder_angle_rad_to_cylinder_displacement(cylinder_angle_rad);
            const float cylinder_pos_norm_exact =
                g_engine_kinematics_calculator.cylinder_displacement_to_cylinder_pos_norm(cylinder_displacement);

            VALLE_LOG_INFO("{},{},{},{},{},{}",
                           norm_count,
                           rotor_angle_rad,
                           cylinder_angle_rad,
                           cylinder_pos_norm_approx,
                           cylinder_pos_norm_exact,
                           cylinder_displacement);
            TimingContext::delay_ms(100);
        }
    }

}  // namespace valle::app