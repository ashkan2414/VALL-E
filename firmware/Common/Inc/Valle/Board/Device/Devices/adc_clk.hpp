#pragma once

#include <variant>

#include "Valle/Board/Device/device_core.hpp"
#include "Valle/Board/Traits/adc_clk.hpp"
#include "Valle/Core/Logging/logger.hpp"
#include "stm32g4xx_hal_rcc_ex.h"

namespace valle
{
    // =============================================================================
    // FORWARD DECLARATIONS
    // =============================================================================
    class ADCRootClockDevice;

    template <ADCClockID tkClockID>
    class ADCClockDevice;

    // ============================================================================
    // ROOT ADC CLOCK DEVICE (INTERFACE DEVICE)
    // ============================================================================
    /**
     * @brief ADC Clock Device (Interface Device), represents the ADC clock family.
     *
     */
    class ADCRootClockDevice
    {
    public:
        struct Descriptor : public InterfaceDeviceDescriptor
        {
            using Children = DeviceTreeList<ADCClockDevice<ADCClockID::kADC12>, ADCClockDevice<ADCClockID::kADC345>>;
        };
    };

    // ============================================================================
    // ADC CLOCK DEVICE (SHARED DEVICE)
    // ============================================================================

    // -----------------------------------------------------------------------------
    // CONFIGURATIONS
    // -----------------------------------------------------------------------------
    struct ADCAsyncClockConfig
    {
        ADCAsyncClockSource    source    = ADCAsyncClockSource::kSysclk;
        ADCAsyncClockPrescaler prescaler = ADCAsyncClockPrescaler::kDiv1;
    };

    struct ADCSyncClockConfig
    {
        ADCSyncClockPrescaler prescaler = ADCSyncClockPrescaler::kDiv1;
    };

    using ADCClockConfig = std::variant<ADCAsyncClockConfig, ADCSyncClockConfig>;

    // -----------------------------------------------------------------------------
    // DEVICE CLASS
    // -----------------------------------------------------------------------------

    template <ADCClockID tkClockID>
    class ADCClockDevice
    {
    public:
        struct Descriptor : public SharedDeviceDescriptor
        {
        };

        static constexpr ADCClockID skClockID = tkClockID;
        using ClockTraitsT                    = ADCClockTraits<skClockID>;

        using DependDevices = TypeList<ADCRootClockDevice>;
        using InjectDevices = TypeList<>;

        // --- Initialization ---
        /**
         * @brief Initialize ADC Clock with given configuration.
         * @param config ADC Clock configuration.
         * @note The ADC has certain clock speed limitations depending on use. Refer to Table 66. ADC characteristics
         * in the datasheet.
         *
         */
        [[nodiscard]] bool init(const ADCClockConfig& config)
        {
            // TODO: Validate clock speed based on system clock and prescaler
            const bool result = std::visit(
                Overloaded{
                    [](const ADCAsyncClockConfig& async_config)
                    {
                        // Configure Asynchronous Clock (RCC Multiplexer)
                        RCC_PeriphCLKInitTypeDef clk_init{};

                        clk_init.PeriphClockSelection = ClockTraitsT::skPeripClock;

                        if constexpr (skClockID == ADCClockID::kADC12)
                        {
                            clk_init.Adc12ClockSelection = ClockTraitsT::get_async_clock_selection(async_config.source);
                        }
                        else if constexpr (skClockID == ADCClockID::kADC345)
                        {
                            clk_init.Adc345ClockSelection =
                                ClockTraitsT::get_async_clock_selection(async_config.source);
                        }
                        else
                        {
                            static_assert(false, "Unhandled ADCClockID in ADCClockDevice initialization");
                        }

                        if (HAL_RCCEx_PeriphCLKConfig(&clk_init) != HAL_OK)
                        {
                            VALLE_LOG_FATAL("ADC clock configuration failed");
                            return false;
                        }

                        LL_ADC_SetCommonClock(ClockTraitsT::skCommon, static_cast<uint32_t>(async_config.prescaler));
                        return true;
                    },
                    [](const ADCSyncClockConfig& sync_config)
                    {
                        // Configure Synchronous Clock
                        LL_ADC_SetCommonClock(ClockTraitsT::skCommon, static_cast<uint32_t>(sync_config.prescaler));
                        return true;
                    }},
                config);

            if (!result)
            {
                return result;
            }

            // Enable Clock
            LL_AHB2_GRP1_EnableClock(ClockTraitsT::skClockBusEnable);
            return true;
        }

        [[nodiscard]] bool post_init()
        {
            // Nothing to do for now
            return true;
        }
    };

    // -----------------------------------------------------------------------------
    // DEVICE ALIASES
    // -----------------------------------------------------------------------------

    using ADC12ClockDevice  = ADCClockDevice<ADCClockID::kADC12>;
    using ADC345ClockDevice = ADCClockDevice<ADCClockID::kADC345>;

}  // namespace valle