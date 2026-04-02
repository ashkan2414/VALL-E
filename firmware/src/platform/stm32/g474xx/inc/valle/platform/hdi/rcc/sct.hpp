#pragma once

#include "valle/platform/hardware/rcc/sct.hpp"

namespace valle::platform
{
    template <typename T = void>
    struct SctHdi : public SctInterface
    {
        struct Descriptor : public UniqueDeviceDescriptor
        {
        };
    };

    // =========================================================================
    // SCT INFO HDI
    // =========================================================================
    template <typename T = void>
    class SctInfoHdi
    {
    public:
        struct Descriptor : public SharedDeviceDescriptor
        {
            constexpr static bool skNeedsInit = false;
        };

        using InterfaceT         = SctInterface;
        using HseOscillatorInfoT = HseOscillatorInfo<T>;
        using HsiOscillatorInfoT = HsiOscillatorInfo<T>;
        using PllInfoT           = PllInfo<T>;

        using DependDevices = TypeList<>;
        using InjectDevices = TypeList<HseOscillatorInfoT, HsiOscillatorInfoT, PllInfoT>;

    private:
        InterfaceT                                          m_interface{};
        [[no_unique_address]] DeviceRef<HseOscillatorInfoT> m_hse_info;
        [[no_unique_address]] DeviceRef<HsiOscillatorInfoT> m_hsi_info;
        [[no_unique_address]] DeviceRef<PllInfoT>           m_pll_info;

    public:
        SctInfoHdi() = delete;

        SctInfoHdi(DeviceRef<HseOscillatorInfoT>&& hse_info,
                   DeviceRef<HsiOscillatorInfoT>&& hsi_info,
                   DeviceRef<PllInfoT>&&           pll_info)
            : m_hse_info(std::move(hse_info))
            , m_hsi_info(std::move(hsi_info))
            , m_pll_info(std::move(pll_info)){}

            : m_hse_info(std::move(hse_info)),
              m_hsi_info(std::move(hsi_info)),
              m_pll_info(std::move(pll_info))
        {
        }

        [[nodiscard]] SctSourceStatus get_source_status() const
        {
            return m_interface.get_source_status();
        }

        [[nodiscard]] SctAHBPrescaler get_ahb_prescaler() const
        {
            return m_interface.get_ahb_prescaler();
        }

        [[nodiscard]] SctAPB1Prescaler get_apb1_prescaler() const
        {
            return m_interface.get_apb1_prescaler();
        }

        [[nodiscard]] SctAPB2Prescaler get_apb2_prescaler() const
        {
            return m_interface.get_apb2_prescaler();
        }

        [[nodiscard]] uint32_t get_sysclk_freq_hz_for_source(const SctSYSCLKSource sysclk_source) const
        {
            switch (sysclk_source)
            {
                case SctSYSCLKSource::kHSI:
                    return m_hsi_info->get_frequency_hz();

                case SctSYSCLKSource::kHSE:
                    return m_hse_info->get_frequency_hz();

                case SctSYSCLKSource::kPLL:
                    return m_pll_info->get_r_output_freq_hz();
            }

            return 0;
        }

        [[nodiscard]] uint32_t get_sysclk_freq_hz() const
        {
            switch (get_source_status())
            {
                case SctSourceStatus::kHSI:
                    return m_hsi_info->get_frequency_hz();

                case SctSourceStatus::kHSE:
                    return m_hse_info->get_frequency_hz();

                case SctSourceStatus::kPLL:
                    return m_pll_info->get_r_output_freq_hz();
            }

            return 0;
        }

        [[nodiscard]] uint32_t get_hclk_freq_hz() const
        {
            return m_interface.get_hclk_freq_hz(get_sysclk_freq_hz());
        }

        [[nodiscard]] uint32_t get_pclk1_freq_hz() const
        {
            return m_interface.get_pclk1_freq_hz(get_hclk_freq_hz());
        }

        [[nodiscard]] uint32_t get_apb1_peripheral_freq_hz() const
        {
            return m_interface.get_apb1_peripheral_freq_hz(get_pclk1_freq_hz());
        }

        [[nodiscard]] uint32_t get_apb1_timer_freq_hz() const
        {
            return m_interface.get_apb1_timer_freq_hz(get_pclk1_freq_hz());
        }

        [[nodiscard]] uint32_t get_pclk2_freq_hz() const
        {
            return m_interface.get_pclk2_freq_hz(get_hclk_freq_hz());
        }

        [[nodiscard]] uint32_t get_apb2_peripheral_freq_hz() const
        {
            return m_interface.get_apb2_peripheral_freq_hz(get_pclk2_freq_hz());
        }

        [[nodiscard]] uint32_t get_apb2_timer_freq_hz() const
        {
            return m_interface.get_apb2_timer_freq_hz(get_pclk2_freq_hz());
        }

        // ---------------------------------------------------------------------
        // Peripheral clock tree
        // ---------------------------------------------------------------------
        [[nodiscard]] SctUSART1ClockSource get_usart1_source() const
        {
            return m_interface.get_usart1_source();
        }

        [[nodiscard]] SctUSART2ClockSource get_usart2_source() const
        {
            return m_interface.get_usart2_source();
        }

        [[nodiscard]] SctUSART3ClockSource get_usart3_source() const
        {
            return m_interface.get_usart3_source();
        }

#if defined(UART4)
        [[nodiscard]] SctUart4ClockSource get_uart4_source() const
        {
            return m_interface.get_uart4_source();
        }

        [[nodiscard]] SctUart5ClockSource get_uart5_source() const
        {
            return m_interface.get_uart5_source();
        }
#endif

        [[nodiscard]] SctLPUart1ClockSource get_lpuart1_source() const
        {
            return m_interface.get_lpuart1_source();
        }

        [[nodiscard]] SctI2c1ClockSource get_i2c1_source() const
        {
            return m_interface.get_i2c1_source();
        }

        [[nodiscard]] SctI2c2ClockSource get_i2c2_source() const
        {
            return m_interface.get_i2c2_source();
        }

        [[nodiscard]] SctI2c3ClockSource get_i2c3_source() const
        {
            return m_interface.get_i2c3_source();
        }

        [[nodiscard]] SctI2c4ClockSource get_i2c4_source() const
        {
            return m_interface.get_i2c4_source();
        }

        [[nodiscard]] SctLPTim1ClockSource get_lptim1_source() const
        {
            return m_interface.get_lptim1_source();
        }

        [[nodiscard]] SctSAI1ClockSource get_sai1_source() const
        {
            return m_interface.get_sai1_source();
        }

        [[nodiscard]] SctI2SClockSource get_i2s_source() const
        {
            return m_interface.get_i2s_source();
        }

#if defined(FDCAN1)
        [[nodiscard]] SctFDCANClockSource get_fdcan_source() const
        {
            return m_interface.get_fdcan_source();
        }
#endif /* FDCAN1 */

        [[nodiscard]] SctRNGClockSource get_rng_source() const
        {
            return m_interface.get_rng_source();
        }

        [[nodiscard]] SctUSBClockSource get_usb_source() const
        {
            return m_interface.get_usb_source();
        }

        [[nodiscard]] SctAdc12ClockSource get_adc12_source() const
        {
            return m_interface.get_adc12_source();
        }

        [[nodiscard]] SctAdc345ClockSource get_adc345_source() const
        {
            return m_interface.get_adc345_source();
        }

        [[nodiscard]] SctQuadSpiClockSource get_quadspi_source() const
        {
            return m_interface.get_quadspi_source();
        }
    };

}  // namespace valle::platform