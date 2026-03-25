#pragma once

#include "stm32g4xx_ll_bus.h"
#include "stm32g4xx_ll_dma.h"
#include "valle/platform/hardware/dma/id.hpp"
#include "valle/platform/hardware/dmamux.hpp"

namespace valle::platform
{
    // ============================================================================
    // ENUMERATIONS
    // ============================================================================

    enum class DmaDirection : uint32_t
    {
        kPeriphToMem = LL_DMA_DIRECTION_PERIPH_TO_MEMORY,
        kMemToPeriph = LL_DMA_DIRECTION_MEMORY_TO_PERIPH,
        kMemToMem    = LL_DMA_DIRECTION_MEMORY_TO_MEMORY
    };

    enum class DmaPriority : uint32_t
    {
        kLow      = LL_DMA_PRIORITY_LOW,
        kMedium   = LL_DMA_PRIORITY_MEDIUM,
        kHigh     = LL_DMA_PRIORITY_HIGH,
        kVeryHigh = LL_DMA_PRIORITY_VERYHIGH
    };

    enum class DmaMode : uint32_t
    {
        kNormal   = LL_DMA_MODE_NORMAL,
        kCircular = LL_DMA_MODE_CIRCULAR
    };

    enum class DmaPeripheralDataWidth : uint32_t
    {
        kByte     = LL_DMA_PDATAALIGN_BYTE,
        kHalfWord = LL_DMA_PDATAALIGN_HALFWORD,
        kWord     = LL_DMA_PDATAALIGN_WORD
    };

    enum class DmaMemoryDataWidth : uint32_t
    {
        kByte     = LL_DMA_MDATAALIGN_BYTE,
        kHalfWord = LL_DMA_MDATAALIGN_HALFWORD,
        kWord     = LL_DMA_MDATAALIGN_WORD
    };

    enum class DmaIncrementMode : uint32_t
    {
        kPeriphFixed     = LL_DMA_PERIPH_NOINCREMENT,
        kPeriphIncrement = LL_DMA_PERIPH_INCREMENT,
        kMemoryFixed     = LL_DMA_MEMORY_NOINCREMENT,
        kMemoryIncrement = LL_DMA_MEMORY_INCREMENT
    };

    // ============================================================================
    // HARDWARE TRAITS
    // ============================================================================

    template <DmaPeripheralId tkPeripheralId>
    struct DmaControllerTraits;

    template <>
    struct DmaControllerTraits<DmaPeripheralId::kDma1>
    {
        static constexpr DmaMuxPeripheralId skMuxId = DmaMuxPeripheralId::kDmaMux1;
        using MuxTraitsT                            = DmaMuxControllerTraits<skMuxId>;
        static inline Dma_TypeDef* const skInstance = Dma1;
        static constexpr uint32_t        skClock    = LL_AHB1_GRP1_PERIPH_DMA1;

        void enable_clock() const
        {
            LL_AHB1_GRP1_EnableClock(skClock);
        }
    };

    template <>
    struct DmaControllerTraits<DmaPeripheralId::kDma2>
    {
        static constexpr DmaMuxPeripheralId skMuxId = DmaMuxPeripheralId::kDmaMux1;
        using MuxTraitsT                            = DmaMuxControllerTraits<skMuxId>;
        static inline Dma_TypeDef* const skInstance = Dma2;
        static constexpr uint32_t        skClock    = LL_AHB1_GRP1_PERIPH_DMA2;

        void enable_clock() const
        {
            LL_AHB1_GRP1_EnableClock(skClock);
        }
    };

    template <DmaPeripheralId tkPeripheralId, DmaChannelId tkChannelId>
    struct DmaChannelTraits
    {
    private:
        static constexpr uint8_t skChannelIdx = static_cast<uint8_t>(tkChannelId);

        // Helper to get IRQn
        static constexpr IRQn_Type get_irq_n()
        {
            if constexpr (tkPeripheralId == DmaPeripheralId::kDma1)  // NOLINT(bugprone-branch-clone)
            {
                if constexpr (skChannelIdx == 1) return Dma1_Channel1_IRQn;
                if constexpr (skChannelIdx == 2) return Dma1_Channel2_IRQn;
                if constexpr (skChannelIdx == 3) return Dma1_Channel3_IRQn;
                if constexpr (skChannelIdx == 4) return Dma1_Channel4_IRQn;
                if constexpr (skChannelIdx == 5) return Dma1_Channel5_IRQn;
                if constexpr (skChannelIdx == 6) return Dma1_Channel6_IRQn;
                if constexpr (skChannelIdx == 7) return Dma1_Channel7_IRQn;
                if constexpr (skChannelIdx == 8) return Dma1_Channel8_IRQn;
            }
            else
            {
                if constexpr (skChannelIdx == 1) return Dma2_Channel1_IRQn;
                if constexpr (skChannelIdx == 2) return Dma2_Channel2_IRQn;
                if constexpr (skChannelIdx == 3) return Dma2_Channel3_IRQn;
                if constexpr (skChannelIdx == 4) return Dma2_Channel4_IRQn;
                if constexpr (skChannelIdx == 5) return Dma2_Channel5_IRQn;
                if constexpr (skChannelIdx == 6) return Dma2_Channel6_IRQn;
                if constexpr (skChannelIdx == 7) return Dma2_Channel7_IRQn;
                if constexpr (skChannelIdx == 8) return Dma2_Channel8_IRQn;
            }
        }

        // Helper to get Channel Instance
        Dma_Channel_TypeDef* get_channel_instance() const
        {
            if constexpr (tkPeripheralId == DmaPeripheralId::kDma1)  // NOLINT(bugprone-branch-clone)
            {
                if constexpr (skChannelIdx == 1) return Dma1_Channel1;
                if constexpr (skChannelIdx == 2) return Dma1_Channel2;
                if constexpr (skChannelIdx == 3) return Dma1_Channel3;
                if constexpr (skChannelIdx == 4) return Dma1_Channel4;
                if constexpr (skChannelIdx == 5) return Dma1_Channel5;
                if constexpr (skChannelIdx == 6) return Dma1_Channel6;
                if constexpr (skChannelIdx == 7) return Dma1_Channel7;
                if constexpr (skChannelIdx == 8) return Dma1_Channel8;
            }
            else if constexpr (tkPeripheralId == DmaPeripheralId::kDma2)
            {
                if constexpr (skChannelIdx == 1) return Dma2_Channel1;
                if constexpr (skChannelIdx == 2) return Dma2_Channel2;
                if constexpr (skChannelIdx == 3) return Dma2_Channel3;
                if constexpr (skChannelIdx == 4) return Dma2_Channel4;
                if constexpr (skChannelIdx == 5) return Dma2_Channel5;
                if constexpr (skChannelIdx == 6) return Dma2_Channel6;
                if constexpr (skChannelIdx == 7) return Dma2_Channel7;
                if constexpr (skChannelIdx == 8) return Dma2_Channel8;
            }
        }

        consteval uint32_t get_ll_channel_id() const
        {
            // NOLINTBEGIN(readability-magic-numbers)
            if constexpr (skChannelIdx == 1)
            {
                return LL_DMA_CHANNEL_1;
            }
            else if constexpr (skChannelIdx == 2)
            {
                return LL_DMA_CHANNEL_2;
            }
            else if constexpr (skChannelIdx == 3)
            {
                return LL_DMA_CHANNEL_3;
            }
            else if constexpr (skChannelIdx == 4)
            {
                return LL_DMA_CHANNEL_4;
            }
            else if constexpr (skChannelIdx == 5)
            {
                return LL_DMA_CHANNEL_5;
            }
            else if constexpr (skChannelIdx == 6)
            {
                return LL_DMA_CHANNEL_6;
            }
            else if constexpr (skChannelIdx == 7)
            {
                return LL_DMA_CHANNEL_7;
            }
            else if constexpr (skChannelIdx == 8)
            {
                return LL_DMA_CHANNEL_8;
            }
        }

        consteval DmaMuxChannel get_mux_channel_id() const
        {
            // NOLINTBEGIN(readability-magic-numbers)
            if constexpr (tkPeripheralId == DmaPeripheralId::kDma1)
            {
                if constexpr (tkChannelId == DmaChannelId::kChannel1)
                {
                    return DmaMuxChannel::kDma1Channel1;
                }
                else if constexpr (tkChannelId == DmaChannelId::kChannel2)
                {
                    return DmaMuxChannel::kDma1Channel2;
                }
                else if constexpr (tkChannelId == DmaChannelId::kChannel3)
                {
                    return DmaMuxChannel::kDma1Channel3;
                }
                else if constexpr (tkChannelId == DmaChannelId::kChannel4)
                {
                    return DmaMuxChannel::kDma1Channel4;
                }
                else if constexpr (tkChannelId == DmaChannelId::kChannel5)
                {
                    return DmaMuxChannel::kDma1Channel5;
                }
                else if constexpr (tkChannelId == DmaChannelId::kChannel6)
                {
                    return DmaMuxChannel::kDma1Channel6;
                }
                else if constexpr (tkChannelId == DmaChannelId::kChannel7)
                {
                    return DmaMuxChannel::kDma1Channel7;
                }
                else if constexpr (tkChannelId == DmaChannelId::kChannel8)
                {
                    return DmaMuxChannel::kDma1Channel8;
                }
                else
                {
                    static_assert(false, "Invalid DMA Channel ID");
                }
            }
            else if constexpr (tkPeripheralId == DmaPeripheralId::kDma2)
            {
                if constexpr (tkChannelId == DmaChannelId::kChannel1)
                {
                    return DmaMuxChannel::kDma2Channel1;
                }
                else if constexpr (tkChannelId == DmaChannelId::kChannel2)
                {
                    return DmaMuxChannel::kDma2Channel2;
                }
                else if constexpr (tkChannelId == DmaChannelId::kChannel3)
                {
                    return DmaMuxChannel::kDma2Channel3;
                }
                else if constexpr (tkChannelId == DmaChannelId::kChannel4)
                {
                    return DmaMuxChannel::kDma2Channel4;
                }
                else if constexpr (tkChannelId == DmaChannelId::kChannel5)
                {
                    return DmaMuxChannel::kDma2Channel5;
                }
                else if constexpr (tkChannelId == DmaChannelId::kChannel6)
                {
                    return DmaMuxChannel::kDma2Channel6;
                }
                else if constexpr (tkChannelId == DmaChannelId::kChannel7)
                {
                    return DmaMuxChannel::kDma2Channel7;
                }
                else if constexpr (tkChannelId == DmaChannelId::kChannel8)
                {
                    return DmaMuxChannel::kDma2Channel8;
                }
                else
                {
                    static_assert(kAlwaysFalseV<tkChannelId>, "Invalid DMA Channel ID");
                }
            }
            else
            {
                static_assert(kAlwaysFalseV<tkPeripheralId>, "Invalid DMA Peripheral ID");
            }
            // NOLINTEND(readability-magic-numbers)
        }

    public:
        static inline Dma_Channel_TypeDef* const skInstance     = get_channel_instance();
        static constexpr uint32_t                skChannelLLId  = get_ll_channel_id();
        static constexpr DmaMuxChannel           skMuxChannelId = get_mux_channel_id();

        static constexpr IRQn_Type skIRQn = get_irq_n();
    };

}  // namespace valle::platform