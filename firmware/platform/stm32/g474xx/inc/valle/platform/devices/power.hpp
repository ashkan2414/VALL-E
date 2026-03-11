#pragma once

#include <optional>

#include "valle/platform/core.hpp"
#include "valle/platform/hardware/power.hpp"


namespace valle::platform
{
    // =============================================================================
    // FORWARD DECLARATIONS
    // =============================================================================
    class PowerRootDevice;

    template <typename T>
    class PowerDevice;

    // =============================================================================
    // ROOT POWER DEVICE
    // =============================================================================
    /**
     * @brief Root interface device for the PWR peripheral.
     *
     * The STM32 PWR peripheral is treated as a single ownership domain.
     * Functional subdivision lives in PowerInterface, but mutation ownership
     * remains centralized in PowerDevice.
     */
    class PowerRootDevice
    {
    public:
        struct Descriptor : public InterfaceDeviceDescriptor
        {
            using Children = DeviceTreeList<PowerDevice<void>>;
        };
    };

    // =============================================================================
    // POWER INFO DEVICE
    // =============================================================================

    /**
     * @brief Shared read-only info device for the PWR peripheral.
     *
     * This device provides non-mutating access to PWR state so multiple parts of
     * the system can observe power configuration and status without owning the
     * mutable PowerDevice.
     */
    template <typename T = void>
    class PowerInfoDevice
    {
    public:
        struct Descriptor : public SharedDeviceDescriptor
        {
            constexpr static bool skNeedsInit = false;
        };

        using InterfaceT = PowerInterface;

        using DependDevices = TypeList<PowerRootDevice>;
        using InjectDevices = TypeList<>;

        // ---------------------------------------------------------------------
        // REGULATION
        // ---------------------------------------------------------------------
        [[nodiscard]] static PowerVoltageRange get_voltage_range()
        {
            return InterfaceT::get_regulation_voltage_scaling();
        }

        [[nodiscard]] static PowerVoltageRangeMode get_voltage_range_mode()
        {
            return InterfaceT::get_voltage_range_mode();
        }

        [[nodiscard]] static bool is_voltage_scaling_transition_done()
        {
            return InterfaceT::voltage_scaling_transition_done();
        }

#ifdef VALLE_PWR_HAS_RANGE1_BOOST
        [[nodiscard]] static bool is_range1_boost_enabled()
        {
            return InterfaceT::is_range1_boost_enabled();
        }
#endif

        // ---------------------------------------------------------------------
        // BACKUP DOMAIN
        // ---------------------------------------------------------------------
        [[nodiscard]] static bool is_backup_domain_access_enabled()
        {
            return InterfaceT::backup_domain_is_access_enabled();
        }

        // ---------------------------------------------------------------------
        // POWER MODE / LOW POWER RUN
        // ---------------------------------------------------------------------
        [[nodiscard]] static PowerMode get_power_mode()
        {
            return InterfaceT::get_power_mode();
        }

        [[nodiscard]] static bool is_low_power_run_enabled()
        {
            return InterfaceT::is_low_power_run_enabled();
        }

        [[nodiscard]] static bool is_standby_active()
        {
            return InterfaceT::is_standby_active();
        }

        [[nodiscard]] static bool is_low_power_mode_active()
        {
            return InterfaceT::is_low_power_mode_active();
        }

        [[nodiscard]] static bool is_low_power_mode_transition_active()
        {
            return InterfaceT::is_low_power_mode_transition_active();
        }

        // ---------------------------------------------------------------------
        // INTERNAL WAKEUP
        // ---------------------------------------------------------------------
        [[nodiscard]] static bool is_internal_wakeup_enabled()
        {
            return InterfaceT::internal_wakeup_is_enabled();
        }

        [[nodiscard]] static bool is_internal_wakeup_active()
        {
            return InterfaceT::internal_wakeup_is_active();
        }

        // ---------------------------------------------------------------------
        // PUPD CONFIG
        // ---------------------------------------------------------------------
        [[nodiscard]] static bool is_pupd_config_enabled()
        {
            return InterfaceT::pupd_config_is_enabled();
        }

#if defined(SRAM2_BASE)
        // ---------------------------------------------------------------------
        // SRAM2 RETENTION
        // ---------------------------------------------------------------------
        [[nodiscard]] static bool is_sram2_retention_enabled()
        {
            return InterfaceT::sram2_retention_is_enabled();
        }
#endif

        // ---------------------------------------------------------------------
        // PVM
        // ---------------------------------------------------------------------
        [[nodiscard]] static bool is_pvm_enabled(const PowerPVMPeripheralVoltage peripheral_voltage)
        {
            return InterfaceT::pvm_is_enabled(peripheral_voltage);
        }

#if defined(PWR_SR2_PVMO1)
        [[nodiscard]] static bool is_pvm_output1_active()
        {
            return InterfaceT::pvm_output1_is_active();
        }
#endif

#if defined(PWR_SR2_PVMO2)
        [[nodiscard]] static bool is_pvm_output2_active()
        {
            return InterfaceT::pvm_output2_is_active();
        }
#endif

        [[nodiscard]] static bool is_pvm_output3_active()
        {
            return InterfaceT::pvm_output3_is_active();
        }

        [[nodiscard]] static bool is_pvm_output4_active()
        {
            return InterfaceT::pvm_output4_is_active();
        }

        // ---------------------------------------------------------------------
        // PVD
        // ---------------------------------------------------------------------
        [[nodiscard]] static PowerPVDLevel get_pvd_level()
        {
            return InterfaceT::pvd_get_level();
        }

        [[nodiscard]] static bool is_pvd_enabled()
        {
            return InterfaceT::pvd_is_enabled();
        }

        [[nodiscard]] static bool is_pvd_output_active()
        {
            return InterfaceT::pvd_output_is_active();
        }

        // ---------------------------------------------------------------------
        // WAKEUP PINS
        // ---------------------------------------------------------------------
        [[nodiscard]] static bool is_wakeup_pin_enabled(const PowerWakeupPin wakeup_pin)
        {
            return InterfaceT::wakeup_pin_is_enabled(wakeup_pin);
        }

        [[nodiscard]] static bool is_wakeup_pin_polarity_low(const PowerWakeupPin wakeup_pin)
        {
            return InterfaceT::wakeup_pin_is_polarity_low(wakeup_pin);
        }

        [[nodiscard]] static bool is_wakeup_pin_active(const PowerWakeupPin wakeup_pin)
        {
            return InterfaceT::wakeup_pin_is_active(wakeup_pin);
        }

        // ---------------------------------------------------------------------
        // BATTERY CHARGING
        // ---------------------------------------------------------------------
        [[nodiscard]] static PowerBatteryChargeResistor get_battery_charge_resistor()
        {
            return InterfaceT::battery_get_charge_resistor();
        }

        [[nodiscard]] static bool is_battery_charging_enabled()
        {
            return InterfaceT::battery_is_charging_enabled();
        }

        // ---------------------------------------------------------------------
        // GPIO PULL CONFIG
        // ---------------------------------------------------------------------
        // NOLINENEXTLINE(bugprone-easily-swappable-parameters)
        [[nodiscard]] static bool is_gpio_pull_up_enabled(const PowerGPIOPort gpio, const PowerGPIOBit gpio_bit)
        {
            return InterfaceT::gpio_pull_is_pull_up_enabled(gpio, gpio_bit);
        }

        // NOLINENEXTLINE(bugprone-easily-swappable-parameters)
        [[nodiscard]] static bool is_gpio_pull_down_enabled(const PowerGPIOPort gpio, const PowerGPIOBit gpio_bit)
        {
            return InterfaceT::gpio_pull_is_pull_down_enabled(gpio, gpio_bit);
        }

#if defined(PWR_CR3_UCPD_STDBY)
        // ---------------------------------------------------------------------
        // UCPD
        // ---------------------------------------------------------------------
        [[nodiscard]] static bool is_ucpd_standby_mode_enabled()
        {
            return InterfaceT::ucpd_is_standby_mode_enabled();
        }
#endif

#if defined(PWR_CR3_UCPD_DBDIS)
        [[nodiscard]] static bool is_ucpd_dead_battery_enabled()
        {
            return InterfaceT::ucpd_is_dead_battery_enabled();
        }
#endif

#if defined(PWR_CR2_USV)
        // ---------------------------------------------------------------------
        // SUPPLY
        // ---------------------------------------------------------------------
        [[nodiscard]] static bool is_vdd_usb_enabled()
        {
            return InterfaceT::supply_is_vdd_usb_enabled();
        }
#endif

#if defined(PWR_CR2_IOSV)
        [[nodiscard]] static bool is_vdd_io2_enabled()
        {
            return InterfaceT::supply_is_vdd_io2_enabled();
        }
#endif
    };

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
    class PowerDevice
    {
    public:
        struct Descriptor : public UniqueDeviceDescriptor
        {
        };

        using InterfaceT = PowerInterface;

        using DependDevices = TypeList<PowerRootDevice>;
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