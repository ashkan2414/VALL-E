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
    // FORWARD DECLARATIONS
    // =============================================================================
    class RCCRootDevice;

    template <typename T>
    class RCCDevice;

    // =============================================================================
    // ROOT RCC DEVICE
    // =============================================================================
    /**
     * @brief Root interface device for the system clock tree.
     */
    class RCCRootDevice
    {
    public:
        struct Descriptor : public InterfaceDeviceDescriptor
        {
            using Children = DeviceTreeList<RCCDevice<void>>;
        };
    };

    // =============================================================================
    // RCC INFO DEVICE
    // =============================================================================
    /**
     * @brief Aggregated read-only information device for the entire RCC system.
     */
    template <typename T = void>
    class RCCInfoDevice
    {
    public:
        struct Descriptor : public SharedDeviceDescriptor
        {
            constexpr static bool skNeedsInit = false;
        };

        using DependDevices = TypeList<RCCRootDevice>;
        using InjectDevices = TypeList<HSIOscillatorInfoDevice<>,
                                       HSEOscillatorInfoDevice<>,
                                       HSI48OscillatorInfoDevice<>,
                                       LSIOscillatorInfoDevice<>,
                                       LSEOscillatorInfoDevice<>,
                                       PLLInfoDevice<>,
                                       SCTInfoDevice<>>;

    private:
        [[no_unique_address]] DeviceRef<HSIOscillatorInfoDevice<>>   m_hsi_info;
        [[no_unique_address]] DeviceRef<HSEOscillatorInfoDevice<>>   m_hse_info;
        [[no_unique_address]] DeviceRef<HSI48OscillatorInfoDevice<>> m_hsi48_info;
        [[no_unique_address]] DeviceRef<LSIOscillatorInfoDevice<>>   m_lsi_info;
        [[no_unique_address]] DeviceRef<LSEOscillatorInfoDevice<>>   m_lse_info;
        [[no_unique_address]] DeviceRef<PLLInfoDevice<>>             m_pll_info;
        [[no_unique_address]] DeviceRef<SCTInfoDevice<>>             m_sct_info;

    public:
        RCCInfoDevice() = delete;

        RCCInfoDevice(DeviceRef<HSIOscillatorInfoDevice<>>&&   hsi,
                      DeviceRef<HSEOscillatorInfoDevice<>>&&   hse,
                      DeviceRef<HSI48OscillatorInfoDevice<>>&& hsi48,
                      DeviceRef<LSIOscillatorInfoDevice<>>&&   lsi,
                      DeviceRef<LSEOscillatorInfoDevice<>>&&   lse,
                      DeviceRef<PLLInfoDevice<>>&&             pll,
                      DeviceRef<SCTInfoDevice<>>&&             sct)
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
        [[nodiscard]] PLLSource get_pll_source() const
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
        [[nodiscard]] SCTAHBPrescaler get_ahb_prescaler() const
        {
            return m_sct_info->get_ahb_prescaler();
        }
        [[nodiscard]] SCTAPB1Prescaler get_apb1_prescaler() const
        {
            return m_sct_info->get_apb1_prescaler();
        }
        [[nodiscard]] SCTAPB2Prescaler get_apb2_prescaler() const
        {
            return m_sct_info->get_apb2_prescaler();
        }

        // ---------------------------------------------------------------------
        // SYSTEM CLOCK TREE (SCT)
        // ---------------------------------------------------------------------
        [[nodiscard]] SCTSourceStatus get_sysclk_source() const
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
        [[nodiscard]] SCTUSART1ClockSource get_usart1_source() const
        {
            return m_sct_info->get_usart1_source();
        }

        [[nodiscard]] SCTUSART2ClockSource get_usart2_source() const
        {
            return m_sct_info->get_usart2_source();
        }

        [[nodiscard]] SCTUSART3ClockSource get_usart3_source() const
        {
            return m_sct_info->get_usart3_source();
        }

#if defined(UART4)
        [[nodiscard]] SCTUART4ClockSource get_uart4_source() const
        {
            return m_sct_info->get_uart4_source();
        }

        [[nodiscard]] SCTUART5ClockSource get_uart5_source() const
        {
            return m_sct_info->get_uart5_source();
        }
#endif

        [[nodiscard]] SCTLPUART1ClockSource get_lpuart1_source() const
        {
            return m_sct_info->get_lpuart1_source();
        }

        [[nodiscard]] SCTI2C1ClockSource get_i2c1_source() const
        {
            return m_sct_info->get_i2c1_source();
        }

        [[nodiscard]] SCTI2C2ClockSource get_i2c2_source() const
        {
            return m_sct_info->get_i2c2_source();
        }

        [[nodiscard]] SCTI2C3ClockSource get_i2c3_source() const
        {
            return m_sct_info->get_i2c3_source();
        }

        [[nodiscard]] SCTI2C4ClockSource get_i2c4_source() const
        {
            return m_sct_info->get_i2c4_source();
        }

        [[nodiscard]] SCTLPTIM1ClockSource get_lptim1_source() const
        {
            return m_sct_info->get_lptim1_source();
        }

        [[nodiscard]] SCTSAI1ClockSource get_sai1_source() const
        {
            return m_sct_info->get_sai1_source();
        }

        [[nodiscard]] SCTI2SClockSource get_i2s_source() const
        {
            return m_sct_info->get_i2s_source();
        }

#if defined(FDCAN1)
        [[nodiscard]] SCTFDCANClockSource get_fdcan_source() const
        {
            return m_sct_info->get_fdcan_source();
        }
#endif /* FDCAN1 */

        [[nodiscard]] SCTRNGClockSource get_rng_source() const
        {
            return m_sct_info->get_rng_source();
        }

        [[nodiscard]] SCTUSBClockSource get_usb_source() const
        {
            return m_sct_info->get_usb_source();
        }

        [[nodiscard]] SCTADC12ClockSource get_adc12_source() const
        {
            return m_sct_info->get_adc12_source();
        }

        [[nodiscard]] SCTADC345ClockSource get_adc345_source() const
        {
            return m_sct_info->get_adc345_source();
        }

        [[nodiscard]] SCTQUADSPIClockSource get_quadspi_source() const
        {
            return m_sct_info->get_quadspi_source();
        }

        [[nodiscard]] uint32_t get_usart1_freq_hz() const
        {
            switch (get_usart1_source())
            {
                case SCTUSART1ClockSource::kPclk2:
                    return get_pclk2_freq_hz();
                case SCTUSART1ClockSource::kSysclk:
                    return get_sysclk_freq_hz();
                case SCTUSART1ClockSource::kHSI:
                    return get_hsi_freq_hz();
                case SCTUSART1ClockSource::kLSE:
                    return get_lse_freq_hz();
            }

            return 0U;
        }

        [[nodiscard]] uint32_t get_usart2_freq_hz() const
        {
            switch (get_usart2_source())
            {
                case SCTUSART2ClockSource::kPclk1:
                    return get_pclk1_freq_hz();
                case SCTUSART2ClockSource::kSysclk:
                    return get_sysclk_freq_hz();
                case SCTUSART2ClockSource::kHSI:
                    return get_hsi_freq_hz();
                case SCTUSART2ClockSource::kLSE:
                    return get_lse_freq_hz();
            }

            return 0U;
        }

        [[nodiscard]] uint32_t get_usart3_freq_hz() const
        {
            switch (get_usart3_source())
            {
                case SCTUSART3ClockSource::kPclk1:
                    return get_pclk1_freq_hz();
                case SCTUSART3ClockSource::kSysclk:
                    return get_sysclk_freq_hz();
                case SCTUSART3ClockSource::kHSI:
                    return get_hsi_freq_hz();
                case SCTUSART3ClockSource::kLSE:
                    return get_lse_freq_hz();
            }

            return 0U;
        }

        [[nodiscard]] uint32_t get_uart4_freq_hz() const
        {
            switch (get_uart4_source())
            {
                case SCTUART4ClockSource::kPclk1:
                    return get_pclk1_freq_hz();
                case SCTUART4ClockSource::kSysclk:
                    return get_sysclk_freq_hz();
                case SCTUART4ClockSource::kHSI:
                    return get_hsi_freq_hz();
                case SCTUART4ClockSource::kLSE:
                    return get_lse_freq_hz();
            }

            return 0U;
        }

        [[nodiscard]] uint32_t get_uart5_freq_hz() const
        {
            switch (get_uart5_source())
            {
                case SCTUART5ClockSource::kPclk1:
                    return get_pclk1_freq_hz();
                case SCTUART5ClockSource::kSysclk:
                    return get_sysclk_freq_hz();
                case SCTUART5ClockSource::kHSI:
                    return get_hsi_freq_hz();
                case SCTUART5ClockSource::kLSE:
                    return get_lse_freq_hz();
            }

            return 0U;
        }

        [[nodiscard]] uint32_t get_lpuart1_freq_hz() const
        {
            switch (get_lpuart1_source())
            {
                case SCTLPUART1ClockSource::kPclk1:
                    return get_pclk1_freq_hz();
                case SCTLPUART1ClockSource::kSysclk:
                    return get_sysclk_freq_hz();
                case SCTLPUART1ClockSource::kHSI:
                    return get_hsi_freq_hz();
                case SCTLPUART1ClockSource::kLSE:
                    return get_lse_freq_hz();
            }

            return 0U;
        }

        [[nodiscard]] uint32_t get_i2c1_freq_hz() const
        {
            switch (get_i2c1_source())
            {
                case SCTI2C1ClockSource::kPclk1:
                    return get_pclk1_freq_hz();
                case SCTI2C1ClockSource::kSysclk:
                    return get_sysclk_freq_hz();
                case SCTI2C1ClockSource::kHSI:
                    return get_hsi_freq_hz();
            }

            return 0U;
        }

        [[nodiscard]] uint32_t get_i2c2_freq_hz() const
        {
            switch (get_i2c2_source())
            {
                case SCTI2C2ClockSource::kPclk1:
                    return get_pclk1_freq_hz();
                case SCTI2C2ClockSource::kSysclk:
                    return get_sysclk_freq_hz();
                case SCTI2C2ClockSource::kHSI:
                    return get_hsi_freq_hz();
            }

            return 0U;
        }

        [[nodiscard]] uint32_t get_i2c3_freq_hz() const
        {
            switch (get_i2c3_source())
            {
                case SCTI2C3ClockSource::kPclk1:
                    return get_pclk1_freq_hz();
                case SCTI2C3ClockSource::kSysclk:
                    return get_sysclk_freq_hz();
                case SCTI2C3ClockSource::kHSI:
                    return get_hsi_freq_hz();
            }

            return 0U;
        }

        [[nodiscard]] uint32_t get_i2c4_freq_hz() const
        {
            switch (get_i2c4_source())
            {
                case SCTI2C4ClockSource::kPclk1:
                    return get_pclk1_freq_hz();
                case SCTI2C4ClockSource::kSysclk:
                    return get_sysclk_freq_hz();
                case SCTI2C4ClockSource::kHSI:
                    return get_hsi_freq_hz();
            }

            return 0U;
        }

        [[nodiscard]] uint32_t get_lptim1_freq_hz() const
        {
            switch (get_lptim1_source())
            {
                case SCTLPTIM1ClockSource::kPclk1:
                    return get_pclk1_freq_hz();
                case SCTLPTIM1ClockSource::kLSI:
                    return get_lsi_freq_hz();
                case SCTLPTIM1ClockSource::kHSI:
                    return get_hsi_freq_hz();
                case SCTLPTIM1ClockSource::kLSE:
                    return get_lse_freq_hz();
            }

            return 0U;
        }

        [[nodiscard]] std::optional<uint32_t> get_sai1_freq_hz() const
        {
            switch (get_sai1_source())
            {
                case SCTSAI1ClockSource::kSysclk:
                    return get_sysclk_freq_hz();
                case SCTSAI1ClockSource::kPLLQ:
                    return get_pll_q_output_freq_hz();
                case SCTSAI1ClockSource::kPIN:
                    return std::nullopt;
                case SCTSAI1ClockSource::kHSI:
                    return get_hsi_freq_hz();
            }

            return std::nullopt;
        }

        [[nodiscard]] std::optional<uint32_t> get_i2s_freq_hz() const
        {
            switch (get_i2s_source())
            {
                case SCTI2SClockSource::kSysclk:
                    return get_sysclk_freq_hz();
                case SCTI2SClockSource::kPLLQ:
                    return get_pll_q_output_freq_hz();
                case SCTI2SClockSource::kPIN:
                    return std::nullopt;
                case SCTI2SClockSource::kHSI:
                    return get_hsi_freq_hz();
            }

            return std::nullopt;
        }

        [[nodiscard]] uint32_t get_fdcan_freq_hz() const
        {
            switch (get_fdcan_source())
            {
                case SCTFDCANClockSource::kHSE:
                    return get_hse_freq_hz();
                case SCTFDCANClockSource::kPLLQ:
                    return get_pll_q_output_freq_hz();
                case SCTFDCANClockSource::kPclk1:
                    return get_pclk1_freq_hz();
            }

            return 0U;
        }

        [[nodiscard]] uint32_t get_rng_freq_hz() const
        {
            switch (get_rng_source())
            {
                case SCTRNGClockSource::kHSI48:
                    return get_hsi48_freq_hz();
                case SCTRNGClockSource::kPLLQ:
                    return get_pll_q_output_freq_hz();
            }

            return 0U;
        }

        [[nodiscard]] uint32_t get_usb_freq_hz() const
        {
            switch (get_usb_source())
            {
                case SCTUSBClockSource::kHSI48:
                    return get_hsi48_freq_hz();
                case SCTUSBClockSource::kPLLQ:
                    return get_pll_q_output_freq_hz();
            }

            return 0U;
        }

        [[nodiscard]] uint32_t get_adc12_freq_hz() const
        {
            switch (get_adc12_source())
            {
                case SCTADC12ClockSource::kNone:
                    return get_hclk_freq_hz();
                case SCTADC12ClockSource::kPLLP:
                    return get_pll_p_output_freq_hz();
                case SCTADC12ClockSource::kSysclk:
                    return get_sysclk_freq_hz();
            }

            return 0U;
        }

        [[nodiscard]] uint32_t get_adc345_freq_hz() const
        {
            switch (get_adc345_source())
            {
                case SCTADC345ClockSource::kNone:
                    return get_hclk_freq_hz();
                case SCTADC345ClockSource::kPLLP:
                    return get_pll_p_output_freq_hz();
                case SCTADC345ClockSource::kSysclk:
                    return get_sysclk_freq_hz();
            }

            return 0U;
        }

        [[nodiscard]] uint32_t get_quadspi_freq_hz() const
        {
            switch (get_quadspi_source())
            {
                case SCTQUADSPIClockSource::kSysclk:
                    return get_sysclk_freq_hz();
                case SCTQUADSPIClockSource::kHSI:
                    return get_hsi_freq_hz();
                case SCTQUADSPIClockSource::kPLLQ:
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
    struct RCCConfig
    {
        // Oscillators
        HSIOscillatorConfig   hsi{};
        HSEOscillatorConfig   hse{};
        HSI48OscillatorConfig hsi48{};
        LSIOscillatorConfig   lsi{};
        LSEOscillatorConfig   lse{};

        // PLL
        PLLConfig pll{};

        // System clock tree
        SCTConfig sct{};

        [[nodiscard]] constexpr std::optional<std::string_view> validate(
            const PowerVoltageRangeMode voltage_range) const
        {
            if (const auto pll_error = pll.validate(get_pll_source_freq_hz(), voltage_range))
            {
                return pll_error;
            }

            switch (sct.sysclk_source)
            {
                case SCTSYSCLKSource::kHSI:
                {
                    if (!hsi.enabled)
                    {
                        return "SCT source is set to HSI, but HSI oscillator is not enabled";
                    }

                    break;
                }
                case SCTSYSCLKSource::kHSE:
                {
                    if (!hse.enabled)
                    {
                        return "SCT source is set to HSE, but HSE oscillator is not enabled";
                    }

                    break;
                }
                case SCTSYSCLKSource::kPLL:
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
                case PLLSource::kHSI:
                    return get_hsi_freq_hz();

                case PLLSource::kHSE:
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
                case SCTSYSCLKSource::kHSI:
                    return get_hsi_freq_hz();

                case SCTSYSCLKSource::kHSE:
                    return get_hse_freq_hz();

                case SCTSYSCLKSource::kPLL:
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
                case SCTMCOClockSource::kNoClock:
                    return 0U;
                case SCTMCOClockSource::kHSI:
                    return get_hsi_freq_hz();
                case SCTMCOClockSource::kHSE:
                    return get_hse_freq_hz();
                case SCTMCOClockSource::kHSI48:
                    return get_hsi48_freq_hz();
                case SCTMCOClockSource::kLSI:
                    return get_lsi_freq_hz();
                case SCTMCOClockSource::kLSE:
                    return get_lse_freq_hz();
                case SCTMCOClockSource::kPLL:
                    return get_pll_r_output_freq_hz();
                case SCTMCOClockSource::kSysclk:
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
                case SCTUSART1ClockSource::kPclk2:
                    return get_pclk2_freq_hz();
                case SCTUSART1ClockSource::kSysclk:
                    return get_sysclk_freq_hz();
                case SCTUSART1ClockSource::kHSI:
                    return get_hsi_freq_hz();
                case SCTUSART1ClockSource::kLSE:
                    return get_lse_freq_hz();
            }

            return 0U;
        }

        [[nodiscard]] constexpr uint32_t get_usart2_freq_hz() const
        {
            switch (sct.usart2_source)
            {
                case SCTUSART2ClockSource::kPclk1:
                    return get_pclk1_freq_hz();
                case SCTUSART2ClockSource::kSysclk:
                    return get_sysclk_freq_hz();
                case SCTUSART2ClockSource::kHSI:
                    return get_hsi_freq_hz();
                case SCTUSART2ClockSource::kLSE:
                    return get_lse_freq_hz();
            }

            return 0U;
        }

        [[nodiscard]] constexpr uint32_t get_usart3_freq_hz() const
        {
            switch (sct.usart3_source)
            {
                case SCTUSART3ClockSource::kPclk1:
                    return get_pclk1_freq_hz();
                case SCTUSART3ClockSource::kSysclk:
                    return get_sysclk_freq_hz();
                case SCTUSART3ClockSource::kHSI:
                    return get_hsi_freq_hz();
                case SCTUSART3ClockSource::kLSE:
                    return get_lse_freq_hz();
            }

            return 0U;
        }

        [[nodiscard]] constexpr uint32_t get_uart4_freq_hz() const
        {
            switch (sct.uart4_source)
            {
                case SCTUART4ClockSource::kPclk1:
                    return get_pclk1_freq_hz();
                case SCTUART4ClockSource::kSysclk:
                    return get_sysclk_freq_hz();
                case SCTUART4ClockSource::kHSI:
                    return get_hsi_freq_hz();
                case SCTUART4ClockSource::kLSE:
                    return get_lse_freq_hz();
            }

            return 0U;
        }

        [[nodiscard]] constexpr uint32_t get_uart5_freq_hz() const
        {
            switch (sct.uart5_source)
            {
                case SCTUART5ClockSource::kPclk1:
                    return get_pclk1_freq_hz();
                case SCTUART5ClockSource::kSysclk:
                    return get_sysclk_freq_hz();
                case SCTUART5ClockSource::kHSI:
                    return get_hsi_freq_hz();
                case SCTUART5ClockSource::kLSE:
                    return get_lse_freq_hz();
            }

            return 0U;
        }

        [[nodiscard]] constexpr uint32_t get_lpuart1_freq_hz() const
        {
            switch (sct.lpuart1_source)
            {
                case SCTLPUART1ClockSource::kPclk1:
                    return get_pclk1_freq_hz();
                case SCTLPUART1ClockSource::kSysclk:
                    return get_sysclk_freq_hz();
                case SCTLPUART1ClockSource::kHSI:
                    return get_hsi_freq_hz();
                case SCTLPUART1ClockSource::kLSE:
                    return get_lse_freq_hz();
            }

            return 0U;
        }

        [[nodiscard]] constexpr uint32_t get_i2c1_freq_hz() const
        {
            switch (sct.i2c1_source)
            {
                case SCTI2C1ClockSource::kPclk1:
                    return get_pclk1_freq_hz();
                case SCTI2C1ClockSource::kSysclk:
                    return get_sysclk_freq_hz();
                case SCTI2C1ClockSource::kHSI:
                    return get_hsi_freq_hz();
            }

            return 0U;
        }

        [[nodiscard]] constexpr uint32_t get_i2c2_freq_hz() const
        {
            switch (sct.i2c2_source)
            {
                case SCTI2C2ClockSource::kPclk1:
                    return get_pclk1_freq_hz();
                case SCTI2C2ClockSource::kSysclk:
                    return get_sysclk_freq_hz();
                case SCTI2C2ClockSource::kHSI:
                    return get_hsi_freq_hz();
            }

            return 0U;
        }

        [[nodiscard]] constexpr uint32_t get_i2c3_freq_hz() const
        {
            switch (sct.i2c3_source)
            {
                case SCTI2C3ClockSource::kPclk1:
                    return get_pclk1_freq_hz();
                case SCTI2C3ClockSource::kSysclk:
                    return get_sysclk_freq_hz();
                case SCTI2C3ClockSource::kHSI:
                    return get_hsi_freq_hz();
            }

            return 0U;
        }

        [[nodiscard]] constexpr uint32_t get_i2c4_freq_hz() const
        {
            switch (sct.i2c4_source)
            {
                case SCTI2C4ClockSource::kPclk1:
                    return get_pclk1_freq_hz();
                case SCTI2C4ClockSource::kSysclk:
                    return get_sysclk_freq_hz();
                case SCTI2C4ClockSource::kHSI:
                    return get_hsi_freq_hz();
            }

            return 0U;
        }

        [[nodiscard]] constexpr uint32_t get_lptim1_freq_hz() const
        {
            switch (sct.lptim1_source)
            {
                case SCTLPTIM1ClockSource::kPclk1:
                    return get_pclk1_freq_hz();
                case SCTLPTIM1ClockSource::kLSI:
                    return get_lsi_freq_hz();
                case SCTLPTIM1ClockSource::kHSI:
                    return get_hsi_freq_hz();
                case SCTLPTIM1ClockSource::kLSE:
                    return get_lse_freq_hz();
            }

            return 0U;
        }

        [[nodiscard]] constexpr std::optional<uint32_t> get_sai1_freq_hz() const
        {
            switch (sct.sai1_source)
            {
                case SCTSAI1ClockSource::kSysclk:
                    return get_sysclk_freq_hz();
                case SCTSAI1ClockSource::kPLLQ:
                    return get_pll_q_output_freq_hz();
                case SCTSAI1ClockSource::kPIN:
                    return std::nullopt;
                case SCTSAI1ClockSource::kHSI:
                    return get_hsi_freq_hz();
            }

            return std::nullopt;
        }

        [[nodiscard]] constexpr std::optional<uint32_t> get_i2s_freq_hz() const
        {
            switch (sct.i2s_source)
            {
                case SCTI2SClockSource::kSysclk:
                    return get_sysclk_freq_hz();
                case SCTI2SClockSource::kPLLQ:
                    return get_pll_q_output_freq_hz();
                case SCTI2SClockSource::kPIN:
                    return std::nullopt;
                case SCTI2SClockSource::kHSI:
                    return get_hsi_freq_hz();
            }

            return std::nullopt;
        }

        [[nodiscard]] constexpr uint32_t get_fdcan_freq_hz() const
        {
            switch (sct.fdcan_source)
            {
                case SCTFDCANClockSource::kHSE:
                    return get_hse_freq_hz();
                case SCTFDCANClockSource::kPLLQ:
                    return get_pll_q_output_freq_hz();
                case SCTFDCANClockSource::kPclk1:
                    return get_pclk1_freq_hz();
            }

            return 0U;
        }

        [[nodiscard]] constexpr uint32_t get_rng_freq_hz() const
        {
            switch (sct.rng_source)
            {
                case SCTRNGClockSource::kHSI48:
                    return get_hsi48_freq_hz();
                case SCTRNGClockSource::kPLLQ:
                    return get_pll_q_output_freq_hz();
            }

            return 0U;
        }

        [[nodiscard]] constexpr uint32_t get_usb_freq_hz() const
        {
            switch (sct.usb_source)
            {
                case SCTUSBClockSource::kHSI48:
                    return get_hsi48_freq_hz();
                case SCTUSBClockSource::kPLLQ:
                    return get_pll_q_output_freq_hz();
            }

            return 0U;
        }

        [[nodiscard]] constexpr uint32_t get_adc12_freq_hz() const
        {
            switch (sct.adc12_source)
            {
                case SCTADC12ClockSource::kNone:
                    return get_hclk_freq_hz();
                case SCTADC12ClockSource::kPLLP:
                    return get_pll_p_output_freq_hz();
                case SCTADC12ClockSource::kSysclk:
                    return get_sysclk_freq_hz();
            }

            return 0U;
        }

        [[nodiscard]] constexpr uint32_t get_adc345_freq_hz() const
        {
            switch (sct.adc345_source)
            {
                case SCTADC345ClockSource::kNone:
                    return get_hclk_freq_hz();
                case SCTADC345ClockSource::kPLLP:
                    return get_pll_p_output_freq_hz();
                case SCTADC345ClockSource::kSysclk:
                    return get_sysclk_freq_hz();
            }

            return 0U;
        }

        [[nodiscard]] constexpr uint32_t get_quadspi_freq_hz() const
        {
            switch (sct.quadspi_source)
            {
                case SCTQUADSPIClockSource::kSysclk:
                    return get_sysclk_freq_hz();
                case SCTQUADSPIClockSource::kHSI:
                    return get_hsi_freq_hz();
                case SCTQUADSPIClockSource::kPLLQ:
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
    class RCCDevice
    {
    public:
        struct Descriptor : public UniqueDeviceDescriptor
        {
        };

        using DependDevices = TypeList<RCCRootDevice>;
        using InjectDevices = TypeList<HSIOscillatorDevice<>,
                                       HSEOscillatorDevice<>,
                                       HSI48OscillatorDevice<>,
                                       LSIOscillatorDevice<>,
                                       LSEOscillatorDevice<>,
                                       PLLDevice<>,
                                       SCTDevice<>>;

    private:
        [[no_unique_address]] DeviceRef<HSIOscillatorDevice<>>   m_hsi;
        [[no_unique_address]] DeviceRef<HSEOscillatorDevice<>>   m_hse;
        [[no_unique_address]] DeviceRef<HSI48OscillatorDevice<>> m_hsi48;
        [[no_unique_address]] DeviceRef<LSIOscillatorDevice<>>   m_lsi;
        [[no_unique_address]] DeviceRef<LSEOscillatorDevice<>>   m_lse;
        [[no_unique_address]] DeviceRef<PLLDevice<>>             m_pll;
        [[no_unique_address]] DeviceRef<SCTDevice<>>             m_sct;

    public:
        RCCDevice() = delete;

        RCCDevice(DeviceRef<HSIOscillatorDevice<>>&&   hsi,
                  DeviceRef<HSEOscillatorDevice<>>&&   hse,
                  DeviceRef<HSI48OscillatorDevice<>>&& hsi48,
                  DeviceRef<LSIOscillatorDevice<>>&&   lsi,
                  DeviceRef<LSEOscillatorDevice<>>&&   lse,
                  DeviceRef<PLLDevice<>>&&             pll,
                  DeviceRef<SCTDevice<>>&&             sct)
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
        [[nodiscard]] inline bool init(const RCCConfig& config)
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