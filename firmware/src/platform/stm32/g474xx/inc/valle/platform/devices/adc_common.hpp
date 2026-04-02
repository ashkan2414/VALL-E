#pragma once

#include <optional>
#include <string_view>
#include <variant>

#include "valle/platform/core.hpp"
#include "valle/platform/devices/rcc.hpp"
#include "valle/platform/hdi/adc_common.hpp"

namespace valle::platform
{
    // ============================================================================
    // ADC CLOCK DEVICE (SHARED DEVICE)
    // ============================================================================

    // -----------------------------------------------------------------------------
    // CONFIGURATIONS
    // -----------------------------------------------------------------------------
    enum class AdcCommonClockSource : uint8_t
    {
        kAsyncPllP = 0,
        kAsyncSysclk,
        kSync
    };

    struct AdcCommonAsyncClockConfig
    {
        AdcCommonAsyncClockPrescaler prescaler = AdcCommonAsyncClockPrescaler::kDiv1;
    };

    struct AdcCommonSyncClockConfig
    {
        AdcCommonSyncClockPrescaler prescaler = AdcCommonSyncClockPrescaler::kDiv1;
    };

    struct AdcCommonConfig
    {
        std::variant<AdcCommonAsyncClockConfig, AdcCommonSyncClockConfig> clock_config;

        [[nodiscard]] static constexpr std::optional<std::string_view> validate(const AdcCommonConfig      config,
                                                                                const AdcCommonClockSource clock_source,
                                                                                const uint32_t source_freq_hz)
        {
            return std::visit(
                Overloaded{[clock_source, source_freq_hz](
                               const AdcCommonAsyncClockConfig& async_config) -> std::optional<std::string_view>
                           {
                               if (clock_source != AdcCommonClockSource::kAsyncPllP &&
                                   clock_source != AdcCommonClockSource::kAsyncSysclk)
                               {
                                   return "ADC clock source must be asynchronous for AdcCommonAsyncClockConfig";
                               }

                               const auto target_freq_hz = AdcCommonRootTraits::calculate_async_clock_freq_hz(
                                   source_freq_hz, async_config.prescaler);

                               if (target_freq_hz > AdcCommonRootTraits::skMaxClockFreqHz)
                               {
                                   return "ADC clock frequency exceeds maximum allowed frequency";
                               }

                               return std::nullopt;
                           },
                           [clock_source, source_freq_hz](
                               const AdcCommonSyncClockConfig& sync_config) -> std::optional<std::string_view>
                           {
                               if (clock_source != AdcCommonClockSource::kSync)
                               {
                                   return "ADC clock source must be synchronous for AdcCommonSyncClockConfig";
                               }

                               const auto target_freq_hz = AdcCommonRootTraits::calculate_sync_clock_freq_hz(
                                   source_freq_hz, sync_config.prescaler);

                               if (target_freq_hz > AdcCommonRootTraits::skMaxClockFreqHz)
                               {
                                   return "ADC clock frequency exceeds maximum allowed frequency";
                               }

                               return std::nullopt;
                           }},
                config.clock_config);
        }

        template <AdcCommonControllerId tkControllerId>
        [[nodiscard]] static constexpr std::optional<std::string_view> validate(const AdcCommonConfig config,
                                                                                const RccConfig&      rcc_config)
        {
            const auto clock_source   = get_clock_source<tkControllerId>(rcc_config.sct);
            const auto source_freq_hz = get_source_clock_freq_hz<tkControllerId>(rcc_config);
            return validate(config, clock_source, source_freq_hz);
        }

        template <AdcCommonControllerId tkControllerId>
        [[nodiscard]] static constexpr AdcCommonClockSource get_clock_source(const SctConfig& sct_config)
        {
            if constexpr (tkControllerId == AdcCommonControllerId::kAdc12)
            {
                switch (sct_config.adc12_source)
                {
                    case SctAdc12ClockSource::kSysclk:
                        return AdcCommonClockSource::kAsyncSysclk;
                    case SctAdc12ClockSource::kPllOutputP:
                        return AdcCommonClockSource::kAsyncPllP;
                    case SctAdc12ClockSource::kNone:
                        return AdcCommonClockSource::kSync;
                }
            }
            else if constexpr (tkControllerId == AdcCommonControllerId::kAdc345)
            {
                switch (sct_config.adc345_source)
                {
                    case SctAdc345ClockSource::kSysclk:
                        return AdcCommonClockSource::kAsyncSysclk;
                    case SctAdc345ClockSource::kPllOutputP:
                        return AdcCommonClockSource::kAsyncPllP;
                    case SctAdc345ClockSource::kNone:
                        return AdcCommonClockSource::kSync;
                }
            }
            else
            {
                static_assert(kAlwaysFalseV<tkControllerId>, "Unsupported ADC Common ID");
            }
        }

        template <AdcCommonControllerId tkControllerId>
        [[nodiscard]] static constexpr uint32_t get_source_clock_freq_hz(const RccConfig& rcc_config)
        {
            if constexpr (tkControllerId == AdcCommonControllerId::kAdc12)
            {
                return rcc_config.get_adc12_freq_hz();
            }
            else if constexpr (tkControllerId == AdcCommonControllerId::kAdc345)
            {
                return rcc_config.get_adc345_freq_hz();
            }
            else
            {
                static_assert(kAlwaysFalseV<tkControllerId>, "Unsupported ADC Common ID");
            }
        }
    };

    // -----------------------------------------------------------------------------
    // DEVICE CLASS
    // -----------------------------------------------------------------------------

    template <AdcCommonControllerId tkControllerId>
    class AdcCommonController
    {
    public:
        struct Descriptor : public SharedDeviceDescriptor
        {
        };

        static constexpr AdcCommonControllerId skCommonId = tkControllerId;
        using ClockTraitsT                                = AdcCommonControllerTraits<skCommonId>;
        using ControllerHdiT                              = AdcCommonControllerHdi<skCommonId>;
        using InjectDevices                               = TypeList<ControllerHdiT, RccInfo<>>;

    private:
        [[no_unique_address]] DeviceRef<ControllerHdiT> m_controller_hw;
        [[no_unique_address]] DeviceRef<RccInfo<>>      m_rcc_info;

    public:
        AdcCommonController() = delete;

        AdcCommonController(DeviceRef<ControllerHdiT>&& hardware_key, DeviceRef<RccInfo<>>&& rcc_info)
            : m_controller_hw(std::move(hardware_key)), m_rcc_info(std::move(rcc_info))
        {
        }

        // --- Initialization ---
        /**
         * @brief Initialize ADC Clock with given configuration.
         * @param config ADC Clock configuration.
         * @note The ADC has certain clock speed limitations depending on use. Refer to Table 66. ADC characteristics
         * in the datasheet.
         */
        [[nodiscard]] bool init(const AdcCommonConfig& config)
        {
            // Validate configuration against current clock tree settings
            const auto clock_source   = get_clock_source();
            const auto source_freq_hz = get_source_clock_freq_hz();
            if (AdcCommonConfig::validate(config, clock_source, source_freq_hz).has_value())
            {
                return false;
            }

            // Enable the ADC peripheral clock (REQUIRED before writing to any ADC registers)
            m_controller_hw->enable_clock(true);

            // Now we can safely configure the ADC Common Control Register (Adc_CCR)
            std::visit(Overloaded{[](const AdcCommonAsyncClockConfig& async_config)
                                  { m_controller_hw->set_common_clock_async(async_config.prescaler); },
                                  [](const AdcCommonSyncClockConfig& sync_config)
                                  { m_controller_hw->set_common_clock_sync(sync_config.prescaler); }},
                       config.clock_config);

            return true;
        }

        [[nodiscard]] AdcCommonClockSource get_clock_source() const
        {
            if constexpr (tkControllerId == AdcCommonControllerId::kAdc12)
            {
                switch (m_rcc_info->get_adc12_source())
                {
                    case SctAdc12ClockSource::kSysclk:
                        return AdcCommonClockSource::kAsyncSysclk;
                    case SctAdc12ClockSource::kPllOutputP:
                        return AdcCommonClockSource::kAsyncPllP;
                    case SctAdc12ClockSource::kNone:
                        return AdcCommonClockSource::kSync;
                }
            }
            else if constexpr (tkControllerId == AdcCommonControllerId::kAdc345)
            {
                switch (m_rcc_info->get_adc345_source())
                {
                    case SctAdc345ClockSource::kSysclk:
                        return AdcCommonClockSource::kAsyncSysclk;
                    case SctAdc345ClockSource::kPllOutputP:
                        return AdcCommonClockSource::kAsyncPllP;
                    case SctAdc345ClockSource::kNone:
                        return AdcCommonClockSource::kSync;
                }
            }
            else
            {
                static_assert(kAlwaysFalseV<tkControllerId>, "Unsupported ADC Common ID");
            }
        }

        [[nodiscard]] uint32_t get_source_clock_freq_hz() const
        {
            if constexpr (tkControllerId == AdcCommonControllerId::kAdc12)
            {
                return m_rcc_info->get_adc12_freq_hz();
            }
            else if constexpr (tkControllerId == AdcCommonControllerId::kAdc345)
            {
                return m_rcc_info->get_adc345_freq_hz();
            }
            else
            {
                static_assert(kAlwaysFalseV<tkControllerId>, "Unsupported ADC Common ID");
            }
        }
    };

    // -----------------------------------------------------------------------------
    // DEVICE ALIASES
    // -----------------------------------------------------------------------------

    using Adc12CommonController  = AdcCommonController<AdcCommonControllerId::kAdc12>;
    using Adc345CommonController = AdcCommonController<AdcCommonControllerId::kAdc345>;

}  // namespace valle::platform