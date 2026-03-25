#include "valle/platform/devices/dma.hpp"

#include "magic_enum/magic_enum.hpp"

#if __has_include("app_isr_bindings.hpp")
#include "app_isr_bindings.hpp"
#endif

namespace valle::platform
{
    /**
     * @brief Check if any granular ISR handler is bound for this DMA Channel.
     *
     * @tparam tkPeripheralId DMA Peripheral ID.
     * @tparam tkChannelId    DMA Channel ID.
     */
    template <DmaPeripheralId tkPeripheralId, DmaChannelId tkChannelId>
    [[nodiscard]] consteval static inline bool dma_has_any_granular_isr_handler()
    {
        constexpr auto values = magic_enum::enum_values<DmaChannelInterruptType>();

        return [values]<std::size_t... Is>(std::index_sequence<Is...>)
        {
            return (CBoundIsrRouter<DmaIsrRouter<tkPeripheralId, tkChannelId, values[Is]>> || ...);
        }(std::make_index_sequence<values.size()>{});
    }

    /**
     * @brief DMA Interrupt Handler Router
     *
     * @tparam tkPeripheralId DMA Controller Index (1 or 2)
     * @tparam tkChannelId DMA Channel Index (1-8)
     */
    template <DmaPeripheralId tkPeripheralId, DmaChannelId tkChannelId>
    static inline void dma_irq_handler()
    {
        using GlobalRouterT               = DmaGlobalIsrRouter<tkPeripheralId, tkChannelId>;
        constexpr bool kHasGlobalRouter   = CBoundIsrRouter<GlobalRouterT>;
        constexpr bool kHasGranularRouter = dma_has_any_granular_isr_handler<tkPeripheralId, tkChannelId>();
        static_assert(!(kHasGlobalRouter && kHasGranularRouter), "VALLE CONFLICT: Global and Granular ISRs detected.");

        if constexpr (kHasGlobalRouter)
        {
            GlobalRouterT::handle();
            return;
        }

#define HANDLE_DMA_INT(tkIntType)                                                            \
    {                                                                                        \
        using RouterT = DmaIsrRouter<tkPeripheralId, tkChannelId, (tkIntType)>;              \
        using TraitsT = DmaChannelInterruptTraits<tkPeripheralId, tkChannelId, (tkIntType)>; \
        if constexpr (CBoundIsrRouter<RouterT>)                                              \
        {                                                                                    \
            if (TraitsT::is_pending())                                                       \
            {                                                                                \
                if constexpr (kIsrRouterConfigAck<RouterT>)                                  \
                {                                                                            \
                    TraitsT::ack();                                                          \
                }                                                                            \
                RouterT::handle();                                                           \
            }                                                                                \
        }                                                                                    \
        else                                                                                 \
        {                                                                                    \
            if constexpr (TraitsT::skShouldClear)                                            \
            {                                                                                \
                if (TraitsT::is_pending())                                                   \
                {                                                                            \
                    TraitsT::ack();                                                          \
                }                                                                            \
            }                                                                                \
        }                                                                                    \
    }

        HANDLE_DMA_INT(DmaChannelInterruptType::kTransferError);
        HANDLE_DMA_INT(DmaChannelInterruptType::kHalfTransfer);
        HANDLE_DMA_INT(DmaChannelInterruptType::kTransferComplete);
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
            dma_irq_handler<DmaPeripheralId::kDma1, DmaChannelId::kChannel1>();
        }
        void DMA1_Channel2_IRQHandler(void)
        {
            dma_irq_handler<DmaPeripheralId::kDma1, DmaChannelId::kChannel2>();
        }
        void DMA1_Channel3_IRQHandler(void)
        {
            dma_irq_handler<DmaPeripheralId::kDma1, DmaChannelId::kChannel3>();
        }
        void DMA1_Channel4_IRQHandler(void)
        {
            dma_irq_handler<DmaPeripheralId::kDma1, DmaChannelId::kChannel4>();
        }
        void DMA1_Channel5_IRQHandler(void)
        {
            dma_irq_handler<DmaPeripheralId::kDma1, DmaChannelId::kChannel5>();
        }
        void DMA1_Channel6_IRQHandler(void)
        {
            dma_irq_handler<DmaPeripheralId::kDma1, DmaChannelId::kChannel6>();
        }
        void DMA1_Channel7_IRQHandler(void)
        {
            dma_irq_handler<DmaPeripheralId::kDma1, DmaChannelId::kChannel7>();
        }
        void DMA1_Channel8_IRQHandler(void)
        {
            dma_irq_handler<DmaPeripheralId::kDma1, DmaChannelId::kChannel8>();
        }

        // --- DMA 2 --------------------------------------------------------------
        void DMA2_Channel1_IRQHandler(void)
        {
            dma_irq_handler<DmaPeripheralId::kDma2, DmaChannelId::kChannel1>();
        }
        void DMA2_Channel2_IRQHandler(void)
        {
            dma_irq_handler<DmaPeripheralId::kDma2, DmaChannelId::kChannel2>();
        }
        void DMA2_Channel3_IRQHandler(void)
        {
            dma_irq_handler<DmaPeripheralId::kDma2, DmaChannelId::kChannel3>();
        }
        void DMA2_Channel4_IRQHandler(void)
        {
            dma_irq_handler<DmaPeripheralId::kDma2, DmaChannelId::kChannel4>();
        }
        void DMA2_Channel5_IRQHandler(void)
        {
            dma_irq_handler<DmaPeripheralId::kDma2, DmaChannelId::kChannel5>();
        }
        void DMA2_Channel6_IRQHandler(void)
        {
            dma_irq_handler<DmaPeripheralId::kDma2, DmaChannelId::kChannel6>();
        }
        void DMA2_Channel7_IRQHandler(void)
        {
            dma_irq_handler<DmaPeripheralId::kDma2, DmaChannelId::kChannel7>();
        }
        void DMA2_Channel8_IRQHandler(void)
        {
            dma_irq_handler<DmaPeripheralId::kDma2, DmaChannelId::kChannel8>();
        }
    }

}  // namespace valle::platform