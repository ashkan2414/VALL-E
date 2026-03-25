#pragma once

#include <algorithm>
#include <array>
#include <cstdint>
#include <optional>
#include <variant>

#include "valle/platform/core.hpp"
#include "valle/platform/devices/adc_common.hpp"
#include "valle/platform/devices/dma.hpp"
#include "valle/platform/drivers/gpio/analog_in.hpp"
#include "valle/platform/hardware/adc.hpp"

namespace valle::platform
{
    // ============================================================================
    // ADC CONTROLLER DEVICE (SHARED DEVICE)
    // ============================================================================

    // -----------------------------------------------------------------------------
    // DEVICE TRAITS
    // -----------------------------------------------------------------------------
    template <AdcPeripheralId tkPeripheralId>
    struct AdcControllerDeviceTraits;

    template <>
    struct AdcControllerDeviceTraits<AdcPeripheralId::kAdc1>
    {
        using AdcClockDeviceT = Adc12CommonDevice;
    };

    template <>
    struct AdcControllerDeviceTraits<AdcPeripheralId::kAdc2>
    {
        using AdcClockDeviceT = Adc12CommonDevice;
    };

    template <>
    struct AdcControllerDeviceTraits<AdcPeripheralId::kAdc3>
    {
        using AdcClockDeviceT = Adc345CommonDevice;
    };

    template <>
    struct AdcControllerDeviceTraits<AdcPeripheralId::kAdc4>
    {
        using AdcClockDeviceT = Adc345CommonDevice;
    };

    template <>
    struct AdcControllerDeviceTraits<AdcPeripheralId::kAdc5>  // NOLINT(readability-magic-numbers)
    {
        using AdcClockDeviceT = Adc345CommonDevice;
    };

    // -----------------------------------------------------------------------------
    // COMPILE TimE CONFIGURATIONS
    // -----------------------------------------------------------------------------
    struct AdcControllerCTDefaultConfig
    {
        using DmaChannelT = DmaNullChannelDevice;
    };

    template <typename T>
    concept CValidAdcControllerCTConfig = requires {
        typename T::DmaChannelT;
    } && (CNullDmaChannel<typename T::DmaChannelT> || CDmaChannelDevice<typename T::DmaChannelT>);

    template <AdcPeripheralId tkPeripheralId>
    struct AdcControllerCTConfigRegistry
    {
        static constexpr auto skConfig = AdcControllerCTDefaultConfig{};
    };

#define VALLE_DEFINE_ADC_CONTROLLER_CT_CONFIG(tkPeripheralId, config)                                           \
    namespace valle::platform                                                                                   \
    {                                                                                                           \
        template <>                                                                                             \
        struct AdcControllerCTConfigRegistry<(tkPeripheralId)>                                                  \
        {                                                                                                       \
            static constexpr auto skConfig = (config);                                                          \
            static_assert(CValidAdcControllerCTConfig<decltype(skConfig)>, "Invalid ADC Controller CT Config"); \
        };                                                                                                      \
    }

    // -----------------------------------------------------------------------------
    // CONFIGURATIONS
    // -----------------------------------------------------------------------------

    /**
     * @brief Configuration for ADC Injected Group
     *
     */
    struct AdcInjectGroupConfig
    {
        // What triggers the inject group (software or external)
        AdcInjectGroupTriggerSource trigger_source = AdcInjectGroupTriggerSource::kSoftware;

        // What edge to trigger on (only for external triggers)
        AdcInjectGroupTriggerEdge trigger_edge = AdcInjectGroupTriggerEdge::kRising;

        // RM0440 Section 21.4.16: Injected group can auto-trigger after regular group
        bool auto_trigger_from_regular = false;
    };

    /**
     * @brief Configuration for ADC Regular Group DMA Transfer
     *
     */
    struct AdcRegularGroupDmaConfig
    {
        /// DMA Channel Priority
        DmaPriority priority = DmaPriority::kMedium;

        /// True = Continuous/Circular, False = One-Shot/Normal
        bool circular_mode = true;

        /// Optional Interrupt Configuration
        std::optional<DmaChannelInterruptConfig> interrupts = std::nullopt;
    };

    /**
     * @brief Configuration for ADC Regular Group (Background/DMA).
     *
     */
    struct AdcRegularGroupConfig
    {
        /// Source of trigger (Software/External)
        AdcRegularGroupTriggerSource trigger_source = AdcRegularGroupTriggerSource::kSoftware;

        /// What edge causes trigger (Rising/Falling/Both)
        AdcRegularGroupTriggerEdge trigger_edge = AdcRegularGroupTriggerEdge::kRising;

        /// DMA Transfer configuration
        AdcRegularGroupDmaConfig dma = AdcRegularGroupDmaConfig{};

        /// Overrun behavior: Preserve vs Overwrite
        AdcRegularGroupOverrunBehavior overrun = AdcRegularGroupOverrunBehavior::kOverwrite;

        /// Conversion mode: Single Shot vs Continuous
        AdcRegularGroupConversionMode conversion_mode = AdcRegularGroupConversionMode::kSingleShot;

        /// Oversampling mode: Continuous vs discontinuous
        AdcRegularGroupOversamplingMode oversampling_mode = AdcRegularGroupOversamplingMode::kDiscontinuous;
    };

    /**
     * @brief Configuration for ADC Oversampling (Hardware Averaging).
     *
     */
    struct AdcOversamplingConfig
    {
        // How many times to sample (2x to 256x)
        AdcOversamplingRatio ratio = AdcOversamplingRatio::k2x;

        // Right shift bits (0 to 8)
        AdcOversamplingShift shift = AdcOversamplingShift::kNone;

        // Oversampling behavior when both groups are selected
        AdcOversamplingScope scope = AdcOversamplingScope::kDisable;
    };

    /**
     * @brief Configuration for the ADC Peripheral (Global).
     */
    struct AdcControllerConfig
    {
        // --- Core Settings ---
        // Sampling resolution (12, 10, 8, 6 bits)
        AdcResolution resolution = AdcResolution::k12Bit;

        // Right/Left data alignment
        AdcDataAlignment data_alignment = AdcDataAlignment::kRight;

        // Low power mode
        AdcLowPowerMode low_power = AdcLowPowerMode::kNone;

        // Inject Group (High Priority/Interrupt)
        AdcInjectGroupConfig inj = AdcInjectGroupConfig{};

        // Regular Group (Background/DMA)
        AdcRegularGroupConfig reg = AdcRegularGroupConfig{};

        // Oversampling (Hardware Averaging)
        std::optional<AdcOversamplingConfig> oversampling = std::nullopt;
    };

    struct AdcChannelOffsetConfig
    {
        /// Offset Index (1 to 4)
        AdcChannelOffsetIdx idx = AdcChannelOffsetIdx::kOffset1;

        /// Offset Value (Raw ADC counts)
        uint32_t value = 0;
    };

    /**
     * @brief Configuration for a single ADC Channel (Physics).
     */
    struct AdcChannelConfig
    {
        /**
         * @brief Sampling time for the channel.
         * RM0440 Section 21.4.8, Table 115:
         * - Internal channels (VREFINT, VBAT, TempSensor): Minimum 12.5 cycles
         * - External channels with high impedance (> 50 kΩ): Consider longer sampling times
         * - At 60 MHz ADC clock (max): 12.5 cycles = 0.208 µs
         */
        AdcChannelSampleTime sampling_time = AdcChannelSampleTime::k24Cycles5;

        // Input mode (Single-Ended/Differential)
        AdcChannelInputMode input_mode = AdcChannelInputMode::kSingleEnded;

        // Optional offset configuration
        std::optional<AdcChannelOffsetConfig> offset = std::nullopt;
    };

    // -----------------------------------------------------------------------------
    // DEVICE CLASS
    // -----------------------------------------------------------------------------

    template <AdcPeripheralId tkPeripheralId>
    class AdcControllerDevice
    {
    public:
        struct Descriptor : public SharedDeviceDescriptor
        {
        };
        static constexpr AdcPeripheralId skPeripheralId = tkPeripheralId;
        using ControllerTraitsT                         = AdcControllerTraits<skPeripheralId>;
        using ControllerDeviceTraitsT                   = AdcControllerDeviceTraits<skPeripheralId>;
        using InterfaceT                                = AdcControllerInterface<skPeripheralId>;
        using InjectGroupInterfaceT                     = AdcInjectGroupInterface<skPeripheralId>;
        using RegularGroupInterfaceT                    = AdcRegularGroupInterface<skPeripheralId>;

        template <AdcChannelId tkChannelId>
        using ChannelTraitsT = AdcChannelTraits<tkPeripheralId, tkChannelId>;

        using ClockDeviceT         = typename ControllerDeviceTraitsT::AdcClockDeviceT;
        using InterruptControllerT = AdcInterruptController<skPeripheralId>;

        using CTConfigRegistryT          = AdcControllerCTConfigRegistry<skPeripheralId>;
        static constexpr auto skCTConfig = CTConfigRegistryT::skConfig;
        using CTConfigT                  = decltype(skCTConfig);
        using DmaChannelT                = CTConfigT::DmaChannelT;
        static constexpr bool skHasDMA   = !CNullDmaChannel<DmaChannelT>;

        using DependDevices = TypeList<ClockDeviceT>;
        using InjectDevices = std::conditional_t<skHasDMA, TypeList<DmaChannelT>, TypeList<>>;

    private:
        template <std::size_t... I>
        [[nodiscard]] static constexpr auto make_channel_traits(std::index_sequence<I...>)
        {
            constexpr auto channels = magic_enum::enum_values<AdcChannelId>();

            return std::tuple<ChannelTraitsT<channels[I]>...>{};
        }

        static constexpr auto skChannelTraits =
            make_channel_traits(std::make_index_sequence<magic_enum::enum_count<AdcChannelId>()>{});

        [[no_unique_address]] ConditionalDeviceRef<skHasDMA, DmaChannelT> m_dma;

        // --- Storage ---
        // volatile is CRITICAL for DMA buffer
        // STM32G4 (Cortex-M4F) has no data cache, but volatile ensures:
        // 1. Each read goes to memory (DMA updates continuously in circular mode)
        // 2. Compiler won't optimize away repeated reads
        // 3. Proper memory ordering with DMA transfers
        // If future code removes volatile, stale data will be read in fast loops!
        alignas(32) volatile AdcValue m_dma_buffer[kAdcMaxRegChannels];

        // Mappings
        // Ch -> Rank (1-4)
        std::array<std::optional<AdcInjectChannelRank>, kNumAdcChannels> m_inj_cidx_to_rank_map{};

        // Ch -> Rank (1-16) (rank-1 for dma idx)
        std::array<std::optional<AdcRegularChannelRank>, kNumAdcChannels> m_reg_cidx_to_rank_map{};

        // Cached values for optimization
        uint32_t m_inject_effective_resolution_range  = 4095;
        uint32_t m_regular_effective_resolution_range = 4095;
        float    m_ref_voltage                        = 3.3F;

    public:
        AdcControllerDevice(DeviceRef<DmaChannelT>&& dma_channel)
            requires(skHasDMA)
            : m_dma(std::move(dma_channel))
        {
            std::fill(std::begin(m_dma_buffer), std::end(m_dma_buffer), 0);
        }

        AdcControllerDevice()
            requires(!skHasDMA)
        {
            std::fill(std::begin(m_dma_buffer), std::end(m_dma_buffer), 0);
        }

        // --- Initialization (Called by Main/Builder) ---

        /**
         * @brief Initialize ADC Peripheral with given configuration.
         *
         * @param config ADC Peripheral Configuration.
         */
        [[nodiscard]] bool init()
        {
            if (!disable())
            {
                VALLE_LOG_FATAL("Failed to disable ADC");
                return false;
            }

            if (!disable_deep_power_mode())
            {
                VALLE_LOG_FATAL("Failed to disable ADC deep power mode");
                return false;
            }

            if (!enable_voltage_regulator())
            {
                VALLE_LOG_FATAL("Failed to enable ADC internal regulator");
                return false;
            }

            if (!calibrate())
            {
                VALLE_LOG_FATAL("ADC calibration failed");
                return false;
            }

            return true;
        }

        // --- Registration API (Called by Channel) ---
        /**
         * @brief Initialize ADC channel as an Inject Channel.
         *
         * @tparam tkChannelId Channel ID to initialize.
         * @tparam tkRank Rank in Inject Sequence (1-4).
         */
        template <AdcChannelId tkChannelId, AdcInjectChannelRank tkRank>
        void init_channel_as_inject(const AdcChannelConfig& cfg)
        {
            init_channel_core<tkChannelId>(cfg);
            return register_inject_sequence<tkChannelId, tkRank>();
        }

        /**
         * @brief Initialize ADC channel as a Regular Channel.
         *
         * @tparam tkChannelId Channel ID to initialize.
         * @tparam tkRank Rank in Regular Sequence (1-16).
         */
        template <AdcChannelId tkChannelId, AdcRegularChannelRank tkRank>
        void init_channel_as_regular(const AdcChannelConfig& cfg)
        {
            init_channel_core<tkChannelId>(cfg);
            return register_regular_sequence<tkChannelId, tkRank>();
        }

        /**
         * @brief Post init called after init and channel registration. Sets up sequences and starts ADC.
         *
         */
        [[nodiscard]] bool post_init(const AdcControllerConfig& config)
        {
            const uint8_t inj_count = static_cast<uint8_t>(std::count_if(std::begin(m_inj_cidx_to_rank_map),
                                                                         std::end(m_inj_cidx_to_rank_map),
                                                                         [](const auto& v) { return v.has_value(); }));

            const uint8_t reg_count = static_cast<uint8_t>(std::count_if(std::begin(m_reg_cidx_to_rank_map),
                                                                         std::end(m_reg_cidx_to_rank_map),
                                                                         [](const auto& v) { return v.has_value(); }));

            // Global Configuration
            InterfaceT::set_resolution(config.resolution);
            InterfaceT::set_data_alignment(config.data_alignment);
            InterfaceT::set_low_power_mode(config.low_power);
            InterfaceT::set_gain_compensation(0);  // No gain compensation
            InterfaceT::set_common_sample_time(AdcCommonSamplingTime::kDefault);

            // Oversampling Configuration
            if (config.oversampling.has_value())
            {
                AdcOversamplingConfig ovs_cfg = config.oversampling.value();
                InterfaceT::set_oversampling_scope(ovs_cfg.scope);
                InterfaceT::set_oversampling_ratio_shift(ovs_cfg.ratio, ovs_cfg.shift);
                RegularGroupInterfaceT::set_oversampling_mode(config.reg.oversampling_mode);
            }

            if (!post_init_inj_pre_enable(inj_count, config.inj))
            {
                return false;
            }

            if (!post_init_reg_pre_enable(reg_count, config.reg, config.data_alignment))
            {
                return false;
            }

            // Enable ADC
            if (!enable())
            {
                VALLE_LOG_FATAL("ADC enable failed");
                return false;
            }

            if (!post_init_inj_post_enable(inj_count, config.inj))
            {
                return false;
            }

            if (!post_init_reg_post_enable(reg_count, config.reg))
            {
                return false;
            }

            m_inject_effective_resolution_range  = InterfaceT::get_inject_group_effective_resolution_range();
            m_regular_effective_resolution_range = InterfaceT::get_regular_group_effective_resolution_range();

            return true;
        }

        [[nodiscard]] bool initialized() const
        {
            return InterfaceT::is_enabled();
        }

        // --- Control API (Called by Application) ---
        void enable_interrupts(const AdcInterruptConfig& config)
        {
            if (!initialized())
            {
                VALLE_LOG_ERROR("ADC must be initialized before enabling interrupts");
                return;
            }

            InterruptControllerT::enable_interrupts(config);
        }

        void disable_interrupts()
        {
            InterruptControllerT::disable_interrupts();
        }

        /**
         * @brief Arms the Inject Sequence.
         * @note Must be called AFTER ADC is enabled (post_init).
         */
        void start_inject()
        {
            if (!initialized())
            {
                VALLE_LOG_ERROR("ADC must be initialized before starting conversions.");
                return;
            }

            AdcInterruptTraits<tkPeripheralId, AdcInterruptType::kInjectEndOfSequence>::ack();

            // ARM THE TRIGGER (The most important part)
            // If Trigger is Hardware (HRTIM): ADC goes into "Waiting for Trigger" state.
            // If Trigger is Software: ADC converts immediately.
            trigger_inject();
        }

        /**
         * @brief Rearm Inject Conversions.
         *
         */
        void trigger_inject()
        {
            InterfaceT::start_inject();
        }

        /**
         * @brief Stop Inject Conversions.
         *
         */
        void stop_inject()
        {
            InterfaceT::stop_inject();
        }

        /**
         * @brief Arms the Regular Sequence.
         *
         */
        void start_regular()
        {
            if (!initialized())
            {
                VALLE_LOG_ERROR("ADC must be initialized before starting conversions.");
                return;
            }

            // Clear all relevant flags to avoid immediate false triggers
            // RM0440 Section 21.4.20: OVR flag must be cleared before starting
            AdcInterruptTraits<tkPeripheralId, AdcInterruptType::kRegularEndOfSequence>::ack();
            AdcInterruptTraits<tkPeripheralId, AdcInterruptType::kOverrun>::ack();

            if constexpr (skHasDMA)
            {
                // Start DMA Transfer
                const uint32_t adc_dr_addr = reinterpret_cast<uint32_t>(&(ControllerTraitsT::skInstance->DR));
                const uint32_t buffer_addr = reinterpret_cast<uint32_t>(m_dma_buffer);
                const uint32_t reg_count   = RegularGroupInterfaceT::get_sequencer_length();
                m_dma->start_periph_to_mem(adc_dr_addr, buffer_addr, reg_count);
            }

            // ARM THE TRIGGER (The most important part)
            // If Trigger is Hardware (HRTIM): ADC goes into "Waiting for Trigger" state.
            // If Trigger is Software: ADC converts immediately.
            trigger_regular();
        }

        /**
         * @brief Rearm Regular Conversions.
         *
         */
        void trigger_regular()
        {
            InterfaceT::start_regular();
        }

        /**
         * @brief Stop Regular Conversions.
         *
         */
        void stop_regular()
        {
            InterfaceT::stop_regular();
            if constexpr (skHasDMA)
            {
                m_dma->disable_interrupts();
            }
        }

        // --- Data Access ---
        /**
         * @brief Read data from Regular Channel.
         *
         * @tparam tkRank Rank in Regular Sequence (1-16).
         * @return AdcValue Raw ADC value.
         */
        template <AdcInjectChannelRank tkRank>
        [[nodiscard]] constexpr inline AdcValue read_inject_data() const
        {
            return read_inject_data(tkRank);
        }

        /**
         * @brief Read data from Inject Channel (Slow).
         *
         * @tparam tkChannelId Channel ID to read.
         * @return AdcValue Raw ADC value.
         */
        template <AdcChannelId tkChannelId>
        [[nodiscard]] constexpr inline AdcValue read_inject_data_slow()
        {
            const auto rank = get_adc_channel_rank_inject<tkChannelId>();
            expect(rank.has_value(), "Channel not registered in inject sequence");
            return read_inject_data(rank.value());
        }

        /**
         * @brief Read data from Regular Channel.
         *
         * @tparam tkRank Rank in Regular Sequence (1-16).
         * @return AdcValue Raw ADC value.
         */
        template <AdcRegularChannelRank tkRank>
        [[nodiscard]] constexpr inline AdcValue read_regular_data() const
        {
            return read_regular_data(tkRank);
        }

        /**
         * @brief Read data from Regular Channel (Slow).
         *
         * @tparam tkChannelId Channel ID to read.
         * @return AdcValue Raw ADC value.
         */
        template <AdcChannelId tkChannelId>
        [[nodiscard]] constexpr inline AdcValue read_regular_data_slow()
        {
            const auto rank = get_adc_channel_rank_regular<tkChannelId>();
            expect(rank.has_value(), "Channel not registered in regular sequence");
            return read_regular_data(rank.value());
        }

        /**
         * @brief Convert raw ADC value from inject group to normalized float (0.0 to 1.0).
         *
         * @param raw Raw ADC value.
         * @return float Normalized value.
         */
        [[nodiscard]] constexpr float raw_to_normalized_inject(const AdcValue raw) const
        {
            return static_cast<float>(raw) / m_inject_effective_resolution_range;
        }

        /**
         * @brief Convert raw ADC value from regular group to normalized float (0.0 to 1.0).
         *
         * @param raw Raw ADC value.
         * @return float Normalized value.
         */
        [[nodiscard]] constexpr float raw_to_normalized_regular(const AdcValue raw) const
        {
            return static_cast<float>(raw) / m_regular_effective_resolution_range;
        }

        /**
         * @brief Convert raw ADC value from inject group to voltage (Volts).
         *
         * @param raw Raw ADC value.
         * @return float Voltage in Volts.
         */
        [[nodiscard]] constexpr float raw_to_voltage_inject(const AdcValue raw) const
        {
            return raw_to_normalized_inject(raw) * m_ref_voltage;
        }

        /**
         * @brief Convert raw ADC value from regular group to voltage (Volts).
         *
         * @param raw Raw ADC value.
         * @return float Voltage in Volts.
         */
        [[nodiscard]] constexpr float raw_to_voltage_regular(const AdcValue raw) const
        {
            return raw_to_normalized_regular(raw) * m_ref_voltage;
        }

    private:
        [[nodiscard]] static bool disable()
        {
            if (InterfaceT::is_enabled())
            {
                InterfaceT::disable();
            }

            return true;
        }

        [[nodiscard]] static bool enable()
        {
            InterfaceT::enable();
            const bool adc_ready = TimingContext::wait_for_with_timeout_us(
                []() { return LL_ADC_IsActiveFlag_ADRDY(ControllerTraitsT::skInstance) != 0; }, 100U);

            // If timeout occurred, ADC is not ready
            return adc_ready;
        }

        [[nodiscard]] bool disable_deep_power_mode()
        {
            if (InterfaceT::deep_power_down_enabled())
            {
                // Disable ADC deep power down mode to allow access to internal
                // voltage regulator and calibration
                InterfaceT::disable_deep_power_down();

                // System was in deep power down mode, calibration must
                // be relaunched or a previously saved calibration factor
                // re-applied once the ADC voltage regulator is enabled

                TimingContext::delay_ms_busy(10u);  // Short delay to ensure deep power down is fully exited
            }

            if (InterfaceT::deep_power_down_enabled())
            {
                return false;
            }

            return true;
        }

        [[nodiscard]] bool enable_voltage_regulator()
        {
            if (!InterfaceT::internal_regulator_enabled())
            {
                // Enable ADC internal voltage regulator
                InterfaceT::enable_internal_regulator();

                // RM0440 Section 21.4.6: tADCVREG_STUP = 20 µs (typ)
                // wait for 100 to be safe.
                TimingContext::delay_ms_busy(100u);
            }

            if (!InterfaceT::internal_regulator_enabled())
            {
                return false;
            }

            return true;
        }

        [[nodiscard]] bool calibrate()
        {
            // Calibration with precise timeout
            // RM0440 Section 21.4.6: tCAL = 116 ADC clock cycles
            // At 40 MHz ADC clock: ~3 µs typical, allow up to 100 µs for safety
            //
            // NOTE: If ADC clock frequency changes at runtime (e.g., entering
            // low-power mode), calibration must be re-run.
            InterfaceT::start_calibration();
            const bool calibration_success = TimingContext::wait_for_with_timeout_us(
                []() { return InterfaceT::is_calibration_ongoing() == 0; }, 100u);

            // If timeout occurred, calibration failed
            return calibration_success;
        }

        /**
         * @brief Helper function to iterate over all channel traits with an index sequence.
         *
         * @tparam TFunc Type of the function to apply to each channel trait.
         * @tparam I Index sequence for tuple unpacking.
         * @param func The function to apply to each channel trait.
         */
        template <typename TFunc, std::size_t... I>
        static constexpr void for_each_channel_impl(TFunc&& func, std::index_sequence<I...>)
        {
            (func(std::get<I>(skChannelTraits)), ...);  // fold expression over tuple
        }

        /**
         * @brief Iterate over all channel traits and apply the given function.
         *
         * @tparam TFunc Type of the function to apply to each channel trait. Should be callable with a single argument
         * of type ChannelTraitsT<tkChannelId>.
         * @param func The function to apply to each channel trait.
         */
        template <typename TFunc>
        static constexpr void for_each_channel(TFunc&& func)
        {
            constexpr auto N = std::tuple_size_v<std::remove_reference_t<decltype(skChannelTraits)>>;
            for_each_channel_impl(std::forward<TFunc>(func), std::make_index_sequence<N>{});
        }

        [[nodiscard]] bool post_init_inj_pre_enable(const uint32_t inj_count, const AdcInjectGroupConfig& config)
        {
            if (inj_count == 0)
            {
                return true;  // No inject channels configured, skip inject group setup
            }

            // Core Config
            InjectGroupInterfaceT::set_queue_mode(AdcInjectGroupQueueMode::kDisable);  // Queue mode not supported
            InjectGroupInterfaceT::set_trigger_mode(config.auto_trigger_from_regular
                                                        ? AdcInjectGroupTriggerMode::kFromRegularGroup
                                                        : AdcInjectGroupTriggerMode::kIndependent);
            InjectGroupInterfaceT::set_sequencer_discontinuity_mode(
                AdcInjectGroupSequencerDiscontinuityMode::kDisable);  // Discontinuity not supported
            return true;
        }

        [[nodiscard]] bool post_init_reg_pre_enable(const uint32_t               reg_count,
                                                    const AdcRegularGroupConfig& config,
                                                    AdcDataAlignment             data_alignment)
        {
            if (reg_count == 0)
            {
                return true;  // No regular channels configured, skip regular group setup
            }

            // Core Config
            RegularGroupInterfaceT::config_trigger(config.trigger_source, config.trigger_edge);
            RegularGroupInterfaceT::set_overrun_behavior(config.overrun);
            RegularGroupInterfaceT::set_conversion_mode(config.conversion_mode);

            // Sequence Config
            RegularGroupInterfaceT::set_sequencer_length(reg_count);

            for_each_channel(
                [this](const auto& channel_trait)
                {
                    using ChannelTraitT = std::decay_t<decltype(channel_trait)>;
                    const auto& rank    = m_reg_cidx_to_rank_map[ChannelTraitT::skChannelIdx];

                    if (rank.has_value())
                    {
                        RegularGroupInterfaceT::template set_sequencer_ranks<ChannelTraitT::skChannelId>(rank.value());
                    }
                });

            // DMA Configuration
            if constexpr (skHasDMA)
            {
                AdcRegularGroupDmaTransfer dma_transfer = config.dma.circular_mode
                                                              ? AdcRegularGroupDmaTransfer::kUnlimited
                                                              : AdcRegularGroupDmaTransfer::kLimited;
                RegularGroupInterfaceT::set_dma_transfer(dma_transfer);

                static_assert(sizeof(AdcValue) == 2 || sizeof(AdcValue) == 4, "AdcValue must be 2 or 4 bytes");

                // Match DMA width to ADC alignment
                // RM0440 Section 21.4.20: Left-aligned ADC data requires 32-bit DMA
                DmaDataWidth dma_width;
                if (data_alignment == AdcDataAlignment::kLeft)
                {
                    // Left-aligned: Always use 32-bit (word) transfer
                    dma_width = DmaDataWidth::kWord;
                }
                else
                {
                    // Right-aligned: Use AdcValue size (16-bit or 32-bit)
                    dma_width = sizeof(AdcValue) == 2 ? DmaDataWidth::kHalfWord : DmaDataWidth::kWord;
                }

                // Configure DMA Channel
                if (!m_dma->init(DmaChannelConfig{
                        .direction         = DmaDirection::kPeriphToMem,
                        .priority          = config.dma.priority,
                        .mode              = config.dma.circular_mode ? DmaMode::kCircular : DmaMode::kNormal,
                        .periph_data_width = dma_width,
                        .memory_data_width = dma_width,
                        .inc_periph        = false,
                        .inc_memory        = true,
                        .request_id        = ControllerTraitsT::skDmaMuxRequest,

                    }))
                {
                    VALLE_LOG_FATAL("ADC DMA Channel initialization failed");
                    return false;
                }

                if (config.dma.interrupts.has_value())
                {
                    m_dma->enable_interrupts(config.dma.interrupts.value());
                }
            }
            else
            {
                RegularGroupInterfaceT::set_dma_transfer(AdcRegularGroupDmaTransfer::kNone);
            }

            return true;
        }

        [[nodiscard]] bool post_init_inj_post_enable(const uint32_t inj_count, const AdcInjectGroupConfig& config)
        {
            if (inj_count == 0)
            {
                return true;  // No inject channels configured, skip inject group setup
            }

            // Core Config
            InjectGroupInterfaceT::config_trigger(config.trigger_source, config.trigger_edge);

            // Sequence Config
            InjectGroupInterfaceT::set_sequencer_length(inj_count);

            for_each_channel(
                [this](const auto& channel_trait)
                {
                    using ChannelTraitT = std::decay_t<decltype(channel_trait)>;
                    const auto rank     = m_inj_cidx_to_rank_map[ChannelTraitT::skChannelIdx];

                    if (rank.has_value())
                    {
                        InjectGroupInterfaceT::template set_sequencer_ranks<ChannelTraitT::skChannelId>(rank.value());
                    }
                });

            return true;
        }

        [[nodiscard]] bool post_init_reg_post_enable(const uint32_t reg_count, const AdcRegularGroupConfig& config)
        {
            return true;
        }

        /**
         * @brief Get the ADC Channel Rank in Regular Sequence.
         *
         * @tparam tkChannelId The ID of the channel.
         * @return std::optional<uint8_t> Rank (1-16).
         */
        template <AdcChannelId tkChannelId>
        [[nodiscard]] constexpr inline std::optional<AdcRegularChannelRank> get_adc_channel_rank_regular() const
        {
            constexpr uint32_t idx = ChannelTraitsT<tkChannelId>::skChannelIdx;
            return m_reg_cidx_to_rank_map[idx];
        }

        /**
         * @brief Get the ADC Channel Rank in Inject Sequence.
         *
         * @tparam tkChannelId The ID of the channel.
         * @return std::optional<uint8_t> Rank (1-4).
         */
        template <AdcChannelId tkChannelId>
        [[nodiscard]] constexpr inline std::optional<AdcInjectChannelRank> get_adc_channel_rank_inject() const
        {
            constexpr uint32_t idx = ChannelTraitsT<tkChannelId>::skChannelIdx;
            return m_inj_cidx_to_rank_map[idx];
        }

        /**
         * @brief Configure and register this channel.
         *
         * @tparam tkChannelId The ID of the channel to configure.
         * @param cfg        Physics configuration (Sampling time, Single/Diff, etc.)
         */
        template <AdcChannelId tkChannelId>
        void init_channel_core(const AdcChannelConfig& cfg)
        {
            using ChannelInterfaceT = AdcChannelInterface<tkPeripheralId, tkChannelId>;

            ChannelInterfaceT::set_sampling_time(cfg.sampling_time);
            ChannelInterfaceT::set_input_mode(cfg.input_mode);

            if (cfg.offset.has_value())
            {
                AdcChannelOffsetConfig offset_cfg = cfg.offset.value();
                ChannelInterfaceT::config_offset(offset_cfg.idx, offset_cfg.value);
            }
        }

        /**
         * @brief Registers a channel for the inject conversion sequence.
         *
         * @tparam tkChannelId The ID of the channel to register.
         * @tparam tkRank Rank in Inject Sequence (1-4).
         */
        template <AdcChannelId tkChannelId, AdcInjectChannelRank tkRank>
        void register_inject_sequence()
        {
            constexpr uint32_t idx = ChannelTraitsT<tkChannelId>::skChannelIdx;
            if (!m_inj_cidx_to_rank_map[idx].has_value())
            {
                m_inj_cidx_to_rank_map[idx] = tkRank;
            }
        }

        /**
         * @brief Registers a channel for the regular conversion sequence.
         *
         * @tparam tkChannelId The ID of the channel to register.
         * @tparam tkRank The rank in the regular sequence (1-16).
         */
        template <AdcChannelId tkChannelId, AdcRegularChannelRank tkRank>
        void register_regular_sequence()
        {
            constexpr uint32_t idx = ChannelTraitsT<tkChannelId>::skChannelIdx;
            if (!m_reg_cidx_to_rank_map[idx].has_value())
            {
                m_reg_cidx_to_rank_map[idx] = tkRank;
            }
        }

        /**
         * @brief Read data from inject channel.
         *
         * @param rank Rank in Inject Sequence (1-4).
         * @return AdcValue Raw ADC value.
         */
        [[nodiscard]] inline AdcValue read_inject_data(const AdcInjectChannelRank rank) const
        {
            return InjectGroupInterfaceT::read(rank);
        }

        /**
         * @brief Read data from regular channel.
         *
         * @param rank Rank in Regular Sequence (1-16).
         * @return AdcValue Raw ADC value.
         */
        [[nodiscard]] constexpr inline AdcValue read_regular_data(const AdcRegularChannelRank rank) const
        {
            // Memory barrier to ensure DMA writes are visible to CPU
            // STM32G4 (Cortex-M4F) doesn't have data cache, but memory barrier
            // ensures proper ordering of DMA transfers vs CPU reads
            __DMB();
            const uint8_t buf_idx = get_regular_channel_rank_number(rank) - 1;
            return m_dma_buffer[buf_idx];
        }
    };

    // -----------------------------------------------------------------------------
    // DEVICE ALIASES
    // -----------------------------------------------------------------------------

    using Adc1ControllerDevice = AdcControllerDevice<AdcPeripheralId::kAdc1>;
    using Adc2ControllerDevice = AdcControllerDevice<AdcPeripheralId::kAdc2>;
    using Adc3ControllerDevice = AdcControllerDevice<AdcPeripheralId::kAdc3>;
    using Adc4ControllerDevice = AdcControllerDevice<AdcPeripheralId::kAdc4>;
    using Adc5ControllerDevice = AdcControllerDevice<AdcPeripheralId::kAdc5>;

    // ============================================================================
    // ADC CHANNEL (UNIQUE DEVICE)
    // ============================================================================

    // ----------------------------------------------------------------------------
    // CONFIGURATIONS
    // ----------------------------------------------------------------------------

    // ----------------------------------------------------------------------------
    // DEVICE CLASS
    // ----------------------------------------------------------------------------

    template <typename T>
    struct AdcPinDevice;

    template <CNullAdcPinMap T>
    struct AdcPinDevice<T>
    {
        using type = GpioNullPinDevice;
    };

    template <typename T>
    struct AdcPinDevice
    {
        using type = GpioPinDevice<T::skPort, T::skPin>;
    };

    template <AdcPeripheralId tkPeripheralId, AdcChannelId tkChannelId>
    class AdcChannelDevice
    {
    public:
        struct Descriptor : public UniqueDeviceDescriptor
        {
        };

        static constexpr AdcPeripheralId skPeripheralId = tkPeripheralId;
        static constexpr AdcChannelId    skChannelId    = tkChannelId;
        using ControllerT                               = AdcControllerDevice<tkPeripheralId>;
        using ControllerTraitsT                         = AdcControllerTraits<tkPeripheralId>;
        using ChannelTraitsT                            = AdcChannelTraits<tkPeripheralId, tkChannelId>;

        using PinT                     = typename AdcPinDevice<AdcPinMap<tkPeripheralId, tkChannelId>>::type;
        static constexpr bool skHasPin = !CNullGpioPinDevice<PinT>;

        using InjectDevices = std::conditional_t<skHasPin, TypeList<ControllerT, PinT>, TypeList<ControllerT>>;

        template <AdcPeripheralId tkCId, AdcChannelId tkChId, AdcInjectChannelRank tkRank>
        friend class AdcInjectChannelDevice;

        template <AdcPeripheralId tkCId, AdcChannelId tkChId, AdcRegularChannelRank tkRank>
        friend class AdcRegularChannelDevice;

    private:
        [[no_unique_address]] DeviceRef<ControllerT>                           m_adc;
        std::conditional_t<skHasPin, GpioAnalogInDriver<PinT>, std::monostate> m_pin;

    public:
        explicit AdcChannelDevice(DeviceRef<ControllerT>&& adc, DeviceRef<PinT>&& pin)
            requires(skHasPin)
            : m_adc(std::move(adc)), m_pin(std::move(pin))
        {
        }

        explicit AdcChannelDevice(DeviceRef<ControllerT> adc)
            requires(!skHasPin)
            : m_adc(std::move(adc))
        {
        }

    protected:
        // --- Initialization ---

        /**
         * @brief Configure and register this channel for the Inject Sequence (Interrupt).
         *
         * @tparam tkRank Rank in Inject Sequence (1-4).
         * @param config Physics configuration (Sampling time, Single/Diff, etc.)
         */
        template <AdcRegularChannelRank tkRank>
        [[nodiscard]] bool init_as_regular(const AdcChannelConfig& config)
        {
            m_adc->template init_channel_as_regular<skChannelId, tkRank>(config);
            return gpio_init();
        }

        /**
         * @brief Configure and register this channel for the Regular Sequence (DMA).
         *
         * @tparam tkRank Rank in Regular Sequence (1-16).
         * @param config Physics configuration (Sampling time, Single/Diff, etc.)
         */
        template <AdcInjectChannelRank tkRank>
        [[nodiscard]] bool init_as_inject(const AdcChannelConfig& config)
        {
            m_adc->template init_channel_as_inject<skChannelId, tkRank>(config);
            return gpio_init();
        }

        // --- Accessors ---
        // Read raw
        /**
         * @brief Read data from inject channel.
         *
         * @tparam tkRank Rank in Inject Sequence (1-4).
         */
        template <AdcInjectChannelRank tkRank>
        [[nodiscard]] inline AdcValue read_inject() const
        {
            return m_adc->template read_inject_data<tkRank>();
        }

        /**
         * @brief Read data from inject channel (slow method, use read_inject() instead).
         *
         * @return AdcValue Raw ADC value.
         */
        [[nodiscard]] inline AdcValue read_inject_slow() const
        {
            return m_adc->template read_inject_data_slow<skChannelId>();
        }

        /**
         * @brief Read data from regular channel.
         *
         * @tparam tkRank Rank in Regular Sequence (1-16).
         */
        template <AdcRegularChannelRank tkRank>
        [[nodiscard]] inline AdcValue read_regular() const
        {
            return m_adc->template read_regular_data<tkRank>();
        }

        /**
         * @brief Read data from regular channel (slow method, use read_regular() instead).
         *
         * @return AdcValue Raw ADC value.
         */
        [[nodiscard]] inline AdcValue read_regular_slow() const
        {
            return m_adc->template read_regular_data_slow<skChannelId>();
        }

        // Read voltage
        /**
         * @brief Read voltage from inject channel.
         *
         * @tparam tkRank Rank in Inject Sequence (1-4).
         * @return float Voltage in Volts.
         */
        template <AdcInjectChannelRank tkRank>
        [[nodiscard]] inline float read_inject_voltage() const
        {
            return raw_to_voltage_inject(read_inject<tkRank>());
        }

        /**
         * @brief Read voltage from inject channel (slow method, use read_inject_voltage() instead).
         *
         * @return float Voltage in Volts.
         */
        [[nodiscard]] inline float read_inject_voltage_slow() const
        {
            return raw_to_voltage_inject(read_inject_slow());
        }

        /**
         * @brief Read voltage from regular channel.
         *
         * @tparam tkRank Rank in Regular Sequence (1-16).
         * @return float Voltage in Volts.
         */
        template <AdcRegularChannelRank tkRank>
        [[nodiscard]] inline float read_regular_voltage() const
        {
            return raw_to_voltage_regular(read_regular<tkRank>());
        }

        /**
         * @brief Read voltage from regular channel (slow method, use read_regular_voltage() instead).
         *
         * @return float Voltage in Volts.
         */
        [[nodiscard]] inline float read_regular_voltage_slow() const
        {
            return raw_to_voltage_regular(read_regular_slow());
        }

        // Read normalized
        /**
         * @brief Read normalized value (0.0 to 1.0) from inject channel.
         *
         * @tparam tkRank Rank in Inject Sequence (1-4).
         * @return float Normalized value.
         */
        template <AdcInjectChannelRank tkRank>
        [[nodiscard]] inline float read_inject_normalized() const
        {
            return raw_to_normalized_inject(read_inject<tkRank>());
        }

        /**
         * @brief Read normalized value (0.0 to 1.0) from inject channel (slow method, use read_inject_normalized()
         * instead).
         *
         * @return float Normalized value.
         */
        [[nodiscard]] inline float read_inject_normalized_slow() const
        {
            return raw_to_normalized_inject(read_inject_slow());
        }

        /**
         * @brief Read normalized value (0.0 to 1.0) from regular channel.
         *
         * @tparam tkRank Rank in Regular Sequence (1-16).
         */
        template <AdcRegularChannelRank tkRank>
        [[nodiscard]] inline float read_regular_normalized() const
        {
            return raw_to_normalized_regular(read_regular<tkRank>());
        }

        /**
         * @brief Read normalized value (0.0 to 1.0) from regular channel (slow method, use read_regular_normalized()
         * instead).
         *
         * @return float Normalized value.
         */
        [[nodiscard]] inline float read_regular_normalized_slow() const
        {
            return raw_to_normalized_regular(read_regular_slow());
        }

        [[nodiscard]] constexpr float raw_to_normalized_inject(const AdcValue raw) const
        {
            return m_adc->raw_to_normalized_inject(raw);
        }

        [[nodiscard]] constexpr float raw_to_normalized_regular(const AdcValue raw) const
        {
            return m_adc->raw_to_normalized_regular(raw);
        }

        [[nodiscard]] constexpr float raw_to_voltage_inject(const AdcValue raw) const
        {
            return m_adc->raw_to_voltage_inject(raw);
        }

        [[nodiscard]] constexpr float raw_to_voltage_regular(const AdcValue raw) const
        {
            return m_adc->raw_to_voltage_regular(raw);
        }

    private:
        /**
         * @brief Initialize GPIO Pin (if pin exists).
         *
         */
        [[nodiscard]] inline bool gpio_init()
        {
            if constexpr (skHasPin)
            {
                return m_pin.init(GpioAnalogInConfig{});
            }
            return true;
        }
    };

    // -----------------------------------------------------------------------------
    // DEVICE ALIASES
    // -----------------------------------------------------------------------------

#define DECLARE_ADC_CHANNEL_DEVICE_ALIASES(controller_id_num)                                        \
    using ADC##controller_id_num##Channel0Device =                                                   \
        AdcChannelDevice<static_cast<AdcPeripheralId>(controller_id_num), AdcChannelId::kChannel0>;  \
    using ADC##controller_id_num##Channel1Device =                                                   \
        AdcChannelDevice<static_cast<AdcPeripheralId>(controller_id_num), AdcChannelId::kChannel1>;  \
    using ADC##controller_id_num##Channel2Device =                                                   \
        AdcChannelDevice<static_cast<AdcPeripheralId>(controller_id_num), AdcChannelId::kChannel2>;  \
    using ADC##controller_id_num##Channel3Device =                                                   \
        AdcChannelDevice<static_cast<AdcPeripheralId>(controller_id_num), AdcChannelId::kChannel3>;  \
    using ADC##controller_id_num##Channel4Device =                                                   \
        AdcChannelDevice<static_cast<AdcPeripheralId>(controller_id_num), AdcChannelId::kChannel4>;  \
    using ADC##controller_id_num##Channel5Device =                                                   \
        AdcChannelDevice<static_cast<AdcPeripheralId>(controller_id_num), AdcChannelId::kChannel5>;  \
    using ADC##controller_id_num##Channel6Device =                                                   \
        AdcChannelDevice<static_cast<AdcPeripheralId>(controller_id_num), AdcChannelId::kChannel6>;  \
    using ADC##controller_id_num##Channel7Device =                                                   \
        AdcChannelDevice<static_cast<AdcPeripheralId>(controller_id_num), AdcChannelId::kChannel7>;  \
    using ADC##controller_id_num##Channel8Device =                                                   \
        AdcChannelDevice<static_cast<AdcPeripheralId>(controller_id_num), AdcChannelId::kChannel8>;  \
    using ADC##controller_id_num##Channel9Device =                                                   \
        AdcChannelDevice<static_cast<AdcPeripheralId>(controller_id_num), AdcChannelId::kChannel9>;  \
    using ADC##controller_id_num##Channel10Device =                                                  \
        AdcChannelDevice<static_cast<AdcPeripheralId>(controller_id_num), AdcChannelId::kChannel10>; \
    using ADC##controller_id_num##Channel11Device =                                                  \
        AdcChannelDevice<static_cast<AdcPeripheralId>(controller_id_num), AdcChannelId::kChannel11>; \
    using ADC##controller_id_num##Channel12Device =                                                  \
        AdcChannelDevice<static_cast<AdcPeripheralId>(controller_id_num), AdcChannelId::kChannel12>; \
    using ADC##controller_id_num##Channel13Device =                                                  \
        AdcChannelDevice<static_cast<AdcPeripheralId>(controller_id_num), AdcChannelId::kChannel13>; \
    using ADC##controller_id_num##Channel14Device =                                                  \
        AdcChannelDevice<static_cast<AdcPeripheralId>(controller_id_num), AdcChannelId::kChannel14>; \
    using ADC##controller_id_num##Channel15Device =                                                  \
        AdcChannelDevice<static_cast<AdcPeripheralId>(controller_id_num), AdcChannelId::kChannel15>; \
    using ADC##controller_id_num##Channel16Device =                                                  \
        AdcChannelDevice<static_cast<AdcPeripheralId>(controller_id_num), AdcChannelId::kChannel16>; \
    using ADC##controller_id_num##Channel17Device =                                                  \
        AdcChannelDevice<static_cast<AdcPeripheralId>(controller_id_num), AdcChannelId::kChannel17>; \
    using ADC##controller_id_num##Channel18Device =                                                  \
        AdcChannelDevice<static_cast<AdcPeripheralId>(controller_id_num), AdcChannelId::kChannel18>;

    DECLARE_ADC_CHANNEL_DEVICE_ALIASES(1);
    DECLARE_ADC_CHANNEL_DEVICE_ALIASES(2);
    DECLARE_ADC_CHANNEL_DEVICE_ALIASES(3);
    DECLARE_ADC_CHANNEL_DEVICE_ALIASES(4);
    DECLARE_ADC_CHANNEL_DEVICE_ALIASES(5);

#undef DECLARE_ADC_CHANNEL_DEVICE_ALIASES

    // ============================================================================
    // ADC INJECTED CHANNEL (UNIQUE DEVICE)
    // ============================================================================

    template <AdcPeripheralId tkPeripheralId, AdcChannelId tkChannelId, AdcInjectChannelRank tkRank>
    class AdcInjectChannelDevice
    {
    public:
        struct Descriptor : public UniqueDeviceDescriptor
        {
        };

        using ChannelT      = AdcChannelDevice<tkPeripheralId, tkChannelId>;
        using InjectDevices = TypeList<ChannelT>;

        static constexpr AdcPeripheralId      skPeripheralId = tkPeripheralId;
        static constexpr AdcChannelId         skChannelId    = tkChannelId;
        static constexpr AdcChannelGroup      skGroup        = AdcChannelGroup::kInject;
        static constexpr AdcInjectChannelRank skRank         = tkRank;

    private:
        [[no_unique_address]] DeviceRef<ChannelT> m_channel;

    public:
        explicit AdcInjectChannelDevice(DeviceRef<ChannelT> channel) : m_channel(std::move(channel))
        {
        }

        // --- Initialization ---
        /**
         * @brief Initialize Inject Channel.
         *
         */
        [[nodiscard]] bool init(const AdcChannelConfig& config)
        {
            return m_channel->template init_as_inject<skRank>(config);
        }

        // --- Accessors ---
        /**
         * @brief Read data from inject channel.
         *
         * @return AdcValue Raw ADC value.
         */
        [[nodiscard]] AdcValue read() const
        {
            return m_channel->template read_inject<skRank>();
        }

        /**
         * @brief Read voltage from inject channel.
         *
         * @return float Voltage in Volts.
         */
        [[nodiscard]] float read_voltage() const
        {
            return m_channel->template read_inject_voltage<skRank>();
        }

        /**
         * @brief Read normalized value from inject channel.
         *
         * @return float Normalized value (0.0 to 1.0).
         */
        [[nodiscard]] float read_normalized() const
        {
            return m_channel->template read_inject_normalized<skRank>();
        }
    };

    // -----------------------------------------------------------------------------
    // DEVICE ALIASES
    // -----------------------------------------------------------------------------

    template <AdcPeripheralId tkPeripheralId, AdcChannelId tkChannelId>
    using AdcInjectChannelRank1Device =
        AdcInjectChannelDevice<tkPeripheralId, tkChannelId, AdcInjectChannelRank::kRank1>;

    template <AdcPeripheralId tkPeripheralId, AdcChannelId tkChannelId>
    using AdcInjectChannelRank2Device =
        AdcInjectChannelDevice<tkPeripheralId, tkChannelId, AdcInjectChannelRank::kRank2>;
    template <AdcPeripheralId tkPeripheralId, AdcChannelId tkChannelId>
    using AdcInjectChannelRank3Device =
        AdcInjectChannelDevice<tkPeripheralId, tkChannelId, AdcInjectChannelRank::kRank3>;

    template <AdcPeripheralId tkPeripheralId, AdcChannelId tkChannelId>
    using AdcInjectChannelRank4Device =
        AdcInjectChannelDevice<tkPeripheralId, tkChannelId, AdcInjectChannelRank::kRank4>;

    namespace detail
    {
        // Helper to unpack the Index Sequence alongside the Channel Pack
        template <AdcPeripheralId tkPeripheralId, typename TIndexSeq, AdcChannelId... tkChannelIds>
        struct MakeInjectSequenceImpl;

        template <AdcPeripheralId tkPeripheralId, size_t... Is, AdcChannelId... tkChannelIds>
        struct MakeInjectSequenceImpl<tkPeripheralId, std::index_sequence<Is...>, tkChannelIds...>
        {
            // Expand both packs: Channel[i] paired with Rank[i + 1]
            using type = TypeList<
                AdcInjectChannelDevice<tkPeripheralId, tkChannelIds, static_cast<AdcInjectChannelRank>(Is + 1)>...>;
        };
    }  // namespace detail

    template <AdcPeripheralId tkPeripheralId, AdcChannelId... tkChannelIds>
        requires(sizeof...(tkChannelIds) <= kAdcMaxInjChannels)
    using AdcInjectChannelDeviceSequence =
        typename detail::MakeInjectSequenceImpl<tkPeripheralId,
                                                std::make_index_sequence<sizeof...(tkChannelIds)>,  // Generates 0,
                                                                                                    // 1, 2...
                                                tkChannelIds...>::type;

#define DECLARE_ADC_INJECT_CHANNEL_DEVICE_ALIASES_FOR_RANK(controller_id_num, rank_num) \
    using ADC##controller_id_num##InjectChannel0Rank##rank_num##Device =                \
        AdcInjectChannelDevice<static_cast<AdcPeripheralId>(controller_id_num),         \
                               AdcChannelId::kChannel0,                                 \
                               static_cast<AdcInjectChannelRank>(rank_num)>;            \
    using ADC##controller_id_num##InjectChannel1Rank##rank_num##Device =                \
        AdcInjectChannelDevice<static_cast<AdcPeripheralId>(controller_id_num),         \
                               AdcChannelId::kChannel1,                                 \
                               static_cast<AdcInjectChannelRank>(rank_num)>;            \
    using ADC##controller_id_num##InjectChannel2Rank##rank_num##Device =                \
        AdcInjectChannelDevice<static_cast<AdcPeripheralId>(controller_id_num),         \
                               AdcChannelId::kChannel2,                                 \
                               static_cast<AdcInjectChannelRank>(rank_num)>;            \
    using ADC##controller_id_num##InjectChannel3Rank##rank_num##Device =                \
        AdcInjectChannelDevice<static_cast<AdcPeripheralId>(controller_id_num),         \
                               AdcChannelId::kChannel3,                                 \
                               static_cast<AdcInjectChannelRank>(rank_num)>;            \
    using ADC##controller_id_num##InjectChannel4Rank##rank_num##Device =                \
        AdcInjectChannelDevice<static_cast<AdcPeripheralId>(controller_id_num),         \
                               AdcChannelId::kChannel4,                                 \
                               static_cast<AdcInjectChannelRank>(rank_num)>;            \
    using ADC##controller_id_num##InjectChannel5Rank##rank_num##Device =                \
        AdcInjectChannelDevice<static_cast<AdcPeripheralId>(controller_id_num),         \
                               AdcChannelId::kChannel5,                                 \
                               static_cast<AdcInjectChannelRank>(rank_num)>;            \
    using ADC##controller_id_num##InjectChannel6Rank##rank_num##Device =                \
        AdcInjectChannelDevice<static_cast<AdcPeripheralId>(controller_id_num),         \
                               AdcChannelId::kChannel6,                                 \
                               static_cast<AdcInjectChannelRank>(rank_num)>;            \
    using ADC##controller_id_num##InjectChannel7Rank##rank_num##Device =                \
        AdcInjectChannelDevice<static_cast<AdcPeripheralId>(controller_id_num),         \
                               AdcChannelId::kChannel7,                                 \
                               static_cast<AdcInjectChannelRank>(rank_num)>;            \
    using ADC##controller_id_num##InjectChannel8Rank##rank_num##Device =                \
        AdcInjectChannelDevice<static_cast<AdcPeripheralId>(controller_id_num),         \
                               AdcChannelId::kChannel8,                                 \
                               static_cast<AdcInjectChannelRank>(rank_num)>;            \
    using ADC##controller_id_num##InjectChannel9Rank##rank_num##Device =                \
        AdcInjectChannelDevice<static_cast<AdcPeripheralId>(controller_id_num),         \
                               AdcChannelId::kChannel9,                                 \
                               static_cast<AdcInjectChannelRank>(rank_num)>;            \
    using ADC##controller_id_num##InjectChannel10Rank##rank_num##Device =               \
        AdcInjectChannelDevice<static_cast<AdcPeripheralId>(controller_id_num),         \
                               AdcChannelId::kChannel10,                                \
                               static_cast<AdcInjectChannelRank>(rank_num)>;            \
    using ADC##controller_id_num##InjectChannel11Rank##rank_num##Device =               \
        AdcInjectChannelDevice<static_cast<AdcPeripheralId>(controller_id_num),         \
                               AdcChannelId::kChannel11,                                \
                               static_cast<AdcInjectChannelRank>(rank_num)>;            \
    using ADC##controller_id_num##InjectChannel12Rank##rank_num##Device =               \
        AdcInjectChannelDevice<static_cast<AdcPeripheralId>(controller_id_num),         \
                               AdcChannelId::kChannel12,                                \
                               static_cast<AdcInjectChannelRank>(rank_num)>;            \
    using ADC##controller_id_num##InjectChannel13Rank##rank_num##Device =               \
        AdcInjectChannelDevice<static_cast<AdcPeripheralId>(controller_id_num),         \
                               AdcChannelId::kChannel13,                                \
                               static_cast<AdcInjectChannelRank>(rank_num)>;            \
    using ADC##controller_id_num##InjectChannel14Rank##rank_num##Device =               \
        AdcInjectChannelDevice<static_cast<AdcPeripheralId>(controller_id_num),         \
                               AdcChannelId::kChannel14,                                \
                               static_cast<AdcInjectChannelRank>(rank_num)>;            \
    using ADC##controller_id_num##InjectChannel15Rank##rank_num##Device =               \
        AdcInjectChannelDevice<static_cast<AdcPeripheralId>(controller_id_num),         \
                               AdcChannelId::kChannel15,                                \
                               static_cast<AdcInjectChannelRank>(rank_num)>;            \
    using ADC##controller_id_num##InjectChannel16Rank##rank_num##Device =               \
        AdcInjectChannelDevice<static_cast<AdcPeripheralId>(controller_id_num),         \
                               AdcChannelId::kChannel16,                                \
                               static_cast<AdcInjectChannelRank>(rank_num)>;            \
    using ADC##controller_id_num##InjectChannel17Rank##rank_num##Device =               \
        AdcInjectChannelDevice<static_cast<AdcPeripheralId>(controller_id_num),         \
                               AdcChannelId::kChannel17,                                \
                               static_cast<AdcInjectChannelRank>(rank_num)>;            \
    using ADC##controller_id_num##InjectChannel18Rank##rank_num##Device =               \
        AdcInjectChannelDevice<static_cast<AdcPeripheralId>(controller_id_num),         \
                               AdcChannelId::kChannel18,                                \
                               static_cast<AdcInjectChannelRank>(rank_num)>;

#define DECLARE_ADC_INJECT_CHANNEL_DEVICE_ALIASES(controller_id_num)          \
    DECLARE_ADC_INJECT_CHANNEL_DEVICE_ALIASES_FOR_RANK(controller_id_num, 1); \
    DECLARE_ADC_INJECT_CHANNEL_DEVICE_ALIASES_FOR_RANK(controller_id_num, 2); \
    DECLARE_ADC_INJECT_CHANNEL_DEVICE_ALIASES_FOR_RANK(controller_id_num, 3); \
    DECLARE_ADC_INJECT_CHANNEL_DEVICE_ALIASES_FOR_RANK(controller_id_num, 4);

    DECLARE_ADC_INJECT_CHANNEL_DEVICE_ALIASES(1);
    DECLARE_ADC_INJECT_CHANNEL_DEVICE_ALIASES(2);
    DECLARE_ADC_INJECT_CHANNEL_DEVICE_ALIASES(3);
    DECLARE_ADC_INJECT_CHANNEL_DEVICE_ALIASES(4);

#undef DECLARE_ADC_INJECT_CHANNEL_DEVICE_ALIASES_FOR_RANK
#undef DECLARE_ADC_INJECT_CHANNEL_DEVICE_ALIASES

    // ============================================================================
    // ADC REGULAR CHANNEL (UNIQUE DEVICE)
    // ============================================================================

    template <AdcPeripheralId tkPeripheralId, AdcChannelId tkChannelId, AdcRegularChannelRank tkRank>
    class AdcRegularChannelDevice
    {
    public:
        struct Descriptor : public UniqueDeviceDescriptor
        {
        };

        using ChannelT                                        = AdcChannelDevice<tkPeripheralId, tkChannelId>;
        using InjectDevices                                   = TypeList<ChannelT>;
        static constexpr AdcPeripheralId       skPeripheralId = tkPeripheralId;
        static constexpr AdcChannelId          skChannelId    = tkChannelId;
        static constexpr AdcChannelGroup       skGroup        = AdcChannelGroup::kRegular;
        static constexpr AdcRegularChannelRank skRank         = tkRank;

    private:
        [[no_unique_address]] DeviceRef<ChannelT> m_channel;

    public:
        explicit AdcRegularChannelDevice(DeviceRef<ChannelT>&& channel) : m_channel(std::move(channel))
        {
        }

        // --- Initialization ---
        /**
         * @brief Initialize Regular Channel.
         *
         */
        [[nodiscard]] bool init(const AdcChannelConfig& config)
        {
            return m_channel->template init_as_regular<skRank>(config);
        }

        // --- Accessors ---
        /**
         * @brief Read data from regular channel.
         *
         * @return AdcValue Raw ADC value.
         */
        [[nodiscard]] AdcValue read() const
        {
            return m_channel->template read_regular<skRank>();
        }

        /**
         * @brief Read voltage from regular channel.
         *
         * @return float Voltage in Volts.
         */
        [[nodiscard]] float read_voltage() const
        {
            return m_channel->template read_regular_voltage<skRank>();
        }

        /**
         * @brief Read normalized value from regular channel.
         *
         * @return float Normalized value (0.0 to 1.0).
         */
        [[nodiscard]] float read_normalized() const
        {
            return m_channel->template read_regular_normalized<skRank>();
        }
    };

    // -----------------------------------------------------------------------------
    // DEVICE ALIASES
    // -----------------------------------------------------------------------------

    template <AdcPeripheralId tkPeripheralId, AdcChannelId tkChannelId>
    using AdcRegularChannelRank1Device =
        AdcRegularChannelDevice<tkPeripheralId, tkChannelId, AdcRegularChannelRank::kRank1>;

    template <AdcPeripheralId tkPeripheralId, AdcChannelId tkChannelId>
    using AdcRegularChannelRank2Device =
        AdcRegularChannelDevice<tkPeripheralId, tkChannelId, AdcRegularChannelRank::kRank2>;

    template <AdcPeripheralId tkPeripheralId, AdcChannelId tkChannelId>
    using AdcRegularChannelRank3Device =
        AdcRegularChannelDevice<tkPeripheralId, tkChannelId, AdcRegularChannelRank::kRank3>;

    template <AdcPeripheralId tkPeripheralId, AdcChannelId tkChannelId>
    using AdcRegularChannelRank4Device =
        AdcRegularChannelDevice<tkPeripheralId, tkChannelId, AdcRegularChannelRank::kRank4>;

    template <AdcPeripheralId tkPeripheralId, AdcChannelId tkChannelId>
    using AdcRegularChannelRank5Device =
        AdcRegularChannelDevice<tkPeripheralId, tkChannelId, AdcRegularChannelRank::kRank5>;

    template <AdcPeripheralId tkPeripheralId, AdcChannelId tkChannelId>
    using AdcRegularChannelRank6Device =
        AdcRegularChannelDevice<tkPeripheralId, tkChannelId, AdcRegularChannelRank::kRank6>;

    template <AdcPeripheralId tkPeripheralId, AdcChannelId tkChannelId>
    using AdcRegularChannelRank7Device =
        AdcRegularChannelDevice<tkPeripheralId, tkChannelId, AdcRegularChannelRank::kRank7>;

    template <AdcPeripheralId tkPeripheralId, AdcChannelId tkChannelId>
    using AdcRegularChannelRank8Device =
        AdcRegularChannelDevice<tkPeripheralId, tkChannelId, AdcRegularChannelRank::kRank8>;

    template <AdcPeripheralId tkPeripheralId, AdcChannelId tkChannelId>
    using AdcRegularChannelRank9Device =
        AdcRegularChannelDevice<tkPeripheralId, tkChannelId, AdcRegularChannelRank::kRank9>;

    template <AdcPeripheralId tkPeripheralId, AdcChannelId tkChannelId>
    using AdcRegularChannelRank10Device =
        AdcRegularChannelDevice<tkPeripheralId, tkChannelId, AdcRegularChannelRank::kRank10>;

    template <AdcPeripheralId tkPeripheralId, AdcChannelId tkChannelId>
    using AdcRegularChannelRank11Device =
        AdcRegularChannelDevice<tkPeripheralId, tkChannelId, AdcRegularChannelRank::kRank11>;

    template <AdcPeripheralId tkPeripheralId, AdcChannelId tkChannelId>
    using AdcRegularChannelRank12Device =
        AdcRegularChannelDevice<tkPeripheralId, tkChannelId, AdcRegularChannelRank::kRank12>;

    template <AdcPeripheralId tkPeripheralId, AdcChannelId tkChannelId>
    using AdcRegularChannelRank13Device =
        AdcRegularChannelDevice<tkPeripheralId, tkChannelId, AdcRegularChannelRank::kRank13>;

    template <AdcPeripheralId tkPeripheralId, AdcChannelId tkChannelId>
    using AdcRegularChannelRank14Device =
        AdcRegularChannelDevice<tkPeripheralId, tkChannelId, AdcRegularChannelRank::kRank14>;

    template <AdcPeripheralId tkPeripheralId, AdcChannelId tkChannelId>
    using AdcRegularChannelRank15Device =
        AdcRegularChannelDevice<tkPeripheralId, tkChannelId, AdcRegularChannelRank::kRank15>;

    template <AdcPeripheralId tkPeripheralId, AdcChannelId tkChannelId>
    using AdcRegularChannelRank16Device =
        AdcRegularChannelDevice<tkPeripheralId, tkChannelId, AdcRegularChannelRank::kRank16>;

    namespace detail
    {
        // Helper to unpack the Index Sequence alongside the Channel Pack
        template <AdcPeripheralId tkPeripheralId, typename TIndexSeq, AdcChannelId... tkChannelIds>
        struct MakeRegularSequenceImpl;

        template <AdcPeripheralId tkPeripheralId, size_t... Is, AdcChannelId... tkChannelIds>
        struct MakeRegularSequenceImpl<tkPeripheralId, std::index_sequence<Is...>, tkChannelIds...>
        {
            // Expand both packs: Channel[i] paired with Rank[i + 1]
            using type = TypeList<
                AdcRegularChannelDevice<tkPeripheralId, tkChannelIds, static_cast<AdcRegularChannelRank>(Is + 1)>...>;
        };
    }  // namespace detail

    template <AdcPeripheralId tkPeripheralId, AdcChannelId... tkChannelIds>
        requires(sizeof...(tkChannelIds) <= kAdcMaxRegChannels)
    using AdcRegularChannelDeviceSequence =
        typename detail::MakeRegularSequenceImpl<tkPeripheralId,
                                                 std::make_index_sequence<sizeof...(tkChannelIds)>,  // Generates 0,
                                                                                                     // 1, 2...
                                                 tkChannelIds...>::type;

#define DECLARE_ADC_REGULAR_CHANNEL_DEVICE_ALIASES_FOR_RANK(controller_id_num, rank_num) \
    using ADC##controller_id_num##RegularChannel0Rank##rank_num##Device =                \
        AdcRegularChannelDevice<static_cast<AdcPeripheralId>(controller_id_num),         \
                                AdcChannelId::kChannel0,                                 \
                                static_cast<AdcRegularChannelRank>(rank_num)>;           \
    using ADC##controller_id_num##RegularChannel1Rank##rank_num##Device =                \
        AdcRegularChannelDevice<static_cast<AdcPeripheralId>(controller_id_num),         \
                                AdcChannelId::kChannel1,                                 \
                                static_cast<AdcRegularChannelRank>(rank_num)>;           \
    using ADC##controller_id_num##RegularChannel2Rank##rank_num##Device =                \
        AdcRegularChannelDevice<static_cast<AdcPeripheralId>(controller_id_num),         \
                                AdcChannelId::kChannel2,                                 \
                                static_cast<AdcRegularChannelRank>(rank_num)>;           \
    using ADC##controller_id_num##RegularChannel3Rank##rank_num##Device =                \
        AdcRegularChannelDevice<static_cast<AdcPeripheralId>(controller_id_num),         \
                                AdcChannelId::kChannel3,                                 \
                                static_cast<AdcRegularChannelRank>(rank_num)>;           \
    using ADC##controller_id_num##RegularChannel4Rank##rank_num##Device =                \
        AdcRegularChannelDevice<static_cast<AdcPeripheralId>(controller_id_num),         \
                                AdcChannelId::kChannel4,                                 \
                                static_cast<AdcRegularChannelRank>(rank_num)>;           \
    using ADC##controller_id_num##RegularChannel5Rank##rank_num##Device =                \
        AdcRegularChannelDevice<static_cast<AdcPeripheralId>(controller_id_num),         \
                                AdcChannelId::kChannel5,                                 \
                                static_cast<AdcRegularChannelRank>(rank_num)>;           \
    using ADC##controller_id_num##RegularChannel6Rank##rank_num##Device =                \
        AdcRegularChannelDevice<static_cast<AdcPeripheralId>(controller_id_num),         \
                                AdcChannelId::kChannel6,                                 \
                                static_cast<AdcRegularChannelRank>(rank_num)>;           \
    using ADC##controller_id_num##RegularChannel7Rank##rank_num##Device =                \
        AdcRegularChannelDevice<static_cast<AdcPeripheralId>(controller_id_num),         \
                                AdcChannelId::kChannel7,                                 \
                                static_cast<AdcRegularChannelRank>(rank_num)>;           \
    using ADC##controller_id_num##RegularChannel8Rank##rank_num##Device =                \
        AdcRegularChannelDevice<static_cast<AdcPeripheralId>(controller_id_num),         \
                                AdcChannelId::kChannel8,                                 \
                                static_cast<AdcRegularChannelRank>(rank_num)>;           \
    using ADC##controller_id_num##RegularChannel9Rank##rank_num##Device =                \
        AdcRegularChannelDevice<static_cast<AdcPeripheralId>(controller_id_num),         \
                                AdcChannelId::kChannel9,                                 \
                                static_cast<AdcRegularChannelRank>(rank_num)>;           \
    using ADC##controller_id_num##RegularChannel10Rank##rank_num##Device =               \
        AdcRegularChannelDevice<static_cast<AdcPeripheralId>(controller_id_num),         \
                                AdcChannelId::kChannel10,                                \
                                static_cast<AdcRegularChannelRank>(rank_num)>;           \
    using ADC##controller_id_num##RegularChannel11Rank##rank_num##Device =               \
        AdcRegularChannelDevice<static_cast<AdcPeripheralId>(controller_id_num),         \
                                AdcChannelId::kChannel11,                                \
                                static_cast<AdcRegularChannelRank>(rank_num)>;           \
    using ADC##controller_id_num##RegularChannel12Rank##rank_num##Device =               \
        AdcRegularChannelDevice<static_cast<AdcPeripheralId>(controller_id_num),         \
                                AdcChannelId::kChannel12,                                \
                                static_cast<AdcRegularChannelRank>(rank_num)>;           \
    using ADC##controller_id_num##RegularChannel13Rank##rank_num##Device =               \
        AdcRegularChannelDevice<static_cast<AdcPeripheralId>(controller_id_num),         \
                                AdcChannelId::kChannel13,                                \
                                static_cast<AdcRegularChannelRank>(rank_num)>;           \
    using ADC##controller_id_num##RegularChannel14Rank##rank_num##Device =               \
        AdcRegularChannelDevice<static_cast<AdcPeripheralId>(controller_id_num),         \
                                AdcChannelId::kChannel14,                                \
                                static_cast<AdcRegularChannelRank>(rank_num)>;           \
    using ADC##controller_id_num##RegularChannel15Rank##rank_num##Device =               \
        AdcRegularChannelDevice<static_cast<AdcPeripheralId>(controller_id_num),         \
                                AdcChannelId::kChannel15,                                \
                                static_cast<AdcRegularChannelRank>(rank_num)>;           \
    using ADC##controller_id_num##RegularChannel16Rank##rank_num##Device =               \
        AdcRegularChannelDevice<static_cast<AdcPeripheralId>(controller_id_num),         \
                                AdcChannelId::kChannel16,                                \
                                static_cast<AdcRegularChannelRank>(rank_num)>;           \
    using ADC##controller_id_num##RegularChannel17Rank##rank_num##Device =               \
        AdcRegularChannelDevice<static_cast<AdcPeripheralId>(controller_id_num),         \
                                AdcChannelId::kChannel17,                                \
                                static_cast<AdcRegularChannelRank>(rank_num)>;           \
    using ADC##controller_id_num##RegularChannel18Rank##rank_num##Device =               \
        AdcRegularChannelDevice<static_cast<AdcPeripheralId>(controller_id_num),         \
                                AdcChannelId::kChannel18,                                \
                                static_cast<AdcRegularChannelRank>(rank_num)>;

#define DECLARE_ADC_REGULAR_CHANNEL_DEVICE_ALIASES(controller_id_num)           \
    DECLARE_ADC_REGULAR_CHANNEL_DEVICE_ALIASES_FOR_RANK(controller_id_num, 1);  \
    DECLARE_ADC_REGULAR_CHANNEL_DEVICE_ALIASES_FOR_RANK(controller_id_num, 2);  \
    DECLARE_ADC_REGULAR_CHANNEL_DEVICE_ALIASES_FOR_RANK(controller_id_num, 3);  \
    DECLARE_ADC_REGULAR_CHANNEL_DEVICE_ALIASES_FOR_RANK(controller_id_num, 4);  \
    DECLARE_ADC_REGULAR_CHANNEL_DEVICE_ALIASES_FOR_RANK(controller_id_num, 5);  \
    DECLARE_ADC_REGULAR_CHANNEL_DEVICE_ALIASES_FOR_RANK(controller_id_num, 6);  \
    DECLARE_ADC_REGULAR_CHANNEL_DEVICE_ALIASES_FOR_RANK(controller_id_num, 7);  \
    DECLARE_ADC_REGULAR_CHANNEL_DEVICE_ALIASES_FOR_RANK(controller_id_num, 8);  \
    DECLARE_ADC_REGULAR_CHANNEL_DEVICE_ALIASES_FOR_RANK(controller_id_num, 9);  \
    DECLARE_ADC_REGULAR_CHANNEL_DEVICE_ALIASES_FOR_RANK(controller_id_num, 10); \
    DECLARE_ADC_REGULAR_CHANNEL_DEVICE_ALIASES_FOR_RANK(controller_id_num, 11); \
    DECLARE_ADC_REGULAR_CHANNEL_DEVICE_ALIASES_FOR_RANK(controller_id_num, 12); \
    DECLARE_ADC_REGULAR_CHANNEL_DEVICE_ALIASES_FOR_RANK(controller_id_num, 13); \
    DECLARE_ADC_REGULAR_CHANNEL_DEVICE_ALIASES_FOR_RANK(controller_id_num, 14); \
    DECLARE_ADC_REGULAR_CHANNEL_DEVICE_ALIASES_FOR_RANK(controller_id_num, 15); \
    DECLARE_ADC_REGULAR_CHANNEL_DEVICE_ALIASES_FOR_RANK(controller_id_num, 16);

    DECLARE_ADC_REGULAR_CHANNEL_DEVICE_ALIASES(1);
    DECLARE_ADC_REGULAR_CHANNEL_DEVICE_ALIASES(2);
    DECLARE_ADC_REGULAR_CHANNEL_DEVICE_ALIASES(3);
    DECLARE_ADC_REGULAR_CHANNEL_DEVICE_ALIASES(4);
    DECLARE_ADC_REGULAR_CHANNEL_DEVICE_ALIASES(5);

#undef DECLARE_ADC_REGULAR_CHANNEL_DEVICE_ALIASES_FOR_RANK
#undef DECLARE_ADC_REGULAR_CHANNEL_DEVICE_ALIASES

}  // namespace valle::platform
