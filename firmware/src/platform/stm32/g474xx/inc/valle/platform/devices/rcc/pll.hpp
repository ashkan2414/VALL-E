#pragma once

#include <optional>

#include "valle/platform/core.hpp"
#include "valle/platform/devices/power.hpp"
#include "valle/platform/devices/rcc/hse.hpp"
#include "valle/platform/devices/rcc/hsi.hpp"
#include "valle/platform/hardware/rcc/pll.hpp"
#include "valle/platform/hardware/rcc/sct.hpp"

namespace valle::platform
{
    // =========================================================================
    // PLL INFO DEVICE
    // =========================================================================
    template <typename T = void>
    class PllInfoDevice
    {
    public:
        struct Descriptor : public SharedDeviceDescriptor
        {
            constexpr static bool skNeedsInit = false;
        };

        using InterfaceT = PllInterface;

        using DependDevices = TypeList<>;
        using InjectDevices = TypeList<HseOscillatorInfoDevice<>, HsiOscillatorInfoDevice<>>;

    private:
        [[no_unique_address]] DeviceRef<HseOscillatorInfoDevice<>> m_hse_info_device;
        [[no_unique_address]] DeviceRef<HsiOscillatorInfoDevice<>> m_hsi_info_device;

    public:
        PllInfoDevice() = delete;

        PllInfoDevice(DeviceRef<HseOscillatorInfoDevice<>>&& hse_info_device,
                      DeviceRef<HsiOscillatorInfoDevice<>>&& hsi_info_device)
            : m_hse_info_device(std::move(hse_info_device)), m_hsi_info_device(std::move(hsi_info_device))
        {
        }

        [[nodiscard]] bool is_ready() const
        {
            return InterfaceT::is_ready();
        }

        [[nodiscard]] PllSource get_source() const
        {
            return InterfaceT::get_source();
        }

        [[nodiscard]] PllMDivider get_m_divider() const
        {
            return InterfaceT::get_m_divider();
        }

        [[nodiscard]] uint32_t get_n_multiplier() const
        {
            return InterfaceT::get_n_multiplier();
        }

        [[nodiscard]] PllPDivider get_p_divider() const
        {
            return InterfaceT::get_p_divider();
        }

        [[nodiscard]] PllQDivider get_q_divider() const
        {
            return InterfaceT::get_q_divider();
        }

        [[nodiscard]] PllRDivider get_r_divider() const
        {
            return InterfaceT::get_r_divider();
        }

        [[nodiscard]] bool is_p_output_enabled() const
        {
            return InterfaceT::is_p_output_enabled();
        }

        [[nodiscard]] bool is_q_output_enabled() const
        {
            return InterfaceT::is_q_output_enabled();
        }

        [[nodiscard]] bool is_r_output_enabled() const
        {
            return InterfaceT::is_r_output_enabled();
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
            return InterfaceT::get_r_output_freq_hz(get_source_freq_hz());
        }

        [[nodiscard]] uint32_t get_p_output_freq_hz() const
        {
            return InterfaceT::get_p_output_freq_hz(get_source_freq_hz());
        }

        [[nodiscard]] uint32_t get_q_output_freq_hz() const
        {
            return InterfaceT::get_q_output_freq_hz(get_source_freq_hz());
        }
    };

    // =============================================================================
    // PLL DEVICE
    // =============================================================================

    // ----------------------------------------------------------------------------
    // CONFIGURATION
    // ----------------------------------------------------------------------------
    /**
     * @brief PLL configuration.
     *
     * Notes:
     * - source_hz is the frequency of the already-configured PLL source oscillator.
     *   For example, 16 MHz for HSI or 8 MHz for HSE.
     * - voltage_range determines the legal PLL/VCO/output frequency limits.
     * - P/Q/R dividers are optional because not every PLL output must be enabled.
     * - At least one output should generally be configured if the PLL is being used.
     */
    struct PllConfig
    {
        bool        enabled          = true;
        PllSource   source           = PllSource::kHSI;
        PllMDivider m_divider        = PllMDivider::kDiv4;
        uint32_t    n_multiplier     = 85;
        PllRDivider r_divider        = PllRDivider::kDiv2;
        PllPDivider p_divider        = PllPDivider::kDiv2;
        PllQDivider q_divider        = PllQDivider::kDiv2;
        bool        r_output_enabled = false;
        bool        p_output_enabled = false;
        bool        q_output_enabled = false;

        [[nodiscard]] constexpr std::optional<std::string_view>
        validate(  // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
            const uint32_t              source_hz,
            const PowerVoltageRangeMode voltage_range) const
        {
            if (!enabled)
            {
                // If PLL is not enabled, we don't need to validate the rest of the config.
                return std::nullopt;
            }

            return PllInterface::validate_config(
                source_hz, voltage_range, m_divider, n_multiplier, r_divider, p_divider, q_divider);
        }

        [[nodiscard]] constexpr uint32_t get_input_freq_hz(const uint32_t source_hz) const
        {
            if (enabled)
            {
                return PllInterface::calculate_input_freq_hz(source_hz, m_divider);
            }
            return 0U;
        }

        [[nodiscard]] constexpr uint32_t get_vco_freq_hz(const uint32_t source_hz) const
        {
            if (enabled)
            {
                return PllInterface::calculate_vco_freq_hz(source_hz, m_divider, n_multiplier);
            }

            return 0U;
        }

        [[nodiscard]] constexpr uint32_t get_r_output_freq_hz(const uint32_t vco_hz) const
        {
            if (enabled && r_output_enabled)
            {
                return PllInterface::calculate_r_output_freq_hz(vco_hz, r_divider);
            }

            return 0U;
        }

        [[nodiscard]] constexpr uint32_t get_p_output_freq_hz(const uint32_t vco_hz) const
        {
            if (enabled && p_output_enabled)
            {
                return PllInterface::calculate_p_output_freq_hz(vco_hz, p_divider);
            }

            return 0U;
        }

        [[nodiscard]] constexpr uint32_t get_q_output_freq_hz(const uint32_t vco_hz) const
        {
            if (enabled && q_output_enabled)
            {
                return PllInterface::calculate_q_output_freq_hz(vco_hz, q_divider);
            }

            return 0U;
        }
    };

    // ----------------------------------------------------------------------------
    // DEVICE CLASS
    // ----------------------------------------------------------------------------

    /**
     * @brief Main PLL device.
     */
    template <typename T = void>
    class PllDevice
    {
    public:
        struct Descriptor : public UniqueDeviceDescriptor
        {
        };

        using InterfaceT = PllInterface;

        using DependDevices = TypeList<PowerDevice<>>;
        using InjectDevices = TypeList<PllInfoDevice<>, PowerInfoDevice<>>;

    private:
        [[no_unique_address]] DeviceRef<PllInfoDevice<>>   m_info;
        [[no_unique_address]] DeviceRef<PowerInfoDevice<>> m_power_info;

    public:
        PllDevice() = delete;

        PllDevice(DeviceRef<PllInfoDevice<>>&& info, DeviceRef<PowerInfoDevice<>>&& power_info)
            : m_info(std::move(info)), m_power_info(std::move(power_info))
        {
        }

        // -------------------------------------------------------------------------
        // INITIALIZATION
        // -------------------------------------------------------------------------
        /**
         * @brief Initialize and optionally enable the PLL.
         *
         * Expected sequence:
         * 1. Validate the requested configuration.
         * 2. Disable PLL and wait until it is fully unlocked.
         * 3. Configure all requested PLL parameters.
         * 4. Enable requested outputs.
         * 5. Enable PLL and wait for lock.
         *
         * @param config PLL configuration.
         * @return true if initialization succeeded, false if the PLL failed to lock.
         */
        [[nodiscard]] inline bool init(const PllConfig& config)
        {
            const auto error = config.validate(m_info->get_source_freq_hz_for_source(config.source),
                                               m_power_info->get_voltage_range_mode());
            expect(!error.has_value(), error.value_or("Unknown PLL configuration error"));

            const bool pll_used_as_sysclk = SctInterface::get_source_status() == SctSourceStatus::kPLL;
            expect(!pll_used_as_sysclk, "Cannot reconfigure or disable PLL while it is used as SYSCLK");

            // Hardware requires PLL reconfiguration to happen only while disabled.
            InterfaceT::disable();

            // Wait until the PLL is fully disabled/unlocked before reconfiguring.
            expect(wait_for_not_ready(InterfaceT::skDefaultDisableTimeoutCount),
                   "PLL failed to disable within timeout");

            // Validate the requested configuration against hardware limits and expected source frequency.
            InterfaceT::config_domains(config.source,
                                       config.m_divider,
                                       config.n_multiplier,
                                       config.r_divider,
                                       config.p_divider,
                                       config.q_divider);

            // Enable only the requested PLL outputs.
            if (config.r_output_enabled)  // NOLINT(bugprone-branch-clone)
            {
                InterfaceT::enable_r_output();
            }
            else
            {
                InterfaceT::disable_r_output();
            }

            if (config.p_output_enabled)  // NOLINT(bugprone-branch-clone)
            {
                InterfaceT::enable_p_output();
            }
            else
            {
                InterfaceT::disable_p_output();
            }

            if (config.q_output_enabled)  // NOLINT(bugprone-branch-clone)
            {
                InterfaceT::enable_q_output();
            }
            else
            {
                InterfaceT::disable_q_output();
            }

            // If the PLL should remain disabled after configuration, stop here.
            if (!config.enabled)
            {
                return true;
            }

            InterfaceT::enable();

            expect(wait_for_ready(InterfaceT::skDefaultEnableTimeoutCount),
                   "PLL failed to become ready within timeout");

            return true;
        }

    private:
        [[nodiscard]] static bool wait_for_ready(const uint32_t timeout_count)
        {
            return TimingContext::wait_for_with_timeout_countdown([]() { return InterfaceT::is_ready(); },
                                                                  timeout_count);
        }

        [[nodiscard]] static bool wait_for_not_ready(const uint32_t timeout_count)
        {
            return TimingContext::wait_for_with_timeout_countdown([]() -> bool { return !InterfaceT::is_ready(); },
                                                                  timeout_count);
        }
    };

}  // namespace valle::platform