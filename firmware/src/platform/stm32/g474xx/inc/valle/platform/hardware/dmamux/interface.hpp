#pragma once

#include "stm32g4xx_ll_bus.h"
#include "stm32g4xx_ll_dmamux.h"
#include "valle/platform/hardware/dmamux/id.hpp"

namespace valle::platform
{
    // ============================================================================
    // ENUMERATIONS
    // ============================================================================
    struct DmaMuxRequestId : public LLDriverEnumValue<DmaMuxRequestId>
    {
        static const DmaMuxRequestId kMem2Mem;
        static const DmaMuxRequestId kGenerator0;
        static const DmaMuxRequestId kGenerator1;
        static const DmaMuxRequestId kGenerator2;
        static const DmaMuxRequestId kGenerator3;
        static const DmaMuxRequestId kAdc1;
        static const DmaMuxRequestId kDAC1Ch1;
        static const DmaMuxRequestId kDAC1Ch2;
        static const DmaMuxRequestId kTim6Up;
        static const DmaMuxRequestId kTim7Up;
        static const DmaMuxRequestId kSpi1Rx;
        static const DmaMuxRequestId kSpi1Tx;
        static const DmaMuxRequestId kSpi2Rx;
        static const DmaMuxRequestId kSpi2Tx;
        static const DmaMuxRequestId kSpi3Rx;
        static const DmaMuxRequestId kSpi3Tx;
        static const DmaMuxRequestId kI2c1Rx;
        static const DmaMuxRequestId kI2c1Tx;
        static const DmaMuxRequestId kI2c2Rx;
        static const DmaMuxRequestId kI2c2Tx;
        static const DmaMuxRequestId kI2c3Rx;
        static const DmaMuxRequestId kI2c3Tx;
        static const DmaMuxRequestId kI2c4Rx;
        static const DmaMuxRequestId kI2c4Tx;
        static const DmaMuxRequestId kUSART1Rx;
        static const DmaMuxRequestId kUSART1Tx;
        static const DmaMuxRequestId kUSART2Rx;
        static const DmaMuxRequestId kUSART2Tx;
        static const DmaMuxRequestId kUSART3Rx;
        static const DmaMuxRequestId kUSART3Tx;
        static const DmaMuxRequestId kUart4Rx;
        static const DmaMuxRequestId kUart4Tx;
        static const DmaMuxRequestId kUart5Rx;
        static const DmaMuxRequestId kUart5Tx;
        static const DmaMuxRequestId kLPUart1Rx;
        static const DmaMuxRequestId kLPUart1Tx;
        static const DmaMuxRequestId kAdc2;
        static const DmaMuxRequestId kAdc3;
        static const DmaMuxRequestId kAdc4;
        static const DmaMuxRequestId kAdc5;
        static const DmaMuxRequestId kQSPI;
        static const DmaMuxRequestId kDAC2Ch1;
        static const DmaMuxRequestId kTim1Ch1;
        static const DmaMuxRequestId kTim1Ch2;
        static const DmaMuxRequestId kTim1Ch3;
        static const DmaMuxRequestId kTim1Ch4;
        static const DmaMuxRequestId kTim1Up;
        static const DmaMuxRequestId kTim1Trig;
        static const DmaMuxRequestId kTim1Com;
        static const DmaMuxRequestId kTim8Ch1;
        static const DmaMuxRequestId kTim8Ch2;
        static const DmaMuxRequestId kTim8Ch3;
        static const DmaMuxRequestId kTim8Ch4;
        static const DmaMuxRequestId kTim8Up;
        static const DmaMuxRequestId kTim8Trig;
        static const DmaMuxRequestId kTim8Com;
        static const DmaMuxRequestId kTim2Ch1;
        static const DmaMuxRequestId kTim2Ch2;
        static const DmaMuxRequestId kTim2Ch3;
        static const DmaMuxRequestId kTim2Ch4;
        static const DmaMuxRequestId kTim2Up;
        static const DmaMuxRequestId kTim3Ch1;
        static const DmaMuxRequestId kTim3Ch2;
        static const DmaMuxRequestId kTim3Ch3;
        static const DmaMuxRequestId kTim3Ch4;
        static const DmaMuxRequestId kTim3Up;
        static const DmaMuxRequestId kTim3Trig;
        static const DmaMuxRequestId kTim4Ch1;
        static const DmaMuxRequestId kTim4Ch2;
        static const DmaMuxRequestId kTim4Ch3;
        static const DmaMuxRequestId kTim4Ch4;
        static const DmaMuxRequestId kTim4Up;
        static const DmaMuxRequestId kTim5Ch1;
        static const DmaMuxRequestId kTim5Ch2;
        static const DmaMuxRequestId kTim5Ch3;
        static const DmaMuxRequestId kTim5Ch4;
        static const DmaMuxRequestId kTim5Up;
        static const DmaMuxRequestId kTim5Trig;
        static const DmaMuxRequestId kTim15Ch1;
        static const DmaMuxRequestId kTim15Up;
        static const DmaMuxRequestId kTim15Trig;
        static const DmaMuxRequestId kTim15Com;
        static const DmaMuxRequestId kTim16Ch1;
        static const DmaMuxRequestId kTim16Up;
        static const DmaMuxRequestId kTim17Ch1;
        static const DmaMuxRequestId kTim17Up;
        static const DmaMuxRequestId kTim20Ch1;
        static const DmaMuxRequestId kTim20Ch2;
        static const DmaMuxRequestId kTim20Ch3;
        static const DmaMuxRequestId kTim20Ch4;
        static const DmaMuxRequestId kTim20Up;
        static const DmaMuxRequestId kAESIn;
        static const DmaMuxRequestId kAESOut;
        static const DmaMuxRequestId kTim20Trig;
        static const DmaMuxRequestId kTim20Com;
        static const DmaMuxRequestId kHrtim1M;
        static const DmaMuxRequestId kHrtim1A;
        static const DmaMuxRequestId kHrtim1B;
        static const DmaMuxRequestId kHrtim1C;
        static const DmaMuxRequestId kHrtim1D;
        static const DmaMuxRequestId kHrtim1E;
        static const DmaMuxRequestId kHrtim1F;
        static const DmaMuxRequestId kDAC3Ch1;
        static const DmaMuxRequestId kDAC3Ch2;
        static const DmaMuxRequestId kDAC4Ch1;
        static const DmaMuxRequestId kDAC4Ch2;
        static const DmaMuxRequestId kSpi4Rx;
        static const DmaMuxRequestId kSpi4Tx;
        static const DmaMuxRequestId kSAI1A;
        static const DmaMuxRequestId kSAI1B;
        static const DmaMuxRequestId kFMACRead;
        static const DmaMuxRequestId kFMACWrite;
        static const DmaMuxRequestId kCORDICRead;
        static const DmaMuxRequestId kCORDICWrite;
        static const DmaMuxRequestId kUCPD1Rx;
        static const DmaMuxRequestId kUCPD1Tx;
    };

    inline constexpr DmaMuxRequestId DmaMuxRequestId::kMem2Mem = DmaMuxRequestId::from_ll<LL_DMAMUX_REQ_MEM2MEM>();
    inline constexpr DmaMuxRequestId DmaMuxRequestId::kGenerator0 = DmaMuxRequestId::from_ll<LL_DMAMUX_REQ_GENERATOR0>();
    inline constexpr DmaMuxRequestId DmaMuxRequestId::kGenerator1 = DmaMuxRequestId::from_ll<LL_DMAMUX_REQ_GENERATOR1>();
    inline constexpr DmaMuxRequestId DmaMuxRequestId::kGenerator2 = DmaMuxRequestId::from_ll<LL_DMAMUX_REQ_GENERATOR2>();
    inline constexpr DmaMuxRequestId DmaMuxRequestId::kGenerator3 = DmaMuxRequestId::from_ll<LL_DMAMUX_REQ_GENERATOR3>();
    inline constexpr DmaMuxRequestId DmaMuxRequestId::kAdc1 = DmaMuxRequestId::from_ll<LL_DMAMUX_REQ_ADC1>();
    inline constexpr DmaMuxRequestId DmaMuxRequestId::kDAC1Ch1 = DmaMuxRequestId::from_ll<LL_DMAMUX_REQ_DAC1_CH1>();
    inline constexpr DmaMuxRequestId DmaMuxRequestId::kDAC1Ch2 = DmaMuxRequestId::from_ll<LL_DMAMUX_REQ_DAC1_CH2>();
    inline constexpr DmaMuxRequestId DmaMuxRequestId::kTim6Up = DmaMuxRequestId::from_ll<LL_DMAMUX_REQ_TIM6_UP>();
    inline constexpr DmaMuxRequestId DmaMuxRequestId::kTim7Up = DmaMuxRequestId::from_ll<LL_DMAMUX_REQ_TIM7_UP>();
    inline constexpr DmaMuxRequestId DmaMuxRequestId::kSpi1Rx = DmaMuxRequestId::from_ll<LL_DMAMUX_REQ_SPI1_RX>();
    inline constexpr DmaMuxRequestId DmaMuxRequestId::kSpi1Tx = DmaMuxRequestId::from_ll<LL_DMAMUX_REQ_SPI1_TX>();
    inline constexpr DmaMuxRequestId DmaMuxRequestId::kSpi2Rx = DmaMuxRequestId::from_ll<LL_DMAMUX_REQ_SPI2_RX>();
    inline constexpr DmaMuxRequestId DmaMuxRequestId::kSpi2Tx = DmaMuxRequestId::from_ll<LL_DMAMUX_REQ_SPI2_TX>();
    inline constexpr DmaMuxRequestId DmaMuxRequestId::kSpi3Rx = DmaMuxRequestId::from_ll<LL_DMAMUX_REQ_SPI3_RX>();
    inline constexpr DmaMuxRequestId DmaMuxRequestId::kSpi3Tx = DmaMuxRequestId::from_ll<LL_DMAMUX_REQ_SPI3_TX>();
    inline constexpr DmaMuxRequestId DmaMuxRequestId::kI2c1Rx = DmaMuxRequestId::from_ll<LL_DMAMUX_REQ_I2C1_RX>();
    inline constexpr DmaMuxRequestId DmaMuxRequestId::kI2c1Tx = DmaMuxRequestId::from_ll<LL_DMAMUX_REQ_I2C1_TX>();
    inline constexpr DmaMuxRequestId DmaMuxRequestId::kI2c2Rx = DmaMuxRequestId::from_ll<LL_DMAMUX_REQ_I2C2_RX>();
    inline constexpr DmaMuxRequestId DmaMuxRequestId::kI2c2Tx = DmaMuxRequestId::from_ll<LL_DMAMUX_REQ_I2C2_TX>();
    inline constexpr DmaMuxRequestId DmaMuxRequestId::kI2c3Rx = DmaMuxRequestId::from_ll<LL_DMAMUX_REQ_I2C3_RX>();
    inline constexpr DmaMuxRequestId DmaMuxRequestId::kI2c3Tx = DmaMuxRequestId::from_ll<LL_DMAMUX_REQ_I2C3_TX>();
    inline constexpr DmaMuxRequestId DmaMuxRequestId::kI2c4Rx = DmaMuxRequestId::from_ll<LL_DMAMUX_REQ_I2C4_RX>();
    inline constexpr DmaMuxRequestId DmaMuxRequestId::kI2c4Tx = DmaMuxRequestId::from_ll<LL_DMAMUX_REQ_I2C4_TX>();
    inline constexpr DmaMuxRequestId DmaMuxRequestId::kUSART1Rx = DmaMuxRequestId::from_ll<LL_DMAMUX_REQ_USART1_RX>();
    inline constexpr DmaMuxRequestId DmaMuxRequestId::kUSART1Tx = DmaMuxRequestId::from_ll<LL_DMAMUX_REQ_USART1_TX>();
    inline constexpr DmaMuxRequestId DmaMuxRequestId::kUSART2Rx = DmaMuxRequestId::from_ll<LL_DMAMUX_REQ_USART2_RX>();
    inline constexpr DmaMuxRequestId DmaMuxRequestId::kUSART2Tx = DmaMuxRequestId::from_ll<LL_DMAMUX_REQ_USART2_TX>();
    inline constexpr DmaMuxRequestId DmaMuxRequestId::kUSART3Rx = DmaMuxRequestId::from_ll<LL_DMAMUX_REQ_USART3_RX>();
    inline constexpr DmaMuxRequestId DmaMuxRequestId::kUSART3Tx = DmaMuxRequestId::from_ll<LL_DMAMUX_REQ_USART3_TX>();
    inline constexpr DmaMuxRequestId DmaMuxRequestId::kUart4Rx = DmaMuxRequestId::from_ll<LL_DMAMUX_REQ_UART4_RX>();
    inline constexpr DmaMuxRequestId DmaMuxRequestId::kUart4Tx = DmaMuxRequestId::from_ll<LL_DMAMUX_REQ_UART4_TX>();
    inline constexpr DmaMuxRequestId DmaMuxRequestId::kUart5Rx = DmaMuxRequestId::from_ll<LL_DMAMUX_REQ_UART5_RX>();
    inline constexpr DmaMuxRequestId DmaMuxRequestId::kUart5Tx = DmaMuxRequestId::from_ll<LL_DMAMUX_REQ_UART5_TX>();
    inline constexpr DmaMuxRequestId DmaMuxRequestId::kLPUart1Rx = DmaMuxRequestId::from_ll<LL_DMAMUX_REQ_LPUART1_RX>();
    inline constexpr DmaMuxRequestId DmaMuxRequestId::kLPUart1Tx = DmaMuxRequestId::from_ll<LL_DMAMUX_REQ_LPUART1_TX>();
    inline constexpr DmaMuxRequestId DmaMuxRequestId::kAdc2 = DmaMuxRequestId::from_ll<LL_DMAMUX_REQ_ADC2>();
    inline constexpr DmaMuxRequestId DmaMuxRequestId::kAdc3 = DmaMuxRequestId::from_ll<LL_DMAMUX_REQ_ADC3>();
    inline constexpr DmaMuxRequestId DmaMuxRequestId::kAdc4 = DmaMuxRequestId::from_ll<LL_DMAMUX_REQ_ADC4>();
    inline constexpr DmaMuxRequestId DmaMuxRequestId::kAdc5 = DmaMuxRequestId::from_ll<LL_DMAMUX_REQ_ADC5>();
    inline constexpr DmaMuxRequestId DmaMuxRequestId::kQSPI = DmaMuxRequestId::from_ll<LL_DMAMUX_REQ_QSPI>();
    inline constexpr DmaMuxRequestId DmaMuxRequestId::kDAC2Ch1 = DmaMuxRequestId::from_ll<LL_DMAMUX_REQ_DAC2_CH1>();
    inline constexpr DmaMuxRequestId DmaMuxRequestId::kTim1Ch1 = DmaMuxRequestId::from_ll<LL_DMAMUX_REQ_TIM1_CH1>();
    inline constexpr DmaMuxRequestId DmaMuxRequestId::kTim1Ch2 = DmaMuxRequestId::from_ll<LL_DMAMUX_REQ_TIM1_CH2>();
    inline constexpr DmaMuxRequestId DmaMuxRequestId::kTim1Ch3 = DmaMuxRequestId::from_ll<LL_DMAMUX_REQ_TIM1_CH3>();
    inline constexpr DmaMuxRequestId DmaMuxRequestId::kTim1Ch4 = DmaMuxRequestId::from_ll<LL_DMAMUX_REQ_TIM1_CH4>();
    inline constexpr DmaMuxRequestId DmaMuxRequestId::kTim1Up = DmaMuxRequestId::from_ll<LL_DMAMUX_REQ_TIM1_UP>();
    inline constexpr DmaMuxRequestId DmaMuxRequestId::kTim1Trig = DmaMuxRequestId::from_ll<LL_DMAMUX_REQ_TIM1_TRIG>();
    inline constexpr DmaMuxRequestId DmaMuxRequestId::kTim1Com = DmaMuxRequestId::from_ll<LL_DMAMUX_REQ_TIM1_COM>();
    inline constexpr DmaMuxRequestId DmaMuxRequestId::kTim8Ch1 = DmaMuxRequestId::from_ll<LL_DMAMUX_REQ_TIM8_CH1>();
    inline constexpr DmaMuxRequestId DmaMuxRequestId::kTim8Ch2 = DmaMuxRequestId::from_ll<LL_DMAMUX_REQ_TIM8_CH2>();
    inline constexpr DmaMuxRequestId DmaMuxRequestId::kTim8Ch3 = DmaMuxRequestId::from_ll<LL_DMAMUX_REQ_TIM8_CH3>();
    inline constexpr DmaMuxRequestId DmaMuxRequestId::kTim8Ch4 = DmaMuxRequestId::from_ll<LL_DMAMUX_REQ_TIM8_CH4>();
    inline constexpr DmaMuxRequestId DmaMuxRequestId::kTim8Up = DmaMuxRequestId::from_ll<LL_DMAMUX_REQ_TIM8_UP>();
    inline constexpr DmaMuxRequestId DmaMuxRequestId::kTim8Trig = DmaMuxRequestId::from_ll<LL_DMAMUX_REQ_TIM8_TRIG>();
    inline constexpr DmaMuxRequestId DmaMuxRequestId::kTim8Com = DmaMuxRequestId::from_ll<LL_DMAMUX_REQ_TIM8_COM>();
    inline constexpr DmaMuxRequestId DmaMuxRequestId::kTim2Ch1 = DmaMuxRequestId::from_ll<LL_DMAMUX_REQ_TIM2_CH1>();
    inline constexpr DmaMuxRequestId DmaMuxRequestId::kTim2Ch2 = DmaMuxRequestId::from_ll<LL_DMAMUX_REQ_TIM2_CH2>();
    inline constexpr DmaMuxRequestId DmaMuxRequestId::kTim2Ch3 = DmaMuxRequestId::from_ll<LL_DMAMUX_REQ_TIM2_CH3>();
    inline constexpr DmaMuxRequestId DmaMuxRequestId::kTim2Ch4 = DmaMuxRequestId::from_ll<LL_DMAMUX_REQ_TIM2_CH4>();
    inline constexpr DmaMuxRequestId DmaMuxRequestId::kTim2Up = DmaMuxRequestId::from_ll<LL_DMAMUX_REQ_TIM2_UP>();
    inline constexpr DmaMuxRequestId DmaMuxRequestId::kTim3Ch1 = DmaMuxRequestId::from_ll<LL_DMAMUX_REQ_TIM3_CH1>();
    inline constexpr DmaMuxRequestId DmaMuxRequestId::kTim3Ch2 = DmaMuxRequestId::from_ll<LL_DMAMUX_REQ_TIM3_CH2>();
    inline constexpr DmaMuxRequestId DmaMuxRequestId::kTim3Ch3 = DmaMuxRequestId::from_ll<LL_DMAMUX_REQ_TIM3_CH3>();
    inline constexpr DmaMuxRequestId DmaMuxRequestId::kTim3Ch4 = DmaMuxRequestId::from_ll<LL_DMAMUX_REQ_TIM3_CH4>();
    inline constexpr DmaMuxRequestId DmaMuxRequestId::kTim3Up = DmaMuxRequestId::from_ll<LL_DMAMUX_REQ_TIM3_UP>();
    inline constexpr DmaMuxRequestId DmaMuxRequestId::kTim3Trig = DmaMuxRequestId::from_ll<LL_DMAMUX_REQ_TIM3_TRIG>();
    inline constexpr DmaMuxRequestId DmaMuxRequestId::kTim4Ch1 = DmaMuxRequestId::from_ll<LL_DMAMUX_REQ_TIM4_CH1>();
    inline constexpr DmaMuxRequestId DmaMuxRequestId::kTim4Ch2 = DmaMuxRequestId::from_ll<LL_DMAMUX_REQ_TIM4_CH2>();
    inline constexpr DmaMuxRequestId DmaMuxRequestId::kTim4Ch3 = DmaMuxRequestId::from_ll<LL_DMAMUX_REQ_TIM4_CH3>();
    inline constexpr DmaMuxRequestId DmaMuxRequestId::kTim4Ch4 = DmaMuxRequestId::from_ll<LL_DMAMUX_REQ_TIM4_CH4>();
    inline constexpr DmaMuxRequestId DmaMuxRequestId::kTim4Up = DmaMuxRequestId::from_ll<LL_DMAMUX_REQ_TIM4_UP>();
    inline constexpr DmaMuxRequestId DmaMuxRequestId::kTim5Ch1 = DmaMuxRequestId::from_ll<LL_DMAMUX_REQ_TIM5_CH1>();
    inline constexpr DmaMuxRequestId DmaMuxRequestId::kTim5Ch2 = DmaMuxRequestId::from_ll<LL_DMAMUX_REQ_TIM5_CH2>();
    inline constexpr DmaMuxRequestId DmaMuxRequestId::kTim5Ch3 = DmaMuxRequestId::from_ll<LL_DMAMUX_REQ_TIM5_CH3>();
    inline constexpr DmaMuxRequestId DmaMuxRequestId::kTim5Ch4 = DmaMuxRequestId::from_ll<LL_DMAMUX_REQ_TIM5_CH4>();
    inline constexpr DmaMuxRequestId DmaMuxRequestId::kTim5Up = DmaMuxRequestId::from_ll<LL_DMAMUX_REQ_TIM5_UP>();
    inline constexpr DmaMuxRequestId DmaMuxRequestId::kTim5Trig = DmaMuxRequestId::from_ll<LL_DMAMUX_REQ_TIM5_TRIG>();
    inline constexpr DmaMuxRequestId DmaMuxRequestId::kTim15Ch1 = DmaMuxRequestId::from_ll<LL_DMAMUX_REQ_TIM15_CH1>();
    inline constexpr DmaMuxRequestId DmaMuxRequestId::kTim15Up = DmaMuxRequestId::from_ll<LL_DMAMUX_REQ_TIM15_UP>();
    inline constexpr DmaMuxRequestId DmaMuxRequestId::kTim15Trig = DmaMuxRequestId::from_ll<LL_DMAMUX_REQ_TIM15_TRIG>();
    inline constexpr DmaMuxRequestId DmaMuxRequestId::kTim15Com = DmaMuxRequestId::from_ll<LL_DMAMUX_REQ_TIM15_COM>();
    inline constexpr DmaMuxRequestId DmaMuxRequestId::kTim16Ch1 = DmaMuxRequestId::from_ll<LL_DMAMUX_REQ_TIM16_CH1>();
    inline constexpr DmaMuxRequestId DmaMuxRequestId::kTim16Up = DmaMuxRequestId::from_ll<LL_DMAMUX_REQ_TIM16_UP>();
    inline constexpr DmaMuxRequestId DmaMuxRequestId::kTim17Ch1 = DmaMuxRequestId::from_ll<LL_DMAMUX_REQ_TIM17_CH1>();
    inline constexpr DmaMuxRequestId DmaMuxRequestId::kTim17Up = DmaMuxRequestId::from_ll<LL_DMAMUX_REQ_TIM17_UP>();
    inline constexpr DmaMuxRequestId DmaMuxRequestId::kTim20Ch1 = DmaMuxRequestId::from_ll<LL_DMAMUX_REQ_TIM20_CH1>();
    inline constexpr DmaMuxRequestId DmaMuxRequestId::kTim20Ch2 = DmaMuxRequestId::from_ll<LL_DMAMUX_REQ_TIM20_CH2>();
    inline constexpr DmaMuxRequestId DmaMuxRequestId::kTim20Ch3 = DmaMuxRequestId::from_ll<LL_DMAMUX_REQ_TIM20_CH3>();
    inline constexpr DmaMuxRequestId DmaMuxRequestId::kTim20Ch4 = DmaMuxRequestId::from_ll<LL_DMAMUX_REQ_TIM20_CH4>();
    inline constexpr DmaMuxRequestId DmaMuxRequestId::kTim20Up = DmaMuxRequestId::from_ll<LL_DMAMUX_REQ_TIM20_UP>();
    inline constexpr DmaMuxRequestId DmaMuxRequestId::kAESIn = DmaMuxRequestId::from_ll<LL_DMAMUX_REQ_AES_IN>();
    inline constexpr DmaMuxRequestId DmaMuxRequestId::kAESOut = DmaMuxRequestId::from_ll<LL_DMAMUX_REQ_AES_OUT>();
    inline constexpr DmaMuxRequestId DmaMuxRequestId::kTim20Trig = DmaMuxRequestId::from_ll<LL_DMAMUX_REQ_TIM20_TRIG>();
    inline constexpr DmaMuxRequestId DmaMuxRequestId::kTim20Com = DmaMuxRequestId::from_ll<LL_DMAMUX_REQ_TIM20_COM>();
    inline constexpr DmaMuxRequestId DmaMuxRequestId::kHrtim1M = DmaMuxRequestId::from_ll<LL_DMAMUX_REQ_HRTIM1_M>();
    inline constexpr DmaMuxRequestId DmaMuxRequestId::kHrtim1A = DmaMuxRequestId::from_ll<LL_DMAMUX_REQ_HRTIM1_A>();
    inline constexpr DmaMuxRequestId DmaMuxRequestId::kHrtim1B = DmaMuxRequestId::from_ll<LL_DMAMUX_REQ_HRTIM1_B>();
    inline constexpr DmaMuxRequestId DmaMuxRequestId::kHrtim1C = DmaMuxRequestId::from_ll<LL_DMAMUX_REQ_HRTIM1_C>();
    inline constexpr DmaMuxRequestId DmaMuxRequestId::kHrtim1D = DmaMuxRequestId::from_ll<LL_DMAMUX_REQ_HRTIM1_D>();
    inline constexpr DmaMuxRequestId DmaMuxRequestId::kHrtim1E = DmaMuxRequestId::from_ll<LL_DMAMUX_REQ_HRTIM1_E>();
    inline constexpr DmaMuxRequestId DmaMuxRequestId::kHrtim1F = DmaMuxRequestId::from_ll<LL_DMAMUX_REQ_HRTIM1_F>();
    inline constexpr DmaMuxRequestId DmaMuxRequestId::kDAC3Ch1 = DmaMuxRequestId::from_ll<LL_DMAMUX_REQ_DAC3_CH1>();
    inline constexpr DmaMuxRequestId DmaMuxRequestId::kDAC3Ch2 = DmaMuxRequestId::from_ll<LL_DMAMUX_REQ_DAC3_CH2>();
    inline constexpr DmaMuxRequestId DmaMuxRequestId::kDAC4Ch1 = DmaMuxRequestId::from_ll<LL_DMAMUX_REQ_DAC4_CH1>();
    inline constexpr DmaMuxRequestId DmaMuxRequestId::kDAC4Ch2 = DmaMuxRequestId::from_ll<LL_DMAMUX_REQ_DAC4_CH2>();
    inline constexpr DmaMuxRequestId DmaMuxRequestId::kSpi4Rx = DmaMuxRequestId::from_ll<LL_DMAMUX_REQ_SPI4_RX>();
    inline constexpr DmaMuxRequestId DmaMuxRequestId::kSpi4Tx = DmaMuxRequestId::from_ll<LL_DMAMUX_REQ_SPI4_TX>();
    inline constexpr DmaMuxRequestId DmaMuxRequestId::kSAI1A = DmaMuxRequestId::from_ll<LL_DMAMUX_REQ_SAI1_A>();
    inline constexpr DmaMuxRequestId DmaMuxRequestId::kSAI1B = DmaMuxRequestId::from_ll<LL_DMAMUX_REQ_SAI1_B>();
    inline constexpr DmaMuxRequestId DmaMuxRequestId::kFMACRead = DmaMuxRequestId::from_ll<LL_DMAMUX_REQ_FMAC_READ>();
    inline constexpr DmaMuxRequestId DmaMuxRequestId::kFMACWrite = DmaMuxRequestId::from_ll<LL_DMAMUX_REQ_FMAC_WRITE>();
    inline constexpr DmaMuxRequestId DmaMuxRequestId::kCORDICRead = DmaMuxRequestId::from_ll<LL_DMAMUX_REQ_CORDIC_READ>();
    inline constexpr DmaMuxRequestId DmaMuxRequestId::kCORDICWrite = DmaMuxRequestId::from_ll<LL_DMAMUX_REQ_CORDIC_WRITE>();
    inline constexpr DmaMuxRequestId DmaMuxRequestId::kUCPD1Rx = DmaMuxRequestId::from_ll<LL_DMAMUX_REQ_UCPD1_RX>();
    inline constexpr DmaMuxRequestId DmaMuxRequestId::kUCPD1Tx = DmaMuxRequestId::from_ll<LL_DMAMUX_REQ_UCPD1_TX>();


    struct DmaMuxSyncPolarity : public LLDriverEnumValue<DmaMuxSyncPolarity>
    {
        static const DmaMuxSyncPolarity kNoEvent;
        static const DmaMuxSyncPolarity kRising;
        static const DmaMuxSyncPolarity kFalling;
        static const DmaMuxSyncPolarity kBoth;
    };

    inline constexpr DmaMuxSyncPolarity DmaMuxSyncPolarity::kNoEvent = DmaMuxSyncPolarity::from_ll<LL_DMAMUX_SYNC_NO_EVENT>();
    inline constexpr DmaMuxSyncPolarity DmaMuxSyncPolarity::kRising = DmaMuxSyncPolarity::from_ll<LL_DMAMUX_SYNC_POL_RISING>();
    inline constexpr DmaMuxSyncPolarity DmaMuxSyncPolarity::kFalling = DmaMuxSyncPolarity::from_ll<LL_DMAMUX_SYNC_POL_FALLING>();
    inline constexpr DmaMuxSyncPolarity DmaMuxSyncPolarity::kBoth = DmaMuxSyncPolarity::from_ll<LL_DMAMUX_SYNC_POL_RISING_FALLING>();


    struct DmaMuxSyncId : public LLDriverEnumValue<DmaMuxSyncId>
    {
        static const DmaMuxSyncId kExtiLine0;
        static const DmaMuxSyncId kExtiLine1;
        static const DmaMuxSyncId kExtiLine2;
        static const DmaMuxSyncId kExtiLine3;
        static const DmaMuxSyncId kExtiLine4;
        static const DmaMuxSyncId kExtiLine5;
        static const DmaMuxSyncId kExtiLine6;
        static const DmaMuxSyncId kExtiLine7;
        static const DmaMuxSyncId kExtiLine8;
        static const DmaMuxSyncId kExtiLine9;
        static const DmaMuxSyncId kExtiLine10;
        static const DmaMuxSyncId kExtiLine11;
        static const DmaMuxSyncId kExtiLine12;
        static const DmaMuxSyncId kExtiLine13;
        static const DmaMuxSyncId kExtiLine14;
        static const DmaMuxSyncId kExtiLine15;
        static const DmaMuxSyncId kDmaMuxCh0;
        static const DmaMuxSyncId kDmaMuxCh1;
        static const DmaMuxSyncId kDmaMuxCh2;
        static const DmaMuxSyncId kDmaMuxCh3;
        static const DmaMuxSyncId kLptim1Out;
    };

    inline constexpr DmaMuxSyncId DmaMuxSyncId::kExtiLine0 = DmaMuxSyncId::from_ll<LL_DMAMUX_SYNC_EXTI_LINE0>();
    inline constexpr DmaMuxSyncId DmaMuxSyncId::kExtiLine1 = DmaMuxSyncId::from_ll<LL_DMAMUX_SYNC_EXTI_LINE1>();
    inline constexpr DmaMuxSyncId DmaMuxSyncId::kExtiLine2 = DmaMuxSyncId::from_ll<LL_DMAMUX_SYNC_EXTI_LINE2>();
    inline constexpr DmaMuxSyncId DmaMuxSyncId::kExtiLine3 = DmaMuxSyncId::from_ll<LL_DMAMUX_SYNC_EXTI_LINE3>();
    inline constexpr DmaMuxSyncId DmaMuxSyncId::kExtiLine4 = DmaMuxSyncId::from_ll<LL_DMAMUX_SYNC_EXTI_LINE4>();
    inline constexpr DmaMuxSyncId DmaMuxSyncId::kExtiLine5 = DmaMuxSyncId::from_ll<LL_DMAMUX_SYNC_EXTI_LINE5>();
    inline constexpr DmaMuxSyncId DmaMuxSyncId::kExtiLine6 = DmaMuxSyncId::from_ll<LL_DMAMUX_SYNC_EXTI_LINE6>();
    inline constexpr DmaMuxSyncId DmaMuxSyncId::kExtiLine7 = DmaMuxSyncId::from_ll<LL_DMAMUX_SYNC_EXTI_LINE7>();
    inline constexpr DmaMuxSyncId DmaMuxSyncId::kExtiLine8 = DmaMuxSyncId::from_ll<LL_DMAMUX_SYNC_EXTI_LINE8>();
    inline constexpr DmaMuxSyncId DmaMuxSyncId::kExtiLine9 = DmaMuxSyncId::from_ll<LL_DMAMUX_SYNC_EXTI_LINE9>();
    inline constexpr DmaMuxSyncId DmaMuxSyncId::kExtiLine10 = DmaMuxSyncId::from_ll<LL_DMAMUX_SYNC_EXTI_LINE10>();
    inline constexpr DmaMuxSyncId DmaMuxSyncId::kExtiLine11 = DmaMuxSyncId::from_ll<LL_DMAMUX_SYNC_EXTI_LINE11>();
    inline constexpr DmaMuxSyncId DmaMuxSyncId::kExtiLine12 = DmaMuxSyncId::from_ll<LL_DMAMUX_SYNC_EXTI_LINE12>();
    inline constexpr DmaMuxSyncId DmaMuxSyncId::kExtiLine13 = DmaMuxSyncId::from_ll<LL_DMAMUX_SYNC_EXTI_LINE13>();
    inline constexpr DmaMuxSyncId DmaMuxSyncId::kExtiLine14 = DmaMuxSyncId::from_ll<LL_DMAMUX_SYNC_EXTI_LINE14>();
    inline constexpr DmaMuxSyncId DmaMuxSyncId::kExtiLine15 = DmaMuxSyncId::from_ll<LL_DMAMUX_SYNC_EXTI_LINE15>();
    inline constexpr DmaMuxSyncId DmaMuxSyncId::kDmaMuxCh0 = DmaMuxSyncId::from_ll<LL_DMAMUX_SYNC_DMAMUX_CH0>();
    inline constexpr DmaMuxSyncId DmaMuxSyncId::kDmaMuxCh1 = DmaMuxSyncId::from_ll<LL_DMAMUX_SYNC_DMAMUX_CH1>();
    inline constexpr DmaMuxSyncId DmaMuxSyncId::kDmaMuxCh2 = DmaMuxSyncId::from_ll<LL_DMAMUX_SYNC_DMAMUX_CH2>();
    inline constexpr DmaMuxSyncId DmaMuxSyncId::kDmaMuxCh3 = DmaMuxSyncId::from_ll<LL_DMAMUX_SYNC_DMAMUX_CH3>();
    inline constexpr DmaMuxSyncId DmaMuxSyncId::kLptim1Out = DmaMuxSyncId::from_ll<LL_DMAMUX_SYNC_LPTIM1_OUT>();


    struct DmaMuxRequestGenPolarity : public LLDriverEnumValue<DmaMuxRequestGenPolarity>
    {
        static const DmaMuxRequestGenPolarity kNoEvent;
        static const DmaMuxRequestGenPolarity kRising;
        static const DmaMuxRequestGenPolarity kFalling;
        static const DmaMuxRequestGenPolarity kBoth;
    };

    inline constexpr DmaMuxRequestGenPolarity DmaMuxRequestGenPolarity::kNoEvent = DmaMuxRequestGenPolarity::from_ll<LL_DMAMUX_REQ_GEN_NO_EVENT>();
    inline constexpr DmaMuxRequestGenPolarity DmaMuxRequestGenPolarity::kRising = DmaMuxRequestGenPolarity::from_ll<LL_DMAMUX_REQ_GEN_POL_RISING>();
    inline constexpr DmaMuxRequestGenPolarity DmaMuxRequestGenPolarity::kFalling = DmaMuxRequestGenPolarity::from_ll<LL_DMAMUX_REQ_GEN_POL_FALLING>();
    inline constexpr DmaMuxRequestGenPolarity DmaMuxRequestGenPolarity::kBoth = DmaMuxRequestGenPolarity::from_ll<LL_DMAMUX_REQ_GEN_POL_RISING_FALLING>();


    struct DmaRequestGenSignalId : public LLDriverEnumValue<DmaRequestGenSignalId>
    {
        static const DmaRequestGenSignalId kExtiLine0;
        static const DmaRequestGenSignalId kExtiLine1;
        static const DmaRequestGenSignalId kExtiLine2;
        static const DmaRequestGenSignalId kExtiLine3;
        static const DmaRequestGenSignalId kExtiLine4;
        static const DmaRequestGenSignalId kExtiLine5;
        static const DmaRequestGenSignalId kExtiLine6;
        static const DmaRequestGenSignalId kExtiLine7;
        static const DmaRequestGenSignalId kExtiLine8;
        static const DmaRequestGenSignalId kExtiLine9;
        static const DmaRequestGenSignalId kExtiLine10;
        static const DmaRequestGenSignalId kExtiLine11;
        static const DmaRequestGenSignalId kExtiLine12;
        static const DmaRequestGenSignalId kExtiLine13;
        static const DmaRequestGenSignalId kExtiLine14;
        static const DmaRequestGenSignalId kExtiLine15;
        static const DmaRequestGenSignalId kDmaMuxCh0;
        static const DmaRequestGenSignalId kDmaMuxCh1;
        static const DmaRequestGenSignalId kDmaMuxCh2;
        static const DmaRequestGenSignalId kDmaMuxCh3;
        static const DmaRequestGenSignalId kLptim1Out;
    };

    inline constexpr DmaRequestGenSignalId DmaRequestGenSignalId::kExtiLine0 = DmaRequestGenSignalId::from_ll<LL_DMAMUX_REQ_GEN_EXTI_LINE0>();
    inline constexpr DmaRequestGenSignalId DmaRequestGenSignalId::kExtiLine1 = DmaRequestGenSignalId::from_ll<LL_DMAMUX_REQ_GEN_EXTI_LINE1>();
    inline constexpr DmaRequestGenSignalId DmaRequestGenSignalId::kExtiLine2 = DmaRequestGenSignalId::from_ll<LL_DMAMUX_REQ_GEN_EXTI_LINE2>();
    inline constexpr DmaRequestGenSignalId DmaRequestGenSignalId::kExtiLine3 = DmaRequestGenSignalId::from_ll<LL_DMAMUX_REQ_GEN_EXTI_LINE3>();
    inline constexpr DmaRequestGenSignalId DmaRequestGenSignalId::kExtiLine4 = DmaRequestGenSignalId::from_ll<LL_DMAMUX_REQ_GEN_EXTI_LINE4>();
    inline constexpr DmaRequestGenSignalId DmaRequestGenSignalId::kExtiLine5 = DmaRequestGenSignalId::from_ll<LL_DMAMUX_REQ_GEN_EXTI_LINE5>();
    inline constexpr DmaRequestGenSignalId DmaRequestGenSignalId::kExtiLine6 = DmaRequestGenSignalId::from_ll<LL_DMAMUX_REQ_GEN_EXTI_LINE6>();
    inline constexpr DmaRequestGenSignalId DmaRequestGenSignalId::kExtiLine7 = DmaRequestGenSignalId::from_ll<LL_DMAMUX_REQ_GEN_EXTI_LINE7>();
    inline constexpr DmaRequestGenSignalId DmaRequestGenSignalId::kExtiLine8 = DmaRequestGenSignalId::from_ll<LL_DMAMUX_REQ_GEN_EXTI_LINE8>();
    inline constexpr DmaRequestGenSignalId DmaRequestGenSignalId::kExtiLine9 = DmaRequestGenSignalId::from_ll<LL_DMAMUX_REQ_GEN_EXTI_LINE9>();
    inline constexpr DmaRequestGenSignalId DmaRequestGenSignalId::kExtiLine10 = DmaRequestGenSignalId::from_ll<LL_DMAMUX_REQ_GEN_EXTI_LINE10>();
    inline constexpr DmaRequestGenSignalId DmaRequestGenSignalId::kExtiLine11 = DmaRequestGenSignalId::from_ll<LL_DMAMUX_REQ_GEN_EXTI_LINE11>();
    inline constexpr DmaRequestGenSignalId DmaRequestGenSignalId::kExtiLine12 = DmaRequestGenSignalId::from_ll<LL_DMAMUX_REQ_GEN_EXTI_LINE12>();
    inline constexpr DmaRequestGenSignalId DmaRequestGenSignalId::kExtiLine13 = DmaRequestGenSignalId::from_ll<LL_DMAMUX_REQ_GEN_EXTI_LINE13>();
    inline constexpr DmaRequestGenSignalId DmaRequestGenSignalId::kExtiLine14 = DmaRequestGenSignalId::from_ll<LL_DMAMUX_REQ_GEN_EXTI_LINE14>();
    inline constexpr DmaRequestGenSignalId DmaRequestGenSignalId::kExtiLine15 = DmaRequestGenSignalId::from_ll<LL_DMAMUX_REQ_GEN_EXTI_LINE15>();
    inline constexpr DmaRequestGenSignalId DmaRequestGenSignalId::kDmaMuxCh0 = DmaRequestGenSignalId::from_ll<LL_DMAMUX_REQ_GEN_DMAMUX_CH0>();
    inline constexpr DmaRequestGenSignalId DmaRequestGenSignalId::kDmaMuxCh1 = DmaRequestGenSignalId::from_ll<LL_DMAMUX_REQ_GEN_DMAMUX_CH1>();
    inline constexpr DmaRequestGenSignalId DmaRequestGenSignalId::kDmaMuxCh2 = DmaRequestGenSignalId::from_ll<LL_DMAMUX_REQ_GEN_DMAMUX_CH2>();
    inline constexpr DmaRequestGenSignalId DmaRequestGenSignalId::kDmaMuxCh3 = DmaRequestGenSignalId::from_ll<LL_DMAMUX_REQ_GEN_DMAMUX_CH3>();
    inline constexpr DmaRequestGenSignalId DmaRequestGenSignalId::kLptim1Out = DmaRequestGenSignalId::from_ll<LL_DMAMUX_REQ_GEN_LPTIM1_OUT>();


    // =============================================================================
    // ROOT TRAITS
    // =============================================================================
    struct DmaMuxRootTraits
    {
        static constexpr std::array<uint32_t, 16> skChannelLLIdTable = {
            LL_DMAMUX_CHANNEL_0,
            LL_DMAMUX_CHANNEL_1,
            LL_DMAMUX_CHANNEL_2,
            LL_DMAMUX_CHANNEL_3,
            LL_DMAMUX_CHANNEL_4,
            LL_DMAMUX_CHANNEL_5,
            LL_DMAMUX_CHANNEL_6,
            LL_DMAMUX_CHANNEL_7,
            LL_DMAMUX_CHANNEL_8,
            LL_DMAMUX_CHANNEL_9,
            LL_DMAMUX_CHANNEL_10,
            LL_DMAMUX_CHANNEL_11,
            LL_DMAMUX_CHANNEL_12,
            LL_DMAMUX_CHANNEL_13,
            LL_DMAMUX_CHANNEL_14,
            LL_DMAMUX_CHANNEL_15,
        };

        static constexpr std::array<uint32_t, 4> skRequestGenChannelLLIdTable = {
            LL_DMAMUX_REQ_GEN_0,
            LL_DMAMUX_REQ_GEN_1,
            LL_DMAMUX_REQ_GEN_2,
            LL_DMAMUX_REQ_GEN_3,
        };

        [[nodiscard]] static constexpr uint8_t controller_id_to_index(const DmaMuxControllerId controller_id)
        {
            static_assert(static_cast<uint8_t>(DmaMuxControllerId::kDmaMux1) == 1,
                          "DmaMuxControllerId enum values must start at 1");
            return static_cast<uint8_t>(controller_id) - 1;
        }

        [[nodiscard]] static constexpr uint8_t controller_id_to_number(const DmaMuxControllerId controller_id)
        {
            static_assert(static_cast<uint8_t>(DmaMuxControllerId::kDmaMux1) == 1,
                          "DmaMuxControllerId enum values must start at 1");
            return static_cast<uint8_t>(controller_id);
        }

        [[nodiscard]] static constexpr uint32_t channel_id_to_index(const DmaMuxChannelId channel_id)
        {
            static_assert(static_cast<uint8_t>(DmaMuxChannelId::kChannel0) == 0,
                          "DmaMuxChannelId enum values must start at 0");
            return static_cast<uint8_t>(channel_id);
        }

        [[nodiscard]] static constexpr uint8_t channel_id_to_number(const DmaMuxChannelId channel_id)
        {
            static_assert(static_cast<uint8_t>(DmaMuxChannelId::kChannel0) == 0,
                          "DmaMuxChannelId enum values must start at 0");
            return static_cast<uint8_t>(channel_id);
        }

        [[nodiscard]] static constexpr uint32_t channel_id_to_ll_id(const DmaMuxChannelId channel_id)
        {
            return skChannelLLIdTable[channel_id_to_index(channel_id)];
        }

        [[nodiscard]] static constexpr uint8_t request_gen_channel_id_to_index(
            const DmaMuxRequestGenChannelId channel_id)
        {
            static_assert(static_cast<uint8_t>(DmaMuxRequestGenChannelId::kGenerator0) == 0,
                          "DmaMuxRequestGenChannelId enum values must start at 0");
            return static_cast<uint8_t>(channel_id);
        }

        [[nodiscard]] static constexpr uint8_t request_gen_channel_id_to_number(
            const DmaMuxRequestGenChannelId channel_id)
        {
            static_assert(static_cast<uint8_t>(DmaMuxRequestGenChannelId::kGenerator0) == 0,
                          "DmaMuxRequestGenChannelId enum values must start at 0");
            return static_cast<uint8_t>(channel_id);
        }

        [[nodiscard]] static constexpr uint32_t request_gen_channel_id_to_ll_id(
            const DmaMuxRequestGenChannelId channel_id)
        {
            return skRequestGenChannelLLIdTable[request_gen_channel_id_to_index(channel_id)];
        };

        // ============================================================================
        // HARDWARE TRAITS
        // ============================================================================
        template <DmaMuxControllerSpec tkControllerSpec>
        struct DmaMuxControllerTraits;

        template <>
        struct DmaMuxControllerTraits<kDmaMux1ControllerSpec>
        {
            static inline DMAMUX_Channel_TypeDef *const skInstance = DMAMUX1;
        };

        template <DmaMuxChannelSpec tkChannelSpec>
        struct DmaMuxChannelTraits
        {
            static constexpr DmaMuxChannelSpec skChannelSpec = tkChannelSpec;
            static constexpr uint32_t skChannelLLId = DmaMuxRootTraits::channel_id_to_ll_id(skChannelSpec.channel_id);
        };

        template <DmaMuxRequestGenChannelSpec tkRequestGenChannelSpec>
        struct DmaMuxRequestGenChannelTraits
        {
            static constexpr DmaMuxRequestGenChannelSpec skChannelSpec = tkRequestGenChannelSpec;
            static constexpr uint32_t                    skChannelLLId =
                DmaMuxRootTraits::request_gen_channel_id_to_ll_id(skChannelSpec.generator_id);
        };

        // ============================================================================
        // INTERFACE
        // ============================================================================

        template <DmaMuxControllerSpec tkControllerSpec>
        struct DmaMuxControllerInterface
        {
            void enable_clock() const
            {
                if constexpr (tkControllerSpec.controller_id == DmaMuxControllerId::kDmaMux1)
                {
                    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMAMUX1);
                }
            }

            void disable_clock() const
            {
                if constexpr (tkControllerSpec.controller_id == DmaMuxControllerId::kDmaMux1)
                {
                    LL_AHB1_GRP1_DisableClock(LL_AHB1_GRP1_PERIPH_DMAMUX1);
                }
            }
        };

        template <DmaMuxChannelSpec tkChannelSpec>
        struct DmaMuxChannelInterface
        {
            using ControllerTraitsT = DmaMuxControllerTraits<tkChannelSpec.controller_spec()>;
            using ChannelTraitsT    = DmaMuxChannelTraits<tkChannelSpec>;

            void set_request_id(const DmaMuxRequestId request_id) const
            {
                LL_DMAMUX_SetRequestID(
                    ControllerTraitsT::skInstance, ChannelTraitsT::skChannelLLId, static_cast<uint32_t>(request_id));
            }

            [[nodiscard]] DmaMuxRequestId get_request_id() const
            {
                return static_cast<DmaMuxRequestId>(
                    LL_DMAMUX_GetRequestID(ControllerTraitsT::skInstance, ChannelTraitsT::skChannelLLId));
            }

            void set_sync_request_number(const uint32_t request_number) const
            {
                LL_DMAMUX_SetSyncRequestNb(
                    ControllerTraitsT::skInstance, ChannelTraitsT::skChannelLLId, request_number);
            }

            [[nodiscard]] uint32_t get_sync_request_number() const
            {
                return LL_DMAMUX_GetSyncRequestNb(ControllerTraitsT::skInstance, ChannelTraitsT::skChannelLLId);
            }

            void set_sync_polarity(const DmaMuxSyncPolarity polarity) const
            {
                LL_DMAMUX_SetSyncPolarity(
                    ControllerTraitsT::skInstance, ChannelTraitsT::skChannelLLId, static_cast<uint32_t>(polarity));
            }

            [[nodiscard]] DmaMuxSyncPolarity get_sync_polarity() const
            {
                return static_cast<DmaMuxSyncPolarity>(
                    LL_DMAMUX_GetSyncPolarity(ControllerTraitsT::skInstance, ChannelTraitsT::skChannelLLId));
            }

            void enable_event_generation() const
            {
                LL_DMAMUX_EnableEventGeneration(ControllerTraitsT::skInstance, ChannelTraitsT::skChannelLLId);
            }

            void disable_event_generation() const
            {
                LL_DMAMUX_DisableEventGeneration(ControllerTraitsT::skInstance, ChannelTraitsT::skChannelLLId);
            }

            [[nodiscard]] bool is_enabled_event_generation() const
            {
                return static_cast<bool>(LL_DMAMUX_IsEnabledEventGeneration(ControllerTraitsT::skInstance,
                                                                            ChannelTraitsT::skChannelLLId) != 0);
            }

            void enable_sync() const
            {
                LL_DMAMUX_EnableSync(ControllerTraitsT::skInstance, ChannelTraitsT::skChannelLLId);
            }

            void disable_sync() const
            {
                LL_DMAMUX_DisableSync(ControllerTraitsT::skInstance, ChannelTraitsT::skChannelLLId);
            }

            [[nodiscard]] bool is_enabled_sync() const
            {
                return static_cast<bool>(
                    LL_DMAMUX_IsEnabledSync(ControllerTraitsT::skInstance, ChannelTraitsT::skChannelLLId) != 0);
            }

            void set_sync_id(const DmaMuxSyncId sync_id) const
            {
                LL_DMAMUX_SetSyncID(
                    ControllerTraitsT::skInstance, ChannelTraitsT::skChannelLLId, static_cast<uint32_t>(sync_id));
            }

            [[nodiscard]] DmaMuxSyncId get_sync_id() const
            {
                return static_cast<DmaMuxSyncId>(
                    LL_DMAMUX_GetSyncID(ControllerTraitsT::skInstance, ChannelTraitsT::skChannelLLId));
            }

            void clear_sync_overrun_flag() const
            {
                if constexpr (tkChannelSpec.channel_id == DmaMuxChannelId::kChannel0)
                {
                    LL_DMAMUX_ClearFlag_SO0(ControllerTraitsT::skInstance);
                }
                else if constexpr (tkChannelSpec.channel_id == DmaMuxChannelId::kChannel1)
                {
                    LL_DMAMUX_ClearFlag_SO1(ControllerTraitsT::skInstance);
                }
                else if constexpr (tkChannelSpec.channel_id == DmaMuxChannelId::kChannel2)
                {
                    LL_DMAMUX_ClearFlag_SO2(ControllerTraitsT::skInstance);
                }
                else if constexpr (tkChannelSpec.channel_id == DmaMuxChannelId::kChannel3)
                {
                    LL_DMAMUX_ClearFlag_SO3(ControllerTraitsT::skInstance);
                }
                else if constexpr (tkChannelSpec.channel_id == DmaMuxChannelId::kChannel4)
                {
                    LL_DMAMUX_ClearFlag_SO4(ControllerTraitsT::skInstance);
                }
                else if constexpr (tkChannelSpec.channel_id == DmaMuxChannelId::kChannel5)
                {
                    LL_DMAMUX_ClearFlag_SO5(ControllerTraitsT::skInstance);
                }
                else if constexpr (tkChannelSpec.channel_id == DmaMuxChannelId::kChannel6)
                {
                    LL_DMAMUX_ClearFlag_SO6(ControllerTraitsT::skInstance);
                }
                else if constexpr (tkChannelSpec.channel_id == DmaMuxChannelId::kChannel7)
                {
                    LL_DMAMUX_ClearFlag_SO7(ControllerTraitsT::skInstance);
                }
                else if constexpr (tkChannelSpec.channel_id == DmaMuxChannelId::kChannel8)
                {
                    LL_DMAMUX_ClearFlag_SO8(ControllerTraitsT::skInstance);
                }
                else if constexpr (tkChannelSpec.channel_id == DmaMuxChannelId::kChannel9)
                {
                    LL_DMAMUX_ClearFlag_SO9(ControllerTraitsT::skInstance);
                }
                else if constexpr (tkChannelSpec.channel_id == DmaMuxChannelId::kChannel10)
                {
                    LL_DMAMUX_ClearFlag_SO10(ControllerTraitsT::skInstance);
                }
                else if constexpr (tkChannelSpec.channel_id == DmaMuxChannelId::kChannel11)
                {
                    LL_DMAMUX_ClearFlag_SO11(ControllerTraitsT::skInstance);
                }
                else if constexpr (tkChannelSpec.channel_id == DmaMuxChannelId::kChannel12)
                {
                    LL_DMAMUX_ClearFlag_SO12(ControllerTraitsT::skInstance);
                }
                else if constexpr (tkChannelSpec.channel_id == DmaMuxChannelId::kChannel13)
                {
                    LL_DMAMUX_ClearFlag_SO13(ControllerTraitsT::skInstance);
                }
                else if constexpr (tkChannelSpec.channel_id == DmaMuxChannelId::kChannel14)
                {
                    LL_DMAMUX_ClearFlag_SO14(ControllerTraitsT::skInstance);
                }
                else if constexpr (tkChannelSpec.channel_id == DmaMuxChannelId::kChannel15)
                {
                    LL_DMAMUX_ClearFlag_SO15(ControllerTraitsT::skInstance);
                }
                else
                {
                    static_assert(kAlwaysFalseV<DmaMuxChannelId>, "Invalid DmaMuxChannelId");
                }
            }

            [[nodiscard]] bool is_sync_overrun_flag_active() const
            {
                if constexpr (tkChannelSpec.channel_id == DmaMuxChannelId::kChannel0)
                {
                    return static_cast<bool>(LL_DMAMUX_IsActiveFlag_SO0(ControllerTraitsT::skInstance) != 0);
                }
                else if constexpr (tkChannelSpec.channel_id == DmaMuxChannelId::kChannel1)
                {
                    return static_cast<bool>(LL_DMAMUX_IsActiveFlag_SO1(ControllerTraitsT::skInstance) != 0);
                }
                else if constexpr (tkChannelSpec.channel_id == DmaMuxChannelId::kChannel2)
                {
                    return static_cast<bool>(LL_DMAMUX_IsActiveFlag_SO2(ControllerTraitsT::skInstance) != 0);
                }
                else if constexpr (tkChannelSpec.channel_id == DmaMuxChannelId::kChannel3)
                {
                    return static_cast<bool>(LL_DMAMUX_IsActiveFlag_SO3(ControllerTraitsT::skInstance) != 0);
                }
                else if constexpr (tkChannelSpec.channel_id == DmaMuxChannelId::kChannel4)
                {
                    return static_cast<bool>(LL_DMAMUX_IsActiveFlag_SO4(ControllerTraitsT::skInstance) != 0);
                }
                else if constexpr (tkChannelSpec.channel_id == DmaMuxChannelId::kChannel5)
                {
                    return static_cast<bool>(LL_DMAMUX_IsActiveFlag_SO5(ControllerTraitsT::skInstance) != 0);
                }
                else if constexpr (tkChannelSpec.channel_id == DmaMuxChannelId::kChannel6)
                {
                    return static_cast<bool>(LL_DMAMUX_IsActiveFlag_SO6(ControllerTraitsT::skInstance) != 0);
                }
                else if constexpr (tkChannelSpec.channel_id == DmaMuxChannelId::kChannel7)
                {
                    return static_cast<bool>(LL_DMAMUX_IsActiveFlag_SO7(ControllerTraitsT::skInstance) != 0);
                }
                else if constexpr (tkChannelSpec.channel_id == DmaMuxChannelId::kChannel8)
                {
                    return static_cast<bool>(LL_DMAMUX_IsActiveFlag_SO8(ControllerTraitsT::skInstance) != 0);
                }
                else if constexpr (tkChannelSpec.channel_id == DmaMuxChannelId::kChannel9)
                {
                    return static_cast<bool>(LL_DMAMUX_IsActiveFlag_SO9(ControllerTraitsT::skInstance) != 0);
                }
                else if constexpr (tkChannelSpec.channel_id == DmaMuxChannelId::kChannel10)
                {
                    return static_cast<bool>(LL_DMAMUX_IsActiveFlag_SO10(ControllerTraitsT::skInstance) != 0);
                }
                else if constexpr (tkChannelSpec.channel_id == DmaMuxChannelId::kChannel11)
                {
                    return static_cast<bool>(LL_DMAMUX_IsActiveFlag_SO11(ControllerTraitsT::skInstance) != 0);
                }
                else if constexpr (tkChannelSpec.channel_id == DmaMuxChannelId::kChannel12)
                {
                    return static_cast<bool>(LL_DMAMUX_IsActiveFlag_SO12(ControllerTraitsT::skInstance) != 0);
                }
                else if constexpr (tkChannelSpec.channel_id == DmaMuxChannelId::kChannel13)
                {
                    return static_cast<bool>(LL_DMAMUX_IsActiveFlag_SO13(ControllerTraitsT::skInstance) != 0);
                }
                else if constexpr (tkChannelSpec.channel_id == DmaMuxChannelId::kChannel14)
                {
                    return static_cast<bool>(LL_DMAMUX_IsActiveFlag_SO14(ControllerTraitsT::skInstance) != 0);
                }
                else if constexpr (tkChannelSpec.channel_id == DmaMuxChannelId::kChannel15)
                {
                    return static_cast<bool>(LL_DMAMUX_IsActiveFlag_SO15(ControllerTraitsT::skInstance) != 0);
                }
                else
                {
                    static_assert(kAlwaysFalseV<tkChannelSpec.channel_id>, "Invalid DmaMuxChannelId");
                    return false;
                }
            }

            void enable_sync_overrun_interrupt() const
            {
                LL_DMAMUX_EnableIT_SO(ControllerTraitsT::skInstance, ChannelTraitsT::skChannelLLId);
            }

            void disable_sync_overrun_interrupt() const
            {
                LL_DMAMUX_DisableIT_SO(ControllerTraitsT::skInstance, ChannelTraitsT::skChannelLLId);
            }

            [[nodiscard]] bool is_sync_overrun_interrupt_enabled() const
            {
                return static_cast<bool>(
                    LL_DMAMUX_IsEnabledIT_SO(ControllerTraitsT::skInstance, ChannelTraitsT::skChannelLLId) != 0);
            }
        };

        template <DmaMuxControllerId tkControllerId, DmaMuxRequestGenChannelId tkRequestGenChannelId>
        struct DmaMuxRequestGenChannelInterface
        {
            using ControllerTraitsT = DmaMuxControllerTraits<tkControllerId>;
            using ChannelTraitsT    = DmaMuxRequestGenChannelTraits<tkControllerId, tkRequestGenChannelId>;

            void enable_request_generation() const
            {
                LL_DMAMUX_EnableRequestGen(ControllerTraitsT::skInstance, ChannelTraitsT::skChannelLLId);
            }

            void disable_request_generation() const
            {
                LL_DMAMUX_DisableRequestGen(ControllerTraitsT::skInstance, ChannelTraitsT::skChannelLLId);
            }

            [[nodiscard]] bool is_request_generation_enabled() const
            {
                return static_cast<bool>(
                    LL_DMAMUX_IsEnabledRequestGen(ControllerTraitsT::skInstance, ChannelTraitsT::skChannelLLId) != 0);
            }

            void set_request_gen_polarity(const DmaMuxRequestGenPolarity polarity) const
            {
                LL_DMAMUX_SetRequestGenPolarity(
                    ControllerTraitsT::skInstance, ChannelTraitsT::skChannelLLId, static_cast<uint32_t>(polarity));
            }

            [[nodiscard]] DmaMuxRequestGenPolarity get_request_gen_polarity() const
            {
                return static_cast<DmaMuxRequestGenPolarity>(
                    LL_DMAMUX_GetRequestGenPolarity(ControllerTraitsT::skInstance, ChannelTraitsT::skChannelLLId));
            }

            void set_gen_request_number(const uint32_t request_number) const
            {
                LL_DMAMUX_SetGenRequestNb(ControllerTraitsT::skInstance, ChannelTraitsT::skChannelLLId, request_number);
            }

            [[nodiscard]] uint32_t get_gen_request_number() const
            {
                return static_cast<uint32_t>(
                    LL_DMAMUX_GetGenRequestNb(ControllerTraitsT::skInstance, ChannelTraitsT::skChannelLLId));
            }

            void set_request_signal_id(const DmaMuxRequestGenSignalId signal_id) const
            {
                LL_DMAMUX_SetRequestSignalID(
                    ControllerTraitsT::skInstance, ChannelTraitsT::skChannelLLId, static_cast<uint32_t>(signal_id));
            }

            [[nodiscard]] DmaMuxRequestGenSignalId get_request_signal_id() const
            {
                return static_cast<DmaMuxRequestGenSignalId>(
                    LL_DMAMUX_GetRequestSignalID(ControllerTraitsT::skInstance, ChannelTraitsT::skChannelLLId));
            }

            void clear_overrun_flag() const
            {
                if constexpr (tkRequestGenChannelId == DmaMuxRequestGenChannelId::kGenerator0)
                {
                    LL_DMAMUX_ClearFlag_RGO0(ControllerTraitsT::skInstance);
                }
                else if constexpr (tkRequestGenChannelId == DmaMuxRequestGenChannelId::kGenerator1)
                {
                    LL_DMAMUX_ClearFlag_RGO1(ControllerTraitsT::skInstance);
                }
                else if constexpr (tkRequestGenChannelId == DmaMuxRequestGenChannelId::kGenerator2)
                {
                    LL_DMAMUX_ClearFlag_RGO2(ControllerTraitsT::skInstance);
                }
                else if constexpr (tkRequestGenChannelId == DmaMuxRequestGenChannelId::kGenerator3)
                {
                    LL_DMAMUX_ClearFlag_RGO3(ControllerTraitsT::skInstance);
                }
                else
                {
                    static_assert(kAlwaysFalsev<DmaMuxRequestGenChannelId>, "Invalid DmaMuxRequestGenChannelId");
                }
            }

            [[nodiscard]] bool is_overrun_flag_active() const
            {
                if constexpr (tkRequestGenChannelId == DmaMuxRequestGenChannelId::kGenerator0)
                {
                    return static_cast<bool>(LL_DMAMUX_IsActiveFlag_RGO0(ControllerTraitsT::skInstance) != 0);
                }
                else if constexpr (tkRequestGenChannelId == DmaMuxRequestGenChannelId::kGenerator1)
                {
                    return static_cast<bool>(LL_DMAMUX_IsActiveFlag_RGO1(ControllerTraitsT::skInstance) != 0);
                }
                else if constexpr (tkRequestGenChannelId == DmaMuxRequestGenChannelId::kGenerator2)
                {
                    return static_cast<bool>(LL_DMAMUX_IsActiveFlag_RGO2(ControllerTraitsT::skInstance) != 0);
                }
                else if constexpr (tkRequestGenChannelId == DmaMuxRequestGenChannelId::kGenerator3)
                {
                    return static_cast<bool>(LL_DMAMUX_IsActiveFlag_RGO3(ControllerTraitsT::skInstance) != 0);
                }
                else
                {
                    static_assert(kAlwaysFalsev<DmaMuxRequestGenChannelId>, "Invalid DmaMuxRequestGenChannelId");
                    return false;
                }
            }

            void enable_overrun_interrupt() const
            {
                LL_DMAMUX_EnableIT_RGO(ControllerTraitsT::skInstance, ChannelTraitsT::skChannelLLId);
            }

            void disable_overrun_interrupt() const
            {
                LL_DMAMUX_DisableIT_RGO(ControllerTraitsT::skInstance, ChannelTraitsT::skChannelLLId);
            }

            [[nodiscard]] bool is_overrun_interrupt_enabled() const
            {
                return static_cast<bool>(
                    LL_DMAMUX_IsEnabledIT_RGO(ControllerTraitsT::skInstance, ChannelTraitsT::skChannelLLId) != 0);
            }
        };

    }  // namespace valle::platform
