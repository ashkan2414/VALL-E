#pragma once

#include "stm32g4xx_ll_bus.h"
#include "stm32g4xx_ll_dmamux.h"
#include "valle/platform/hardware/dmamux/id.hpp"

namespace valle::platform
{
    // ============================================================================
    // ENUMERATIONS
    // ============================================================================
    enum class DmaMuxChannel : uint32_t
    {
        kDma1Channel1 = LL_DMAMUX_CHANNEL_0,
        kDma1Channel2 = LL_DMAMUX_CHANNEL_1,
        kDma1Channel3 = LL_DMAMUX_CHANNEL_2,
        kDma1Channel4 = LL_DMAMUX_CHANNEL_3,
        kDma1Channel5 = LL_DMAMUX_CHANNEL_4,
        kDma1Channel6 = LL_DMAMUX_CHANNEL_5,
        kDma1Channel7 = LL_DMAMUX_CHANNEL_6,
        kDma1Channel8 = LL_DMAMUX_CHANNEL_7,
        kDma2Channel1 = LL_DMAMUX_CHANNEL_8,
        kDma2Channel2 = LL_DMAMUX_CHANNEL_9,
        kDma2Channel3 = LL_DMAMUX_CHANNEL_10,
        kDma2Channel4 = LL_DMAMUX_CHANNEL_11,
        kDma2Channel5 = LL_DMAMUX_CHANNEL_12,
        kDma2Channel6 = LL_DMAMUX_CHANNEL_13,
        kDma2Channel7 = LL_DMAMUX_CHANNEL_14,
        kDma2Channel8 = LL_DMAMUX_CHANNEL_15
    };

    enum class DmaMuxRequestId : uint32_t
    {
        kMem2Mem     = LL_DMAMUX_REQ_MEM2MEM,
        kGenerator0  = LL_DMAMUX_REQ_GENERATOR0,
        kGenerator1  = LL_DMAMUX_REQ_GENERATOR1,
        kGenerator2  = LL_DMAMUX_REQ_GENERATOR2,
        kGenerator3  = LL_DMAMUX_REQ_GENERATOR3,
        kAdc1        = LL_DMAMUX_REQ_ADC1,
        kDAC1Ch1     = LL_DMAMUX_REQ_DAC1_CH1,
        kDAC1Ch2     = LL_DMAMUX_REQ_DAC1_CH2,
        kTim6Up      = LL_DMAMUX_REQ_TIM6_UP,
        kTim7Up      = LL_DMAMUX_REQ_TIM7_UP,
        kSpi1Rx      = LL_DMAMUX_REQ_SPI1_RX,
        kSpi1Tx      = LL_DMAMUX_REQ_SPI1_TX,
        kSpi2Rx      = LL_DMAMUX_REQ_SPI2_RX,
        kSpi2Tx      = LL_DMAMUX_REQ_SPI2_TX,
        kSpi3Rx      = LL_DMAMUX_REQ_SPI3_RX,
        kSpi3Tx      = LL_DMAMUX_REQ_SPI3_TX,
        kI2c1Rx      = LL_DMAMUX_REQ_I2C1_RX,
        kI2c1Tx      = LL_DMAMUX_REQ_I2C1_TX,
        kI2c2Rx      = LL_DMAMUX_REQ_I2C2_RX,
        kI2c2Tx      = LL_DMAMUX_REQ_I2C2_TX,
        kI2c3Rx      = LL_DMAMUX_REQ_I2C3_RX,
        kI2c3Tx      = LL_DMAMUX_REQ_I2C3_TX,
        kI2c4Rx      = LL_DMAMUX_REQ_I2C4_RX,
        kI2c4Tx      = LL_DMAMUX_REQ_I2C4_TX,
        kUSART1Rx    = LL_DMAMUX_REQ_USART1_RX,
        kUSART1Tx    = LL_DMAMUX_REQ_USART1_TX,
        kUSART2Rx    = LL_DMAMUX_REQ_USART2_RX,
        kUSART2Tx    = LL_DMAMUX_REQ_USART2_TX,
        kUSART3Rx    = LL_DMAMUX_REQ_USART3_RX,
        kUSART3Tx    = LL_DMAMUX_REQ_USART3_TX,
        kUart4Rx     = LL_DMAMUX_REQ_UART4_RX,
        kUart4Tx     = LL_DMAMUX_REQ_UART4_TX,
        kUart5Rx     = LL_DMAMUX_REQ_UART5_RX,
        kUart5Tx     = LL_DMAMUX_REQ_UART5_TX,
        kLPUart1Rx   = LL_DMAMUX_REQ_LPUart1_RX,
        kLPUart1Tx   = LL_DMAMUX_REQ_LPUart1_TX,
        kAdc2        = LL_DMAMUX_REQ_ADC2,
        kAdc3        = LL_DMAMUX_REQ_ADC3,
        kAdc4        = LL_DMAMUX_REQ_ADC4,
        kAdc5        = LL_DMAMUX_REQ_ADC5,
        kQSPI        = LL_DMAMUX_REQ_QSPI,
        kDAC2Ch1     = LL_DMAMUX_REQ_DAC2_CH1,
        kTim1Ch1     = LL_DMAMUX_REQ_TIM1_CH1,
        kTim1Ch2     = LL_DMAMUX_REQ_TIM1_CH2,
        kTim1Ch3     = LL_DMAMUX_REQ_TIM1_CH3,
        kTim1Ch4     = LL_DMAMUX_REQ_TIM1_CH4,
        kTim1Up      = LL_DMAMUX_REQ_TIM1_UP,
        kTim1Trig    = LL_DMAMUX_REQ_TIM1_TRIG,
        kTim1Com     = LL_DMAMUX_REQ_TIM1_COM,
        kTim8Ch1     = LL_DMAMUX_REQ_TIM8_CH1,
        kTim8Ch2     = LL_DMAMUX_REQ_TIM8_CH2,
        kTim8Ch3     = LL_DMAMUX_REQ_TIM8_CH3,
        kTim8Ch4     = LL_DMAMUX_REQ_TIM8_CH4,
        kTim8Up      = LL_DMAMUX_REQ_TIM8_UP,
        kTim8Trig    = LL_DMAMUX_REQ_TIM8_TRIG,
        kTim8Com     = LL_DMAMUX_REQ_TIM8_COM,
        kTim2Ch1     = LL_DMAMUX_REQ_TIM2_CH1,
        kTim2Ch2     = LL_DMAMUX_REQ_TIM2_CH2,
        kTim2Ch3     = LL_DMAMUX_REQ_TIM2_CH3,
        kTim2Ch4     = LL_DMAMUX_REQ_TIM2_CH4,
        kTim2Up      = LL_DMAMUX_REQ_TIM2_UP,
        kTim3Ch1     = LL_DMAMUX_REQ_TIM3_CH1,
        kTim3Ch2     = LL_DMAMUX_REQ_TIM3_CH2,
        kTim3Ch3     = LL_DMAMUX_REQ_TIM3_CH3,
        kTim3Ch4     = LL_DMAMUX_REQ_TIM3_CH4,
        kTim3Up      = LL_DMAMUX_REQ_TIM3_UP,
        kTim3Trig    = LL_DMAMUX_REQ_TIM3_TRIG,
        kTim4Ch1     = LL_DMAMUX_REQ_TIM4_CH1,
        kTim4Ch2     = LL_DMAMUX_REQ_TIM4_CH2,
        kTim4Ch3     = LL_DMAMUX_REQ_TIM4_CH3,
        kTim4Ch4     = LL_DMAMUX_REQ_TIM4_CH4,
        kTim4Up      = LL_DMAMUX_REQ_TIM4_UP,
        kTim5Ch1     = LL_DMAMUX_REQ_TIM5_CH1,
        kTim5Ch2     = LL_DMAMUX_REQ_TIM5_CH2,
        kTim5Ch3     = LL_DMAMUX_REQ_TIM5_CH3,
        kTim5Ch4     = LL_DMAMUX_REQ_TIM5_CH4,
        kTim5Up      = LL_DMAMUX_REQ_TIM5_UP,
        kTim5Trig    = LL_DMAMUX_REQ_TIM5_TRIG,
        kTim15Ch1    = LL_DMAMUX_REQ_TIM15_CH1,
        kTim15Up     = LL_DMAMUX_REQ_TIM15_UP,
        kTim15Trig   = LL_DMAMUX_REQ_TIM15_TRIG,
        kTim15Com    = LL_DMAMUX_REQ_TIM15_COM,
        kTim16Ch1    = LL_DMAMUX_REQ_TIM16_CH1,
        kTim16Up     = LL_DMAMUX_REQ_TIM16_UP,
        kTim17Ch1    = LL_DMAMUX_REQ_TIM17_CH1,
        kTim17Up     = LL_DMAMUX_REQ_TIM17_UP,
        kTim20Ch1    = LL_DMAMUX_REQ_TIM20_CH1,
        kTim20Ch2    = LL_DMAMUX_REQ_TIM20_CH2,
        kTim20Ch3    = LL_DMAMUX_REQ_TIM20_CH3,
        kTim20Ch4    = LL_DMAMUX_REQ_TIM20_CH4,
        kTim20Up     = LL_DMAMUX_REQ_TIM20_UP,
        kAESIn       = LL_DMAMUX_REQ_AES_IN,
        kAESOut      = LL_DMAMUX_REQ_AES_OUT,
        kTim20Trig   = LL_DMAMUX_REQ_TIM20_TRIG,
        kTim20Com    = LL_DMAMUX_REQ_TIM20_COM,
        kHrtim1M     = LL_DMAMUX_REQ_HRTIM1_M,
        kHrtim1A     = LL_DMAMUX_REQ_HRTIM1_A,
        kHrtim1B     = LL_DMAMUX_REQ_HRTIM1_B,
        kHrtim1C     = LL_DMAMUX_REQ_HRTIM1_C,
        kHrtim1D     = LL_DMAMUX_REQ_HRTIM1_D,
        kHrtim1E     = LL_DMAMUX_REQ_HRTIM1_E,
        kHrtim1F     = LL_DMAMUX_REQ_HRTIM1_F,
        kDAC3Ch1     = LL_DMAMUX_REQ_DAC3_CH1,
        kDAC3Ch2     = LL_DMAMUX_REQ_DAC3_CH2,
        kDAC4Ch1     = LL_DMAMUX_REQ_DAC4_CH1,
        kDAC4Ch2     = LL_DMAMUX_REQ_DAC4_CH2,
        kSpi4Rx      = LL_DMAMUX_REQ_SPI4_RX,
        kSpi4Tx      = LL_DMAMUX_REQ_SPI4_TX,
        kSAI1A       = LL_DMAMUX_REQ_SAI1_A,
        kSAI1B       = LL_DMAMUX_REQ_SAI1_B,
        kFMACRead    = LL_DMAMUX_REQ_FMAC_READ,
        kFMACWrite   = LL_DMAMUX_REQ_FMAC_WRITE,
        kCORDICRead  = LL_DMAMUX_REQ_CORDIC_READ,
        kCORDICWrite = LL_DMAMUX_REQ_CORDIC_WRITE,
        kUCPD1Rx     = LL_DMAMUX_REQ_UCPD1_RX,
        kUCPD1Tx     = LL_DMAMUX_REQ_UCPD1_TX,
    };

    // ============================================================================
    // HARDWARE TRAITS
    // ============================================================================
    template <DmaMuxPeripheralId tkPeripheralId>
    struct DmaMuxControllerTraits;

    template <>
    struct DmaMuxControllerTraits<DmaMuxPeripheralId::kDmaMux1>
    {
        static inline DmaMUX_Channel_TypeDef* const skInstance = DmaMUX1;
        static constexpr uint32_t                   skClock    = LL_AHB1_GRP1_PERIPH_DMAMUX1;

        static inline void enable_clock()
        {
            LL_AHB1_GRP1_EnableClock(skClock);
        }
    };

}  // namespace valle::platform