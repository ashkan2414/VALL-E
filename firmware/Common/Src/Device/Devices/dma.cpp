#include "Valle/Device/Devices/dma.hpp"

// ============================================================================
// DMA INTERRUPT TYPES
// ============================================================================

/**
 * @brief DMA Interrupt Handler Router
 *
 * @tparam tkDmaIdx DMA Controller Index (1 or 2)
 * @tparam tkChannelIdx DMA Channel Index (1-8)
 */
template <uint8_t tkDmaIdx, uint8_t tkChannelIdx>
    requires(kValidDMAIndex<tkDmaIdx> && kValidDMAChannel<tkChannelIdx>)
static inline void dma_irq_handler()
{
    using DMAChannelT = DMAChannelDevice<tkDmaIdx, tkChannelIdx>;

    // --------------------------------------------------------------------
    // ERROR: Transfer Error (TE) - Highest Priority
    // --------------------------------------------------------------------
    if (DMAChannelT::te_int_pending())
    {
        DMAChannelT::ack_te_int();
        DMAIsrRouter<tkDmaIdx, tkChannelIdx, DMAInterruptType::kTransferError>::handle();
    }

    // --------------------------------------------------------------------
    // EVENT: Half Transfer (HT)
    // --------------------------------------------------------------------
    if (DMAChannelT::ht_int_pending())
    {
        DMAChannelT::ack_ht_int();
        DMAIsrRouter<tkDmaIdx, tkChannelIdx, DMAInterruptType::kHalfTransfer>::handle();
    }

    // --------------------------------------------------------------------
    // EVENT: Transfer Complete (TC)
    // --------------------------------------------------------------------
    if (DMAChannelT::tc_int_pending())
    {
        DMAChannelT::ack_tc_int();
        DMAIsrRouter<tkDmaIdx, tkChannelIdx, DMAInterruptType::kTransferComplete>::handle();
    }
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