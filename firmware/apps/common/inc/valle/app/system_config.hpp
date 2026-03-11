#include "valle/platform/drivers/core_system.hpp"

namespace valle::app
{
    constexpr auto kDefaultCoreSystemConfig = CoreSystemConfig{
        .power_config =
            PowerConfig{
                .regulation =
                    PowerRegulationConfig{
                        .voltage_range       = PowerVoltageRange::kRange1,
                        .enable_range1_boost = true,
                    },
                .backup_domain   = {},
                .low_power_run   = {},
                .internal_wakeup = {},
                .pupd_config     = {},
            },
        .rcc_config =
            RCCConfig{
                .hsi =
                    HSIOscillatorConfig{
                        .enabled  = true,
                        .trimming = kDefaultHSICalibrationTrim,
                    },
                .hse =
                    HSEOscillatorConfig{
                        .enabled = false,
                    },
                .hsi48 =
                    HSI48OscillatorConfig{
                        .enabled = false,
                    },
                .lsi =
                    LSIOscillatorConfig{
                        .enabled = false,
                    },
                .lse =
                    LSEOscillatorConfig{
                        .enabled = false,
                    },
                .pll =
                    PLLConfig{
                        .enabled          = true,
                        .source           = PLLSource::kHSI,     // 16 MHz
                        .m_divider        = PLLMDivider::kDiv4,  // 16 MHz / 4 = 4 MHz PLL input
                        .n_multiplier     = 85,                  // 4 MHz * 85 = 340 MHz VCO
                        .r_divider        = PLLRDivider::kDiv2,  // 340 MHz / 2 = 170 MHz PLL_R (HRTIM)
                        .p_divider        = PLLPDivider::kDiv2,  // 340 MHz / 2 = 170 MHz PLL_P (SYSCLK)
                        .q_divider        = PLLQDivider::kDiv2,  // 340 MHz / 2 = 170 MHz PLL_Q (USB, ADC)
                        .r_output_enabled = true,
                        .p_output_enabled = false,
                        .q_output_enabled = false,
                    },
                .sct =
                    SCTConfig{
                        .source         = SCTSource::kPLL,
                        .ahb_prescaler  = SCTAHBPrescaler::kDiv1,
                        .apb1_prescaler = SCTAPB1Prescaler::kDiv1,
                        .apb2_prescaler = SCTAPB2Prescaler::kDiv1,
                    },
            },
    };

    static_assert(!kDefaultCoreSystemConfig.validate().has_value(), "Default core system configuration is invalid");
    static_assert(kDefaultCoreSystemConfig.rcc_config.get_hclk_freq_hz() ==
                      skPlatformConfig.timing_config.core_clock_freq_hz,
                  "Default PLL R output frequency should be 170 MHz");

}  // namespace valle::app