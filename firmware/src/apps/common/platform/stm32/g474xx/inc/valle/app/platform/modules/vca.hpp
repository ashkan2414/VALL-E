#pragma once

#include <algorithm>
#include <atomic>
#include <cstdint>

#include "valle/app/modules/vca.hpp"
#include "valle/platform/drivers/hrtim/half_bridge.hpp"

namespace valle::platform::app
{

    using VCAControllerHrtimHalfBridgeInterfaceConfig = HrtimHalfBridgeDriverConfig;

    /**
     * @brief VCAControllerModule (Voice Coil Actuator) Controller Class
     *
     * @tparam THrtimTimer Hardware Device class for HRTIM control.
     * @tparam tkMode  Control mode (open-loop duty or closed-loop current).
     */
    template <typename THrtim>
    class VCAControllerHrtimHalfBridgeInterface
        : public valle::app::VCAControllerModuleHalfBridgeInterface<VCAControllerHrtimHalfBridgeInterface<THrtim>,
                                                                    VCAControllerHrtimHalfBridgeInterfaceConfig>
    {
    public:
        using ConfigT = VCAControllerHrtimHalfBridgeInterfaceConfig;

        using InjectDevices                    = TypeList<THrtim>;
        static constexpr uint8_t skDeviceIndex = THrtim::skIndex;

    private:
        HrtimHalfBridgeDriver<THrtim> m_device;  /// Hardware

    public:
        VCAControllerHrtimHalfBridgeInterface() = delete;

        /**
         * @brief Construct a new VCAControllerHrtimHalfBridgeInterface object
         *
         * @param hw Hardware device reference for the HRTIM control.
         */
        inline explicit VCAControllerHrtimHalfBridgeInterface(DeviceRef<THrtim>&& hw) : m_device(std::move(hw))
        {
        }

        // ------------------------------------------------------------------------
        // Initialization
        // ------------------------------------------------------------------------

        [[nodiscard]] inline bool init_impl(const ConfigT& config)
        {
            return m_device.init(config);
        }

        /**
         * @brief Initializes an ADC trigger for the timer. This allows the timer to trigger ADC conversions at specific
         * points in the PWM cycle.
         *
         * @tparam tkTriggerId The ID of the ADC trigger to configure (e.g., kTrig1, kTrig2, etc.). Each timer has
         * multiple trigger outputs that can be independently configured.
         * @param config The configuration for the ADC trigger, including the source event (e.g., timer period, compare
         * match, etc.) and the rollover mode (when the trigger should occur in up-down counting).
         * @return true if the ADC trigger was successfully initialized, false otherwise (e.g., invalid trigger ID,
         * incompatible configuration, etc.).
         * @return false if the trigger ID is invalid or if the configuration is incompatible with the timer's counting
         * mode. For example, certain trigger sources may not be valid in center-aligned mode, and the function should
         * validate this before applying the configuration.
         */
        template <HrtimTimerAdcTriggerId tkTriggerId>
        [[nodiscard]] inline bool init_adc_trigger(
            const HrtimTimerAdcTriggerConfig<THrtim::skTimerId, tkTriggerId>& config)
        {
            return m_device.init_adc_trigger(config);
        }

        /**
         * @brief Enables the actuator output.
         *
         */
        inline void enable_impl()
        {
            m_device.enable_output();
        }

        /**
         * @brief Disables the actuator output (sets duty to 0% and disables PWM).
         *
         */
        inline void disable_impl()
        {
            m_device.disable_output();
        }

        /**
         * @brief Sets the duty cycle of the PWM signal in normalized form (0.0 to 1.0).
         *
         * @param normalized_duty The normalized duty cycle value (0.0 to 1.0).
         */
        void set_duty_norm_impl(const float normalized_duty)
        {
            m_device.set_duty_cycle(normalized_duty);
        }
    };

    template <typename TControllerConfig>
    using VCAControllerHrtimModuleConfig =
        valle::app::VCAControllerModuleConfigX<VCAControllerHrtimHalfBridgeInterfaceConfig, TControllerConfig>;

    template <typename THrtim, typename TController>
    using VCAControllerHrtimModule =
        valle::app::VCAControllerModuleX<VCAControllerHrtimHalfBridgeInterface<THrtim>, TController>;

}  // namespace valle::platform::app