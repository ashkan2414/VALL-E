#include "valle/platform/hardware/dma.hpp"

#if __has_include("app_isr_bindings.hpp")
#include "app_isr_bindings.hpp"
#endif

namespace valle::platform
{
    /**
     * @brief DMA Interrupt Handler Router
     *
     * @tparam tkChannelSpec    DMA Channel Specification.
     */
    template <DmaChannelSpec tkChannelSpec>
    static void dma_irq_handler()
    {
        DmaChannelInterruptIrqRouterContext::dispatch<tkChannelSpec>();
    }

    // ============================================================================
    // STM32 VECTOR TABLE BINDINGS (External C)
    // ============================================================================
    // On STM32G4, every DMA channel has its own unique vector.

    extern "C"
    {
        // NOLINTBEGIN(readability-identifier-naming)
        // --- DMA 1 --------------------------------------------------------------
        void DMA1_Channel1_IRQHandler(void)
        {
            dma_irq_handler<kDma1Channel1Spec>();
        }
        void DMA1_Channel2_IRQHandler(void)
        {
            dma_irq_handler<kDma1Channel2Spec>();
        }
        void DMA1_Channel3_IRQHandler(void)
        {
            dma_irq_handler<kDma1Channel3Spec>();
        }
        void DMA1_Channel4_IRQHandler(void)
        {
            dma_irq_handler<kDma1Channel4Spec>();
        }
        void DMA1_Channel5_IRQHandler(void)
        {
            dma_irq_handler<kDma1Channel5Spec>();
        }
        void DMA1_Channel6_IRQHandler(void)
        {
            dma_irq_handler<kDma1Channel6Spec>();
        }
        void DMA1_Channel7_IRQHandler(void)
        {
            dma_irq_handler<kDma1Channel7Spec>();
        }
        void DMA1_Channel8_IRQHandler(void)
        {
            dma_irq_handler<kDma1Channel8Spec>();
        }

        // --- DMA 2 --------------------------------------------------------------
        void DMA2_Channel1_IRQHandler(void)
        {
            dma_irq_handler<kDma2Channel1Spec>();
        }
        void DMA2_Channel2_IRQHandler(void)
        {
            dma_irq_handler<kDma2Channel2Spec>();
        }
        void DMA2_Channel3_IRQHandler(void)
        {
            dma_irq_handler<kDma2Channel3Spec>();
        }
        void DMA2_Channel4_IRQHandler(void)
        {
            dma_irq_handler<kDma2Channel4Spec>();
        }
        void DMA2_Channel5_IRQHandler(void)
        {
            dma_irq_handler<kDma2Channel5Spec>();
        }
        void DMA2_Channel6_IRQHandler(void)
        {
            dma_irq_handler<kDma2Channel6Spec>();
        }
        void DMA2_Channel7_IRQHandler(void)
        {
            dma_irq_handler<kDma2Channel7Spec>();
        }
        void DMA2_Channel8_IRQHandler(void)
        {
            dma_irq_handler<kDma2Channel8Spec>();
        }

        // NOLINTEND(readability-identifier-naming)
    }

}  // namespace valle::platform