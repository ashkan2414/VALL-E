#pragma once

#include <variant>

#include "stm32g4xx_hal_rcc_ex.h"
#include "valle/core.hpp"
#include "valle/core/device/device.hpp"
#include "valle/platform/hardware/adc_clk.hpp"

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
         */
        [[nodiscard]] bool init(const ADCClockConfig& config)
        {
            // Configure the RCC clock multiplexer (must be done before enabling the peripheral clock)
            const bool rcc_ok =
                std::visit(Overloaded{[](const ADCAsyncClockConfig& async_config)
                                      {
                                          RCC_PeriphCLKInitTypeDef clk_init{};
                                          clk_init.PeriphClockSelection = ClockTraitsT::skPeripClock;

                                          if constexpr (skClockID == ADCClockID::kADC12)
                                          {
                                              clk_init.Adc12ClockSelection =
                                                  ClockTraitsT::get_async_clock_selection(async_config.source);
                                          }
                                          else if constexpr (skClockID == ADCClockID::kADC345)
                                          {
                                              clk_init.Adc345ClockSelection =
                                                  ClockTraitsT::get_async_clock_selection(async_config.source);
                                          }

                                          return HAL_RCCEx_PeriphCLKConfig(&clk_init) == HAL_OK;
                                      },
                                      [](const ADCSyncClockConfig& sync_config)
                                      {
                                          return true;  // No RCC multiplexer config needed for synchronous clock
                                      }},
                           config);

            if (!rcc_ok)
            {
                VALLE_LOG_FATAL("ADC clock configuration failed");
                return false;
            }

            // Enable the ADC peripheral clock (REQUIRED before writing to any ADC registers)
            ClockTraitsT::enable_clock();

            // Now we can safely configure the ADC Common Control Register (ADC_CCR)
            std::visit(
                Overloaded{
                    [](const ADCAsyncClockConfig& async_config)
                    { LL_ADC_SetCommonClock(ClockTraitsT::skCommon, static_cast<uint32_t>(async_config.prescaler)); },
                    [](const ADCSyncClockConfig& sync_config)
                    { LL_ADC_SetCommonClock(ClockTraitsT::skCommon, static_cast<uint32_t>(sync_config.prescaler)); }},
                config);

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