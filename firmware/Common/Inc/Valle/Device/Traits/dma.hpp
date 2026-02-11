#pragma once

#include "Valle/Device/Traits/dmamux.hpp"
#include "stm32g4xx_ll_bus.h"
#include "stm32g4xx_ll_dma.h"
#include "stm32g4xx_ll_dmamux.h"

namespace valle
{

    // ============================================================================
    // DEVICE IDs & CHECKS
    // ============================================================================

    using DMAControllerID = uint8_t;

    template <DMAControllerID tkControllerID>
    constexpr bool kValidDMAControllerID = (tkControllerID == 1 || tkControllerID == 2);

    using DMAChannelID = uint8_t;

    template <DMAChannelID tkChannelID>
    constexpr bool kValidDMAChannel = (tkChannelID >= 1 && tkChannelID <= 8);

    struct DMAChannelFullID
    {
        DMAControllerID controller_id = 0;
        DMAChannelID    channel_id    = 0;
    };

    // ============================================================================
    // HARDWARE TRAITS
    // ============================================================================

    template <DMAControllerID tkControllerID>
    struct DMAControllerTraits;

    template <>
    struct DMAControllerTraits<1>
    {
        static constexpr DMAMuxControllerID skMuxControllerID = 1;
        using MuxTraitsT                                      = DMAMuxControllerTraits<skMuxControllerID>;
        static inline DMA_TypeDef* const skInstance           = DMA1;
        static constexpr uint32_t        skClock              = LL_AHB1_GRP1_PERIPH_DMA1;

        static void enable_clock()
        {
            LL_AHB1_GRP1_EnableClock(skClock);
        }
    };

    template <>
    struct DMAControllerTraits<2>
    {
        static constexpr DMAMuxControllerID skMuxControllerID = 1;
        using MuxTraitsT                                      = DMAMuxControllerTraits<skMuxControllerID>;
        static inline DMA_TypeDef* const skInstance           = DMA2;
        static constexpr uint32_t        skClock              = LL_AHB1_GRP1_PERIPH_DMA2;

        static void enable_clock()
        {
            LL_AHB1_GRP1_EnableClock(skClock);
        }
    };

    template <DMAControllerID tkControllerID, DMAChannelID tkChannelID>
    struct DMAChannelTraits
    {
    private:
        // Helper to get IRQn
        static constexpr IRQn_Type get_irq_n()
        {
            if constexpr (tkControllerID == 1)  // NOLINT(bugprone-branch-clone)
            {
                if constexpr (tkChannelID == 1) return DMA1_Channel1_IRQn;
                if constexpr (tkChannelID == 2) return DMA1_Channel2_IRQn;
                if constexpr (tkChannelID == 3) return DMA1_Channel3_IRQn;
                if constexpr (tkChannelID == 4) return DMA1_Channel4_IRQn;
                if constexpr (tkChannelID == 5) return DMA1_Channel5_IRQn;
                if constexpr (tkChannelID == 6) return DMA1_Channel6_IRQn;
                if constexpr (tkChannelID == 7) return DMA1_Channel7_IRQn;
                if constexpr (tkChannelID == 8) return DMA1_Channel8_IRQn;
            }
            else
            {
                if constexpr (tkChannelID == 1) return DMA2_Channel1_IRQn;
                if constexpr (tkChannelID == 2) return DMA2_Channel2_IRQn;
                if constexpr (tkChannelID == 3) return DMA2_Channel3_IRQn;
                if constexpr (tkChannelID == 4) return DMA2_Channel4_IRQn;
                if constexpr (tkChannelID == 5) return DMA2_Channel5_IRQn;
                if constexpr (tkChannelID == 6) return DMA2_Channel6_IRQn;
                if constexpr (tkChannelID == 7) return DMA2_Channel7_IRQn;
                if constexpr (tkChannelID == 8) return DMA2_Channel8_IRQn;
            }
        }

        // Helper to get Channel Instance
        static DMA_Channel_TypeDef* get_channel_instance()
        {
            if constexpr (tkControllerID == 1)  // NOLINT(bugprone-branch-clone)
            {
                if constexpr (tkChannelID == 1) return DMA1_Channel1;
                if constexpr (tkChannelID == 2) return DMA1_Channel2;
                if constexpr (tkChannelID == 3) return DMA1_Channel3;
                if constexpr (tkChannelID == 4) return DMA1_Channel4;
                if constexpr (tkChannelID == 5) return DMA1_Channel5;
                if constexpr (tkChannelID == 6) return DMA1_Channel6;
                if constexpr (tkChannelID == 7) return DMA1_Channel7;
                if constexpr (tkChannelID == 8) return DMA1_Channel8;
            }
            else if constexpr (tkControllerID == 2)
            {
                if constexpr (tkChannelID == 1) return DMA2_Channel1;
                if constexpr (tkChannelID == 2) return DMA2_Channel2;
                if constexpr (tkChannelID == 3) return DMA2_Channel3;
                if constexpr (tkChannelID == 4) return DMA2_Channel4;
                if constexpr (tkChannelID == 5) return DMA2_Channel5;
                if constexpr (tkChannelID == 6) return DMA2_Channel6;
                if constexpr (tkChannelID == 7) return DMA2_Channel7;
                if constexpr (tkChannelID == 8) return DMA2_Channel8;
            }
            else
            {
                static_assert(false, "Invalid DMA Controller ID");
            }
        }

        static consteval uint32_t get_ll_channel_id()
        {
            // NOLINTBEGIN(readability-magic-numbers)
            if constexpr (tkChannelID == 1)
            {
                return LL_DMA_CHANNEL_1;
            }
            else if constexpr (tkChannelID == 2)
            {
                return LL_DMA_CHANNEL_2;
            }
            else if constexpr (tkChannelID == 3)
            {
                return LL_DMA_CHANNEL_3;
            }
            else if constexpr (tkChannelID == 4)
            {
                return LL_DMA_CHANNEL_4;
            }
            else if constexpr (tkChannelID == 5)
            {
                return LL_DMA_CHANNEL_5;
            }
            else if constexpr (tkChannelID == 6)
            {
                return LL_DMA_CHANNEL_6;
            }
            else if constexpr (tkChannelID == 7)
            {
                return LL_DMA_CHANNEL_7;
            }
            else if constexpr (tkChannelID == 8)
            {
                return LL_DMA_CHANNEL_8;
            }
            else
            {
                static_assert(false, "Invalid DMA Channel ID");
            }
            // NOLINTEND(readability-magic-numbers)
        }

        static consteval DMAMuxChannel get_mux_channel_id()
        {
            // NOLINTBEGIN(readability-magic-numbers)
            if constexpr (tkControllerID == 1)
            {
                if constexpr (tkChannelID == 1)
                {
                    return DMAMuxChannel::kDMA1Channel1;
                }
                else if constexpr (tkChannelID == 2)
                {
                    return DMAMuxChannel::kDMA1Channel2;
                }
                else if constexpr (tkChannelID == 3)
                {
                    return DMAMuxChannel::kDMA1Channel3;
                }
                else if constexpr (tkChannelID == 4)
                {
                    return DMAMuxChannel::kDMA1Channel4;
                }
                else if constexpr (tkChannelID == 5)
                {
                    return DMAMuxChannel::kDMA1Channel5;
                }
                else if constexpr (tkChannelID == 6)
                {
                    return DMAMuxChannel::kDMA1Channel6;
                }
                else if constexpr (tkChannelID == 7)
                {
                    return DMAMuxChannel::kDMA1Channel7;
                }
                else if constexpr (tkChannelID == 8)
                {
                    return DMAMuxChannel::kDMA1Channel8;
                }
                else
                {
                    static_assert(false, "Invalid DMA Channel ID");
                }
            }
            else if constexpr (tkControllerID == 2)
            {
                if constexpr (tkChannelID == 1)
                {
                    return DMAMuxChannel::kDMA2Channel1;
                }
                else if constexpr (tkChannelID == 2)
                {
                    return DMAMuxChannel::kDMA2Channel2;
                }
                else if constexpr (tkChannelID == 3)
                {
                    return DMAMuxChannel::kDMA2Channel3;
                }
                else if constexpr (tkChannelID == 4)
                {
                    return DMAMuxChannel::kDMA2Channel4;
                }
                else if constexpr (tkChannelID == 5)
                {
                    return DMAMuxChannel::kDMA2Channel5;
                }
                else if constexpr (tkChannelID == 6)
                {
                    return DMAMuxChannel::kDMA2Channel6;
                }
                else if constexpr (tkChannelID == 7)
                {
                    return DMAMuxChannel::kDMA2Channel7;
                }
                else if constexpr (tkChannelID == 8)
                {
                    return DMAMuxChannel::kDMA2Channel8;
                }
                else
                {
                    static_assert(false, "Invalid DMA Channel ID");
                }
            }
            else
            {
                static_assert(false, "Invalid DMA Controller ID");
            }
            // NOLINTEND(readability-magic-numbers)
        }

    public:
        static inline DMA_Channel_TypeDef* const skInstance     = get_channel_instance();
        static constexpr uint32_t                skChannelLLID  = get_ll_channel_id();
        static constexpr DMAMuxChannel           skMuxChannelID = get_mux_channel_id();

        static constexpr IRQn_Type skIRQn = get_irq_n();
    };

    // ============================================================================
    // ENUMERATIONS
    // ============================================================================

    enum class DMADirection : uint32_t
    {
        kPeriphToMem = LL_DMA_DIRECTION_PERIPH_TO_MEMORY,
        kMemToPeriph = LL_DMA_DIRECTION_MEMORY_TO_PERIPH,
        kMemToMem    = LL_DMA_DIRECTION_MEMORY_TO_MEMORY
    };

    enum class DMAPriority : uint32_t
    {
        kLow      = LL_DMA_PRIORITY_LOW,
        kMedium   = LL_DMA_PRIORITY_MEDIUM,
        kHigh     = LL_DMA_PRIORITY_HIGH,
        kVeryHigh = LL_DMA_PRIORITY_VERYHIGH
    };

    enum class DMAMode : uint32_t
    {
        kNormal   = LL_DMA_MODE_NORMAL,
        kCircular = LL_DMA_MODE_CIRCULAR
    };

    enum class DMAPeripheralDataWidth : uint32_t
    {
        kByte     = LL_DMA_PDATAALIGN_BYTE,
        kHalfWord = LL_DMA_PDATAALIGN_HALFWORD,
        kWord     = LL_DMA_PDATAALIGN_WORD
    };

    enum class DMAMemoryDataWidth : uint32_t
    {
        kByte     = LL_DMA_MDATAALIGN_BYTE,
        kHalfWord = LL_DMA_MDATAALIGN_HALFWORD,
        kWord     = LL_DMA_MDATAALIGN_WORD
    };

    enum class DMAIncrementMode : uint32_t
    {
        kPeriphFixed     = LL_DMA_PERIPH_NOINCREMENT,
        kPeriphIncrement = LL_DMA_PERIPH_INCREMENT,
        kMemoryFixed     = LL_DMA_MEMORY_NOINCREMENT,
        kMemoryIncrement = LL_DMA_MEMORY_INCREMENT
    };

}  // namespace valle