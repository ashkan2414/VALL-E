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
            RccConfig{
                .hsi =
                    HsiOscillatorConfig{
                        .enabled  = true,
                        .trimming = kDefaultHsiCalibrationTrim,
                    },
                .hse =
                    HseOscillatorConfig{
                        .enabled = false,
                    },
                .hsi48 =
                    Hsi48OscillatorConfig{
                        .enabled = false,
                    },
                .lsi =
                    LsiOscillatorConfig{
                        .enabled = false,
                    },
                .lse =
                    LseOscillatorConfig{
                        .enabled = false,
                    },
                .pll =
                    PllConfig{
                        .enabled          = true,
                        .source           = PllSource::kHSI,     // 16 MHz
                        .m_divider        = PllMDivider::kDiv4,  // 16 MHz / 4 = 4 MHz PLL input
                        .n_multiplier     = 85,                  // 4 MHz * 85 = 340 MHz VCO
                        .r_divider        = PllRDivider::kDiv2,  // 340 MHz / 2 = 170 MHz Pll_R (HRTIM)
                        .p_divider        = PllPDivider::kDiv2,  // 340 MHz / 2 = 170 MHz Pll_P (SYSCLK)
                        .q_divider        = PllQDivider::kDiv2,  // 340 MHz / 2 = 170 MHz Pll_Q (USB, ADC)
                        .r_output_enabled = true,
                        .p_output_enabled = false,
                        .q_output_enabled = false,
                    },
                .sct =
                    SctConfig{
                        .sysclk_source  = SctSYSCLKSource::kPLL,
                        .ahb_prescaler  = SctAHBPrescaler::kDiv1,
                        .apb1_prescaler = SctAPB1Prescaler::kDiv1,
                        .apb2_prescaler = SctAPB2Prescaler::kDiv1,
                        .mco_source     = SctMCOClockSource::kSysclk,
                        .mco_prescaler  = SctMCOPrescaler::kDiv1,
                        .usart1_source  = SctUSART1ClockSource::kPclk2,
                        .usart2_source  = SctUSART2ClockSource::kPclk1,
                        .usart3_source  = SctUSART3ClockSource::kPclk1,
                        .uart4_source   = SctUart4ClockSource::kPclk1,
                        .uart5_source   = SctUart5ClockSource::kPclk1,
                        .lpuart1_source = SctLPUart1ClockSource::kPclk1,
                        .i2c1_source    = SctI2c1ClockSource::kPclk1,
                        .i2c2_source    = SctI2c2ClockSource::kPclk1,
                        .i2c3_source    = SctI2c3ClockSource::kPclk1,
                        .i2c4_source    = SctI2c4ClockSource::kPclk1,
                        .lptim1_source  = SctLPTim1ClockSource::kPclk1,
                        .sai1_source    = SctSAI1ClockSource::kSysclk,
                        .i2s_source     = SctI2SClockSource::kSysclk,
                        .fdcan_source   = SctFDCANClockSource::kHSE,
                        .rng_source     = SctRNGClockSource::kHsi48,
                        .usb_source     = SctUSBClockSource::kHsi48,
                        .adc12_source   = SctAdc12ClockSource::kSysclk,
                        .adc345_source  = SctAdc345ClockSource::kSysclk,
                        .quadspi_source = SctQuadSpiClockSource::kSysclk,
                    },
            },
    };

    static_assert(!kDefaultCoreSystemConfig.validate().has_value(), "Default core system configuration is invalid");
    static_assert(kDefaultCoreSystemConfig.rcc_config.get_hclk_freq_hz() ==
                      valle::app::kPlatformConfig.timing_config.core_clock_freq_hz,
                  "Default PLL R output frequency should be 170 MHz");

}  // namespace valle::platform::app