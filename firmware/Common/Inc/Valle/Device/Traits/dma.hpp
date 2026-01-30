#pragma once

#include "stm32g4xx_ll_bus.h"
#include "stm32g4xx_ll_dma.h"
#include "stm32g4xx_ll_dmamux.h"

namespace valle
{

    // ============================================================================
    // CONSTANTS & CHECKS
    // ============================================================================

    using DMAControllerID = uint8_t;

    template <DMAControllerID tkControllerID>
    constexpr bool kValidDMAID = (tkControllerID == 1 || tkControllerID == 2);

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
        static inline DMA_TypeDef* const skInstance = DMA1;
        static constexpr uint32_t        skClock    = LL_AHB1_GRP1_PERIPH_DMA1;
    };

    template <>
    struct DMAControllerTraits<2>
    {
        static inline DMA_TypeDef* const skInstance = DMA2;
        static constexpr uint32_t        skClock    = LL_AHB1_GRP1_PERIPH_DMA2;
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
        static inline DMA_Channel_TypeDef* get_channel_instance()
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

    public:
        static inline DMA_Channel_TypeDef* const skInstance = get_channel_instance();
        // Helper to map index (1-8) to LL Constant (LL_DMA_CHANNEL_x)
        static constexpr uint32_t skChannelLLID = (tkChannelID == 1)   ? LL_DMA_CHANNEL_1
                                                  : (tkChannelID == 2) ? LL_DMA_CHANNEL_2
                                                  : (tkChannelID == 3) ? LL_DMA_CHANNEL_3
                                                  : (tkChannelID == 4) ? LL_DMA_CHANNEL_4
                                                  : (tkChannelID == 5) ? LL_DMA_CHANNEL_5
                                                  : (tkChannelID == 6) ? LL_DMA_CHANNEL_6
                                                  : (tkChannelID == 7) ? LL_DMA_CHANNEL_7
                                                                       : LL_DMA_CHANNEL_8;

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

    enum class DMADataWidth : uint32_t
    {
        kByte     = LL_DMA_PDATAALIGN_BYTE,
        kHalfWord = LL_DMA_PDATAALIGN_HALFWORD,
        kWord     = LL_DMA_PDATAALIGN_WORD
    };

    enum class DMAIncrementMode : uint32_t
    {
        kPeriphFixed     = LL_DMA_PERIPH_NOINCREMENT,
        kPeriphIncrement = LL_DMA_PERIPH_INCREMENT,
        kMemoryFixed     = LL_DMA_MEMORY_NOINCREMENT,
        kMemoryIncrement = LL_DMA_MEMORY_INCREMENT
    };

    enum class DMARequestID : uint32_t
    {
        kMem2Mem     = LL_DMAMUX_REQ_MEM2MEM,
        kGenerator0  = LL_DMAMUX_REQ_GENERATOR0,
        kGenerator1  = LL_DMAMUX_REQ_GENERATOR1,
        kGenerator2  = LL_DMAMUX_REQ_GENERATOR2,
        kGenerator3  = LL_DMAMUX_REQ_GENERATOR3,
        kADC1        = LL_DMAMUX_REQ_ADC1,
        kDAC1Ch1     = LL_DMAMUX_REQ_DAC1_CH1,
        kDAC1Ch2     = LL_DMAMUX_REQ_DAC1_CH2,
        kTIM6Up      = LL_DMAMUX_REQ_TIM6_UP,
        kTIM7Up      = LL_DMAMUX_REQ_TIM7_UP,
        kSPI1Rx      = LL_DMAMUX_REQ_SPI1_RX,
        kSPI1Tx      = LL_DMAMUX_REQ_SPI1_TX,
        kSPI2Rx      = LL_DMAMUX_REQ_SPI2_RX,
        kSPI2Tx      = LL_DMAMUX_REQ_SPI2_TX,
        kSPI3Rx      = LL_DMAMUX_REQ_SPI3_RX,
        kSPI3Tx      = LL_DMAMUX_REQ_SPI3_TX,
        kI2C1Rx      = LL_DMAMUX_REQ_I2C1_RX,
        kI2C1Tx      = LL_DMAMUX_REQ_I2C1_TX,
        kI2C2Rx      = LL_DMAMUX_REQ_I2C2_RX,
        kI2C2Tx      = LL_DMAMUX_REQ_I2C2_TX,
        kI2C3Rx      = LL_DMAMUX_REQ_I2C3_RX,
        kI2C3Tx      = LL_DMAMUX_REQ_I2C3_TX,
        kI2C4Rx      = LL_DMAMUX_REQ_I2C4_RX,
        kI2C4Tx      = LL_DMAMUX_REQ_I2C4_TX,
        kUSART1Rx    = LL_DMAMUX_REQ_USART1_RX,
        kUSART1Tx    = LL_DMAMUX_REQ_USART1_TX,
        kUSART2Rx    = LL_DMAMUX_REQ_USART2_RX,
        kUSART2Tx    = LL_DMAMUX_REQ_USART2_TX,
        kUSART3Rx    = LL_DMAMUX_REQ_USART3_RX,
        kUSART3Tx    = LL_DMAMUX_REQ_USART3_TX,
        kUART4Rx     = LL_DMAMUX_REQ_UART4_RX,
        kUART4Tx     = LL_DMAMUX_REQ_UART4_TX,
        kUART5Rx     = LL_DMAMUX_REQ_UART5_RX,
        kUART5Tx     = LL_DMAMUX_REQ_UART5_TX,
        kLPUART1Rx   = LL_DMAMUX_REQ_LPUART1_RX,
        kLPUART1Tx   = LL_DMAMUX_REQ_LPUART1_TX,
        kADC2        = LL_DMAMUX_REQ_ADC2,
        kADC3        = LL_DMAMUX_REQ_ADC3,
        kADC4        = LL_DMAMUX_REQ_ADC4,
        kADC5        = LL_DMAMUX_REQ_ADC5,
        kQSPI        = LL_DMAMUX_REQ_QSPI,
        kDAC2Ch1     = LL_DMAMUX_REQ_DAC2_CH1,
        kTIM1Ch1     = LL_DMAMUX_REQ_TIM1_CH1,
        kTIM1Ch2     = LL_DMAMUX_REQ_TIM1_CH2,
        kTIM1Ch3     = LL_DMAMUX_REQ_TIM1_CH3,
        kTIM1Ch4     = LL_DMAMUX_REQ_TIM1_CH4,
        kTIM1Up      = LL_DMAMUX_REQ_TIM1_UP,
        kTIM1Trig    = LL_DMAMUX_REQ_TIM1_TRIG,
        kTIM1Com     = LL_DMAMUX_REQ_TIM1_COM,
        kTIM8Ch1     = LL_DMAMUX_REQ_TIM8_CH1,
        kTIM8Ch2     = LL_DMAMUX_REQ_TIM8_CH2,
        kTIM8Ch3     = LL_DMAMUX_REQ_TIM8_CH3,
        kTIM8Ch4     = LL_DMAMUX_REQ_TIM8_CH4,
        kTIM8Up      = LL_DMAMUX_REQ_TIM8_UP,
        kTIM8Trig    = LL_DMAMUX_REQ_TIM8_TRIG,
        kTIM8Com     = LL_DMAMUX_REQ_TIM8_COM,
        kTIM2Ch1     = LL_DMAMUX_REQ_TIM2_CH1,
        kTIM2Ch2     = LL_DMAMUX_REQ_TIM2_CH2,
        kTIM2Ch3     = LL_DMAMUX_REQ_TIM2_CH3,
        kTIM2Ch4     = LL_DMAMUX_REQ_TIM2_CH4,
        kTIM2Up      = LL_DMAMUX_REQ_TIM2_UP,
        kTIM3Ch1     = LL_DMAMUX_REQ_TIM3_CH1,
        kTIM3Ch2     = LL_DMAMUX_REQ_TIM3_CH2,
        kTIM3Ch3     = LL_DMAMUX_REQ_TIM3_CH3,
        kTIM3Ch4     = LL_DMAMUX_REQ_TIM3_CH4,
        kTIM3Up      = LL_DMAMUX_REQ_TIM3_UP,
        kTIM3Trig    = LL_DMAMUX_REQ_TIM3_TRIG,
        kTIM4Ch1     = LL_DMAMUX_REQ_TIM4_CH1,
        kTIM4Ch2     = LL_DMAMUX_REQ_TIM4_CH2,
        kTIM4Ch3     = LL_DMAMUX_REQ_TIM4_CH3,
        kTIM4Ch4     = LL_DMAMUX_REQ_TIM4_CH4,
        kTIM4Up      = LL_DMAMUX_REQ_TIM4_UP,
        kTIM5Ch1     = LL_DMAMUX_REQ_TIM5_CH1,
        kTIM5Ch2     = LL_DMAMUX_REQ_TIM5_CH2,
        kTIM5Ch3     = LL_DMAMUX_REQ_TIM5_CH3,
        kTIM5Ch4     = LL_DMAMUX_REQ_TIM5_CH4,
        kTIM5Up      = LL_DMAMUX_REQ_TIM5_UP,
        kTIM5Trig    = LL_DMAMUX_REQ_TIM5_TRIG,
        kTIM15Ch1    = LL_DMAMUX_REQ_TIM15_CH1,
        kTIM15Up     = LL_DMAMUX_REQ_TIM15_UP,
        kTIM15Trig   = LL_DMAMUX_REQ_TIM15_TRIG,
        kTIM15Com    = LL_DMAMUX_REQ_TIM15_COM,
        kTIM16Ch1    = LL_DMAMUX_REQ_TIM16_CH1,
        kTIM16Up     = LL_DMAMUX_REQ_TIM16_UP,
        kTIM17Ch1    = LL_DMAMUX_REQ_TIM17_CH1,
        kTIM17Up     = LL_DMAMUX_REQ_TIM17_UP,
        kTIM20Ch1    = LL_DMAMUX_REQ_TIM20_CH1,
        kTIM20Ch2    = LL_DMAMUX_REQ_TIM20_CH2,
        kTIM20Ch3    = LL_DMAMUX_REQ_TIM20_CH3,
        kTIM20Ch4    = LL_DMAMUX_REQ_TIM20_CH4,
        kTIM20Up     = LL_DMAMUX_REQ_TIM20_UP,
        kAESIn       = LL_DMAMUX_REQ_AES_IN,
        kAESOut      = LL_DMAMUX_REQ_AES_OUT,
        kTIM20Trig   = LL_DMAMUX_REQ_TIM20_TRIG,
        kTIM20Com    = LL_DMAMUX_REQ_TIM20_COM,
        kHRTIM1M     = LL_DMAMUX_REQ_HRTIM1_M,
        kHRTIM1A     = LL_DMAMUX_REQ_HRTIM1_A,
        kHRTIM1B     = LL_DMAMUX_REQ_HRTIM1_B,
        kHRTIM1C     = LL_DMAMUX_REQ_HRTIM1_C,
        kHRTIM1D     = LL_DMAMUX_REQ_HRTIM1_D,
        kHRTIM1E     = LL_DMAMUX_REQ_HRTIM1_E,
        kHRTIM1F     = LL_DMAMUX_REQ_HRTIM1_F,
        kDAC3Ch1     = LL_DMAMUX_REQ_DAC3_CH1,
        kDAC3Ch2     = LL_DMAMUX_REQ_DAC3_CH2,
        kDAC4Ch1     = LL_DMAMUX_REQ_DAC4_CH1,
        kDAC4Ch2     = LL_DMAMUX_REQ_DAC4_CH2,
        kSPI4Rx      = LL_DMAMUX_REQ_SPI4_RX,
        kSPI4Tx      = LL_DMAMUX_REQ_SPI4_TX,
        kSAI1A       = LL_DMAMUX_REQ_SAI1_A,
        kSAI1B       = LL_DMAMUX_REQ_SAI1_B,
        kFMACRead    = LL_DMAMUX_REQ_FMAC_READ,
        kFMACWrite   = LL_DMAMUX_REQ_FMAC_WRITE,
        kCORDICRead  = LL_DMAMUX_REQ_CORDIC_READ,
        kCORDICWrite = LL_DMAMUX_REQ_CORDIC_WRITE,
        kUCPD1Rx     = LL_DMAMUX_REQ_UCPD1_RX,
        kUCPD1Tx     = LL_DMAMUX_REQ_UCPD1_TX,
    };

}  // namespace valle