#include "Valle/Device/Devices/dma.hpp"

#include "magic_enum/magic_enum.hpp"

#if __has_include("app_isr_bindings.hpp")
#include "app_isr_bindings.hpp"
#endif

namespace valle
{
    /**
     * @brief Check if any granular ISR handler is bound for this DMA Channel.
     *
     * @tparam tkControllerID DMA Controller ID.
     * @tparam tkChannelID    DMA Channel ID.
     */
    template <DMAControllerID tkControllerID, DMAChannelID tkChannelID>
        requires(kValidDMAID<tkControllerID> && kValidDMAChannel<tkChannelID>)
    [[nodiscard]] consteval static inline bool dma_has_any_granular_isr_handler()
    {
        constexpr auto values = magic_enum::enum_values<DMAInterruptType>();

        return [values]<std::size_t... Is>(std::index_sequence<Is...>)
        {
            return (CBoundISRRouter<DMAISRRouter<tkControllerID, tkChannelID, values[Is]>> || ...);
        }(std::make_index_sequence<values.size()>{});
    }

    /**
     * @brief DMA Interrupt Handler Router
     *
     * @tparam tkControllerID DMA Controller Index (1 or 2)
     * @tparam tkChannelID DMA Channel Index (1-8)
     */
    template <DMAControllerID tkControllerID, DMAChannelID tkChannelID>
        requires(kValidDMAID<tkControllerID> && kValidDMAChannel<tkChannelID>)
    static inline void dma_irq_handler()
    {
        using GlobalRouterT               = DMAGlobalISRRouter<tkControllerID, tkChannelID>;
        constexpr bool kHasGlobalRouter   = CBoundISRRouter<GlobalRouterT>;
        constexpr bool kHasGranularRouter = dma_has_any_granular_isr_handler<tkControllerID, tkChannelID>();
        static_assert(!(kHasGlobalRouter && kHasGranularRouter), "VALLE CONFLICT: Global and Granular ISRs detected.");

        if constexpr (kHasGlobalRouter)
        {
            GlobalRouterT::handle();
            return;
        }

#define HANDLE_DMA_INT(tkIntType)                                                     \
    {                                                                                 \
        using RouterT = DMAISRRouter<tkControllerID, tkChannelID, (tkIntType)>;       \
        using TraitsT = DMAInterruptTraits<tkControllerID, tkChannelID, (tkIntType)>; \
        if constexpr (CBoundISRRouter<RouterT>)                                       \
        {                                                                             \
            if (TraitsT::is_pending())                                                \
            {                                                                         \
                if constexpr (kISRRouterConfigAck<RouterT>)                           \
                {                                                                     \
                    TraitsT::ack();                                                   \
                }                                                                     \
                RouterT::handle();                                                    \
            }                                                                         \
        }                                                                             \
    }

        HANDLE_DMA_INT(DMAInterruptType::kTransferError);
        HANDLE_DMA_INT(DMAInterruptType::kHalfTransfer);
        HANDLE_DMA_INT(DMAInterruptType::kTransferComplete);
#undef HANDLE_DMA_INT
    }

    // ============================================================================
    // STM32 VECTOR TABLE BINDINGS (External C)
    // ============================================================================
    // On STM32G4, every DMA channel has its own unique vector.

    extern "C"
    {
        // --- DMA 1 --------------------------------------------------------------
        void DMA1_Channel1_IRQHandler(void)
        {
            dma_irq_handler<1, 1>();
        }
        void DMA1_Channel2_IRQHandler(void)
        {
            dma_irq_handler<1, 2>();
        }
        void DMA1_Channel3_IRQHandler(void)
        {
            dma_irq_handler<1, 3>();
        }
        void DMA1_Channel4_IRQHandler(void)
        {
            dma_irq_handler<1, 4>();
        }
        void DMA1_Channel5_IRQHandler(void)
        {
            dma_irq_handler<1, 5>();
        }
        void DMA1_Channel6_IRQHandler(void)
        {
            dma_irq_handler<1, 6>();
        }
        void DMA1_Channel7_IRQHandler(void)
        {
            dma_irq_handler<1, 7>();
        }
        void DMA1_Channel8_IRQHandler(void)
        {
            dma_irq_handler<1, 8>();
        }

        // --- DMA 2 --------------------------------------------------------------
        void DMA2_Channel1_IRQHandler(void)
        {
            dma_irq_handler<2, 1>();
        }
        void DMA2_Channel2_IRQHandler(void)
        {
            dma_irq_handler<2, 2>();
        }
        void DMA2_Channel3_IRQHandler(void)
        {
            dma_irq_handler<2, 3>();
        }
        void DMA2_Channel4_IRQHandler(void)
        {
            dma_irq_handler<2, 4>();
        }
        void DMA2_Channel5_IRQHandler(void)
        {
            dma_irq_handler<2, 5>();
        }
        void DMA2_Channel6_IRQHandler(void)
        {
            dma_irq_handler<2, 6>();
        }
        void DMA2_Channel7_IRQHandler(void)
        {
            dma_irq_handler<2, 7>();
        }
        void DMA2_Channel8_IRQHandler(void)
        {
            dma_irq_handler<2, 8>();
        }
    }

}  // namespace valle