#pragma once

#include <stm32g4xx_hal.h>
#include <system_stm32g4xx.h>

#include "valle/platform/core.hpp"
#include "valle/platform/devices/power.hpp"
#include "valle/platform/devices/rcc/hse.hpp"
#include "valle/platform/devices/rcc/hsi.hpp"
#include "valle/platform/devices/rcc/pll.hpp"
#include "valle/platform/hardware/flash.hpp"
#include "valle/platform/hardware/rcc/sct.hpp"

namespace valle::platform
{
    // =============================================================================
    // FORWARD DECLARATIONS
    // =============================================================================
    class SCTRootDevice;

    template <typename T>
    class SCTDevice;

    // =============================================================================
    // ROOT SCT DEVICE
    // =============================================================================
    /**
     * @brief Root interface device for the system clock tree.
     */
    class SCTRootDevice
    {
    public:
        struct Descriptor : public InterfaceDeviceDescriptor
        {
            using Children = DeviceTreeList<SCTDevice<void>>;
        };
    };

    // =========================================================================
    // SCT INFO DEVICE
    // =========================================================================
    template <typename T = void>
    class SCTInfoDevice
    {
    public:
        struct Descriptor : public SharedDeviceDescriptor
        {
            constexpr static bool skNeedsInit = false;
        };

        using DependDevices = TypeList<SCTRootDevice>;
        using InjectDevices = TypeList<HSEOscillatorInfoDevice<>, HSIOscillatorInfoDevice<>, PLLInfoDevice<>>;

    private:
        [[no_unique_address]] DeviceRef<HSEOscillatorInfoDevice<>> m_hse_info;
        [[no_unique_address]] DeviceRef<HSIOscillatorInfoDevice<>> m_hsi_info;
        [[no_unique_address]] DeviceRef<PLLInfoDevice<>>           m_pll_info;

    public:
        SCTInfoDevice() = delete;

        SCTInfoDevice(DeviceRef<HSEOscillatorInfoDevice<>>&& hse_info,
                      DeviceRef<HSIOscillatorInfoDevice<>>&& hsi_info,
                      DeviceRef<PLLInfoDevice<>>&&           pll_info)
            : m_hse_info(std::move(hse_info)), m_hsi_info(std::move(hsi_info)), m_pll_info(std::move(pll_info))
        {
        }

        [[nodiscard]] SCTSourceStatus get_source_status() const
        {
            return SCTInterface::get_source_status();
        }

        [[nodiscard]] SCTAHBPrescaler get_ahb_prescaler() const
        {
            return SCTInterface::get_ahb_prescaler();
        }

        [[nodiscard]] SCTAPB1Prescaler get_apb1_prescaler() const
        {
            return SCTInterface::get_apb1_prescaler();
        }

        [[nodiscard]] SCTAPB2Prescaler get_apb2_prescaler() const
        {
            return SCTInterface::get_apb2_prescaler();
        }

        [[nodiscard]] uint32_t get_sysclk_freq_hz_for_source(const SCTSYSCLKSource sysclk_source) const
        {
            switch (sysclk_source)
            {
                case SCTSYSCLKSource::kHSI:
                    return m_hsi_info->get_frequency_hz();

                case SCTSYSCLKSource::kHSE:
                    return m_hse_info->get_frequency_hz();

                case SCTSYSCLKSource::kPLL:
                    return m_pll_info->get_r_output_freq_hz();
            }

            return 0;
        }

        [[nodiscard]] uint32_t get_sysclk_freq_hz() const
        {
            switch (get_source_status())
            {
                case SCTSourceStatus::kHSI:
                    return m_hsi_info->get_frequency_hz();

                case SCTSourceStatus::kHSE:
                    return m_hse_info->get_frequency_hz();

                case SCTSourceStatus::kPLL:
                    return m_pll_info->get_r_output_freq_hz();
            }

            return 0;
        }

        [[nodiscard]] uint32_t get_hclk_freq_hz() const
        {
            return SCTInterface::get_hclk_freq_hz(get_sysclk_freq_hz());
        }

        [[nodiscard]] uint32_t get_pclk1_freq_hz() const
        {
            return SCTInterface::get_pclk1_freq_hz(get_hclk_freq_hz());
        }

        [[nodiscard]] uint32_t get_apb1_peripheral_freq_hz() const
        {
            return SCTInterface::get_apb1_peripheral_freq_hz(get_pclk1_freq_hz());
        }

        [[nodiscard]] uint32_t get_apb1_timer_freq_hz() const
        {
            return SCTInterface::get_apb1_timer_freq_hz(get_pclk1_freq_hz());
        }

        [[nodiscard]] uint32_t get_pclk2_freq_hz() const
        {
            return SCTInterface::get_pclk2_freq_hz(get_hclk_freq_hz());
        }

        [[nodiscard]] uint32_t get_apb2_peripheral_freq_hz() const
        {
            return SCTInterface::get_apb2_peripheral_freq_hz(get_pclk2_freq_hz());
        }

        [[nodiscard]] uint32_t get_apb2_timer_freq_hz() const
        {
            return SCTInterface::get_apb2_timer_freq_hz(get_pclk2_freq_hz());
        }

        // ---------------------------------------------------------------------
        // Peripheral clock tree
        // ---------------------------------------------------------------------
        [[nodiscard]] SCTUSART1ClockSource get_usart1_source() const
        {
            return SCTInterface::get_usart1_source();
        }

        [[nodiscard]] SCTUSART2ClockSource get_usart2_source() const
        {
            return SCTInterface::get_usart2_source();
        }

        [[nodiscard]] SCTUSART3ClockSource get_usart3_source() const
        {
            return SCTInterface::get_usart3_source();
        }

#if defined(UART4)
        [[nodiscard]] SCTUART4ClockSource get_uart4_source() const
        {
            return SCTInterface::get_uart4_source();
        }

        [[nodiscard]] SCTUART5ClockSource get_uart5_source() const
        {
            return SCTInterface::get_uart5_source();
        }
#endif

        [[nodiscard]] SCTLPUART1ClockSource get_lpuart1_source() const
        {
            return SCTInterface::get_lpuart1_source();
        }

        [[nodiscard]] SCTI2C1ClockSource get_i2c1_source() const
        {
            return SCTInterface::get_i2c1_source();
        }

        [[nodiscard]] SCTI2C2ClockSource get_i2c2_source() const
        {
            return SCTInterface::get_i2c2_source();
        }

        [[nodiscard]] SCTI2C3ClockSource get_i2c3_source() const
        {
            return SCTInterface::get_i2c3_source();
        }

        [[nodiscard]] SCTI2C4ClockSource get_i2c4_source() const
        {
            return SCTInterface::get_i2c4_source();
        }

        [[nodiscard]] SCTLPTIM1ClockSource get_lptim1_source() const
        {
            return SCTInterface::get_lptim1_source();
        }

        [[nodiscard]] SCTSAI1ClockSource get_sai1_source() const
        {
            return SCTInterface::get_sai1_source();
        }

        [[nodiscard]] SCTI2SClockSource get_i2s_source() const
        {
            return SCTInterface::get_i2s_source();
        }

#if defined(FDCAN1)
        [[nodiscard]] SCTFDCANClockSource get_fdcan_source() const
        {
            return SCTInterface::get_fdcan_source();
        }
#endif /* FDCAN1 */

        [[nodiscard]] SCTRNGClockSource get_rng_source() const
        {
            return SCTInterface::get_rng_source();
        }

        [[nodiscard]] SCTUSBClockSource get_usb_source() const
        {
            return SCTInterface::get_usb_source();
        }

        [[nodiscard]] SCTADC12ClockSource get_adc12_source() const
        {
            return SCTInterface::get_adc12_source();
        }

        [[nodiscard]] SCTADC345ClockSource get_adc345_source() const
        {
            return SCTInterface::get_adc345_source();
        }

        [[nodiscard]] SCTQUADSPIClockSource get_quadspi_source() const
        {
            return SCTInterface::get_quadspi_source();
        }
    };

    // =============================================================================
    // SCT DEVICE
    // =============================================================================

    // --------------------------------------------------------------------------------
    // CONFIGURATION
    // --------------------------------------------------------------------------------

    /**
     * @brief Configuration for the SCT device.
     */
    struct SCTConfig
    {
        SCTSYSCLKSource  sysclk_source  = kResetSYSCLKSource;
        SCTAHBPrescaler  ahb_prescaler  = kResetAHBPrescaler;
        SCTAPB1Prescaler apb1_prescaler = kResetAPB1Prescaler;
        SCTAPB2Prescaler apb2_prescaler = kResetAPB2Prescaler;

        SCTMCOClockSource     mco_source     = kResetMCOClockSource;
        SCTMCOPrescaler       mco_prescaler  = kResetMCOPrescaler;
        SCTUSART1ClockSource  usart1_source  = kResetUSART1ClockSource;
        SCTUSART2ClockSource  usart2_source  = kResetUSART2ClockSource;
        SCTUSART3ClockSource  usart3_source  = kResetUSART3ClockSource;
        SCTUART4ClockSource   uart4_source   = kResetUART4ClockSource;
        SCTUART5ClockSource   uart5_source   = kResetUART5ClockSource;
        SCTLPUART1ClockSource lpuart1_source = kResetLPUART1ClockSource;
        SCTI2C1ClockSource    i2c1_source    = kResetI2C1ClockSource;
        SCTI2C2ClockSource    i2c2_source    = kResetI2C2ClockSource;
        SCTI2C3ClockSource    i2c3_source    = kResetI2C3ClockSource;
        SCTI2C4ClockSource    i2c4_source    = kResetI2C4ClockSource;
        SCTLPTIM1ClockSource  lptim1_source  = kResetLPTIM1ClockSource;
        SCTSAI1ClockSource    sai1_source    = kResetSAI1ClockSource;
        SCTI2SClockSource     i2s_source     = kResetI2SClockSource;
        SCTFDCANClockSource   fdcan_source   = kResetFDCANClockSource;
        SCTRNGClockSource     rng_source     = kResetRNGClockSource;
        SCTUSBClockSource     usb_source     = kResetUSBClockSource;
        SCTADC12ClockSource   adc12_source   = kResetADC12ClockSource;
        SCTADC345ClockSource  adc345_source  = kResetADC345ClockSource;
        SCTQUADSPIClockSource quadspi_source = kResetQUADSPIClockSource;

        [[nodiscard]] constexpr std::optional<std::string_view>
        validate(  // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
            const uint32_t              sysclk_source_hz,
            const PowerVoltageRangeMode voltage_range) const
        {
            return SCTInterface::validate_config(
                sysclk_source_hz, voltage_range, ahb_prescaler, apb1_prescaler, apb2_prescaler);
        }

        [[nodiscard]] constexpr uint32_t get_sysclk_freq_hz(const uint32_t sysclk_source_hz) const
        {
            return sysclk_source_hz;
        }

        [[nodiscard]] constexpr uint32_t get_hclk_freq_hz(const uint32_t sysclk_source_hz) const
        {
            return SCTInterface::calculate_hclk_freq_hz(get_sysclk_freq_hz(sysclk_source_hz), ahb_prescaler);
        }

        [[nodiscard]] constexpr uint32_t get_pclk1_freq_hz(const uint32_t hclk_hz) const
        {
            return SCTInterface::calculate_pclk1_freq_hz(hclk_hz, apb1_prescaler);
        }

        [[nodiscard]] constexpr uint32_t get_apb1_peripheral_freq_hz(const uint32_t pclk1_hz) const
        {
            return SCTInterface::calculate_apb1_peripheral_freq_hz(pclk1_hz);
        }

        [[nodiscard]] constexpr uint32_t get_apb1_timer_freq_hz(const uint32_t pclk1_hz) const
        {
            return SCTInterface::calculate_apb1_timer_freq_hz(pclk1_hz, apb1_prescaler);
        }

        [[nodiscard]] constexpr uint32_t get_pclk2_freq_hz(const uint32_t hclk_hz) const
        {
            return SCTInterface::calculate_pclk2_freq_hz(hclk_hz, apb2_prescaler);
        }

        [[nodiscard]] constexpr uint32_t get_apb2_peripheral_freq_hz(const uint32_t pclk2_hz) const
        {
            return SCTInterface::calculate_apb2_peripheral_freq_hz(pclk2_hz);
        }

        [[nodiscard]] constexpr uint32_t get_apb2_timer_freq_hz(const uint32_t pclk2_hz) const
        {
            return SCTInterface::calculate_apb2_timer_freq_hz(pclk2_hz, apb2_prescaler);
        }

        [[nodiscard]] constexpr uint32_t get_mco_freq_hz(const uint32_t source_hz) const
        {
            return SCTInterface::calculate_mco_freq_hz(source_hz, mco_prescaler);
        }
    };

    /**
     * @brief System clock tree device.
     */
    template <typename T = void>
    class SCTDevice
    {
    public:
        struct Descriptor : public UniqueDeviceDescriptor
        {
        };

        using InterfaceT = SCTInterface;

        using DependDevices = TypeList<SCTRootDevice, HSIOscillatorDevice<>, HSEOscillatorDevice<>, PLLDevice<>>;
        using InjectDevices = TypeList<SCTInfoDevice<>, PowerInfoDevice<>>;

    private:
        [[no_unique_address]] DeviceRef<SCTInfoDevice<>>   m_info;
        [[no_unique_address]] DeviceRef<PowerInfoDevice<>> m_power_info;

    public:
        SCTDevice() = delete;

        SCTDevice(DeviceRef<SCTInfoDevice<>>&& info, DeviceRef<PowerInfoDevice<>>&& power_info)
            : m_info(std::move(info)), m_power_info(std::move(power_info))
        {
        }

        // -------------------------------------------------------------------------
        // INITIALIZATION
        // -------------------------------------------------------------------------
        /**
         * @brief Initialize SCT by switching to the requested source.
         *
         * @param config SCT configuration.
         * @return true if the source switch completed successfully.
         */
        [[nodiscard]] inline bool init(const SCTConfig& config)
        {
            const PowerVoltageRangeMode voltage_range     = m_power_info->get_voltage_range_mode();
            const uint32_t              current_sysclk_hz = m_info->get_sysclk_freq_hz();
            const uint32_t              target_sysclk_hz  = m_info->get_sysclk_freq_hz_for_source(config.sysclk_source);

            const uint32_t target_hclk_hz = config.get_hclk_freq_hz(target_sysclk_hz);

            // ---------------------------------------------------------------------
            // Validate configuration
            // ---------------------------------------------------------------------
            auto error = config.validate(target_sysclk_hz, voltage_range);
            expect(!error.has_value(), error.value_or("Unknown SCT configuration error"));

            // ---------------------------------------------------------------------
            // Validate source readiness
            // ---------------------------------------------------------------------
            switch (config.sysclk_source)
            {
                case SCTSYSCLKSource::kHSI:
                    expect(HSIOscillatorInterface::is_ready(), "Cannot switch SYSCLK to HSI because HSI is not ready");
                    break;

                case SCTSYSCLKSource::kHSE:
                    expect(HSEOscillatorInterface::is_ready(), "Cannot switch SYSCLK to HSE because HSE is not ready");
                    break;

                case SCTSYSCLKSource::kPLL:
                    expect(PLLInterface::is_ready(), "Cannot switch SYSCLK to PLL because PLL is not ready");
                    break;
            }

            // ---------------------------------------------------------------------
            // Increase FLASH latency first if required
            // ---------------------------------------------------------------------
            const FlashLatency required_flash_latency =
                FlashInterface::calculate_flash_latency(target_hclk_hz, m_power_info->get_voltage_range_mode());

            if (required_flash_latency > FlashInterface::get_latency())
            {
                FlashInterface::set_latency(required_flash_latency);

                expect(FlashInterface::wait_for_latency(required_flash_latency,
                                                        FlashInterface::skDefaultLatencySwitchTimeoutCount),
                       "FLASH latency failed to increase within timeout");
            }

            // ---------------------------------------------------------------------
            // Work out transition behavior
            // ---------------------------------------------------------------------

            const bool switching_to_pll_above_80mhz =
                (config.sysclk_source == SCTSYSCLKSource::kPLL) && (target_sysclk_hz > 80'000'000U);

            const bool leaving_pll_above_80mhz = (m_info->get_source_status() == SCTSourceStatus::kPLL) &&
                                                 (current_sysclk_hz > 80'000'000U) &&
                                                 (config.sysclk_source != SCTSYSCLKSource::kPLL);

            bool temporary_hpre_div2_applied = false;

            // ---------------------------------------------------------------------
            // Intermediate HCLK /2 step before dangerous source transitions
            // ---------------------------------------------------------------------
            if (switching_to_pll_above_80mhz)
            {
                if ((m_info->get_ahb_prescaler() == SCTAHBPrescaler::kDiv1) ||
                    (config.ahb_prescaler == SCTAHBPrescaler::kDiv1))
                {
                    InterfaceT::set_ahb_prescaler(SCTAHBPrescaler::kDiv2);
                    temporary_hpre_div2_applied = true;
                }
            }
            else if (leaving_pll_above_80mhz)
            {
                InterfaceT::set_ahb_prescaler(SCTAHBPrescaler::kDiv2);
                temporary_hpre_div2_applied = true;
            }

            // ---------------------------------------------------------------------
            // Switch SYSCLK source
            // ---------------------------------------------------------------------
            InterfaceT::set_sysclk_source(config.sysclk_source);

            expect(wait_for_source_active(config.sysclk_source, InterfaceT::skDefaultSourceSwitchTimeoutCount),
                   "SCT source switch failed to complete within timeout");

            // ---------------------------------------------------------------------
            // HCLK / APB sequencing
            //
            // Match HAL behavior:
            // - if HCLK changes, first force APB1/APB2 to /16
            // - then apply final HCLK
            // - then apply final APB1/APB2
            // - if only temporary /2 was applied, complete with final HCLK now
            // ---------------------------------------------------------------------

            const bool hclk_changed =
                temporary_hpre_div2_applied || (InterfaceT::get_ahb_prescaler() != config.ahb_prescaler);

            if (hclk_changed)
            {
                InterfaceT::set_apb1_prescaler(SCTAPB1Prescaler::kDiv16);
                InterfaceT::set_apb2_prescaler(SCTAPB2Prescaler::kDiv16);
                InterfaceT::set_ahb_prescaler(config.ahb_prescaler);
            }
            else if (temporary_hpre_div2_applied)
            {
                InterfaceT::set_ahb_prescaler(config.ahb_prescaler);
            }

            InterfaceT::set_apb1_prescaler(config.apb1_prescaler);
            InterfaceT::set_apb2_prescaler(config.apb2_prescaler);

            // Short delay to ensure APB prescaler changes take effect before frequency checks below
            TimingContext::delay_countdown(2000U);

            // ---------------------------------------------------------------------
            // Decrease FLASH latency afterward if allowed
            // ---------------------------------------------------------------------
            if (required_flash_latency < FlashInterface::get_latency())
            {
                FlashInterface::set_latency(required_flash_latency);

                expect(TimingContext::wait_for_with_timeout_countdown(
                           [&]() -> bool { return FlashInterface::get_latency() == required_flash_latency; },
                           InterfaceT::skDefaultSourceSwitchTimeoutCount),
                       "FLASH latency failed to decrease within timeout");
            }

            SystemCoreClockUpdate();
            HAL_InitTick(uwTickPrio);

            // -----------------------------------------------------------------
            // MCO
            // -----------------------------------------------------------------
            InterfaceT::config_mco(config.mco_source, config.mco_prescaler);

            // -----------------------------------------------------------------
            // Peripeheral clock mux
            // -----------------------------------------------------------------
            InterfaceT::set_usart1_source(config.usart1_source);
            InterfaceT::set_usart2_source(config.usart2_source);
            InterfaceT::set_usart3_source(config.usart3_source);
            InterfaceT::set_uart4_source(config.uart4_source);
            InterfaceT::set_uart5_source(config.uart5_source);
            InterfaceT::set_lpuart1_source(config.lpuart1_source);
            InterfaceT::set_i2c1_source(config.i2c1_source);
            InterfaceT::set_i2c2_source(config.i2c2_source);
            InterfaceT::set_i2c3_source(config.i2c3_source);
            InterfaceT::set_i2c4_source(config.i2c4_source);
            InterfaceT::set_lptim1_source(config.lptim1_source);
            InterfaceT::set_sai1_source(config.sai1_source);
            InterfaceT::set_i2s_source(config.i2s_source);
            InterfaceT::set_fdcan_source(config.fdcan_source);
            InterfaceT::set_rng_source(config.rng_source);
            InterfaceT::set_usb_source(config.usb_source);
            InterfaceT::set_adc12_source(config.adc12_source);
            InterfaceT::set_adc345_source(config.adc345_source);
            InterfaceT::set_quadspi_source(config.quadspi_source);

            return true;
        }

    private:
        [[nodiscard]] static bool wait_for_source_active(const SCTSYSCLKSource sysclk_source,
                                                         const uint32_t        timeout_count)
        {
            return TimingContext::wait_for_with_timeout_countdown(
                [sysclk_source]() -> bool { return InterfaceT::is_source_active(sysclk_source); }, timeout_count);
        }
    };

}  // namespace valle::platform