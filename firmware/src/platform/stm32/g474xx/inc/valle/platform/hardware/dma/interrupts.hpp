#pragma once

#include "valle/platform/hardware/dma/interface.hpp"

namespace valle::platform
{
    // ============================================================================
    // INTERRUPT TRAITS
    // ============================================================================
    enum class DmaChannelInterruptType : uint8_t
    {
        kTransferComplete,
        kHalfTransfer,
        kTransferError
    };

    template <DmaPeripheralId tkPeripheralId, DmaChannelId tkChannelId, DmaChannelInterruptType tkIntType>
    struct DmaChannelInterruptTraits;

#define DEFINE_DMA_INT_TRAIT(tkIntType, ll_name, should_clear)                                                 \
    template <DmaPeripheralId tkPeripheralId, DmaChannelId tkChannelId>                                        \
    struct DmaChannelInterruptTraits<tkPeripheralId, tkChannelId, (tkIntType)>                                 \
    {                                                                                                          \
        static constexpr bool skShouldClear = (should_clear);                                                  \
                                                                                                               \
        static inline void enable()                                                                            \
        {                                                                                                      \
            LL_DMA_EnableIT_##ll_name(DmaControllerTraits<tkPeripheralId>::skInstance,                         \
                                      DmaChannelTraits<tkPeripheralId, tkChannelId>::skChannelLLId);           \
        }                                                                                                      \
        static inline void disable()                                                                           \
        {                                                                                                      \
            LL_DMA_DisableIT_##ll_name(DmaControllerTraits<tkPeripheralId>::skInstance,                        \
                                       DmaChannelTraits<tkPeripheralId, tkChannelId>::skChannelLLId);          \
        }                                                                                                      \
        static inline bool is_enabled()                                                                        \
        {                                                                                                      \
            return LL_DMA_IsEnabledIT_##ll_name(DmaControllerTraits<tkPeripheralId>::skInstance,               \
                                                DmaChannelTraits<tkPeripheralId, tkChannelId>::skChannelLLId); \
        }                                                                                                      \
        static inline bool flag_active()                                                                       \
        {                                                                                                      \
            if constexpr (tkChannelId == DmaChannelId::kChannel1)                                              \
            {                                                                                                  \
                return LL_DMA_IsActiveFlag_##ll_name##1(DmaControllerTraits<tkPeripheralId>::skInstance);      \
            }                                                                                                  \
            else if constexpr (tkChannelId == DmaChannelId::kChannel2)                                         \
            {                                                                                                  \
                return LL_DMA_IsActiveFlag_##ll_name##2(DmaControllerTraits<tkPeripheralId>::skInstance);      \
            }                                                                                                  \
            else if constexpr (tkChannelId == DmaChannelId::kChannel3)                                         \
            {                                                                                                  \
                return LL_DMA_IsActiveFlag_##ll_name##3(DmaControllerTraits<tkPeripheralId>::skInstance);      \
            }                                                                                                  \
            else if constexpr (tkChannelId == DmaChannelId::kChannel4)                                         \
            {                                                                                                  \
                return LL_DMA_IsActiveFlag_##ll_name##4(DmaControllerTraits<tkPeripheralId>::skInstance);      \
            }                                                                                                  \
            else if constexpr (tkChannelId == DmaChannelId::kChannel5)                                         \
            {                                                                                                  \
                return LL_DMA_IsActiveFlag_##ll_name##5(DmaControllerTraits<tkPeripheralId>::skInstance);      \
            }                                                                                                  \
            else if constexpr (tkChannelId == DmaChannelId::kChannel6)                                         \
            {                                                                                                  \
                return LL_DMA_IsActiveFlag_##ll_name##6(DmaControllerTraits<tkPeripheralId>::skInstance);      \
            }                                                                                                  \
            else if constexpr (tkChannelId == DmaChannelId::kChannel7)                                         \
            {                                                                                                  \
                return LL_DMA_IsActiveFlag_##ll_name##7(DmaControllerTraits<tkPeripheralId>::skInstance);      \
            }                                                                                                  \
            else if constexpr (tkChannelId == DmaChannelId::kChannel8)                                         \
            {                                                                                                  \
                return LL_DMA_IsActiveFlag_##ll_name##8(DmaControllerTraits<tkPeripheralId>::skInstance);      \
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
            if constexpr (tkChannelId == DmaChannelId::kChannel1)                                              \
            {                                                                                                  \
                LL_DMA_ClearFlag_##ll_name##1(DmaControllerTraits<tkPeripheralId>::skInstance);                \
            }                                                                                                  \
            else if constexpr (tkChannelId == DmaChannelId::kChannel2)                                         \
            {                                                                                                  \
                LL_DMA_ClearFlag_##ll_name##2(DmaControllerTraits<tkPeripheralId>::skInstance);                \
            }                                                                                                  \
            else if constexpr (tkChannelId == DmaChannelId::kChannel3)                                         \
            {                                                                                                  \
                LL_DMA_ClearFlag_##ll_name##3(DmaControllerTraits<tkPeripheralId>::skInstance);                \
            }                                                                                                  \
            else if constexpr (tkChannelId == DmaChannelId::kChannel4)                                         \
            {                                                                                                  \
                LL_DMA_ClearFlag_##ll_name##4(DmaControllerTraits<tkPeripheralId>::skInstance);                \
            }                                                                                                  \
            else if constexpr (tkChannelId == DmaChannelId::kChannel5)                                         \
            {                                                                                                  \
                LL_DMA_ClearFlag_##ll_name##5(DmaControllerTraits<tkPeripheralId>::skInstance);                \
            }                                                                                                  \
            else if constexpr (tkChannelId == DmaChannelId::kChannel6)                                         \
            {                                                                                                  \
                LL_DMA_ClearFlag_##ll_name##6(DmaControllerTraits<tkPeripheralId>::skInstance);                \
            }                                                                                                  \
            else if constexpr (tkChannelId == DmaChannelId::kChannel7)                                         \
            {                                                                                                  \
                LL_DMA_ClearFlag_##ll_name##7(DmaControllerTraits<tkPeripheralId>::skInstance);                \
            }                                                                                                  \
            else if constexpr (tkChannelId == DmaChannelId::kChannel8)                                         \
            {                                                                                                  \
                LL_DMA_ClearFlag_##ll_name##8(DmaControllerTraits<tkPeripheralId>::skInstance);                \
            }                                                                                                  \
            else                                                                                               \
            {                                                                                                  \
                static_assert(false, "Invalid DMA Channel ID");                                                \
            }                                                                                                  \
        }                                                                                                      \
    };

    DEFINE_DMA_INT_TRAIT(DmaChannelInterruptType::kTransferComplete, TC, true);
    DEFINE_DMA_INT_TRAIT(DmaChannelInterruptType::kHalfTransfer, HT, true);
    DEFINE_DMA_INT_TRAIT(DmaChannelInterruptType::kTransferError, TE, true);

#undef DEFINE_DMA_INT_TRAIT

    // ===========================================================================
    // INTERRUPT CONTROLLER
    // ===========================================================================
    struct DmaChannelInterruptMask
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
    struct DmaChannelInterruptConfig
    {
        uint32_t                priority = 5;  // NVIC Priority (0 = Highest, 15 = Lowest)
        DmaChannelInterruptMask interrupts{};  // Which interrupts to enable
    };

    template <DmaPeripheralId tkPeripheralId, DmaChannelId tkChannelId>

    struct DmaChannelInterruptController
    {
    public:
        static constexpr DmaPeripheralId skPeripheralId = tkPeripheralId;
        static constexpr DmaChannelId    skChannelId    = tkChannelId;
        using ControllerTraitsT                         = DmaControllerTraits<tkPeripheralId>;
        using ChannelTraitsT                            = DmaChannelTraits<tkPeripheralId, tkChannelId>;

        /**
         * @brief Enable interrupts for this channel.
         * @param config Configuration for DMA Interrupts.
         */
        static void enable_interrupts(const DmaChannelInterruptConfig& config)
        {
            if (config.interrupts.mask == 0)
            {
                return;  // No interrupts to enable
            }

            if (config.interrupts.transfer_complete)
            {
                DmaChannelInterruptTraits<tkPeripheralId, tkChannelId, DmaChannelInterruptType::kTransferComplete>::
                    ack();
                DmaChannelInterruptTraits<tkPeripheralId, tkChannelId, DmaChannelInterruptType::kTransferComplete>::
                    enable();
            }
            if (config.interrupts.half_transfer)
            {
                DmaChannelInterruptTraits<tkPeripheralId, tkChannelId, DmaChannelInterruptType::kHalfTransfer>::ack();
                DmaChannelInterruptTraits<tkPeripheralId, tkChannelId, DmaChannelInterruptType::kHalfTransfer>::
                    enable();
            }

            if (config.interrupts.transfer_error)
            {
                DmaChannelInterruptTraits<tkPeripheralId, tkChannelId, DmaChannelInterruptType::kTransferError>::ack();
                DmaChannelInterruptTraits<tkPeripheralId, tkChannelId, DmaChannelInterruptType::kTransferError>::
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
            DmaChannelInterruptTraits<tkPeripheralId, tkChannelId, DmaChannelInterruptType::kTransferComplete>::
                disable();
            DmaChannelInterruptTraits<tkPeripheralId, tkChannelId, DmaChannelInterruptType::kHalfTransfer>::disable();
            DmaChannelInterruptTraits<tkPeripheralId, tkChannelId, DmaChannelInterruptType::kTransferError>::disable();
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
     * @tparam tkPeripheralId DMA Peripheral ID.
     * @tparam tkChannelId    DMA Channel ID.
     */
    template <DmaPeripheralId tkPeripheralId, DmaChannelId tkChannelId>
    struct DmaGlobalIsrRouter
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
     * @tparam tkPeripheralId DMA Peripheral ID (1 or 2)
     * @tparam tkChannelId    DMA Channel ID (1-8)
     * @tparam tkIntType      DMA Interrupt Type
     */
    template <DmaPeripheralId tkPeripheralId, DmaChannelId tkChannelId, DmaChannelInterruptType tkIntType>
    struct DmaIsrRouter
    {
        using UnboundIsrHandlerTag = void;
        static void handle()
        {
            // Do nothing by default (optimized away)
        }
    };

}  // namespace valle::platform
