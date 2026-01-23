#pragma once

#include "Valle/Device/Traits/dma.hpp"
#include "Valle/Device/device_core.hpp"
#include "stm32g4xx_ll_bus.h"

// ============================================================================
// CONFIGURATIONS
// ============================================================================

/**
 * @brief Configuration for DMA Interrupts.
 */
struct DMAInterruptConfig
{
    uint32_t priority;   // NVIC Priority (0 = Highest, 15 = Lowest)
    bool     enable_tc;  // Transfer Complete Interrupt Enable
    bool     enable_ht;  // Half Transfer Interrupt Enable
    bool     enable_te;  // Transfer Error Interrupt Enable
};

/**
 * @brief Configuration for a specific DMA Channel Transfer.
 */
struct DMAChannelConfig
{
    DMADirection direction;
    DMAPriority  priority;
    DMAMode      mode;
    DMADataWidth data_width;  // Applied to both source and dest for simplicity

    // Increment settings
    bool inc_periph;  // Typically FALSE for ADC/SPI
    bool inc_memory;  // Typically TRUE for Buffers

    // DMAMUX Request ID (The "Wire" connection)
    DMARequestId request_id;

    [[nodiscard]] static constexpr DMAChannelConfig default_adc_config()
    {
        return DMAChannelConfig{
            .direction  = DMADirection::kPeriphToMem,  // ADC is peripheral to memory
            .priority   = DMAPriority::kHigh,          // ADC usually high priority
            .mode       = DMAMode::kCircular,          // ADC usually circular
            .data_width = DMADataWidth::kHalfWord,     // 16-bit ADC data
            .inc_periph = false,                       // Read from fixed ADC_DR
            .inc_memory = true,                        // Write to buffer[]
            .request_id = DMARequestId::kMem2Mem       // User must specify!
        };
    }
};

// =============================================================================
// COMPILE TIME CONFIGURATIONS
// =============================================================================

// ============================================================================
// FORWARD DECLARATIONS
// ============================================================================

class DMADevice;

template <uint8_t tkDmaIdx>
    requires(kValidDMAIndex<tkDmaIdx>)
class DMAControllerDevice;

template <uint8_t tkDmaIdx, uint8_t tkChannelIdx>
    requires(kValidDMAIndex<tkDmaIdx> && kValidDMAChannel<tkChannelIdx>)
class DMAChannelDevice;

// ============================================================================
// DMA DEVICE INTERFACE
// ============================================================================
class DMADevice
{
public:
    struct Descriptor : public InterfaceDeviceDescriptor
    {
        using Children = DeviceList<DMAControllerDevice<1>, DMAControllerDevice<2>>;
    };
};

// ============================================================================
// DMA CONTROLLER (SHARED)
// ============================================================================
template <uint8_t tkDmaIdx>
    requires(kValidDMAIndex<tkDmaIdx>)
class DMAControllerDevice
{
public:
    struct Descriptor : public SharedDeviceDescriptor
    {
        // Declares ownership of all 8 channels for this controller
        using Children = DeviceList<DMAChannelDevice<tkDmaIdx, 1>,
                                    DMAChannelDevice<tkDmaIdx, 2>,
                                    DMAChannelDevice<tkDmaIdx, 3>,
                                    DMAChannelDevice<tkDmaIdx, 4>,
                                    DMAChannelDevice<tkDmaIdx, 5>,
                                    DMAChannelDevice<tkDmaIdx, 6>,
                                    DMAChannelDevice<tkDmaIdx, 7>,
                                    DMAChannelDevice<tkDmaIdx, 8>>;
    };

    using DependDevices = TypeList<DMADevice>;
    using Traits        = DMATraits<tkDmaIdx>;

    static inline void init()
    {
        // Enable Controller Clock
        LL_AHB1_GRP1_EnableClock(Traits::skClock);

        // Enable DMAMUX Clock (Shared global resource)
        // It is safe to call this multiple times (e.g. by DMA1 and DMA2)
        LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMAMUX1);
    }

    static inline void post_init()
    {
        // Additional post-initialization if needed
    }
};

using DMA1ControllerDevice = DMAControllerDevice<1>;
using DMA2ControllerDevice = DMAControllerDevice<2>;

// ============================================================================
// DMA CHANNEL (UNIQUE)
// ============================================================================
template <uint8_t tkDmaIdx, uint8_t tkChannelIdx>
    requires(kValidDMAIndex<tkDmaIdx> && kValidDMAChannel<tkChannelIdx>)
class DMAChannelDevice
{
public:
    struct Descriptor : public UniqueDeviceDescriptor
    {
    };

    using ControllerT   = DMAControllerDevice<tkDmaIdx>;
    using DependDevices = TypeList<ControllerT>;
    using Traits        = DMATraits<tkDmaIdx>;
    using ChTraits      = DMAChannelTraits<tkDmaIdx, tkChannelIdx>;

    static constexpr uint8_t   skDmaIdx      = tkDmaIdx;
    static constexpr uint8_t   skChannelIdx  = tkChannelIdx;
    static constexpr uint32_t  skChannelId   = ChTraits::skChannelId;
    static inline DMA_TypeDef* skDMAInstance = Traits::skInstance;
    static constexpr IRQn_Type skIRQn        = ChTraits::skIRQn;

public:
    explicit DMAChannelDevice()
    {
    }

    /**
     * @brief Initialize the DMA Channel and route the DMAMUX.
     */
    static void init(const DMAChannelConfig& config)
    {
        // Configure Transfer Parameters
        LL_DMA_ConfigTransfer(skDMAInstance,
                              skChannelId,
                              static_cast<uint32_t>(config.direction) | static_cast<uint32_t>(config.priority) |
                                  static_cast<uint32_t>(config.mode) |
                                  static_cast<uint32_t>(config.data_width) |  // Peripheral Width
                                  static_cast<uint32_t>(config.data_width) |  // Memory Width (Keep same for now)
                                  (config.inc_periph ? LL_DMA_PERIPH_INCREMENT : LL_DMA_PERIPH_NOINCREMENT) |
                                  (config.inc_memory ? LL_DMA_MEMORY_INCREMENT : LL_DMA_MEMORY_NOINCREMENT));

        // Configure Routing (DMAMUX)
        // This is what connects "ADC1" to "DMA1 Channel 1"
        LL_DMAMUX_SetRequestID(DMAMUX1, skChannelId, static_cast<uint32_t>(config.request_id));
    }

    /**
     * @brief Configure memory addresses and start the transfer.
     * @param src_addr Source Address (Peripheral or Memory)
     * @param dst_addr Destination Address (Peripheral or Memory)
     * @param length   Number of data items (not bytes, but items based on data_width)
     */
    static void start(uint32_t src_addr, uint32_t dst_addr, uint32_t length)
    {
        // Note: LL_DMA_ConfigAddresses logic depends on direction.
        // For PeriphToMem: Src=Periph, Dst=Mem
        // For MemToPeriph: Src=Mem, Dst=Periph
        // LL_DMA_ConfigAddresses takes (PeriphAddr, MemAddr, Direction).
        // We need to be careful with the argument order based on direction.

        // However, a safer raw way that works for generic transfers is setting registers directly
        // or using the LL helper which assumes (Periph, Mem, Direction).

        uint32_t direction = LL_DMA_GetDataTransferDirection(skDMAInstance, skChannelId);

        if (direction == LL_DMA_DIRECTION_MEMORY_TO_PERIPH)
        {
            // Src is Mem, Dst is Periph
            LL_DMA_ConfigAddresses(skDMAInstance, skChannelId, dst_addr, src_addr, direction);
        }
        else
        {
            // Src is Periph (or MemToMem), Dst is Mem
            LL_DMA_ConfigAddresses(skDMAInstance, skChannelId, src_addr, dst_addr, direction);
        }

        LL_DMA_SetDataLength(skDMAInstance, skChannelId, length);
        LL_DMA_EnableChannel(skDMAInstance, skChannelId);
    }

    static void stop()
    {
        LL_DMA_DisableChannel(skDMAInstance, skChannelId);
    }

    /**
     * @brief Enable interrupts for this channel.
     * @param config Configuration for DMA Interrupts.
     */
    static void enable_interrupts(const DMAInterruptConfig& config)
    {
        if (config.enable_tc) LL_DMA_EnableIT_TC(skDMAInstance, skChannelId);
        if (config.enable_ht) LL_DMA_EnableIT_HT(skDMAInstance, skChannelId);
        if (config.enable_te) LL_DMA_EnableIT_TE(skDMAInstance, skChannelId);
        NVIC_SetPriority(skIRQn, config.priority);
        NVIC_EnableIRQ(skIRQn);
    }

    /**
     * @brief Disable interrupts for this channel.
     */
    static void disable_interrupts()
    {
        LL_DMA_DisableIT_TC(skDMAInstance, skChannelId);
        LL_DMA_DisableIT_HT(skDMAInstance, skChannelId);
        LL_DMA_DisableIT_TE(skDMAInstance, skChannelId);
        NVIC_DisableIRQ(skIRQn);
    }

    // Check specific flags (useful for Router logic)
    static inline bool tc_flag_active()
    {
        if constexpr (skChannelId == LL_DMA_CHANNEL_1)
        {
            return LL_DMA_IsActiveFlag_TC1(skDMAInstance);
        }
        else if constexpr (skChannelId == LL_DMA_CHANNEL_2)
        {
            return LL_DMA_IsActiveFlag_TC2(skDMAInstance);
        }
        else if constexpr (skChannelId == LL_DMA_CHANNEL_3)
        {
            return LL_DMA_IsActiveFlag_TC3(skDMAInstance);
        }
        else if constexpr (skChannelId == LL_DMA_CHANNEL_4)
        {
            return LL_DMA_IsActiveFlag_TC4(skDMAInstance);
        }
        else if constexpr (skChannelId == LL_DMA_CHANNEL_5)
        {
            return LL_DMA_IsActiveFlag_TC5(skDMAInstance);
        }
        else if constexpr (skChannelId == LL_DMA_CHANNEL_6)
        {
            return LL_DMA_IsActiveFlag_TC6(skDMAInstance);
        }
        else if constexpr (skChannelId == LL_DMA_CHANNEL_7)
        {
            return LL_DMA_IsActiveFlag_TC7(skDMAInstance);
        }
        else if constexpr (skChannelId == LL_DMA_CHANNEL_8)
        {
            return LL_DMA_IsActiveFlag_TC8(skDMAInstance);
        }
        else
        {
            static_assert(false, "Invalid DMA Channel ID");
        }
    }

    static inline bool tc_int_pending()
    {
        return tc_flag_active() && LL_DMA_IsEnabledIT_TC(skDMAInstance, skChannelId);
    }

    static inline bool ht_flag_active()
    {
        if constexpr (skChannelId == LL_DMA_CHANNEL_1)
        {
            return LL_DMA_IsActiveFlag_HT1(skDMAInstance);
        }
        else if constexpr (skChannelId == LL_DMA_CHANNEL_2)
        {
            return LL_DMA_IsActiveFlag_HT2(skDMAInstance);
        }
        else if constexpr (skChannelId == LL_DMA_CHANNEL_3)
        {
            return LL_DMA_IsActiveFlag_HT3(skDMAInstance);
        }
        else if constexpr (skChannelId == LL_DMA_CHANNEL_4)
        {
            return LL_DMA_IsActiveFlag_HT4(skDMAInstance);
        }
        else if constexpr (skChannelId == LL_DMA_CHANNEL_5)
        {
            return LL_DMA_IsActiveFlag_HT5(skDMAInstance);
        }
        else if constexpr (skChannelId == LL_DMA_CHANNEL_6)
        {
            return LL_DMA_IsActiveFlag_HT6(skDMAInstance);
        }
        else if constexpr (skChannelId == LL_DMA_CHANNEL_7)
        {
            return LL_DMA_IsActiveFlag_HT7(skDMAInstance);
        }
        else if constexpr (skChannelId == LL_DMA_CHANNEL_8)
        {
            return LL_DMA_IsActiveFlag_HT8(skDMAInstance);
        }
        else
        {
            static_assert(false, "Invalid DMA Channel ID");
        }
    }

    static inline bool ht_int_pending()
    {
        return ht_flag_active() && LL_DMA_IsEnabledIT_HT(skDMAInstance, skChannelId);
    }

    static inline bool te_flag_active()
    {
        if constexpr (skChannelId == LL_DMA_CHANNEL_1)
        {
            return LL_DMA_IsActiveFlag_TE1(skDMAInstance);
        }
        else if constexpr (skChannelId == LL_DMA_CHANNEL_2)
        {
            return LL_DMA_IsActiveFlag_TE2(skDMAInstance);
        }
        else if constexpr (skChannelId == LL_DMA_CHANNEL_3)
        {
            return LL_DMA_IsActiveFlag_TE3(skDMAInstance);
        }
        else if constexpr (skChannelId == LL_DMA_CHANNEL_4)
        {
            return LL_DMA_IsActiveFlag_TE4(skDMAInstance);
        }
        else if constexpr (skChannelId == LL_DMA_CHANNEL_5)
        {
            return LL_DMA_IsActiveFlag_TE5(skDMAInstance);
        }
        else if constexpr (skChannelId == LL_DMA_CHANNEL_6)
        {
            return LL_DMA_IsActiveFlag_TE6(skDMAInstance);
        }
        else if constexpr (skChannelId == LL_DMA_CHANNEL_7)
        {
            return LL_DMA_IsActiveFlag_TE7(skDMAInstance);
        }
        else if constexpr (skChannelId == LL_DMA_CHANNEL_8)
        {
            return LL_DMA_IsActiveFlag_TE8(skDMAInstance);
        }
        else
        {
            static_assert(false, "Invalid DMA Channel ID");
        }
    }

    static inline bool te_int_pending()
    {
        return te_flag_active() && LL_DMA_IsEnabledIT_TE(skDMAInstance, skChannelId);
    }

    // Static helper for ISRs
    static inline void ack_tc_int()
    {
        if constexpr (skChannelId == LL_DMA_CHANNEL_1)
        {
            LL_DMA_ClearFlag_TC1(skDMAInstance);
        }
        else if constexpr (skChannelId == LL_DMA_CHANNEL_2)
        {
            LL_DMA_ClearFlag_TC2(skDMAInstance);
        }
        else if constexpr (skChannelId == LL_DMA_CHANNEL_3)
        {
            LL_DMA_ClearFlag_TC3(skDMAInstance);
        }
        else if constexpr (skChannelId == LL_DMA_CHANNEL_4)
        {
            LL_DMA_ClearFlag_TC4(skDMAInstance);
        }
        else if constexpr (skChannelId == LL_DMA_CHANNEL_5)
        {
            LL_DMA_ClearFlag_TC5(skDMAInstance);
        }
        else if constexpr (skChannelId == LL_DMA_CHANNEL_6)
        {
            LL_DMA_ClearFlag_TC6(skDMAInstance);
        }
        else if constexpr (skChannelId == LL_DMA_CHANNEL_7)
        {
            LL_DMA_ClearFlag_TC7(skDMAInstance);
        }
        else if constexpr (skChannelId == LL_DMA_CHANNEL_8)
        {
            LL_DMA_ClearFlag_TC8(skDMAInstance);
        }
        else
        {
            static_assert(false, "Invalid DMA Channel ID");
        }
    }

    static inline void ack_ht_int()
    {
        if constexpr (skChannelId == LL_DMA_CHANNEL_1)
        {
            LL_DMA_ClearFlag_HT1(skDMAInstance);
        }
        else if constexpr (skChannelId == LL_DMA_CHANNEL_2)
        {
            LL_DMA_ClearFlag_HT2(skDMAInstance);
        }
        else if constexpr (skChannelId == LL_DMA_CHANNEL_3)
        {
            LL_DMA_ClearFlag_HT3(skDMAInstance);
        }
        else if constexpr (skChannelId == LL_DMA_CHANNEL_4)
        {
            LL_DMA_ClearFlag_HT4(skDMAInstance);
        }
        else if constexpr (skChannelId == LL_DMA_CHANNEL_5)
        {
            LL_DMA_ClearFlag_HT5(skDMAInstance);
        }
        else if constexpr (skChannelId == LL_DMA_CHANNEL_6)
        {
            LL_DMA_ClearFlag_HT6(skDMAInstance);
        }
        else if constexpr (skChannelId == LL_DMA_CHANNEL_7)
        {
            LL_DMA_ClearFlag_HT7(skDMAInstance);
        }
        else if constexpr (skChannelId == LL_DMA_CHANNEL_8)
        {
            LL_DMA_ClearFlag_HT8(skDMAInstance);
        }
        else
        {
            static_assert(false, "Invalid DMA Channel ID");
        }
    }

    static inline void ack_te_int()
    {
        if constexpr (skChannelId == LL_DMA_CHANNEL_1)
        {
            LL_DMA_ClearFlag_TE1(skDMAInstance);
        }
        else if constexpr (skChannelId == LL_DMA_CHANNEL_2)
        {
            LL_DMA_ClearFlag_TE2(skDMAInstance);
        }
        else if constexpr (skChannelId == LL_DMA_CHANNEL_3)
        {
            LL_DMA_ClearFlag_TE3(skDMAInstance);
        }
        else if constexpr (skChannelId == LL_DMA_CHANNEL_4)
        {
            LL_DMA_ClearFlag_TE4(skDMAInstance);
        }
        else if constexpr (skChannelId == LL_DMA_CHANNEL_5)
        {
            LL_DMA_ClearFlag_TE5(skDMAInstance);
        }
        else if constexpr (skChannelId == LL_DMA_CHANNEL_6)
        {
            LL_DMA_ClearFlag_TE6(skDMAInstance);
        }
        else if constexpr (skChannelId == LL_DMA_CHANNEL_7)
        {
            LL_DMA_ClearFlag_TE7(skDMAInstance);
        }
        else if constexpr (skChannelId == LL_DMA_CHANNEL_8)
        {
            LL_DMA_ClearFlag_TE8(skDMAInstance);
        }
        else
        {
            static_assert(false, "Invalid DMA Channel ID");
        }
    }
};

using DMA1Channel1Device = DMAChannelDevice<1, 1>;
using DMA1Channel2Device = DMAChannelDevice<1, 2>;
using DMA1Channel3Device = DMAChannelDevice<1, 3>;
using DMA1Channel4Device = DMAChannelDevice<1, 4>;
using DMA1Channel5Device = DMAChannelDevice<1, 5>;
using DMA1Channel6Device = DMAChannelDevice<1, 6>;
using DMA1Channel7Device = DMAChannelDevice<1, 7>;
using DMA1Channel8Device = DMAChannelDevice<1, 8>;

using DMA2Channel1Device = DMAChannelDevice<2, 1>;
using DMA2Channel2Device = DMAChannelDevice<2, 2>;
using DMA2Channel3Device = DMAChannelDevice<2, 3>;
using DMA2Channel4Device = DMAChannelDevice<2, 4>;
using DMA2Channel5Device = DMAChannelDevice<2, 5>;
using DMA2Channel6Device = DMAChannelDevice<2, 6>;
using DMA2Channel7Device = DMAChannelDevice<2, 7>;
using DMA2Channel8Device = DMAChannelDevice<2, 8>;

class DMANullChannelDevice
{
public:
    struct Descriptor : public NullDeviceDescriptor
    {
    };
};

template <typename T>
concept CNullDMAChannel = std::is_same_v<T, DMANullChannelDevice>;

// ============================================================================
// ISR ROUTER TEMPLATE
// ============================================================================
enum class DMAInterruptType : uint8_t
{
    kTransferComplete,
    kHalfTransfer,
    kTransferError
};

template <uint8_t tkDmaIdx, uint8_t tkChannelIdx, DMAInterruptType tkIntType>
struct DMAIsrRouter
{
    static void handle()
    {
        // Do nothing by default (optimized away)
    }
};