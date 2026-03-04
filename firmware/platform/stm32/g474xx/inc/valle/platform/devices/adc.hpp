#pragma once

#include <algorithm>
#include <array>
#include <cstdint>
#include <optional>
#include <variant>

#include "valle/core.hpp"
#include "valle/core/device/device.hpp"
#include "valle/platform/core.hpp"
#include "valle/platform/devices/adc_clk.hpp"
#include "valle/platform/devices/dma.hpp"
#include "valle/platform/drivers/gpio/analog_in.hpp"
#include "valle/platform/hardware/adc.hpp"

namespace valle
{
    // ============================================================================
    // FORWARD DECLARATIONS
    // ============================================================================
    class ADCRootDevice;

    template <ADCControllerID tkControllerID>
        requires(kValidADCControllerID<tkControllerID>)
    class ADCControllerDevice;

    template <ADCControllerID tkControllerID, ADCChannelID tkChannelId>
        requires(kValidADCChannelID<tkControllerID, tkChannelId>)
    class ADCChannelDevice;

    template <ADCControllerID tkControllerID, ADCChannelID tkChannelId, ADCInjectChannelRank tkRank>
        requires(kValidADCChannelID<tkControllerID, tkChannelId> && kValidADCInjectRank<tkRank>)
    class ADCInjectChannelDevice;

    template <ADCControllerID tkControllerID, ADCChannelID tkChannelId, ADCRegularChannelRank tkRank>
        requires(kValidADCChannelID<tkControllerID, tkChannelId> && kValidADCRegularRank<tkRank>)
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
    struct ADCControllerCTConfigDefaults
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
        static constexpr auto skConfig = ADCControllerCTConfigDefaults{};
    };

#define VALLE_DEFINE_ADC_CONTROLLER_CT_CONFIG(tkControllerID, config)                                           \
    namespace valle                                                                                             \
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
        ADCOversamplingShiftBits shift = ADCOversamplingShiftBits::kNone;

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
        using ClockDeviceT                              = typename ControllerDeviceTraitsT::ADCClockDeviceT;
        using InterruptControllerT                      = ADCInterruptController<skControllerID>;

        using CTConfigTraitT             = ADCControllerCTConfigTraits<skControllerID>;
        static constexpr auto skCTConfig = CTConfigTraitT::skConfig;
        using CTConfigT                  = decltype(skCTConfig);
        using DMAChannelT                = CTConfigT::DMAChannelT;
        static constexpr bool skHasDMA   = !CNullDMAChannel<DMAChannelT>;

        using DependDevices = TypeList<ClockDeviceT, ADCRootDevice>;
        using InjectDevices = std::conditional_t<skHasDMA, TypeList<DMAChannelT>, TypeList<>>;

    private:
        static constexpr uint8_t skChannelRankFreeFlag = 0xFF;

        // --- Storage ---
        // volatile is CRITICAL for DMA buffer
        // STM32G4 (Cortex-M4F) has no data cache, but volatile ensures:
        // 1. Each read goes to memory (DMA updates continuously in circular mode)
        // 2. Compiler won't optimize away repeated reads
        // 3. Proper memory ordering with DMA transfers
        // If future code removes volatile, stale data will be read in fast loops!
        alignas(32) volatile ADCValue m_dma_buffer[kADCMaxRegChannels];

        // Mappings
        uint8_t m_reg_cidx_to_rank_map[kADCMaxChannelId];  // Ch -> Rank (1-16) (rank-1 for dma idx)
        uint8_t m_inj_cidx_to_rank_map[kADCMaxChannelId];  // Ch -> Rank (1-4)

        [[no_unique_address]] ConditionalDeviceRef<skHasDMA, DMAChannelT>      m_dma;
        std::conditional_t<skHasDMA, ADCRegularGroupDMAConfig, std::monostate> m_dma_config;

        // Store data alignment for DMA configuration
        ADCDataAlignment     m_data_alignment = ADCDataAlignment::kRight;
        ADCInjectGroupConfig m_inj_config{};

    public:
        ADCControllerDevice(DeviceRef<DMAChannelT>&& dma_channel)
            requires(skHasDMA)

            : m_dma(std::move(dma_channel))
        {
            std::fill(std::begin(m_reg_cidx_to_rank_map), std::end(m_reg_cidx_to_rank_map), skChannelRankFreeFlag);
            std::fill(std::begin(m_inj_cidx_to_rank_map), std::end(m_inj_cidx_to_rank_map), skChannelRankFreeFlag);
            std::fill(std::begin(m_dma_buffer), std::end(m_dma_buffer), 0);
        }

        ADCControllerDevice()
            requires(!skHasDMA)
        {
            std::fill(std::begin(m_reg_cidx_to_rank_map), std::end(m_reg_cidx_to_rank_map), skChannelRankFreeFlag);
            std::fill(std::begin(m_inj_cidx_to_rank_map), std::end(m_inj_cidx_to_rank_map), skChannelRankFreeFlag);
            std::fill(std::begin(m_dma_buffer), std::end(m_dma_buffer), 0);
        }

        // --- Initialization (Called by Main/Builder) ---

        /**
         * @brief Initialize ADC Peripheral with given configuration.
         *
         * @param config ADC Peripheral Configuration.
         */
        [[nodiscard]] bool init(const ADCControllerConfig& config)
        {
            if constexpr (skHasDMA)
            {
                m_dma_config = config.reg.dma;
            }

            // Store data alignment for DMA width calculation
            m_data_alignment = config.data_alignment;
            m_inj_config     = config.inj;

            if (LL_ADC_IsEnabled(ControllerTraitsT::skInstance) == 0)
            {
                LL_ADC_DisableDeepPowerDown(ControllerTraitsT::skInstance);
                LL_ADC_EnableInternalRegulator(ControllerTraitsT::skInstance);

                // RM0440 Section 21.4.6: tADCVREG_STUP = 20 µs (typ)
                // wait for 100 to be safe.
                delay_us_busy(100u);
            }

            // Calibration with precise timeout
            // RM0440 Section 21.4.6: tCAL = 116 ADC clock cycles
            // At 40 MHz ADC clock: ~3 µs typical, allow up to 1 ms for safety
            //
            // NOTE: If ADC clock frequency changes at runtime (e.g., entering
            // low-power mode), calibration must be re-run.
            LL_ADC_StartCalibration(ControllerTraitsT::skInstance, LL_ADC_SINGLE_ENDED);
            const bool calibration_success = wait_for_with_timeout_us(
                []() { return LL_ADC_IsCalibrationOnGoing(ControllerTraitsT::skInstance) == 0; }, 1000u);

            // If timeout occurred, calibration failed
            if (!calibration_success)
            {
                VALLE_LOG_FATAL("ADC calibration timeout");
                return false;
            }

            // Global Configuration
            LL_ADC_SetResolution(ControllerTraitsT::skInstance, static_cast<uint32_t>(config.resolution));
            LL_ADC_SetDataAlignment(ControllerTraitsT::skInstance, static_cast<uint32_t>(config.data_alignment));
            LL_ADC_SetLowPowerMode(ControllerTraitsT::skInstance, static_cast<uint32_t>(config.low_power));

            // Inject Group Configuration (must be done before enable)
            LL_ADC_INJ_SetQueueMode(ControllerTraitsT::skInstance,
                                    LL_ADC_INJ_QUEUE_DISABLE);  // Queue mode not supported
            LL_ADC_INJ_SetTrigAuto(
                ControllerTraitsT::skInstance,
                config.inj.auto_trigger_from_regular ? LL_ADC_INJ_TRIG_FROM_GRP_REGULAR : LL_ADC_INJ_TRIG_INDEPENDENT);

            // Regular Group Configuration
            LL_ADC_REG_SetTriggerSource(ControllerTraitsT::skInstance,
                                        static_cast<uint32_t>(config.reg.trigger_source));
            if (config.reg.trigger_source != ADCRegularGroupTriggerSource::kSoftware)
            {
                LL_ADC_REG_SetTriggerEdge(ControllerTraitsT::skInstance,
                                          static_cast<uint32_t>(config.reg.trigger_edge));
            }
            LL_ADC_REG_SetOverrun(ControllerTraitsT::skInstance, static_cast<uint32_t>(config.reg.overrun));
            LL_ADC_REG_SetContinuousMode(ControllerTraitsT::skInstance,
                                         static_cast<uint32_t>(config.reg.conversion_mode));

            // Oversampling Configuration
            if (config.oversampling.has_value())
            {
                ADCOversamplingConfig ovs_cfg = config.oversampling.value();
                LL_ADC_SetOverSamplingScope(ControllerTraitsT::skInstance, static_cast<uint32_t>(ovs_cfg.scope));
                LL_ADC_ConfigOverSamplingRatioShift(ControllerTraitsT::skInstance,
                                                    static_cast<uint32_t>(ovs_cfg.ratio),
                                                    static_cast<uint32_t>(ovs_cfg.shift));
                LL_ADC_SetOverSamplingDiscont(ControllerTraitsT::skInstance,
                                              static_cast<uint32_t>(config.reg.oversampling_mode));
            }

            return true;
        }

        // --- Registration API (Called by Channel) ---
        /**
         * @brief Initialize ADC Channel with given configuration and group/rank.
         *
         * @tparam tkChannelId Channel ID to initialize.
         * @tparam tkGroup Channel Group (Regular/Inject).
         * @tparam tkRank Rank in Sequence.
         */
        template <ADCChannelID tkChannelId, ADCChannelGroup tkGroup, uint8_t tkRank>
            requires(kValidADCChannelID<tkControllerID, tkChannelId>)
        auto init_channel(const ADCChannelConfig& config)
        {
            if constexpr (tkGroup == ADCChannelGroup::kRegular)
            {
                init_channel_as_regular<tkChannelId, tkRank>(config);
            }
            else if constexpr (tkGroup == ADCChannelGroup::kInject)
            {
                init_channel_as_inject<tkChannelId, tkRank>(config);
            }
            else
            {
                static_assert(false, "Unsupported ADC Channel Group");
            }
        }

        /**
         * @brief Initialize ADC channel as an Inject Channel.
         *
         * @tparam tkChannelId Channel ID to initialize.
         * @tparam tkRank Rank in Inject Sequence (1-4).
         */
        template <ADCChannelID tkChannelId, ADCInjectChannelRank tkRank>
            requires(kValidADCChannelID<tkControllerID, tkChannelId> && kValidADCInjectRank<tkRank>)
        void init_channel_as_inject(const ADCChannelConfig& cfg)
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
        template <ADCChannelID tkChannelId, ADCRegularChannelRank tkRank>
            requires(kValidADCChannelID<tkControllerID, tkChannelId> && kValidADCRegularRank<tkRank>)
        void init_channel_as_regular(const ADCChannelConfig& cfg)
        {
            init_channel_core<tkChannelId>(cfg);
            return register_regular_sequence<tkChannelId, tkRank>();
        }

        /**
         * @brief Post init called after init and channel registration. Sets up sequences and starts ADC.
         *
         */
        [[nodiscard]] bool post_init()
        {
            const uint8_t reg_count =
                static_cast<uint8_t>(std::count_if(std::begin(m_reg_cidx_to_rank_map),
                                                   std::end(m_reg_cidx_to_rank_map),
                                                   [](const uint8_t v) { return v != skChannelRankFreeFlag; }));

            // Regular Sequence Config
            if (reg_count > 0)
            {
                LL_ADC_REG_SetSequencerLength(ControllerTraitsT::skInstance,
                                              ADCRegularGroupTraits::count_to_sequence_length(reg_count));

                for (uint32_t i = 0; i < kADCMaxChannelId; ++i)
                {
                    if (m_reg_cidx_to_rank_map[i] != skChannelRankFreeFlag)
                    {
                        const uint32_t ch = __LL_ADC_DECIMAL_NB_TO_CHANNEL(i);
                        LL_ADC_REG_SetSequencerRanks(ControllerTraitsT::skInstance,
                                                     ADCRegularGroupTraits::rank_to_rank_reg(m_reg_cidx_to_rank_map[i]),
                                                     ch);
                    }
                }

                if constexpr (skHasDMA)
                {
                    ADCRegularGroupDMATransfer dma_transfer = m_dma_config.circular_mode
                                                                  ? ADCRegularGroupDMATransfer::kUnlimited
                                                                  : ADCRegularGroupDMATransfer::kLimited;
                    LL_ADC_REG_SetDMATransfer(ControllerTraitsT::skInstance, static_cast<uint32_t>(dma_transfer));

                    static_assert(sizeof(ADCValue) == 2 || sizeof(ADCValue) == 4, "ADCValue must be 2 or 4 bytes");

                    // Match DMA width to ADC alignment
                    // RM0440 Section 21.4.20: Left-aligned ADC data requires 32-bit DMA
                    DMADataWidth dma_width;
                    if (m_data_alignment == ADCDataAlignment::kLeft)
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
                            .priority          = m_dma_config.priority,
                            .mode              = m_dma_config.circular_mode ? DMAMode::kCircular : DMAMode::kNormal,
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
                }
                else
                {
                    LL_ADC_REG_SetDMATransfer(ControllerTraitsT::skInstance,
                                              static_cast<uint32_t>(ADCRegularGroupDMATransfer::kNone));
                }
            }

            // Wait for ADC ready with precise timeout
            // RM0440: Typically a few ADC clock cycles, allow up to 100 µs for safety
            LL_ADC_Enable(ControllerTraitsT::skInstance);
            const bool adc_ready = wait_for_with_timeout_us(
                []() { return LL_ADC_IsActiveFlag_ADRDY(ControllerTraitsT::skInstance) != 0; }, 100u);

            if (!adc_ready)
            {
                VALLE_LOG_FATAL("ADC enable timeout");
                return false;
            }

            // Inject Group Configuration (after enable)
            LL_ADC_INJ_SetTriggerSource(ControllerTraitsT::skInstance,
                                        static_cast<uint32_t>(m_inj_config.trigger_source));
            if (m_inj_config.trigger_source != ADCInjectGroupTriggerSource::kSoftware)
            {
                LL_ADC_INJ_SetTriggerEdge(ControllerTraitsT::skInstance,
                                          static_cast<uint32_t>(m_inj_config.trigger_edge));
            }

            const uint8_t inj_count =
                static_cast<uint8_t>(std::count_if(std::begin(m_inj_cidx_to_rank_map),
                                                   std::end(m_inj_cidx_to_rank_map),
                                                   [](const uint8_t v) { return v != skChannelRankFreeFlag; }));

            // Inject Sequence Config
            if (inj_count > 0)
            {
                LL_ADC_INJ_SetSequencerLength(ControllerTraitsT::skInstance,
                                              ADCInjectGroupTraits::count_to_sequence_length(inj_count));

                for (uint32_t i = 0; i < kADCMaxChannelId; ++i)
                {
                    if (m_inj_cidx_to_rank_map[i] != skChannelRankFreeFlag)
                    {
                        const uint32_t ch = __LL_ADC_DECIMAL_NB_TO_CHANNEL(i);
                        LL_ADC_INJ_SetSequencerRanks(ControllerTraitsT::skInstance,
                                                     ADCInjectGroupTraits::rank_to_rank_reg(m_inj_cidx_to_rank_map[i]),
                                                     ch);
                    }
                }
            }

            return true;
        }

        [[nodiscard]] bool initialized() const
        {
            return LL_ADC_IsActiveFlag_ADRDY(ControllerTraitsT::skInstance) != 0;
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
        static void trigger_inject()
        {
            // ARM THE TRIGGER (The most important part)
            // If Trigger is Hardware (HRTIM): ADC goes into "Waiting for Trigger" state.
            // If Trigger is Software: ADC converts immediately.
            LL_ADC_INJ_StartConversion(ControllerTraitsT::skInstance);
        }

        /**
         * @brief Stop Inject Conversions.
         *
         */
        void stop_inject()
        {
            LL_ADC_INJ_StopConversion(ControllerTraitsT::skInstance);
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
                if (m_dma_config.interrupts.has_value())
                {
                    m_dma->enable_interrupts(m_dma_config.interrupts.value());
                }

                // Start DMA Transfer
                const uint32_t adc_dr_addr = reinterpret_cast<uint32_t>(&(ControllerTraitsT::skInstance->DR));
                const uint32_t buffer_addr = reinterpret_cast<uint32_t>(m_dma_buffer);
                const uint32_t reg_count   = ADCRegularGroupTraits::sequence_length_to_count(
                    LL_ADC_REG_GetSequencerLength(ControllerTraitsT::skInstance));
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
        static void trigger_regular()
        {
            // ARM THE TRIGGER (The most important part)
            // If Trigger is Hardware (HRTIM): ADC goes into "Waiting for Trigger" state.
            // If Trigger is Software: ADC converts immediately.
            LL_ADC_REG_StartConversion(ControllerTraitsT::skInstance);
        }

        /**
         * @brief Stop Regular Conversions.
         *
         */
        void stop_regular()
        {
            LL_ADC_REG_StopConversion(ControllerTraitsT::skInstance);
            if constexpr (skHasDMA)
            {
                m_dma->disable_interrupts();
            }
        }

        // --- Resolution Info ---
        [[nodiscard]] static inline float get_resolution_bits()
        {
            switch (LL_ADC_GetResolution(ControllerTraitsT::skInstance))
            {
                case LL_ADC_RESOLUTION_12B:
                    return 12.0F;
                case LL_ADC_RESOLUTION_10B:
                    return 10.0F;
                case LL_ADC_RESOLUTION_8B:
                    return 8.0F;
                case LL_ADC_RESOLUTION_6B:
                    return 6.0F;
                default:
                    return 12.0F;
            }
        }

        [[nodiscard]] static inline float get_resolution_range()
        {
            switch (LL_ADC_GetResolution(ControllerTraitsT::skInstance))
            {
                case LL_ADC_RESOLUTION_12B:
                    return 4095.0F;
                case LL_ADC_RESOLUTION_10B:
                    return 1023.0F;
                case LL_ADC_RESOLUTION_8B:
                    return 255.0F;
                case LL_ADC_RESOLUTION_6B:
                    return 63.0F;
                default:
                    return 4095.0F;  // assume 12-bit
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
            return LL_ADC_INJ_ReadConversionData32(ControllerTraitsT::skInstance,
                                                   ADCInjectGroupTraits::rank_to_rank_reg(tkRank));
        }

        /**
         * @brief Read data from Inject Channel (Slow).
         *
         * @tparam tkChannelId Channel ID to read.
         * @return ADCValue Raw ADC value.
         */
        template <ADCChannelID tkChannelId>
        [[nodiscard]] constexpr inline ADCValue read_inject_data_slow()
        {
            return read_inject_data(get_adc_channel_rank_inject<tkChannelId>());
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
         * @tparam tkChannelId Channel ID to read.
         * @return ADCValue Raw ADC value.
         */
        template <ADCChannelID tkChannelId>
        [[nodiscard]] constexpr inline ADCValue read_regular_data_slow()
        {
            return read_regular_data(get_adc_channel_rank_regular<tkChannelId>());
        }

    private:
        /**
         * @brief Get the ADC Channel Index from Channel ID.
         *
         * @tparam tkChannelId Channel ID.
         * @return uint32_t Channel Index.
         */
        template <ADCChannelID tkChannelId>
        [[nodiscard]] static inline uint32_t get_adc_channel_idx()
        {
            return __LL_ADC_CHANNEL_TO_DECIMAL_NB(static_cast<uint32_t>(tkChannelId));
        }

        /**
         * @brief Get the ADC Channel Rank in Regular Sequence.
         *
         * @tparam tkChannelId The ID of the channel.
         * @return uint8_t Rank (1-16).
         */
        template <ADCChannelID tkChannelId>
        [[nodiscard]] constexpr inline ADCRegularChannelRank get_adc_channel_rank_regular() const
        {
            const uint32_t idx = get_adc_channel_idx<tkChannelId>();
            return m_reg_cidx_to_rank_map[idx];
        }

        /**
         * @brief Get the ADC Channel Rank in Inject Sequence.
         *
         * @tparam tkChannelId The ID of the channel.
         * @return uint8_t Rank (1-4).
         */
        template <ADCChannelID tkChannelId>
        [[nodiscard]] constexpr inline ADCInjectChannelRank get_adc_channel_rank_inject() const
        {
            const uint32_t idx = get_adc_channel_idx<tkChannelId>();
            return m_inj_cidx_to_rank_map[idx];
        }

        /**
         * @brief Configure and register this channel.
         *
         * @tparam tkChannelId The ID of the channel to configure.
         * @param cfg        Physics configuration (Sampling time, Single/Diff, etc.)
         */
        template <ADCChannelID tkChannelId>
            requires(kValidADCChannelID<tkControllerID, tkChannelId>)
        void init_channel_core(const ADCChannelConfig& cfg)
        {
            LL_ADC_SetChannelSamplingTime(ControllerTraitsT::skInstance,
                                          static_cast<uint32_t>(tkChannelId),
                                          static_cast<uint32_t>(cfg.sampling_time));
            LL_ADC_SetChannelSingleDiff(ControllerTraitsT::skInstance,
                                        static_cast<uint32_t>(tkChannelId),
                                        static_cast<uint32_t>(cfg.input_mode));

            if (cfg.offset.has_value())
            {
                ADCChannelOffsetConfig offset_cfg = cfg.offset.value();

                LL_ADC_SetOffset(ControllerTraitsT::skInstance,
                                 static_cast<uint32_t>(offset_cfg.idx),
                                 static_cast<uint32_t>(tkChannelId),
                                 offset_cfg.value);
            }
        }

        /**
         * @brief Registers a channel for the inject conversion sequence.
         *
         * @tparam tkChannelId The ID of the channel to register.
         * @tparam tkRank Rank in Inject Sequence (1-4).
         */
        template <ADCChannelID tkChannelId, ADCInjectChannelRank tkRank>
            requires(kValidADCChannelID<tkControllerID, tkChannelId> && kValidADCInjectRank<tkRank>)
        void register_inject_sequence()
        {
            const uint32_t idx = get_adc_channel_idx<tkChannelId>();
            if (idx < kADCMaxChannelId && m_inj_cidx_to_rank_map[idx] == skChannelRankFreeFlag)
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
        template <ADCChannelID tkChannelId, ADCRegularChannelRank tkRank>
            requires(kValidADCChannelID<tkControllerID, tkChannelId> && kValidADCRegularRank<tkRank>)
        void register_regular_sequence()
        {
            const uint32_t idx = get_adc_channel_idx<tkChannelId>();
            if (m_reg_cidx_to_rank_map[idx] == skChannelRankFreeFlag)
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
            return LL_ADC_INJ_ReadConversionData32(ControllerTraitsT::skInstance,
                                                   ADCInjectGroupTraits::rank_to_rank_reg(rank));
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

    template <ADCControllerID tkControllerID, ADCChannelID tkChannelId>
        requires(kValidADCChannelID<tkControllerID, tkChannelId>)
    class ADCChannelDevice
    {
    public:
        struct Descriptor : public UniqueDeviceDescriptor
        {
        };

        using ControllerT              = ADCControllerDevice<tkControllerID>;
        using PinT                     = typename ADCPinDevice<ADCPinMap<tkControllerID, tkChannelId>>::type;
        static constexpr bool skHasPin = !CNullGPIOPinDevice<PinT>;

        using InjectDevices = std::conditional_t<skHasPin, TypeList<ControllerT, PinT>, TypeList<ControllerT>>;

        static constexpr ADCControllerID skControllerID = tkControllerID;
        static constexpr ADCChannelID    skChannelID    = tkChannelId;

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
            m_adc->template init_channel<tkChannelId, tkGroup, tkRank>(config);
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
            return raw_to_voltage(read_inject<tkRank>());
        }

        /**
         * @brief Read voltage from inject channel (slow method, use read_inject_voltage() instead).
         *
         * @return float Voltage in Volts.
         */
        [[nodiscard]] inline float read_inject_voltage_slow() const
        {
            return raw_to_voltage(read_inject_slow());
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
            return raw_to_voltage(read_regular<tkRank>());
        }

        /**
         * @brief Read voltage from regular channel (slow method, use read_regular_voltage() instead).
         *
         * @return float Voltage in Volts.
         */
        [[nodiscard]] inline float read_regular_voltage_slow() const
        {
            return raw_to_voltage(read_regular_slow());
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
            return raw_to_normalized(read_inject<tkRank>());
        }

        /**
         * @brief Read normalized value (0.0 to 1.0) from inject channel (slow method, use read_inject_normalized()
         * instead).
         *
         * @return float Normalized value.
         */
        [[nodiscard]] inline float read_inject_normalized_slow() const
        {
            return raw_to_normalized(read_inject_slow());
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
            return raw_to_normalized(read_regular<tkRank>());
        }

        /**
         * @brief Read normalized value (0.0 to 1.0) from regular channel (slow method, use read_regular_normalized()
         * instead).
         *
         * @return float Normalized value.
         */
        [[nodiscard]] inline float read_regular_normalized_slow() const
        {
            return raw_to_normalized(read_regular_slow());
        }

        /**
         * @brief Convert raw ADC value to normalized float (0.0 to 1.0).
         *
         * @param raw Raw ADC value.
         * @return float Normalized value.
         */
        [[nodiscard]] inline float raw_to_normalized(const ADCValue raw) const
        {
            return static_cast<float>(raw) / m_adc->get_resolution_range();
        }

        /**
         * @brief Convert raw ADC value to voltage (Volts).
         *
         * @param raw Raw ADC value.
         * @return float Voltage in Volts.
         */
        [[nodiscard]] inline float raw_to_voltage(const ADCValue raw) const
        {
            return raw_to_normalized(raw) * 3.3F;
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

    template <ADCControllerID tkControllerID, ADCChannelID tkChannelId, ADCInjectChannelRank tkRank>
        requires(kValidADCChannelID<tkControllerID, tkChannelId> &&
                 kValidADCInjectRank<tkRank>)  // Only 4 inject channels supported
    class ADCInjectChannelDevice
    {
    public:
        struct Descriptor : public UniqueDeviceDescriptor
        {
        };

        using ChannelT      = ADCChannelDevice<tkControllerID, tkChannelId>;
        using InjectDevices = TypeList<ChannelT>;

        static constexpr ADCControllerID      skControllerID = tkControllerID;
        static constexpr ADCChannelID         skChannelID    = tkChannelId;
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

    template <ADCControllerID tkControllerID, ADCChannelID tkChannelId>
        requires(kValidADCChannelID<tkControllerID, tkChannelId>)
    using ADCInjectChannelRank1Device = ADCInjectChannelDevice<tkControllerID, tkChannelId, 1>;

    template <ADCControllerID tkControllerID, ADCChannelID tkChannelId>
        requires(kValidADCChannelID<tkControllerID, tkChannelId>)
    using ADCInjectChannelRank2Device = ADCInjectChannelDevice<tkControllerID, tkChannelId, 2>;
    template <ADCControllerID tkControllerID, ADCChannelID tkChannelId>
        requires(kValidADCChannelID<tkControllerID, tkChannelId>)
    using ADCInjectChannelRank3Device = ADCInjectChannelDevice<tkControllerID, tkChannelId, 3>;

    template <ADCControllerID tkControllerID, ADCChannelID tkChannelId>
        requires(kValidADCChannelID<tkControllerID, tkChannelId>)
    using ADCInjectChannelRank4Device = ADCInjectChannelDevice<tkControllerID, tkChannelId, 4>;

    namespace detail
    {
        // Helper to unpack the Index Sequence alongside the Channel Pack
        template <ADCControllerID tkControllerID, typename TIndexSeq, ADCChannelID... tkChannelIds>
        struct MakeInjectSequenceImpl;

        template <ADCControllerID tkControllerID, size_t... Is, ADCChannelID... tkChannelIds>
        struct MakeInjectSequenceImpl<tkControllerID, std::index_sequence<Is...>, tkChannelIds...>
        {
            // Expand both packs: Channel[i] paired with Rank[i + 1]
            using type = TypeList<ADCInjectChannelDevice<tkControllerID, tkChannelIds, (Is + 1)>...>;
        };
    }  // namespace detail

    template <ADCControllerID tkControllerID, ADCChannelID... tkChannelIds>
        requires((kValidADCChannelID<tkControllerID, tkChannelIds> && ...) &&
                 (sizeof...(tkChannelIds) <= kADCMaxInjChannels))
    using ADCInjectChannelDeviceSequence =
        typename detail::MakeInjectSequenceImpl<tkControllerID,
                                                std::make_index_sequence<sizeof...(tkChannelIds)>,  // Generates 0,
                                                                                                    // 1, 2...
                                                tkChannelIds...>::type;

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

    template <ADCControllerID tkControllerID, ADCChannelID tkChannelId, ADCRegularChannelRank tkRank>
        requires(kValidADCChannelID<tkControllerID, tkChannelId> &&
                 kValidADCRegularRank<tkRank>)  // Only 16 regular channels supported
    class ADCRegularChannelDevice
    {
    public:
        struct Descriptor : public UniqueDeviceDescriptor
        {
        };

        using ChannelT                                        = ADCChannelDevice<tkControllerID, tkChannelId>;
        using InjectDevices                                   = TypeList<ChannelT>;
        static constexpr ADCControllerID       skControllerID = tkControllerID;
        static constexpr ADCChannelID          skChannelID    = tkChannelId;
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

    template <ADCControllerID tkControllerID, ADCChannelID tkChannelId>
        requires(kValidADCChannelID<tkControllerID, tkChannelId>)
    using ADCRegularChannelRank1Device = ADCRegularChannelDevice<tkControllerID, tkChannelId, 1>;

    template <ADCControllerID tkControllerID, ADCChannelID tkChannelId>
        requires(kValidADCChannelID<tkControllerID, tkChannelId>)
    using ADCRegularChannelRank2Device = ADCRegularChannelDevice<tkControllerID, tkChannelId, 2>;

    template <ADCControllerID tkControllerID, ADCChannelID tkChannelId>
        requires(kValidADCChannelID<tkControllerID, tkChannelId>)
    using ADCRegularChannelRank3Device = ADCRegularChannelDevice<tkControllerID, tkChannelId, 3>;

    template <ADCControllerID tkControllerID, ADCChannelID tkChannelId>
        requires(kValidADCChannelID<tkControllerID, tkChannelId>)
    using ADCRegularChannelRank4Device = ADCRegularChannelDevice<tkControllerID, tkChannelId, 4>;

    template <ADCControllerID tkControllerID, ADCChannelID tkChannelId>
        requires(kValidADCChannelID<tkControllerID, tkChannelId>)
    using ADCRegularChannelRank5Device = ADCRegularChannelDevice<tkControllerID, tkChannelId, 5>;

    template <ADCControllerID tkControllerID, ADCChannelID tkChannelId>
        requires(kValidADCChannelID<tkControllerID, tkChannelId>)
    using ADCRegularChannelRank6Device = ADCRegularChannelDevice<tkControllerID, tkChannelId, 6>;

    template <ADCControllerID tkControllerID, ADCChannelID tkChannelId>
        requires(kValidADCChannelID<tkControllerID, tkChannelId>)
    using ADCRegularChannelRank7Device = ADCRegularChannelDevice<tkControllerID, tkChannelId, 7>;

    template <ADCControllerID tkControllerID, ADCChannelID tkChannelId>
        requires(kValidADCChannelID<tkControllerID, tkChannelId>)
    using ADCRegularChannelRank8Device = ADCRegularChannelDevice<tkControllerID, tkChannelId, 8>;

    template <ADCControllerID tkControllerID, ADCChannelID tkChannelId>
        requires(kValidADCChannelID<tkControllerID, tkChannelId>)
    using ADCRegularChannelRank9Device = ADCRegularChannelDevice<tkControllerID, tkChannelId, 9>;

    template <ADCControllerID tkControllerID, ADCChannelID tkChannelId>
        requires(kValidADCChannelID<tkControllerID, tkChannelId>)
    using ADCRegularChannelRank10Device = ADCRegularChannelDevice<tkControllerID, tkChannelId, 10>;

    template <ADCControllerID tkControllerID, ADCChannelID tkChannelId>
        requires(kValidADCChannelID<tkControllerID, tkChannelId>)
    using ADCRegularChannelRank11Device = ADCRegularChannelDevice<tkControllerID, tkChannelId, 11>;

    template <ADCControllerID tkControllerID, ADCChannelID tkChannelId>
        requires(kValidADCChannelID<tkControllerID, tkChannelId>)
    using ADCRegularChannelRank12Device = ADCRegularChannelDevice<tkControllerID, tkChannelId, 12>;

    template <ADCControllerID tkControllerID, ADCChannelID tkChannelId>
        requires(kValidADCChannelID<tkControllerID, tkChannelId>)
    using ADCRegularChannelRank13Device = ADCRegularChannelDevice<tkControllerID, tkChannelId, 13>;

    template <ADCControllerID tkControllerID, ADCChannelID tkChannelId>
        requires(kValidADCChannelID<tkControllerID, tkChannelId>)
    using ADCRegularChannelRank14Device = ADCRegularChannelDevice<tkControllerID, tkChannelId, 14>;

    template <ADCControllerID tkControllerID, ADCChannelID tkChannelId>
        requires(kValidADCChannelID<tkControllerID, tkChannelId>)
    using ADCRegularChannelRank15Device = ADCRegularChannelDevice<tkControllerID, tkChannelId, 15>;

    template <ADCControllerID tkControllerID, ADCChannelID tkChannelId>
        requires(kValidADCChannelID<tkControllerID, tkChannelId>)
    using ADCRegularChannelRank16Device = ADCRegularChannelDevice<tkControllerID, tkChannelId, 16>;

    namespace detail
    {
        // Helper to unpack the Index Sequence alongside the Channel Pack
        template <ADCControllerID tkControllerID, typename TIndexSeq, ADCChannelID... tkChannelIds>
        struct MakeRegularSequenceImpl;

        template <ADCControllerID tkControllerID, size_t... Is, ADCChannelID... tkChannelIds>
        struct MakeRegularSequenceImpl<tkControllerID, std::index_sequence<Is...>, tkChannelIds...>
        {
            // Expand both packs: Channel[i] paired with Rank[i + 1]
            using type = TypeList<ADCRegularChannelDevice<tkControllerID, tkChannelIds, (Is + 1)>...>;
        };
    }  // namespace detail

    template <ADCControllerID tkControllerID, ADCChannelID... tkChannelIds>
        requires((kValidADCChannelID<tkControllerID, tkChannelIds> && ...) &&
                 (sizeof...(tkChannelIds) <= kADCMaxRegChannels))
    using ADCRegularChannelDeviceSequence =
        typename detail::MakeRegularSequenceImpl<tkControllerID,
                                                 std::make_index_sequence<sizeof...(tkChannelIds)>,  // Generates 0,
                                                                                                     // 1, 2...
                                                 tkChannelIds...>::type;

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

}  // namespace valle