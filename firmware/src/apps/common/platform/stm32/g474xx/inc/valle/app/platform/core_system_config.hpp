#pragma once

#include "valle/platform/drivers/core_system.hpp"

namespace valle::platform::app
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
                        .sysclk_source  = SCTSYSCLKSource::kPLL,
                        .ahb_prescaler  = SCTAHBPrescaler::kDiv1,
                        .apb1_prescaler = SCTAPB1Prescaler::kDiv1,
                        .apb2_prescaler = SCTAPB2Prescaler::kDiv1,
                        .mco_source     = SCTMCOClockSource::kSysclk,
                        .mco_prescaler  = SCTMCOPrescaler::kDiv1,
                        .usart1_source  = SCTUSART1ClockSource::kPclk2,
                        .usart2_source  = SCTUSART2ClockSource::kPclk1,
                        .usart3_source  = SCTUSART3ClockSource::kPclk1,
                        .uart4_source   = SCTUART4ClockSource::kPclk1,
                        .uart5_source   = SCTUART5ClockSource::kPclk1,
                        .lpuart1_source = SCTLPUART1ClockSource::kPclk1,
                        .i2c1_source    = SCTI2C1ClockSource::kPclk1,
                        .i2c2_source    = SCTI2C2ClockSource::kPclk1,
                        .i2c3_source    = SCTI2C3ClockSource::kPclk1,
                        .i2c4_source    = SCTI2C4ClockSource::kPclk1,
                        .lptim1_source  = SCTLPTIM1ClockSource::kPclk1,
                        .sai1_source    = SCTSAI1ClockSource::kSysclk,
                        .i2s_source     = SCTI2SClockSource::kSysclk,
                        .fdcan_source   = SCTFDCANClockSource::kHSE,
                        .rng_source     = SCTRNGClockSource::kHSI48,
                        .usb_source     = SCTUSBClockSource::kHSI48,
                        .adc12_source   = SCTADC12ClockSource::kSysclk,
                        .adc345_source  = SCTADC345ClockSource::kSysclk,
                        .quadspi_source = SCTQUADSPIClockSource::kSysclk,
                    },
            },
    };

    static_assert(!kDefaultCoreSystemConfig.validate().has_value(), "Default core system configuration is invalid");
    static_assert(kDefaultCoreSystemConfig.rcc_config.get_hclk_freq_hz() ==
                      valle::app::kPlatformConfig.timing_config.core_clock_freq_hz,
                  "Default PLL R output frequency should be 170 MHz");

}  // namespace valle::platform::app