#pragma once

#include "valle/app/platform/modules/ldc161x.hpp"

namespace valle::platform::app
{
    constexpr auto kGroveLDC161XCoilConfig = valle::app::LDC161XCoilConfig{
        .inductance_uh  = 18.147F,
        .capacitance_pf = 100.0F,
        .rp_kohm        = 15.727F,
        .q_factor       = 35.97F,
    };
    constexpr auto kGroveLDC161XChannelConfig = valle::app::LDC161XChannelConfig{
        .coil_config   = kGroveLDC161XCoilConfig,
        .drive_current = valle::app::LDC161XIdriveCurrent::from_coil_rp(kGroveLDC161XCoilConfig.rp_kohm),
        .offset_config = valle::app::LDC161XOffsetConfigFrequency{.offset_mhz = 2.5F},
    };

    template <size_t tkNumChannels>
    [[nodiscard]] constexpr auto get_grove_ldc161x_config(const uint16_t sample_rate_hz,
                                                          const uint32_t intb_interrupt_priority)
    {
        std::array<valle::app::LDC161XChannelConfig, tkNumChannels> channel_configs{};
        for (size_t i = 0; i < tkNumChannels; ++i)
        {
            channel_configs[i] = kGroveLDC161XChannelConfig;
        }

        return LDC161XSensorModuleConfig<tkNumChannels>{
            .i2c_config =
                {
                    .intb_interrupt_priority = intb_interrupt_priority,
                },
            .sensor_config = {
                .clock_source       = valle::app::LDC161XClockSourceExternalClock{.fclk_mhz = 40.0F},
                .sample_rate_hz     = sample_rate_hz,
                .deglitch_bandwidth = valle::app::LDC161XDeglitchBandwidth::kBand10MHz,
                .interrupt_config =
                    valle::app::LDC161XInterruptConfig{
                        .int_on_data_ready = true,
                    },
                .sensor_activation_mode = valle::app::LDC161XSensorActivationMode::kFullCurrentMode,
                .enable_rp_override     = true,
                .auto_amplitude_en      = false,
                // .high_current_drive_en = false,
                .channels = channel_configs,
            }};
    }

}  // namespace valle::platform::app