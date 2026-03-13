#pragma once

#include <cstdint>
#include <optional>
#include <string_view>

#include "stm32g4xx_ll_rcc.h"
#include "valle/platform/hardware/power.hpp"

namespace valle::platform
{
    // =============================================================================
    // ENUMERATIONS
    // =============================================================================
    /**
     * @brief System clock source selection.
     *
     * These represent the possible clock sources that can feed SYSCLK.
     */
    enum class SCTSYSCLKSource : uint32_t
    {
        kHSI = LL_RCC_SYS_CLKSOURCE_HSI,
        kHSE = LL_RCC_SYS_CLKSOURCE_HSE,
        kPLL = LL_RCC_SYS_CLKSOURCE_PLL
    };

    /**
     * @brief Current system clock source status.
     *
     * These values represent the hardware-reported active SYSCLK source after
     * clock switching has completed.
     */
    enum class SCTSourceStatus : uint32_t
    {
        kHSI = LL_RCC_SYS_CLKSOURCE_STATUS_HSI,
        kHSE = LL_RCC_SYS_CLKSOURCE_STATUS_HSE,
        kPLL = LL_RCC_SYS_CLKSOURCE_STATUS_PLL
    };

    enum class SCTAHBPrescaler : uint32_t
    {
        kDiv1   = LL_RCC_SYSCLK_DIV_1,
        kDiv2   = LL_RCC_SYSCLK_DIV_2,
        kDiv4   = LL_RCC_SYSCLK_DIV_4,
        kDiv8   = LL_RCC_SYSCLK_DIV_8,
        kDiv16  = LL_RCC_SYSCLK_DIV_16,
        kDiv64  = LL_RCC_SYSCLK_DIV_64,
        kDiv128 = LL_RCC_SYSCLK_DIV_128,
        kDiv256 = LL_RCC_SYSCLK_DIV_256,
        kDiv512 = LL_RCC_SYSCLK_DIV_512
    };

    enum class SCTAPB1Prescaler : uint32_t
    {
        kDiv1  = LL_RCC_APB1_DIV_1,
        kDiv2  = LL_RCC_APB1_DIV_2,
        kDiv4  = LL_RCC_APB1_DIV_4,
        kDiv8  = LL_RCC_APB1_DIV_8,
        kDiv16 = LL_RCC_APB1_DIV_16
    };

    enum class SCTAPB2Prescaler : uint32_t
    {
        kDiv1  = LL_RCC_APB2_DIV_1,
        kDiv2  = LL_RCC_APB2_DIV_2,
        kDiv4  = LL_RCC_APB2_DIV_4,
        kDiv8  = LL_RCC_APB2_DIV_8,
        kDiv16 = LL_RCC_APB2_DIV_16
    };

    enum class SCTMCOClockSource : uint32_t
    {
        kNoClock = LL_RCC_MCO1SOURCE_NOCLOCK,
        kSysclk  = LL_RCC_MCO1SOURCE_SYSCLK,
        kHSI     = LL_RCC_MCO1SOURCE_HSI,
        kHSE     = LL_RCC_MCO1SOURCE_HSE,
        kHSI48   = LL_RCC_MCO1SOURCE_HSI48,
        kPLL     = LL_RCC_MCO1SOURCE_PLLCLK,
        kLSI     = LL_RCC_MCO1SOURCE_LSI,
        kLSE     = LL_RCC_MCO1SOURCE_LSE
    };

    enum class SCTMCOPrescaler : uint32_t
    {
        kDiv1  = LL_RCC_MCO1_DIV_1,
        kDiv2  = LL_RCC_MCO1_DIV_2,
        kDiv4  = LL_RCC_MCO1_DIV_4,
        kDiv8  = LL_RCC_MCO1_DIV_8,
        kDiv16 = LL_RCC_MCO1_DIV_16
    };

    enum class SCTUSART1ClockSource : uint32_t
    {
        kPclk2  = LL_RCC_USART1_CLKSOURCE_PCLK2,
        kSysclk = LL_RCC_USART1_CLKSOURCE_SYSCLK,
        kHSI    = LL_RCC_USART1_CLKSOURCE_HSI,
        kLSE    = LL_RCC_USART1_CLKSOURCE_LSE
    };

    enum class SCTUSART2ClockSource : uint32_t
    {
        kPclk1  = LL_RCC_USART2_CLKSOURCE_PCLK1,
        kSysclk = LL_RCC_USART2_CLKSOURCE_SYSCLK,
        kHSI    = LL_RCC_USART2_CLKSOURCE_HSI,
        kLSE    = LL_RCC_USART2_CLKSOURCE_LSE
    };

    enum class SCTUSART3ClockSource : uint32_t
    {
        kPclk1  = LL_RCC_USART3_CLKSOURCE_PCLK1,
        kSysclk = LL_RCC_USART3_CLKSOURCE_SYSCLK,
        kHSI    = LL_RCC_USART3_CLKSOURCE_HSI,
        kLSE    = LL_RCC_USART3_CLKSOURCE_LSE
    };

    enum class SCTUART4ClockSource : uint32_t
    {
        kPclk1  = LL_RCC_UART4_CLKSOURCE_PCLK1,
        kSysclk = LL_RCC_UART4_CLKSOURCE_SYSCLK,
        kHSI    = LL_RCC_UART4_CLKSOURCE_HSI,
        kLSE    = LL_RCC_UART4_CLKSOURCE_LSE
    };

    enum class SCTUART5ClockSource : uint32_t
    {
        kPclk1  = LL_RCC_UART5_CLKSOURCE_PCLK1,
        kSysclk = LL_RCC_UART5_CLKSOURCE_SYSCLK,
        kHSI    = LL_RCC_UART5_CLKSOURCE_HSI,
        kLSE    = LL_RCC_UART5_CLKSOURCE_LSE
    };

    enum class SCTLPUART1ClockSource : uint32_t
    {
        kPclk1  = LL_RCC_LPUART1_CLKSOURCE_PCLK1,
        kSysclk = LL_RCC_LPUART1_CLKSOURCE_SYSCLK,
        kHSI    = LL_RCC_LPUART1_CLKSOURCE_HSI,
        kLSE    = LL_RCC_LPUART1_CLKSOURCE_LSE
    };

    enum class SCTI2C1ClockSource : uint32_t
    {
        kPclk1  = LL_RCC_I2C1_CLKSOURCE_PCLK1,
        kSysclk = LL_RCC_I2C1_CLKSOURCE_SYSCLK,
        kHSI    = LL_RCC_I2C1_CLKSOURCE_HSI
    };

    enum class SCTI2C2ClockSource : uint32_t
    {
        kPclk1  = LL_RCC_I2C2_CLKSOURCE_PCLK1,
        kSysclk = LL_RCC_I2C2_CLKSOURCE_SYSCLK,
        kHSI    = LL_RCC_I2C2_CLKSOURCE_HSI
    };

    enum class SCTI2C3ClockSource : uint32_t
    {
        kPclk1  = LL_RCC_I2C3_CLKSOURCE_PCLK1,
        kSysclk = LL_RCC_I2C3_CLKSOURCE_SYSCLK,
        kHSI    = LL_RCC_I2C3_CLKSOURCE_HSI
    };

    enum class SCTI2C4ClockSource : uint32_t
    {
        kPclk1  = LL_RCC_I2C4_CLKSOURCE_PCLK1,
        kSysclk = LL_RCC_I2C4_CLKSOURCE_SYSCLK,
        kHSI    = LL_RCC_I2C4_CLKSOURCE_HSI
    };

    enum class SCTLPTIM1ClockSource : uint32_t
    {
        kPclk1 = LL_RCC_LPTIM1_CLKSOURCE_PCLK1,
        kLSI   = LL_RCC_LPTIM1_CLKSOURCE_LSI,
        kHSI   = LL_RCC_LPTIM1_CLKSOURCE_HSI,
        kLSE   = LL_RCC_LPTIM1_CLKSOURCE_LSE
    };

    enum class SCTSAI1ClockSource : uint32_t
    {
        kSysclk = LL_RCC_SAI1_CLKSOURCE_SYSCLK,
        kPLLQ   = LL_RCC_SAI1_CLKSOURCE_PLL,
        kPIN    = LL_RCC_SAI1_CLKSOURCE_PIN,
        kHSI    = LL_RCC_SAI1_CLKSOURCE_HSI
    };

    enum class SCTI2SClockSource : uint32_t
    {
        kSysclk = LL_RCC_I2S_CLKSOURCE_SYSCLK,
        kPLLQ   = LL_RCC_I2S_CLKSOURCE_PLL,
        kPIN    = LL_RCC_I2S_CLKSOURCE_PIN,
        kHSI    = LL_RCC_I2S_CLKSOURCE_HSI
    };

    enum class SCTFDCANClockSource : uint32_t
    {
        kHSE   = LL_RCC_FDCAN_CLKSOURCE_HSE,
        kPLLQ  = LL_RCC_FDCAN_CLKSOURCE_PLL,
        kPclk1 = LL_RCC_FDCAN_CLKSOURCE_PCLK1,
    };

    enum class SCTRNGClockSource : uint32_t
    {
        kHSI48 = LL_RCC_RNG_CLKSOURCE_HSI48,
        kPLLQ  = LL_RCC_RNG_CLKSOURCE_PLL,
    };

    enum class SCTUSBClockSource : uint32_t
    {
        kHSI48 = LL_RCC_USB_CLKSOURCE_HSI48,
        kPLLQ  = LL_RCC_USB_CLKSOURCE_PLL,
    };

    enum class SCTADC12ClockSource : uint32_t
    {
        kNone   = LL_RCC_ADC12_CLKSOURCE_NONE,
        kPLLP   = LL_RCC_ADC12_CLKSOURCE_PLL,
        kSysclk = LL_RCC_ADC12_CLKSOURCE_SYSCLK
    };

    enum class SCTADC345ClockSource : uint32_t
    {
        kNone   = LL_RCC_ADC345_CLKSOURCE_NONE,
        kPLLP   = LL_RCC_ADC345_CLKSOURCE_PLL,
        kSysclk = LL_RCC_ADC345_CLKSOURCE_SYSCLK
    };

    enum class SCTQUADSPIClockSource : uint32_t
    {
        kSysclk = LL_RCC_QUADSPI_CLKSOURCE_SYSCLK,
        kHSI    = LL_RCC_QUADSPI_CLKSOURCE_HSI,
        kPLLQ   = LL_RCC_QUADSPI_CLKSOURCE_PLL
    };

    constexpr auto kResetSYSCLKSource       = SCTSYSCLKSource::kHSI;
    constexpr auto kResetAHBPrescaler       = SCTAHBPrescaler::kDiv1;
    constexpr auto kResetAPB1Prescaler      = SCTAPB1Prescaler::kDiv1;
    constexpr auto kResetAPB2Prescaler      = SCTAPB2Prescaler::kDiv1;
    constexpr auto kResetMCOClockSource     = SCTMCOClockSource::kSysclk;
    constexpr auto kResetMCOPrescaler       = SCTMCOPrescaler::kDiv1;
    constexpr auto kResetUSART1ClockSource  = SCTUSART1ClockSource::kPclk2;
    constexpr auto kResetUSART2ClockSource  = SCTUSART2ClockSource::kPclk1;
    constexpr auto kResetUSART3ClockSource  = SCTUSART3ClockSource::kPclk1;
    constexpr auto kResetUART4ClockSource   = SCTUART4ClockSource::kPclk1;
    constexpr auto kResetUART5ClockSource   = SCTUART5ClockSource::kPclk1;
    constexpr auto kResetLPUART1ClockSource = SCTLPUART1ClockSource::kPclk1;
    constexpr auto kResetI2C1ClockSource    = SCTI2C1ClockSource::kPclk1;
    constexpr auto kResetI2C2ClockSource    = SCTI2C2ClockSource::kPclk1;
    constexpr auto kResetI2C3ClockSource    = SCTI2C3ClockSource::kPclk1;
    constexpr auto kResetI2C4ClockSource    = SCTI2C4ClockSource::kPclk1;
    constexpr auto kResetLPTIM1ClockSource  = SCTLPTIM1ClockSource::kPclk1;
    constexpr auto kResetSAI1ClockSource    = SCTSAI1ClockSource::kSysclk;
    constexpr auto kResetI2SClockSource     = SCTI2SClockSource::kSysclk;
    constexpr auto kResetFDCANClockSource   = SCTFDCANClockSource::kHSE;
    constexpr auto kResetRNGClockSource     = SCTRNGClockSource::kHSI48;
    constexpr auto kResetUSBClockSource     = SCTUSBClockSource::kHSI48;
    constexpr auto kResetADC12ClockSource   = SCTADC12ClockSource::kSysclk;
    constexpr auto kResetADC345ClockSource  = SCTADC345ClockSource::kSysclk;
    constexpr auto kResetQUADSPIClockSource = SCTQUADSPIClockSource::kSysclk;

    // =============================================================================
    // INTERFACE
    // =============================================================================
    struct SCTFrequencyLimits
    {
        uint32_t sysclk_freq_hz_min = 0U;
        uint32_t sysclk_freq_hz_max = 0U;
        uint32_t hclk_freq_hz_min   = 0U;
        uint32_t hclk_freq_hz_max   = 0U;
        uint32_t pclk1_freq_hz_min  = 0U;
        uint32_t pclk1_freq_hz_max  = 0U;
        uint32_t pclk2_freq_hz_min  = 0U;
        uint32_t pclk2_freq_hz_max  = 0U;
        uint32_t flash_freq_hz_min  = 0U;
        uint32_t flash_freq_hz_max  = 0U;
    };

    /**
     * @brief Interface for the system clock source multiplexer.
     */
    struct SCTInterface
    {
        static constexpr uint32_t skDefaultSourceSwitchTimeoutCount = 1'000'000U;

        static constexpr auto skRange1BoostFrequencyLimits = SCTFrequencyLimits{.sysclk_freq_hz_min = 0U,
                                                                                .sysclk_freq_hz_max = 170'000'000U,
                                                                                .hclk_freq_hz_min   = 0U,
                                                                                .hclk_freq_hz_max   = 170'000'000U,
                                                                                .pclk1_freq_hz_min  = 0U,
                                                                                .pclk1_freq_hz_max  = 170'000'000U,
                                                                                .pclk2_freq_hz_min  = 0U,
                                                                                .pclk2_freq_hz_max  = 170'000'000U,
                                                                                .flash_freq_hz_min  = 0U,
                                                                                .flash_freq_hz_max  = 170'000'000U};

        static constexpr auto skRange1NormalFrequencyLimits = SCTFrequencyLimits{.sysclk_freq_hz_min = 0U,
                                                                                 .sysclk_freq_hz_max = 150'000'000U,
                                                                                 .hclk_freq_hz_min   = 0U,
                                                                                 .hclk_freq_hz_max   = 150'000'000U,
                                                                                 .pclk1_freq_hz_min  = 0U,
                                                                                 .pclk1_freq_hz_max  = 150'000'000U,
                                                                                 .pclk2_freq_hz_min  = 0U,
                                                                                 .pclk2_freq_hz_max  = 150'000'000U,
                                                                                 .flash_freq_hz_min  = 0U,
                                                                                 .flash_freq_hz_max  = 150'000'000U};

        static constexpr auto skRange2FrequencyLimits = SCTFrequencyLimits{.sysclk_freq_hz_min = 0U,
                                                                           .sysclk_freq_hz_max = 26'000'000U,
                                                                           .hclk_freq_hz_min   = 0U,
                                                                           .hclk_freq_hz_max   = 26'000'000U,
                                                                           .pclk1_freq_hz_min  = 0U,
                                                                           .pclk1_freq_hz_max  = 26'000'000U,
                                                                           .pclk2_freq_hz_min  = 0U,
                                                                           .pclk2_freq_hz_max  = 26'000'000U,
                                                                           .flash_freq_hz_min  = 0U,
                                                                           .flash_freq_hz_max  = 26'000'000U};
        // ---------------------------------------------------------------------
        // SYSCLK source control
        // ---------------------------------------------------------------------
        /**
         * @brief Set the requested SYSCLK source.
         *
         * @param source Desired SYSCLK source.
         */
        static void set_sysclk_source(const SCTSYSCLKSource source)
        {
            LL_RCC_SetSysClkSource(static_cast<uint32_t>(source));
        }

        /**
         * @brief Get the currently active SYSCLK source status.
         *
         * This reflects the hardware-reported active system clock source after
         * clock switching has completed.
         *
         * @return Active SYSCLK source status.
         */
        [[nodiscard]] static SCTSourceStatus get_source_status()
        {
            return static_cast<SCTSourceStatus>(LL_RCC_GetSysClkSource());
        }

        /**
         * @brief Convert a requested SYSCLK source into its corresponding status value.
         *
         * @param source Requested SYSCLK source.
         * @return Expected active source status corresponding to that source.
         */
        [[nodiscard]] static constexpr SCTSourceStatus to_status(const SCTSYSCLKSource source)
        {
            switch (source)
            {
                case SCTSYSCLKSource::kHSI:
                    return SCTSourceStatus::kHSI;
                case SCTSYSCLKSource::kHSE:
                    return SCTSourceStatus::kHSE;
                case SCTSYSCLKSource::kPLL:
                    return SCTSourceStatus::kPLL;
            }

            return SCTSourceStatus::kHSI;
        }

        /**
         * @brief Check whether the requested SYSCLK source is currently active.
         *
         * @param source Requested SYSCLK source.
         * @return true if the source switch has completed and the requested source
         *         is active, false otherwise.
         */
        [[nodiscard]] static bool is_source_active(const SCTSYSCLKSource source)
        {
            return get_source_status() == to_status(source);
        }

        // ---------------------------------------------------------------------
        // Prescaler control
        // ---------------------------------------------------------------------

        static void set_ahb_prescaler(const SCTAHBPrescaler prescaler)
        {
            LL_RCC_SetAHBPrescaler(static_cast<uint32_t>(prescaler));
        }

        static void set_apb1_prescaler(const SCTAPB1Prescaler prescaler)
        {
            LL_RCC_SetAPB1Prescaler(static_cast<uint32_t>(prescaler));
        }

        static void set_apb2_prescaler(const SCTAPB2Prescaler prescaler)
        {
            LL_RCC_SetAPB2Prescaler(static_cast<uint32_t>(prescaler));
        }

        [[nodiscard]] static SCTAHBPrescaler get_ahb_prescaler()
        {
            return static_cast<SCTAHBPrescaler>(LL_RCC_GetAHBPrescaler());
        }

        [[nodiscard]] static SCTAPB1Prescaler get_apb1_prescaler()
        {
            return static_cast<SCTAPB1Prescaler>(LL_RCC_GetAPB1Prescaler());
        }

        [[nodiscard]] static SCTAPB2Prescaler get_apb2_prescaler()
        {
            return static_cast<SCTAPB2Prescaler>(LL_RCC_GetAPB2Prescaler());
        }

        // ---------------------------------------------------------------------
        // Peripheral clock source control
        // ---------------------------------------------------------------------
        static void config_mco(const SCTMCOClockSource source, const SCTMCOPrescaler prescaler)
        {
            LL_RCC_ConfigMCO(static_cast<uint32_t>(source), static_cast<uint32_t>(prescaler));
        }

        static void set_usart1_source(const SCTUSART1ClockSource source)
        {
            LL_RCC_SetUSARTClockSource(static_cast<uint32_t>(source));
        }

        static void set_usart2_source(const SCTUSART2ClockSource source)
        {
            LL_RCC_SetUSARTClockSource(static_cast<uint32_t>(source));
        }

        static void set_usart3_source(const SCTUSART3ClockSource source)
        {
            LL_RCC_SetUSARTClockSource(static_cast<uint32_t>(source));
        }

#if defined(UART4)
        static void set_uart4_source(const SCTUART4ClockSource source)
        {
            LL_RCC_SetUARTClockSource(static_cast<uint32_t>(source));
        }

        static void set_uart5_source(const SCTUART5ClockSource source)
        {
            LL_RCC_SetUARTClockSource(static_cast<uint32_t>(source));
        }
#endif /* UART4 */

        static void set_lpuart1_source(const SCTLPUART1ClockSource source)
        {
            LL_RCC_SetLPUARTClockSource(static_cast<uint32_t>(source));
        }

        static void set_i2c1_source(const SCTI2C1ClockSource source)
        {
            LL_RCC_SetI2CClockSource(static_cast<uint32_t>(source));
        }

        static void set_i2c2_source(const SCTI2C2ClockSource source)
        {
            LL_RCC_SetI2CClockSource(static_cast<uint32_t>(source));
        }

        static void set_i2c3_source(const SCTI2C3ClockSource source)
        {
            LL_RCC_SetI2CClockSource(static_cast<uint32_t>(source));
        }

        static void set_i2c4_source(const SCTI2C4ClockSource source)
        {
            LL_RCC_SetI2CClockSource(static_cast<uint32_t>(source));
        }

        static void set_lptim1_source(const SCTLPTIM1ClockSource source)
        {
            LL_RCC_SetLPTIMClockSource(static_cast<uint32_t>(source));
        }

#if defined(SAI1)
        static void set_sai1_source(const SCTSAI1ClockSource source)
        {
            LL_RCC_SetSAIClockSource(static_cast<uint32_t>(source));
        }
#endif /* SAI1 */

#if defined(SPI_I2S_SUPPORT)
        static void set_i2s_source(const SCTI2SClockSource source)
        {
            LL_RCC_SetI2SClockSource(static_cast<uint32_t>(source));
        }
#endif /* SPI_I2S_SUPPORT */

#if defined(FDCAN1)
        static void set_fdcan_source(const SCTFDCANClockSource source)
        {
            LL_RCC_SetFDCANClockSource(static_cast<uint32_t>(source));
        }
#endif /* FDCAN1 */

        static void set_rng_source(const SCTRNGClockSource source)
        {
            LL_RCC_SetRNGClockSource(static_cast<uint32_t>(source));
        }

        static void set_usb_source(const SCTUSBClockSource source)
        {
            LL_RCC_SetUSBClockSource(static_cast<uint32_t>(source));
        }

        static void set_adc12_source(const SCTADC12ClockSource source)
        {
            LL_RCC_SetADCClockSource(static_cast<uint32_t>(source));
        }

        static void set_adc345_source(const SCTADC345ClockSource source)
        {
            LL_RCC_SetADCClockSource(static_cast<uint32_t>(source));
        }

#if defined(QUADSPI)
        static void set_quadspi_source(const SCTQUADSPIClockSource source)
        {
            LL_RCC_SetQUADSPIClockSource(static_cast<uint32_t>(source));
        }
#endif /* QUADSPI */

        [[nodiscard]] static SCTUSART1ClockSource get_usart1_source()
        {
            return static_cast<SCTUSART1ClockSource>(LL_RCC_GetUSARTClockSource(LL_RCC_USART1_CLKSOURCE));
        }

        [[nodiscard]] static SCTUSART2ClockSource get_usart2_source()
        {
            return static_cast<SCTUSART2ClockSource>(LL_RCC_GetUSARTClockSource(LL_RCC_USART2_CLKSOURCE));
        }

        [[nodiscard]] static SCTUSART3ClockSource get_usart3_source()
        {
            return static_cast<SCTUSART3ClockSource>(LL_RCC_GetUSARTClockSource(LL_RCC_USART3_CLKSOURCE));
        }

#if defined(UART4)
        [[nodiscard]] static SCTUART4ClockSource get_uart4_source()
        {
            return static_cast<SCTUART4ClockSource>(LL_RCC_GetUARTClockSource(LL_RCC_UART4_CLKSOURCE));
        }

        [[nodiscard]] static SCTUART5ClockSource get_uart5_source()
        {
            return static_cast<SCTUART5ClockSource>(LL_RCC_GetUARTClockSource(LL_RCC_UART5_CLKSOURCE));
        }
#endif

        [[nodiscard]] static SCTLPUART1ClockSource get_lpuart1_source()
        {
            return static_cast<SCTLPUART1ClockSource>(LL_RCC_GetLPUARTClockSource(LL_RCC_LPUART1_CLKSOURCE));
        }

        [[nodiscard]] static SCTI2C1ClockSource get_i2c1_source()
        {
            return static_cast<SCTI2C1ClockSource>(LL_RCC_GetI2CClockSource(LL_RCC_I2C1_CLKSOURCE));
        }

        [[nodiscard]] static SCTI2C2ClockSource get_i2c2_source()
        {
            return static_cast<SCTI2C2ClockSource>(LL_RCC_GetI2CClockSource(LL_RCC_I2C2_CLKSOURCE));
        }

        [[nodiscard]] static SCTI2C3ClockSource get_i2c3_source()
        {
            return static_cast<SCTI2C3ClockSource>(LL_RCC_GetI2CClockSource(LL_RCC_I2C3_CLKSOURCE));
        }

        [[nodiscard]] static SCTI2C4ClockSource get_i2c4_source()
        {
            return static_cast<SCTI2C4ClockSource>(LL_RCC_GetI2CClockSource(LL_RCC_I2C4_CLKSOURCE));
        }

        [[nodiscard]] static SCTLPTIM1ClockSource get_lptim1_source()
        {
            return static_cast<SCTLPTIM1ClockSource>(LL_RCC_GetLPTIMClockSource(LL_RCC_LPTIM1_CLKSOURCE));
        }

        [[nodiscard]] static SCTSAI1ClockSource get_sai1_source()
        {
            return static_cast<SCTSAI1ClockSource>(LL_RCC_GetSAIClockSource(LL_RCC_SAI1_CLKSOURCE));
        }

        [[nodiscard]] static SCTI2SClockSource get_i2s_source()
        {
            return static_cast<SCTI2SClockSource>(LL_RCC_GetSAIClockSource(LL_RCC_I2S_CLKSOURCE));
        }

#if defined(FDCAN1)
        [[nodiscard]] static SCTFDCANClockSource get_fdcan_source()
        {
            return static_cast<SCTFDCANClockSource>(LL_RCC_GetFDCANClockSource(LL_RCC_FDCAN_CLKSOURCE));
        }
#endif /* FDCAN1 */

        [[nodiscard]] static SCTRNGClockSource get_rng_source()
        {
            return static_cast<SCTRNGClockSource>(LL_RCC_GetRNGClockSource(LL_RCC_RNG_CLKSOURCE));
        }

        [[nodiscard]] static SCTUSBClockSource get_usb_source()
        {
            return static_cast<SCTUSBClockSource>(LL_RCC_GetUSBClockSource(LL_RCC_USB_CLKSOURCE));
        }

        [[nodiscard]] static SCTADC12ClockSource get_adc12_source()
        {
            return static_cast<SCTADC12ClockSource>(LL_RCC_GetADCClockSource(LL_RCC_ADC12_CLKSOURCE));
        }

        [[nodiscard]] static SCTADC345ClockSource get_adc345_source()
        {
            return static_cast<SCTADC345ClockSource>(LL_RCC_GetADCClockSource(LL_RCC_ADC345_CLKSOURCE));
        }

        [[nodiscard]] static SCTQUADSPIClockSource get_quadspi_source()
        {
            return static_cast<SCTQUADSPIClockSource>(LL_RCC_GetQUADSPIClockSource(LL_RCC_QUADSPI_CLKSOURCE));
        }

        // ---------------------------------------------------------------------
        // Divider factor helpers
        // ---------------------------------------------------------------------
        [[nodiscard]] static constexpr uint32_t get_ahb_divider_factor(const SCTAHBPrescaler prescaler)
        {
            // NOLINTBEGIN(readability-magic-numbers)
            switch (prescaler)
            {
                case SCTAHBPrescaler::kDiv1:
                    return 1;
                case SCTAHBPrescaler::kDiv2:
                    return 2;
                case SCTAHBPrescaler::kDiv4:
                    return 4;
                case SCTAHBPrescaler::kDiv8:
                    return 8;
                case SCTAHBPrescaler::kDiv16:
                    return 16;
                case SCTAHBPrescaler::kDiv64:
                    return 64;
                case SCTAHBPrescaler::kDiv128:
                    return 128;
                case SCTAHBPrescaler::kDiv256:
                    return 256;
                case SCTAHBPrescaler::kDiv512:
                    return 512;
            }
            // NOLINTEND(readability-magic-numbers)

            return 0;
        }

        [[nodiscard]] static constexpr uint32_t get_apb1_divider_factor(const SCTAPB1Prescaler prescaler)
        {
            // NOLINTBEGIN(readability-magic-numbers)
            switch (prescaler)
            {
                case SCTAPB1Prescaler::kDiv1:
                    return 1;
                case SCTAPB1Prescaler::kDiv2:
                    return 2;
                case SCTAPB1Prescaler::kDiv4:
                    return 4;
                case SCTAPB1Prescaler::kDiv8:
                    return 8;
                case SCTAPB1Prescaler::kDiv16:
                    return 16;
            }
            // NOLINTEND(readability-magic-numbers)

            return 0;
        }

        [[nodiscard]] static constexpr uint32_t get_apb2_divider_factor(const SCTAPB2Prescaler prescaler)
        {
            // NOLINTBEGIN(readability-magic-numbers)
            switch (prescaler)
            {
                case SCTAPB2Prescaler::kDiv1:
                    return 1;
                case SCTAPB2Prescaler::kDiv2:
                    return 2;
                case SCTAPB2Prescaler::kDiv4:
                    return 4;
                case SCTAPB2Prescaler::kDiv8:
                    return 8;
                case SCTAPB2Prescaler::kDiv16:
                    return 16;
            }
            // NOLINTEND(readability-magic-numbers)

            return 0;
        }

        [[nodiscard]] static constexpr uint32_t get_mco_divider_factor(const SCTMCOPrescaler prescaler)
        {
            // NOLINTBEGIN(readability-magic-numbers)
            switch (prescaler)
            {
                case SCTMCOPrescaler::kDiv1:
                    return 1;
                case SCTMCOPrescaler::kDiv2:
                    return 2;
                case SCTMCOPrescaler::kDiv4:
                    return 4;
                case SCTMCOPrescaler::kDiv8:
                    return 8;
                case SCTMCOPrescaler::kDiv16:
                    return 16;
            }
            // NOLINTEND(readability-magic-numbers)

            return 0;
        }

        // ---------------------------------------------------------------------
        // Frequency Calculation Helpers
        // ---------------------------------------------------------------------
        [[nodiscard]] static constexpr uint32_t calculate_hclk_freq_hz(const uint32_t        sysclk_hz,
                                                                       const SCTAHBPrescaler ahb_prescaler)
        {
            const uint32_t ahb_div = get_ahb_divider_factor(ahb_prescaler);
            return (ahb_div == 0U) ? 0U : (sysclk_hz / ahb_div);
        }

        [[nodiscard]] static constexpr uint32_t calculate_flash_freq_hz(const uint32_t hclk_hz)
        {
            return hclk_hz;
        }

        [[nodiscard]] static constexpr uint32_t calculate_pclk1_freq_hz(const uint32_t         hclk_hz,
                                                                        const SCTAPB1Prescaler apb1_prescaler)
        {
            const uint32_t apb1_div = get_apb1_divider_factor(apb1_prescaler);
            return (apb1_div == 0U) ? 0U : (hclk_hz / apb1_div);
        }

        [[nodiscard]] static constexpr uint32_t calculate_apb1_peripheral_freq_hz(const uint32_t pclk1_hz)
        {
            return pclk1_hz;
        }

        [[nodiscard]] static constexpr uint32_t calculate_apb1_timer_freq_hz(const uint32_t         pclk1_hz,
                                                                             const SCTAPB1Prescaler apb1_prescaler)
        {
            if (apb1_prescaler == SCTAPB1Prescaler::kDiv1)
            {
                return pclk1_hz;
            }

            return pclk1_hz * 2;
        }

        [[nodiscard]] static constexpr uint32_t calculate_pclk2_freq_hz(const uint32_t         hclk_hz,
                                                                        const SCTAPB2Prescaler apb2_prescaler)
        {
            const uint32_t apb2_div = get_apb2_divider_factor(apb2_prescaler);
            return (apb2_div == 0U) ? 0U : (hclk_hz / apb2_div);
        }

        [[nodiscard]] static constexpr uint32_t calculate_apb2_peripheral_freq_hz(const uint32_t pclk2_hz)
        {
            return pclk2_hz;
        }

        [[nodiscard]] static constexpr uint32_t calculate_apb2_timer_freq_hz(const uint32_t         pclk2_hz,
                                                                             const SCTAPB2Prescaler apb2_prescaler)
        {
            if (apb2_prescaler == SCTAPB2Prescaler::kDiv1)
            {
                return pclk2_hz;
            }

            return pclk2_hz * 2;
        }

        [[nodiscard]] static constexpr uint32_t calculate_mco_freq_hz(const uint32_t        source_hz,
                                                                      const SCTMCOPrescaler prescaler)
        {
            const uint32_t mco_div = get_mco_divider_factor(prescaler);
            return (mco_div == 0U) ? 0U : (source_hz / mco_div);
        }

        // ---------------------------------------------------------------------
        // Frequency Helpers
        // ---------------------------------------------------------------------
        [[nodiscard]] static uint32_t get_hclk_freq_hz(const uint32_t sysclk_hz)
        {
            return calculate_hclk_freq_hz(sysclk_hz, get_ahb_prescaler());
        }

        [[nodiscard]] static uint32_t get_flash_freq_hz(const uint32_t hclk_hz)
        {
            return hclk_hz;
        }

        [[nodiscard]] static uint32_t get_pclk1_freq_hz(const uint32_t hclk_hz)
        {
            return calculate_pclk1_freq_hz(hclk_hz, get_apb1_prescaler());
        }

        [[nodiscard]] static uint32_t get_apb1_peripheral_freq_hz(const uint32_t pclk1_hz)
        {
            return calculate_apb1_peripheral_freq_hz(pclk1_hz);
        }

        [[nodiscard]] static uint32_t get_apb1_timer_freq_hz(const uint32_t pclk1_hz)
        {
            return calculate_apb1_timer_freq_hz(pclk1_hz, get_apb1_prescaler());
        }

        [[nodiscard]] static uint32_t get_pclk2_freq_hz(const uint32_t hclk_hz)
        {
            return calculate_pclk2_freq_hz(hclk_hz, get_apb2_prescaler());
        }

        [[nodiscard]] static uint32_t get_apb2_peripheral_freq_hz(const uint32_t pclk2_hz)
        {
            return calculate_apb2_peripheral_freq_hz(pclk2_hz);
        }

        [[nodiscard]] static uint32_t get_apb2_timer_freq_hz(const uint32_t pclk2_hz)
        {
            return calculate_apb2_timer_freq_hz(pclk2_hz, get_apb2_prescaler());
        }

        // ---------------------------------------------------------------------
        // Validation Helpers
        // ---------------------------------------------------------------------
        [[nodiscard]] static constexpr SCTFrequencyLimits get_frequency_limits(
            const PowerVoltageRangeMode voltage_range)
        {
            switch (voltage_range)
            {
                case PowerVoltageRangeMode::kRange1Boost:
                    return skRange1BoostFrequencyLimits;
                case PowerVoltageRangeMode::kRange1Normal:
                    return skRange1NormalFrequencyLimits;
                case PowerVoltageRangeMode::kRange2:
                    return skRange2FrequencyLimits;
                default:
                    return SCTFrequencyLimits{};
            }
        }

        [[nodiscard]] static constexpr std::optional<std::string_view> validate_sysclk_freq(
            const uint32_t sysclk_hz, const SCTFrequencyLimits& limits)
        {
            if (sysclk_hz < limits.sysclk_freq_hz_min || sysclk_hz > limits.sysclk_freq_hz_max)
            {
                return "SYSCLK frequency is out of range";
            }
            return std::nullopt;
        }

        [[nodiscard]] static constexpr std::optional<std::string_view> validate_hclk_freq(
            const uint32_t hclk_hz, const SCTFrequencyLimits& limits)
        {
            if (hclk_hz < limits.hclk_freq_hz_min || hclk_hz > limits.hclk_freq_hz_max)
            {
                return "HCLK frequency is out of range";
            }
            return std::nullopt;
        }

        [[nodiscard]] static constexpr std::optional<std::string_view> validate_pclk1_freq(
            const uint32_t pclk1_hz, const SCTFrequencyLimits& limits)
        {
            if (pclk1_hz < limits.pclk1_freq_hz_min || pclk1_hz > limits.pclk1_freq_hz_max)
            {
                return "PCLK1 frequency is out of range";
            }
            return std::nullopt;
        }

        [[nodiscard]] static constexpr std::optional<std::string_view> validate_pclk2_freq(
            const uint32_t pclk2_hz, const SCTFrequencyLimits& limits)
        {
            if (pclk2_hz < limits.pclk2_freq_hz_min || pclk2_hz > limits.pclk2_freq_hz_max)
            {
                return "PCLK2 frequency is out of range";
            }
            return std::nullopt;
        }

        [[nodiscard]] static constexpr std::optional<std::string_view> validate_flash_frequency(
            const uint32_t hclk_hz, const SCTFrequencyLimits& limits)
        {
            if (hclk_hz < limits.flash_freq_hz_min || hclk_hz > limits.flash_freq_hz_max)
            {
                return "Flash frequency is out of range";
            }
            return std::nullopt;
        }

        [[nodiscard]] static constexpr std::optional<std::string_view>
        validate_config(  // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
            const uint32_t              sysclk_hz,
            const PowerVoltageRangeMode voltage_range,
            const SCTAHBPrescaler       ahb_prescaler,
            const SCTAPB1Prescaler      apb1_prescaler,
            const SCTAPB2Prescaler      apb2_prescaler)
        {
            const SCTFrequencyLimits frequency_limits = get_frequency_limits(voltage_range);

            if (sysclk_hz == 0U)
            {
                return "SYSCLK frequency cannot be zero";
            }

            if (const auto sysclk_freq_error = validate_sysclk_freq(sysclk_hz, frequency_limits))
            {
                return sysclk_freq_error;
            }

            const uint32_t hclk_hz = calculate_hclk_freq_hz(sysclk_hz, ahb_prescaler);
            if (const auto hclk_freq_error = validate_hclk_freq(hclk_hz, frequency_limits))
            {
                return hclk_freq_error;
            }

            const uint32_t flash_hz = calculate_flash_freq_hz(hclk_hz);
            if (const auto flash_freq_error = validate_flash_frequency(flash_hz, frequency_limits))
            {
                return flash_freq_error;
            }

            const uint32_t pclk1_hz = calculate_pclk1_freq_hz(hclk_hz, apb1_prescaler);
            if (const auto pclk1_freq_error = validate_pclk1_freq(pclk1_hz, frequency_limits))
            {
                return pclk1_freq_error;
            }

            const uint32_t pclk2_hz = calculate_pclk2_freq_hz(hclk_hz, apb2_prescaler);
            if (const auto pclk2_freq_error = validate_pclk2_freq(pclk2_hz, frequency_limits))
            {
                return pclk2_freq_error;
            }

            return std::nullopt;
        }
    };

}  // namespace valle::platform
