#pragma once

#include <optional>
#include <string_view>
#include <variant>

#include "valle/platform/core.hpp"
#include "valle/platform/devices/rcc.hpp"
#include "valle/platform/hardware/adc_common.hpp"

namespace valle::platform
{
    // =============================================================================
    // FORWARD DECLARATIONS
    // =============================================================================
    class ADCCommonRootDevice;

    template <ADCCommonID tkCommonID>
    class ADCCommonDevice;

    // ============================================================================
    // ROOT ADC CLOCK DEVICE (INTERFACE DEVICE)
    // ============================================================================
    /**
     * @brief ADC Clock Device (Interface Device), represents the ADC clock family.
     *
     */
    class ADCCommonRootDevice
    {
    public:
        struct Descriptor : public InterfaceDeviceDescriptor
        {
            using Children =
                DeviceTreeList<ADCCommonDevice<ADCCommonID::kADC12>, ADCCommonDevice<ADCCommonID::kADC345>>;
        };
    };

    // ============================================================================
    // ADC CLOCK DEVICE (SHARED DEVICE)
    // ============================================================================

    // -----------------------------------------------------------------------------
    // CONFIGURATIONS
    // -----------------------------------------------------------------------------
    enum class ADCCommonClockSource
    {
        kAsyncPLLP,
        kAsyncSysclk,
        kSync
    };

    struct ADCCommonAsyncClockConfig
    {
        ADCCommonAsyncClockPrescaler prescaler = ADCCommonAsyncClockPrescaler::kDiv1;
    };

    struct ADCCommonSyncClockConfig
    {
        ADCCommonSyncClockPrescaler prescaler = ADCCommonSyncClockPrescaler::kDiv1;
    };

    template <ADCCommonID tkCommonID>
    struct ADCCommonConfig
    {
        std::variant<ADCCommonAsyncClockConfig, ADCCommonSyncClockConfig> clock_config;

        [[nodiscard]] constexpr std::optional<std::string_view> validate(const RCCConfig& rcc_config) const
        {
            const auto clock_source   = get_clock_source(rcc_config.sct);
            const auto source_freq_hz = get_source_clock_freq_hz(rcc_config);
            return validate(clock_source, source_freq_hz);
        }

        [[nodiscard]] constexpr std::optional<std::string_view> validate(const ADCCommonClockSource clock_source,
                                                                         const uint32_t source_freq_hz) const
        {
            return std::visit(
                Overloaded{[clock_source, source_freq_hz](
                               const ADCCommonAsyncClockConfig& async_config) -> std::optional<std::string_view>
                           {
                               if (clock_source != ADCCommonClockSource::kAsyncPLLP &&
                                   clock_source != ADCCommonClockSource::kAsyncSysclk)
                               {
                                   return "ADC clock source must be asynchronous for ADCCommonAsyncClockConfig";
                               }

                               const auto target_freq_hz = ADCCommonRootInterface::calculate_async_clock_freq_hz(
                                   source_freq_hz, async_config.prescaler);

                               if (target_freq_hz > ADCCommonRootInterface::skMaxClockFreqHz)
                               {
                                   return "ADC clock frequency exceeds maximum allowed frequency";
                               }

                               return std::nullopt;
                           },
                           [clock_source, source_freq_hz](
                               const ADCCommonSyncClockConfig& sync_config) -> std::optional<std::string_view>
                           {
                               if (clock_source != ADCCommonClockSource::kSync)
                               {
                                   return "ADC clock source must be synchronous for ADCCommonSyncClockConfig";
                               }

                               const auto target_freq_hz = ADCCommonRootInterface::calculate_sync_clock_freq_hz(
                                   source_freq_hz, sync_config.prescaler);

                               if (target_freq_hz > ADCCommonRootInterface::skMaxClockFreqHz)
                               {
                                   return "ADC clock frequency exceeds maximum allowed frequency";
                               }

                               return std::nullopt;
                           }},
                clock_config);
        }

        [[nodiscard]] static constexpr ADCCommonClockSource get_clock_source(const SCTConfig& sct_config)
        {
            if constexpr (tkCommonID == ADCCommonID::kADC12)
            {
                switch (sct_config.adc12_source)
                {
                    case SCTADC12ClockSource::kSysclk:
                        return ADCCommonClockSource::kAsyncSysclk;
                    case SCTADC12ClockSource::kPLLP:
                        return ADCCommonClockSource::kAsyncPLLP;
                    case SCTADC12ClockSource::kNone:
                        return ADCCommonClockSource::kSync;
                }
            }
            else if constexpr (tkCommonID == ADCCommonID::kADC345)
            {
                switch (sct_config.adc345_source)
                {
                    case SCTADC345ClockSource::kSysclk:
                        return ADCCommonClockSource::kAsyncSysclk;
                    case SCTADC345ClockSource::kPLLP:
                        return ADCCommonClockSource::kAsyncPLLP;
                    case SCTADC345ClockSource::kNone:
                        return ADCCommonClockSource::kSync;
                }
            }
            else
            {
                static_assert(kAlwaysFalseV<tkCommonID>, "Unsupported ADC Common ID");
            }
        }

        [[nodiscard]] static constexpr uint32_t get_source_clock_freq_hz(const RCCConfig& rcc_config)
        {
            if constexpr (tkCommonID == ADCCommonID::kADC12)
            {
                return rcc_config.get_adc12_freq_hz();
            }
            else if constexpr (tkCommonID == ADCCommonID::kADC345)
            {
                return rcc_config.get_adc345_freq_hz();
            }
            else
            {
                static_assert(kAlwaysFalseV<tkCommonID>, "Unsupported ADC Common ID");
            }
        }
    };

    // -----------------------------------------------------------------------------
    // DEVICE CLASS
    // -----------------------------------------------------------------------------

    template <ADCCommonID tkCommonID>
    class ADCCommonDevice
    {
    public:
        struct Descriptor : public SharedDeviceDescriptor
        {
        };

        static constexpr ADCCommonID skCommonID = tkCommonID;
        using ClockTraitsT                      = ADCCommonTraits<skCommonID>;
        using RootInterfaceT                    = ADCCommonRootInterface;

        using DependDevices = TypeList<ADCCommonRootDevice>;
        using InjectDevices = TypeList<RCCInfoDevice<>>;

    private:
        [[no_unique_address]] DeviceRef<RCCInfoDevice<>> m_rcc_info;

    public:
        ADCCommonDevice() = delete;

        ADCCommonDevice(DeviceRef<RCCInfoDevice<>>&& rcc_info) : m_rcc_info(std::move(rcc_info))
        {
        }

        // --- Initialization ---
        /**
         * @brief Initialize ADC Clock with given configuration.
         * @param config ADC Clock configuration.
         * @note The ADC has certain clock speed limitations depending on use. Refer to Table 66. ADC characteristics
         * in the datasheet.
         */
        [[nodiscard]] bool init(const ADCCommonConfig<skCommonID>& config)
        {
            // Validate configuration against current clock tree settings
            const auto clock_source   = get_clock_source();
            const auto source_freq_hz = get_source_clock_freq_hz();
            if (config.validate(clock_source, source_freq_hz).has_value())
            {
                return false;
            }

            // Enable the ADC peripheral clock (REQUIRED before writing to any ADC registers)
            ClockTraitsT::enable_clock();

            // Now we can safely configure the ADC Common Control Register (ADC_CCR)
            std::visit(
                Overloaded{
                    [](const ADCCommonAsyncClockConfig& async_config)
                    { LL_ADC_SetCommonClock(ClockTraitsT::skInstance, static_cast<uint32_t>(async_config.prescaler)); },
                    [](const ADCCommonSyncClockConfig& sync_config)
                    { LL_ADC_SetCommonClock(ClockTraitsT::skInstance, static_cast<uint32_t>(sync_config.prescaler)); }},
                config.clock_config);

            return true;
        }

        [[nodiscard]] ADCCommonClockSource get_clock_source() const
        {
            if constexpr (tkCommonID == ADCCommonID::kADC12)
            {
                switch (m_rcc_info->get_adc12_source())
                {
                    case SCTADC12ClockSource::kSysclk:
                        return ADCCommonClockSource::kAsyncSysclk;
                    case SCTADC12ClockSource::kPLLP:
                        return ADCCommonClockSource::kAsyncPLLP;
                    case SCTADC12ClockSource::kNone:
                        return ADCCommonClockSource::kSync;
                }
            }
            else if constexpr (tkCommonID == ADCCommonID::kADC345)
            {
                switch (m_rcc_info->get_adc345_source())
                {
                    case SCTADC345ClockSource::kSysclk:
                        return ADCCommonClockSource::kAsyncSysclk;
                    case SCTADC345ClockSource::kPLLP:
                        return ADCCommonClockSource::kAsyncPLLP;
                    case SCTADC345ClockSource::kNone:
                        return ADCCommonClockSource::kSync;
                }
            }
            else
            {
                static_assert(kAlwaysFalseV<tkCommonID>, "Unsupported ADC Common ID");
            }
        }

        [[nodiscard]] uint32_t get_source_clock_freq_hz() const
        {
            if constexpr (tkCommonID == ADCCommonID::kADC12)
            {
                return m_rcc_info->get_adc12_freq_hz();
            }
            else if constexpr (tkCommonID == ADCCommonID::kADC345)
            {
                return m_rcc_info->get_adc345_freq_hz();
            }
            else
            {
                static_assert(kAlwaysFalseV<tkCommonID>, "Unsupported ADC Common ID");
            }
        }
    };

    // -----------------------------------------------------------------------------
    // DEVICE ALIASES
    // -----------------------------------------------------------------------------

    using ADC12CommonDevice  = ADCCommonDevice<ADCCommonID::kADC12>;
    using ADC345CommonDevice = ADCCommonDevice<ADCCommonID::kADC345>;

    using ADC12CommonConfig  = ADCCommonConfig<ADCCommonID::kADC12>;
    using ADC345CommonConfig = ADCCommonConfig<ADCCommonID::kADC345>;

}  // namespace valle::platform