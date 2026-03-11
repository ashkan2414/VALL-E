#pragma once

#include <algorithm>
#include <array>
#include <cstdint>
#include <optional>
#include <variant>

#include "valle/platform/core.hpp"
#include "valle/platform/devices/adc_clk.hpp"
#include "valle/platform/devices/dma.hpp"
#include "valle/platform/drivers/gpio/analog_in.hpp"
#include "valle/platform/hardware/adc.hpp"


namespace valle::platform
{
    // ============================================================================
    // FORWARD DECLARATIONS
    // ============================================================================
    class ADCRootDevice;

    template <ADCControllerID tkControllerID>
        requires(kValidADCControllerID<tkControllerID>)
    class ADCControllerDevice;

    template <ADCControllerID tkControllerID, ADCChannelID tkChannelID>
        requires(kValidADCChannelID<tkControllerID, tkChannelID>)
    class ADCChannelDevice;

    template <ADCControllerID tkControllerID, ADCChannelID tkChannelID, ADCInjectChannelRank tkRank>
        requires(kValidADCChannelID<tkControllerID, tkChannelID> && kValidADCInjectRank<tkRank>)
    class ADCInjectChannelDevice;

    template <ADCControllerID tkControllerID, ADCChannelID tkChannelID, ADCRegularChannelRank tkRank>
        requires(kValidADCChannelID<tkControllerID, tkChannelID> && kValidADCRegularRank<tkRank>)
    class ADCRegularChannelDevice;

    // ============================================================================
    // Base ADC DEVICE (INTERFACE DEVICE)
    // ============================================================================
    /**
     * @brief ADC Device (Interface Device), represents the ADC peripheral family.
     *
     */
    class ADCRootDevice
    {
    public:
        struct Descriptor : public InterfaceDeviceDescriptor
        {
            using Children = DeviceTreeList<ADCControllerDevice<1>,
                                            ADCControllerDevice<2>,
                                            ADCControllerDevice<3>,
                                            ADCControllerDevice<4>,
                                            ADCControllerDevice<5>>;
        };
    };

    // ============================================================================
    // ADC CONTROLLER DEVICE (SHARED DEVICE)
    // ============================================================================

    // -----------------------------------------------------------------------------
    // DEVICE TRAITS
    // -----------------------------------------------------------------------------
    template <ADCControllerID tkControllerID>
    struct ADCControllerDeviceTraits;

    template <>
    struct ADCControllerDeviceTraits<1>
    {
        using ADCClockDeviceT = ADC12ClockDevice;
    };

    template <>
    struct ADCControllerDeviceTraits<2>
    {
        using ADCClockDeviceT = ADC12ClockDevice;
    };

    template <>
    struct ADCControllerDeviceTraits<3>
    {
        using ADCClockDeviceT = ADC345ClockDevice;
    };

    template <>
    struct ADCControllerDeviceTraits<4>
    {
        using ADCClockDeviceT = ADC345ClockDevice;
    };

    template <>
    struct ADCControllerDeviceTraits<5>  // NOLINT(readability-magic-numbers)
    {
        using ADCClockDeviceT = ADC345ClockDevice;
    };

    // -----------------------------------------------------------------------------
    // COMPILE TIME CONFIGURATIONS
    // -----------------------------------------------------------------------------
    struct ADCControllerCTDefaultConfig
    {
        using DMAChannelT = DMANullChannelDevice;
    };

    template <typename T>
    concept CValidADCControllerCTConfig = requires {
        typename T::DMAChannelT;
    } && (CNullDMAChannel<typename T::DMAChannelT> || CDMAChannelDevice<typename T::DMAChannelT>);

    template <ADCControllerID tkControllerID>
        requires(kValidADCControllerID<tkControllerID>)
    struct ADCControllerCTConfigTraits
    {
        static constexpr auto skConfig = ADCControllerCTDefaultConfig{};
    };

#define VALLE_DEFINE_ADC_CONTROLLER_CT_CONFIG(tkControllerID, config)                                           \
    namespace valle::platform                                                                                   \
    {                                                                                                           \
        template <>                                                                                             \
        struct ADCControllerCTConfigTraits<(tkControllerID)>                                                    \
        {                                                                                                       \
            static constexpr auto skConfig = (config);                                                          \
            static_assert(CValidADCControllerCTConfig<decltype(skConfig)>, "Invalid ADC Controller CT Config"); \
        };                                                                                                      \
    }

    // -----------------------------------------------------------------------------
    // CONFIGURATIONS
    // -----------------------------------------------------------------------------

    /**
     * @brief Configuration for ADC Injected Group
     *
     */
    struct ADCInjectGroupConfig
    {
        // What triggers the inject group (software or external)
        ADCInjectGroupTriggerSource trigger_source = ADCInjectGroupTriggerSource::kSoftware;

        // What edge to trigger on (only for external triggers)
        ADCInjectGroupTriggerEdge trigger_edge = ADCInjectGroupTriggerEdge::kRising;

        // RM0440 Section 21.4.16: Injected group can auto-trigger after regular group
        bool auto_trigger_from_regular = false;
    };

    /**
     * @brief Configuration for ADC Regular Group DMA Transfer
     *
     */
    struct ADCRegularGroupDMAConfig
    {
        /// DMA Channel Priority
        DMAPriority priority = DMAPriority::kMedium;

        /// True = Continuous/Circular, False = One-Shot/Normal
        bool circular_mode = true;

        /// Optional Interrupt Configuration
        std::optional<DMAChannelInterruptConfig> interrupts = std::nullopt;
    };

    /**
     * @brief Configuration for ADC Regular Group (Background/DMA).
     *
     */
    struct ADCRegularGroupConfig
    {
        /// Source of trigger (Software/External)
        ADCRegularGroupTriggerSource trigger_source = ADCRegularGroupTriggerSource::kSoftware;

        /// What edge causes trigger (Rising/Falling/Both)
        ADCRegularGroupTriggerEdge trigger_edge = ADCRegularGroupTriggerEdge::kRising;

        /// DMA Transfer configuration
        ADCRegularGroupDMAConfig dma = ADCRegularGroupDMAConfig{};

        /// Overrun behavior: Preserve vs Overwrite
        ADCRegularGroupOverrunBehavior overrun = ADCRegularGroupOverrunBehavior::kOverwrite;

        /// Conversion mode: Single Shot vs Continuous
        ADCRegularGroupConversionMode conversion_mode = ADCRegularGroupConversionMode::kSingleShot;

        /// Oversampling mode: Continuous vs discontinuous
        ADCRegularGroupOversamplingMode oversampling_mode = ADCRegularGroupOversamplingMode::kDiscontinuous;
    };

    /**
     * @brief Configuration for ADC Oversampling (Hardware Averaging).
     *
     */
    struct ADCOversamplingConfig
    {
        // How many times to sample (2x to 256x)
        ADCOversamplingRatio ratio = ADCOversamplingRatio::k2x;

        // Right shift bits (0 to 8)
        ADCOversamplingShift shift = ADCOversamplingShift::kNone;

        // Oversampling behavior when both groups are selected
        ADCOversamplingScope scope = ADCOversamplingScope::kDisable;
    };

    /**
     * @brief Configuration for the ADC Peripheral (Global).
     */
    struct ADCControllerConfig
    {
        // --- Core Settings ---
        // Sampling resolution (12, 10, 8, 6 bits)
        ADCResolution resolution = ADCResolution::k12Bit;

        // Right/Left data alignment
        ADCDataAlignment data_alignment = ADCDataAlignment::kRight;

        // Low power mode
        ADCLowPowerMode low_power = ADCLowPowerMode::kNone;

        // Inject Group (High Priority/Interrupt)
        ADCInjectGroupConfig inj = ADCInjectGroupConfig{};

        // Regular Group (Background/DMA)
        ADCRegularGroupConfig reg = ADCRegularGroupConfig{};

        // Oversampling (Hardware Averaging)
        std::optional<ADCOversamplingConfig> oversampling = std::nullopt;
    };

    struct ADCChannelOffsetConfig
    {
        /// Offset Index (1 to 4)
        ADCChannelOffsetIdx idx = ADCChannelOffsetIdx::kOffset1;

        /// Offset Value (Raw ADC counts)
        uint32_t value = 0;
    };

    /**
     * @brief Configuration for a single ADC Channel (Physics).
     */
    struct ADCChannelConfig
    {
        /**
         * @brief Sampling time for the channel.
         * RM0440 Section 21.4.8, Table 115:
         * - Internal channels (VREFINT, VBAT, TempSensor): Minimum 12.5 cycles
         * - External channels with high impedance (> 50 kΩ): Consider longer sampling times
         * - At 60 MHz ADC clock (max): 12.5 cycles = 0.208 µs
         */
        ADCChannelSampleTime sampling_time = ADCChannelSampleTime::k24Cycles5;

        // Input mode (Single-Ended/Differential)
        ADCChannelInputMode input_mode = ADCChannelInputMode::kSingleEnded;

        // Optional offset configuration
        std::optional<ADCChannelOffsetConfig> offset = std::nullopt;
    };

    // -----------------------------------------------------------------------------
    // DEVICE CLASS
    // -----------------------------------------------------------------------------

    template <ADCControllerID tkControllerID>
        requires(kValidADCControllerID<tkControllerID>)
    class ADCControllerDevice
    {
    public:
        struct Descriptor : public SharedDeviceDescriptor
        {
            using Children = DeviceTreeList<ADCChannelDevice<tkControllerID, ADCChannelID::kChannel0>,
                                            ADCChannelDevice<tkControllerID, ADCChannelID::kChannel1>,
                                            ADCChannelDevice<tkControllerID, ADCChannelID::kChannel2>,
                                            ADCChannelDevice<tkControllerID, ADCChannelID::kChannel3>,
                                            ADCChannelDevice<tkControllerID, ADCChannelID::kChannel4>,
                                            ADCChannelDevice<tkControllerID, ADCChannelID::kChannel5>,
                                            ADCChannelDevice<tkControllerID, ADCChannelID::kChannel6>,
                                            ADCChannelDevice<tkControllerID, ADCChannelID::kChannel7>,
                                            ADCChannelDevice<tkControllerID, ADCChannelID::kChannel8>,
                                            ADCChannelDevice<tkControllerID, ADCChannelID::kChannel9>,
                                            ADCChannelDevice<tkControllerID, ADCChannelID::kChannel10>,
                                            ADCChannelDevice<tkControllerID, ADCChannelID::kChannel11>,
                                            ADCChannelDevice<tkControllerID, ADCChannelID::kChannel12>,
                                            ADCChannelDevice<tkControllerID, ADCChannelID::kChannel13>,
                                            ADCChannelDevice<tkControllerID, ADCChannelID::kChannel14>,
                                            ADCChannelDevice<tkControllerID, ADCChannelID::kChannel15>,
                                            ADCChannelDevice<tkControllerID, ADCChannelID::kChannel16>,
                                            ADCChannelDevice<tkControllerID, ADCChannelID::kChannel17>,
                                            ADCChannelDevice<tkControllerID, ADCChannelID::kChannel18>,
                                            ADCChannelDevice<tkControllerID, ADCChannelID::kChannelVRefInt>,
                                            ADCChannelDevice<tkControllerID, ADCChannelID::kChannelTempSensorADC1>,
                                            ADCChannelDevice<tkControllerID, ADCChannelID::kChannelTempSensorADC5>,
                                            ADCChannelDevice<tkControllerID, ADCChannelID::kChannelVBat>,
                                            ADCChannelDevice<tkControllerID, ADCChannelID::kChannelVOPAmp1>,
                                            ADCChannelDevice<tkControllerID, ADCChannelID::kChannelVOPAmp2>,
                                            ADCChannelDevice<tkControllerID, ADCChannelID::kChannelVOPAmp3ADC2>,
                                            ADCChannelDevice<tkControllerID, ADCChannelID::kChannelVOPAmp3ADC3>,
                                            ADCChannelDevice<tkControllerID, ADCChannelID::kChannelVOPAmp4>,
                                            ADCChannelDevice<tkControllerID, ADCChannelID::kChannelVOPAmp5>,
                                            ADCChannelDevice<tkControllerID, ADCChannelID::kChannelVOPAmp6>>;
        };
        static constexpr ADCControllerID skControllerID = tkControllerID;
        using ControllerTraitsT                         = ADCControllerTraits<skControllerID>;
        using ControllerDeviceTraitsT                   = ADCControllerDeviceTraits<skControllerID>;
        using InterfaceT                                = ADCControllerInterface<skControllerID>;
        using InjectGroupInterfaceT                     = ADCInjectGroupInterface<skControllerID>;
        using RegularGroupInterfaceT                    = ADCRegularGroupInterface<skControllerID>;

        template <ADCChannelID tkChannelID>
            requires(kValidADCChannelID<tkControllerID, tkChannelID>)
        using ChannelTraitsT = ADCChannelTraits<tkControllerID, tkChannelID>;

        using ClockDeviceT         = typename ControllerDeviceTraitsT::ADCClockDeviceT;
        using InterruptControllerT = ADCInterruptController<skControllerID>;

        using CTConfigTraitT             = ADCControllerCTConfigTraits<skControllerID>;
        static constexpr auto skCTConfig = CTConfigTraitT::skConfig;
        using CTConfigT                  = decltype(skCTConfig);
        using DMAChannelT                = CTConfigT::DMAChannelT;
        static constexpr bool skHasDMA   = !CNullDMAChannel<DMAChannelT>;

        using DependDevices = TypeList<ClockDeviceT, ADCRootDevice>;
        using InjectDevices = std::conditional_t<skHasDMA, TypeList<DMAChannelT>, TypeList<>>;

    private:
        template <std::size_t... I>
        [[nodiscard]] static constexpr auto make_channel_traits(std::index_sequence<I...>)
        {
            constexpr auto channels = magic_enum::enum_values<ADCChannelID>();

            return std::tuple<ChannelTraitsT<channels[I]>...>{};
        }

        static constexpr auto skChannelTraits =
            make_channel_traits(std::make_index_sequence<magic_enum::enum_count<ADCChannelID>()>{});

        [[no_unique_address]] ConditionalDeviceRef<skHasDMA, DMAChannelT> m_dma;

        // --- Storage ---
        // volatile is CRITICAL for DMA buffer
        // STM32G4 (Cortex-M4F) has no data cache, but volatile ensures:
        // 1. Each read goes to memory (DMA updates continuously in circular mode)
        // 2. Compiler won't optimize away repeated reads
        // 3. Proper memory ordering with DMA transfers
        // If future code removes volatile, stale data will be read in fast loops!
        alignas(32) volatile ADCValue m_dma_buffer[kADCMaxRegChannels];

        // Mappings
        // Ch -> Rank (1-4)
        std::array<std::optional<ADCInjectChannelRank>, kNumADCChannels> m_inj_cidx_to_rank_map{};

        // Ch -> Rank (1-16) (rank-1 for dma idx)
        std::array<std::optional<ADCRegularChannelRank>, kNumADCChannels> m_reg_cidx_to_rank_map{};

        // Cached values for optimization
        uint32_t m_inject_effective_resolution_range  = 4095;
        uint32_t m_regular_effective_resolution_range = 4095;
        float    m_ref_voltage                        = 3.3F;

    public:
        ADCControllerDevice(DeviceRef<DMAChannelT>&& dma_channel)
            requires(skHasDMA)
            : m_dma(std::move(dma_channel))
        {
            std::fill(std::begin(m_dma_buffer), std::end(m_dma_buffer), 0);
        }

        ADCControllerDevice()
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
         * @brief Initialize ADC Channel with given configuration and group/rank.
         *
         * @tparam tkChannelID Channel ID to initialize.
         * @tparam tkGroup Channel Group (Regular/Inject).
         * @tparam tkRank Rank in Sequence.
         */
        template <ADCChannelID tkChannelID, ADCChannelGroup tkGroup, uint8_t tkRank>
            requires(kValidADCChannelID<tkControllerID, tkChannelID>)
        auto init_channel(const ADCChannelConfig& config)
        {
            if constexpr (tkGroup == ADCChannelGroup::kRegular)
            {
                init_channel_as_regular<tkChannelID, tkRank>(config);
            }
            else if constexpr (tkGroup == ADCChannelGroup::kInject)
            {
                init_channel_as_inject<tkChannelID, tkRank>(config);
            }
            else
            {
                static_assert(false, "Unsupported ADC Channel Group");
            }
        }

        /**
         * @brief Initialize ADC channel as an Inject Channel.
         *
         * @tparam tkChannelID Channel ID to initialize.
         * @tparam tkRank Rank in Inject Sequence (1-4).
         */
        template <ADCChannelID tkChannelID, ADCInjectChannelRank tkRank>
            requires(kValidADCChannelID<tkControllerID, tkChannelID> && kValidADCInjectRank<tkRank>)
        void init_channel_as_inject(const ADCChannelConfig& cfg)
        {
            init_channel_core<tkChannelID>(cfg);
            return register_inject_sequence<tkChannelID, tkRank>();
        }

        /**
         * @brief Initialize ADC channel as a Regular Channel.
         *
         * @tparam tkChannelID Channel ID to initialize.
         * @tparam tkRank Rank in Regular Sequence (1-16).
         */
        template <ADCChannelID tkChannelID, ADCRegularChannelRank tkRank>
            requires(kValidADCChannelID<tkControllerID, tkChannelID> && kValidADCRegularRank<tkRank>)
        void init_channel_as_regular(const ADCChannelConfig& cfg)
        {
            init_channel_core<tkChannelID>(cfg);
            return register_regular_sequence<tkChannelID, tkRank>();
        }

        /**
         * @brief Post init called after init and channel registration. Sets up sequences and starts ADC.
         *
         */
        [[nodiscard]] bool post_init(const ADCControllerConfig& config)
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
            InterfaceT::set_common_sample_time(ADCCommonSamplingTime::kDefault);

            // Oversampling Configuration
            if (config.oversampling.has_value())
            {
                ADCOversamplingConfig ovs_cfg = config.oversampling.value();
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
        void enable_interrupts(const ADCInterruptConfig& config)
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

            ADCInterruptTraits<tkControllerID, ADCInterruptType::kInjectEndOfSequence>::ack();

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
            ADCInterruptTraits<tkControllerID, ADCInterruptType::kRegularEndOfSequence>::ack();
            ADCInterruptTraits<tkControllerID, ADCInterruptType::kOverrun>::ack();

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
         * @return ADCValue Raw ADC value.
         */
        template <ADCInjectChannelRank tkRank>
            requires(kValidADCInjectRank<tkRank>)
        [[nodiscard]] constexpr inline ADCValue read_inject_data() const
        {
            return read_inject_data(tkRank);
        }

        /**
         * @brief Read data from Inject Channel (Slow).
         *
         * @tparam tkChannelID Channel ID to read.
         * @return ADCValue Raw ADC value.
         */
        template <ADCChannelID tkChannelID>
        [[nodiscard]] constexpr inline ADCValue read_inject_data_slow()
        {
            const auto rank = get_adc_channel_rank_inject<tkChannelID>();
            expect(rank.has_value(), "Channel not registered in inject sequence");
            return read_inject_data(rank.value());
        }

        /**
         * @brief Read data from Regular Channel.
         *
         * @tparam tkRank Rank in Regular Sequence (1-16).
         * @return ADCValue Raw ADC value.
         */
        template <ADCRegularChannelRank tkRank>
            requires(kValidADCRegularRank<tkRank>)
        [[nodiscard]] constexpr inline ADCValue read_regular_data() const
        {
            return read_regular_data(tkRank);
        }

        /**
         * @brief Read data from Regular Channel (Slow).
         *
         * @tparam tkChannelID Channel ID to read.
         * @return ADCValue Raw ADC value.
         */
        template <ADCChannelID tkChannelID>
        [[nodiscard]] constexpr inline ADCValue read_regular_data_slow()
        {
            const auto rank = get_adc_channel_rank_regular<tkChannelID>();
            expect(rank.has_value(), "Channel not registered in regular sequence");
            return read_regular_data(rank.value());
        }

        /**
         * @brief Convert raw ADC value from inject group to normalized float (0.0 to 1.0).
         *
         * @param raw Raw ADC value.
         * @return float Normalized value.
         */
        [[nodiscard]] constexpr float raw_to_normalized_inject(const ADCValue raw) const
        {
            return static_cast<float>(raw) / m_inject_effective_resolution_range;
        }

        /**
         * @brief Convert raw ADC value from regular group to normalized float (0.0 to 1.0).
         *
         * @param raw Raw ADC value.
         * @return float Normalized value.
         */
        [[nodiscard]] constexpr float raw_to_normalized_regular(const ADCValue raw) const
        {
            return static_cast<float>(raw) / m_regular_effective_resolution_range;
        }

        /**
         * @brief Convert raw ADC value from inject group to voltage (Volts).
         *
         * @param raw Raw ADC value.
         * @return float Voltage in Volts.
         */
        [[nodiscard]] constexpr float raw_to_voltage_inject(const ADCValue raw) const
        {
            return raw_to_normalized_inject(raw) * m_ref_voltage;
        }

        /**
         * @brief Convert raw ADC value from regular group to voltage (Volts).
         *
         * @param raw Raw ADC value.
         * @return float Voltage in Volts.
         */
        [[nodiscard]] constexpr float raw_to_voltage_regular(const ADCValue raw) const
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
         * of type ChannelTraitsT<tkChannelID>.
         * @param func The function to apply to each channel trait.
         */
        template <typename TFunc>
        static constexpr void for_each_channel(TFunc&& func)
        {
            constexpr auto N = std::tuple_size_v<std::remove_reference_t<decltype(skChannelTraits)>>;
            for_each_channel_impl(std::forward<TFunc>(func), std::make_index_sequence<N>{});
        }

        [[nodiscard]] bool post_init_inj_pre_enable(const uint32_t inj_count, const ADCInjectGroupConfig& config)
        {
            if (inj_count == 0)
            {
                return true;  // No inject channels configured, skip inject group setup
            }

            // Core Config
            InjectGroupInterfaceT::set_queue_mode(ADCInjectGroupQueueMode::kDisable);  // Queue mode not supported
            InjectGroupInterfaceT::set_trigger_mode(config.auto_trigger_from_regular
                                                        ? ADCInjectGroupTriggerMode::kFromRegularGroup
                                                        : ADCInjectGroupTriggerMode::kIndependent);
            InjectGroupInterfaceT::set_sequencer_discontinuity_mode(
                ADCInjectGroupSequencerDiscontinuityMode::kDisable);  // Discontinuity not supported
            return true;
        }

        [[nodiscard]] bool post_init_reg_pre_enable(const uint32_t               reg_count,
                                                    const ADCRegularGroupConfig& config,
                                                    ADCDataAlignment             data_alignment)
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
                        RegularGroupInterfaceT::template set_sequencer_ranks<ChannelTraitT::skChannelID>(rank.value());
                    }
                });

            // DMA Configuration
            if constexpr (skHasDMA)
            {
                ADCRegularGroupDMATransfer dma_transfer = config.dma.circular_mode
                                                              ? ADCRegularGroupDMATransfer::kUnlimited
                                                              : ADCRegularGroupDMATransfer::kLimited;
                RegularGroupInterfaceT::set_dma_transfer(dma_transfer);

                static_assert(sizeof(ADCValue) == 2 || sizeof(ADCValue) == 4, "ADCValue must be 2 or 4 bytes");

                // Match DMA width to ADC alignment
                // RM0440 Section 21.4.20: Left-aligned ADC data requires 32-bit DMA
                DMADataWidth dma_width;
                if (data_alignment == ADCDataAlignment::kLeft)
                {
                    // Left-aligned: Always use 32-bit (word) transfer
                    dma_width = DMADataWidth::kWord;
                }
                else
                {
                    // Right-aligned: Use ADCValue size (16-bit or 32-bit)
                    dma_width = sizeof(ADCValue) == 2 ? DMADataWidth::kHalfWord : DMADataWidth::kWord;
                }

                // Configure DMA Channel
                if (!m_dma->init(DMAChannelConfig{
                        .direction         = DMADirection::kPeriphToMem,
                        .priority          = config.dma.priority,
                        .mode              = config.dma.circular_mode ? DMAMode::kCircular : DMAMode::kNormal,
                        .periph_data_width = dma_width,
                        .memory_data_width = dma_width,
                        .inc_periph        = false,
                        .inc_memory        = true,
                        .request_id        = ControllerTraitsT::skDMAMuxRequest,

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
                RegularGroupInterfaceT::set_dma_transfer(ADCRegularGroupDMATransfer::kNone);
            }

            return true;
        }

        [[nodiscard]] bool post_init_inj_post_enable(const uint32_t inj_count, const ADCInjectGroupConfig& config)
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
                        InjectGroupInterfaceT::template set_sequencer_ranks<ChannelTraitT::skChannelID>(rank.value());
                    }
                });

            return true;
        }

        [[nodiscard]] bool post_init_reg_post_enable(const uint32_t reg_count, const ADCRegularGroupConfig& config)
        {
            return true;
        }

        /**
         * @brief Get the ADC Channel Rank in Regular Sequence.
         *
         * @tparam tkChannelID The ID of the channel.
         * @return std::optional<uint8_t> Rank (1-16).
         */
        template <ADCChannelID tkChannelID>
        [[nodiscard]] constexpr inline std::optional<ADCRegularChannelRank> get_adc_channel_rank_regular() const
        {
            constexpr uint32_t idx = ChannelTraitsT<tkChannelID>::skChannelIdx;
            return m_reg_cidx_to_rank_map[idx];
        }

        /**
         * @brief Get the ADC Channel Rank in Inject Sequence.
         *
         * @tparam tkChannelID The ID of the channel.
         * @return std::optional<uint8_t> Rank (1-4).
         */
        template <ADCChannelID tkChannelID>
        [[nodiscard]] constexpr inline std::optional<ADCInjectChannelRank> get_adc_channel_rank_inject() const
        {
            constexpr uint32_t idx = ChannelTraitsT<tkChannelID>::skChannelIdx;
            return m_inj_cidx_to_rank_map[idx];
        }

        /**
         * @brief Configure and register this channel.
         *
         * @tparam tkChannelID The ID of the channel to configure.
         * @param cfg        Physics configuration (Sampling time, Single/Diff, etc.)
         */
        template <ADCChannelID tkChannelID>
            requires(kValidADCChannelID<tkControllerID, tkChannelID>)
        void init_channel_core(const ADCChannelConfig& cfg)
        {
            using ChannelInterfaceT = ADCChannelInterface<tkControllerID, tkChannelID>;

            ChannelInterfaceT::set_sampling_time(cfg.sampling_time);
            ChannelInterfaceT::set_input_mode(cfg.input_mode);

            if (cfg.offset.has_value())
            {
                ADCChannelOffsetConfig offset_cfg = cfg.offset.value();
                ChannelInterfaceT::config_offset(offset_cfg.idx, offset_cfg.value);
            }
        }

        /**
         * @brief Registers a channel for the inject conversion sequence.
         *
         * @tparam tkChannelID The ID of the channel to register.
         * @tparam tkRank Rank in Inject Sequence (1-4).
         */
        template <ADCChannelID tkChannelID, ADCInjectChannelRank tkRank>
            requires(kValidADCChannelID<tkControllerID, tkChannelID> && kValidADCInjectRank<tkRank>)
        void register_inject_sequence()
        {
            constexpr uint32_t idx = ChannelTraitsT<tkChannelID>::skChannelIdx;
            if (!m_inj_cidx_to_rank_map[idx].has_value())
            {
                m_inj_cidx_to_rank_map[idx] = tkRank;
            }
        }

        /**
         * @brief Registers a channel for the regular conversion sequence.
         *
         * @tparam tkChannelID The ID of the channel to register.
         * @tparam tkRank The rank in the regular sequence (1-16).
         */
        template <ADCChannelID tkChannelID, ADCRegularChannelRank tkRank>
            requires(kValidADCChannelID<tkControllerID, tkChannelID> && kValidADCRegularRank<tkRank>)
        void register_regular_sequence()
        {
            constexpr uint32_t idx = ChannelTraitsT<tkChannelID>::skChannelIdx;
            if (!m_reg_cidx_to_rank_map[idx].has_value())
            {
                m_reg_cidx_to_rank_map[idx] = tkRank;
            }
        }

        /**
         * @brief Read data from inject channel.
         *
         * @param rank Rank in Inject Sequence (1-4).
         * @return ADCValue Raw ADC value.
         */
        [[nodiscard]] inline ADCValue read_inject_data(const ADCInjectChannelRank rank) const
        {
            return InjectGroupInterfaceT::read(rank);
        }

        /**
         * @brief Read data from regular channel.
         *
         * @param rank Rank in Regular Sequence (1-16).
         * @return ADCValue Raw ADC value.
         */
        [[nodiscard]] constexpr inline ADCValue read_regular_data(const ADCRegularChannelRank rank) const
        {
            // Memory barrier to ensure DMA writes are visible to CPU
            // STM32G4 (Cortex-M4F) doesn't have data cache, but memory barrier
            // ensures proper ordering of DMA transfers vs CPU reads
            __DMB();
            const uint8_t buf_idx = rank - 1;
            return m_dma_buffer[buf_idx];
        }
    };

    // -----------------------------------------------------------------------------
    // DEVICE ALIASES
    // -----------------------------------------------------------------------------

    using ADC1ControllerDevice = ADCControllerDevice<1>;
    using ADC2ControllerDevice = ADCControllerDevice<2>;
    using ADC3ControllerDevice = ADCControllerDevice<3>;
    using ADC4ControllerDevice = ADCControllerDevice<4>;
    using ADC5ControllerDevice = ADCControllerDevice<5>;

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
    struct ADCPinDevice;

    template <CNullADCPinMap T>
    struct ADCPinDevice<T>
    {
        using type = GPIONullPinDevice;
    };

    template <typename T>
    struct ADCPinDevice
    {
        using type = GPIOPinDevice<T::skPort, T::skPin>;
    };

    template <ADCControllerID tkControllerID, ADCChannelID tkChannelID>
        requires(kValidADCChannelID<tkControllerID, tkChannelID>)
    class ADCChannelDevice
    {
    public:
        struct Descriptor : public UniqueDeviceDescriptor
        {
        };

        static constexpr ADCControllerID skControllerID = tkControllerID;
        static constexpr ADCChannelID    skChannelID    = tkChannelID;
        using ControllerT                               = ADCControllerDevice<tkControllerID>;
        using ControllerTraitsT                         = ADCControllerTraits<tkControllerID>;
        using ChannelTraitsT                            = ADCChannelTraits<tkControllerID, tkChannelID>;

        using PinT                     = typename ADCPinDevice<ADCPinMap<tkControllerID, tkChannelID>>::type;
        static constexpr bool skHasPin = !CNullGPIOPinDevice<PinT>;

        using InjectDevices = std::conditional_t<skHasPin, TypeList<ControllerT, PinT>, TypeList<ControllerT>>;

        template <ADCControllerID tkCID, ADCChannelID tkChID, ADCInjectChannelRank tkRank>
            requires(kValidADCChannelID<tkCID, tkChID> && kValidADCInjectRank<tkRank>)
        friend class ADCInjectChannelDevice;

        template <ADCControllerID tkCID, ADCChannelID tkChID, ADCRegularChannelRank tkRank>
            requires(kValidADCChannelID<tkCID, tkChID> && kValidADCRegularRank<tkRank>)
        friend class ADCRegularChannelDevice;

    private:
        [[no_unique_address]] DeviceRef<ControllerT>                           m_adc;
        std::conditional_t<skHasPin, GPIOAnalogInDriver<PinT>, std::monostate> m_pin;

    public:
        explicit ADCChannelDevice(DeviceRef<ControllerT>&& adc, DeviceRef<PinT>&& pin)
            requires(skHasPin)
            : m_adc(std::move(adc)), m_pin(std::move(pin))
        {
        }

        explicit ADCChannelDevice(DeviceRef<ControllerT> adc)
            requires(!skHasPin)
            : m_adc(std::move(adc))
        {
        }

    protected:
        // --- Initialization ---
        /**
         * @brief Configure and register this channel.
         *
         * @tparam tkGroup ADC Channel Group (Regular/Inject).
         * @tparam tkRank Rank in Sequence (Regular: 1-16, Inject: 1-4).
         * @param config Physics configuration (Sampling time, Single/Diff, etc.)
         */
        template <ADCChannelGroup tkGroup, uint8_t tkRank = 0>
            requires((tkGroup == ADCChannelGroup::kRegular && kValidADCRegularRank<tkRank>) ||
                     (tkGroup == ADCChannelGroup::kInject && kValidADCInjectRank<tkRank>))
        [[nodiscard]] bool init(const ADCChannelConfig& config)
        {
            m_adc->template init_channel<tkChannelID, tkGroup, tkRank>(config);
            return gpio_init();
        }

        /**
         * @brief Configure and register this channel for the Inject Sequence (Interrupt).
         *
         * @tparam tkRank Rank in Inject Sequence (1-4).
         * @param config Physics configuration (Sampling time, Single/Diff, etc.)
         */
        template <ADCRegularChannelRank tkRank>
        [[nodiscard]] bool init_as_regular(const ADCChannelConfig& config)
        {
            m_adc->template init_channel_as_regular<skChannelID, tkRank>(config);
            return gpio_init();
        }

        /**
         * @brief Configure and register this channel for the Regular Sequence (DMA).
         *
         * @tparam tkRank Rank in Regular Sequence (1-16).
         * @param config Physics configuration (Sampling time, Single/Diff, etc.)
         */
        template <ADCInjectChannelRank tkRank>
        [[nodiscard]] bool init_as_inject(const ADCChannelConfig& config)
        {
            m_adc->template init_channel_as_inject<skChannelID, tkRank>(config);
            return gpio_init();
        }

        // --- Accessors ---
        // Read raw
        /**
         * @brief Read data from inject channel.
         *
         * @tparam tkRank Rank in Inject Sequence (1-4).
         */
        template <ADCInjectChannelRank tkRank>
            requires(kValidADCInjectRank<tkRank>)
        [[nodiscard]] inline ADCValue read_inject() const
        {
            return m_adc->template read_inject_data<tkRank>();
        }

        /**
         * @brief Read data from inject channel (slow method, use read_inject() instead).
         *
         * @return ADCValue Raw ADC value.
         */
        [[nodiscard]] inline ADCValue read_inject_slow() const
        {
            return m_adc->template read_inject_data_slow<skChannelID>();
        }

        /**
         * @brief Read data from regular channel.
         *
         * @tparam tkRank Rank in Regular Sequence (1-16).
         */
        template <ADCRegularChannelRank tkRank>
            requires(kValidADCRegularRank<tkRank>)
        [[nodiscard]] inline ADCValue read_regular() const
        {
            return m_adc->template read_regular_data<tkRank>();
        }

        /**
         * @brief Read data from regular channel (slow method, use read_regular() instead).
         *
         * @return ADCValue Raw ADC value.
         */
        [[nodiscard]] inline ADCValue read_regular_slow() const
        {
            return m_adc->template read_regular_data_slow<skChannelID>();
        }

        // Read voltage
        /**
         * @brief Read voltage from inject channel.
         *
         * @tparam tkRank Rank in Inject Sequence (1-4).
         * @return float Voltage in Volts.
         */
        template <ADCInjectChannelRank tkRank>
            requires(kValidADCInjectRank<tkRank>)
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
        template <ADCRegularChannelRank tkRank>
            requires(kValidADCRegularRank<tkRank>)
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
        template <ADCInjectChannelRank tkRank>
            requires(kValidADCInjectRank<tkRank>)
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
        template <ADCRegularChannelRank tkRank>
            requires(kValidADCRegularRank<tkRank>)
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

        [[nodiscard]] constexpr float raw_to_normalized_inject(const ADCValue raw) const
        {
            return m_adc->raw_to_normalized_inject(raw);
        }

        [[nodiscard]] constexpr float raw_to_normalized_regular(const ADCValue raw) const
        {
            return m_adc->raw_to_normalized_regular(raw);
        }

        [[nodiscard]] constexpr float raw_to_voltage_inject(const ADCValue raw) const
        {
            return m_adc->raw_to_voltage_inject(raw);
        }

        [[nodiscard]] constexpr float raw_to_voltage_regular(const ADCValue raw) const
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
                return m_pin.init(GPIOAnalogInConfig{});
            }
            return true;
        }
    };

    // -----------------------------------------------------------------------------
    // DEVICE ALIASES
    // -----------------------------------------------------------------------------

#define DECLARE_ADC_CHANNEL_DEVICE_ALIASES(controller_id)                                                  \
    using ADC##controller_id##Channel0Device  = ADCChannelDevice<controller_id, ADCChannelID::kChannel0>;  \
    using ADC##controller_id##Channel1Device  = ADCChannelDevice<controller_id, ADCChannelID::kChannel1>;  \
    using ADC##controller_id##Channel2Device  = ADCChannelDevice<controller_id, ADCChannelID::kChannel2>;  \
    using ADC##controller_id##Channel3Device  = ADCChannelDevice<controller_id, ADCChannelID::kChannel3>;  \
    using ADC##controller_id##Channel4Device  = ADCChannelDevice<controller_id, ADCChannelID::kChannel4>;  \
    using ADC##controller_id##Channel5Device  = ADCChannelDevice<controller_id, ADCChannelID::kChannel5>;  \
    using ADC##controller_id##Channel6Device  = ADCChannelDevice<controller_id, ADCChannelID::kChannel6>;  \
    using ADC##controller_id##Channel7Device  = ADCChannelDevice<controller_id, ADCChannelID::kChannel7>;  \
    using ADC##controller_id##Channel8Device  = ADCChannelDevice<controller_id, ADCChannelID::kChannel8>;  \
    using ADC##controller_id##Channel9Device  = ADCChannelDevice<controller_id, ADCChannelID::kChannel9>;  \
    using ADC##controller_id##Channel10Device = ADCChannelDevice<controller_id, ADCChannelID::kChannel10>; \
    using ADC##controller_id##Channel11Device = ADCChannelDevice<controller_id, ADCChannelID::kChannel11>; \
    using ADC##controller_id##Channel12Device = ADCChannelDevice<controller_id, ADCChannelID::kChannel12>; \
    using ADC##controller_id##Channel13Device = ADCChannelDevice<controller_id, ADCChannelID::kChannel13>; \
    using ADC##controller_id##Channel14Device = ADCChannelDevice<controller_id, ADCChannelID::kChannel14>; \
    using ADC##controller_id##Channel15Device = ADCChannelDevice<controller_id, ADCChannelID::kChannel15>; \
    using ADC##controller_id##Channel16Device = ADCChannelDevice<controller_id, ADCChannelID::kChannel16>; \
    using ADC##controller_id##Channel17Device = ADCChannelDevice<controller_id, ADCChannelID::kChannel17>; \
    using ADC##controller_id##Channel18Device = ADCChannelDevice<controller_id, ADCChannelID::kChannel18>;

    DECLARE_ADC_CHANNEL_DEVICE_ALIASES(1);
    DECLARE_ADC_CHANNEL_DEVICE_ALIASES(2);
    DECLARE_ADC_CHANNEL_DEVICE_ALIASES(3);
    DECLARE_ADC_CHANNEL_DEVICE_ALIASES(4);
    DECLARE_ADC_CHANNEL_DEVICE_ALIASES(5);

#undef DECLARE_ADC_CHANNEL_DEVICE_ALIASES

    // ============================================================================
    // ADC INJECTED CHANNEL (UNIQUE DEVICE)
    // ============================================================================

    template <ADCControllerID tkControllerID, ADCChannelID tkChannelID, ADCInjectChannelRank tkRank>
        requires(kValidADCChannelID<tkControllerID, tkChannelID> &&
                 kValidADCInjectRank<tkRank>)  // Only 4 inject channels supported
    class ADCInjectChannelDevice
    {
    public:
        struct Descriptor : public UniqueDeviceDescriptor
        {
        };

        using ChannelT      = ADCChannelDevice<tkControllerID, tkChannelID>;
        using InjectDevices = TypeList<ChannelT>;

        static constexpr ADCControllerID      skControllerID = tkControllerID;
        static constexpr ADCChannelID         skChannelID    = tkChannelID;
        static constexpr ADCChannelGroup      skGroup        = ADCChannelGroup::kInject;
        static constexpr ADCInjectChannelRank skRank         = tkRank;

    private:
        [[no_unique_address]] DeviceRef<ChannelT> m_channel;

    public:
        explicit ADCInjectChannelDevice(DeviceRef<ChannelT> channel) : m_channel(std::move(channel))
        {
        }

        // --- Initialization ---
        /**
         * @brief Initialize Inject Channel.
         *
         */
        [[nodiscard]] bool init(const ADCChannelConfig& config)
        {
            return m_channel->template init_as_inject<skRank>(config);
        }

        // --- Accessors ---
        /**
         * @brief Read data from inject channel.
         *
         * @return ADCValue Raw ADC value.
         */
        [[nodiscard]] ADCValue read() const
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

    template <ADCControllerID tkControllerID, ADCChannelID tkChannelID>
        requires(kValidADCChannelID<tkControllerID, tkChannelID>)
    using ADCInjectChannelRank1Device = ADCInjectChannelDevice<tkControllerID, tkChannelID, 1>;

    template <ADCControllerID tkControllerID, ADCChannelID tkChannelID>
        requires(kValidADCChannelID<tkControllerID, tkChannelID>)
    using ADCInjectChannelRank2Device = ADCInjectChannelDevice<tkControllerID, tkChannelID, 2>;
    template <ADCControllerID tkControllerID, ADCChannelID tkChannelID>
        requires(kValidADCChannelID<tkControllerID, tkChannelID>)
    using ADCInjectChannelRank3Device = ADCInjectChannelDevice<tkControllerID, tkChannelID, 3>;

    template <ADCControllerID tkControllerID, ADCChannelID tkChannelID>
        requires(kValidADCChannelID<tkControllerID, tkChannelID>)
    using ADCInjectChannelRank4Device = ADCInjectChannelDevice<tkControllerID, tkChannelID, 4>;

    namespace detail
    {
        // Helper to unpack the Index Sequence alongside the Channel Pack
        template <ADCControllerID tkControllerID, typename TIndexSeq, ADCChannelID... tkChannelIDs>
        struct MakeInjectSequenceImpl;

        template <ADCControllerID tkControllerID, size_t... Is, ADCChannelID... tkChannelIDs>
        struct MakeInjectSequenceImpl<tkControllerID, std::index_sequence<Is...>, tkChannelIDs...>
        {
            // Expand both packs: Channel[i] paired with Rank[i + 1]
            using type = TypeList<ADCInjectChannelDevice<tkControllerID, tkChannelIDs, (Is + 1)>...>;
        };
    }  // namespace detail

    template <ADCControllerID tkControllerID, ADCChannelID... tkChannelIDs>
        requires((kValidADCChannelID<tkControllerID, tkChannelIDs> && ...) &&
                 (sizeof...(tkChannelIDs) <= kADCMaxInjChannels))
    using ADCInjectChannelDeviceSequence =
        typename detail::MakeInjectSequenceImpl<tkControllerID,
                                                std::make_index_sequence<sizeof...(tkChannelIDs)>,  // Generates 0,
                                                                                                    // 1, 2...
                                                tkChannelIDs...>::type;

#define DECLARE_ADC_INJECT_CHANNEL_DEVICE_ALIASES_FOR_RANK(controller_id, rank) \
    using ADC##controller_id##InjectChannel0Rank##rank##Device =                \
        ADCInjectChannelDevice<controller_id, ADCChannelID::kChannel0, rank>;   \
    using ADC##controller_id##InjectChannel1Rank##rank##Device =                \
        ADCInjectChannelDevice<controller_id, ADCChannelID::kChannel1, rank>;   \
    using ADC##controller_id##InjectChannel2Rank##rank##Device =                \
        ADCInjectChannelDevice<controller_id, ADCChannelID::kChannel2, rank>;   \
    using ADC##controller_id##InjectChannel3Rank##rank##Device =                \
        ADCInjectChannelDevice<controller_id, ADCChannelID::kChannel3, rank>;   \
    using ADC##controller_id##InjectChannel4Rank##rank##Device =                \
        ADCInjectChannelDevice<controller_id, ADCChannelID::kChannel4, rank>;   \
    using ADC##controller_id##InjectChannel5Rank##rank##Device =                \
        ADCInjectChannelDevice<controller_id, ADCChannelID::kChannel5, rank>;   \
    using ADC##controller_id##InjectChannel6Rank##rank##Device =                \
        ADCInjectChannelDevice<controller_id, ADCChannelID::kChannel6, rank>;   \
    using ADC##controller_id##InjectChannel7Rank##rank##Device =                \
        ADCInjectChannelDevice<controller_id, ADCChannelID::kChannel7, rank>;   \
    using ADC##controller_id##InjectChannel8Rank##rank##Device =                \
        ADCInjectChannelDevice<controller_id, ADCChannelID::kChannel8, rank>;   \
    using ADC##controller_id##InjectChannel9Rank##rank##Device =                \
        ADCInjectChannelDevice<controller_id, ADCChannelID::kChannel9, rank>;   \
    using ADC##controller_id##InjectChannel10Rank##rank##Device =               \
        ADCInjectChannelDevice<controller_id, ADCChannelID::kChannel10, rank>;  \
    using ADC##controller_id##InjectChannel11Rank##rank##Device =               \
        ADCInjectChannelDevice<controller_id, ADCChannelID::kChannel11, rank>;  \
    using ADC##controller_id##InjectChannel12Rank##rank##Device =               \
        ADCInjectChannelDevice<controller_id, ADCChannelID::kChannel12, rank>;  \
    using ADC##controller_id##InjectChannel13Rank##rank##Device =               \
        ADCInjectChannelDevice<controller_id, ADCChannelID::kChannel13, rank>;  \
    using ADC##controller_id##InjectChannel14Rank##rank##Device =               \
        ADCInjectChannelDevice<controller_id, ADCChannelID::kChannel14, rank>;  \
    using ADC##controller_id##InjectChannel15Rank##rank##Device =               \
        ADCInjectChannelDevice<controller_id, ADCChannelID::kChannel15, rank>;  \
    using ADC##controller_id##InjectChannel16Rank##rank##Device =               \
        ADCInjectChannelDevice<controller_id, ADCChannelID::kChannel16, rank>;  \
    using ADC##controller_id##InjectChannel17Rank##rank##Device =               \
        ADCInjectChannelDevice<controller_id, ADCChannelID::kChannel17, rank>;  \
    using ADC##controller_id##InjectChannel18Rank##rank##Device =               \
        ADCInjectChannelDevice<controller_id, ADCChannelID::kChannel18, rank>;

#define DECLARE_ADC_INJECT_CHANNEL_DEVICE_ALIASES(controller_id)          \
    DECLARE_ADC_INJECT_CHANNEL_DEVICE_ALIASES_FOR_RANK(controller_id, 1); \
    DECLARE_ADC_INJECT_CHANNEL_DEVICE_ALIASES_FOR_RANK(controller_id, 2); \
    DECLARE_ADC_INJECT_CHANNEL_DEVICE_ALIASES_FOR_RANK(controller_id, 3); \
    DECLARE_ADC_INJECT_CHANNEL_DEVICE_ALIASES_FOR_RANK(controller_id, 4);

    DECLARE_ADC_INJECT_CHANNEL_DEVICE_ALIASES(1);
    DECLARE_ADC_INJECT_CHANNEL_DEVICE_ALIASES(2);
    DECLARE_ADC_INJECT_CHANNEL_DEVICE_ALIASES(3);
    DECLARE_ADC_INJECT_CHANNEL_DEVICE_ALIASES(4);

#undef DECLARE_ADC_INJECT_CHANNEL_DEVICE_ALIASES_FOR_RANK
#undef DECLARE_ADC_INJECT_CHANNEL_DEVICE_ALIASES

    // ============================================================================
    // ADC REGULAR CHANNEL (UNIQUE DEVICE)
    // ============================================================================

    template <ADCControllerID tkControllerID, ADCChannelID tkChannelID, ADCRegularChannelRank tkRank>
        requires(kValidADCChannelID<tkControllerID, tkChannelID> &&
                 kValidADCRegularRank<tkRank>)  // Only 16 regular channels supported
    class ADCRegularChannelDevice
    {
    public:
        struct Descriptor : public UniqueDeviceDescriptor
        {
        };

        using ChannelT                                        = ADCChannelDevice<tkControllerID, tkChannelID>;
        using InjectDevices                                   = TypeList<ChannelT>;
        static constexpr ADCControllerID       skControllerID = tkControllerID;
        static constexpr ADCChannelID          skChannelID    = tkChannelID;
        static constexpr ADCChannelGroup       skGroup        = ADCChannelGroup::kRegular;
        static constexpr ADCRegularChannelRank skRank         = tkRank;

    private:
        [[no_unique_address]] DeviceRef<ChannelT> m_channel;

    public:
        explicit ADCRegularChannelDevice(DeviceRef<ChannelT>&& channel) : m_channel(std::move(channel))
        {
        }

        // --- Initialization ---
        /**
         * @brief Initialize Regular Channel.
         *
         */
        [[nodiscard]] bool init(const ADCChannelConfig& config)
        {
            return m_channel->template init_as_regular<skRank>(config);
        }

        // --- Accessors ---
        /**
         * @brief Read data from regular channel.
         *
         * @return ADCValue Raw ADC value.
         */
        [[nodiscard]] ADCValue read() const
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

    template <ADCControllerID tkControllerID, ADCChannelID tkChannelID>
        requires(kValidADCChannelID<tkControllerID, tkChannelID>)
    using ADCRegularChannelRank1Device = ADCRegularChannelDevice<tkControllerID, tkChannelID, 1>;

    template <ADCControllerID tkControllerID, ADCChannelID tkChannelID>
        requires(kValidADCChannelID<tkControllerID, tkChannelID>)
    using ADCRegularChannelRank2Device = ADCRegularChannelDevice<tkControllerID, tkChannelID, 2>;

    template <ADCControllerID tkControllerID, ADCChannelID tkChannelID>
        requires(kValidADCChannelID<tkControllerID, tkChannelID>)
    using ADCRegularChannelRank3Device = ADCRegularChannelDevice<tkControllerID, tkChannelID, 3>;

    template <ADCControllerID tkControllerID, ADCChannelID tkChannelID>
        requires(kValidADCChannelID<tkControllerID, tkChannelID>)
    using ADCRegularChannelRank4Device = ADCRegularChannelDevice<tkControllerID, tkChannelID, 4>;

    template <ADCControllerID tkControllerID, ADCChannelID tkChannelID>
        requires(kValidADCChannelID<tkControllerID, tkChannelID>)
    using ADCRegularChannelRank5Device = ADCRegularChannelDevice<tkControllerID, tkChannelID, 5>;

    template <ADCControllerID tkControllerID, ADCChannelID tkChannelID>
        requires(kValidADCChannelID<tkControllerID, tkChannelID>)
    using ADCRegularChannelRank6Device = ADCRegularChannelDevice<tkControllerID, tkChannelID, 6>;

    template <ADCControllerID tkControllerID, ADCChannelID tkChannelID>
        requires(kValidADCChannelID<tkControllerID, tkChannelID>)
    using ADCRegularChannelRank7Device = ADCRegularChannelDevice<tkControllerID, tkChannelID, 7>;

    template <ADCControllerID tkControllerID, ADCChannelID tkChannelID>
        requires(kValidADCChannelID<tkControllerID, tkChannelID>)
    using ADCRegularChannelRank8Device = ADCRegularChannelDevice<tkControllerID, tkChannelID, 8>;

    template <ADCControllerID tkControllerID, ADCChannelID tkChannelID>
        requires(kValidADCChannelID<tkControllerID, tkChannelID>)
    using ADCRegularChannelRank9Device = ADCRegularChannelDevice<tkControllerID, tkChannelID, 9>;

    template <ADCControllerID tkControllerID, ADCChannelID tkChannelID>
        requires(kValidADCChannelID<tkControllerID, tkChannelID>)
    using ADCRegularChannelRank10Device = ADCRegularChannelDevice<tkControllerID, tkChannelID, 10>;

    template <ADCControllerID tkControllerID, ADCChannelID tkChannelID>
        requires(kValidADCChannelID<tkControllerID, tkChannelID>)
    using ADCRegularChannelRank11Device = ADCRegularChannelDevice<tkControllerID, tkChannelID, 11>;

    template <ADCControllerID tkControllerID, ADCChannelID tkChannelID>
        requires(kValidADCChannelID<tkControllerID, tkChannelID>)
    using ADCRegularChannelRank12Device = ADCRegularChannelDevice<tkControllerID, tkChannelID, 12>;

    template <ADCControllerID tkControllerID, ADCChannelID tkChannelID>
        requires(kValidADCChannelID<tkControllerID, tkChannelID>)
    using ADCRegularChannelRank13Device = ADCRegularChannelDevice<tkControllerID, tkChannelID, 13>;

    template <ADCControllerID tkControllerID, ADCChannelID tkChannelID>
        requires(kValidADCChannelID<tkControllerID, tkChannelID>)
    using ADCRegularChannelRank14Device = ADCRegularChannelDevice<tkControllerID, tkChannelID, 14>;

    template <ADCControllerID tkControllerID, ADCChannelID tkChannelID>
        requires(kValidADCChannelID<tkControllerID, tkChannelID>)
    using ADCRegularChannelRank15Device = ADCRegularChannelDevice<tkControllerID, tkChannelID, 15>;

    template <ADCControllerID tkControllerID, ADCChannelID tkChannelID>
        requires(kValidADCChannelID<tkControllerID, tkChannelID>)
    using ADCRegularChannelRank16Device = ADCRegularChannelDevice<tkControllerID, tkChannelID, 16>;

    namespace detail
    {
        // Helper to unpack the Index Sequence alongside the Channel Pack
        template <ADCControllerID tkControllerID, typename TIndexSeq, ADCChannelID... tkChannelIDs>
        struct MakeRegularSequenceImpl;

        template <ADCControllerID tkControllerID, size_t... Is, ADCChannelID... tkChannelIDs>
        struct MakeRegularSequenceImpl<tkControllerID, std::index_sequence<Is...>, tkChannelIDs...>
        {
            // Expand both packs: Channel[i] paired with Rank[i + 1]
            using type = TypeList<ADCRegularChannelDevice<tkControllerID, tkChannelIDs, (Is + 1)>...>;
        };
    }  // namespace detail

    template <ADCControllerID tkControllerID, ADCChannelID... tkChannelIDs>
        requires((kValidADCChannelID<tkControllerID, tkChannelIDs> && ...) &&
                 (sizeof...(tkChannelIDs) <= kADCMaxRegChannels))
    using ADCRegularChannelDeviceSequence =
        typename detail::MakeRegularSequenceImpl<tkControllerID,
                                                 std::make_index_sequence<sizeof...(tkChannelIDs)>,  // Generates 0,
                                                                                                     // 1, 2...
                                                 tkChannelIDs...>::type;

#define DECLARE_ADC_REGULAR_CHANNEL_DEVICE_ALIASES_FOR_RANK(controller_id, rank) \
    using ADC##controller_id##RegularChannel0Rank##rank##Device =                \
        ADCRegularChannelDevice<controller_id, ADCChannelID::kChannel0, rank>;   \
    using ADC##controller_id##RegularChannel1Rank##rank##Device =                \
        ADCRegularChannelDevice<controller_id, ADCChannelID::kChannel1, rank>;   \
    using ADC##controller_id##RegularChannel2Rank##rank##Device =                \
        ADCRegularChannelDevice<controller_id, ADCChannelID::kChannel2, rank>;   \
    using ADC##controller_id##RegularChannel3Rank##rank##Device =                \
        ADCRegularChannelDevice<controller_id, ADCChannelID::kChannel3, rank>;   \
    using ADC##controller_id##RegularChannel4Rank##rank##Device =                \
        ADCRegularChannelDevice<controller_id, ADCChannelID::kChannel4, rank>;   \
    using ADC##controller_id##RegularChannel5Rank##rank##Device =                \
        ADCRegularChannelDevice<controller_id, ADCChannelID::kChannel5, rank>;   \
    using ADC##controller_id##RegularChannel6Rank##rank##Device =                \
        ADCRegularChannelDevice<controller_id, ADCChannelID::kChannel6, rank>;   \
    using ADC##controller_id##RegularChannel7Rank##rank##Device =                \
        ADCRegularChannelDevice<controller_id, ADCChannelID::kChannel7, rank>;   \
    using ADC##controller_id##RegularChannel8Rank##rank##Device =                \
        ADCRegularChannelDevice<controller_id, ADCChannelID::kChannel8, rank>;   \
    using ADC##controller_id##RegularChannel9Rank##rank##Device =                \
        ADCRegularChannelDevice<controller_id, ADCChannelID::kChannel9, rank>;   \
    using ADC##controller_id##RegularChannel10Rank##rank##Device =               \
        ADCRegularChannelDevice<controller_id, ADCChannelID::kChannel10, rank>;  \
    using ADC##controller_id##RegularChannel11Rank##rank##Device =               \
        ADCRegularChannelDevice<controller_id, ADCChannelID::kChannel11, rank>;  \
    using ADC##controller_id##RegularChannel12Rank##rank##Device =               \
        ADCRegularChannelDevice<controller_id, ADCChannelID::kChannel12, rank>;  \
    using ADC##controller_id##RegularChannel13Rank##rank##Device =               \
        ADCRegularChannelDevice<controller_id, ADCChannelID::kChannel13, rank>;  \
    using ADC##controller_id##RegularChannel14Rank##rank##Device =               \
        ADCRegularChannelDevice<controller_id, ADCChannelID::kChannel14, rank>;  \
    using ADC##controller_id##RegularChannel15Rank##rank##Device =               \
        ADCRegularChannelDevice<controller_id, ADCChannelID::kChannel15, rank>;  \
    using ADC##controller_id##RegularChannel16Rank##rank##Device =               \
        ADCRegularChannelDevice<controller_id, ADCChannelID::kChannel16, rank>;  \
    using ADC##controller_id##RegularChannel17Rank##rank##Device =               \
        ADCRegularChannelDevice<controller_id, ADCChannelID::kChannel17, rank>;  \
    using ADC##controller_id##RegularChannel18Rank##rank##Device =               \
        ADCRegularChannelDevice<controller_id, ADCChannelID::kChannel18, rank>;

#define DECLARE_ADC_REGULAR_CHANNEL_DEVICE_ALIASES(controller_id)           \
    DECLARE_ADC_REGULAR_CHANNEL_DEVICE_ALIASES_FOR_RANK(controller_id, 1);  \
    DECLARE_ADC_REGULAR_CHANNEL_DEVICE_ALIASES_FOR_RANK(controller_id, 2);  \
    DECLARE_ADC_REGULAR_CHANNEL_DEVICE_ALIASES_FOR_RANK(controller_id, 3);  \
    DECLARE_ADC_REGULAR_CHANNEL_DEVICE_ALIASES_FOR_RANK(controller_id, 4);  \
    DECLARE_ADC_REGULAR_CHANNEL_DEVICE_ALIASES_FOR_RANK(controller_id, 5);  \
    DECLARE_ADC_REGULAR_CHANNEL_DEVICE_ALIASES_FOR_RANK(controller_id, 6);  \
    DECLARE_ADC_REGULAR_CHANNEL_DEVICE_ALIASES_FOR_RANK(controller_id, 7);  \
    DECLARE_ADC_REGULAR_CHANNEL_DEVICE_ALIASES_FOR_RANK(controller_id, 8);  \
    DECLARE_ADC_REGULAR_CHANNEL_DEVICE_ALIASES_FOR_RANK(controller_id, 9);  \
    DECLARE_ADC_REGULAR_CHANNEL_DEVICE_ALIASES_FOR_RANK(controller_id, 10); \
    DECLARE_ADC_REGULAR_CHANNEL_DEVICE_ALIASES_FOR_RANK(controller_id, 11); \
    DECLARE_ADC_REGULAR_CHANNEL_DEVICE_ALIASES_FOR_RANK(controller_id, 12); \
    DECLARE_ADC_REGULAR_CHANNEL_DEVICE_ALIASES_FOR_RANK(controller_id, 13); \
    DECLARE_ADC_REGULAR_CHANNEL_DEVICE_ALIASES_FOR_RANK(controller_id, 14); \
    DECLARE_ADC_REGULAR_CHANNEL_DEVICE_ALIASES_FOR_RANK(controller_id, 15); \
    DECLARE_ADC_REGULAR_CHANNEL_DEVICE_ALIASES_FOR_RANK(controller_id, 16);

    DECLARE_ADC_REGULAR_CHANNEL_DEVICE_ALIASES(1);
    DECLARE_ADC_REGULAR_CHANNEL_DEVICE_ALIASES(2);
    DECLARE_ADC_REGULAR_CHANNEL_DEVICE_ALIASES(3);
    DECLARE_ADC_REGULAR_CHANNEL_DEVICE_ALIASES(4);
    DECLARE_ADC_REGULAR_CHANNEL_DEVICE_ALIASES(5);

#undef DECLARE_ADC_REGULAR_CHANNEL_DEVICE_ALIASES_FOR_RANK
#undef DECLARE_ADC_REGULAR_CHANNEL_DEVICE_ALIASES

}  // namespace valle::platform