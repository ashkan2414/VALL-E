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
    uint32_t priority  = 5;      // NVIC Priority (0 = Highest, 15 = Lowest)
    bool     enable_tc = false;  // Transfer Complete Interrupt Enable
    bool     enable_ht = false;  // Half Transfer Interrupt Enable
    bool     enable_te = false;  // Transfer Error Interrupt Enable
};

/**
 * @brief Configuration for a specific DMA Channel Transfer.
 */
struct DMAChannelConfig
{
    DMADirection direction  = DMADirection::kPeriphToMem;
    DMAPriority  priority   = DMAPriority::kLow;
    DMAMode      mode       = DMAMode::kNormal;
    DMADataWidth data_width = DMADataWidth::kByte;  // Applied to both source and dest for simplicity

    // Increment settings
    bool inc_periph = false;  // Typically FALSE for ADC/SPI
    bool inc_memory = true;   // Typically TRUE for Buffers

    // DMAMUX Request ID (The "Wire" connection)
    DMARequestID request_id = DMARequestID::kMem2Mem;
};

// =============================================================================
// COMPILE TIME CONFIGURATIONS
// =============================================================================

// ============================================================================
// ISR ROUTER TEMPLATE
// ============================================================================
enum class DMAInterruptType : uint8_t
{
    kTransferComplete,
    kHalfTransfer,
    kTransferError
};

template <DMAControllerID tkControllerID, DMAChannelID tkChannelID, DMAInterruptType tkIntType>
struct DMAInterruptTraits;

#define DEFINE_DMA_INT_TRAIT(tkIntType, ll_name)                                                               \
    template <DMAControllerID tkControllerID, DMAChannelID tkChannelID>                                        \
    struct DMAInterruptTraits<tkControllerID, tkChannelID, tkIntType>                                          \
    {                                                                                                          \
        static inline void enable()                                                                            \
        {                                                                                                      \
            LL_DMA_EnableIT_##ll_name(DMAControllerTraits<tkControllerID>::skInstance,                         \
                                      DMAChannelTraits<tkControllerID, tkChannelID>::skChannelLLID);           \
        }                                                                                                      \
        static inline void disable()                                                                           \
        {                                                                                                      \
            LL_DMA_DisableIT_##ll_name(DMAControllerTraits<tkControllerID>::skInstance,                        \
                                       DMAChannelTraits<tkControllerID, tkChannelID>::skChannelLLID);          \
        }                                                                                                      \
        static inline bool is_enabled()                                                                        \
        {                                                                                                      \
            return LL_DMA_IsEnabledIT_##ll_name(DMAControllerTraits<tkControllerID>::skInstance,               \
                                                DMAChannelTraits<tkControllerID, tkChannelID>::skChannelLLID); \
        }                                                                                                      \
        static inline bool flag_active()                                                                       \
        {                                                                                                      \
            if constexpr (tkChannelID == 1)                                                                    \
            {                                                                                                  \
                return LL_DMA_IsActiveFlag_##ll_name##1(DMAControllerTraits<tkControllerID>::skInstance);      \
            }                                                                                                  \
            else if constexpr (tkChannelID == 2)                                                               \
            {                                                                                                  \
                return LL_DMA_IsActiveFlag_##ll_name##2(DMAControllerTraits<tkControllerID>::skInstance);      \
            }                                                                                                  \
            else if constexpr (tkChannelID == 3)                                                               \
            {                                                                                                  \
                return LL_DMA_IsActiveFlag_##ll_name##3(DMAControllerTraits<tkControllerID>::skInstance);      \
            }                                                                                                  \
            else if constexpr (tkChannelID == 4)                                                               \
            {                                                                                                  \
                return LL_DMA_IsActiveFlag_##ll_name##4(DMAControllerTraits<tkControllerID>::skInstance);      \
            }                                                                                                  \
            else if constexpr (tkChannelID == 5)                                                               \
            {                                                                                                  \
                return LL_DMA_IsActiveFlag_##ll_name##5(DMAControllerTraits<tkControllerID>::skInstance);      \
            }                                                                                                  \
            else if constexpr (tkChannelID == 6)                                                               \
            {                                                                                                  \
                return LL_DMA_IsActiveFlag_##ll_name##6(DMAControllerTraits<tkControllerID>::skInstance);      \
            }                                                                                                  \
            else if constexpr (tkChannelID == 7)                                                               \
            {                                                                                                  \
                return LL_DMA_IsActiveFlag_##ll_name##7(DMAControllerTraits<tkControllerID>::skInstance);      \
            }                                                                                                  \
            else if constexpr (tkChannelID == 8)                                                               \
            {                                                                                                  \
                return LL_DMA_IsActiveFlag_##ll_name##8(DMAControllerTraits<tkControllerID>::skInstance);      \
            }                                                                                                  \
            else                                                                                               \
            {                                                                                                  \
                static_assert(false, "Invalid DMA Channel ID");                                                \
            }                                                                                                  \
        }                                                                                                      \
                                                                                                               \
        static inline bool is_pending()                                                                        \
        {                                                                                                      \
            return flag_active() && is_enabled();                                                              \
        }                                                                                                      \
        static inline void ack()                                                                               \
        {                                                                                                      \
            if constexpr (tkChannelID == 1)                                                                    \
            {                                                                                                  \
                LL_DMA_ClearFlags_##ll_name##1(DMAControllerTraits<tkControllerID>::skInstance);               \
            }                                                                                                  \
            else if constexpr (tkChannelID == 2)                                                               \
            {                                                                                                  \
                LL_DMA_ClearFlags_##ll_name##2(DMAControllerTraits<tkControllerID>::skInstance);               \
            }                                                                                                  \
            else if constexpr (tkChannelID == 3)                                                               \
            {                                                                                                  \
                LL_DMA_ClearFlags_##ll_name##3(DMAControllerTraits<tkControllerID>::skInstance);               \
            }                                                                                                  \
            else if constexpr (tkChannelID == 4)                                                               \
            {                                                                                                  \
                LL_DMA_ClearFlags_##ll_name##4(DMAControllerTraits<tkControllerID>::skInstance);               \
            }                                                                                                  \
            else if constexpr (tkChannelID == 5)                                                               \
            {                                                                                                  \
                LL_DMA_ClearFlags_##ll_name##5(DMAControllerTraits<tkControllerID>::skInstance);               \
            }                                                                                                  \
            else if constexpr (tkChannelID == 6)                                                               \
            {                                                                                                  \
                LL_DMA_ClearFlags_##ll_name##6(DMAControllerTraits<tkControllerID>::skInstance);               \
            }                                                                                                  \
            else if constexpr (tkChannelID == 7)                                                               \
            {                                                                                                  \
                LL_DMA_ClearFlags_##ll_name##7(DMAControllerTraits<tkControllerID>::skInstance);               \
            }                                                                                                  \
            else if constexpr (tkChannelID == 8)                                                               \
            {                                                                                                  \
                LL_DMA_ClearFlags_##ll_name##8(DMAControllerTraits<tkControllerID>::skInstance);               \
            }                                                                                                  \
            else                                                                                               \
            {                                                                                                  \
                static_assert(false, "Invalid DMA Channel ID");                                                \
            }                                                                                                  \
        }                                                                                                      \
    };

DEFINE_DMA_INT_TRAIT(DMAInterruptType::kTransferComplete, TC);
DEFINE_DMA_INT_TRAIT(DMAInterruptType::kHalfTransfer, HT);
DEFINE_DMA_INT_TRAIT(DMAInterruptType::kTransferError, TE);

#undef DEFINE_DMA_INT_TRAIT

template <DMAControllerID tkControllerID, DMAChannelID tkChannelID, DMAInterruptType tkIntType>
struct DMAIsrRouter
{
    using UnboundIsrHandlerTag = void;
    static void handle()
    {
        // Do nothing by default (optimized away)
    }
};

// ============================================================================
// FORWARD DECLARATIONS
// ============================================================================

class DMADevice;

template <DMAControllerID tkControllerID>
    requires(kValidDMAID<tkControllerID>)
class DMAControllerDevice;

template <DMAControllerID tkControllerID, DMAChannelID tkChannelID>
    requires(kValidDMAID<tkControllerID> && kValidDMAChannel<tkChannelID>)
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
template <DMAControllerID tkControllerID>
    requires(kValidDMAID<tkControllerID>)
class DMAControllerDevice
{
public:
    struct Descriptor : public SharedDeviceDescriptor
    {
        // Declares ownership of all 8 channels for this controller
        using Children = DeviceList<DMAChannelDevice<tkControllerID, 1>,
                                    DMAChannelDevice<tkControllerID, 2>,
                                    DMAChannelDevice<tkControllerID, 3>,
                                    DMAChannelDevice<tkControllerID, 4>,
                                    DMAChannelDevice<tkControllerID, 5>,
                                    DMAChannelDevice<tkControllerID, 6>,
                                    DMAChannelDevice<tkControllerID, 7>,
                                    DMAChannelDevice<tkControllerID, 8>>;
    };

    using DependDevices     = TypeList<DMADevice>;
    using ControllerTraitsT = DMAControllerTraits<tkControllerID>;

    static inline void init()
    {
        // Enable Controller Clock
        LL_AHB1_GRP1_EnableClock(ControllerTraitsT::skClock);

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
template <DMAControllerID tkControllerID, DMAChannelID tkChannelID>
    requires(kValidDMAID<tkControllerID> && kValidDMAChannel<tkChannelID>)
class DMAChannelDevice
{
public:
    struct Descriptor : public UniqueDeviceDescriptor
    {
    };

    using ControllerT       = DMAControllerDevice<tkControllerID>;
    using DependDevices     = TypeList<ControllerT>;
    using ControllerTraitsT = DMAControllerTraits<tkControllerID>;
    using ChannelTraitsT    = DMAChannelTraits<tkControllerID, tkChannelID>;

    static constexpr uint8_t skControllerID = tkControllerID;
    static constexpr uint8_t skChannelID    = tkChannelID;

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
        LL_DMA_ConfigTransfer(ControllerTraitsT::skInstance,
                              ChannelTraitsT::skChannelLLID,
                              static_cast<uint32_t>(config.direction) | static_cast<uint32_t>(config.priority) |
                                  static_cast<uint32_t>(config.mode) |
                                  static_cast<uint32_t>(config.data_width) |  // Peripheral Width
                                  static_cast<uint32_t>(config.data_width) |  // Memory Width (Keep same for now)
                                  (config.inc_periph ? LL_DMA_PERIPH_INCREMENT : LL_DMA_PERIPH_NOINCREMENT) |
                                  (config.inc_memory ? LL_DMA_MEMORY_INCREMENT : LL_DMA_MEMORY_NOINCREMENT));

        // Configure Routing (DMAMUX)
        // This is what connects "ADC1" to "DMA1 Channel 1"
        LL_DMAMUX_SetRequestID(DMAMUX1, ChannelTraitsT::skChannelLLID, static_cast<uint32_t>(config.request_id));
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

        uint32_t direction =
            LL_DMA_GetDataTransferDirection(ControllerTraitsT::skInstance, ChannelTraitsT::skChannelLLID);

        if (direction == LL_DMA_DIRECTION_MEMORY_TO_PERIPH)
        {
            // Src is Mem, Dst is Periph
            LL_DMA_ConfigAddresses(
                ControllerTraitsT::skInstance, ChannelTraitsT::skChannelLLID, dst_addr, src_addr, direction);
        }
        else
        {
            // Src is Periph (or MemToMem), Dst is Mem
            LL_DMA_ConfigAddresses(
                ControllerTraitsT::skInstance, ChannelTraitsT::skChannelLLID, src_addr, dst_addr, direction);
        }

        LL_DMA_SetDataLength(ControllerTraitsT::skInstance, ChannelTraitsT::skChannelLLID, length);
        LL_DMA_EnableChannel(ControllerTraitsT::skInstance, ChannelTraitsT::skChannelLLID);
    }

    static void stop()
    {
        LL_DMA_DisableChannel(ControllerTraitsT::skInstance, ChannelTraitsT::skChannelLLID);
    }

    /**
     * @brief Enable interrupts for this channel.
     * @param config Configuration for DMA Interrupts.
     */
    static void enable_interrupts(const DMAInterruptConfig& config)
    {
        if (config.enable_tc) LL_DMA_EnableIT_TC(ControllerTraitsT::skInstance, ChannelTraitsT::skChannelLLID);
        if (config.enable_ht) LL_DMA_EnableIT_HT(ControllerTraitsT::skInstance, ChannelTraitsT::skChannelLLID);
        if (config.enable_te) LL_DMA_EnableIT_TE(ControllerTraitsT::skInstance, ChannelTraitsT::skChannelLLID);
        NVIC_SetPriority(ChannelTraitsT::skIRQn, config.priority);
        NVIC_EnableIRQ(ChannelTraitsT::skIRQn);
    }

    /**
     * @brief Disable interrupts for this channel.
     */
    static void disable_interrupts()
    {
        LL_DMA_DisableIT_TC(ControllerTraitsT::skInstance, ChannelTraitsT::skChannelLLID);
        LL_DMA_DisableIT_HT(ControllerTraitsT::skInstance, ChannelTraitsT::skChannelLLID);
        LL_DMA_DisableIT_TE(ControllerTraitsT::skInstance, ChannelTraitsT::skChannelLLID);
        NVIC_DisableIRQ(ChannelTraitsT::skIRQn);
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