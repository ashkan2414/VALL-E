#pragma once

#include "valle/platform/hardware/dma/interface.hpp"

namespace valle::platform
{
    // ============================================================================
    // INTERRUPT TRAITS
    // ============================================================================
    enum class DMAChannelInterruptType : uint8_t
    {
        kTransferComplete,
        kHalfTransfer,
        kTransferError
    };

    template <DMAControllerID tkControllerID, DMAChannelID tkChannelID, DMAChannelInterruptType tkIntType>
    struct DMAChannelInterruptTraits;

#define DEFINE_DMA_INT_TRAIT(tkIntType, ll_name, should_clear)                                                 \
    template <DMAControllerID tkControllerID, DMAChannelID tkChannelID>                                        \
    struct DMAChannelInterruptTraits<tkControllerID, tkChannelID, (tkIntType)>                                 \
    {                                                                                                          \
        static constexpr bool skShouldClear = (should_clear);                                                  \
                                                                                                               \
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
                LL_DMA_ClearFlag_##ll_name##1(DMAControllerTraits<tkControllerID>::skInstance);                \
            }                                                                                                  \
            else if constexpr (tkChannelID == 2)                                                               \
            {                                                                                                  \
                LL_DMA_ClearFlag_##ll_name##2(DMAControllerTraits<tkControllerID>::skInstance);                \
            }                                                                                                  \
            else if constexpr (tkChannelID == 3)                                                               \
            {                                                                                                  \
                LL_DMA_ClearFlag_##ll_name##3(DMAControllerTraits<tkControllerID>::skInstance);                \
            }                                                                                                  \
            else if constexpr (tkChannelID == 4)                                                               \
            {                                                                                                  \
                LL_DMA_ClearFlag_##ll_name##4(DMAControllerTraits<tkControllerID>::skInstance);                \
            }                                                                                                  \
            else if constexpr (tkChannelID == 5)                                                               \
            {                                                                                                  \
                LL_DMA_ClearFlag_##ll_name##5(DMAControllerTraits<tkControllerID>::skInstance);                \
            }                                                                                                  \
            else if constexpr (tkChannelID == 6)                                                               \
            {                                                                                                  \
                LL_DMA_ClearFlag_##ll_name##6(DMAControllerTraits<tkControllerID>::skInstance);                \
            }                                                                                                  \
            else if constexpr (tkChannelID == 7)                                                               \
            {                                                                                                  \
                LL_DMA_ClearFlag_##ll_name##7(DMAControllerTraits<tkControllerID>::skInstance);                \
            }                                                                                                  \
            else if constexpr (tkChannelID == 8)                                                               \
            {                                                                                                  \
                LL_DMA_ClearFlag_##ll_name##8(DMAControllerTraits<tkControllerID>::skInstance);                \
            }                                                                                                  \
            else                                                                                               \
            {                                                                                                  \
                static_assert(false, "Invalid DMA Channel ID");                                                \
            }                                                                                                  \
        }                                                                                                      \
    };

    DEFINE_DMA_INT_TRAIT(DMAChannelInterruptType::kTransferComplete, TC, true);
    DEFINE_DMA_INT_TRAIT(DMAChannelInterruptType::kHalfTransfer, HT, true);
    DEFINE_DMA_INT_TRAIT(DMAChannelInterruptType::kTransferError, TE, true);

#undef DEFINE_DMA_INT_TRAIT

    // ===========================================================================
    // INTERRUPT CONTROLLER
    // ===========================================================================
    struct DMAChannelInterruptMask
    {
        union
        {
            uint32_t mask;  // Full mask (for easy initialization)
            struct
            {
                bool transfer_complete : 1 = false;  // Transfer Complete Interrupt Enable
                bool half_transfer : 1     = false;  // Half Transfer Interrupt Enable
                bool transfer_error : 1    = false;  // Transfer Error Interrupt Enable
            };
        };
    };

    /**
     * @brief Configuration for DMA Interrupts.
     */
    struct DMAChannelInterruptConfig
    {
        uint32_t                priority = 5;  // NVIC Priority (0 = Highest, 15 = Lowest)
        DMAChannelInterruptMask interrupts{};  // Which interrupts to enable
    };

    template <DMAControllerID tkControllerID, DMAChannelID tkChannelID>
        requires(kValidDMAControllerID<tkControllerID> && kValidDMAChannel<tkChannelID>)
    struct DMAChannelInterruptController
    {
    public:
        static constexpr DMAControllerID skControllerID = tkControllerID;
        static constexpr DMAChannelID    skChannelID    = tkChannelID;
        using ControllerTraitsT                         = DMAControllerTraits<tkControllerID>;
        using ChannelTraitsT                            = DMAChannelTraits<tkControllerID, tkChannelID>;

        /**
         * @brief Enable interrupts for this channel.
         * @param config Configuration for DMA Interrupts.
         */
        static void enable_interrupts(const DMAChannelInterruptConfig& config)
        {
            if (config.interrupts.mask == 0)
            {
                return;  // No interrupts to enable
            }

            if (config.interrupts.transfer_complete)
            {
                DMAChannelInterruptTraits<tkControllerID, tkChannelID, DMAChannelInterruptType::kTransferComplete>::
                    ack();
                DMAChannelInterruptTraits<tkControllerID, tkChannelID, DMAChannelInterruptType::kTransferComplete>::
                    enable();
            }
            if (config.interrupts.half_transfer)
            {
                DMAChannelInterruptTraits<tkControllerID, tkChannelID, DMAChannelInterruptType::kHalfTransfer>::ack();
                DMAChannelInterruptTraits<tkControllerID, tkChannelID, DMAChannelInterruptType::kHalfTransfer>::
                    enable();
            }

            if (config.interrupts.transfer_error)
            {
                DMAChannelInterruptTraits<tkControllerID, tkChannelID, DMAChannelInterruptType::kTransferError>::ack();
                DMAChannelInterruptTraits<tkControllerID, tkChannelID, DMAChannelInterruptType::kTransferError>::
                    enable();
            }

            NVIC_SetPriority(ChannelTraitsT::skIRQn, config.priority);
            NVIC_EnableIRQ(ChannelTraitsT::skIRQn);
        }

        /**
         * @brief Disable interrupts for this channel.
         */
        static void disable_interrupts()
        {
            DMAChannelInterruptTraits<tkControllerID, tkChannelID, DMAChannelInterruptType::kTransferComplete>::
                disable();
            DMAChannelInterruptTraits<tkControllerID, tkChannelID, DMAChannelInterruptType::kHalfTransfer>::disable();
            DMAChannelInterruptTraits<tkControllerID, tkChannelID, DMAChannelInterruptType::kTransferError>::disable();
            NVIC_DisableIRQ(ChannelTraitsT::skIRQn);
        }
    };

    // ===========================================================================
    // INTERRUPT ROUTERS
    // ===========================================================================

    // ---------------------------------------------------------------------------
    // GLOBAL ISR ROUTER
    // ---------------------------------------------------------------------------

    /**
     * @brief Global ISR Router for a specific DMA.
     * Specializing this allows you to handle the entire ISR in one function
     * (e.g., when delegating to the ST HAL).
     *
     * @tparam tkControllerID DMA Controller ID.
     * @tparam tkChannelID    DMA Channel ID.
     */
    template <DMAControllerID tkControllerID, DMAChannelID tkChannelID>
    struct DMAGlobalISRRouter
    {
        using UnboundIsrHandlerTag = void;

        static void handle()
        {
        }
    };

    // ---------------------------------------------------------------------------
    // GRANULAR ISR ROUTER
    // ---------------------------------------------------------------------------

    /**
     * @brief DMA ISR Router
     *
     * Specialize this template to handle specific DMA interrupts for a given
     * controller and channel.
     *
     * @tparam tkControllerID DMA Controller ID (1 or 2)
     * @tparam tkChannelID    DMA Channel ID (1-8)
     * @tparam tkIntType      DMA Interrupt Type
     */
    template <DMAControllerID tkControllerID, DMAChannelID tkChannelID, DMAChannelInterruptType tkIntType>
    struct DMAISRRouter
    {
        using UnboundIsrHandlerTag = void;
        static void handle()
        {
            // Do nothing by default (optimized away)
        }
    };

}  // namespace valle::platform