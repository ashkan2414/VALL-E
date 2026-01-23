#pragma once

#include "stm32g4xx_ll_bus.h"
#include "stm32g4xx_ll_dma.h"
#include "stm32g4xx_ll_dmamux.h"

// ============================================================================
// CONSTANTS & CHECKS
// ============================================================================

template <uint8_t tkDmaIdx>
constexpr bool kValidDMAIndex = (tkDmaIdx == 1 || tkDmaIdx == 2);

template <uint8_t tkChannelIdx>
constexpr bool kValidDMAChannel = (tkChannelIdx >= 1 && tkChannelIdx <= 8);

// ============================================================================
// HARDWARE TRAITS
// ============================================================================

template <uint8_t tkDmaIdx>
struct DMATraits;

template <>
struct DMATraits<1>
{
    static inline DMA_TypeDef* skInstance = DMA1;
    static constexpr uint32_t  skClock    = LL_AHB1_GRP1_PERIPH_DMA1;
};

template <>
struct DMATraits<2>
{
    static inline DMA_TypeDef* skInstance = DMA2;
    static constexpr uint32_t  skClock    = LL_AHB1_GRP1_PERIPH_DMA2;
};

template <uint8_t tkDmaIdx, uint8_t tkChannelIdx>
struct DMAChannelTraits
{
private:
    // Helper to get IRQn
    static constexpr IRQn_Type get_irq_n()
    {
        if constexpr (tkDmaIdx == 1)
        {
            if constexpr (tkChannelIdx == 1) return DMA1_Channel1_IRQn;
            if constexpr (tkChannelIdx == 2) return DMA1_Channel2_IRQn;
            if constexpr (tkChannelIdx == 3) return DMA1_Channel3_IRQn;
            if constexpr (tkChannelIdx == 4) return DMA1_Channel4_IRQn;
            if constexpr (tkChannelIdx == 5) return DMA1_Channel5_IRQn;
            if constexpr (tkChannelIdx == 6) return DMA1_Channel6_IRQn;
            if constexpr (tkChannelIdx == 7) return DMA1_Channel7_IRQn;
            return DMA1_Channel8_IRQn;
        }
        else
        {
            if constexpr (tkChannelIdx == 1) return DMA2_Channel1_IRQn;
            if constexpr (tkChannelIdx == 2) return DMA2_Channel2_IRQn;
            if constexpr (tkChannelIdx == 3) return DMA2_Channel3_IRQn;
            if constexpr (tkChannelIdx == 4) return DMA2_Channel4_IRQn;
            if constexpr (tkChannelIdx == 5) return DMA2_Channel5_IRQn;
            if constexpr (tkChannelIdx == 6) return DMA2_Channel6_IRQn;
            if constexpr (tkChannelIdx == 7) return DMA2_Channel7_IRQn;
            return DMA2_Channel8_IRQn;
        }
    }

public:
    // Helper to map index (1-8) to LL Constant (LL_DMA_CHANNEL_x)
    static constexpr uint32_t skChannelId = (tkChannelIdx == 1)   ? LL_DMA_CHANNEL_1
                                            : (tkChannelIdx == 2) ? LL_DMA_CHANNEL_2
                                            : (tkChannelIdx == 3) ? LL_DMA_CHANNEL_3
                                            : (tkChannelIdx == 4) ? LL_DMA_CHANNEL_4
                                            : (tkChannelIdx == 5) ? LL_DMA_CHANNEL_5
                                            : (tkChannelIdx == 6) ? LL_DMA_CHANNEL_6
                                            : (tkChannelIdx == 7) ? LL_DMA_CHANNEL_7
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

enum class DMARequestId : uint32_t
{
    kMem2Mem      = LL_DMAMUX_REQ_MEM2MEM,
    kGenerator0   = LL_DMAMUX_REQ_GENERATOR0,
    kGenerator1   = LL_DMAMUX_REQ_GENERATOR1,
    kGenerator2   = LL_DMAMUX_REQ_GENERATOR2,
    kGenerator3   = LL_DMAMUX_REQ_GENERATOR3,
    kADC1         = LL_DMAMUX_REQ_ADC1,
    kDAC1_CH1     = LL_DMAMUX_REQ_DAC1_CH1,
    kDAC1_CH2     = LL_DMAMUX_REQ_DAC1_CH2,
    kTIM6_UP      = LL_DMAMUX_REQ_TIM6_UP,
    kTIM7_UP      = LL_DMAMUX_REQ_TIM7_UP,
    kSPI1_RX      = LL_DMAMUX_REQ_SPI1_RX,
    kSPI1_TX      = LL_DMAMUX_REQ_SPI1_TX,
    kSPI2_RX      = LL_DMAMUX_REQ_SPI2_RX,
    kSPI2_TX      = LL_DMAMUX_REQ_SPI2_TX,
    kSPI3_RX      = LL_DMAMUX_REQ_SPI3_RX,
    kSPI3_TX      = LL_DMAMUX_REQ_SPI3_TX,
    kI2C1_RX      = LL_DMAMUX_REQ_I2C1_RX,
    kI2C1_TX      = LL_DMAMUX_REQ_I2C1_TX,
    kI2C2_RX      = LL_DMAMUX_REQ_I2C2_RX,
    kI2C2_TX      = LL_DMAMUX_REQ_I2C2_TX,
    kI2C3_RX      = LL_DMAMUX_REQ_I2C3_RX,
    kI2C3_TX      = LL_DMAMUX_REQ_I2C3_TX,
    kI2C4_RX      = LL_DMAMUX_REQ_I2C4_RX,
    kI2C4_TX      = LL_DMAMUX_REQ_I2C4_TX,
    kUSART1_RX    = LL_DMAMUX_REQ_USART1_RX,
    kUSART1_TX    = LL_DMAMUX_REQ_USART1_TX,
    kUSART2_RX    = LL_DMAMUX_REQ_USART2_RX,
    kUSART2_TX    = LL_DMAMUX_REQ_USART2_TX,
    kUSART3_RX    = LL_DMAMUX_REQ_USART3_RX,
    kUSART3_TX    = LL_DMAMUX_REQ_USART3_TX,
    kUART4_RX     = LL_DMAMUX_REQ_UART4_RX,
    kUART4_TX     = LL_DMAMUX_REQ_UART4_TX,
    kUART5_RX     = LL_DMAMUX_REQ_UART5_RX,
    kUART5_TX     = LL_DMAMUX_REQ_UART5_TX,
    kLPUART1_RX   = LL_DMAMUX_REQ_LPUART1_RX,
    kLPUART1_TX   = LL_DMAMUX_REQ_LPUART1_TX,
    kADC2         = LL_DMAMUX_REQ_ADC2,
    kADC3         = LL_DMAMUX_REQ_ADC3,
    kADC4         = LL_DMAMUX_REQ_ADC4,
    kADC5         = LL_DMAMUX_REQ_ADC5,
    kQSPI         = LL_DMAMUX_REQ_QSPI,
    kDAC2_CH1     = LL_DMAMUX_REQ_DAC2_CH1,
    kTIM1_CH1     = LL_DMAMUX_REQ_TIM1_CH1,
    kTIM1_CH2     = LL_DMAMUX_REQ_TIM1_CH2,
    kTIM1_CH3     = LL_DMAMUX_REQ_TIM1_CH3,
    kTIM1_CH4     = LL_DMAMUX_REQ_TIM1_CH4,
    kTIM1_UP      = LL_DMAMUX_REQ_TIM1_UP,
    kTIM1_TRIG    = LL_DMAMUX_REQ_TIM1_TRIG,
    kTIM1_COM     = LL_DMAMUX_REQ_TIM1_COM,
    kTIM8_CH1     = LL_DMAMUX_REQ_TIM8_CH1,
    kTIM8_CH2     = LL_DMAMUX_REQ_TIM8_CH2,
    kTIM8_CH3     = LL_DMAMUX_REQ_TIM8_CH3,
    kTIM8_CH4     = LL_DMAMUX_REQ_TIM8_CH4,
    kTIM8_UP      = LL_DMAMUX_REQ_TIM8_UP,
    kTIM8_TRIG    = LL_DMAMUX_REQ_TIM8_TRIG,
    kTIM8_COM     = LL_DMAMUX_REQ_TIM8_COM,
    kTIM2_CH1     = LL_DMAMUX_REQ_TIM2_CH1,
    kTIM2_CH2     = LL_DMAMUX_REQ_TIM2_CH2,
    kTIM2_CH3     = LL_DMAMUX_REQ_TIM2_CH3,
    kTIM2_CH4     = LL_DMAMUX_REQ_TIM2_CH4,
    kTIM2_UP      = LL_DMAMUX_REQ_TIM2_UP,
    kTIM3_CH1     = LL_DMAMUX_REQ_TIM3_CH1,
    kTIM3_CH2     = LL_DMAMUX_REQ_TIM3_CH2,
    kTIM3_CH3     = LL_DMAMUX_REQ_TIM3_CH3,
    kTIM3_CH4     = LL_DMAMUX_REQ_TIM3_CH4,
    kTIM3_UP      = LL_DMAMUX_REQ_TIM3_UP,
    kTIM3_TRIG    = LL_DMAMUX_REQ_TIM3_TRIG,
    kTIM4_CH1     = LL_DMAMUX_REQ_TIM4_CH1,
    kTIM4_CH2     = LL_DMAMUX_REQ_TIM4_CH2,
    kTIM4_CH3     = LL_DMAMUX_REQ_TIM4_CH3,
    kTIM4_CH4     = LL_DMAMUX_REQ_TIM4_CH4,
    kTIM4_UP      = LL_DMAMUX_REQ_TIM4_UP,
    kTIM5_CH1     = LL_DMAMUX_REQ_TIM5_CH1,
    kTIM5_CH2     = LL_DMAMUX_REQ_TIM5_CH2,
    kTIM5_CH3     = LL_DMAMUX_REQ_TIM5_CH3,
    kTIM5_CH4     = LL_DMAMUX_REQ_TIM5_CH4,
    kTIM5_UP      = LL_DMAMUX_REQ_TIM5_UP,
    kTIM5_TRIG    = LL_DMAMUX_REQ_TIM5_TRIG,
    kTIM15_CH1    = LL_DMAMUX_REQ_TIM15_CH1,
    kTIM15_UP     = LL_DMAMUX_REQ_TIM15_UP,
    kTIM15_TRIG   = LL_DMAMUX_REQ_TIM15_TRIG,
    kTIM15_COM    = LL_DMAMUX_REQ_TIM15_COM,
    kTIM16_CH1    = LL_DMAMUX_REQ_TIM16_CH1,
    kTIM16_UP     = LL_DMAMUX_REQ_TIM16_UP,
    kTIM17_CH1    = LL_DMAMUX_REQ_TIM17_CH1,
    kTIM17_UP     = LL_DMAMUX_REQ_TIM17_UP,
    kTIM20_CH1    = LL_DMAMUX_REQ_TIM20_CH1,
    kTIM20_CH2    = LL_DMAMUX_REQ_TIM20_CH2,
    kTIM20_CH3    = LL_DMAMUX_REQ_TIM20_CH3,
    kTIM20_CH4    = LL_DMAMUX_REQ_TIM20_CH4,
    kTIM20_UP     = LL_DMAMUX_REQ_TIM20_UP,
    kAES_IN       = LL_DMAMUX_REQ_AES_IN,
    kAES_OUT      = LL_DMAMUX_REQ_AES_OUT,
    kTIM20_TRIG   = LL_DMAMUX_REQ_TIM20_TRIG,
    kTIM20_COM    = LL_DMAMUX_REQ_TIM20_COM,
    kHRTIM1_M     = LL_DMAMUX_REQ_HRTIM1_M,
    kHRTIM1_A     = LL_DMAMUX_REQ_HRTIM1_A,
    kHRTIM1_B     = LL_DMAMUX_REQ_HRTIM1_B,
    kHRTIM1_C     = LL_DMAMUX_REQ_HRTIM1_C,
    kHRTIM1_D     = LL_DMAMUX_REQ_HRTIM1_D,
    kHRTIM1_E     = LL_DMAMUX_REQ_HRTIM1_E,
    kHRTIM1_F     = LL_DMAMUX_REQ_HRTIM1_F,
    kDAC3_CH1     = LL_DMAMUX_REQ_DAC3_CH1,
    kDAC3_CH2     = LL_DMAMUX_REQ_DAC3_CH2,
    kDAC4_CH1     = LL_DMAMUX_REQ_DAC4_CH1,
    kDAC4_CH2     = LL_DMAMUX_REQ_DAC4_CH2,
    kSPI4_RX      = LL_DMAMUX_REQ_SPI4_RX,
    kSPI4_TX      = LL_DMAMUX_REQ_SPI4_TX,
    kSAI1_A       = LL_DMAMUX_REQ_SAI1_A,
    kSAI1_B       = LL_DMAMUX_REQ_SAI1_B,
    kFMAC_READ    = LL_DMAMUX_REQ_FMAC_READ,
    kFMAC_WRITE   = LL_DMAMUX_REQ_FMAC_WRITE,
    kCORDIC_READ  = LL_DMAMUX_REQ_CORDIC_READ,
    kCORDIC_WRITE = LL_DMAMUX_REQ_CORDIC_WRITE,
    kUCPD1_RX     = LL_DMAMUX_REQ_UCPD1_RX,
    kUCPD1_TX     = LL_DMAMUX_REQ_UCPD1_TX,
};
