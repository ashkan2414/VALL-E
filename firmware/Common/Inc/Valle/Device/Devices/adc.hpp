#pragma once

#include <algorithm>
#include <array>
#include <cstdint>
#include <optional>
#include <variant>

#include "Valle/Device/device_core.hpp"
#include "Valle/Drivers/gpio.hpp"
#include "stm32g4xx_ll_adc.h"
#include "stm32g4xx_ll_bus.h"
#include "stm32g4xx_ll_rcc.h"

template <uint8_t tkADCIdx>
constexpr bool kValidADCIndex = (1 <= tkADCIdx && tkADCIdx <= 5);

enum class ADCChannelId : uint32_t
{
    kChannel0              = LL_ADC_CHANNEL_0,
    kChannel1              = LL_ADC_CHANNEL_1,
    kChannel2              = LL_ADC_CHANNEL_2,
    kChannel3              = LL_ADC_CHANNEL_3,
    kChannel4              = LL_ADC_CHANNEL_4,
    kChannel5              = LL_ADC_CHANNEL_5,
    kChannel6              = LL_ADC_CHANNEL_6,
    kChannel7              = LL_ADC_CHANNEL_7,
    kChannel8              = LL_ADC_CHANNEL_8,
    kChannel9              = LL_ADC_CHANNEL_9,
    kChannel10             = LL_ADC_CHANNEL_10,
    kChannel11             = LL_ADC_CHANNEL_11,
    kChannel12             = LL_ADC_CHANNEL_12,
    kChannel13             = LL_ADC_CHANNEL_13,
    kChannel14             = LL_ADC_CHANNEL_14,
    kChannel15             = LL_ADC_CHANNEL_15,
    kChannel16             = LL_ADC_CHANNEL_16,
    kChannel17             = LL_ADC_CHANNEL_17,
    kChannel18             = LL_ADC_CHANNEL_18,
    kChannelVRefInt        = LL_ADC_CHANNEL_VREFINT,
    kChannelTempSensorADC1 = LL_ADC_CHANNEL_TEMPSENSOR_ADC1,
    kChannelTempSensorADC5 = LL_ADC_CHANNEL_TEMPSENSOR_ADC5,
    kChannelVBat           = LL_ADC_CHANNEL_VBAT,
    kChannelVOPAmp1        = LL_ADC_CHANNEL_VOPAMP1,
    kChannelVOPAmp2        = LL_ADC_CHANNEL_VOPAMP2,
    kChannelVOPAmp3ADC2    = LL_ADC_CHANNEL_VOPAMP3_ADC2,
    kChannelVOPAmp3ADC3    = LL_ADC_CHANNEL_VOPAMP3_ADC3,
    kChannelVOPAmp4        = LL_ADC_CHANNEL_VOPAMP4,
    kChannelVOPAmp5        = LL_ADC_CHANNEL_VOPAMP5,
    kChannelVOPAmp6        = LL_ADC_CHANNEL_VOPAMP6,
};

template <uint8_t tkADCIdx, ADCChannelId tkChannel>
constexpr bool kValidADCChannel = kValidADCIndex<tkADCIdx>;  // TODO: Add channel validation per ADC if needed

template <uint8_t tkRank>
constexpr bool kValidADCInjectRank = (tkRank >= 1 && tkRank <= 4);

template <uint8_t tkRank>
constexpr bool kValidADCRegularRank = (tkRank >= 1 && tkRank <= 16);

// ============================================================================
// TRAITS
// ============================================================================
template <uint8_t tkADCIdx>
    requires(kValidADCIndex<tkADCIdx>)
struct ADCTraits;

template <>
struct ADCTraits<1>
{
    static inline ADC_TypeDef*              skInstance = ADC1;
    static constexpr uint32_t               skClock    = LL_AHB2_GRP1_PERIPH_ADC12;
    static inline const ADC_Common_TypeDef* skCommon   = ADC12_COMMON;
};
template <>
struct ADCTraits<2>
{
    static inline ADC_TypeDef*              skInstance = ADC2;
    static constexpr uint32_t               skClock    = LL_AHB2_GRP1_PERIPH_ADC12;
    static inline const ADC_Common_TypeDef* skCommon   = ADC12_COMMON;
};

template <>
struct ADCTraits<3>
{
    static inline ADC_TypeDef*              skInstance = ADC3;
    static constexpr uint32_t               skClock    = LL_AHB2_GRP1_PERIPH_ADC345;
    static inline const ADC_Common_TypeDef* skCommon   = ADC345_COMMON;
};

template <>
struct ADCTraits<4>
{
    static inline ADC_TypeDef*              skInstance = ADC4;
    static constexpr uint32_t               skClock    = LL_AHB2_GRP1_PERIPH_ADC345;
    static inline const ADC_Common_TypeDef* skCommon   = ADC345_COMMON;
};

template <>
struct ADCTraits<5>
{
    static inline ADC_TypeDef*              skInstance = ADC5;
    static constexpr uint32_t               skClock    = LL_AHB2_GRP1_PERIPH_ADC345;
    static inline const ADC_Common_TypeDef* skCommon   = ADC345_COMMON;
};

// =============================================================================
// ADC Pin Mappings
// =============================================================================
template <uint8_t tkAdcIdx, ADCChannelId tkChannel>
struct ADCPinMap
{
    using type = GPIONullPinDevice;
};

#define DECLARE_ADC_PIN_MAP(tkAdcIdx, tkChannel, PinT) \
    template <>                                        \
    struct ADCPinMap<tkAdcIdx, tkChannel>              \
    {                                                  \
        using type = PinT;                             \
    };

// ADC1
DECLARE_ADC_PIN_MAP(1, ADCChannelId::kChannel3, GPIOPinA2);    // PA2
DECLARE_ADC_PIN_MAP(1, ADCChannelId::kChannel4, GPIOPinA3);    // PA3
DECLARE_ADC_PIN_MAP(1, ADCChannelId::kChannel5, GPIOPinB14);   // PB14
DECLARE_ADC_PIN_MAP(1, ADCChannelId::kChannel10, GPIOPinF0);   // PF0 (OSC_IN - Check SB)
DECLARE_ADC_PIN_MAP(1, ADCChannelId::kChannel11, GPIOPinB12);  // PB12
DECLARE_ADC_PIN_MAP(1, ADCChannelId::kChannel12, GPIOPinB1);   // PB1
DECLARE_ADC_PIN_MAP(1, ADCChannelId::kChannel15, GPIOPinB0);   // PB0

// ADC2
DECLARE_ADC_PIN_MAP(2, ADCChannelId::kChannel3, GPIOPinA6);    // PA6
DECLARE_ADC_PIN_MAP(2, ADCChannelId::kChannel4, GPIOPinA7);    // PA7
DECLARE_ADC_PIN_MAP(2, ADCChannelId::kChannel5, GPIOPinC4);    // PC4
DECLARE_ADC_PIN_MAP(2, ADCChannelId::kChannel10, GPIOPinF1);   // PF1 (OSC_OUT - Check SB)
DECLARE_ADC_PIN_MAP(2, ADCChannelId::kChannel11, GPIOPinC5);   // PC5
DECLARE_ADC_PIN_MAP(2, ADCChannelId::kChannel12, GPIOPinB2);   // PB2
DECLARE_ADC_PIN_MAP(2, ADCChannelId::kChannel13, GPIOPinA5);   // PA5
DECLARE_ADC_PIN_MAP(2, ADCChannelId::kChannel15, GPIOPinB15);  // PB15
DECLARE_ADC_PIN_MAP(2, ADCChannelId::kChannel17, GPIOPinA4);   // PA4

// ADC3
DECLARE_ADC_PIN_MAP(3, ADCChannelId::kChannel1, GPIOPinB1);   // PB1
DECLARE_ADC_PIN_MAP(3, ADCChannelId::kChannel2, GPIOPinE9);   // PE9
DECLARE_ADC_PIN_MAP(3, ADCChannelId::kChannel3, GPIOPinE13);  // PE13
DECLARE_ADC_PIN_MAP(3, ADCChannelId::kChannel4, GPIOPinE7);   // PE7
DECLARE_ADC_PIN_MAP(3, ADCChannelId::kChannel5, GPIOPinB13);  // PB13
DECLARE_ADC_PIN_MAP(3, ADCChannelId::kChannel12, GPIOPinB0);  // PB0

// ADC4
DECLARE_ADC_PIN_MAP(4, ADCChannelId::kChannel1, GPIOPinE14);  // PE14
DECLARE_ADC_PIN_MAP(4, ADCChannelId::kChannel2, GPIOPinE15);  // PE15
DECLARE_ADC_PIN_MAP(4, ADCChannelId::kChannel3, GPIOPinB12);  // PB12
DECLARE_ADC_PIN_MAP(4, ADCChannelId::kChannel4, GPIOPinB14);  // PB14
DECLARE_ADC_PIN_MAP(4, ADCChannelId::kChannel5, GPIOPinB15);  // PB15
DECLARE_ADC_PIN_MAP(4, ADCChannelId::kChannel12, GPIOPinD8);  // PD8
DECLARE_ADC_PIN_MAP(4, ADCChannelId::kChannel13, GPIOPinD9);  // PD9

// ADC5
DECLARE_ADC_PIN_MAP(5, ADCChannelId::kChannel1, GPIOPinA8);   // PA8
DECLARE_ADC_PIN_MAP(5, ADCChannelId::kChannel2, GPIOPinA9);   // PA9
DECLARE_ADC_PIN_MAP(5, ADCChannelId::kChannel12, GPIOPinD8);  // PD8
DECLARE_ADC_PIN_MAP(5, ADCChannelId::kChannel13, GPIOPinD9);  // PD9

// =============================================================================
// FORWARD DECLARATIONS
// =============================================================================
class ADCDevice;

template <uint8_t tkADCIdx>
    requires(kValidADCIndex<tkADCIdx>)
class ADCPeripheralDevice;

template <uint8_t tkADCIdx, ADCChannelId tkChannelId>
    requires(kValidADCChannel<tkADCIdx, tkChannelId>)
class ADCChannelDevice;

template <uint8_t tkADCIdx, ADCChannelId tkChannelId, uint8_t tkRank>
    requires(kValidADCChannel<tkADCIdx, tkChannelId> && kValidADCInjectRank<tkRank>)
class ADCInjectChannelDevice;

template <uint8_t tkADCIdx, ADCChannelId tkChannelId, uint8_t tkRank>
    requires(kValidADCChannel<tkADCIdx, tkChannelId> && kValidADCRegularRank<tkRank>)
class ADCRegularChannelDevice;

using ADCValue = uint16_t;

// ============================================================================
// Base ADC DEVICE (INTERFACE DEVICE)
// ============================================================================
/**
 * @brief ADC Device (Interface Device), represents the ADC peripheral family.
 *
 */
class ADCDevice
{
public:
    struct Descriptor : public InterfaceDeviceDescriptor
    {
        using Children = DeviceList<ADCPeripheralDevice<1>,
                                    ADCPeripheralDevice<2>,
                                    ADCPeripheralDevice<3>,
                                    ADCPeripheralDevice<4>,
                                    ADCPeripheralDevice<5>>;
    };

    static constexpr std::array<uint32_t, 4> skInjectRankIdxToRankReg = {
        LL_ADC_INJ_RANK_1, LL_ADC_INJ_RANK_2, LL_ADC_INJ_RANK_3, LL_ADC_INJ_RANK_4};

    static constexpr std::array<uint32_t, 16> skRegularRankIdxToRankReg = {LL_ADC_REG_RANK_1,
                                                                           LL_ADC_REG_RANK_2,
                                                                           LL_ADC_REG_RANK_3,
                                                                           LL_ADC_REG_RANK_4,
                                                                           LL_ADC_REG_RANK_5,
                                                                           LL_ADC_REG_RANK_6,
                                                                           LL_ADC_REG_RANK_7,
                                                                           LL_ADC_REG_RANK_8,
                                                                           LL_ADC_REG_RANK_9,
                                                                           LL_ADC_REG_RANK_10,
                                                                           LL_ADC_REG_RANK_11,
                                                                           LL_ADC_REG_RANK_12,
                                                                           LL_ADC_REG_RANK_13,
                                                                           LL_ADC_REG_RANK_14,
                                                                           LL_ADC_REG_RANK_15,
                                                                           LL_ADC_REG_RANK_16};

    static inline constexpr uint32_t get_regular_sequencer_length(const uint32_t count)
    {
        switch (count)
        {
            case 1:
                return LL_ADC_REG_SEQ_SCAN_DISABLE;
            case 2:
                return LL_ADC_REG_SEQ_SCAN_ENABLE_2RANKS;
            case 3:
                return LL_ADC_REG_SEQ_SCAN_ENABLE_3RANKS;
            case 4:
                return LL_ADC_REG_SEQ_SCAN_ENABLE_4RANKS;
            case 5:
                return LL_ADC_REG_SEQ_SCAN_ENABLE_5RANKS;
            case 6:
                return LL_ADC_REG_SEQ_SCAN_ENABLE_6RANKS;
            case 7:
                return LL_ADC_REG_SEQ_SCAN_ENABLE_7RANKS;
            case 8:
                return LL_ADC_REG_SEQ_SCAN_ENABLE_8RANKS;
            case 9:
                return LL_ADC_REG_SEQ_SCAN_ENABLE_9RANKS;
            case 10:
                return LL_ADC_REG_SEQ_SCAN_ENABLE_10RANKS;
            case 11:
                return LL_ADC_REG_SEQ_SCAN_ENABLE_11RANKS;
            case 12:
                return LL_ADC_REG_SEQ_SCAN_ENABLE_12RANKS;
            case 13:
                return LL_ADC_REG_SEQ_SCAN_ENABLE_13RANKS;
            case 14:
                return LL_ADC_REG_SEQ_SCAN_ENABLE_14RANKS;
            case 15:
                return LL_ADC_REG_SEQ_SCAN_ENABLE_15RANKS;
            case 16:
                return LL_ADC_REG_SEQ_SCAN_ENABLE_16RANKS;
            default:
                return LL_ADC_REG_SEQ_SCAN_DISABLE;
        }
    }

    static inline uint32_t get_inject_sequencer_length(uint32_t count)
    {
        switch (count)
        {
            case 1:
                return LL_ADC_INJ_SEQ_SCAN_DISABLE;
            case 2:
                return LL_ADC_INJ_SEQ_SCAN_ENABLE_2RANKS;
            case 3:
                return LL_ADC_INJ_SEQ_SCAN_ENABLE_3RANKS;
            case 4:
                return LL_ADC_INJ_SEQ_SCAN_ENABLE_4RANKS;
            default:
                return LL_ADC_INJ_SEQ_SCAN_DISABLE;
        }
    }

    static inline constexpr uint32_t inject_rank_to_rank_reg(uint8_t rank)
    {
        return skInjectRankIdxToRankReg[rank - 1];
    }

    static inline constexpr uint8_t inject_rank_reg_to(const uint32_t rank_reg)
    {
        switch (rank_reg)
        {
            case LL_ADC_INJ_RANK_1:
                return 1;
            case LL_ADC_INJ_RANK_2:
                return 2;
            case LL_ADC_INJ_RANK_3:
                return 3;
            case LL_ADC_INJ_RANK_4:
                return 4;
            default:
                return 0;
        }
    }

    static inline constexpr uint32_t regular_rank_to_rank_reg(uint8_t rank)
    {
        return skRegularRankIdxToRankReg[rank - 1];
    }

    static inline constexpr uint8_t regular_rank_reg_to_rank(uint32_t rank)
    {
        switch (rank)
        {
            case LL_ADC_REG_RANK_1:
                return 1;
            case LL_ADC_REG_RANK_2:
                return 2;
            case LL_ADC_REG_RANK_3:
                return 3;
            case LL_ADC_REG_RANK_4:
                return 4;
            case LL_ADC_REG_RANK_5:
                return 5;
            case LL_ADC_REG_RANK_6:
                return 6;
            case LL_ADC_REG_RANK_7:
                return 7;
            case LL_ADC_REG_RANK_8:
                return 8;
            case LL_ADC_REG_RANK_9:
                return 9;
            case LL_ADC_REG_RANK_10:
                return 10;
            case LL_ADC_REG_RANK_11:
                return 11;
            case LL_ADC_REG_RANK_12:
                return 12;
            case LL_ADC_REG_RANK_13:
                return 13;
            case LL_ADC_REG_RANK_14:
                return 14;
            case LL_ADC_REG_RANK_15:
                return 15;
            case LL_ADC_REG_RANK_16:
                return 16;
            default:
                return 0;
        }
    }
};

// ============================================================================
// ADC Rank Traits
// ============================================================================
template <uint8_t tkRank>
struct ADCInjectRankTraits
{
    static constexpr uint32_t skRank = tkRank;
    static constexpr uint32_t skReg  = ADCDevice::inject_rank_to_rank_reg(tkRank);
};

template <uint8_t tkRank>
struct ADCRegularRankTraits
{
    static constexpr uint32_t skRank = tkRank;
    static constexpr uint32_t skReg  = ADCDevice::regular_rank_to_rank_reg(tkRank);
};

// ============================================================================
// ADC PERIPHERAL (SHARED DEVICE)
// ============================================================================

enum class ADCResolution : uint32_t
{
    k12Bit = LL_ADC_RESOLUTION_12B,
    k10Bit = LL_ADC_RESOLUTION_10B,
    k8Bit  = LL_ADC_RESOLUTION_8B,
    k6Bit  = LL_ADC_RESOLUTION_6B,
};

enum class ADCDataAlignment : uint32_t
{
    kRight = LL_ADC_DATA_ALIGN_RIGHT,
    kLeft  = LL_ADC_DATA_ALIGN_LEFT,
};

enum class ADCLowPowerMode : uint32_t
{
    kNone     = LL_ADC_LP_MODE_NONE,  /// No low power
    kAutoWait = LL_ADC_LP_AUTOWAIT,   /// Auto wait between conversions
};

enum class ADCInjectGroupTriggerSource : uint32_t
{
    kSoftware      = LL_ADC_INJ_TRIG_SOFTWARE,
    kExtTim1TRGO   = LL_ADC_INJ_TRIG_EXT_TIM1_TRGO,
    kExtTim1TRGO2  = LL_ADC_INJ_TRIG_EXT_TIM1_TRGO2,
    kExtTim1CH3    = LL_ADC_INJ_TRIG_EXT_TIM1_CH3,
    kExtTim1CH4    = LL_ADC_INJ_TRIG_EXT_TIM1_CH4,
    kExtTim2TRGO   = LL_ADC_INJ_TRIG_EXT_TIM2_TRGO,
    kExtTim2CH1    = LL_ADC_INJ_TRIG_EXT_TIM2_CH1,
    kExtTim3TRGO   = LL_ADC_INJ_TRIG_EXT_TIM3_TRGO,
    kExtTim3CH1    = LL_ADC_INJ_TRIG_EXT_TIM3_CH1,
    kExtTim3CH3    = LL_ADC_INJ_TRIG_EXT_TIM3_CH3,
    kExtTim3CH4    = LL_ADC_INJ_TRIG_EXT_TIM3_CH4,
    kExtTim4TRGO   = LL_ADC_INJ_TRIG_EXT_TIM4_TRGO,
    kExtTim4CH3    = LL_ADC_INJ_TRIG_EXT_TIM4_CH3,
    kExtTim4CH4    = LL_ADC_INJ_TRIG_EXT_TIM4_CH4,
    kExtTim6TRGO   = LL_ADC_INJ_TRIG_EXT_TIM6_TRGO,
    kExtTim7TRGO   = LL_ADC_INJ_TRIG_EXT_TIM7_TRGO,
    kExtTim8TRGO   = LL_ADC_INJ_TRIG_EXT_TIM8_TRGO,
    kExtTim8TRGO2  = LL_ADC_INJ_TRIG_EXT_TIM8_TRGO2,
    kExtTim8CH2    = LL_ADC_INJ_TRIG_EXT_TIM8_CH2,
    kExtTim8CH4    = LL_ADC_INJ_TRIG_EXT_TIM8_CH4,
    kExtTim15TRGO  = LL_ADC_INJ_TRIG_EXT_TIM15_TRGO,
    kExtTim16CH1   = LL_ADC_INJ_TRIG_EXT_TIM16_CH1,
    kExtTim20TRGO  = LL_ADC_INJ_TRIG_EXT_TIM20_TRGO,
    kExtTim20TRGO2 = LL_ADC_INJ_TRIG_EXT_TIM20_TRGO2,
    kExtTim20CH2   = LL_ADC_INJ_TRIG_EXT_TIM20_CH2,
    kExtTim20CH4   = LL_ADC_INJ_TRIG_EXT_TIM20_CH4,
    kExtHrtimTRG1  = LL_ADC_INJ_TRIG_EXT_HRTIM_TRG1,
    kExtHrtimTRG2  = LL_ADC_INJ_TRIG_EXT_HRTIM_TRG2,
    kExtHrtimTRG3  = LL_ADC_INJ_TRIG_EXT_HRTIM_TRG3,
    kExtHrtimTRG4  = LL_ADC_INJ_TRIG_EXT_HRTIM_TRG4,
    kExtHrtimTRG5  = LL_ADC_INJ_TRIG_EXT_HRTIM_TRG5,
    kExtHrtimTRG6  = LL_ADC_INJ_TRIG_EXT_HRTIM_TRG6,
    kExtHrtimTRG7  = LL_ADC_INJ_TRIG_EXT_HRTIM_TRG7,
    kExtHrtimTRG8  = LL_ADC_INJ_TRIG_EXT_HRTIM_TRG8,
    kExtHrtimTRG9  = LL_ADC_INJ_TRIG_EXT_HRTIM_TRG9,
    kExtHrtimTRG10 = LL_ADC_INJ_TRIG_EXT_HRTIM_TRG10,
    kExtExtiLine3  = LL_ADC_INJ_TRIG_EXT_EXTI_LINE3,
    kExtExtiLine15 = LL_ADC_INJ_TRIG_EXT_EXTI_LINE15,
    kExtLptimOut   = LL_ADC_INJ_TRIG_EXT_LPTIM_OUT,
};

enum class ADCInjectGroupTriggerEdge : uint32_t
{
    kRising        = LL_ADC_INJ_TRIG_EXT_RISING,
    kFalling       = LL_ADC_INJ_TRIG_EXT_FALLING,
    kRisingFalling = LL_ADC_INJ_TRIG_EXT_RISINGFALLING
};

enum class ADCRegularGroupTriggerSource : uint32_t
{
    kSoftware      = LL_ADC_REG_TRIG_SOFTWARE,
    kExtTim1TRGO   = LL_ADC_REG_TRIG_EXT_TIM1_TRGO,
    kExtTim1TRGO2  = LL_ADC_REG_TRIG_EXT_TIM1_TRGO2,
    kExtTim1CH1    = LL_ADC_REG_TRIG_EXT_TIM1_CH1,
    kExtTim1CH2    = LL_ADC_REG_TRIG_EXT_TIM1_CH2,
    kExtTim1CH3    = LL_ADC_REG_TRIG_EXT_TIM1_CH3,
    kExtTim2TRGO   = LL_ADC_REG_TRIG_EXT_TIM2_TRGO,
    kExtTim2CH1    = LL_ADC_REG_TRIG_EXT_TIM2_CH1,
    kExtTim2CH2    = LL_ADC_REG_TRIG_EXT_TIM2_CH2,
    kExtTim2CH3    = LL_ADC_REG_TRIG_EXT_TIM2_CH3,
    kExtTim3TRGO   = LL_ADC_REG_TRIG_EXT_TIM3_TRGO,
    kExtTim3CH1    = LL_ADC_REG_TRIG_EXT_TIM3_CH1,
    kExtTim3CH4    = LL_ADC_REG_TRIG_EXT_TIM3_CH4,
    kExtTim4TRGO   = LL_ADC_REG_TRIG_EXT_TIM4_TRGO,
    kExtTim4CH1    = LL_ADC_REG_TRIG_EXT_TIM4_CH1,
    kExtTim4CH4    = LL_ADC_REG_TRIG_EXT_TIM4_CH4,
    kExtTim6TRGO   = LL_ADC_REG_TRIG_EXT_TIM6_TRGO,
    kExtTim7TRGO   = LL_ADC_REG_TRIG_EXT_TIM7_TRGO,
    kExtTim8TRGO   = LL_ADC_REG_TRIG_EXT_TIM8_TRGO,
    kExtTim8TRGO2  = LL_ADC_REG_TRIG_EXT_TIM8_TRGO2,
    kExtTim8CH1    = LL_ADC_REG_TRIG_EXT_TIM8_CH1,
    kExtTim15TRGO  = LL_ADC_REG_TRIG_EXT_TIM15_TRGO,
    kExtTim20TRGO  = LL_ADC_REG_TRIG_EXT_TIM20_TRGO,
    kExtTim20TRGO2 = LL_ADC_REG_TRIG_EXT_TIM20_TRGO2,
    kExtTim20CH1   = LL_ADC_REG_TRIG_EXT_TIM20_CH1,
    kExtTim20CH2   = LL_ADC_REG_TRIG_EXT_TIM20_CH2,
    kExtTim20CH3   = LL_ADC_REG_TRIG_EXT_TIM20_CH3,
    kExtHrtimTRG1  = LL_ADC_REG_TRIG_EXT_HRTIM_TRG1,
    kExtHrtimTRG2  = LL_ADC_REG_TRIG_EXT_HRTIM_TRG2,
    kExtHrtimTRG3  = LL_ADC_REG_TRIG_EXT_HRTIM_TRG3,
    kExtHrtimTRG4  = LL_ADC_REG_TRIG_EXT_HRTIM_TRG4,
    kExtHrtimTRG5  = LL_ADC_REG_TRIG_EXT_HRTIM_TRG5,
    kExtHrtimTRG6  = LL_ADC_REG_TRIG_EXT_HRTIM_TRG6,
    kExtHrtimTRG7  = LL_ADC_REG_TRIG_EXT_HRTIM_TRG7,
    kExtHrtimTRG8  = LL_ADC_REG_TRIG_EXT_HRTIM_TRG8,
    kExtHrtimTRG9  = LL_ADC_REG_TRIG_EXT_HRTIM_TRG9,
    kExtHrtimTRG10 = LL_ADC_REG_TRIG_EXT_HRTIM_TRG10,
    kExtExtiLine11 = LL_ADC_REG_TRIG_EXT_EXTI_LINE11,
    kExtExtiLine2  = LL_ADC_REG_TRIG_EXT_EXTI_LINE2,
    kExtLptimOut   = LL_ADC_REG_TRIG_EXT_LPTIM_OUT,
};

enum class ADCRegularGroupTriggerEdge : uint32_t
{
    kRising        = LL_ADC_REG_TRIG_EXT_RISING,
    kFalling       = LL_ADC_REG_TRIG_EXT_FALLING,
    kRisingFalling = LL_ADC_REG_TRIG_EXT_RISINGFALLING
};

enum class ADCRegularGroupDMATransfer : uint32_t
{
    kNone      = LL_ADC_REG_DMA_TRANSFER_NONE,
    kUnlimited = LL_ADC_REG_DMA_TRANSFER_UNLIMITED,
    kLimited   = LL_ADC_REG_DMA_TRANSFER_LIMITED
};

enum class ADCRegularGroupOverrunBehavior : uint32_t
{
    kPreserve  = LL_ADC_REG_OVR_DATA_PRESERVED,
    kOverwrite = LL_ADC_REG_OVR_DATA_OVERWRITTEN,
};

enum class ADCRegularGroupConversionMode : uint32_t
{
    kSingleShot = LL_ADC_REG_CONV_SINGLE,
    kContinuous = LL_ADC_REG_CONV_CONTINUOUS
};

enum class ADCRegularGroupOversamplingMode : uint32_t
{
    kDiscontinuous = LL_ADC_OVS_REG_DISCONT,
    kContinuous    = LL_ADC_OVS_REG_CONT
};

enum class ADCOversamplingRatio : uint32_t
{
    k2x   = LL_ADC_OVS_RATIO_2,
    k4x   = LL_ADC_OVS_RATIO_4,
    k8x   = LL_ADC_OVS_RATIO_8,
    k16x  = LL_ADC_OVS_RATIO_16,
    k32x  = LL_ADC_OVS_RATIO_32,
    k64x  = LL_ADC_OVS_RATIO_64,
    k128x = LL_ADC_OVS_RATIO_128,
    k256x = LL_ADC_OVS_RATIO_256
};

enum class ADCOversamplingShiftBits : uint32_t
{
    kNone   = LL_ADC_OVS_SHIFT_NONE,
    kRight1 = LL_ADC_OVS_SHIFT_RIGHT_1,
    kRight2 = LL_ADC_OVS_SHIFT_RIGHT_2,
    kRight3 = LL_ADC_OVS_SHIFT_RIGHT_3,
    kRight4 = LL_ADC_OVS_SHIFT_RIGHT_4,
    kRight5 = LL_ADC_OVS_SHIFT_RIGHT_5,
    kRight6 = LL_ADC_OVS_SHIFT_RIGHT_6,
    kRight7 = LL_ADC_OVS_SHIFT_RIGHT_7,
    kRight8 = LL_ADC_OVS_SHIFT_RIGHT_8
};

enum class ADCOversamplingScope : uint32_t
{
    kDisable              = LL_ADC_OVS_DISABLE,
    kRegularContinued     = LL_ADC_OVS_GRP_REGULAR_CONTINUED,
    kRegularResumed       = LL_ADC_OVS_GRP_REGULAR_RESUMED,
    kInject               = LL_ADC_OVS_GRP_INJECTED,
    kInjectRegularResumed = LL_ADC_OVS_GRP_INJ_REG_RESUMED
};

/**
 * @brief Configuration for ADC Injected Group
 *
 */
struct ADCInjectGroupConfig
{
    ADCInjectGroupTriggerSource trigger_source;  /// What triggers the inject group (software or external)
    ADCInjectGroupTriggerEdge   trigger_edge;    /// What edge to trigger on (only for external triggers)

    [[nodiscard]] static constexpr ADCInjectGroupConfig default_config()
    {
        return ADCInjectGroupConfig{
            .trigger_source = ADCInjectGroupTriggerSource::kSoftware,
            .trigger_edge   = ADCInjectGroupTriggerEdge::kRising,
        };
    }
};

/**
 * @brief Configuration for ADC Regular Group (Background/DMA).
 *
 */
struct ADCRegularGroupConfig
{
    ADCRegularGroupTriggerSource    trigger_source;     /// Source of trigger (Software/External)
    ADCRegularGroupTriggerEdge      trigger_edge;       /// What edge causes trigger (Rising/Falling/Both)
    ADCRegularGroupDMATransfer      dma_transfer;       /// DMA config: Circular (Buffer) vs Limited
    ADCRegularGroupOverrunBehavior  overrun;            /// Overrun behavior: Preserve vs Overwrite
    ADCRegularGroupConversionMode   conversion_mode;    /// Conversion mode: Single Shot vs Continuous
    ADCRegularGroupOversamplingMode oversampling_mode;  /// Oversampling mode: Continuous vs discontinuous

    [[nodiscard]] static constexpr ADCRegularGroupConfig default_config()
    {
        return ADCRegularGroupConfig{.trigger_source    = ADCRegularGroupTriggerSource::kSoftware,
                                     .trigger_edge      = ADCRegularGroupTriggerEdge::kRising,
                                     .dma_transfer      = ADCRegularGroupDMATransfer::kUnlimited,
                                     .overrun           = ADCRegularGroupOverrunBehavior::kOverwrite,
                                     .conversion_mode   = ADCRegularGroupConversionMode::kSingleShot,
                                     .oversampling_mode = ADCRegularGroupOversamplingMode::kDiscontinuous};
    }
};

/**
 * @brief Configuration for ADC Oversampling (Hardware Averaging).
 *
 */
struct ADCOversamplingConfig
{
    ADCOversamplingRatio     ratio;  /// How many times to sample (2x to 256x)
    ADCOversamplingShiftBits shift;  /// Right shift bits (0 to 8)
    ADCOversamplingScope     scope;  /// Oversampling behavior when both groups are selected
};

/**
 * @brief Configuration for the ADC Peripheral (Global).
 */
struct ADCPeripheralConfig
{
    // --- Core Settings ---
    ADCResolution    resolution;      // Sampling resolution (12, 10, 8, 6 bits)
    ADCDataAlignment data_alignment;  // Right/Left data alignment
    ADCLowPowerMode  low_power;       // Low power mode

    // --- Inject Group (High Priority/Interrupt) ---
    ADCInjectGroupConfig inj;

    // --- Regular Group (Background/DMA) ---
    ADCRegularGroupConfig reg;

    // --- Oversampling (Hardware Averaging) ---
    std::optional<ADCOversamplingConfig> oversampling;

    [[nodiscard]] static constexpr ADCPeripheralConfig default_config()
    {
        return ADCPeripheralConfig{
            .resolution     = ADCResolution::k12Bit,
            .data_alignment = ADCDataAlignment::kRight,
            .low_power      = ADCLowPowerMode::kNone,
            .inj            = ADCInjectGroupConfig::default_config(),
            .reg            = ADCRegularGroupConfig::default_config(),
            .oversampling   = std::nullopt  // No oversampling by default
        };
    }
};

enum class ADCChannelGroup
{
    kRegular,
    kInject
};

enum class ADCChannelSampleTime
{
    k2Cycles5   = LL_ADC_SAMPLINGTIME_2CYCLES_5,    /// 2.5 ADC clock cycles
    k6Cycles5   = LL_ADC_SAMPLINGTIME_6CYCLES_5,    /// 6.5 ADC clock cycles
    k12Cycles5  = LL_ADC_SAMPLINGTIME_12CYCLES_5,   /// 12.5 ADC clock cycles
    k24Cycles5  = LL_ADC_SAMPLINGTIME_24CYCLES_5,   /// 24.5 ADC clock cycles
    k47Cycles5  = LL_ADC_SAMPLINGTIME_47CYCLES_5,   /// 47.5 ADC clock cycles
    k92Cycles5  = LL_ADC_SAMPLINGTIME_92CYCLES_5,   /// 92.5 ADC clock cycles
    k247Cycles5 = LL_ADC_SAMPLINGTIME_247CYCLES_5,  /// 247.5 ADC clock cycles
    k640Cycles5 = LL_ADC_SAMPLINGTIME_640CYCLES_5   /// 640.5 ADC clock cycles
};

enum class ADCChannelInputMode
{
    kSingleEnded  = LL_ADC_SINGLE_ENDED,       /// Single Ended Input
    kDifferential = LL_ADC_DIFFERENTIAL_ENDED  /// Differential Input
};

enum class ADCChannelOffsetIdx
{
    kOffset1 = LL_ADC_OFFSET_1,  /// Offset 1
    kOffset2 = LL_ADC_OFFSET_2,  /// Offset 2
    kOffset3 = LL_ADC_OFFSET_3,  /// Offset 3
    kOffset4 = LL_ADC_OFFSET_4   /// Offset 4
};

struct ADCChannelOffsetConfig
{
    ADCChannelOffsetIdx idx;    /// Offset Index (1 to 4)
    uint32_t            value;  /// Offset Value (Raw ADC counts)
};

/**
 * @brief Configuration for a single ADC Channel (Physics).
 */
struct ADCChannelConfig
{
    ADCChannelSampleTime                  sampling_time;  /// Sampling time
    ADCChannelInputMode                   input_mode;     /// Input mode (Single-Ended/Differential)
    std::optional<ADCChannelOffsetConfig> offset;         /// Optional offset configuration

    [[nodiscard]] static constexpr ADCChannelConfig default_config()
    {
        return ADCChannelConfig{
            .sampling_time = ADCChannelSampleTime::k24Cycles5,
            .input_mode    = ADCChannelInputMode::kSingleEnded,
            .offset        = std::nullopt  // No offset by default
        };
    };
};

template <uint8_t tkADCIdx>
    requires(kValidADCIndex<tkADCIdx>)
class ADCPeripheralDevice
{
public:
    struct Descriptor : public SharedDeviceDescriptor
    {
        using Children = DeviceList<ADCChannelDevice<tkADCIdx, ADCChannelId::kChannel0>,
                                    ADCChannelDevice<tkADCIdx, ADCChannelId::kChannel1>,
                                    ADCChannelDevice<tkADCIdx, ADCChannelId::kChannel2>,
                                    ADCChannelDevice<tkADCIdx, ADCChannelId::kChannel3>,
                                    ADCChannelDevice<tkADCIdx, ADCChannelId::kChannel4>,
                                    ADCChannelDevice<tkADCIdx, ADCChannelId::kChannel5>,
                                    ADCChannelDevice<tkADCIdx, ADCChannelId::kChannel6>,
                                    ADCChannelDevice<tkADCIdx, ADCChannelId::kChannel7>,
                                    ADCChannelDevice<tkADCIdx, ADCChannelId::kChannel8>,
                                    ADCChannelDevice<tkADCIdx, ADCChannelId::kChannel9>,
                                    ADCChannelDevice<tkADCIdx, ADCChannelId::kChannel10>,
                                    ADCChannelDevice<tkADCIdx, ADCChannelId::kChannel11>,
                                    ADCChannelDevice<tkADCIdx, ADCChannelId::kChannel12>,
                                    ADCChannelDevice<tkADCIdx, ADCChannelId::kChannel13>,
                                    ADCChannelDevice<tkADCIdx, ADCChannelId::kChannel14>,
                                    ADCChannelDevice<tkADCIdx, ADCChannelId::kChannel15>,
                                    ADCChannelDevice<tkADCIdx, ADCChannelId::kChannel16>,
                                    ADCChannelDevice<tkADCIdx, ADCChannelId::kChannel17>,
                                    ADCChannelDevice<tkADCIdx, ADCChannelId::kChannel18>,
                                    ADCChannelDevice<tkADCIdx, ADCChannelId::kChannelVRefInt>,
                                    ADCChannelDevice<tkADCIdx, ADCChannelId::kChannelTempSensorADC1>,
                                    ADCChannelDevice<tkADCIdx, ADCChannelId::kChannelTempSensorADC5>,
                                    ADCChannelDevice<tkADCIdx, ADCChannelId::kChannelVBat>,
                                    ADCChannelDevice<tkADCIdx, ADCChannelId::kChannelVOPAmp1>,
                                    ADCChannelDevice<tkADCIdx, ADCChannelId::kChannelVOPAmp2>,
                                    ADCChannelDevice<tkADCIdx, ADCChannelId::kChannelVOPAmp3ADC2>,
                                    ADCChannelDevice<tkADCIdx, ADCChannelId::kChannelVOPAmp3ADC3>,
                                    ADCChannelDevice<tkADCIdx, ADCChannelId::kChannelVOPAmp4>,
                                    ADCChannelDevice<tkADCIdx, ADCChannelId::kChannelVOPAmp5>,
                                    ADCChannelDevice<tkADCIdx, ADCChannelId::kChannelVOPAmp6>>;
    };
    using DependDevices = TypeList<ADCDevice>;
    using Traits        = ADCTraits<tkADCIdx>;

    static constexpr size_t kMaxRegChannels = 16;
    static constexpr size_t kMaxInjChannels = 4;
    static constexpr size_t kMaxChannelId   = 20;

private:
    static constexpr uint8_t   skChannelRankFreeFlag = 0xFF;
    static inline ADC_TypeDef* skADCInstance         = Traits::skInstance;

    // --- Storage ---
    alignas(32) volatile ADCValue m_dma_buffer[kMaxRegChannels];

    // Mappings
    uint8_t m_reg_cidx_to_rank_map[kMaxChannelId];  // Ch -> Rank (1-16) (rank-1 for dma idx)
    uint8_t m_inj_cidx_to_rank_map[kMaxChannelId];  // Ch -> Rank (1-4)

    ADCPeripheralConfig m_config;

public:
    ADCPeripheralDevice()
    {
        std::fill(std::begin(m_reg_cidx_to_rank_map), std::end(m_reg_cidx_to_rank_map), skChannelRankFreeFlag);
        std::fill(std::begin(m_inj_cidx_to_rank_map), std::end(m_inj_cidx_to_rank_map), 0);
        std::fill(std::begin(m_dma_buffer), std::end(m_dma_buffer), 0);
    }

    // --- Initialization (Called by Main/Builder) ---

    /**
     * @brief Initialize ADC Peripheral with given configuration.
     *
     * @param config ADC Peripheral Configuration.
     */
    void init(const ADCPeripheralConfig& config)
    {
        m_config = config;

        // Clock & Power
        LL_AHB2_GRP1_EnableClock(Traits::skClock);

        if (LL_ADC_IsEnabled(skADCInstance) == 0)
        {
            LL_ADC_DisableDeepPowerDown(skADCInstance);
            LL_ADC_EnableInternalRegulator(skADCInstance);
            volatile uint32_t wait = 5000;
            while (wait > 0)
            {
                wait = wait - 1;
            }
        }

        // Calibration
        LL_ADC_StartCalibration(skADCInstance, LL_ADC_SINGLE_ENDED);
        while (LL_ADC_IsCalibrationOnGoing(skADCInstance));

        // Global Configuration
        LL_ADC_SetResolution(skADCInstance, static_cast<uint32_t>(m_config.resolution));
        LL_ADC_SetDataAlignment(skADCInstance, static_cast<uint32_t>(m_config.data_alignment));
        LL_ADC_SetLowPowerMode(skADCInstance, static_cast<uint32_t>(m_config.low_power));

        // Inject Group Configuration
        LL_ADC_INJ_SetTriggerSource(skADCInstance, static_cast<uint32_t>(m_config.inj.trigger_source));
        LL_ADC_INJ_SetTriggerEdge(skADCInstance, static_cast<uint32_t>(m_config.inj.trigger_edge));
        LL_ADC_INJ_SetTrigAuto(skADCInstance, LL_ADC_INJ_TRIG_INDEPENDENT);  // Auto trigger not supported
        LL_ADC_INJ_SetQueueMode(skADCInstance, LL_ADC_INJ_QUEUE_DISABLE);    // Queue mode not supported

        // Regular Group Configuration
        LL_ADC_REG_SetTriggerSource(skADCInstance, static_cast<uint32_t>(m_config.reg.trigger_source));
        LL_ADC_REG_SetTriggerEdge(skADCInstance, static_cast<uint32_t>(m_config.reg.trigger_edge));
        LL_ADC_REG_SetDMATransfer(skADCInstance, static_cast<uint32_t>(m_config.reg.dma_transfer));
        LL_ADC_REG_SetOverrun(skADCInstance, static_cast<uint32_t>(m_config.reg.overrun));
        LL_ADC_REG_SetContinuousMode(skADCInstance, static_cast<uint32_t>(m_config.reg.conversion_mode));

        // Oversampling Configuration
        if (m_config.oversampling.has_value())
        {
            ADCOversamplingConfig ovs_cfg = m_config.oversampling.value();
            LL_ADC_SetOverSamplingScope(skADCInstance, static_cast<uint32_t>(ovs_cfg.scope));
            LL_ADC_ConfigOverSamplingRatioShift(
                skADCInstance, static_cast<uint32_t>(ovs_cfg.ratio), static_cast<uint32_t>(ovs_cfg.shift));
            LL_ADC_SetOverSamplingDiscont(skADCInstance, static_cast<uint32_t>(m_config.reg.oversampling_mode));
        }
    }

    // --- Registration API (Called by Channel) ---
    /**
     * @brief Initialize ADC Channel with given configuration and group/rank.
     *
     * @tparam tkChannelId Channel ID to initialize.
     * @tparam tkGroup Channel Group (Regular/Inject).
     * @tparam tkRank Rank in Sequence.
     */
    template <ADCChannelId tkChannelId, ADCChannelGroup tkGroup, uint8_t tkRank>
        requires(kValidADCChannel<tkADCIdx, tkChannelId>)
    auto init_channel(const ADCChannelConfig& config)
    {
        if constexpr (tkGroup == ADCChannelGroup::kRegular)
        {
            init_channel_as_regular<tkChannelId, tkRank>(config);
        }
        else if constexpr (tkGroup == ADCChannelGroup::kInject)
        {
            init_channel_as_inject<tkChannelId, tkRank>(config);
        }
        else
        {
            static_assert(false, "Unsupported ADC Channel Group");
        }
    }

    /**
     * @brief Initialize ADC channel as an Inject Channel.
     *
     * @tparam tkChannelId Channel ID to initialize.
     * @tparam tkRank Rank in Inject Sequence (1-4).
     */
    template <ADCChannelId tkChannelId, uint8_t tkRank>
        requires(kValidADCChannel<tkADCIdx, tkChannelId> && kValidADCInjectRank<tkRank>)
    void init_channel_as_inject(const ADCChannelConfig& cfg)
    {
        init_channel_core<tkChannelId>(cfg);
        return register_inject_sequence<tkChannelId, tkRank>();
    }

    /**
     * @brief Initialize ADC channel as a Regular Channel.
     *
     * @tparam tkChannelId Channel ID to initialize.
     * @tparam tkRank Rank in Regular Sequence (1-16).
     */
    template <ADCChannelId tkChannelId, uint8_t tkRank>
        requires(kValidADCChannel<tkADCIdx, tkChannelId> && kValidADCRegularRank<tkRank>)
    void init_channel_as_regular(const ADCChannelConfig& cfg)
    {
        init_channel_core<tkChannelId>(cfg);
        return register_regular_sequence<tkChannelId, tkRank>();
    }

    /**
     * @brief Post init called after init and channel registration. Sets up sequences and starts ADC.
     *
     */
    void post_init(const bool start_inject_group = false, const bool start_regular_group = false)
    {
        const uint8_t reg_count = std::find_if(std::begin(m_reg_cidx_to_rank_map),
                                               std::end(m_reg_cidx_to_rank_map),
                                               [](uint8_t v) { return v == skChannelRankFreeFlag; }) -
                                  std::begin(m_reg_cidx_to_rank_map);

        // Regular Sequence Config
        if (reg_count > 0)
        {
            LL_ADC_REG_SetSequencerLength(skADCInstance, ADCDevice::get_regular_sequencer_length(reg_count));

            for (uint32_t i = 0; i < kMaxChannelId; ++i)
            {
                if (m_reg_cidx_to_rank_map[i] != skChannelRankFreeFlag)
                {
                    const uint32_t ch = __LL_ADC_DECIMAL_NB_TO_CHANNEL(i);
                    LL_ADC_REG_SetSequencerRanks(
                        skADCInstance, ADCDevice::regular_rank_to_rank_reg(m_reg_cidx_to_rank_map[i]), ch);
                }
            }
        }

        const uint8_t inj_count = std::find_if(std::begin(m_inj_cidx_to_rank_map),
                                               std::end(m_inj_cidx_to_rank_map),
                                               [](uint8_t v) { return v == 0; }) -
                                  std::begin(m_inj_cidx_to_rank_map);

        // Inject Sequence Config
        if (inj_count > 0)
        {
            LL_ADC_INJ_SetSequencerLength(skADCInstance, ADCDevice::get_inject_sequencer_length(inj_count));

            for (uint32_t i = 0; i < kMaxChannelId; ++i)
            {
                if (m_inj_cidx_to_rank_map[i] != 0)
                {
                    const uint32_t ch = __LL_ADC_DECIMAL_NB_TO_CHANNEL(i);
                    LL_ADC_INJ_SetSequencerRanks(
                        skADCInstance, ADCDevice::inject_rank_to_rank_reg(m_inj_cidx_to_rank_map[i]), ch);
                }
            }
        }

        // Enable
        LL_ADC_Enable(skADCInstance);
        while (!LL_ADC_IsActiveFlag_ADRDY(skADCInstance));

        if (start_inject_group)
        {
            start_inject();
        }

        if (start_regular_group)
        {
            start_regular();
        }
    }

    /**
     * @brief Arms the Inject Sequence.
     * @note Must be called AFTER ADC is enabled (post_init).
     */
    static void start_inject()
    {
        ack_inject();

        // Enable Inject End of Sequence Interrupt (JEOS)
        // This allows the Vector Table to jump to ADC1_2_IRQHandler
        LL_ADC_EnableIT_JEOS(skADCInstance);

        // ARM THE TRIGGER (The most important part)
        // If Trigger is Hardware (HRTIM): ADC goes into "Waiting for Trigger" state.
        // If Trigger is Software: ADC converts immediately.
        trigger_inject();
    }

    static void ack_inject()
    {
        // Clear any pending flags to avoid immediate false triggers
        LL_ADC_ClearFlag_JEOS(skADCInstance);
    }

    /**
     * @brief Rearm Inject Conversions.
     *
     */
    static void trigger_inject()
    {
        // ARM THE TRIGGER (The most important part)
        // If Trigger is Hardware (HRTIM): ADC goes into "Waiting for Trigger" state.
        // If Trigger is Software: ADC converts immediately.
        LL_ADC_INJ_StartConversion(skADCInstance);
    }

    /**
     * @brief Stop Inject Conversions.
     *
     */
    static void stop_inject()
    {
        LL_ADC_INJ_StopConversion(skADCInstance);
    }

    /**
     * @brief Arms the Regular Sequence.
     *
     */
    static void start_regular()
    {
        // TODO: start DMA

        // Clear any pending flags to avoid immediate false triggers
        ack_regular();

        // Don't need to enable interrupt since its handled by DMA

        // ARM THE TRIGGER (The most important part)
        // If Trigger is Hardware (HRTIM): ADC goes into "Waiting for Trigger" state.
        // If Trigger is Software: ADC converts immediately.
        trigger_regular();
    }

    /**
     * @brief Acknowledge Regular Conversion Complete.
     *
     */
    static void ack_regular()
    {
        // Clear any pending flags to avoid immediate false triggers
        LL_ADC_ClearFlag_EOS(skADCInstance);
    }

    /**
     * @brief Rearm Regular Conversions.
     *
     */
    static void trigger_regular()
    {
        // ARM THE TRIGGER (The most important part)
        // If Trigger is Hardware (HRTIM): ADC goes into "Waiting for Trigger" state.
        // If Trigger is Software: ADC converts immediately.
        LL_ADC_REG_StartConversion(skADCInstance);
    }

    /**
     * @brief Stop Regular Conversions.
     *
     */
    static void stop_regular()
    {
        LL_ADC_REG_StopConversion(skADCInstance);
    }

    // --- Resolution Info ---
    [[nodiscard]] static inline float get_resolution_bits()
    {
        switch (LL_ADC_GetResolution(skADCInstance))
        {
            case LL_ADC_RESOLUTION_12B:
                return 12.0f;
            case LL_ADC_RESOLUTION_10B:
                return 10.0f;
            case LL_ADC_RESOLUTION_8B:
                return 8.0f;
            case LL_ADC_RESOLUTION_6B:
                return 6.0f;
            default:
                return 12.0f;
        }
    }

    [[nodiscard]] static inline float get_resolution_range()
    {
        switch (LL_ADC_GetResolution(skADCInstance))
        {
            case LL_ADC_RESOLUTION_12B:
                return 4095.0f;
            case LL_ADC_RESOLUTION_10B:
                return 1023.0f;
            case LL_ADC_RESOLUTION_8B:
                return 255.0f;
            case LL_ADC_RESOLUTION_6B:
                return 63.0f;
            default:
                return 4095.0f;  // assume 12-bit
        }
    }

    // --- Data Access ---
    /**
     * @brief Read data from Regular Channel.
     *
     * @tparam tkRank Rank in Regular Sequence (1-16).
     * @return ADCValue Raw ADC value.
     */
    template <uint8_t tkRank>
        requires(kValidADCInjectRank<tkRank>)
    [[nodiscard]] constexpr inline ADCValue read_inject_data() const
    {
        return LL_ADC_INJ_ReadConversionData32(skADCInstance, ADCDevice::inject_rank_to_rank_reg(tkRank));
    }

    /**
     * @brief Read data from Inject Channel (Slow).
     *
     * @tparam tkChannelId Channel ID to read.
     * @return ADCValue Raw ADC value.
     */
    template <ADCChannelId tkChannelId>
    [[nodiscard]] constexpr inline ADCValue read_inject_data_slow()
    {
        return read_inject_data(get_adc_channel_rank_inject<tkChannelId>());
    }

    /**
     * @brief Read data from Regular Channel.
     *
     * @tparam tkRank Rank in Regular Sequence (1-16).
     * @return ADCValue Raw ADC value.
     */
    template <uint8_t tkRank>
        requires(kValidADCRegularRank<tkRank>)
    [[nodiscard]] constexpr inline ADCValue read_regular_data() const
    {
        return read_regular_data(tkRank);
    }

    /**
     * @brief Read data from Regular Channel (Slow).
     *
     * @tparam tkChannelId Channel ID to read.
     * @return ADCValue Raw ADC value.
     */
    template <ADCChannelId tkChannelId>
    [[nodiscard]] constexpr inline ADCValue read_regular_data_slow()
    {
        return read_regular_data(get_adc_channel_rank_regular<tkChannelId>());
    }

private:
    /**
     * @brief Get the ADC Channel Index from Channel ID.
     *
     * @tparam tkChannelId Channel ID.
     * @return uint32_t Channel Index.
     */
    template <ADCChannelId tkChannelId>
    [[nodiscard]] static inline uint32_t get_adc_channel_idx()
    {
        return __LL_ADC_CHANNEL_TO_DECIMAL_NB(static_cast<uint32_t>(tkChannelId));
    }

    /**
     * @brief Get the ADC Channel Rank in Regular Sequence.
     *
     * @tparam tkChannelId The ID of the channel.
     * @return uint8_t Rank (1-16).
     */
    template <ADCChannelId tkChannelId>
    [[nodiscard]] constexpr inline uint8_t get_adc_channel_rank_regular() const
    {
        const uint32_t idx = get_adc_channel_idx<tkChannelId>();
        return m_reg_cidx_to_rank_map[idx];
    }

    /**
     * @brief Get the ADC Channel Rank in Inject Sequence.
     *
     * @tparam tkChannelId The ID of the channel.
     * @return uint8_t Rank (1-4).
     */
    template <ADCChannelId tkChannelId>
    [[nodiscard]] constexpr inline uint8_t get_adc_channel_rank_inject() const
    {
        const uint32_t idx = get_adc_channel_idx<tkChannelId>();
        return m_inj_cidx_to_rank_map[idx];
    }

    /**
     * @brief Configure and register this channel.
     *
     * @tparam tkChannelId The ID of the channel to configure.
     * @param cfg        Physics configuration (Sampling time, Single/Diff, etc.)
     */
    template <ADCChannelId tkChannelId>
        requires(kValidADCChannel<tkADCIdx, tkChannelId>)
    void init_channel_core(const ADCChannelConfig& cfg)
    {
        LL_ADC_SetChannelSamplingTime(
            skADCInstance, static_cast<uint32_t>(tkChannelId), static_cast<uint32_t>(cfg.sampling_time));
        LL_ADC_SetChannelSingleDiff(
            skADCInstance, static_cast<uint32_t>(tkChannelId), static_cast<uint32_t>(cfg.input_mode));

        if (cfg.offset.has_value())
        {
            ADCChannelOffsetConfig offset_cfg = cfg.offset.value();

            LL_ADC_SetOffset(skADCInstance,
                             static_cast<uint32_t>(offset_cfg.idx),
                             static_cast<uint32_t>(tkChannelId),
                             offset_cfg.value);
        }
    }

    /**
     * @brief Registers a channel for the inject conversion sequence.
     *
     * @tparam tkChannelId The ID of the channel to register.
     * @tparam tkRank Rank in Inject Sequence (1-4).
     */
    template <ADCChannelId tkChannelId, uint8_t tkRank>
        requires(kValidADCChannel<tkADCIdx, tkChannelId> && kValidADCInjectRank<tkRank>)
    void register_inject_sequence()
    {
        const uint32_t idx = get_adc_channel_idx<tkChannelId>();
        if (idx < kMaxChannelId && m_inj_cidx_to_rank_map[idx] == 0)
        {
            m_inj_cidx_to_rank_map[idx] = tkRank;
        }
    }

    /**
     * @brief Registers a channel for the regular conversion sequence.
     *
     * @tparam tkChannelId The ID of the channel to register.
     * @tparam tkRank The rank in the regular sequence (1-16).
     */
    template <ADCChannelId tkChannelId, uint8_t tkRank>
        requires(kValidADCChannel<tkADCIdx, tkChannelId> && kValidADCRegularRank<tkRank>)
    void register_regular_sequence()
    {
        constexpr uint32_t idx = get_adc_channel_idx<tkChannelId>();
        if (m_reg_cidx_to_rank_map[idx] == skChannelRankFreeFlag)
        {
            m_reg_cidx_to_rank_map[idx] = tkRank;
        }
    }

    /**
     * @brief Read data from inject channel.
     *
     * @param rank Rank in Inject Sequence (1-4).
     * @return ADCValue Raw ADC value.
     */
    [[nodiscard]] inline ADCValue read_inject_data(const uint8_t rank) const
    {
        return LL_ADC_INJ_ReadConversionData32(Traits::skInstance, ADCDevice::inject_rank_to_rank_reg(rank));
    }

    /**
     * @brief Read data from regular channel.
     *
     * @param rank Rank in Regular Sequence (1-16).
     * @return ADCValue Raw ADC value.
     */
    [[nodiscard]] constexpr inline ADCValue read_regular_data(const uint8_t rank) const
    {
        const uint8_t buf_idx = rank - 1;
        return m_dma_buffer[buf_idx];
    }
};

// ============================================================================
// ADC CHANNEL (UNIQUE DEVICE)
// ============================================================================

template <uint8_t tkADCIdx, ADCChannelId tkChannelId>
    requires(kValidADCChannel<tkADCIdx, tkChannelId>)
class ADCChannelDevice
{
public:
    struct Descriptor : public UniqueDeviceDescriptor
    {
    };

    using PeripheralT              = ADCPeripheralDevice<tkADCIdx>;
    using PinT                     = typename ADCPinMap<tkADCIdx, tkChannelId>::type;
    static constexpr bool skHasPin = !CNullPin<PinT>;

    using InjectDevices = std::conditional_t<skHasPin, TypeList<PeripheralT, PinT>, TypeList<PeripheralT>>;

    static constexpr uint8_t      skADCIdx    = tkADCIdx;
    static constexpr ADCChannelId skChannelId = tkChannelId;

    template <uint8_t tkAIdx, ADCChannelId tkCID, uint8_t tkRank>
        requires(kValidADCChannel<tkAIdx, tkCID> && kValidADCInjectRank<tkRank>)
    friend class ADCInjectChannelDevice;

    template <uint8_t tkAIdx, ADCChannelId tkCID, uint8_t tkRank>
        requires(kValidADCChannel<tkAIdx, tkCID> && kValidADCRegularRank<tkRank>)
    friend class ADCRegularChannelDevice;

private:
    DeviceRef<PeripheralT>                                                 m_adc;
    std::conditional_t<skHasPin, GPIOAnalogInDriver<PinT>, std::monostate> m_pin;

public:
    explicit ADCChannelDevice(DeviceRef<PeripheralT>&& adc, DeviceRef<PinT>&& pin)
        requires(skHasPin)
        : m_adc(std::move(adc)), m_pin(std::move(pin))
    {
    }

    explicit ADCChannelDevice(DeviceRef<PeripheralT> adc)
        requires(!skHasPin)
        : m_adc(std::move(adc))
    {
    }

protected:
    // --- Initialization ---
    /**
     * @brief Configure and register this channel.
     *
     * @tparam tkGroup ADC Channel Group (Regular/Inject).
     * @tparam tkRank Rank in Sequence (Regular: 1-16, Inject: 1-4).
     * @param config Physics configuration (Sampling time, Single/Diff, etc.)
     */
    template <ADCChannelGroup tkGroup, uint8_t tkRank = 0>
        requires((tkGroup == ADCChannelGroup::kRegular && kValidADCRegularRank<tkRank>) ||
                 (tkGroup == ADCChannelGroup::kInject && kValidADCInjectRank<tkRank>))
    void init(const ADCChannelConfig& config = ADCChannelConfig::default_config())
    {
        m_adc.get().template init_channel<tkChannelId, tkGroup, tkRank>(config);
        gpio_init();
    }

    /**
     * @brief Configure and register this channel for the Inject Sequence (Interrupt).
     *
     * @tparam tkRank Rank in Inject Sequence (1-4).
     * @param config Physics configuration (Sampling time, Single/Diff, etc.)
     */
    template <uint8_t tkRank>
    void init_as_regular(const ADCChannelConfig& config)
    {
        m_adc.get().template init_channel_as_regular<skChannelId, tkRank>(config);
        gpio_init();
    }

    /**
     * @brief Configure and register this channel for the Regular Sequence (DMA).
     *
     * @tparam tkRank Rank in Regular Sequence (1-16).
     * @param config Physics configuration (Sampling time, Single/Diff, etc.)
     */
    template <uint8_t tkRank>
    void init_as_inject(const ADCChannelConfig& config)
    {
        m_adc.get().template init_channel_as_inject<skChannelId, tkRank>(config);
        gpio_init();
    }

    // --- Accessors ---
    // Read raw
    /**
     * @brief Read data from inject channel.
     *
     * @tparam tkRank Rank in Inject Sequence (1-4).
     */
    template <uint8_t tkRank>
        requires(kValidADCInjectRank<tkRank>)
    [[nodiscard]] inline ADCValue read_inject() const
    {
        return m_adc.get().template read_inject_data<tkRank>();
    }

    /**
     * @brief Read data from inject channel (slow method, use read_inject() instead).
     *
     * @return ADCValue Raw ADC value.
     */
    [[nodiscard]] inline ADCValue read_inject_slow() const
    {
        return m_adc.get().template read_inject_data_slow<skChannelId>();
    }

    /**
     * @brief Read data from regular channel.
     *
     * @tparam tkRank Rank in Regular Sequence (1-16).
     */
    template <uint8_t tkRank>
        requires(kValidADCRegularRank<tkRank>)
    [[nodiscard]] inline ADCValue read_regular() const
    {
        return m_adc.get().template read_regular_data<tkRank>();
    }

    /**
     * @brief Read data from regular channel (slow method, use read_regular() instead).
     *
     * @return ADCValue Raw ADC value.
     */
    [[nodiscard]] inline ADCValue read_regular_slow() const
    {
        return m_adc.get().template read_regular_data_slow<skChannelId>();
    }

    // Read voltage
    /**
     * @brief Read voltage from inject channel.
     *
     * @tparam tkRank Rank in Inject Sequence (1-4).
     * @return float Voltage in Volts.
     */
    template <uint8_t tkRank>
        requires(kValidADCInjectRank<tkRank>)
    [[nodiscard]] inline float read_inject_voltage() const
    {
        return raw_to_voltage(read_inject<tkRank>());
    }

    /**
     * @brief Read voltage from inject channel (slow method, use read_inject_voltage() instead).
     *
     * @return float Voltage in Volts.
     */
    [[nodiscard]] inline float read_inject_voltage_slow() const
    {
        return raw_to_voltage(read_inject_slow());
    }

    /**
     * @brief Read voltage from regular channel.
     *
     * @tparam tkRank Rank in Regular Sequence (1-16).
     * @return float Voltage in Volts.
     */
    template <uint8_t tkRank>
        requires(kValidADCRegularRank<tkRank>)
    [[nodiscard]] inline float read_regular_voltage() const
    {
        return raw_to_voltage(read_regular<tkRank>());
    }

    /**
     * @brief Read voltage from regular channel (slow method, use read_regular_voltage() instead).
     *
     * @return float Voltage in Volts.
     */
    [[nodiscard]] inline float read_regular_voltage_slow() const
    {
        return raw_to_voltage(read_regular_slow());
    }

    // Read normalized
    /**
     * @brief Read normalized value (0.0 to 1.0) from inject channel.
     *
     * @tparam tkRank Rank in Inject Sequence (1-4).
     * @return float Normalized value.
     */
    template <uint8_t tkRank>
        requires(kValidADCInjectRank<tkRank>)
    [[nodiscard]] inline float read_inject_normalized() const
    {
        return raw_normalized(read_inject<tkRank>());
    }

    /**
     * @brief Read normalized value (0.0 to 1.0) from inject channel (slow method, use read_inject_normalized() instead).
     *
     * @return float Normalized value.
     */
    [[nodiscard]] inline float read_inject_normalized_slow() const
    {
        return raw_normalized(read_inject_slow());
    }

    /**
     * @brief Read normalized value (0.0 to 1.0) from regular channel.
     *
     * @tparam tkRank Rank in Regular Sequence (1-16).
     */
    template <uint8_t tkRank>
        requires(kValidADCRegularRank<tkRank>)
    [[nodiscard]] inline float read_regular_normalized() const
    {
        return raw_normalized(read_regular<tkRank>());
    }

    /**
     * @brief Read normalized value (0.0 to 1.0) from regular channel (slow method, use read_regular_normalized() instead).
     *
     * @return float Normalized value.
     */
    [[nodiscard]] inline float read_regular_normalized_slow() const
    {
        return raw_normalized(read_regular_slow());
    }

    /**
     * @brief Convert raw ADC value to normalized float (0.0 to 1.0).
     *
     * @param raw Raw ADC value.
     * @return float Normalized value.
     */
    [[nodiscard]] inline float raw_normalized(const ADCValue raw) const
    {
        return static_cast<float>(raw) / m_adc.get().get_resolution_range();
    }

    /**
     * @brief Convert raw ADC value to voltage (Volts).
     *
     * @param raw Raw ADC value.
     * @return float Voltage in Volts.
     */
    [[nodiscard]] inline float raw_to_voltage(const ADCValue raw)
    {
        return raw_normalized(raw) * 3.3f;
    }

private:
    /**
     * @brief Initialize GPIO Pin (if pin exists).
     *
     */
    inline void gpio_init()
    {
        if constexpr (skHasPin)
        {
            m_pin.init();
        }
    }
};

// ============================================================================
// ADC INJECTED CHANNEL (UNIQUE DEVICE)
// ============================================================================

template <uint8_t tkADCIdx, ADCChannelId tkChannelId, uint8_t tkRank>
    requires(kValidADCChannel<tkADCIdx, tkChannelId> &&
             kValidADCInjectRank<tkRank>)  // Only 4 inject channels supported
class ADCInjectChannelDevice
{
public:
    struct Descriptor : public UniqueDeviceDescriptor
    {
    };

    using ChannelT                               = ADCChannelDevice<tkADCIdx, tkChannelId>;
    using InjectDevices                          = TypeList<ChannelT>;
    static constexpr uint8_t         skADCIdx    = tkADCIdx;
    static constexpr ADCChannelId    skChannelId = tkChannelId;
    static constexpr ADCChannelGroup skGroup     = ADCChannelGroup::kInject;
    static constexpr uint8_t         skRank      = tkRank;

private:
    DeviceRef<ChannelT> m_channel;

public:
    explicit ADCInjectChannelDevice(DeviceRef<ChannelT> channel) : m_channel(std::move(channel))
    {
    }

    // --- Initialization ---
    /**
     * @brief Initialize Inject Channel.
     *
     */
    void init(const ADCChannelConfig& config)
    {
        m_channel.get().template init_as_inject<skRank>(config);
    }

    // --- Accessors ---
    /**
     * @brief Read data from inject channel.
     *
     * @return ADCValue Raw ADC value.
     */
    [[nodiscard]] ADCValue read() const
    {
        return m_channel.get().template read_inject<skRank>();
    }

    /**
     * @brief Read voltage from inject channel.
     *
     * @return float Voltage in Volts.
     */
    [[nodiscard]] float read_voltage() const
    {
        return m_channel.get().template read_inject_voltage<skRank>();
    }

    /**
     * @brief Read normalized value from inject channel.
     *
     * @return float Normalized value (0.0 to 1.0).
     */
    [[nodiscard]] float read_normalized() const
    {
        return m_channel.get().template read_inject_normalized<skRank>();
    }
};

template <uint8_t tkADCIdx, ADCChannelId tkChannelId>
    requires(kValidADCChannel<tkADCIdx, tkChannelId>)
using ADCInjectChannelDevice1 = ADCInjectChannelDevice<tkADCIdx, tkChannelId, 1>;

template <uint8_t tkADCIdx, ADCChannelId tkChannelId>
    requires(kValidADCChannel<tkADCIdx, tkChannelId>)
using ADCInjectChannelDevice2 = ADCInjectChannelDevice<tkADCIdx, tkChannelId, 2>;
template <uint8_t tkADCIdx, ADCChannelId tkChannelId>
    requires(kValidADCChannel<tkADCIdx, tkChannelId>)
using ADCInjectChannelDevice3 = ADCInjectChannelDevice<tkADCIdx, tkChannelId, 3>;

template <uint8_t tkADCIdx, ADCChannelId tkChannelId>
    requires(kValidADCChannel<tkADCIdx, tkChannelId>)
using ADCInjectChannelDevice4 = ADCInjectChannelDevice<tkADCIdx, tkChannelId, 4>;

namespace detail
{
    // Helper to unpack the Index Sequence alongside the Channel Pack
    template <uint8_t tkADCIdx, typename TIndexSeq, ADCChannelId... tkChannelIds>
    struct MakeInjectSequenceImpl;

    template <uint8_t tkADCIdx, size_t... Is, ADCChannelId... tkChannelIds>
    struct MakeInjectSequenceImpl<tkADCIdx, std::index_sequence<Is...>, tkChannelIds...>
    {
        // Expand both packs: Channel[i] paired with Rank[i + 1]
        using type = TypeList<ADCInjectChannelDevice<tkADCIdx, tkChannelIds, (Is + 1)>...>;
    };
}  // namespace detail

template <uint8_t tkADCIdx, ADCChannelId... tkChannelIds>
    requires((kValidADCChannel<tkADCIdx, tkChannelIds> && ...) && (sizeof...(tkChannelIds) <= 4))
using ADCInjectChannelDeviceSequence =
    typename detail::MakeInjectSequenceImpl<tkADCIdx,
                                            std::make_index_sequence<sizeof...(tkChannelIds)>,  // Generates 0, 1, 2...
                                            tkChannelIds...>::type;

// ============================================================================
// ADC REGULAR CHANNEL (UNIQUE DEVICE)
// ============================================================================

template <uint8_t tkADCIdx, ADCChannelId tkChannelId, uint8_t tkRank>
    requires(kValidADCChannel<tkADCIdx, tkChannelId> &&
             kValidADCRegularRank<tkRank>)  // Only 16 regular channels supported
class ADCRegularChannelDevice
{
public:
    struct Descriptor : public UniqueDeviceDescriptor
    {
    };

    using ChannelT                               = ADCChannelDevice<tkADCIdx, tkChannelId>;
    using InjectDevices                          = TypeList<ChannelT>;
    static constexpr uint8_t         skADCIdx    = tkADCIdx;
    static constexpr ADCChannelId    skChannelId = tkChannelId;
    static constexpr ADCChannelGroup skGroup     = ADCChannelGroup::kRegular;
    static constexpr uint8_t         skRank      = tkRank;

private:
    DeviceRef<ChannelT> m_channel;

public:
    explicit ADCRegularChannelDevice(DeviceRef<ChannelT>&& channel) : m_channel(std::move(channel))
    {
    }

    // --- Initialization ---
    /**
     * @brief Initialize Regular Channel.
     *
     */
    void init(const ADCChannelConfig& config)
    {
        m_channel.get().template init_as_regular<skRank>(config);
    }

    // --- Accessors ---
    /**
     * @brief Read data from regular channel.
     *
     * @return ADCValue Raw ADC value.
     */
    [[nodiscard]] ADCValue read() const
    {
        return m_channel.get().template read_regular<skRank>();
    }

    /**
     * @brief Read voltage from regular channel.
     *
     * @return float Voltage in Volts.
     */
    [[nodiscard]] float read_voltage() const
    {
        return m_channel.get().template read_regular_voltage<skRank>();
    }

    /**
     * @brief Read normalized value from regular channel.
     *
     * @return float Normalized value (0.0 to 1.0).
     */
    [[nodiscard]] float read_normalized() const
    {
        return m_channel.get().template read_regular_normalized<skRank>();
    }
};

namespace detail
{
    // Helper to unpack the Index Sequence alongside the Channel Pack
    template <uint8_t tkADCIdx, typename TIndexSeq, ADCChannelId... tkChannelIds>
    struct MakeRegularSequenceImpl;

    template <uint8_t tkADCIdx, size_t... Is, ADCChannelId... tkChannelIds>
    struct MakeRegularSequenceImpl<tkADCIdx, std::index_sequence<Is...>, tkChannelIds...>
    {
        // Expand both packs: Channel[i] paired with Rank[i + 1]
        using type = TypeList<ADCRegularChannelDevice<tkADCIdx, tkChannelIds, (Is + 1)>...>;
    };
}  // namespace detail

template <uint8_t tkADCIdx, ADCChannelId... tkChannelIds>
    requires((kValidADCChannel<tkADCIdx, tkChannelIds> && ...) && (sizeof...(tkChannelIds) <= 16))
using ADCRegularChannelDeviceSequence =
    typename detail::MakeRegularSequenceImpl<tkADCIdx,
                                             std::make_index_sequence<sizeof...(tkChannelIds)>,  // Generates 0, 1, 2...
                                             tkChannelIds...>::type;

// ============================================================================
// INTERRUPT ROUTER (The Socket)
// ============================================================================
enum class ADCInterruptType : uint8_t
{
    kAnalogWatchdog = 0,
    kEndOfRegularSequence,
    kEndOfInjectSequence,
    kOverrun,
};

/**
 * @brief ADC Interrupt Router.
 *
 * @tparam tkADCIdx The ADC peripheral index the interrupt belongs to.
 * @tparam tkIntType The interrupt type triggered.
 */
template <uint8_t tkADCIdx, ADCInterruptType tkIntType>
    requires(kValidADCIndex<tkADCIdx>)
struct ADCIsrRouter
{
    static void handle()
    {
        // Default: Do nothing (Optimized away)
    }
};
