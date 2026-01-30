#pragma once

#include <algorithm>
#include <array>
#include <cstdint>
#include <optional>
#include <variant>

#include "Valle/Device/Devices/dma.hpp"
#include "Valle/Device/Traits/adc.hpp"
#include "Valle/Device/device_core.hpp"
#include "Valle/Drivers/gpio.hpp"

namespace valle
{

    // ============================================================================
    // CONFIGURATIONS
    // ============================================================================

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
        std::optional<DMAInterruptConfig> interrupts = std::nullopt;
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
        // Sampling time
        ADCChannelSampleTime sampling_time = ADCChannelSampleTime::k24Cycles5;

        // Input mode (Single-Ended/Differential)
        ADCChannelInputMode input_mode = ADCChannelInputMode::kSingleEnded;

        // Optional offset configuration
        std::optional<ADCChannelOffsetConfig> offset = std::nullopt;
    };

    // =============================================================================
    // COMPILE TIME CONFIGURATIONS
    // =============================================================================
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

    // ============================================================================
    // INTERRUPT ROUTER (The Socket)
    // ============================================================================
    enum class ADCInterruptType : uint8_t
    {
        kAnalogWatchdog1 = 0,
        kAnalogWatchdog2,
        kAnalogWatchdog3,
        kEndOfRegularSequence,
        kEndOfInjectSequence,
        kOverrun,
    };

    template <ADCControllerID tkControllerID, ADCInterruptType tkIntType>
    struct ADCInterruptTraits;

#define DEFINE_ADC_INT_TRAIT(tkIntType, ll_name)                                         \
    template <ADCControllerID tkControllerID>                                            \
    struct ADCInterruptTraits<tkControllerID, (tkIntType)>                               \
    {                                                                                    \
        static inline void enable()                                                      \
        {                                                                                \
            LL_ADC_EnableIT_##ll_name(ADCTraits<tkControllerID>::skInstance);            \
        }                                                                                \
        static inline void disable()                                                     \
        {                                                                                \
            LL_ADC_DisableIT_##ll_name(ADCTraits<tkControllerID>::skInstance);           \
        }                                                                                \
        static inline bool is_enabled()                                                  \
        {                                                                                \
            return LL_ADC_IsEnabledIT_##ll_name(ADCTraits<tkControllerID>::skInstance);  \
        }                                                                                \
        static inline bool flag_active()                                                 \
        {                                                                                \
            return LL_ADC_IsActiveFlag_##ll_name(ADCTraits<tkControllerID>::skInstance); \
        }                                                                                \
                                                                                         \
        static inline bool is_pending()                                                  \
        {                                                                                \
            return flag_active() && is_enabled();                                        \
        }                                                                                \
        static inline void ack()                                                         \
        {                                                                                \
            LL_ADC_ClearFlag_##ll_name(ADCTraits<tkControllerID>::skInstance);           \
        }                                                                                \
    };

    DEFINE_ADC_INT_TRAIT(ADCInterruptType::kAnalogWatchdog1, AWD1);
    DEFINE_ADC_INT_TRAIT(ADCInterruptType::kAnalogWatchdog2, AWD2);
    DEFINE_ADC_INT_TRAIT(ADCInterruptType::kAnalogWatchdog3, AWD3);
    DEFINE_ADC_INT_TRAIT(ADCInterruptType::kEndOfRegularSequence, EOS);
    DEFINE_ADC_INT_TRAIT(ADCInterruptType::kEndOfInjectSequence, JEOS);
    DEFINE_ADC_INT_TRAIT(ADCInterruptType::kOverrun, OVR);

#undef DEFINE_ADC_INT_TRAIT

    // ===========================================================================
    // GLOBAL ISR ROUTER
    // ===========================================================================

    /**
     * @brief ADC Global ISR Router
     *
     * Specialize this template to handle all ADC interrupts for a given
     * controller in one function (e.g., when using the ST HAL).
     *
     * @tparam tkControllerID ADC Controller ID (1-5)
     */
    template <ADCControllerID tkControllerID>
        requires(kValidADCControllerID<tkControllerID>)
    struct ADCGlobalISRRouter
    {
        using UnboundIsrHandlerTag = void;
        static void handle()
        {
            // Default: Do nothing (Optimized away)
        }
    };

    // ============================================================================
    // GRANULAR ISR ROUTER
    // ============================================================================

    /**
     * @brief ADC Interrupt Router.
     *
     * @tparam tkControllerID The ADC peripheral index the interrupt belongs to.
     * @tparam tkIntType The interrupt type triggered.
     */
    template <ADCControllerID tkControllerID, ADCInterruptType tkIntType>
        requires(kValidADCControllerID<tkControllerID>)
    struct ADCISRRouter
    {
        using UnboundIsrHandlerTag = void;
        static void handle()
        {
            // Default: Do nothing (Optimized away)
        }
    };

    // =============================================================================
    // FORWARD DECLARATIONS
    // =============================================================================
    class ADCDevice;

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
    class ADCDevice
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
    // ADC PERIPHERAL DEVICE (SHARED DEVICE)
    // ============================================================================
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

        using CTConfigTraitT             = ADCControllerCTConfigTraits<tkControllerID>;
        static constexpr auto skCTConfig = CTConfigTraitT::skConfig;
        using CTConfigT                  = decltype(skCTConfig);
        using DMAChannelT                = CTConfigT::DMAChannelT;
        static constexpr bool skHasDMA   = !CNullDMAChannel<DMAChannelT>;

        using DependDevices = TypeList<ADCDevice>;
        using InjectDevices = std::conditional_t<skHasDMA, TypeList<DMAChannelT>, TypeList<>>;

        using ControllerTraitsT                         = ADCTraits<tkControllerID>;
        static constexpr ADCControllerID skControllerID = tkControllerID;

    private:
        static constexpr uint8_t skChannelRankFreeFlag = 0xFF;

        // --- Storage ---
        alignas(32) volatile ADCValue m_dma_buffer[kADCMaxRegChannels];

        // Mappings
        uint8_t m_reg_cidx_to_rank_map[kADCMaxChannelId];  // Ch -> Rank (1-16) (rank-1 for dma idx)
        uint8_t m_inj_cidx_to_rank_map[kADCMaxChannelId];  // Ch -> Rank (1-4)

        [[no_unique_address]] ConditionalDeviceRef<skHasDMA, DMAChannelT>      m_dma;
        std::conditional_t<skHasDMA, ADCRegularGroupDMAConfig, std::monostate> m_dma_config;

    public:
        ADCControllerDevice(DeviceRef<DMAChannelT>&& dma_channel)
            requires(skHasDMA)

            : m_dma(std::move(dma_channel))
        {
            std::fill(std::begin(m_reg_cidx_to_rank_map), std::end(m_reg_cidx_to_rank_map), skChannelRankFreeFlag);
            std::fill(std::begin(m_inj_cidx_to_rank_map), std::end(m_inj_cidx_to_rank_map), 0);
            std::fill(std::begin(m_dma_buffer), std::end(m_dma_buffer), 0);
        }

        ADCControllerDevice()
            requires(!skHasDMA)
        {
            std::fill(std::begin(m_reg_cidx_to_rank_map), std::end(m_reg_cidx_to_rank_map), skChannelRankFreeFlag);
            std::fill(std::begin(m_inj_cidx_to_rank_map), std::end(m_inj_cidx_to_rank_map), 0);
            std::fill(std::begin(m_dma_buffer), std::end(m_dma_buffer), 0);
        }

        // --- Initialization (Called by Main/Builder) ---

        /**
     * @brief Initialize ADC Peripheral with given configuration.
     *
     * @param config ADC Peripheral Configuration.
     */
        void init(const ADCControllerConfig& config)
        {
            if constexpr (skHasDMA)
            {
                m_dma_config = config.reg.dma;
            }

            // Clock & Power
            LL_AHB2_GRP1_EnableClock(ControllerTraitsT::skClock);

            if (LL_ADC_IsEnabled(ControllerTraitsT::skInstance) == 0)
            {
                LL_ADC_DisableDeepPowerDown(ControllerTraitsT::skInstance);
                LL_ADC_EnableInternalRegulator(ControllerTraitsT::skInstance);
                volatile uint32_t wait = 5000;
                while (wait > 0)
                {
                    wait = wait - 1;
                }
            }

            // Calibration
            LL_ADC_StartCalibration(ControllerTraitsT::skInstance, LL_ADC_SINGLE_ENDED);
            while (LL_ADC_IsCalibrationOnGoing(ControllerTraitsT::skInstance));

            // Global Configuration
            LL_ADC_SetResolution(ControllerTraitsT::skInstance, static_cast<uint32_t>(config.resolution));
            LL_ADC_SetDataAlignment(ControllerTraitsT::skInstance, static_cast<uint32_t>(config.data_alignment));
            LL_ADC_SetLowPowerMode(ControllerTraitsT::skInstance, static_cast<uint32_t>(config.low_power));

            // Inject Group Configuration
            LL_ADC_INJ_SetTriggerSource(ControllerTraitsT::skInstance,
                                        static_cast<uint32_t>(config.inj.trigger_source));
            LL_ADC_INJ_SetTriggerEdge(ControllerTraitsT::skInstance, static_cast<uint32_t>(config.inj.trigger_edge));
            LL_ADC_INJ_SetTrigAuto(ControllerTraitsT::skInstance,
                                   LL_ADC_INJ_TRIG_INDEPENDENT);  // Auto trigger not supported
            LL_ADC_INJ_SetQueueMode(ControllerTraitsT::skInstance,
                                    LL_ADC_INJ_QUEUE_DISABLE);  // Queue mode not supported

            // Regular Group Configuration
            LL_ADC_REG_SetTriggerSource(ControllerTraitsT::skInstance,
                                        static_cast<uint32_t>(config.reg.trigger_source));
            LL_ADC_REG_SetTriggerEdge(ControllerTraitsT::skInstance, static_cast<uint32_t>(config.reg.trigger_edge));
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
        void post_init(const bool start_inject_group = false, const bool start_regular_group = false)
        {
            const uint8_t reg_count = std::find_if(std::begin(m_reg_cidx_to_rank_map),
                                                   std::end(m_reg_cidx_to_rank_map),
                                                   [](uint8_t v) { return v == skChannelRankFreeFlag; }) -
                                      std::begin(m_reg_cidx_to_rank_map);

            // Regular Sequence Config
            if (reg_count > 0)
            {
                LL_ADC_REG_SetSequencerLength(ControllerTraitsT::skInstance,
                                              ADCRegularGroupTraits::get_sequencer_length(reg_count));

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

                    // Configure DMA Channel
                    m_dma->init(DMAChannelConfig{
                        .direction  = DMADirection::kPeriphToMem,
                        .priority   = m_dma_config.priority,
                        .mode       = m_dma_config.circular_mode ? DMAMode::kCircular : DMAMode::kNormal,
                        .data_width = sizeof(ADCValue) == 2 ? DMADataWidth::kHalfWord : DMADataWidth::kWord,
                        .inc_periph = false,
                        .inc_memory = true,
                        .request_id = ControllerTraitsT::skDMARequestId,

                    });
                }
                else
                {
                    LL_ADC_REG_SetDMATransfer(ControllerTraitsT::skInstance,
                                              static_cast<uint32_t>(ADCRegularGroupDMATransfer::kNone));
                }
            }

            const uint8_t inj_count = std::find_if(std::begin(m_inj_cidx_to_rank_map),
                                                   std::end(m_inj_cidx_to_rank_map),
                                                   [](uint8_t v) { return v == 0; }) -
                                      std::begin(m_inj_cidx_to_rank_map);

            // Inject Sequence Config
            if (inj_count > 0)
            {
                LL_ADC_INJ_SetSequencerLength(ControllerTraitsT::skInstance,
                                              ADCInjectGroupTraits::get_sequencer_length(inj_count));

                for (uint32_t i = 0; i < kADCMaxChannelId; ++i)
                {
                    if (m_inj_cidx_to_rank_map[i] != 0)
                    {
                        const uint32_t ch = __LL_ADC_DECIMAL_NB_TO_CHANNEL(i);
                        LL_ADC_INJ_SetSequencerRanks(ControllerTraitsT::skInstance,
                                                     ADCInjectGroupTraits::rank_to_rank_reg(m_inj_cidx_to_rank_map[i]),
                                                     ch);
                    }
                }
            }

            // Enable
            LL_ADC_Enable(ControllerTraitsT::skInstance);
            while (!LL_ADC_IsActiveFlag_ADRDY(ControllerTraitsT::skInstance));

            if (start_inject_group)
            {
                start_inject();
            }

            if (start_regular_group)
            {
                start_regular();
            }
        }

        /**
     * @brief Arms the Inject Sequence.
     * @note Must be called AFTER ADC is enabled (post_init).
     */
        void start_inject()
        {
            ack_inject_eos_int();

            // Enable Inject End of Sequence Interrupt (JEOS)
            // This allows the Vector Table to jump to ADC1_2_IRQHandler
            LL_ADC_EnableIT_JEOS(ControllerTraitsT::skInstance);

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
            if constexpr (skHasDMA)
            {
                if (m_dma_config.interrupts.has_value())
                {
                    m_dma->enable_interrupts(m_dma_config.interrupts.value());
                }
            }

            // Clear any pending flags to avoid immediate false triggers
            ack_regular_eos_int();

            // If not using DMA
            if (!skHasDMA)
            {
                // Enable Regular End of Sequence Interrupt (EOS)
                // This allows the Vector Table to jump to ADC1_2_IRQHandler
                LL_ADC_EnableIT_EOS(ControllerTraitsT::skInstance);
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

        // --- Interrupt Pending Checks ---
        /**
     * @brief Check if Analog Watchdog 1 Interrupt is pending.
     */
        static bool awd1_int_pending()
        {
            return LL_ADC_IsActiveFlag_AWD1(ControllerTraitsT::skInstance) &&
                   LL_ADC_IsEnabledIT_AWD1(ControllerTraitsT::skInstance);
        }

        /**
     * @brief Check if Analog Watchdog 2 Interrupt is pending.
     */
        static bool awd2_int_pending()
        {
            return LL_ADC_IsActiveFlag_AWD2(ControllerTraitsT::skInstance) &&
                   LL_ADC_IsEnabledIT_AWD2(ControllerTraitsT::skInstance);
        }

        /**
     * @brief Check if Analog Watchdog 3 Interrupt is pending.
     */
        static bool awd3_int_pending()
        {
            return LL_ADC_IsActiveFlag_AWD3(ControllerTraitsT::skInstance) &&
                   LL_ADC_IsEnabledIT_AWD3(ControllerTraitsT::skInstance);
        }

        /**
     * @brief Check if end of sequence interrupt is pending for the inject group.
     */
        static bool inject_eos_int_pending()
        {
            return LL_ADC_IsActiveFlag_JEOS(ControllerTraitsT::skInstance) &&
                   LL_ADC_IsEnabledIT_JEOS(ControllerTraitsT::skInstance);
        }

        /**
     * @brief Check if end of sequence interrupt is pending for the regular group.
     */
        static bool regular_eos_int_pending()
        {
            return LL_ADC_IsActiveFlag_EOS(ControllerTraitsT::skInstance) &&
                   LL_ADC_IsEnabledIT_EOS(ControllerTraitsT::skInstance);
        }

        /**
     * @brief Check if Overrun Interrupt is pending.
     */
        static bool ovr_int_pending()
        {
            return LL_ADC_IsActiveFlag_OVR(ControllerTraitsT::skInstance) &&
                   LL_ADC_IsEnabledIT_OVR(ControllerTraitsT::skInstance);
        }

        // --- Interrupt Acknowledgement ---
        /**
     * @brief Acknowledge Analog Watchdog 1 Interrupt.
     *
     */
        static void ack_awd1_int()
        {
            LL_ADC_ClearFlag_AWD1(ControllerTraitsT::skInstance);
        }

        /**
     * @brief Acknowledge Analog Watchdog 2 Interrupt.
     *
     */
        static void ack_awd2_int()
        {
            LL_ADC_ClearFlag_AWD2(ControllerTraitsT::skInstance);
        }

        /**
     * @brief Acknowledge Analog Watchdog 3 Interrupt.
     *
     */
        static void ack_awd3_int()
        {
            LL_ADC_ClearFlag_AWD3(ControllerTraitsT::skInstance);
        }

        /**
     * @brief Acknowledge Inject end of sequence Interrupt.
     *
     */
        static void ack_inject_eos_int()
        {
            LL_ADC_ClearFlag_JEOS(ControllerTraitsT::skInstance);
        }

        /**
     * @brief Acknowledge Regular end of sequence Interrupt.
     *
     */
        static void ack_regular_eos_int()
        {
            LL_ADC_ClearFlag_EOS(ControllerTraitsT::skInstance);
        }

        /**
     * @brief Acknowledge Overrun Interrupt.
     *
     */
        static void ack_ovr_int()
        {
            LL_ADC_ClearFlag_OVR(ControllerTraitsT::skInstance);
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
            if (idx < kADCMaxChannelId && m_inj_cidx_to_rank_map[idx] == 0)
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
            constexpr uint32_t idx = get_adc_channel_idx<tkChannelId>();
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
            const uint8_t buf_idx = rank - 1;
            return m_dma_buffer[buf_idx];
        }
    };

    // ============================================================================
    // ADC CHANNEL (UNIQUE DEVICE)
    // ============================================================================

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
        void init(const ADCChannelConfig& config)
        {
            m_adc.get().template init_channel<tkChannelId, tkGroup, tkRank>(config);
            gpio_init();
        }

        /**
     * @brief Configure and register this channel for the Inject Sequence (Interrupt).
     *
     * @tparam tkRank Rank in Inject Sequence (1-4).
     * @param config Physics configuration (Sampling time, Single/Diff, etc.)
     */
        template <ADCRegularChannelRank tkRank>
        void init_as_regular(const ADCChannelConfig& config)
        {
            m_adc.get().template init_channel_as_regular<skChannelID, tkRank>(config);
            gpio_init();
        }

        /**
     * @brief Configure and register this channel for the Regular Sequence (DMA).
     *
     * @tparam tkRank Rank in Regular Sequence (1-16).
     * @param config Physics configuration (Sampling time, Single/Diff, etc.)
     */
        template <ADCInjectChannelRank tkRank>
        void init_as_inject(const ADCChannelConfig& config)
        {
            m_adc.get().template init_channel_as_inject<skChannelID, tkRank>(config);
            gpio_init();
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
            return m_adc.get().template read_inject_data<tkRank>();
        }

        /**
     * @brief Read data from inject channel (slow method, use read_inject() instead).
     *
     * @return ADCValue Raw ADC value.
     */
        [[nodiscard]] inline ADCValue read_inject_slow() const
        {
            return m_adc.get().template read_inject_data_slow<skChannelID>();
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
            return m_adc.get().template read_regular_data<tkRank>();
        }

        /**
     * @brief Read data from regular channel (slow method, use read_regular() instead).
     *
     * @return ADCValue Raw ADC value.
     */
        [[nodiscard]] inline ADCValue read_regular_slow() const
        {
            return m_adc.get().template read_regular_data_slow<skChannelID>();
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
            return raw_normalized(read_inject<tkRank>());
        }

        /**
     * @brief Read normalized value (0.0 to 1.0) from inject channel (slow method, use read_inject_normalized() instead).
     *
     * @return float Normalized value.
     */
        [[nodiscard]] inline float read_inject_normalized_slow() const
        {
            return raw_normalized(read_inject_slow());
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
            return raw_normalized(read_regular<tkRank>());
        }

        /**
     * @brief Read normalized value (0.0 to 1.0) from regular channel (slow method, use read_regular_normalized() instead).
     *
     * @return float Normalized value.
     */
        [[nodiscard]] inline float read_regular_normalized_slow() const
        {
            return raw_normalized(read_regular_slow());
        }

        /**
     * @brief Convert raw ADC value to normalized float (0.0 to 1.0).
     *
     * @param raw Raw ADC value.
     * @return float Normalized value.
     */
        [[nodiscard]] inline float raw_normalized(const ADCValue raw) const
        {
            return static_cast<float>(raw) / m_adc.get().get_resolution_range();
        }

        /**
     * @brief Convert raw ADC value to voltage (Volts).
     *
     * @param raw Raw ADC value.
     * @return float Voltage in Volts.
     */
        [[nodiscard]] inline float raw_to_voltage(const ADCValue raw)
        {
            return raw_normalized(raw) * 3.3F;
        }

    private:
        /**
     * @brief Initialize GPIO Pin (if pin exists).
     *
     */
        inline void gpio_init()
        {
            if constexpr (skHasPin)
            {
                m_pin.init();
            }
        }
    };

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
        void init(const ADCChannelConfig& config)
        {
            m_channel.get().template init_as_inject<skRank>(config);
        }

        // --- Accessors ---
        /**
     * @brief Read data from inject channel.
     *
     * @return ADCValue Raw ADC value.
     */
        [[nodiscard]] ADCValue read() const
        {
            return m_channel.get().template read_inject<skRank>();
        }

        /**
     * @brief Read voltage from inject channel.
     *
     * @return float Voltage in Volts.
     */
        [[nodiscard]] float read_voltage() const
        {
            return m_channel.get().template read_inject_voltage<skRank>();
        }

        /**
     * @brief Read normalized value from inject channel.
     *
     * @return float Normalized value (0.0 to 1.0).
     */
        [[nodiscard]] float read_normalized() const
        {
            return m_channel.get().template read_inject_normalized<skRank>();
        }
    };

    template <ADCControllerID tkControllerID, ADCChannelID tkChannelId>
        requires(kValidADCChannelID<tkControllerID, tkChannelId>)
    using ADCInjectChannelDevice1 = ADCInjectChannelDevice<tkControllerID, tkChannelId, 1>;

    template <ADCControllerID tkControllerID, ADCChannelID tkChannelId>
        requires(kValidADCChannelID<tkControllerID, tkChannelId>)
    using ADCInjectChannelDevice2 = ADCInjectChannelDevice<tkControllerID, tkChannelId, 2>;
    template <ADCControllerID tkControllerID, ADCChannelID tkChannelId>
        requires(kValidADCChannelID<tkControllerID, tkChannelId>)
    using ADCInjectChannelDevice3 = ADCInjectChannelDevice<tkControllerID, tkChannelId, 3>;

    template <ADCControllerID tkControllerID, ADCChannelID tkChannelId>
        requires(kValidADCChannelID<tkControllerID, tkChannelId>)
    using ADCInjectChannelDevice4 = ADCInjectChannelDevice<tkControllerID, tkChannelId, 4>;

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
    using ADCInjectChannelDeviceSequence = typename detail::MakeInjectSequenceImpl<
        tkControllerID,
        std::make_index_sequence<sizeof...(tkChannelIds)>,  // Generates 0, 1, 2...
        tkChannelIds...>::type;

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
        void init(const ADCChannelConfig& config)
        {
            m_channel.get().template init_as_regular<skRank>(config);
        }

        // --- Accessors ---
        /**
     * @brief Read data from regular channel.
     *
     * @return ADCValue Raw ADC value.
     */
        [[nodiscard]] ADCValue read() const
        {
            return m_channel.get().template read_regular<skRank>();
        }

        /**
     * @brief Read voltage from regular channel.
     *
     * @return float Voltage in Volts.
     */
        [[nodiscard]] float read_voltage() const
        {
            return m_channel.get().template read_regular_voltage<skRank>();
        }

        /**
     * @brief Read normalized value from regular channel.
     *
     * @return float Normalized value (0.0 to 1.0).
     */
        [[nodiscard]] float read_normalized() const
        {
            return m_channel.get().template read_regular_normalized<skRank>();
        }
    };

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
    using ADCRegularChannelDeviceSequence = typename detail::MakeRegularSequenceImpl<
        tkControllerID,
        std::make_index_sequence<sizeof...(tkChannelIds)>,  // Generates 0, 1, 2...
        tkChannelIds...>::type;

}  // namespace valle