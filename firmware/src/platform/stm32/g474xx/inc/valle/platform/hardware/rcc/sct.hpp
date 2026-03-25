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
    enum class SctSYSCLKSource : uint32_t
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
    enum class SctSourceStatus : uint32_t
    {
        kHSI = LL_RCC_SYS_CLKSOURCE_STATUS_HSI,
        kHSE = LL_RCC_SYS_CLKSOURCE_STATUS_HSE,
        kPLL = LL_RCC_SYS_CLKSOURCE_STATUS_PLL
    };

    enum class SctAHBPrescaler : uint32_t
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

    enum class SctAPB1Prescaler : uint32_t
    {
        kDiv1  = LL_RCC_APB1_DIV_1,
        kDiv2  = LL_RCC_APB1_DIV_2,
        kDiv4  = LL_RCC_APB1_DIV_4,
        kDiv8  = LL_RCC_APB1_DIV_8,
        kDiv16 = LL_RCC_APB1_DIV_16
    };

    enum class SctAPB2Prescaler : uint32_t
    {
        kDiv1  = LL_RCC_APB2_DIV_1,
        kDiv2  = LL_RCC_APB2_DIV_2,
        kDiv4  = LL_RCC_APB2_DIV_4,
        kDiv8  = LL_RCC_APB2_DIV_8,
        kDiv16 = LL_RCC_APB2_DIV_16
    };

    enum class SctMCOClockSource : uint32_t
    {
        kNoClock = LL_RCC_MCO1SOURCE_NOCLOCK,
        kSysclk  = LL_RCC_MCO1SOURCE_SYSCLK,
        kHSI     = LL_RCC_MCO1SOURCE_HSI,
        kHSE     = LL_RCC_MCO1SOURCE_HSE,
        kHsi48   = LL_RCC_MCO1SOURCE_HSI48,
        kPLL     = LL_RCC_MCO1SOURCE_PLLCLK,
        kLSI     = LL_RCC_MCO1SOURCE_LSI,
        kLSE     = LL_RCC_MCO1SOURCE_LSE
    };

    enum class SctMCOPrescaler : uint32_t
    {
        kDiv1  = LL_RCC_MCO1_DIV_1,
        kDiv2  = LL_RCC_MCO1_DIV_2,
        kDiv4  = LL_RCC_MCO1_DIV_4,
        kDiv8  = LL_RCC_MCO1_DIV_8,
        kDiv16 = LL_RCC_MCO1_DIV_16
    };

    enum class SctUSART1ClockSource : uint32_t
    {
        kPclk2  = LL_RCC_USART1_CLKSOURCE_PCLK2,
        kSysclk = LL_RCC_USART1_CLKSOURCE_SYSCLK,
        kHSI    = LL_RCC_USART1_CLKSOURCE_HSI,
        kLSE    = LL_RCC_USART1_CLKSOURCE_LSE
    };

    enum class SctUSART2ClockSource : uint32_t
    {
        kPclk1  = LL_RCC_USART2_CLKSOURCE_PCLK1,
        kSysclk = LL_RCC_USART2_CLKSOURCE_SYSCLK,
        kHSI    = LL_RCC_USART2_CLKSOURCE_HSI,
        kLSE    = LL_RCC_USART2_CLKSOURCE_LSE
    };

    enum class SctUSART3ClockSource : uint32_t
    {
        kPclk1  = LL_RCC_USART3_CLKSOURCE_PCLK1,
        kSysclk = LL_RCC_USART3_CLKSOURCE_SYSCLK,
        kHSI    = LL_RCC_USART3_CLKSOURCE_HSI,
        kLSE    = LL_RCC_USART3_CLKSOURCE_LSE
    };

    enum class SctUart4ClockSource : uint32_t
    {
        kPclk1  = LL_RCC_UART4_CLKSOURCE_PCLK1,
        kSysclk = LL_RCC_UART4_CLKSOURCE_SYSCLK,
        kHSI    = LL_RCC_UART4_CLKSOURCE_HSI,
        kLSE    = LL_RCC_UART4_CLKSOURCE_LSE
    };

    enum class SctUart5ClockSource : uint32_t
    {
        kPclk1  = LL_RCC_UART5_CLKSOURCE_PCLK1,
        kSysclk = LL_RCC_UART5_CLKSOURCE_SYSCLK,
        kHSI    = LL_RCC_UART5_CLKSOURCE_HSI,
        kLSE    = LL_RCC_UART5_CLKSOURCE_LSE
    };

    enum class SctLPUart1ClockSource : uint32_t
    {
        kPclk1  = LL_RCC_LPUart1_CLKSOURCE_PCLK1,
        kSysclk = LL_RCC_LPUart1_CLKSOURCE_SYSCLK,
        kHSI    = LL_RCC_LPUart1_CLKSOURCE_HSI,
        kLSE    = LL_RCC_LPUart1_CLKSOURCE_LSE
    };

    enum class SctI2c1ClockSource : uint32_t
    {
        kPclk1  = LL_RCC_I2C1_CLKSOURCE_PCLK1,
        kSysclk = LL_RCC_I2C1_CLKSOURCE_SYSCLK,
        kHSI    = LL_RCC_I2C1_CLKSOURCE_HSI
    };

    enum class SctI2c2ClockSource : uint32_t
    {
        kPclk1  = LL_RCC_I2C2_CLKSOURCE_PCLK1,
        kSysclk = LL_RCC_I2C2_CLKSOURCE_SYSCLK,
        kHSI    = LL_RCC_I2C2_CLKSOURCE_HSI
    };

    enum class SctI2c3ClockSource : uint32_t
    {
        kPclk1  = LL_RCC_I2C3_CLKSOURCE_PCLK1,
        kSysclk = LL_RCC_I2C3_CLKSOURCE_SYSCLK,
        kHSI    = LL_RCC_I2C3_CLKSOURCE_HSI
    };

    enum class SctI2c4ClockSource : uint32_t
    {
        kPclk1  = LL_RCC_I2C4_CLKSOURCE_PCLK1,
        kSysclk = LL_RCC_I2C4_CLKSOURCE_SYSCLK,
        kHSI    = LL_RCC_I2C4_CLKSOURCE_HSI
    };

    enum class SctLPTim1ClockSource : uint32_t
    {
        kPclk1 = LL_RCC_LPTIM1_CLKSOURCE_PCLK1,
        kLSI   = LL_RCC_LPTIM1_CLKSOURCE_LSI,
        kHSI   = LL_RCC_LPTIM1_CLKSOURCE_HSI,
        kLSE   = LL_RCC_LPTIM1_CLKSOURCE_LSE
    };

    enum class SctSAI1ClockSource : uint32_t
    {
        kSysclk     = LL_RCC_SAI1_CLKSOURCE_SYSCLK,
        kPllOutputQ = LL_RCC_SAI1_CLKSOURCE_PLL,
        kPIN        = LL_RCC_SAI1_CLKSOURCE_PIN,
        kHSI        = LL_RCC_SAI1_CLKSOURCE_HSI
    };

    enum class SctI2SClockSource : uint32_t
    {
        kSysclk     = LL_RCC_I2S_CLKSOURCE_SYSCLK,
        kPllOutputQ = LL_RCC_I2S_CLKSOURCE_PLL,
        kPIN        = LL_RCC_I2S_CLKSOURCE_PIN,
        kHSI        = LL_RCC_I2S_CLKSOURCE_HSI
    };

    enum class SctFDCANClockSource : uint32_t
    {
        kHSE        = LL_RCC_FDCAN_CLKSOURCE_HSE,
        kPllOutputQ = LL_RCC_FDCAN_CLKSOURCE_PLL,
        kPclk1      = LL_RCC_FDCAN_CLKSOURCE_PCLK1,
    };

    enum class SctRNGClockSource : uint32_t
    {
        kHsi48      = LL_RCC_RNG_CLKSOURCE_HSI48,
        kPllOutputQ = LL_RCC_RNG_CLKSOURCE_PLL,
    };

    enum class SctUSBClockSource : uint32_t
    {
        kHsi48      = LL_RCC_USB_CLKSOURCE_HSI48,
        kPllOutputQ = LL_RCC_USB_CLKSOURCE_PLL,
    };

    enum class SctAdc12ClockSource : uint32_t
    {
        kNone       = LL_RCC_ADC12_CLKSOURCE_NONE,
        kPllOutputP = LL_RCC_ADC12_CLKSOURCE_PLL,
        kSysclk     = LL_RCC_ADC12_CLKSOURCE_SYSCLK
    };

    enum class SctAdc345ClockSource : uint32_t
    {
        kNone       = LL_RCC_ADC345_CLKSOURCE_NONE,
        kPllOutputP = LL_RCC_ADC345_CLKSOURCE_PLL,
        kSysclk     = LL_RCC_ADC345_CLKSOURCE_SYSCLK
    };

    enum class SctQuadSpiClockSource : uint32_t
    {
        kSysclk     = LL_RCC_QUADSPI_CLKSOURCE_SYSCLK,
        kHSI        = LL_RCC_QUADSPI_CLKSOURCE_HSI,
        kPllOutputQ = LL_RCC_QUADSPI_CLKSOURCE_PLL
    };

    constexpr auto kResetSYSCLKSource       = SctSYSCLKSource::kHSI;
    constexpr auto kResetAHBPrescaler       = SctAHBPrescaler::kDiv1;
    constexpr auto kResetAPB1Prescaler      = SctAPB1Prescaler::kDiv1;
    constexpr auto kResetAPB2Prescaler      = SctAPB2Prescaler::kDiv1;
    constexpr auto kResetMCOClockSource     = SctMCOClockSource::kSysclk;
    constexpr auto kResetMCOPrescaler       = SctMCOPrescaler::kDiv1;
    constexpr auto kResetUSART1ClockSource  = SctUSART1ClockSource::kPclk2;
    constexpr auto kResetUSART2ClockSource  = SctUSART2ClockSource::kPclk1;
    constexpr auto kResetUSART3ClockSource  = SctUSART3ClockSource::kPclk1;
    constexpr auto kResetUart4ClockSource   = SctUart4ClockSource::kPclk1;
    constexpr auto kResetUart5ClockSource   = SctUart5ClockSource::kPclk1;
    constexpr auto kResetLPUart1ClockSource = SctLPUart1ClockSource::kPclk1;
    constexpr auto kResetI2c1ClockSource    = SctI2c1ClockSource::kPclk1;
    constexpr auto kResetI2c2ClockSource    = SctI2c2ClockSource::kPclk1;
    constexpr auto kResetI2c3ClockSource    = SctI2c3ClockSource::kPclk1;
    constexpr auto kResetI2c4ClockSource    = SctI2c4ClockSource::kPclk1;
    constexpr auto kResetLPTim1ClockSource  = SctLPTim1ClockSource::kPclk1;
    constexpr auto kResetSAI1ClockSource    = SctSAI1ClockSource::kSysclk;
    constexpr auto kResetI2SClockSource     = SctI2SClockSource::kSysclk;
    constexpr auto kResetFDCANClockSource   = SctFDCANClockSource::kHSE;
    constexpr auto kResetRNGClockSource     = SctRNGClockSource::kHsi48;
    constexpr auto kResetUSBClockSource     = SctUSBClockSource::kHsi48;
    constexpr auto kResetAdc12ClockSource   = SctAdc12ClockSource::kSysclk;
    constexpr auto kResetAdc345ClockSource  = SctAdc345ClockSource::kSysclk;
    constexpr auto kResetQuadSpiClockSource = SctQuadSpiClockSource::kSysclk;

    // =============================================================================
    // INTERFACE
    // =============================================================================
    struct SctFrequencyLimits
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
    struct SctInterface
    {
        static constexpr uint32_t skDefaultSourceSwitchTimeoutCount = 1'000'000U;

        static constexpr auto skRange1BoostFrequencyLimits = SctFrequencyLimits{.sysclk_freq_hz_min = 0U,
                                                                                .sysclk_freq_hz_max = 170'000'000U,
                                                                                .hclk_freq_hz_min   = 0U,
                                                                                .hclk_freq_hz_max   = 170'000'000U,
                                                                                .pclk1_freq_hz_min  = 0U,
                                                                                .pclk1_freq_hz_max  = 170'000'000U,
                                                                                .pclk2_freq_hz_min  = 0U,
                                                                                .pclk2_freq_hz_max  = 170'000'000U,
                                                                                .flash_freq_hz_min  = 0U,
                                                                                .flash_freq_hz_max  = 170'000'000U};

        static constexpr auto skRange1NormalFrequencyLimits = SctFrequencyLimits{.sysclk_freq_hz_min = 0U,
                                                                                 .sysclk_freq_hz_max = 150'000'000U,
                                                                                 .hclk_freq_hz_min   = 0U,
                                                                                 .hclk_freq_hz_max   = 150'000'000U,
                                                                                 .pclk1_freq_hz_min  = 0U,
                                                                                 .pclk1_freq_hz_max  = 150'000'000U,
                                                                                 .pclk2_freq_hz_min  = 0U,
                                                                                 .pclk2_freq_hz_max  = 150'000'000U,
                                                                                 .flash_freq_hz_min  = 0U,
                                                                                 .flash_freq_hz_max  = 150'000'000U};

        static constexpr auto skRange2FrequencyLimits = SctFrequencyLimits{.sysclk_freq_hz_min = 0U,
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
        static void set_sysclk_source(const SctSYSCLKSource source)
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
        [[nodiscard]] static SctSourceStatus get_source_status()
        {
            return static_cast<SctSourceStatus>(LL_RCC_GetSysClkSource());
        }

        /**
         * @brief Convert a requested SYSCLK source into its corresponding status value.
         *
         * @param source Requested SYSCLK source.
         * @return Expected active source status corresponding to that source.
         */
        [[nodiscard]] static constexpr SctSourceStatus to_status(const SctSYSCLKSource source)
        {
            switch (source)
            {
                case SctSYSCLKSource::kHSI:
                    return SctSourceStatus::kHSI;
                case SctSYSCLKSource::kHSE:
                    return SctSourceStatus::kHSE;
                case SctSYSCLKSource::kPLL:
                    return SctSourceStatus::kPLL;
            }

            return SctSourceStatus::kHSI;
        }

        /**
         * @brief Check whether the requested SYSCLK source is currently active.
         *
         * @param source Requested SYSCLK source.
         * @return true if the source switch has completed and the requested source
         *         is active, false otherwise.
         */
        [[nodiscard]] static bool is_source_active(const SctSYSCLKSource source)
        {
            return get_source_status() == to_status(source);
        }

        // ---------------------------------------------------------------------
        // Prescaler control
        // ---------------------------------------------------------------------

        static void set_ahb_prescaler(const SctAHBPrescaler prescaler)
        {
            LL_RCC_SetAHBPrescaler(static_cast<uint32_t>(prescaler));
        }

        static void set_apb1_prescaler(const SctAPB1Prescaler prescaler)
        {
            LL_RCC_SetAPB1Prescaler(static_cast<uint32_t>(prescaler));
        }

        static void set_apb2_prescaler(const SctAPB2Prescaler prescaler)
        {
            LL_RCC_SetAPB2Prescaler(static_cast<uint32_t>(prescaler));
        }

        [[nodiscard]] static SctAHBPrescaler get_ahb_prescaler()
        {
            return static_cast<SctAHBPrescaler>(LL_RCC_GetAHBPrescaler());
        }

        [[nodiscard]] static SctAPB1Prescaler get_apb1_prescaler()
        {
            return static_cast<SctAPB1Prescaler>(LL_RCC_GetAPB1Prescaler());
        }

        [[nodiscard]] static SctAPB2Prescaler get_apb2_prescaler()
        {
            return static_cast<SctAPB2Prescaler>(LL_RCC_GetAPB2Prescaler());
        }

        // ---------------------------------------------------------------------
        // Peripheral clock source control
        // ---------------------------------------------------------------------
        static void config_mco(const SctMCOClockSource source, const SctMCOPrescaler prescaler)
        {
            LL_RCC_ConfigMCO(static_cast<uint32_t>(source), static_cast<uint32_t>(prescaler));
        }

        static void set_usart1_source(const SctUSART1ClockSource source)
        {
            LL_RCC_SetUSARTClockSource(static_cast<uint32_t>(source));
        }

        static void set_usart2_source(const SctUSART2ClockSource source)
        {
            LL_RCC_SetUSARTClockSource(static_cast<uint32_t>(source));
        }

        static void set_usart3_source(const SctUSART3ClockSource source)
        {
            LL_RCC_SetUSARTClockSource(static_cast<uint32_t>(source));
        }

#if defined(UART4)
        static void set_uart4_source(const SctUart4ClockSource source)
        {
            LL_RCC_SetUartClockSource(static_cast<uint32_t>(source));
        }

        static void set_uart5_source(const SctUart5ClockSource source)
        {
            LL_RCC_SetUartClockSource(static_cast<uint32_t>(source));
        }
#endif /* UART4 */

        static void set_lpuart1_source(const SctLPUart1ClockSource source)
        {
            LL_RCC_SetLPUartClockSource(static_cast<uint32_t>(source));
        }

        static void set_i2c1_source(const SctI2c1ClockSource source)
        {
            LL_RCC_SetI2CClockSource(static_cast<uint32_t>(source));
        }

        static void set_i2c2_source(const SctI2c2ClockSource source)
        {
            LL_RCC_SetI2CClockSource(static_cast<uint32_t>(source));
        }

        static void set_i2c3_source(const SctI2c3ClockSource source)
        {
            LL_RCC_SetI2CClockSource(static_cast<uint32_t>(source));
        }

        static void set_i2c4_source(const SctI2c4ClockSource source)
        {
            LL_RCC_SetI2CClockSource(static_cast<uint32_t>(source));
        }

        static void set_lptim1_source(const SctLPTim1ClockSource source)
        {
            LL_RCC_SetLPTIMClockSource(static_cast<uint32_t>(source));
        }

#if defined(SAI1)
        static void set_sai1_source(const SctSAI1ClockSource source)
        {
            LL_RCC_SetSAIClockSource(static_cast<uint32_t>(source));
        }
#endif /* SAI1 */

#if defined(SPI_I2S_SUPPORT)
        static void set_i2s_source(const SctI2SClockSource source)
        {
            LL_RCC_SetI2SClockSource(static_cast<uint32_t>(source));
        }
#endif /* SPI_I2S_SUPPORT */

#if defined(FDCAN1)
        static void set_fdcan_source(const SctFDCANClockSource source)
        {
            LL_RCC_SetFDCANClockSource(static_cast<uint32_t>(source));
        }
#endif /* FDCAN1 */

        static void set_rng_source(const SctRNGClockSource source)
        {
            LL_RCC_SetRNGClockSource(static_cast<uint32_t>(source));
        }

        static void set_usb_source(const SctUSBClockSource source)
        {
            LL_RCC_SetUSBClockSource(static_cast<uint32_t>(source));
        }

        static void set_adc12_source(const SctAdc12ClockSource source)
        {
            LL_RCC_SetADCClockSource(static_cast<uint32_t>(source));
        }

        static void set_adc345_source(const SctAdc345ClockSource source)
        {
            LL_RCC_SetADCClockSource(static_cast<uint32_t>(source));
        }

#if defined(QUADSPI)
        static void set_quadspi_source(const SctQuadSpiClockSource source)
        {
            LL_RCC_SetQUADSpiClockSource(static_cast<uint32_t>(source));
        }
#endif /* QUADSPI */

        [[nodiscard]] static SctUSART1ClockSource get_usart1_source()
        {
            return static_cast<SctUSART1ClockSource>(LL_RCC_GetUSARTClockSource(LL_RCC_USART1_CLKSOURCE));
        }

        [[nodiscard]] static SctUSART2ClockSource get_usart2_source()
        {
            return static_cast<SctUSART2ClockSource>(LL_RCC_GetUSARTClockSource(LL_RCC_USART2_CLKSOURCE));
        }

        [[nodiscard]] static SctUSART3ClockSource get_usart3_source()
        {
            return static_cast<SctUSART3ClockSource>(LL_RCC_GetUSARTClockSource(LL_RCC_USART3_CLKSOURCE));
        }

#if defined(UART4)
        [[nodiscard]] static SctUart4ClockSource get_uart4_source()
        {
            return static_cast<SctUart4ClockSource>(LL_RCC_GetUartClockSource(LL_RCC_UART4_CLKSOURCE));
        }

        [[nodiscard]] static SctUart5ClockSource get_uart5_source()
        {
            return static_cast<SctUart5ClockSource>(LL_RCC_GetUartClockSource(LL_RCC_UART5_CLKSOURCE));
        }
#endif

        [[nodiscard]] static SctLPUart1ClockSource get_lpuart1_source()
        {
            return static_cast<SctLPUart1ClockSource>(LL_RCC_GetLPUartClockSource(LL_RCC_LPUart1_CLKSOURCE));
        }

        [[nodiscard]] static SctI2c1ClockSource get_i2c1_source()
        {
            return static_cast<SctI2c1ClockSource>(LL_RCC_GetI2CClockSource(LL_RCC_I2C1_CLKSOURCE));
        }

        [[nodiscard]] static SctI2c2ClockSource get_i2c2_source()
        {
            return static_cast<SctI2c2ClockSource>(LL_RCC_GetI2CClockSource(LL_RCC_I2C2_CLKSOURCE));
        }

        [[nodiscard]] static SctI2c3ClockSource get_i2c3_source()
        {
            return static_cast<SctI2c3ClockSource>(LL_RCC_GetI2CClockSource(LL_RCC_I2C3_CLKSOURCE));
        }

        [[nodiscard]] static SctI2c4ClockSource get_i2c4_source()
        {
            return static_cast<SctI2c4ClockSource>(LL_RCC_GetI2CClockSource(LL_RCC_I2C4_CLKSOURCE));
        }

        [[nodiscard]] static SctLPTim1ClockSource get_lptim1_source()
        {
            return static_cast<SctLPTim1ClockSource>(LL_RCC_GetLPTIMClockSource(LL_RCC_LPTIM1_CLKSOURCE));
        }

        [[nodiscard]] static SctSAI1ClockSource get_sai1_source()
        {
            return static_cast<SctSAI1ClockSource>(LL_RCC_GetSAIClockSource(LL_RCC_SAI1_CLKSOURCE));
        }

        [[nodiscard]] static SctI2SClockSource get_i2s_source()
        {
            return static_cast<SctI2SClockSource>(LL_RCC_GetSAIClockSource(LL_RCC_I2S_CLKSOURCE));
        }

#if defined(FDCAN1)
        [[nodiscard]] static SctFDCANClockSource get_fdcan_source()
        {
            return static_cast<SctFDCANClockSource>(LL_RCC_GetFDCANClockSource(LL_RCC_FDCAN_CLKSOURCE));
        }
#endif /* FDCAN1 */

        [[nodiscard]] static SctRNGClockSource get_rng_source()
        {
            return static_cast<SctRNGClockSource>(LL_RCC_GetRNGClockSource(LL_RCC_RNG_CLKSOURCE));
        }

        [[nodiscard]] static SctUSBClockSource get_usb_source()
        {
            return static_cast<SctUSBClockSource>(LL_RCC_GetUSBClockSource(LL_RCC_USB_CLKSOURCE));
        }

        [[nodiscard]] static SctAdc12ClockSource get_adc12_source()
        {
            return static_cast<SctAdc12ClockSource>(LL_RCC_GetADCClockSource(LL_RCC_ADC12_CLKSOURCE));
        }

        [[nodiscard]] static SctAdc345ClockSource get_adc345_source()
        {
            return static_cast<SctAdc345ClockSource>(LL_RCC_GetADCClockSource(LL_RCC_ADC345_CLKSOURCE));
        }

        [[nodiscard]] static SctQuadSpiClockSource get_quadspi_source()
        {
            return static_cast<SctQuadSpiClockSource>(LL_RCC_GetQUADSpiClockSource(LL_RCC_QUADSPI_CLKSOURCE));
        }

        // ---------------------------------------------------------------------
        // Divider factor helpers
        // ---------------------------------------------------------------------
        [[nodiscard]] static constexpr uint32_t get_ahb_divider_factor(const SctAHBPrescaler prescaler)
        {
            // NOLINTBEGIN(readability-magic-numbers)
            switch (prescaler)
            {
                case SctAHBPrescaler::kDiv1:
                    return 1;
                case SctAHBPrescaler::kDiv2:
                    return 2;
                case SctAHBPrescaler::kDiv4:
                    return 4;
                case SctAHBPrescaler::kDiv8:
                    return 8;
                case SctAHBPrescaler::kDiv16:
                    return 16;
                case SctAHBPrescaler::kDiv64:
                    return 64;
                case SctAHBPrescaler::kDiv128:
                    return 128;
                case SctAHBPrescaler::kDiv256:
                    return 256;
                case SctAHBPrescaler::kDiv512:
                    return 512;
            }
            // NOLINTEND(readability-magic-numbers)

            return 0;
        }

        [[nodiscard]] static constexpr uint32_t get_apb1_divider_factor(const SctAPB1Prescaler prescaler)
        {
            // NOLINTBEGIN(readability-magic-numbers)
            switch (prescaler)
            {
                case SctAPB1Prescaler::kDiv1:
                    return 1;
                case SctAPB1Prescaler::kDiv2:
                    return 2;
                case SctAPB1Prescaler::kDiv4:
                    return 4;
                case SctAPB1Prescaler::kDiv8:
                    return 8;
                case SctAPB1Prescaler::kDiv16:
                    return 16;
            }
            // NOLINTEND(readability-magic-numbers)

            return 0;
        }

        [[nodiscard]] static constexpr uint32_t get_apb2_divider_factor(const SctAPB2Prescaler prescaler)
        {
            // NOLINTBEGIN(readability-magic-numbers)
            switch (prescaler)
            {
                case SctAPB2Prescaler::kDiv1:
                    return 1;
                case SctAPB2Prescaler::kDiv2:
                    return 2;
                case SctAPB2Prescaler::kDiv4:
                    return 4;
                case SctAPB2Prescaler::kDiv8:
                    return 8;
                case SctAPB2Prescaler::kDiv16:
                    return 16;
            }
            // NOLINTEND(readability-magic-numbers)

            return 0;
        }

        [[nodiscard]] static constexpr uint32_t get_mco_divider_factor(const SctMCOPrescaler prescaler)
        {
            // NOLINTBEGIN(readability-magic-numbers)
            switch (prescaler)
            {
                case SctMCOPrescaler::kDiv1:
                    return 1;
                case SctMCOPrescaler::kDiv2:
                    return 2;
                case SctMCOPrescaler::kDiv4:
                    return 4;
                case SctMCOPrescaler::kDiv8:
                    return 8;
                case SctMCOPrescaler::kDiv16:
                    return 16;
            }
            // NOLINTEND(readability-magic-numbers)

            return 0;
        }

        // ---------------------------------------------------------------------
        // Frequency Calculation Helpers
        // ---------------------------------------------------------------------
        [[nodiscard]] static constexpr uint32_t calculate_hclk_freq_hz(const uint32_t        sysclk_hz,
                                                                       const SctAHBPrescaler ahb_prescaler)
        {
            const uint32_t ahb_div = get_ahb_divider_factor(ahb_prescaler);
            return (ahb_div == 0U) ? 0U : (sysclk_hz / ahb_div);
        }

        [[nodiscard]] static constexpr uint32_t calculate_flash_freq_hz(const uint32_t hclk_hz)
        {
            return hclk_hz;
        }

        [[nodiscard]] static constexpr uint32_t calculate_pclk1_freq_hz(const uint32_t         hclk_hz,
                                                                        const SctAPB1Prescaler apb1_prescaler)
        {
            const uint32_t apb1_div = get_apb1_divider_factor(apb1_prescaler);
            return (apb1_div == 0U) ? 0U : (hclk_hz / apb1_div);
        }

        [[nodiscard]] static constexpr uint32_t calculate_apb1_peripheral_freq_hz(const uint32_t pclk1_hz)
        {
            return pclk1_hz;
        }

        [[nodiscard]] static constexpr uint32_t calculate_apb1_timer_freq_hz(const uint32_t         pclk1_hz,
                                                                             const SctAPB1Prescaler apb1_prescaler)
        {
            if (apb1_prescaler == SctAPB1Prescaler::kDiv1)
            {
                return pclk1_hz;
            }

            return pclk1_hz * 2;
        }

        [[nodiscard]] static constexpr uint32_t calculate_pclk2_freq_hz(const uint32_t         hclk_hz,
                                                                        const SctAPB2Prescaler apb2_prescaler)
        {
            const uint32_t apb2_div = get_apb2_divider_factor(apb2_prescaler);
            return (apb2_div == 0U) ? 0U : (hclk_hz / apb2_div);
        }

        [[nodiscard]] static constexpr uint32_t calculate_apb2_peripheral_freq_hz(const uint32_t pclk2_hz)
        {
            return pclk2_hz;
        }

        [[nodiscard]] static constexpr uint32_t calculate_apb2_timer_freq_hz(const uint32_t         pclk2_hz,
                                                                             const SctAPB2Prescaler apb2_prescaler)
        {
            if (apb2_prescaler == SctAPB2Prescaler::kDiv1)
            {
                return pclk2_hz;
            }

            return pclk2_hz * 2;
        }

        [[nodiscard]] static constexpr uint32_t calculate_mco_freq_hz(const uint32_t        source_hz,
                                                                      const SctMCOPrescaler prescaler)
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
        [[nodiscard]] static constexpr SctFrequencyLimits get_frequency_limits(
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
                    return SctFrequencyLimits{};
            }
        }

        [[nodiscard]] static constexpr std::optional<std::string_view> validate_sysclk_freq(
            const uint32_t sysclk_hz, const SctFrequencyLimits& limits)
        {
            if (sysclk_hz < limits.sysclk_freq_hz_min || sysclk_hz > limits.sysclk_freq_hz_max)
            {
                return "SYSCLK frequency is out of range";
            }
            return std::nullopt;
        }

        [[nodiscard]] static constexpr std::optional<std::string_view> validate_hclk_freq(
            const uint32_t hclk_hz, const SctFrequencyLimits& limits)
        {
            if (hclk_hz < limits.hclk_freq_hz_min || hclk_hz > limits.hclk_freq_hz_max)
            {
                return "HCLK frequency is out of range";
            }
            return std::nullopt;
        }

        [[nodiscard]] static constexpr std::optional<std::string_view> validate_pclk1_freq(
            const uint32_t pclk1_hz, const SctFrequencyLimits& limits)
        {
            if (pclk1_hz < limits.pclk1_freq_hz_min || pclk1_hz > limits.pclk1_freq_hz_max)
            {
                return "PCLK1 frequency is out of range";
            }
            return std::nullopt;
        }

        [[nodiscard]] static constexpr std::optional<std::string_view> validate_pclk2_freq(
            const uint32_t pclk2_hz, const SctFrequencyLimits& limits)
        {
            if (pclk2_hz < limits.pclk2_freq_hz_min || pclk2_hz > limits.pclk2_freq_hz_max)
            {
                return "PCLK2 frequency is out of range";
            }
            return std::nullopt;
        }

        [[nodiscard]] static constexpr std::optional<std::string_view> validate_flash_frequency(
            const uint32_t hclk_hz, const SctFrequencyLimits& limits)
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
            const SctAHBPrescaler       ahb_prescaler,
            const SctAPB1Prescaler      apb1_prescaler,
            const SctAPB2Prescaler      apb2_prescaler)
        {
            const SctFrequencyLimits frequency_limits = get_frequency_limits(voltage_range);

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
