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

            return PllInfoHdi::validate_config(
                source_hz, voltage_range, m_divider, n_multiplier, r_divider, p_divider, q_divider);
        }

        [[nodiscard]] constexpr uint32_t get_input_freq_hz(const uint32_t source_hz) const
        {
            if (enabled)
            {
                return PllInfoHdi::calculate_input_freq_hz(source_hz, m_divider);
            }
            return 0U;
        }

        [[nodiscard]] constexpr uint32_t get_vco_freq_hz(const uint32_t source_hz) const
        {
            if (enabled)
            {
                return PllInfoHdi::calculate_vco_freq_hz(source_hz, m_divider, n_multiplier);
            }

            return 0U;
        }

        [[nodiscard]] constexpr uint32_t get_r_output_freq_hz(const uint32_t vco_hz) const
        {
            if (enabled && r_output_enabled)
            {
                return PllInfoHdi::calculate_r_output_freq_hz(vco_hz, r_divider);
            }

            return 0U;
        }

        [[nodiscard]] constexpr uint32_t get_p_output_freq_hz(const uint32_t vco_hz) const
        {
            if (enabled && p_output_enabled)
            {
                return PllInfoHdi::calculate_p_output_freq_hz(vco_hz, p_divider);
            }

            return 0U;
        }

        [[nodiscard]] constexpr uint32_t get_q_output_freq_hz(const uint32_t vco_hz) const
        {
            if (enabled && q_output_enabled)
            {
                return PllInfoHdi::calculate_q_output_freq_hz(vco_hz, q_divider);
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
    class Pll
    {
    public:
        struct Descriptor : public UniqueDeviceDescriptor
        {
        };

        using HdiT          = PllHdi<T>;
        using PllInfoT      = PllInfo<T>;
        using PowerInfoHdiT = PowerInfoHdi<T>;
        using SctInfoHdiT   = SctInfoHdi<T>;

        using DependDevices = TypeList<Power<>>;
        using InjectDevices = TypeList<HdiT, PllInfoT, PowerInfoHdiT, SctInfoHdiT>;

    private:
        [[no_unique_address]] DeviceRef<HdiT>          m_hw;
        [[no_unique_address]] DeviceRef<PllInfoT>      m_info;
        [[no_unique_address]] DeviceRef<PowerInfoHdiT> m_power_info;
        [[no_unique_address]] DeviceRef<SctInfoHdiT>   m_sct_info;

    public:
        Pll() = delete;

        Pll(DeviceRef<HdiT>&&          hw,
            DeviceRef<PllInfoT>&&      info,
            DeviceRef<PowerInfoHdiT>&& power_info,
            DeviceRef<SctInfoHdiT>&&   sct_info)
            : m_hw(std::move(hw))
            , m_info(std::move(info))
            , m_power_info(std::move(power_info))
            , m_sct_info(std::move(sct_info))
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

            const bool pll_used_as_sysclk = m_sct_info->get_source_status() == SctSourceStatus::kPLL;
            expect(!pll_used_as_sysclk, "Cannot reconfigure or disable PLL while it is used as SYSCLK");

            // Hardware requires PLL reconfiguration to happen only while disabled.
            m_hw->disable();

            // Wait until the PLL is fully disabled/unlocked before reconfiguring.
            expect(wait_for_not_ready(HdiT::skDefaultDisableTimeoutCount), "PLL failed to disable within timeout");

            // Validate the requested configuration against hardware limits and expected source frequency.
            m_hw->config_domains(config.source,
                                 config.m_divider,
                                 config.n_multiplier,
                                 config.r_divider,
                                 config.p_divider,
                                 config.q_divider);

            // Enable only the requested PLL outputs.
            if (config.r_output_enabled)  // NOLINT(bugprone-branch-clone)
            {
                m_hw->enable_r_output();
            }
            else
            {
                m_hw->disable_r_output();
            }

            if (config.p_output_enabled)  // NOLINT(bugprone-branch-clone)
            {
                m_hw->enable_p_output();
            }
            else
            {
                m_hw->disable_p_output();
            }

            if (config.q_output_enabled)  // NOLINT(bugprone-branch-clone)
            {
                m_hw->enable_q_output();
            }
            else
            {
                m_hw->disable_q_output();
            }

            // If the PLL should remain disabled after configuration, stop here.
            if (!config.enabled)
            {
                return true;
            }

            m_hw->enable();

            expect(wait_for_ready(HdiT::skDefaultEnableTimeoutCount), "PLL failed to become ready within timeout");

            return true;
        }

    private:
        [[nodiscard]] bool wait_for_ready(const uint32_t timeout_count) const
        {
            return TimingContext::wait_for_with_timeout_countdown([this]() { return m_hw->is_ready(); }, timeout_count);
        }

        [[nodiscard]] bool wait_for_not_ready(const uint32_t timeout_count) const
        {
            return TimingContext::wait_for_with_timeout_countdown([this]() -> bool { return !m_hw->is_ready(); },
                                                                  timeout_count);
        }
    };

}  // namespace valle::platform