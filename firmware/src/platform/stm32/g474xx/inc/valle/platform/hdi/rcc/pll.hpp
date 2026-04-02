#pragma once

#include "valle/platform/hardware/rcc/pll.hpp"
#include "valle/platform/hdi/rcc/hse.hpp"
#include "valle/platform/hdi/rcc/hsi.hpp"

namespace valle::platform
{
    template <typename T = void>
    struct PllHdi : public PllInterface
    {
        struct Descriptor : public UniqueDeviceDescriptor
        {
        };
    };

    // =========================================================================
    // PLL INFO HDI
    // =========================================================================
    template <typename T = void>
    class PllInfoHdi
    {
    public:
        struct Descriptor : public SharedDeviceDescriptor
        {
            constexpr static bool skNeedsInit = false;
        };

        using HseOscillatorInfoT = HseOscillatorInfoHdi<>;
        using HsiOscillatorInfoT = HsiOscillatorInfoHdi<>;

        using DependDevices = TypeList<>;
        using InjectDevices = TypeList<HseOscillatorInfoT, HsiOscillatorInfoT>;

    private:
        PllInterface                                        m_interface{};
        [[no_unique_address]] DeviceRef<HseOscillatorInfoT> m_hse_info_device;
        [[no_unique_address]] DeviceRef<HsiOscillatorInfoT> m_hsi_info_device;

    public:
        PllInfoHdi() = delete;

        PllInfoHdi(DeviceRef<HseOscillatorInfoT>&& hse_info_device, DeviceRef<HsiOscillatorInfoT>&& hsi_info_device)
            : m_hse_info_device(std::move(hse_info_device)), m_hsi_info_device(std::move(hsi_info_device))
        {
        }

        [[nodiscard]] bool is_ready() const
        {
            return m_interface.is_ready();
        }

        [[nodiscard]] PllSource get_source() const
        {
            return m_interface.get_source();
        }

        [[nodiscard]] PllMDivider get_m_divider() const
        {
            return m_interface.get_m_divider();
        }

        [[nodiscard]] uint32_t get_n_multiplier() const
        {
            return m_interface.get_n_multiplier();
        }

        [[nodiscard]] PllPDivider get_p_divider() const
        {
            return m_interface.get_p_divider();
        }

        [[nodiscard]] PllQDivider get_q_divider() const
        {
            return m_interface.get_q_divider();
        }

        [[nodiscard]] PllRDivider get_r_divider() const
        {
            return m_interface.get_r_divider();
        }

        [[nodiscard]] bool is_p_output_enabled() const
        {
            return m_interface.is_p_output_enabled();
        }

        [[nodiscard]] bool is_q_output_enabled() const
        {
            return m_interface.is_q_output_enabled();
        }

        [[nodiscard]] bool is_r_output_enabled() const
        {
            return m_interface.is_r_output_enabled();
        }

        [[nodiscard]] constexpr uint32_t get_source_freq_hz_for_source(const PllSource source) const
        {
            switch (source)
            {
                case PllSource::kHSI:
                    return m_hsi_info_device->get_frequency_hz();

                case PllSource::kHSE:
                    return m_hse_info_device->get_frequency_hz();

                default:
                    return 0;
            }
        }

        [[nodiscard]] uint32_t get_source_freq_hz() const
        {
            return get_source_freq_hz_for_source(get_source());
        }

        [[nodiscard]] uint32_t get_r_output_freq_hz() const
        {
            return m_interface.get_r_output_freq_hz(get_source_freq_hz());
        }

        [[nodiscard]] uint32_t get_p_output_freq_hz() const
        {
            return m_interface.get_p_output_freq_hz(get_source_freq_hz());
        }

        [[nodiscard]] uint32_t get_q_output_freq_hz() const
        {
            return m_interface.get_q_output_freq_hz(get_source_freq_hz());
        }
    };

}  // namespace valle::platform