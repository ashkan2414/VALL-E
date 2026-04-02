#pragma once

#include <optional>

#include "valle/platform/core.hpp"
#include "valle/platform/hdi/power.hpp"

namespace valle::platform
{

    // =============================================================================
    // POWER DEVICE
    // =============================================================================

    // -----------------------------------------------------------------------------
    // CONFIGURATION
    // -----------------------------------------------------------------------------

    /**
     * @brief Initial regulation-related configuration.
     */
    struct PowerRegulationConfig
    {
        PowerVoltageRange voltage_range = PowerVoltageRange::kRange1;
#ifdef VALLE_PWR_HAS_RANGE1_BOOST
        bool enable_range1_boost = false;
#endif

        [[nodiscard]] constexpr PowerVoltageRangeMode get_voltage_range_mode() const
        {
#ifdef VALLE_PWR_HAS_RANGE1_BOOST
            const bool range1_boost = enable_range1_boost;
#else
            const bool range1_boost = false;
#endif

            return PowerInterface::get_voltage_range_mode(voltage_range, range1_boost);
        }
    };

    /**
     * @brief Initial backup-domain access configuration.
     */
    struct PowerBackupDomainConfig
    {
        bool enable_access = false;
    };

    /**
     * @brief Initial low-power run configuration.
     */
    struct PowerLowPowerRunConfig
    {
        bool enable = false;
    };

    /**
     * @brief Initial internal wakeup configuration.
     */
    struct PowerInternalWakeupConfig
    {
        bool enable = false;
    };

    /**
     * @brief Initial PUPD configuration access control.
     */
    struct PowerPUPDConfig
    {
        bool enable = false;
    };

#if defined(SRAM2_BASE)
    /**
     * @brief Initial SRAM2 retention configuration.
     */
    struct PowerSRAM2RetentionConfig
    {
        bool enable = false;
    };
#endif

    /**
     * @brief Top-level PWR peripheral configuration.
     *
     * This intentionally focuses on the parts of PWR that commonly matter during
     * startup and system bring-up.
     */
    struct PowerConfig
    {
        PowerRegulationConfig     regulation{};
        PowerBackupDomainConfig   backup_domain{};
        PowerLowPowerRunConfig    low_power_run{};
        PowerInternalWakeupConfig internal_wakeup{};
        PowerPUPDConfig           pupd_config{};
#if defined(SRAM2_BASE)
        PowerSRAM2RetentionConfig sram2_retention{};
#endif
    };

    // -----------------------------------------------------------------------------
    // DEVICE
    // -----------------------------------------------------------------------------
    /**
     * @brief Unique owning device for the PWR peripheral.
     */
    template <typename T = void>
    class Power
    {
    public:
        struct Descriptor : public UniqueDeviceDescriptor
        {
        };

        using InterfaceT = PowerInterface;

        using InjectDevices = TypeList<>;

        static constexpr uint32_t skDefaultVoltageScalingTimeoutCount = 100'000U;

        // -------------------------------------------------------------------------
        // INITIALIZATION
        // -------------------------------------------------------------------------
        /**
         * @brief Initialize the PWR peripheral state used by the platform.
         *
         * @param config Power configuration.
         * @return true if initialization succeeded.
         */
        [[nodiscard]] bool init(const PowerConfig& config)
        {
            // ---------------------------------------------------------------------
            // Voltage scaling
            // ---------------------------------------------------------------------
            if (config.regulation.voltage_range == PowerVoltageRange::kRange1)  // NOLINT(bugprone-branch-clone)
            {
                if (InterfaceT::get_regulation_voltage_scaling() != PowerVoltageRange::kRange1)
                {
                    InterfaceT::set_regulation_voltage_scaling(PowerVoltageRange::kRange1);

                    expect(wait_for_voltage_scaling_transition(skDefaultVoltageScalingTimeoutCount),
                           "PWR voltage scaling transition did not complete within timeout");
                }

#ifdef VALLE_PWR_HAS_RANGE1_BOOST
                if (InterfaceT::is_range1_boost_enabled() != config.regulation.enable_range1_boost)
                {
                    if (config.regulation.enable_range1_boost)  // NOLINT(bugprone-branch-clone)
                    {
                        InterfaceT::enable_range1_boost();
                    }
                    else
                    {
                        InterfaceT::disable_range1_boost();
                    }

                    expect(wait_for_voltage_scaling_transition(skDefaultVoltageScalingTimeoutCount),
                           "PWR voltage range 1 boost transition did not complete within timeout");

                    expect(InterfaceT::is_range1_boost_enabled() == config.regulation.enable_range1_boost,
                           "PWR voltage range 1 boost state does not match expected configuration after transition");
                }
#endif
            }
            else  // We want range 2
            {
                if (InterfaceT::get_regulation_voltage_scaling() != PowerVoltageRange::kRange2)
                {
                    InterfaceT::set_regulation_voltage_scaling(PowerVoltageRange::kRange2);

                    expect(wait_for_voltage_scaling_transition(skDefaultVoltageScalingTimeoutCount),
                           "PWR voltage scaling transition did not complete within timeout");
                }
            }

            // ---------------------------------------------------------------------
            // Backup domain access
            // ---------------------------------------------------------------------
            if (config.backup_domain.enable_access)  // NOLINT(bugprone-branch-clone)
            {
                InterfaceT::backup_domain_enable_access();
            }
            else
            {
                InterfaceT::backup_domain_disable_access();
            }

            // ---------------------------------------------------------------------
            // Low-power run
            // ---------------------------------------------------------------------
            if (config.low_power_run.enable)  // NOLINT(bugprone-branch-clone)
            {
                InterfaceT::low_power_run_enable();
            }
            else
            {
                InterfaceT::low_power_run_disable();
            }

            // ---------------------------------------------------------------------
            // Internal wakeup
            // ---------------------------------------------------------------------
            if (config.internal_wakeup.enable)  // NOLINT(bugprone-branch-clone)
            {
                InterfaceT::internal_wakeup_enable();
            }
            else
            {
                InterfaceT::internal_wakeup_disable();
            }

            // ---------------------------------------------------------------------
            // PUPD config access
            // ---------------------------------------------------------------------
            if (config.pupd_config.enable)  // NOLINT(bugprone-branch-clone)
            {
                InterfaceT::pupd_config_enable();
            }
            else
            {
                InterfaceT::pupd_config_disable();
            }

#ifdef VALLE_PWR_HAS_SRAM2_RETENTION
            // ---------------------------------------------------------------------
            // SRAM2 retention
            // ---------------------------------------------------------------------
            if (config.sram2_retention.enable)  // NOLINT(bugprone-branch-clone)
            {
                InterfaceT::sram2_retention_enable();
            }
            else
            {
                InterfaceT::sram2_retention_disable();
            }
#endif

            return true;
        }

    private:
        [[nodiscard]] static bool wait_for_voltage_scaling_transition(const uint32_t timeout_count)
        {
            return TimingContext::wait_for_with_timeout_countdown(
                []() -> bool { return InterfaceT::voltage_scaling_transition_done(); }, timeout_count);
        }
    };

}  // namespace valle::platform