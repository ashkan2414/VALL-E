#pragma once

#include <optional>
#include <string_view>

#include "valle/platform/core.hpp"
#include "valle/platform/devices/rcc/hse.hpp"
#include "valle/platform/devices/rcc/hsi.hpp"
#include "valle/platform/devices/rcc/hsi48.hpp"
#include "valle/platform/devices/rcc/lsco.hpp"
#include "valle/platform/devices/rcc/lse.hpp"
#include "valle/platform/devices/rcc/lsi.hpp"
#include "valle/platform/devices/rcc/pll.hpp"
#include "valle/platform/devices/rcc/sct.hpp"

namespace valle::platform
{
    // =============================================================================
    // RCC INFO DEVICE
    // =============================================================================
    /**
     * @brief Aggregated read-only information device for the entire RCC system.
     */
    template <typename T = void>
    class RccInfoDevice
    {
    public:
        struct Descriptor : public SharedDeviceDescriptor
        {
            constexpr static bool skNeedsInit = false;
        };

        using InjectDevices = TypeList<HsiOscillatorInfoDevice<>,
                                       HseOscillatorInfoDevice<>,
                                       Hsi48OscillatorInfoDevice<>,
                                       LsiOscillatorInfoDevice<>,
                                       LseOscillatorInfoDevice<>,
                                       PllInfoDevice<>,
                                       SctInfoDevice<>>;

    private:
        [[no_unique_address]] DeviceRef<HsiOscillatorInfoDevice<>>   m_hsi_info;
        [[no_unique_address]] DeviceRef<HseOscillatorInfoDevice<>>   m_hse_info;
        [[no_unique_address]] DeviceRef<Hsi48OscillatorInfoDevice<>> m_hsi48_info;
        [[no_unique_address]] DeviceRef<LsiOscillatorInfoDevice<>>   m_lsi_info;
        [[no_unique_address]] DeviceRef<LseOscillatorInfoDevice<>>   m_lse_info;
        [[no_unique_address]] DeviceRef<PllInfoDevice<>>             m_pll_info;
        [[no_unique_address]] DeviceRef<SctInfoDevice<>>             m_sct_info;

    public:
        RccInfoDevice() = delete;

        RccInfoDevice(DeviceRef<HsiOscillatorInfoDevice<>>&&   hsi,
                      DeviceRef<HseOscillatorInfoDevice<>>&&   hse,
                      DeviceRef<Hsi48OscillatorInfoDevice<>>&& hsi48,
                      DeviceRef<LsiOscillatorInfoDevice<>>&&   lsi,
                      DeviceRef<LseOscillatorInfoDevice<>>&&   lse,
                      DeviceRef<PllInfoDevice<>>&&             pll,
                      DeviceRef<SctInfoDevice<>>&&             sct)
            : m_hsi_info(std::move(hsi))
            , m_hse_info(std::move(hse))
            , m_hsi48_info(std::move(hsi48))
            , m_lsi_info(std::move(lsi))
            , m_lse_info(std::move(lse))
            , m_pll_info(std::move(pll))
            , m_sct_info(std::move(sct))
        {
        }

        // ---------------------------------------------------------------------
        // OSCILLATOR STATUS & FREQUENCY
        // ---------------------------------------------------------------------
        [[nodiscard]] uint32_t get_hsi_freq_hz() const
        {
            return m_hsi_info->get_frequency_hz();
        }
        [[nodiscard]] bool is_hsi_ready() const
        {
            return m_hsi_info->is_ready();
        }

        [[nodiscard]] uint32_t get_hse_freq_hz() const
        {
            return m_hse_info->get_frequency_hz();
        }
        [[nodiscard]] bool is_hse_ready() const
        {
            return m_hse_info->is_ready();
        }

        [[nodiscard]] uint32_t get_hsi48_freq_hz() const
        {
            return m_hsi48_info->get_frequency_hz();
        }

        [[nodiscard]] bool is_hsi48_ready() const
        {
            return m_hsi48_info->is_ready();
        }

        [[nodiscard]] uint32_t get_lsi_freq_hz() const
        {
            return m_lsi_info->get_frequency_hz();
        }
        [[nodiscard]] bool is_lsi_ready() const
        {
            return m_lsi_info->is_ready();
        }

        [[nodiscard]] uint32_t get_lse_freq_hz() const
        {
            return m_lse_info->get_frequency_hz();
        }
        [[nodiscard]] bool is_lse_ready() const
        {
            return m_lse_info->is_ready();
        }

        // ---------------------------------------------------------------------
        // PLL STATUS & FREQUENCIES
        // ---------------------------------------------------------------------
        [[nodiscard]] bool is_pll_ready() const
        {
            return m_pll_info->is_ready();
        }
        [[nodiscard]] PllSource get_pll_source() const
        {
            return m_pll_info->get_source();
        }
        [[nodiscard]] uint32_t get_pll_source_freq_hz() const
        {
            return m_pll_info->get_source_freq_hz();
        }

        [[nodiscard]] uint32_t get_pll_r_output_freq_hz() const
        {
            return m_pll_info->is_r_output_enabled() ? m_pll_info->get_r_output_freq_hz() : 0U;
        }

        [[nodiscard]] uint32_t get_pll_p_output_freq_hz() const
        {
            return m_pll_info->is_p_output_enabled() ? m_pll_info->get_p_output_freq_hz() : 0U;
        }

        [[nodiscard]] uint32_t get_pll_q_output_freq_hz() const
        {
            return m_pll_info->is_q_output_enabled() ? m_pll_info->get_q_output_freq_hz() : 0U;
        }

        // ---------------------------------------------------------------------
        // PRESCALERS
        // ---------------------------------------------------------------------
        [[nodiscard]] SctAHBPrescaler get_ahb_prescaler() const
        {
            return m_sct_info->get_ahb_prescaler();
        }
        [[nodiscard]] SctAPB1Prescaler get_apb1_prescaler() const
        {
            return m_sct_info->get_apb1_prescaler();
        }
        [[nodiscard]] SctAPB2Prescaler get_apb2_prescaler() const
        {
            return m_sct_info->get_apb2_prescaler();
        }

        // ---------------------------------------------------------------------
        // SYSTEM CLOCK TREE (SCT)
        // ---------------------------------------------------------------------
        [[nodiscard]] SctSourceStatus get_sysclk_source() const
        {
            return m_sct_info->get_source_status();
        }

        [[nodiscard]] uint32_t get_sysclk_freq_hz() const
        {
            return m_sct_info->get_sysclk_freq_hz();
        }

        [[nodiscard]] uint32_t get_hclk_freq_hz() const
        {
            return m_sct_info->get_hclk_freq_hz();
        }

        [[nodiscard]] uint32_t get_pclk1_freq_hz() const
        {
            return m_sct_info->get_pclk1_freq_hz();
        }

        [[nodiscard]] uint32_t get_apb1_peripheral_freq_hz() const
        {
            return m_sct_info->get_apb1_peripheral_freq_hz();
        }

        [[nodiscard]] uint32_t get_apb1_timer_freq_hz() const
        {
            return m_sct_info->get_apb1_timer_freq_hz();
        }

        [[nodiscard]] uint32_t get_pclk2_freq_hz() const
        {
            return m_sct_info->get_pclk2_freq_hz();
        }

        [[nodiscard]] uint32_t get_apb2_peripheral_freq_hz() const
        {
            return m_sct_info->get_apb2_peripheral_freq_hz();
        }

        [[nodiscard]] uint32_t get_apb2_timer_freq_hz() const
        {
            return m_sct_info->get_apb2_timer_freq_hz();
        }

        // ---------------------------------------------------------------------
        // PERIPHERAL CLOCK SOURCES
        // ---------------------------------------------------------------------
        [[nodiscard]] SctUSART1ClockSource get_usart1_source() const
        {
            return m_sct_info->get_usart1_source();
        }

        [[nodiscard]] SctUSART2ClockSource get_usart2_source() const
        {
            return m_sct_info->get_usart2_source();
        }

        [[nodiscard]] SctUSART3ClockSource get_usart3_source() const
        {
            return m_sct_info->get_usart3_source();
        }

#if defined(UART4)
        [[nodiscard]] SctUart4ClockSource get_uart4_source() const
        {
            return m_sct_info->get_uart4_source();
        }

        [[nodiscard]] SctUart5ClockSource get_uart5_source() const
        {
            return m_sct_info->get_uart5_source();
        }
#endif

        [[nodiscard]] SctLPUart1ClockSource get_lpuart1_source() const
        {
            return m_sct_info->get_lpuart1_source();
        }

        [[nodiscard]] SctI2c1ClockSource get_i2c1_source() const
        {
            return m_sct_info->get_i2c1_source();
        }

        [[nodiscard]] SctI2c2ClockSource get_i2c2_source() const
        {
            return m_sct_info->get_i2c2_source();
        }

        [[nodiscard]] SctI2c3ClockSource get_i2c3_source() const
        {
            return m_sct_info->get_i2c3_source();
        }

        [[nodiscard]] SctI2c4ClockSource get_i2c4_source() const
        {
            return m_sct_info->get_i2c4_source();
        }

        [[nodiscard]] SctLPTim1ClockSource get_lptim1_source() const
        {
            return m_sct_info->get_lptim1_source();
        }

        [[nodiscard]] SctSAI1ClockSource get_sai1_source() const
        {
            return m_sct_info->get_sai1_source();
        }

        [[nodiscard]] SctI2SClockSource get_i2s_source() const
        {
            return m_sct_info->get_i2s_source();
        }

#if defined(FDCAN1)
        [[nodiscard]] SctFDCANClockSource get_fdcan_source() const
        {
            return m_sct_info->get_fdcan_source();
        }
#endif /* FDCAN1 */

        [[nodiscard]] SctRNGClockSource get_rng_source() const
        {
            return m_sct_info->get_rng_source();
        }

        [[nodiscard]] SctUSBClockSource get_usb_source() const
        {
            return m_sct_info->get_usb_source();
        }

        [[nodiscard]] SctAdc12ClockSource get_adc12_source() const
        {
            return m_sct_info->get_adc12_source();
        }

        [[nodiscard]] SctAdc345ClockSource get_adc345_source() const
        {
            return m_sct_info->get_adc345_source();
        }

        [[nodiscard]] SctQuadSpiClockSource get_quadspi_source() const
        {
            return m_sct_info->get_quadspi_source();
        }

        [[nodiscard]] uint32_t get_usart1_freq_hz() const
        {
            switch (get_usart1_source())
            {
                case SctUSART1ClockSource::kPclk2:
                    return get_pclk2_freq_hz();
                case SctUSART1ClockSource::kSysclk:
                    return get_sysclk_freq_hz();
                case SctUSART1ClockSource::kHSI:
                    return get_hsi_freq_hz();
                case SctUSART1ClockSource::kLSE:
                    return get_lse_freq_hz();
            }

            return 0U;
        }

        [[nodiscard]] uint32_t get_usart2_freq_hz() const
        {
            switch (get_usart2_source())
            {
                case SctUSART2ClockSource::kPclk1:
                    return get_pclk1_freq_hz();
                case SctUSART2ClockSource::kSysclk:
                    return get_sysclk_freq_hz();
                case SctUSART2ClockSource::kHSI:
                    return get_hsi_freq_hz();
                case SctUSART2ClockSource::kLSE:
                    return get_lse_freq_hz();
            }

            return 0U;
        }

        [[nodiscard]] uint32_t get_usart3_freq_hz() const
        {
            switch (get_usart3_source())
            {
                case SctUSART3ClockSource::kPclk1:
                    return get_pclk1_freq_hz();
                case SctUSART3ClockSource::kSysclk:
                    return get_sysclk_freq_hz();
                case SctUSART3ClockSource::kHSI:
                    return get_hsi_freq_hz();
                case SctUSART3ClockSource::kLSE:
                    return get_lse_freq_hz();
            }

            return 0U;
        }

        [[nodiscard]] uint32_t get_uart4_freq_hz() const
        {
            switch (get_uart4_source())
            {
                case SctUart4ClockSource::kPclk1:
                    return get_pclk1_freq_hz();
                case SctUart4ClockSource::kSysclk:
                    return get_sysclk_freq_hz();
                case SctUart4ClockSource::kHSI:
                    return get_hsi_freq_hz();
                case SctUart4ClockSource::kLSE:
                    return get_lse_freq_hz();
            }

            return 0U;
        }

        [[nodiscard]] uint32_t get_uart5_freq_hz() const
        {
            switch (get_uart5_source())
            {
                case SctUart5ClockSource::kPclk1:
                    return get_pclk1_freq_hz();
                case SctUart5ClockSource::kSysclk:
                    return get_sysclk_freq_hz();
                case SctUart5ClockSource::kHSI:
                    return get_hsi_freq_hz();
                case SctUart5ClockSource::kLSE:
                    return get_lse_freq_hz();
            }

            return 0U;
        }

        [[nodiscard]] uint32_t get_lpuart1_freq_hz() const
        {
            switch (get_lpuart1_source())
            {
                case SctLPUart1ClockSource::kPclk1:
                    return get_pclk1_freq_hz();
                case SctLPUart1ClockSource::kSysclk:
                    return get_sysclk_freq_hz();
                case SctLPUart1ClockSource::kHSI:
                    return get_hsi_freq_hz();
                case SctLPUart1ClockSource::kLSE:
                    return get_lse_freq_hz();
            }

            return 0U;
        }

        [[nodiscard]] uint32_t get_i2c1_freq_hz() const
        {
            switch (get_i2c1_source())
            {
                case SctI2c1ClockSource::kPclk1:
                    return get_pclk1_freq_hz();
                case SctI2c1ClockSource::kSysclk:
                    return get_sysclk_freq_hz();
                case SctI2c1ClockSource::kHSI:
                    return get_hsi_freq_hz();
            }

            return 0U;
        }

        [[nodiscard]] uint32_t get_i2c2_freq_hz() const
        {
            switch (get_i2c2_source())
            {
                case SctI2c2ClockSource::kPclk1:
                    return get_pclk1_freq_hz();
                case SctI2c2ClockSource::kSysclk:
                    return get_sysclk_freq_hz();
                case SctI2c2ClockSource::kHSI:
                    return get_hsi_freq_hz();
            }

            return 0U;
        }

        [[nodiscard]] uint32_t get_i2c3_freq_hz() const
        {
            switch (get_i2c3_source())
            {
                case SctI2c3ClockSource::kPclk1:
                    return get_pclk1_freq_hz();
                case SctI2c3ClockSource::kSysclk:
                    return get_sysclk_freq_hz();
                case SctI2c3ClockSource::kHSI:
                    return get_hsi_freq_hz();
            }

            return 0U;
        }

        [[nodiscard]] uint32_t get_i2c4_freq_hz() const
        {
            switch (get_i2c4_source())
            {
                case SctI2c4ClockSource::kPclk1:
                    return get_pclk1_freq_hz();
                case SctI2c4ClockSource::kSysclk:
                    return get_sysclk_freq_hz();
                case SctI2c4ClockSource::kHSI:
                    return get_hsi_freq_hz();
            }

            return 0U;
        }

        [[nodiscard]] uint32_t get_lptim1_freq_hz() const
        {
            switch (get_lptim1_source())
            {
                case SctLPTim1ClockSource::kPclk1:
                    return get_pclk1_freq_hz();
                case SctLPTim1ClockSource::kLSI:
                    return get_lsi_freq_hz();
                case SctLPTim1ClockSource::kHSI:
                    return get_hsi_freq_hz();
                case SctLPTim1ClockSource::kLSE:
                    return get_lse_freq_hz();
            }

            return 0U;
        }

        [[nodiscard]] std::optional<uint32_t> get_sai1_freq_hz() const
        {
            switch (get_sai1_source())
            {
                case SctSAI1ClockSource::kSysclk:
                    return get_sysclk_freq_hz();
                case SctSAI1ClockSource::kPllOutputQ:
                    return get_pll_q_output_freq_hz();
                case SctSAI1ClockSource::kPIN:
                    return std::nullopt;
                case SctSAI1ClockSource::kHSI:
                    return get_hsi_freq_hz();
            }

            return std::nullopt;
        }

        [[nodiscard]] std::optional<uint32_t> get_i2s_freq_hz() const
        {
            switch (get_i2s_source())
            {
                case SctI2SClockSource::kSysclk:
                    return get_sysclk_freq_hz();
                case SctI2SClockSource::kPllOutputQ:
                    return get_pll_q_output_freq_hz();
                case SctI2SClockSource::kPIN:
                    return std::nullopt;
                case SctI2SClockSource::kHSI:
                    return get_hsi_freq_hz();
            }

            return std::nullopt;
        }

        [[nodiscard]] uint32_t get_fdcan_freq_hz() const
        {
            switch (get_fdcan_source())
            {
                case SctFDCANClockSource::kHSE:
                    return get_hse_freq_hz();
                case SctFDCANClockSource::kPllOutputQ:
                    return get_pll_q_output_freq_hz();
                case SctFDCANClockSource::kPclk1:
                    return get_pclk1_freq_hz();
            }

            return 0U;
        }

        [[nodiscard]] uint32_t get_rng_freq_hz() const
        {
            switch (get_rng_source())
            {
                case SctRNGClockSource::kHsi48:
                    return get_hsi48_freq_hz();
                case SctRNGClockSource::kPllOutputQ:
                    return get_pll_q_output_freq_hz();
            }

            return 0U;
        }

        [[nodiscard]] uint32_t get_usb_freq_hz() const
        {
            switch (get_usb_source())
            {
                case SctUSBClockSource::kHsi48:
                    return get_hsi48_freq_hz();
                case SctUSBClockSource::kPllOutputQ:
                    return get_pll_q_output_freq_hz();
            }

            return 0U;
        }

        [[nodiscard]] uint32_t get_adc12_freq_hz() const
        {
            switch (get_adc12_source())
            {
                case SctAdc12ClockSource::kNone:
                    return get_hclk_freq_hz();
                case SctAdc12ClockSource::kPllOutputP:
                    return get_pll_p_output_freq_hz();
                case SctAdc12ClockSource::kSysclk:
                    return get_sysclk_freq_hz();
            }

            return 0U;
        }

        [[nodiscard]] uint32_t get_adc345_freq_hz() const
        {
            switch (get_adc345_source())
            {
                case SctAdc345ClockSource::kNone:
                    return get_hclk_freq_hz();
                case SctAdc345ClockSource::kPllOutputP:
                    return get_pll_p_output_freq_hz();
                case SctAdc345ClockSource::kSysclk:
                    return get_sysclk_freq_hz();
            }

            return 0U;
        }

        [[nodiscard]] uint32_t get_quadspi_freq_hz() const
        {
            switch (get_quadspi_source())
            {
                case SctQuadSpiClockSource::kSysclk:
                    return get_sysclk_freq_hz();
                case SctQuadSpiClockSource::kHSI:
                    return get_hsi_freq_hz();
                case SctQuadSpiClockSource::kPllOutputQ:
                    return get_pll_q_output_freq_hz();
            }

            return 0U;
        }
    };

    // =============================================================================
    // RCC DEVICE
    // =============================================================================

    // -----------------------------------------------------------------------------
    // CONFIGURATION
    // -----------------------------------------------------------------------------
    /**
     * @brief Configuration for the RCC device.
     */
    struct RccConfig
    {
        // Oscillators
        HsiOscillatorConfig   hsi{};
        HseOscillatorConfig   hse{};
        Hsi48OscillatorConfig hsi48{};
        LsiOscillatorConfig   lsi{};
        LseOscillatorConfig   lse{};

        // PLL
        PllConfig pll{};

        // System clock tree
        SctConfig sct{};

        [[nodiscard]] constexpr std::optional<std::string_view> validate(
            const PowerVoltageRangeMode voltage_range) const
        {
            if (const auto pll_error = pll.validate(get_pll_source_freq_hz(), voltage_range))
            {
                return pll_error;
            }

            switch (sct.sysclk_source)
            {
                case SctSYSCLKSource::kHSI:
                {
                    if (!hsi.enabled)
                    {
                        return "SCT source is set to HSI, but HSI oscillator is not enabled";
                    }

                    break;
                }
                case SctSYSCLKSource::kHSE:
                {
                    if (!hse.enabled)
                    {
                        return "SCT source is set to HSE, but HSE oscillator is not enabled";
                    }

                    break;
                }
                case SctSYSCLKSource::kPLL:
                {
                    if (!pll.enabled)
                    {
                        return "SCT source is set to PLL, but PLL is not enabled";
                    }

                    if (!pll.r_output_enabled)
                    {
                        return "SCT source is set to PLL, but PLL R output is not configured";
                    }

                    break;
                }
                default:
                    return "Invalid SCT source";
            }

            return sct.validate(get_pll_r_output_freq_hz(), voltage_range);
        }

        // -------------------------------------------------------------------------
        // OSCILLATORS
        // -------------------------------------------------------------------------

        [[nodiscard]] constexpr uint32_t get_hsi_freq_hz() const
        {
            return hsi.get_frequency_hz();
        }

        [[nodiscard]] constexpr uint32_t get_hse_freq_hz() const
        {
            return hse.get_frequency_hz();
        }

        [[nodiscard]] constexpr uint32_t get_hsi48_freq_hz() const
        {
            return hsi48.get_frequency_hz();
        }

        [[nodiscard]] constexpr uint32_t get_lsi_freq_hz() const
        {
            return lsi.get_frequency_hz();
        }

        [[nodiscard]] constexpr uint32_t get_lse_freq_hz() const
        {
            return lse.get_frequency_hz();
        }

        // -------------------------------------------------------------------------
        // PLL
        // -------------------------------------------------------------------------

        [[nodiscard]] constexpr uint32_t get_pll_source_freq_hz() const
        {
            switch (pll.source)
            {
                case PllSource::kHSI:
                    return get_hsi_freq_hz();

                case PllSource::kHSE:
                    return get_hse_freq_hz();

                default:
                    return 0;
            }
        }

        [[nodiscard]] constexpr uint32_t get_pll_input_freq_hz() const
        {
            return pll.get_input_freq_hz(get_pll_source_freq_hz());
        }

        [[nodiscard]] constexpr uint32_t get_pll_vco_freq_hz() const
        {
            return pll.get_vco_freq_hz(get_pll_source_freq_hz());
        }

        [[nodiscard]] constexpr uint32_t get_pll_r_output_freq_hz() const
        {
            return pll.get_r_output_freq_hz(get_pll_vco_freq_hz());
        }

        [[nodiscard]] constexpr uint32_t get_pll_p_output_freq_hz() const
        {
            return pll.get_p_output_freq_hz(get_pll_vco_freq_hz());
        }

        [[nodiscard]] constexpr uint32_t get_pll_q_output_freq_hz() const
        {
            return pll.get_q_output_freq_hz(get_pll_vco_freq_hz());
        }

        // -------------------------------------------------------------------------
        // MAIN SYSTEM CLOCK TREE
        // -------------------------------------------------------------------------

        [[nodiscard]] constexpr uint32_t get_sysclk_freq_hz() const
        {
            switch (sct.sysclk_source)
            {
                case SctSYSCLKSource::kHSI:
                    return get_hsi_freq_hz();

                case SctSYSCLKSource::kHSE:
                    return get_hse_freq_hz();

                case SctSYSCLKSource::kPLL:
                    return get_pll_r_output_freq_hz();
            }

            return 0U;
        }

        [[nodiscard]] constexpr uint32_t get_hclk_freq_hz() const
        {
            return sct.get_hclk_freq_hz(get_sysclk_freq_hz());
        }

        [[nodiscard]] constexpr uint32_t get_pclk1_freq_hz() const
        {
            return sct.get_pclk1_freq_hz(get_hclk_freq_hz());
        }

        [[nodiscard]] constexpr uint32_t get_apb1_peripheral_freq_hz() const
        {
            return sct.get_apb1_peripheral_freq_hz(get_pclk1_freq_hz());
        }

        [[nodiscard]] constexpr uint32_t get_apb1_timer_freq_hz() const
        {
            return sct.get_apb1_timer_freq_hz(get_pclk1_freq_hz());
        }

        [[nodiscard]] constexpr uint32_t get_pclk2_freq_hz() const
        {
            return sct.get_pclk2_freq_hz(get_hclk_freq_hz());
        }

        [[nodiscard]] constexpr uint32_t get_apb2_peripheral_freq_hz() const
        {
            return sct.get_apb2_peripheral_freq_hz(get_pclk2_freq_hz());
        }

        [[nodiscard]] constexpr uint32_t get_apb2_timer_freq_hz() const
        {
            return sct.get_apb2_timer_freq_hz(get_pclk2_freq_hz());
        }

        // -------------------------------------------------------------------------
        // PERIPHERAL CLOCK TREE
        // -------------------------------------------------------------------------

        [[nodiscard]] constexpr uint32_t get_mco_source_freq_hz() const
        {
            switch (sct.mco_source)
            {
                case SctMCOClockSource::kNoClock:
                    return 0U;
                case SctMCOClockSource::kHSI:
                    return get_hsi_freq_hz();
                case SctMCOClockSource::kHSE:
                    return get_hse_freq_hz();
                case SctMCOClockSource::kHsi48:
                    return get_hsi48_freq_hz();
                case SctMCOClockSource::kLSI:
                    return get_lsi_freq_hz();
                case SctMCOClockSource::kLSE:
                    return get_lse_freq_hz();
                case SctMCOClockSource::kPLL:
                    return get_pll_r_output_freq_hz();
                case SctMCOClockSource::kSysclk:
                    return get_sysclk_freq_hz();
            }

            return 0U;
        }

        [[nodiscard]] constexpr uint32_t get_mco_freq_hz() const
        {
            return sct.get_mco_freq_hz(get_mco_source_freq_hz());
        }

        [[nodiscard]] constexpr uint32_t get_usart1_freq_hz() const
        {
            switch (sct.usart1_source)
            {
                case SctUSART1ClockSource::kPclk2:
                    return get_pclk2_freq_hz();
                case SctUSART1ClockSource::kSysclk:
                    return get_sysclk_freq_hz();
                case SctUSART1ClockSource::kHSI:
                    return get_hsi_freq_hz();
                case SctUSART1ClockSource::kLSE:
                    return get_lse_freq_hz();
            }

            return 0U;
        }

        [[nodiscard]] constexpr uint32_t get_usart2_freq_hz() const
        {
            switch (sct.usart2_source)
            {
                case SctUSART2ClockSource::kPclk1:
                    return get_pclk1_freq_hz();
                case SctUSART2ClockSource::kSysclk:
                    return get_sysclk_freq_hz();
                case SctUSART2ClockSource::kHSI:
                    return get_hsi_freq_hz();
                case SctUSART2ClockSource::kLSE:
                    return get_lse_freq_hz();
            }

            return 0U;
        }

        [[nodiscard]] constexpr uint32_t get_usart3_freq_hz() const
        {
            switch (sct.usart3_source)
            {
                case SctUSART3ClockSource::kPclk1:
                    return get_pclk1_freq_hz();
                case SctUSART3ClockSource::kSysclk:
                    return get_sysclk_freq_hz();
                case SctUSART3ClockSource::kHSI:
                    return get_hsi_freq_hz();
                case SctUSART3ClockSource::kLSE:
                    return get_lse_freq_hz();
            }

            return 0U;
        }

        [[nodiscard]] constexpr uint32_t get_uart4_freq_hz() const
        {
            switch (sct.uart4_source)
            {
                case SctUart4ClockSource::kPclk1:
                    return get_pclk1_freq_hz();
                case SctUart4ClockSource::kSysclk:
                    return get_sysclk_freq_hz();
                case SctUart4ClockSource::kHSI:
                    return get_hsi_freq_hz();
                case SctUart4ClockSource::kLSE:
                    return get_lse_freq_hz();
            }

            return 0U;
        }

        [[nodiscard]] constexpr uint32_t get_uart5_freq_hz() const
        {
            switch (sct.uart5_source)
            {
                case SctUart5ClockSource::kPclk1:
                    return get_pclk1_freq_hz();
                case SctUart5ClockSource::kSysclk:
                    return get_sysclk_freq_hz();
                case SctUart5ClockSource::kHSI:
                    return get_hsi_freq_hz();
                case SctUart5ClockSource::kLSE:
                    return get_lse_freq_hz();
            }

            return 0U;
        }

        [[nodiscard]] constexpr uint32_t get_lpuart1_freq_hz() const
        {
            switch (sct.lpuart1_source)
            {
                case SctLPUart1ClockSource::kPclk1:
                    return get_pclk1_freq_hz();
                case SctLPUart1ClockSource::kSysclk:
                    return get_sysclk_freq_hz();
                case SctLPUart1ClockSource::kHSI:
                    return get_hsi_freq_hz();
                case SctLPUart1ClockSource::kLSE:
                    return get_lse_freq_hz();
            }

            return 0U;
        }

        [[nodiscard]] constexpr uint32_t get_i2c1_freq_hz() const
        {
            switch (sct.i2c1_source)
            {
                case SctI2c1ClockSource::kPclk1:
                    return get_pclk1_freq_hz();
                case SctI2c1ClockSource::kSysclk:
                    return get_sysclk_freq_hz();
                case SctI2c1ClockSource::kHSI:
                    return get_hsi_freq_hz();
            }

            return 0U;
        }

        [[nodiscard]] constexpr uint32_t get_i2c2_freq_hz() const
        {
            switch (sct.i2c2_source)
            {
                case SctI2c2ClockSource::kPclk1:
                    return get_pclk1_freq_hz();
                case SctI2c2ClockSource::kSysclk:
                    return get_sysclk_freq_hz();
                case SctI2c2ClockSource::kHSI:
                    return get_hsi_freq_hz();
            }

            return 0U;
        }

        [[nodiscard]] constexpr uint32_t get_i2c3_freq_hz() const
        {
            switch (sct.i2c3_source)
            {
                case SctI2c3ClockSource::kPclk1:
                    return get_pclk1_freq_hz();
                case SctI2c3ClockSource::kSysclk:
                    return get_sysclk_freq_hz();
                case SctI2c3ClockSource::kHSI:
                    return get_hsi_freq_hz();
            }

            return 0U;
        }

        [[nodiscard]] constexpr uint32_t get_i2c4_freq_hz() const
        {
            switch (sct.i2c4_source)
            {
                case SctI2c4ClockSource::kPclk1:
                    return get_pclk1_freq_hz();
                case SctI2c4ClockSource::kSysclk:
                    return get_sysclk_freq_hz();
                case SctI2c4ClockSource::kHSI:
                    return get_hsi_freq_hz();
            }

            return 0U;
        }

        [[nodiscard]] constexpr uint32_t get_lptim1_freq_hz() const
        {
            switch (sct.lptim1_source)
            {
                case SctLPTim1ClockSource::kPclk1:
                    return get_pclk1_freq_hz();
                case SctLPTim1ClockSource::kLSI:
                    return get_lsi_freq_hz();
                case SctLPTim1ClockSource::kHSI:
                    return get_hsi_freq_hz();
                case SctLPTim1ClockSource::kLSE:
                    return get_lse_freq_hz();
            }

            return 0U;
        }

        [[nodiscard]] constexpr std::optional<uint32_t> get_sai1_freq_hz() const
        {
            switch (sct.sai1_source)
            {
                case SctSAI1ClockSource::kSysclk:
                    return get_sysclk_freq_hz();
                case SctSAI1ClockSource::kPllOutputQ:
                    return get_pll_q_output_freq_hz();
                case SctSAI1ClockSource::kPIN:
                    return std::nullopt;
                case SctSAI1ClockSource::kHSI:
                    return get_hsi_freq_hz();
            }

            return std::nullopt;
        }

        [[nodiscard]] constexpr std::optional<uint32_t> get_i2s_freq_hz() const
        {
            switch (sct.i2s_source)
            {
                case SctI2SClockSource::kSysclk:
                    return get_sysclk_freq_hz();
                case SctI2SClockSource::kPllOutputQ:
                    return get_pll_q_output_freq_hz();
                case SctI2SClockSource::kPIN:
                    return std::nullopt;
                case SctI2SClockSource::kHSI:
                    return get_hsi_freq_hz();
            }

            return std::nullopt;
        }

        [[nodiscard]] constexpr uint32_t get_fdcan_freq_hz() const
        {
            switch (sct.fdcan_source)
            {
                case SctFDCANClockSource::kHSE:
                    return get_hse_freq_hz();
                case SctFDCANClockSource::kPllOutputQ:
                    return get_pll_q_output_freq_hz();
                case SctFDCANClockSource::kPclk1:
                    return get_pclk1_freq_hz();
            }

            return 0U;
        }

        [[nodiscard]] constexpr uint32_t get_rng_freq_hz() const
        {
            switch (sct.rng_source)
            {
                case SctRNGClockSource::kHsi48:
                    return get_hsi48_freq_hz();
                case SctRNGClockSource::kPllOutputQ:
                    return get_pll_q_output_freq_hz();
            }

            return 0U;
        }

        [[nodiscard]] constexpr uint32_t get_usb_freq_hz() const
        {
            switch (sct.usb_source)
            {
                case SctUSBClockSource::kHsi48:
                    return get_hsi48_freq_hz();
                case SctUSBClockSource::kPllOutputQ:
                    return get_pll_q_output_freq_hz();
            }

            return 0U;
        }

        [[nodiscard]] constexpr uint32_t get_adc12_freq_hz() const
        {
            switch (sct.adc12_source)
            {
                case SctAdc12ClockSource::kNone:
                    return get_hclk_freq_hz();
                case SctAdc12ClockSource::kPllOutputP:
                    return get_pll_p_output_freq_hz();
                case SctAdc12ClockSource::kSysclk:
                    return get_sysclk_freq_hz();
            }

            return 0U;
        }

        [[nodiscard]] constexpr uint32_t get_adc345_freq_hz() const
        {
            switch (sct.adc345_source)
            {
                case SctAdc345ClockSource::kNone:
                    return get_hclk_freq_hz();
                case SctAdc345ClockSource::kPllOutputP:
                    return get_pll_p_output_freq_hz();
                case SctAdc345ClockSource::kSysclk:
                    return get_sysclk_freq_hz();
            }

            return 0U;
        }

        [[nodiscard]] constexpr uint32_t get_quadspi_freq_hz() const
        {
            switch (sct.quadspi_source)
            {
                case SctQuadSpiClockSource::kSysclk:
                    return get_sysclk_freq_hz();
                case SctQuadSpiClockSource::kHSI:
                    return get_hsi_freq_hz();
                case SctQuadSpiClockSource::kPllOutputQ:
                    return get_pll_q_output_freq_hz();
            }

            return 0U;
        }
    };

    // -----------------------------------------------------------------------------
    // DEVICE CLASS
    // -----------------------------------------------------------------------------

    /**
     * @brief Reset and Clock Control device.
     */
    template <typename T = void>
    class RccDevice
    {
    public:
        struct Descriptor : public UniqueDeviceDescriptor
        {
        };

        using InjectDevices = TypeList<HsiOscillatorDevice<>,
                                       HseOscillatorDevice<>,
                                       Hsi48OscillatorDevice<>,
                                       LsiOscillatorDevice<>,
                                       LseOscillatorDevice<>,
                                       PllDevice<>,
                                       SctDevice<>>;

    private:
        [[no_unique_address]] DeviceRef<HsiOscillatorDevice<>>   m_hsi;
        [[no_unique_address]] DeviceRef<HseOscillatorDevice<>>   m_hse;
        [[no_unique_address]] DeviceRef<Hsi48OscillatorDevice<>> m_hsi48;
        [[no_unique_address]] DeviceRef<LsiOscillatorDevice<>>   m_lsi;
        [[no_unique_address]] DeviceRef<LseOscillatorDevice<>>   m_lse;
        [[no_unique_address]] DeviceRef<PllDevice<>>             m_pll;
        [[no_unique_address]] DeviceRef<SctDevice<>>             m_sct;

    public:
        RccDevice() = delete;

        RccDevice(DeviceRef<HsiOscillatorDevice<>>&&   hsi,
                  DeviceRef<HseOscillatorDevice<>>&&   hse,
                  DeviceRef<Hsi48OscillatorDevice<>>&& hsi48,
                  DeviceRef<LsiOscillatorDevice<>>&&   lsi,
                  DeviceRef<LseOscillatorDevice<>>&&   lse,
                  DeviceRef<PllDevice<>>&&             pll,
                  DeviceRef<SctDevice<>>&&             sct)
            : m_hsi(std::move(hsi))
            , m_hse(std::move(hse))
            , m_hsi48(std::move(hsi48))
            , m_lsi(std::move(lsi))
            , m_lse(std::move(lse))
            , m_pll(std::move(pll))
            , m_sct(std::move(sct))
        {
        }

        // -------------------------------------------------------------------------
        // INITIALIZATION
        // -------------------------------------------------------------------------
        /**
         * @brief Initialize RCC by switching to the requested source.
         *
         * @param config RCC configuration.
         * @return true if the source switch completed successfully.
         */
        [[nodiscard]] inline bool init(const RccConfig& config)
        {
            if (!m_hsi->init(config.hsi))
            {
                return false;
            }

            if (!m_hse->init(config.hse))
            {
                return false;
            }

            if (!m_hsi48->init(config.hsi48))
            {
                return false;
            }

            if (!m_lsi->init(config.lsi))
            {
                return false;
            }

            if (!m_lse->init(config.lse))
            {
                return false;
            }

            if (!m_pll->init(config.pll))
            {
                return false;
            }

            if (!m_sct->init(config.sct))
            {
                return false;
            }

            return true;
        }
    };

}  // namespace valle::platform