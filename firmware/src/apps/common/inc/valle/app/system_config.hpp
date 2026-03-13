#pragma once

#include "valle/platform/drivers/core_system.hpp"

namespace valle::app
{
    constexpr auto kDefaultCoreSystemConfig =
        platform::CoreSystemConfig{
            .power_config =
                platform::PowerConfig{
                    .regulation =
                        platform::PowerRegulationConfig{
                            .voltage_range       = platform::PowerVoltageRange::kRange1,
                            .enable_range1_boost = true,
                        },
                    .backup_domain   = {},
                    .low_power_run   = {},
                    .internal_wakeup = {},
                    .pupd_config     = {},
                },
            .rcc_config =
                platform::RCCConfig{
                    .hsi =
                        platform::HSIOscillatorConfig{
                            .enabled  = true,
                            .trimming = platform::kDefaultHSICalibrationTrim,
                        },
                    .hse =
                        platform::HSEOscillatorConfig{
                            .enabled = false,
                        },
                    .hsi48 =
                        platform::HSI48OscillatorConfig{
                            .enabled = false,
                        },
                    .lsi =
                        platform::LSIOscillatorConfig{
                            .enabled = false,
                        },
                    .lse =
                        platform::LSEOscillatorConfig{
                            .enabled = false,
                        },
                    .pll =
                        platform::PLLConfig{
                            .enabled          = true,
                            .source           = platform::PLLSource::kHSI,     // 16 MHz
                            .m_divider        = platform::PLLMDivider::kDiv4,  // 16 MHz / 4 = 4 MHz PLL input
                            .n_multiplier     = 85,                            // 4 MHz * 85 = 340 MHz VCO
                            .r_divider        = platform::PLLRDivider::kDiv2,  // 340 MHz / 2 = 170 MHz PLL_R (HRTIM)
                            .p_divider        = platform::PLLPDivider::kDiv2,  // 340 MHz / 2 = 170 MHz PLL_P (SYSCLK)
                            .q_divider        = platform::PLLQDivider::kDiv2,  // 340 MHz / 2 = 170 MHz PLL_Q (USB, ADC)
                            .r_output_enabled = true,
                            .p_output_enabled = false,
                            .q_output_enabled = false,
                        },
                    .sct =
                        platform::SCTConfig{
                            .sysclk_source  = platform::SCTSYSCLKSource::kPLL,
                            .ahb_prescaler  = platform::SCTAHBPrescaler::kDiv1,
                            .apb1_prescaler = platform::SCTAPB1Prescaler::kDiv1,
                            .apb2_prescaler = platform::SCTAPB2Prescaler::kDiv1,
                            .mco_source     = platform::SCTMCOClockSource::kSysclk,
                            .mco_prescaler  = platform::SCTMCOPrescaler::kDiv1,
                            .usart1_source  = platform::SCTUSART1ClockSource::kPclk2,
                            .usart2_source  = platform::SCTUSART2ClockSource::kPclk1,
                            .usart3_source  = platform::SCTUSART3ClockSource::kPclk1,
                            .uart4_source   = platform::SCTUART4ClockSource::kPclk1,
                            .uart5_source   = platform::SCTUART5ClockSource::kPclk1,
                            .lpuart1_source = platform::SCTLPUART1ClockSource::kPclk1,
                            .i2c1_source    = platform::SCTI2C1ClockSource::kPclk1,
                            .i2c2_source    = platform::SCTI2C2ClockSource::kPclk1,
                            .i2c3_source    = platform::SCTI2C3ClockSource::kPclk1,
                            .i2c4_source    = platform::SCTI2C4ClockSource::kPclk1,
                            .lptim1_source  = platform::SCTLPTIM1ClockSource::kPclk1,
                            .sai1_source    = platform::SCTSAI1ClockSource::kSysclk,
                            .i2s_source     = platform::SCTI2SClockSource::kSysclk,
                            .fdcan_source   = platform::SCTFDCANClockSource::kHSE,
                            .rng_source     = platform::SCTRNGClockSource::kHSI48,
                            .usb_source     = platform::SCTUSBClockSource::kHSI48,
                            .adc12_source   = platform::SCTADC12ClockSource::kSysclk,
                            .adc345_source  = platform::SCTADC345ClockSource::kSysclk,
                            .quadspi_source = platform::SCTQUADSPIClockSource::kSysclk,
                        },
                },
        };

    static_assert(!kDefaultCoreSystemConfig.validate().has_value(), "Default core system configuration is invalid");
    static_assert(kDefaultCoreSystemConfig.rcc_config.get_hclk_freq_hz() ==
                      skPlatformConfig.timing_config.core_clock_freq_hz,
                  "Default PLL R output frequency should be 170 MHz");

}  // namespace valle::app