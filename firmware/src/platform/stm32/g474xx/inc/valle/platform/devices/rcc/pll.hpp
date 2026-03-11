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
    // FORWARD DECLARATIONS
    // =============================================================================
    class PLLRootDevice;

    template <typename T>
    class PLLDevice;

    // =============================================================================
    // ROOT PLL DEVICE
    // =============================================================================
    /**
     * @brief Root interface device for the main PLL.
     *
     * This device exists to provide a clean place in the device tree for the PLL
     * synthesizer. The PLL itself is a single hardware block on STM32G474, so this
     * is not templated.
     *
     * The PLL is not an oscillator. It is a frequency synthesizer that derives one
     * or more output clocks from an oscillator source such as HSI or HSE.
     */
    class PLLRootDevice
    {
    public:
        struct Descriptor : public InterfaceDeviceDescriptor
        {
            using Children = DeviceTreeList<PLLDevice<void>>;
        };
    };

    // =========================================================================
    // PLL INFO DEVICE
    // =========================================================================
    template <typename T = void>
    class PLLInfoDevice
    {
    public:
        struct Descriptor : public SharedDeviceDescriptor
        {
            constexpr static bool skNeedsInit = false;
        };

        using InterfaceT = PLLInterface;

        using DependDevices = TypeList<PLLRootDevice>;
        using InjectDevices = TypeList<HSEOscillatorInfoDevice<>, HSIOscillatorInfoDevice<>>;

    private:
        [[no_unique_address]] DeviceRef<HSEOscillatorInfoDevice<>> m_hse_info_device;
        [[no_unique_address]] DeviceRef<HSIOscillatorInfoDevice<>> m_hsi_info_device;

    public:
        PLLInfoDevice() = delete;

        PLLInfoDevice(DeviceRef<HSEOscillatorInfoDevice<>>&& hse_info_device,
                      DeviceRef<HSIOscillatorInfoDevice<>>&& hsi_info_device)
            : m_hse_info_device(std::move(hse_info_device)), m_hsi_info_device(std::move(hsi_info_device))
        {
        }

        [[nodiscard]] bool is_ready() const
        {
            return InterfaceT::is_ready();
        }

        [[nodiscard]] PLLSource get_source() const
        {
            return InterfaceT::get_source();
        }

        [[nodiscard]] PLLMDivider get_m_divider() const
        {
            return InterfaceT::get_m_divider();
        }

        [[nodiscard]] uint32_t get_n_multiplier() const
        {
            return InterfaceT::get_n_multiplier();
        }

        [[nodiscard]] PLLPDivider get_p_divider() const
        {
            return InterfaceT::get_p_divider();
        }

        [[nodiscard]] PLLQDivider get_q_divider() const
        {
            return InterfaceT::get_q_divider();
        }

        [[nodiscard]] PLLRDivider get_r_divider() const
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

        [[nodiscard]] constexpr uint32_t get_source_freq_hz_for_source(const PLLSource source) const
        {
            switch (source)
            {
                case PLLSource::kHSI:
                    return m_hsi_info_device->get_frequency_hz();

                case PLLSource::kHSE:
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
    struct PLLConfig
    {
        bool        enabled          = true;
        PLLSource   source           = PLLSource::kHSI;
        PLLMDivider m_divider        = PLLMDivider::kDiv4;
        uint32_t    n_multiplier     = 85;
        PLLRDivider r_divider        = PLLRDivider::kDiv2;
        PLLPDivider p_divider        = PLLPDivider::kDiv2;
        PLLQDivider q_divider        = PLLQDivider::kDiv2;
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

            return PLLInterface::validate_config(
                source_hz, voltage_range, m_divider, n_multiplier, r_divider, p_divider, q_divider);
        }

        [[nodiscard]] constexpr uint32_t get_input_freq_hz(const uint32_t source_hz) const
        {
            if (enabled)
            {
                return PLLInterface::calculate_input_freq_hz(source_hz, m_divider);
            }
            return 0U;
        }

        [[nodiscard]] constexpr uint32_t get_vco_freq_hz(const uint32_t source_hz) const
        {
            if (enabled)
            {
                return PLLInterface::calculate_vco_freq_hz(source_hz, m_divider, n_multiplier);
            }

            return 0U;
        }

        [[nodiscard]] constexpr uint32_t get_r_output_freq_hz(const uint32_t vco_hz) const
        {
            if (enabled && r_output_enabled)
            {
                return PLLInterface::calculate_r_output_freq_hz(vco_hz, r_divider);
            }

            return 0U;
        }

        [[nodiscard]] constexpr uint32_t get_p_output_freq_hz(const uint32_t vco_hz) const
        {
            if (enabled && p_output_enabled)
            {
                return PLLInterface::calculate_p_output_freq_hz(vco_hz, p_divider);
            }

            return 0U;
        }

        [[nodiscard]] constexpr uint32_t get_q_output_freq_hz(const uint32_t vco_hz) const
        {
            if (enabled && q_output_enabled)
            {
                return PLLInterface::calculate_q_output_freq_hz(vco_hz, q_divider);
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
    class PLLDevice
    {
    public:
        struct Descriptor : public UniqueDeviceDescriptor
        {
        };

        using InterfaceT = PLLInterface;

        using DependDevices = TypeList<PLLRootDevice, PowerDevice<>>;
        using InjectDevices = TypeList<PLLInfoDevice<>, PowerInfoDevice<>>;

    private:
        [[no_unique_address]] DeviceRef<PLLInfoDevice<>>   m_info;
        [[no_unique_address]] DeviceRef<PowerInfoDevice<>> m_power_info;

    public:
        PLLDevice() = delete;

        PLLDevice(DeviceRef<PLLInfoDevice<>>&& info, DeviceRef<PowerInfoDevice<>>&& power_info)
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
        [[nodiscard]] inline bool init(const PLLConfig& config)
        {
            const auto error = config.validate(m_info->get_source_freq_hz_for_source(config.source),
                                               m_power_info->get_voltage_range_mode());
            expect(!error.has_value(), error.value_or("Unknown PLL configuration error"));

            const bool pll_used_as_sysclk = SCTInterface::get_source_status() == SCTSourceStatus::kPLL;
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