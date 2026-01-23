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

// ============================================================================
// CONFIGURATIONS
// ============================================================================

/**
 * @brief Configuration for ADC Injected Group
 *
 */
struct ADCInjectGroupConfig
{
    ADCInjectGroupTriggerSource trigger_source;  /// What triggers the inject group (software or external)
    ADCInjectGroupTriggerEdge   trigger_edge;    /// What edge to trigger on (only for external triggers)

    [[nodiscard]] static constexpr ADCInjectGroupConfig default_config()
    {
        return ADCInjectGroupConfig{
            .trigger_source = ADCInjectGroupTriggerSource::kSoftware,
            .trigger_edge   = ADCInjectGroupTriggerEdge::kRising,
        };
    }
};

/**
 * @brief Configuration for ADC Regular Group DMA Transfer
 *
 */
struct ADCRegularGroupDMAConfig
{
    DMAPriority                       priority;       /// DMA Channel Priority
    bool                              circular_mode;  /// True = Continuous/Circular, False = One-Shot/Normal
    std::optional<DMAInterruptConfig> interrupts;     /// DMA Interrupt configuration

    [[nodiscard]] static inline constexpr ADCRegularGroupDMAConfig default_config()
    {
        return ADCRegularGroupDMAConfig{
            .priority = DMAPriority::kMedium, .circular_mode = true, .interrupts = std::nullopt};
    }
};

/**
 * @brief Configuration for ADC Regular Group (Background/DMA).
 *
 */
struct ADCRegularGroupConfig
{
    ADCRegularGroupTriggerSource    trigger_source;     /// Source of trigger (Software/External)
    ADCRegularGroupTriggerEdge      trigger_edge;       /// What edge causes trigger (Rising/Falling/Both)
    ADCRegularGroupDMAConfig        dma;                /// DMA Transfer configuration
    ADCRegularGroupOverrunBehavior  overrun;            /// Overrun behavior: Preserve vs Overwrite
    ADCRegularGroupConversionMode   conversion_mode;    /// Conversion mode: Single Shot vs Continuous
    ADCRegularGroupOversamplingMode oversampling_mode;  /// Oversampling mode: Continuous vs discontinuous

    [[nodiscard]] static constexpr ADCRegularGroupConfig default_config()
    {
        return ADCRegularGroupConfig{.trigger_source    = ADCRegularGroupTriggerSource::kSoftware,
                                     .trigger_edge      = ADCRegularGroupTriggerEdge::kRising,
                                     .dma               = ADCRegularGroupDMAConfig::default_config(),
                                     .overrun           = ADCRegularGroupOverrunBehavior::kOverwrite,
                                     .conversion_mode   = ADCRegularGroupConversionMode::kSingleShot,
                                     .oversampling_mode = ADCRegularGroupOversamplingMode::kDiscontinuous};
    }
};

/**
 * @brief Configuration for ADC Oversampling (Hardware Averaging).
 *
 */
struct ADCOversamplingConfig
{
    ADCOversamplingRatio     ratio;  /// How many times to sample (2x to 256x)
    ADCOversamplingShiftBits shift;  /// Right shift bits (0 to 8)
    ADCOversamplingScope     scope;  /// Oversampling behavior when both groups are selected
};

/**
 * @brief Configuration for the ADC Peripheral (Global).
 */
struct ADCPeripheralConfig
{
    // --- Core Settings ---
    ADCResolution    resolution;      // Sampling resolution (12, 10, 8, 6 bits)
    ADCDataAlignment data_alignment;  // Right/Left data alignment
    ADCLowPowerMode  low_power;       // Low power mode

    // --- Inject Group (High Priority/Interrupt) ---
    ADCInjectGroupConfig inj;

    // --- Regular Group (Background/DMA) ---
    ADCRegularGroupConfig reg;

    // --- Oversampling (Hardware Averaging) ---
    std::optional<ADCOversamplingConfig> oversampling;

    [[nodiscard]] static constexpr ADCPeripheralConfig default_config()
    {
        return ADCPeripheralConfig{
            .resolution     = ADCResolution::k12Bit,
            .data_alignment = ADCDataAlignment::kRight,
            .low_power      = ADCLowPowerMode::kNone,
            .inj            = ADCInjectGroupConfig::default_config(),
            .reg            = ADCRegularGroupConfig::default_config(),
            .oversampling   = std::nullopt  // No oversampling by default
        };
    }
};

struct ADCChannelOffsetConfig
{
    ADCChannelOffsetIdx idx;    /// Offset Index (1 to 4)
    uint32_t            value;  /// Offset Value (Raw ADC counts)
};

/**
 * @brief Configuration for a single ADC Channel (Physics).
 */
struct ADCChannelConfig
{
    ADCChannelSampleTime                  sampling_time;  /// Sampling time
    ADCChannelInputMode                   input_mode;     /// Input mode (Single-Ended/Differential)
    std::optional<ADCChannelOffsetConfig> offset;         /// Optional offset configuration

    [[nodiscard]] static constexpr ADCChannelConfig default_config()
    {
        return ADCChannelConfig{
            .sampling_time = ADCChannelSampleTime::k24Cycles5,
            .input_mode    = ADCChannelInputMode::kSingleEnded,
            .offset        = std::nullopt  // No offset by default
        };
    };
};

// =============================================================================
// COMPILE TIME CONFIGURATIONS
// =============================================================================

template <uint8_t tkADCIdx>
    requires(kValidADCIndex<tkADCIdx>)
struct ADCPeripheralCTConfigTraits
{
    using NullDMATag = void;
};

template <typename T>
concept CNullDMAConfig = requires { typename T::NullDMATag; };

// =============================================================================
// FORWARD DECLARATIONS
// =============================================================================
class ADCDevice;

template <uint8_t tkADCIdx>
    requires(kValidADCIndex<tkADCIdx>)
class ADCPeripheralDevice;

template <uint8_t tkADCIdx, ADCChannelId tkChannelId>
    requires(kValidADCChannel<tkADCIdx, tkChannelId>)
class ADCChannelDevice;

template <uint8_t tkADCIdx, ADCChannelId tkChannelId, uint8_t tkRank>
    requires(kValidADCChannel<tkADCIdx, tkChannelId> && kValidADCInjectRank<tkRank>)
class ADCInjectChannelDevice;

template <uint8_t tkADCIdx, ADCChannelId tkChannelId, uint8_t tkRank>
    requires(kValidADCChannel<tkADCIdx, tkChannelId> && kValidADCRegularRank<tkRank>)
class ADCRegularChannelDevice;

using ADCValue = uint16_t;

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
        using Children = DeviceList<ADCPeripheralDevice<1>,
                                    ADCPeripheralDevice<2>,
                                    ADCPeripheralDevice<3>,
                                    ADCPeripheralDevice<4>,
                                    ADCPeripheralDevice<5>>;
    };
};

// ============================================================================
// ADC PERIPHERAL DEVICE (SHARED DEVICE)
// ============================================================================

namespace detail
{
    template <typename T>
    struct GetADCDMAChannel;

    template <CNullDMAConfig T>
    struct GetADCDMAChannel<T>
    {
        using type = GPIONullPinDevice;
    };

    template <typename T>
        requires(!CNullDMAConfig<T>)
    struct GetADCDMAChannel<T>
    {
        using type = DMAChannelDevice<T::skDMAIdx, T::skDMAChannelIdx>;
    };
}  // namespace detail

template <uint8_t tkADCIdx>
    requires(kValidADCIndex<tkADCIdx>)
class ADCPeripheralDevice
{
public:
    struct Descriptor : public SharedDeviceDescriptor
    {
        using Children = DeviceList<ADCChannelDevice<tkADCIdx, ADCChannelId::kChannel0>,
                                    ADCChannelDevice<tkADCIdx, ADCChannelId::kChannel1>,
                                    ADCChannelDevice<tkADCIdx, ADCChannelId::kChannel2>,
                                    ADCChannelDevice<tkADCIdx, ADCChannelId::kChannel3>,
                                    ADCChannelDevice<tkADCIdx, ADCChannelId::kChannel4>,
                                    ADCChannelDevice<tkADCIdx, ADCChannelId::kChannel5>,
                                    ADCChannelDevice<tkADCIdx, ADCChannelId::kChannel6>,
                                    ADCChannelDevice<tkADCIdx, ADCChannelId::kChannel7>,
                                    ADCChannelDevice<tkADCIdx, ADCChannelId::kChannel8>,
                                    ADCChannelDevice<tkADCIdx, ADCChannelId::kChannel9>,
                                    ADCChannelDevice<tkADCIdx, ADCChannelId::kChannel10>,
                                    ADCChannelDevice<tkADCIdx, ADCChannelId::kChannel11>,
                                    ADCChannelDevice<tkADCIdx, ADCChannelId::kChannel12>,
                                    ADCChannelDevice<tkADCIdx, ADCChannelId::kChannel13>,
                                    ADCChannelDevice<tkADCIdx, ADCChannelId::kChannel14>,
                                    ADCChannelDevice<tkADCIdx, ADCChannelId::kChannel15>,
                                    ADCChannelDevice<tkADCIdx, ADCChannelId::kChannel16>,
                                    ADCChannelDevice<tkADCIdx, ADCChannelId::kChannel17>,
                                    ADCChannelDevice<tkADCIdx, ADCChannelId::kChannel18>,
                                    ADCChannelDevice<tkADCIdx, ADCChannelId::kChannelVRefInt>,
                                    ADCChannelDevice<tkADCIdx, ADCChannelId::kChannelTempSensorADC1>,
                                    ADCChannelDevice<tkADCIdx, ADCChannelId::kChannelTempSensorADC5>,
                                    ADCChannelDevice<tkADCIdx, ADCChannelId::kChannelVBat>,
                                    ADCChannelDevice<tkADCIdx, ADCChannelId::kChannelVOPAmp1>,
                                    ADCChannelDevice<tkADCIdx, ADCChannelId::kChannelVOPAmp2>,
                                    ADCChannelDevice<tkADCIdx, ADCChannelId::kChannelVOPAmp3ADC2>,
                                    ADCChannelDevice<tkADCIdx, ADCChannelId::kChannelVOPAmp3ADC3>,
                                    ADCChannelDevice<tkADCIdx, ADCChannelId::kChannelVOPAmp4>,
                                    ADCChannelDevice<tkADCIdx, ADCChannelId::kChannelVOPAmp5>,
                                    ADCChannelDevice<tkADCIdx, ADCChannelId::kChannelVOPAmp6>>;
    };

    using Traits                   = ADCTraits<tkADCIdx>;
    using CTConfig                 = ADCPeripheralCTConfigTraits<tkADCIdx>;
    using DMAChannelT              = typename detail::GetADCDMAChannel<CTConfig>::type;
    static constexpr bool skHasDMA = !CNullDMAChannel<DMAChannelT>;

    using DependDevices = TypeList<ADCDevice>;
    using InjectDevices = std::conditional_t<skHasDMA, TypeList<DMAChannelT>, TypeList<>>;

    static constexpr uint8_t kADCIdx = tkADCIdx;

private:
    static constexpr uint8_t   skChannelRankFreeFlag = 0xFF;
    static inline ADC_TypeDef* skADCInstance         = Traits::skInstance;

    // --- Storage ---
    alignas(32) volatile ADCValue m_dma_buffer[kADCMaxRegChannels];

    // Mappings
    uint8_t m_reg_cidx_to_rank_map[kADCMaxChannelId];  // Ch -> Rank (1-16) (rank-1 for dma idx)
    uint8_t m_inj_cidx_to_rank_map[kADCMaxChannelId];  // Ch -> Rank (1-4)

    std::conditional_t<skHasDMA, DeviceRef<DMAChannelT>, std::monostate>   m_dma;
    std::conditional_t<skHasDMA, ADCRegularGroupDMAConfig, std::monostate> m_dma_config;

public:
    ADCPeripheralDevice(DeviceRef<DMAChannelT>&& dma_channel)
        requires(skHasDMA)

        : m_dma(std::move(dma_channel))
    {
        std::fill(std::begin(m_reg_cidx_to_rank_map), std::end(m_reg_cidx_to_rank_map), skChannelRankFreeFlag);
        std::fill(std::begin(m_inj_cidx_to_rank_map), std::end(m_inj_cidx_to_rank_map), 0);
        std::fill(std::begin(m_dma_buffer), std::end(m_dma_buffer), 0);
    }

    ADCPeripheralDevice()
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
    void init(const ADCPeripheralConfig& config)
    {
        if constexpr (skHasDMA)
        {
            m_dma_config = config.reg.dma;
        }

        // Clock & Power
        LL_AHB2_GRP1_EnableClock(Traits::skClock);

        if (LL_ADC_IsEnabled(skADCInstance) == 0)
        {
            LL_ADC_DisableDeepPowerDown(skADCInstance);
            LL_ADC_EnableInternalRegulator(skADCInstance);
            volatile uint32_t wait = 5000;
            while (wait > 0)
            {
                wait = wait - 1;
            }
        }

        // Calibration
        LL_ADC_StartCalibration(skADCInstance, LL_ADC_SINGLE_ENDED);
        while (LL_ADC_IsCalibrationOnGoing(skADCInstance));

        // Global Configuration
        LL_ADC_SetResolution(skADCInstance, static_cast<uint32_t>(config.resolution));
        LL_ADC_SetDataAlignment(skADCInstance, static_cast<uint32_t>(config.data_alignment));
        LL_ADC_SetLowPowerMode(skADCInstance, static_cast<uint32_t>(config.low_power));

        // Inject Group Configuration
        LL_ADC_INJ_SetTriggerSource(skADCInstance, static_cast<uint32_t>(config.inj.trigger_source));
        LL_ADC_INJ_SetTriggerEdge(skADCInstance, static_cast<uint32_t>(config.inj.trigger_edge));
        LL_ADC_INJ_SetTrigAuto(skADCInstance, LL_ADC_INJ_TRIG_INDEPENDENT);  // Auto trigger not supported
        LL_ADC_INJ_SetQueueMode(skADCInstance, LL_ADC_INJ_QUEUE_DISABLE);    // Queue mode not supported

        // Regular Group Configuration
        LL_ADC_REG_SetTriggerSource(skADCInstance, static_cast<uint32_t>(config.reg.trigger_source));
        LL_ADC_REG_SetTriggerEdge(skADCInstance, static_cast<uint32_t>(config.reg.trigger_edge));
        LL_ADC_REG_SetOverrun(skADCInstance, static_cast<uint32_t>(config.reg.overrun));
        LL_ADC_REG_SetContinuousMode(skADCInstance, static_cast<uint32_t>(config.reg.conversion_mode));

        // Oversampling Configuration
        if (config.oversampling.has_value())
        {
            ADCOversamplingConfig ovs_cfg = config.oversampling.value();
            LL_ADC_SetOverSamplingScope(skADCInstance, static_cast<uint32_t>(ovs_cfg.scope));
            LL_ADC_ConfigOverSamplingRatioShift(
                skADCInstance, static_cast<uint32_t>(ovs_cfg.ratio), static_cast<uint32_t>(ovs_cfg.shift));
            LL_ADC_SetOverSamplingDiscont(skADCInstance, static_cast<uint32_t>(config.reg.oversampling_mode));
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
    template <ADCChannelId tkChannelId, ADCChannelGroup tkGroup, uint8_t tkRank>
        requires(kValidADCChannel<tkADCIdx, tkChannelId>)
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
    template <ADCChannelId tkChannelId, uint8_t tkRank>
        requires(kValidADCChannel<tkADCIdx, tkChannelId> && kValidADCInjectRank<tkRank>)
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
    template <ADCChannelId tkChannelId, uint8_t tkRank>
        requires(kValidADCChannel<tkADCIdx, tkChannelId> && kValidADCRegularRank<tkRank>)
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
            LL_ADC_REG_SetSequencerLength(skADCInstance, ADCRegularGroupTraits::get_sequencer_length(reg_count));

            for (uint32_t i = 0; i < kADCMaxChannelId; ++i)
            {
                if (m_reg_cidx_to_rank_map[i] != skChannelRankFreeFlag)
                {
                    const uint32_t ch = __LL_ADC_DECIMAL_NB_TO_CHANNEL(i);
                    LL_ADC_REG_SetSequencerRanks(
                        skADCInstance, ADCRegularGroupTraits::rank_to_rank_reg(m_reg_cidx_to_rank_map[i]), ch);
                }
            }

            if constexpr (skHasDMA)
            {
                ADCRegularGroupDMATransfer dma_transfer = m_dma_config.circular_mode
                                                              ? ADCRegularGroupDMATransfer::kUnlimited
                                                              : ADCRegularGroupDMATransfer::kLimited;
                LL_ADC_REG_SetDMATransfer(skADCInstance, static_cast<uint32_t>(dma_transfer));

                static_assert(sizeof(ADCValue) == 2 || sizeof(ADCValue) == 4, "ADCValue must be 2 or 4 bytes");

                // Configure DMA Channel
                m_dma->init(DMAChannelConfig{
                    .direction  = DMADirection::kPeriphToMem,
                    .priority   = m_dma_config.priority,
                    .mode       = m_dma_config.circular_mode ? DMAMode::kCircular : DMAMode::kNormal,
                    .data_width = sizeof(ADCValue) == 2 ? DMADataWidth::kHalfWord : DMADataWidth::kWord,
                    .inc_periph = false,
                    .inc_memory = true,
                    .request_id = Traits::skDMARequestId,

                });
            }
            else
            {
                LL_ADC_REG_SetDMATransfer(skADCInstance, static_cast<uint32_t>(ADCRegularGroupDMATransfer::kNone));
            }
        }

        const uint8_t inj_count = std::find_if(std::begin(m_inj_cidx_to_rank_map),
                                               std::end(m_inj_cidx_to_rank_map),
                                               [](uint8_t v)
        { return v == 0; }) - std::begin(m_inj_cidx_to_rank_map);

        // Inject Sequence Config
        if (inj_count > 0)
        {
            LL_ADC_INJ_SetSequencerLength(skADCInstance, ADCInjectGroupTraits::get_sequencer_length(inj_count));

            for (uint32_t i = 0; i < kADCMaxChannelId; ++i)
            {
                if (m_inj_cidx_to_rank_map[i] != 0)
                {
                    const uint32_t ch = __LL_ADC_DECIMAL_NB_TO_CHANNEL(i);
                    LL_ADC_INJ_SetSequencerRanks(
                        skADCInstance, ADCInjectGroupTraits::rank_to_rank_reg(m_inj_cidx_to_rank_map[i]), ch);
                }
            }
        }

        // Enable
        LL_ADC_Enable(skADCInstance);
        while (!LL_ADC_IsActiveFlag_ADRDY(skADCInstance));

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
        LL_ADC_EnableIT_JEOS(skADCInstance);

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
        LL_ADC_INJ_StartConversion(skADCInstance);
    }

    /**
     * @brief Stop Inject Conversions.
     *
     */
    void stop_inject()
    {
        LL_ADC_INJ_StopConversion(skADCInstance);
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
            LL_ADC_EnableIT_EOS(skADCInstance);
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
        LL_ADC_REG_StartConversion(skADCInstance);
    }

    /**
     * @brief Stop Regular Conversions.
     *
     */
    void stop_regular()
    {
        LL_ADC_REG_StopConversion(skADCInstance);
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
        return LL_ADC_IsActiveFlag_AWD1(skADCInstance) && LL_ADC_IsEnabledIT_AWD1(skADCInstance);
    }

    /**
     * @brief Check if Analog Watchdog 2 Interrupt is pending.
     */
    static bool awd2_int_pending()
    {
        return LL_ADC_IsActiveFlag_AWD2(skADCInstance) && LL_ADC_IsEnabledIT_AWD2(skADCInstance);
    }

    /**
     * @brief Check if Analog Watchdog 3 Interrupt is pending.
     */
    static bool awd3_int_pending()
    {
        return LL_ADC_IsActiveFlag_AWD3(skADCInstance) && LL_ADC_IsEnabledIT_AWD3(skADCInstance);
    }

    /**
     * @brief Check if end of sequence interrupt is pending for the inject group.
     */
    static bool inject_eos_int_pending()
    {
        return LL_ADC_IsActiveFlag_JEOS(skADCInstance) && LL_ADC_IsEnabledIT_JEOS(skADCInstance);
    }

    /**
     * @brief Check if end of sequence interrupt is pending for the regular group.
     */
    static bool regular_eos_int_pending()
    {
        return LL_ADC_IsActiveFlag_EOS(skADCInstance) && LL_ADC_IsEnabledIT_EOS(skADCInstance);
    }

    /**
     * @brief Check if Overrun Interrupt is pending.
     */
    static bool ovr_int_pending()
    {
        return LL_ADC_IsActiveFlag_OVR(skADCInstance) && LL_ADC_IsEnabledIT_OVR(skADCInstance);
    }

    // --- Interrupt Acknowledgement ---
    /**
     * @brief Acknowledge Analog Watchdog 1 Interrupt.
     *
     */
    static void ack_awd1_int()
    {
        LL_ADC_ClearFlag_AWD1(skADCInstance);
    }

    /**
     * @brief Acknowledge Analog Watchdog 2 Interrupt.
     *
     */
    static void ack_awd2_int()
    {
        LL_ADC_ClearFlag_AWD2(skADCInstance);
    }

    /**
     * @brief Acknowledge Analog Watchdog 3 Interrupt.
     *
     */
    static void ack_awd3_int()
    {
        LL_ADC_ClearFlag_AWD3(skADCInstance);
    }

    /**
     * @brief Acknowledge Inject end of sequence Interrupt.
     *
     */
    static void ack_inject_eos_int()
    {
        LL_ADC_ClearFlag_JEOS(skADCInstance);
    }

    /**
     * @brief Acknowledge Regular end of sequence Interrupt.
     *
     */
    static void ack_regular_eos_int()
    {
        LL_ADC_ClearFlag_EOS(skADCInstance);
    }

    /**
     * @brief Acknowledge Overrun Interrupt.
     *
     */
    static void ack_ovr_int()
    {
        LL_ADC_ClearFlag_OVR(skADCInstance);
    }

    // --- Resolution Info ---
    [[nodiscard]] static inline float get_resolution_bits()
    {
        switch (LL_ADC_GetResolution(skADCInstance))
        {
            case LL_ADC_RESOLUTION_12B:
                return 12.0f;
            case LL_ADC_RESOLUTION_10B:
                return 10.0f;
            case LL_ADC_RESOLUTION_8B:
                return 8.0f;
            case LL_ADC_RESOLUTION_6B:
                return 6.0f;
            default:
                return 12.0f;
        }
    }

    [[nodiscard]] static inline float get_resolution_range()
    {
        switch (LL_ADC_GetResolution(skADCInstance))
        {
            case LL_ADC_RESOLUTION_12B:
                return 4095.0f;
            case LL_ADC_RESOLUTION_10B:
                return 1023.0f;
            case LL_ADC_RESOLUTION_8B:
                return 255.0f;
            case LL_ADC_RESOLUTION_6B:
                return 63.0f;
            default:
                return 4095.0f;  // assume 12-bit
        }
    }

    // --- Data Access ---
    /**
     * @brief Read data from Regular Channel.
     *
     * @tparam tkRank Rank in Regular Sequence (1-16).
     * @return ADCValue Raw ADC value.
     */
    template <uint8_t tkRank>
        requires(kValidADCInjectRank<tkRank>)
    [[nodiscard]] constexpr inline ADCValue read_inject_data() const
    {
        return LL_ADC_INJ_ReadConversionData32(skADCInstance, ADCInjectGroupTraits::rank_to_rank_reg(tkRank));
    }

    /**
     * @brief Read data from Inject Channel (Slow).
     *
     * @tparam tkChannelId Channel ID to read.
     * @return ADCValue Raw ADC value.
     */
    template <ADCChannelId tkChannelId>
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
    template <uint8_t tkRank>
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
    template <ADCChannelId tkChannelId>
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
    template <ADCChannelId tkChannelId>
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
    template <ADCChannelId tkChannelId>
    [[nodiscard]] constexpr inline uint8_t get_adc_channel_rank_regular() const
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
    template <ADCChannelId tkChannelId>
    [[nodiscard]] constexpr inline uint8_t get_adc_channel_rank_inject() const
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
    template <ADCChannelId tkChannelId>
        requires(kValidADCChannel<tkADCIdx, tkChannelId>)
    void init_channel_core(const ADCChannelConfig& cfg)
    {
        LL_ADC_SetChannelSamplingTime(
            skADCInstance, static_cast<uint32_t>(tkChannelId), static_cast<uint32_t>(cfg.sampling_time));
        LL_ADC_SetChannelSingleDiff(
            skADCInstance, static_cast<uint32_t>(tkChannelId), static_cast<uint32_t>(cfg.input_mode));

        if (cfg.offset.has_value())
        {
            ADCChannelOffsetConfig offset_cfg = cfg.offset.value();

            LL_ADC_SetOffset(skADCInstance,
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
    template <ADCChannelId tkChannelId, uint8_t tkRank>
        requires(kValidADCChannel<tkADCIdx, tkChannelId> && kValidADCInjectRank<tkRank>)
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
    template <ADCChannelId tkChannelId, uint8_t tkRank>
        requires(kValidADCChannel<tkADCIdx, tkChannelId> && kValidADCRegularRank<tkRank>)
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
    [[nodiscard]] inline ADCValue read_inject_data(const uint8_t rank) const
    {
        return LL_ADC_INJ_ReadConversionData32(Traits::skInstance, ADCInjectGroupTraits::rank_to_rank_reg(rank));
    }

    /**
     * @brief Read data from regular channel.
     *
     * @param rank Rank in Regular Sequence (1-16).
     * @return ADCValue Raw ADC value.
     */
    [[nodiscard]] constexpr inline ADCValue read_regular_data(const uint8_t rank) const
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
    using type = void;
};

template <typename T>
struct ADCPinDevice
{
    using type = GPIOPinDevice<T::skPort, T::skPin>;
};

template <uint8_t tkADCIdx, ADCChannelId tkChannelId>
    requires(kValidADCChannel<tkADCIdx, tkChannelId>)
class ADCChannelDevice
{
public:
    struct Descriptor : public UniqueDeviceDescriptor
    {
    };

    using PeripheralT              = ADCPeripheralDevice<tkADCIdx>;
    using PinT                     = typename ADCPinDevice<ADCPinMap<tkADCIdx, tkChannelId>>::type;
    static constexpr bool skHasPin = !CNullPin<PinT>;

    using InjectDevices = std::conditional_t<skHasPin, TypeList<PeripheralT, PinT>, TypeList<PeripheralT>>;

    static constexpr uint8_t      skADCIdx    = tkADCIdx;
    static constexpr ADCChannelId skChannelId = tkChannelId;

    template <uint8_t tkAIdx, ADCChannelId tkCID, uint8_t tkRank>
        requires(kValidADCChannel<tkAIdx, tkCID> && kValidADCInjectRank<tkRank>)
    friend class ADCInjectChannelDevice;

    template <uint8_t tkAIdx, ADCChannelId tkCID, uint8_t tkRank>
        requires(kValidADCChannel<tkAIdx, tkCID> && kValidADCRegularRank<tkRank>)
    friend class ADCRegularChannelDevice;

private:
    DeviceRef<PeripheralT>                                                 m_adc;
    std::conditional_t<skHasPin, GPIOAnalogInDriver<PinT>, std::monostate> m_pin;

public:
    explicit ADCChannelDevice(DeviceRef<PeripheralT>&& adc, DeviceRef<PinT>&& pin)
        requires(skHasPin)
        : m_adc(std::move(adc)), m_pin(std::move(pin))
    {
    }

    explicit ADCChannelDevice(DeviceRef<PeripheralT> adc)
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
    void init(const ADCChannelConfig& config = ADCChannelConfig::default_config())
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
    template <uint8_t tkRank>
    void init_as_regular(const ADCChannelConfig& config)
    {
        m_adc.get().template init_channel_as_regular<skChannelId, tkRank>(config);
        gpio_init();
    }

    /**
     * @brief Configure and register this channel for the Regular Sequence (DMA).
     *
     * @tparam tkRank Rank in Regular Sequence (1-16).
     * @param config Physics configuration (Sampling time, Single/Diff, etc.)
     */
    template <uint8_t tkRank>
    void init_as_inject(const ADCChannelConfig& config)
    {
        m_adc.get().template init_channel_as_inject<skChannelId, tkRank>(config);
        gpio_init();
    }

    // --- Accessors ---
    // Read raw
    /**
     * @brief Read data from inject channel.
     *
     * @tparam tkRank Rank in Inject Sequence (1-4).
     */
    template <uint8_t tkRank>
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
        return m_adc.get().template read_inject_data_slow<skChannelId>();
    }

    /**
     * @brief Read data from regular channel.
     *
     * @tparam tkRank Rank in Regular Sequence (1-16).
     */
    template <uint8_t tkRank>
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
        return m_adc.get().template read_regular_data_slow<skChannelId>();
    }

    // Read voltage
    /**
     * @brief Read voltage from inject channel.
     *
     * @tparam tkRank Rank in Inject Sequence (1-4).
     * @return float Voltage in Volts.
     */
    template <uint8_t tkRank>
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
    template <uint8_t tkRank>
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
    template <uint8_t tkRank>
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
    template <uint8_t tkRank>
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
        return raw_normalized(raw) * 3.3f;
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

template <uint8_t tkADCIdx, ADCChannelId tkChannelId, uint8_t tkRank>
    requires(kValidADCChannel<tkADCIdx, tkChannelId> &&
             kValidADCInjectRank<tkRank>)  // Only 4 inject channels supported
class ADCInjectChannelDevice
{
public:
    struct Descriptor : public UniqueDeviceDescriptor
    {
    };

    using ChannelT                               = ADCChannelDevice<tkADCIdx, tkChannelId>;
    using InjectDevices                          = TypeList<ChannelT>;
    static constexpr uint8_t         skADCIdx    = tkADCIdx;
    static constexpr ADCChannelId    skChannelId = tkChannelId;
    static constexpr ADCChannelGroup skGroup     = ADCChannelGroup::kInject;
    static constexpr uint8_t         skRank      = tkRank;

private:
    DeviceRef<ChannelT> m_channel;

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

template <uint8_t tkADCIdx, ADCChannelId tkChannelId>
    requires(kValidADCChannel<tkADCIdx, tkChannelId>)
using ADCInjectChannelDevice1 = ADCInjectChannelDevice<tkADCIdx, tkChannelId, 1>;

template <uint8_t tkADCIdx, ADCChannelId tkChannelId>
    requires(kValidADCChannel<tkADCIdx, tkChannelId>)
using ADCInjectChannelDevice2 = ADCInjectChannelDevice<tkADCIdx, tkChannelId, 2>;
template <uint8_t tkADCIdx, ADCChannelId tkChannelId>
    requires(kValidADCChannel<tkADCIdx, tkChannelId>)
using ADCInjectChannelDevice3 = ADCInjectChannelDevice<tkADCIdx, tkChannelId, 3>;

template <uint8_t tkADCIdx, ADCChannelId tkChannelId>
    requires(kValidADCChannel<tkADCIdx, tkChannelId>)
using ADCInjectChannelDevice4 = ADCInjectChannelDevice<tkADCIdx, tkChannelId, 4>;

namespace detail
{
    // Helper to unpack the Index Sequence alongside the Channel Pack
    template <uint8_t tkADCIdx, typename TIndexSeq, ADCChannelId... tkChannelIds>
    struct MakeInjectSequenceImpl;

    template <uint8_t tkADCIdx, size_t... Is, ADCChannelId... tkChannelIds>
    struct MakeInjectSequenceImpl<tkADCIdx, std::index_sequence<Is...>, tkChannelIds...>
    {
        // Expand both packs: Channel[i] paired with Rank[i + 1]
        using type = TypeList<ADCInjectChannelDevice<tkADCIdx, tkChannelIds, (Is + 1)>...>;
    };
}  // namespace detail

template <uint8_t tkADCIdx, ADCChannelId... tkChannelIds>
    requires((kValidADCChannel<tkADCIdx, tkChannelIds> && ...) && (sizeof...(tkChannelIds) <= kADCMaxInjChannels))
using ADCInjectChannelDeviceSequence =
    typename detail::MakeInjectSequenceImpl<tkADCIdx,
                                            std::make_index_sequence<sizeof...(tkChannelIds)>,  // Generates 0, 1, 2...
                                            tkChannelIds...>::type;

// ============================================================================
// ADC REGULAR CHANNEL (UNIQUE DEVICE)
// ============================================================================

template <uint8_t tkADCIdx, ADCChannelId tkChannelId, uint8_t tkRank>
    requires(kValidADCChannel<tkADCIdx, tkChannelId> &&
             kValidADCRegularRank<tkRank>)  // Only 16 regular channels supported
class ADCRegularChannelDevice
{
public:
    struct Descriptor : public UniqueDeviceDescriptor
    {
    };

    using ChannelT                               = ADCChannelDevice<tkADCIdx, tkChannelId>;
    using InjectDevices                          = TypeList<ChannelT>;
    static constexpr uint8_t         skADCIdx    = tkADCIdx;
    static constexpr ADCChannelId    skChannelId = tkChannelId;
    static constexpr ADCChannelGroup skGroup     = ADCChannelGroup::kRegular;
    static constexpr uint8_t         skRank      = tkRank;

private:
    DeviceRef<ChannelT> m_channel;

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
    template <uint8_t tkADCIdx, typename TIndexSeq, ADCChannelId... tkChannelIds>
    struct MakeRegularSequenceImpl;

    template <uint8_t tkADCIdx, size_t... Is, ADCChannelId... tkChannelIds>
    struct MakeRegularSequenceImpl<tkADCIdx, std::index_sequence<Is...>, tkChannelIds...>
    {
        // Expand both packs: Channel[i] paired with Rank[i + 1]
        using type = TypeList<ADCRegularChannelDevice<tkADCIdx, tkChannelIds, (Is + 1)>...>;
    };
}  // namespace detail

template <uint8_t tkADCIdx, ADCChannelId... tkChannelIds>
    requires((kValidADCChannel<tkADCIdx, tkChannelIds> && ...) && (sizeof...(tkChannelIds) <= kADCMaxRegChannels))
using ADCRegularChannelDeviceSequence =
    typename detail::MakeRegularSequenceImpl<tkADCIdx,
                                             std::make_index_sequence<sizeof...(tkChannelIds)>,  // Generates 0, 1, 2...
                                             tkChannelIds...>::type;

// ============================================================================
// INTERRUPT ROUTER (The Socket)
// ============================================================================
enum class ADCInterruptType : uint8_t
{
    kAnalogWatchdog = 0,
    kEndOfRegularSequence,
    kEndOfInjectSequence,
    kOverrun,
};

/**
 * @brief ADC Interrupt Router.
 *
 * @tparam tkADCIdx The ADC peripheral index the interrupt belongs to.
 * @tparam tkIntType The interrupt type triggered.
 */
template <uint8_t tkADCIdx, ADCInterruptType tkIntType>
    requires(kValidADCIndex<tkADCIdx>)
struct ADCIsrRouter
{
    static void handle()
    {
        // Default: Do nothing (Optimized away)
    }
};
