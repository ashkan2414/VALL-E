#pragma once

#include <cstdint>

#include "stm32g4xx_ll_pwr.h"

#ifdef PWR_CR5_R1MODE
#define VALLE_PWR_HAS_RANGE1_BOOST 1
#endif

#ifdef PWR_CR3_UCPD_STDBY
#define VALLE_PWR_HAS_UCPD_STANDBY 1
#endif

#ifdef PWR_CR3_UCPD_DBDIS
#define VALLE_PWR_HAS_UCPD_DEAD_BATTERY 1
#endif

#ifdef PWR_CR2_USV
#define VALLE_PWR_HAS_VDDUSB_SUPPLY 1
#endif

#ifdef PWR_CR2_IOSV
#define VALLE_PWR_HAS_VDDIO2_SUPPLY 1
#endif

#ifdef PWR_SR2_PVMO1
#define VALLE_PWR_HAS_PVM_OUTPUT1 1
#endif

#ifdef PWR_SR2_PVMO2
#define VALLE_PWR_HAS_PVM_OUTPUT2 1
#endif

#ifdef SRAM2_BASE
#define VALLE_PWR_HAS_SRAM2_RETENTION 1
#endif

namespace valle
{
    // =============================================================================
    // ENUMERATIONS
    // =============================================================================
    /**
     * @brief Core regulator voltage scaling range.
     *
     * These modes constrain the maximum safe system/PLL frequencies.
     */
    enum class PowerVoltageRange : uint32_t
    {
        kRange1 = LL_PWR_REGU_VOLTAGE_SCALE1,
        kRange2 = LL_PWR_REGU_VOLTAGE_SCALE2
    };

    enum class PowerVoltageRangeMode : uint32_t
    {
        kRange1Normal,
        kRange1Boost,
        kRange2,
    };

    enum class PowerMode : uint32_t
    {
        kStop0    = LL_PWR_MODE_STOP0,
        kStop1    = LL_PWR_MODE_STOP1,
        kStandby  = LL_PWR_MODE_STANDBY,
        kShutdown = LL_PWR_MODE_SHUTDOWN,
    };

    enum class PowerPVMPeripheralVoltage : uint32_t
    {
        kVDDAComp     = LL_PWR_PVM_VDDA_COMP,
        kVDDAFastDAC  = LL_PWR_PVM_VDDA_FASTDAC,
        kVDDAADC      = LL_PWR_PVM_VDDA_ADC,
        kVDDAOpAmpDAC = LL_PWR_PVM_VDDA_OPAMP_DAC,
    };

    enum class PowerPVDLevel : uint32_t
    {
        kLevel0 = LL_PWR_PVDLEVEL_0,
        kLevel1 = LL_PWR_PVDLEVEL_1,
        kLevel2 = LL_PWR_PVDLEVEL_2,
        kLevel3 = LL_PWR_PVDLEVEL_3,
        kLevel4 = LL_PWR_PVDLEVEL_4,
        kLevel5 = LL_PWR_PVDLEVEL_5,
        kLevel6 = LL_PWR_PVDLEVEL_6,
        kLevel7 = LL_PWR_PVDLEVEL_7
    };

    enum class PowerBatteryChargeResistor : uint32_t
    {
        kResistor5K   = LL_PWR_BATT_CHARG_RESISTOR_5K,
        kResistor1P5K = LL_PWR_BATT_CHARGRESISTOR_1_5K
    };

    enum class PowerWakeupPin : uint32_t
    {
        kPin1 = LL_PWR_WAKEUP_PIN1,
        kPin2 = LL_PWR_WAKEUP_PIN2,
        kPin3 = LL_PWR_WAKEUP_PIN3,
        kPin4 = LL_PWR_WAKEUP_PIN4,
        kPin5 = LL_PWR_WAKEUP_PIN5
    };

    enum class PowerGPIOPort : uint32_t
    {
        kPortA = 0,
        kPortB,
        kPortC,
        kPortD,
        kPortE,
        kPortF,
        kPortG
    };

    enum class PowerGPIOBit : uint32_t
    {
        kBit0  = LL_PWR_GPIO_BIT_0,
        kBit1  = LL_PWR_GPIO_BIT_1,
        kBit2  = LL_PWR_GPIO_BIT_2,
        kBit3  = LL_PWR_GPIO_BIT_3,
        kBit4  = LL_PWR_GPIO_BIT_4,
        kBit5  = LL_PWR_GPIO_BIT_5,
        kBit6  = LL_PWR_GPIO_BIT_6,
        kBit7  = LL_PWR_GPIO_BIT_7,
        kBit8  = LL_PWR_GPIO_BIT_8,
        kBit9  = LL_PWR_GPIO_BIT_9,
        kBit10 = LL_PWR_GPIO_BIT_10,
        kBit11 = LL_PWR_GPIO_BIT_11,
        kBit12 = LL_PWR_GPIO_BIT_12,
        kBit13 = LL_PWR_GPIO_BIT_13,
        kBit14 = LL_PWR_GPIO_BIT_14,
        kBit15 = LL_PWR_GPIO_BIT_15,
    };

    // =============================================================================
    // INTERFACE
    // =============================================================================
    /**
     * @brief Interface for the STM32G4 PWR voltage scaling controls.
     */
    struct PowerInterface
    {
        static constexpr uint32_t skDefaultVoltageScalingTimeoutCount = 1'000'000U;

        static constexpr uint32_t skLLGPIOPortIDMap[] = {
            LL_PWR_GPIO_A, LL_PWR_GPIO_B, LL_PWR_GPIO_C, LL_PWR_GPIO_D, LL_PWR_GPIO_E, LL_PWR_GPIO_F, LL_PWR_GPIO_G};

        // =========================================================================
        // LOW POWER RUN
        // =========================================================================
        static void low_power_run_enable()
        {
            LL_PWR_EnableLowPowerRunMode();
        }

        static void low_power_run_disable()
        {
            LL_PWR_DisableLowPowerRunMode();
        }

        [[nodiscard]] static bool is_low_power_run_enabled()
        {
            return LL_PWR_IsEnabledLowPowerRunMode() == 1U;
        }

        static void low_power_run_enter()
        {
            LL_PWR_EnterLowPowerRunMode();
        }

        static void low_power_run_exit()
        {
            LL_PWR_ExitLowPowerRunMode();
        }

        // =========================================================================
        // VOLTAGE SCALING
        // =========================================================================
        static void set_regulation_voltage_scaling(const PowerVoltageRange voltage_scaling)
        {
            LL_PWR_SetRegulVoltageScaling(static_cast<uint32_t>(voltage_scaling));
        }

        [[nodiscard]] static PowerVoltageRange get_regulation_voltage_scaling()
        {
            return static_cast<PowerVoltageRange>(LL_PWR_GetRegulVoltageScaling());
        }

        [[nodiscard]] static constexpr PowerVoltageRangeMode get_voltage_range_mode(
            const PowerVoltageRange voltage_range, const bool range1_boost_enabled = false)
        {
            if (voltage_range == PowerVoltageRange::kRange2)
            {
                return PowerVoltageRangeMode::kRange2;
            }

#ifdef VALLE_PWR_HAS_RANGE1_BOOST
            // For Range 1, the voltage range mode depends on whether Range 1 Boost mode is enabled.
            if (range1_boost_enabled)
            {
                return PowerVoltageRangeMode::kRange1Boost;
            }
#endif

            return PowerVoltageRangeMode::kRange1Normal;
        }

        [[nodiscard]] static PowerVoltageRangeMode get_voltage_range_mode()
        {
#ifdef VALLE_PWR_HAS_RANGE1_BOOST
            const bool range1_boost = is_range1_boost_enabled();
#else
            const bool range1_boost = false;
#endif

            return get_voltage_range_mode(get_regulation_voltage_scaling(), range1_boost);
        }

#ifdef VALLE_PWR_HAS_RANGE1_BOOST
        static void enable_range1_boost()
        {
            LL_PWR_EnableRange1BoostMode();
        }

        static void disable_range1_boost()
        {
            LL_PWR_DisableRange1BoostMode();
        }

        [[nodiscard]] static bool is_range1_boost_enabled()
        {
            return LL_PWR_IsEnabledRange1BoostMode() == 1U;
        }
#endif

        [[nodiscard]] static bool voltage_scaling_transition_done()
        {
            return LL_PWR_IsActiveFlag_VOS() == 0U;
        }

        // =========================================================================
        // BACKUP DOMAIN ACCESS
        // =========================================================================
        static void backup_domain_enable_access()
        {
            LL_PWR_EnableBkUpAccess();
        }

        static void backup_domain_disable_access()
        {
            LL_PWR_DisableBkUpAccess();
        }

        [[nodiscard]] static bool backup_domain_is_access_enabled()
        {
            return LL_PWR_IsEnabledBkUpAccess() == 1U;
        }

        // =========================================================================
        // POWER MODE
        // =========================================================================
        static void set_power_mode(const PowerMode power_mode)
        {
            LL_PWR_SetPowerMode(static_cast<uint32_t>(power_mode));
        }

        [[nodiscard]] static PowerMode get_power_mode()
        {
            return static_cast<PowerMode>(LL_PWR_GetPowerMode());
        }

        static void clear_standby()
        {
            LL_PWR_ClearFlag_SB();
        }

        [[nodiscard]] static bool is_standby_active()
        {
            return LL_PWR_IsActiveFlag_SB() == 1U;
        }

        [[nodiscard]] static bool is_low_power_mode_active()
        {
            return LL_PWR_IsActiveFlag_REGLPF() == 1U;
        }

        [[nodiscard]] static bool is_low_power_mode_transition_active()
        {
            return LL_PWR_IsActiveFlag_REGLPS() == 1U;
        }

        // =========================================================================
        // UCPD
        // =========================================================================
#ifdef VALLE_PWR_HAS_UCPD_STANDBY
        static void ucpd_enable_standby_mode()
        {
            LL_PWR_EnableUCPDStandbyMode();
        }

        static void ucpd_disable_standby_mode()
        {
            LL_PWR_DisableUCPDStandbyMode();
        }

        [[nodiscard]] static bool ucpd_is_standby_mode_enabled()
        {
            return LL_PWR_IsEnabledUCPDStandbyMode() == 1U;
        }
#endif

#ifdef VALLE_PWR_HAS_UCPD_DEAD_BATTERY
        static void ucpd_enable_dead_battery()
        {
            LL_PWR_EnableUCPDDeadBattery();
        }

        static void ucpd_disable_dead_battery()
        {
            LL_PWR_DisableUCPDDeadBattery();
        }

        [[nodiscard]] static bool ucpd_is_dead_battery_enabled()
        {
            return LL_PWR_IsEnabledUCPDDeadBattery() == 1U;
        }
#endif

        // =========================================================================
        // SUPPLY MONITORS / SPECIAL SUPPLIES
        // =========================================================================
#ifdef VALLE_PWR_HAS_VDDUSB_SUPPLY
        static void supply_enable_vdd_usb()
        {
            LL_PWR_EnableVddUSB();
        }

        static void supply_disable_vdd_usb()
        {
            LL_PWR_DisableVddUSB();
        }

        [[nodiscard]] static bool supply_is_vdd_usb_enabled()
        {
            return LL_PWR_IsEnabledVddUSB() == 1U;
        }
#endif

#ifdef VALLE_PWR_HAS_VDDIO2_SUPPLY
        static void supply_enable_vdd_io2()
        {
            LL_PWR_EnableVddIO2();
        }

        static void supply_disable_vdd_io2()
        {
            LL_PWR_DisableVddIO2();
        }

        [[nodiscard]] static bool supply_is_vdd_io2_enabled()
        {
            return LL_PWR_IsEnabledVddIO2() == 1U;
        }
#endif

        // =========================================================================
        // PERIPHERAL VOLTAGE MONITOR (PVM)
        // =========================================================================
        static void pvm_enable(const PowerPVMPeripheralVoltage peripheral_voltage)
        {
            LL_PWR_EnablePVM(static_cast<uint32_t>(peripheral_voltage));
        }

        static void pvm_disable(const PowerPVMPeripheralVoltage peripheral_voltage)
        {
            LL_PWR_DisablePVM(static_cast<uint32_t>(peripheral_voltage));
        }

        [[nodiscard]] static bool pvm_is_enabled(const PowerPVMPeripheralVoltage peripheral_voltage)
        {
            return LL_PWR_IsEnabledPVM(static_cast<uint32_t>(peripheral_voltage)) == 1U;
        }

#ifdef VALLE_PWR_HAS_PVM_OUTPUT1
        [[nodiscard]] static bool pvm_output1_is_active()
        {
            return LL_PWR_IsActiveFlag_PVMO1() == 1U;
        }
#endif

#ifdef VALLE_PWR_HAS_PVM_OUTPUT2
        [[nodiscard]] static bool pvm_output2_is_active()
        {
            return LL_PWR_IsActiveFlag_PVMO2() == 1U;
        }
#endif

        [[nodiscard]] static bool pvm_output3_is_active()
        {
            return LL_PWR_IsActiveFlag_PVMO3() == 1U;
        }

        [[nodiscard]] static bool pvm_output4_is_active()
        {
            return LL_PWR_IsActiveFlag_PVMO4() == 1U;
        }

        // =========================================================================
        // PERIPHERAL VOLTAGE DETECTOR (PVD)
        // =========================================================================
        static void pvd_set_level(const PowerPVDLevel pvd_level)
        {
            LL_PWR_SetPVDLevel(static_cast<uint32_t>(pvd_level));
        }

        [[nodiscard]] static PowerPVDLevel pvd_get_level()
        {
            return static_cast<PowerPVDLevel>(LL_PWR_GetPVDLevel());
        }

        static void pvd_enable()
        {
            LL_PWR_EnablePVD();
        }

        static void pvd_disable()
        {
            LL_PWR_DisablePVD();
        }

        [[nodiscard]] static bool pvd_is_enabled()
        {
            return LL_PWR_IsEnabledPVD() == 1U;
        }

        [[nodiscard]] static bool pvd_output_is_active()
        {
            return LL_PWR_IsActiveFlag_PVDO() == 1U;
        }

        // =========================================================================
        // INTERNAL WAKEUP
        // =========================================================================
        static void internal_wakeup_enable()
        {
            LL_PWR_EnableInternWU();
        }

        static void internal_wakeup_disable()
        {
            LL_PWR_DisableInternWU();
        }

        [[nodiscard]] static bool internal_wakeup_is_enabled()
        {
            return LL_PWR_IsEnabledInternWU() == 1U;
        }

        [[nodiscard]] static bool internal_wakeup_is_active()
        {
            return LL_PWR_IsActiveFlag_InternWU() == 1U;
        }

        // =========================================================================
        // PULL-UP / PULL-DOWN CONFIG IN STANDBY / SHUTDOWN
        // =========================================================================

        static void pupd_config_enable()
        {
            LL_PWR_EnablePUPDCfg();
        }

        static void pupd_config_disable()
        {
            LL_PWR_DisablePUPDCfg();
        }

        [[nodiscard]] static bool pupd_config_is_enabled()
        {
            return LL_PWR_IsEnabledPUPDCfg() == 1U;
        }

        // =========================================================================
        // SRAM RETENTION
        // =========================================================================
#ifdef VALLE_PWR_HAS_SRAM2_RETENTION
        static void sram2_retention_enable()
        {
            LL_PWR_EnableSRAM2Retention();
        }

        static void sram2_retention_disable()
        {
            LL_PWR_DisableSRAM2Retention();
        }

        [[nodiscard]] static bool sram2_retention_is_enabled()
        {
            return LL_PWR_IsEnabledSRAM2Retention() == 1U;
        }
#endif

        // =========================================================================
        // WAKEUP PINS
        // =========================================================================
        static void wakeup_pin_enable(const PowerWakeupPin wakeup_pin)
        {
            LL_PWR_EnableWakeUpPin(static_cast<uint32_t>(wakeup_pin));
        }

        static void wakeup_pin_disable(const PowerWakeupPin wakeup_pin)
        {
            LL_PWR_DisableWakeUpPin(static_cast<uint32_t>(wakeup_pin));
        }

        [[nodiscard]] static bool wakeup_pin_is_enabled(const PowerWakeupPin wakeup_pin)
        {
            return LL_PWR_IsEnabledWakeUpPin(static_cast<uint32_t>(wakeup_pin)) == 1U;
        }

        static void wakeup_pin_set_polarity_low(const PowerWakeupPin wakeup_pin)
        {
            LL_PWR_SetWakeUpPinPolarityLow(static_cast<uint32_t>(wakeup_pin));
        }

        static void wakeup_pin_set_polarity_high(const PowerWakeupPin wakeup_pin)
        {
            LL_PWR_SetWakeUpPinPolarityHigh(static_cast<uint32_t>(wakeup_pin));
        }

        [[nodiscard]] static bool wakeup_pin_is_polarity_low(const PowerWakeupPin wakeup_pin)
        {
            return LL_PWR_IsWakeUpPinPolarityLow(static_cast<uint32_t>(wakeup_pin)) == 1U;
        }

        template <PowerWakeupPin wakeup_pin>
        [[nodiscard]] static bool wakeup_pin_is_active()
        {
            if constexpr (wakeup_pin == PowerWakeupPin::kPin1)
            {
                return LL_PWR_IsActiveFlag_WU1() == 1U;
            }
            else if constexpr (wakeup_pin == PowerWakeupPin::kPin2)
            {
                return LL_PWR_IsActiveFlag_WU2() == 1U;
            }
            else if constexpr (wakeup_pin == PowerWakeupPin::kPin3)
            {
                return LL_PWR_IsActiveFlag_WU3() == 1U;
            }
            else if constexpr (wakeup_pin == PowerWakeupPin::kPin4)
            {
                return LL_PWR_IsActiveFlag_WU4() == 1U;
            }
            else if constexpr (wakeup_pin == PowerWakeupPin::kPin5)
            {
                return LL_PWR_IsActiveFlag_WU5() == 1U;
            }
            else
            {
                static_assert(false, "Invalid wakeup pin");
            }
        }

        [[nodiscard]] static bool wakeup_pin_is_active(const PowerWakeupPin wakeup_pin)
        {
            switch (wakeup_pin)
            {
                case PowerWakeupPin::kPin1:
                    return wakeup_pin_is_active<PowerWakeupPin::kPin1>();

                case PowerWakeupPin::kPin2:
                    return wakeup_pin_is_active<PowerWakeupPin::kPin2>();

                case PowerWakeupPin::kPin3:
                    return wakeup_pin_is_active<PowerWakeupPin::kPin3>();

                case PowerWakeupPin::kPin4:
                    return wakeup_pin_is_active<PowerWakeupPin::kPin4>();

                case PowerWakeupPin::kPin5:
                    return wakeup_pin_is_active<PowerWakeupPin::kPin5>();
                default:
                    return false;
            }
        }

        static void clear_wakeup_all()
        {
            LL_PWR_ClearFlag_WU();
        }

        template <PowerWakeupPin wakeup_pin>
        static void clear_wakeup()
        {
            if constexpr (wakeup_pin == PowerWakeupPin::kPin1)
            {
                LL_PWR_ClearFlag_WU1();
            }
            else if constexpr (wakeup_pin == PowerWakeupPin::kPin2)
            {
                LL_PWR_ClearFlag_WU2();
            }
            else if constexpr (wakeup_pin == PowerWakeupPin::kPin3)
            {
                LL_PWR_ClearFlag_WU3();
            }
            else if constexpr (wakeup_pin == PowerWakeupPin::kPin4)
            {
                LL_PWR_ClearFlag_WU4();
            }
            else if constexpr (wakeup_pin == PowerWakeupPin::kPin5)
            {
                LL_PWR_ClearFlag_WU5();
            }
            else
            {
                static_assert(false, "Invalid wakeup pin");
            }
        }

        static void clear_wakeup(const PowerWakeupPin wakeup_pin)
        {
            switch (wakeup_pin)
            {
                case PowerWakeupPin::kPin1:
                    clear_wakeup<PowerWakeupPin::kPin1>();
                    break;

                case PowerWakeupPin::kPin2:
                    clear_wakeup<PowerWakeupPin::kPin2>();
                    break;

                case PowerWakeupPin::kPin3:
                    clear_wakeup<PowerWakeupPin::kPin3>();
                    break;

                case PowerWakeupPin::kPin4:
                    clear_wakeup<PowerWakeupPin::kPin4>();
                    break;

                case PowerWakeupPin::kPin5:
                    clear_wakeup<PowerWakeupPin::kPin5>();
                    break;
                default:
                    break;
            }
        }

        // =========================================================================
        // BATTERY CHARGING
        // =========================================================================
        static void battery_set_charge_resistor(const PowerBatteryChargeResistor resistor)
        {
            LL_PWR_SetBattChargResistor(static_cast<uint32_t>(resistor));
        }

        [[nodiscard]] static PowerBatteryChargeResistor battery_get_charge_resistor()
        {
            return static_cast<PowerBatteryChargeResistor>(LL_PWR_GetBattChargResistor());
        }

        static void battery_enable_charging()
        {
            LL_PWR_EnableBatteryCharging();
        }

        static void battery_disable_charging()
        {
            LL_PWR_DisableBatteryCharging();
        }

        [[nodiscard]] static bool battery_is_charging_enabled()
        {
            return LL_PWR_IsEnabledBatteryCharging() == 1U;
        }

        // =========================================================================
        // GPIO PULL CONFIG IN STANDBY / SHUTDOWN
        // =========================================================================
        static void gpio_pull_enable_pull_up(const PowerGPIOPort gpio, const PowerGPIOBit gpio_number)
        {
            LL_PWR_EnableGPIOPullUp(skLLGPIOPortIDMap[static_cast<uint32_t>(gpio)], static_cast<uint32_t>(gpio_number));
        }

        static void gpio_pull_disable_pull_up(const PowerGPIOPort gpio, const PowerGPIOBit gpio_number)
        {
            LL_PWR_DisableGPIOPullUp(skLLGPIOPortIDMap[static_cast<uint32_t>(gpio)],
                                     static_cast<uint32_t>(gpio_number));
        }

        [[nodiscard]] static bool gpio_pull_is_pull_up_enabled(const PowerGPIOPort gpio, const PowerGPIOBit gpio_number)
        {
            return LL_PWR_IsEnabledGPIOPullUp(skLLGPIOPortIDMap[static_cast<uint32_t>(gpio)],
                                              static_cast<uint32_t>(gpio_number)) == 1U;
        }

        static void gpio_pull_enable_pull_down(const PowerGPIOPort gpio, const PowerGPIOBit gpio_number)
        {
            LL_PWR_EnableGPIOPullDown(skLLGPIOPortIDMap[static_cast<uint32_t>(gpio)],
                                      static_cast<uint32_t>(gpio_number));
        }

        static void gpio_pull_disable_pull_down(const PowerGPIOPort gpio, const PowerGPIOBit gpio_number)
        {
            LL_PWR_DisableGPIOPullDown(skLLGPIOPortIDMap[static_cast<uint32_t>(gpio)],
                                       static_cast<uint32_t>(gpio_number));
        }

        [[nodiscard]] static bool gpio_pull_is_pull_down_enabled(const PowerGPIOPort gpio,
                                                                 const PowerGPIOBit  gpio_number)
        {
            return LL_PWR_IsEnabledGPIOPullDown(skLLGPIOPortIDMap[static_cast<uint32_t>(gpio)],
                                                static_cast<uint32_t>(gpio_number)) == 1U;
        }
    };

}  // namespace valle
