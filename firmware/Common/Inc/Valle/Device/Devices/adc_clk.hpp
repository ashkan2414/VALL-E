#pragma once

#include <variant>

#include "Valle/Core/Logging/logger.hpp"
#include "Valle/Device/Traits/adc_clk.hpp"
#include "Valle/Device/device_core.hpp"
#include "stm32g4xx_hal_rcc_ex.h"

namespace valle
{
    // =============================================================================
    // CONFIGURATIONS
    // =============================================================================
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

    // =============================================================================
    // COMPILE TIME CONFIGURATIONS
    // =============================================================================

    // =============================================================================
    // FORWARD DECLARATIONS
    // =============================================================================
    class ADCClockDomainDevice;

    template <ADCClockID tkClockID>
    class ADCClockDevice;

    // ============================================================================
    // Base ADC DEVICE (INTERFACE DEVICE)
    // ============================================================================
    /**
     * @brief ADC Clock Device (Interface Device), represents the ADC clock family.
     *
     */
    class ADCClockDomainDevice
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
    template <ADCClockID tkClockID>
    class ADCClockDevice
    {
    public:
        struct Descriptor : public SharedDeviceDescriptor
        {
        };

        static constexpr ADCClockID skClockID = tkClockID;
        using ClockTraitsT                    = ADCClockTraits<skClockID>;

        using DependDevices = TypeList<ADCClockDomainDevice>;
        using InjectDevices = TypeList<>;

        // --- Initialization ---
        /**
         * @brief Initialize ADC Clock with given configuration.
         * @param config ADC Clock configuration.
         *
         */
        [[nodiscard]] bool init(const ADCClockConfig& config)
        {
            const bool result = std::visit(
                Overloaded{
                    [](const ADCAsyncClockConfig& async_config)
                    {
                        // Configure Asynchronous Clock (RCC Multiplexer)
                        RCC_PeriphCLKInitTypeDef clk_init = {0};

                        clk_init.PeriphClockSelection = ClockTraitsT::skPeripClock;
                        clk_init.Adc12ClockSelection  = ClockTraitsT::get_async_clock_selection(async_config.source);

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

        void post_init()
        {
            // Nothing to do for now
        }
    };

    using ADC12ClockDevice  = ADCClockDevice<ADCClockID::kADC12>;
    using ADC345ClockDevice = ADCClockDevice<ADCClockID::kADC345>;

}  // namespace valle