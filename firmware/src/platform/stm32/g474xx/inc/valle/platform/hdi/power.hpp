#pragma once

#include "valle/platform/core.hpp"
#include "valle/platform/hardware/power.hpp"

namespace valle::platform
{
    template <typename T = void>
    struct PowerHdi : public PowerInterface<T>
    {
        struct Descriptor : public UniqueDeviceDescriptor
        {
        };
    };

    // =============================================================================
    // POWER INFO HDI
    // =============================================================================

    /**
     * @brief Shared read-only info device for the PWR peripheral.
     *
     * This device provides non-mutating access to PWR state so multiple parts of
     * the system can observe power configuration and status without owning the
     * mutable Power.
     */
    template <typename T = void>
    class PowerInfoHdi
    {
    public:
        struct Descriptor : public SharedDeviceDescriptor
        {
            constexpr static bool skNeedsInit = false;
        };

        using InterfaceT    = PowerInterface;
        using InjectDevices = TypeList<>;

    private:
        InterfaceT m_interface{};

    public:
        // ---------------------------------------------------------------------
        // REGULATION
        // ---------------------------------------------------------------------
        [[nodiscard]] PowerVoltageRange get_voltage_range() const
        {
            return m_interface.get_regulation_voltage_scaling();
        }

        [[nodiscard]] PowerVoltageRangeMode get_voltage_range_mode() const
        {
            return m_interface.get_voltage_range_mode();
        }

        [[nodiscard]] bool is_voltage_scaling_transition_done() const
        {
            return m_interface.voltage_scaling_transition_done();
        }

#ifdef VALLE_PWR_HAS_RANGE1_BOOST
        [[nodiscard]] bool is_range1_boost_enabled() const
        {
            return m_interface.is_range1_boost_enabled();
        }
#endif

        // ---------------------------------------------------------------------
        // BACKUP DOMAIN
        // ---------------------------------------------------------------------
        [[nodiscard]] bool is_backup_domain_access_enabled() const
        {
            return m_interface.backup_domain_is_access_enabled();
        }

        // ---------------------------------------------------------------------
        // POWER MODE / LOW POWER RUN
        // ---------------------------------------------------------------------
        [[nodiscard]] PowerMode get_power_mode() const
        {
            return m_interface.get_power_mode();
        }

        [[nodiscard]] bool is_low_power_run_enabled() const
        {
            return m_interface.is_low_power_run_enabled();
        }

        [[nodiscard]] bool is_standby_active() const
        {
            return m_interface.is_standby_active();
        }

        [[nodiscard]] bool is_low_power_mode_active() const
        {
            return m_interface.is_low_power_mode_active();
        }

        [[nodiscard]] bool is_low_power_mode_transition_active() const
        {
            return m_interface.is_low_power_mode_transition_active();
        }

        // ---------------------------------------------------------------------
        // INTERNAL WAKEUP
        // ---------------------------------------------------------------------
        [[nodiscard]] bool is_internal_wakeup_enabled() const
        {
            return m_interface.internal_wakeup_is_enabled();
        }

        [[nodiscard]] bool is_internal_wakeup_active() const
        {
            return m_interface.internal_wakeup_is_active();
        }

        // ---------------------------------------------------------------------
        // PUPD CONFIG
        // ---------------------------------------------------------------------
        [[nodiscard]] bool is_pupd_config_enabled() const
        {
            return m_interface.pupd_config_is_enabled();
        }

#ifdef VALLE_PWR_HAS_SRAM2_RETENTION
        // ---------------------------------------------------------------------
        // SRAM2 RETENTION
        // ---------------------------------------------------------------------
        [[nodiscard]] bool is_sram2_retention_enabled() const
        {
            return m_interface.sram2_retention_is_enabled();
        }
#endif

        // ---------------------------------------------------------------------
        // PVM
        // ---------------------------------------------------------------------
        [[nodiscard]] bool is_pvm_enabled(const PowerPVMPeripheralVoltage peripheral_voltage) const
        {
            return m_interface.pvm_is_enabled(peripheral_voltage);
        }

#ifdef VALLE_PWR_HAS_PVM_OUTPUT1
        [[nodiscard]] bool is_pvm_output1_active() const
        {
            return m_interface.pvm_output1_is_active();
        }
#endif

#ifdef VALLE_PWR_HAS_PVM_OUTPUT2
        [[nodiscard]] bool is_pvm_output2_active() const
        {
            return m_interface.pvm_output2_is_active();
        }
#endif

        [[nodiscard]] bool is_pvm_output3_active() const
        {
            return m_interface.pvm_output3_is_active();
        }

        [[nodiscard]] bool is_pvm_output4_active() const
        {
            return m_interface.pvm_output4_is_active();
        }

        // ---------------------------------------------------------------------
        // PVD
        // ---------------------------------------------------------------------
        [[nodiscard]] PowerPVDLevel get_pvd_level() const
        {
            return m_interface.pvd_get_level();
        }

        [[nodiscard]] bool is_pvd_enabled() const
        {
            return m_interface.pvd_is_enabled();
        }

        [[nodiscard]] bool is_pvd_output_active() const
        {
            return m_interface.pvd_output_is_active();
        }

        // ---------------------------------------------------------------------
        // WAKEUP PINS
        // ---------------------------------------------------------------------
        [[nodiscard]] bool is_wakeup_pin_enabled(const PowerWakeupPin wakeup_pin) const
        {
            return m_interface.wakeup_pin_is_enabled(wakeup_pin);
        }

        [[nodiscard]] bool is_wakeup_pin_polarity_low(const PowerWakeupPin wakeup_pin) const
        {
            return m_interface.wakeup_pin_is_polarity_low(wakeup_pin);
        }

        [[nodiscard]] bool is_wakeup_pin_active(const PowerWakeupPin wakeup_pin) const
        {
            return m_interface.wakeup_pin_is_active(wakeup_pin);
        }

        // ---------------------------------------------------------------------
        // BATTERY CHARGING
        // ---------------------------------------------------------------------
        [[nodiscard]] PowerBatteryChargeResistor get_battery_charge_resistor() const
        {
            return m_interface.battery_get_charge_resistor();
        }

        [[nodiscard]] bool is_battery_charging_enabled() const
        {
            return m_interface.battery_is_charging_enabled();
        }

        // ---------------------------------------------------------------------
        // GPIO PULL CONFIG
        // ---------------------------------------------------------------------
        // NOLINENEXTLINE(bugprone-easily-swappable-parameters)
        [[nodiscard]] bool is_gpio_pull_up_enabled(const PowerGpioPort gpio, const PowerGpioBit gpio_bit) const
        {
            return m_interface.gpio_pull_is_pull_up_enabled(gpio, gpio_bit);
        }

        // NOLINENEXTLINE(bugprone-easily-swappable-parameters)
        [[nodiscard]] bool is_gpio_pull_down_enabled(const PowerGpioPort gpio, const PowerGpioBit gpio_bit) const
        {
            return m_interface.gpio_pull_is_pull_down_enabled(gpio, gpio_bit);
        }

#ifdef VALLE_PWR_HAS_UCPD_STANDBY
        // ---------------------------------------------------------------------
        // UCPD
        // ---------------------------------------------------------------------
        [[nodiscard]] bool is_ucpd_standby_mode_enabled() const
        {
            return m_interface.ucpd_is_standby_mode_enabled();
        }
#endif

#ifdef VALLE_PWR_HAS_UCPD_DEAD_BATTERY
        [[nodiscard]] bool is_ucpd_dead_battery_enabled() const
        {
            return m_interface.ucpd_is_dead_battery_enabled();
        }
#endif

#ifdef VALLE_PWR_HAS_VDDUSB_SUPPLY
        // ---------------------------------------------------------------------
        // SUPPLY
        // ---------------------------------------------------------------------
        [[nodiscard]] bool is_vdd_usb_enabled() const
        {
            return m_interface.supply_is_vdd_usb_enabled();
        }
#endif

#ifdef VALLE_PWR_HAS_VDDIO2_SUPPLY
        [[nodiscard]] bool is_vdd_io2_enabled() const
        {
            return m_interface.supply_is_vdd_io2_enabled();
        }
#endif
    };
}  // namespace valle::platform