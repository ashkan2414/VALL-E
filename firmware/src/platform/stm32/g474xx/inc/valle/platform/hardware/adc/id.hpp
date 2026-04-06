#pragma once

#include <cstdint>
#include <magic_enum/magic_enum.hpp>

#include "valle/platform/core/id.hpp"

namespace valle::platform
{
    // ============================================================================
    // ADC CONTROLLER
    // ===========================================================================
    struct AdcControllerId : public SequentialEnumValue<AdcControllerId, uint8_t, 1, 5>
    {
        static const AdcControllerId kAdc1;
        static const AdcControllerId kAdc2;
        static const AdcControllerId kAdc3;
        static const AdcControllerId kAdc4;
        static const AdcControllerId kAdc5;
    };

    inline constexpr AdcControllerId AdcControllerId::kAdc1 = AdcControllerId::from_number<1>();
    inline constexpr AdcControllerId AdcControllerId::kAdc2 = AdcControllerId::from_number<2>();
    inline constexpr AdcControllerId AdcControllerId::kAdc3 = AdcControllerId::from_number<3>();
    inline constexpr AdcControllerId AdcControllerId::kAdc4 = AdcControllerId::from_number<4>();
    inline constexpr AdcControllerId AdcControllerId::kAdc5 = AdcControllerId::from_number<5>();

    struct AdcControllerSpec
    {
        AdcControllerId controller_id;
    };

    inline constexpr auto kAdc1ControllerSpec = AdcControllerSpec{AdcControllerId::kAdc1};
    inline constexpr auto kAdc2ControllerSpec = AdcControllerSpec{AdcControllerId::kAdc2};
    inline constexpr auto kAdc3ControllerSpec = AdcControllerSpec{AdcControllerId::kAdc3};
    inline constexpr auto kAdc4ControllerSpec = AdcControllerSpec{AdcControllerId::kAdc4};
    inline constexpr auto kAdc5ControllerSpec = AdcControllerSpec{AdcControllerId::kAdc5};

    // ============================================================================
    // ADC CHANNEL
    // ===========================================================================

    inline constexpr auto kAdcChannelIdNumberMapping =
        LLDriverEnumValueSequentialNumberMapping<19>{.start_number = 0,
                                                     .ll_id_table  = {LL_ADC_CHANNEL_0,
                                                                      LL_ADC_CHANNEL_1,
                                                                      LL_ADC_CHANNEL_2,
                                                                      LL_ADC_CHANNEL_3,
                                                                      LL_ADC_CHANNEL_4,
                                                                      LL_ADC_CHANNEL_5,
                                                                      LL_ADC_CHANNEL_6,
                                                                      LL_ADC_CHANNEL_7,
                                                                      LL_ADC_CHANNEL_8,
                                                                      LL_ADC_CHANNEL_9,
                                                                      LL_ADC_CHANNEL_10,
                                                                      LL_ADC_CHANNEL_11,
                                                                      LL_ADC_CHANNEL_12,
                                                                      LL_ADC_CHANNEL_13,
                                                                      LL_ADC_CHANNEL_14,
                                                                      LL_ADC_CHANNEL_15,
                                                                      LL_ADC_CHANNEL_16,
                                                                      LL_ADC_CHANNEL_17,
                                                                      LL_ADC_CHANNEL_18}};

    struct AdcChannelId : public NumberedLLDriverEnumValue<AdcChannelId, kAdcChannelIdNumberMapping>
    {
        static const AdcChannelId kChannel0;
        static const AdcChannelId kChannel1;
        static const AdcChannelId kChannel2;
        static const AdcChannelId kChannel3;
        static const AdcChannelId kChannel4;
        static const AdcChannelId kChannel5;
        static const AdcChannelId kChannel6;
        static const AdcChannelId kChannel7;
        static const AdcChannelId kChannel8;
        static const AdcChannelId kChannel9;
        static const AdcChannelId kChannel10;
        static const AdcChannelId kChannel11;
        static const AdcChannelId kChannel12;
        static const AdcChannelId kChannel13;
        static const AdcChannelId kChannel14;
        static const AdcChannelId kChannel15;
        static const AdcChannelId kChannel16;
        static const AdcChannelId kChannel17;
        static const AdcChannelId kChannel18;
        static const AdcChannelId kChannelVRefInt;
        static const AdcChannelId kChannelTempSensorAdc1;
        static const AdcChannelId kChannelTempSensorAdc5;
        static const AdcChannelId kChannelVBat;
        static const AdcChannelId kChannelVOPAmp1;
        static const AdcChannelId kChannelVOPAmp2;
        static const AdcChannelId kChannelVOPAmp3Adc2;
        static const AdcChannelId kChannelVOPAmp3Adc3;
        static const AdcChannelId kChannelVOPAmp4;
        static const AdcChannelId kChannelVOPAmp5;
        static const AdcChannelId kChannelVOPAmp6;
    };

    inline constexpr AdcChannelId AdcChannelId::kChannel0       = AdcChannelId::from_number<0>();
    inline constexpr AdcChannelId AdcChannelId::kChannel1       = AdcChannelId::from_number<1>();
    inline constexpr AdcChannelId AdcChannelId::kChannel2       = AdcChannelId::from_number<2>();
    inline constexpr AdcChannelId AdcChannelId::kChannel3       = AdcChannelId::from_number<3>();
    inline constexpr AdcChannelId AdcChannelId::kChannel4       = AdcChannelId::from_number<4>();
    inline constexpr AdcChannelId AdcChannelId::kChannel5       = AdcChannelId::from_number<5>();
    inline constexpr AdcChannelId AdcChannelId::kChannel6       = AdcChannelId::from_number<6>();
    inline constexpr AdcChannelId AdcChannelId::kChannel7       = AdcChannelId::from_number<7>();
    inline constexpr AdcChannelId AdcChannelId::kChannel8       = AdcChannelId::from_number<8>();
    inline constexpr AdcChannelId AdcChannelId::kChannel9       = AdcChannelId::from_number<9>();
    inline constexpr AdcChannelId AdcChannelId::kChannel10      = AdcChannelId::from_number<10>();
    inline constexpr AdcChannelId AdcChannelId::kChannel11      = AdcChannelId::from_number<11>();
    inline constexpr AdcChannelId AdcChannelId::kChannel12      = AdcChannelId::from_number<12>();
    inline constexpr AdcChannelId AdcChannelId::kChannel13      = AdcChannelId::from_number<13>();
    inline constexpr AdcChannelId AdcChannelId::kChannel14      = AdcChannelId::from_number<14>();
    inline constexpr AdcChannelId AdcChannelId::kChannel15      = AdcChannelId::from_number<15>();
    inline constexpr AdcChannelId AdcChannelId::kChannel16      = AdcChannelId::from_number<16>();
    inline constexpr AdcChannelId AdcChannelId::kChannel17      = AdcChannelId::from_number<17>();
    inline constexpr AdcChannelId AdcChannelId::kChannel18      = AdcChannelId::from_number<18>();
    inline constexpr AdcChannelId AdcChannelId::kChannelVRefInt = AdcChannelId::from_ll<LL_ADC_CHANNEL_VREFINT>();
    inline constexpr AdcChannelId AdcChannelId::kChannelTempSensorAdc1 =
        AdcChannelId::from_ll<LL_ADC_CHANNEL_TEMPSENSOR_ADC1>();
    inline constexpr AdcChannelId AdcChannelId::kChannelTempSensorAdc5 =
        AdcChannelId::from_ll<LL_ADC_CHANNEL_TEMPSENSOR_ADC5>();
    inline constexpr AdcChannelId AdcChannelId::kChannelVBat    = AdcChannelId::from_ll<LL_ADC_CHANNEL_VBAT>();
    inline constexpr AdcChannelId AdcChannelId::kChannelVOPAmp1 = AdcChannelId::from_ll<LL_ADC_CHANNEL_VOPAMP1>();
    inline constexpr AdcChannelId AdcChannelId::kChannelVOPAmp2 = AdcChannelId::from_ll<LL_ADC_CHANNEL_VOPAMP2>();
    inline constexpr AdcChannelId AdcChannelId::kChannelVOPAmp3Adc2 =
        AdcChannelId::from_ll<LL_ADC_CHANNEL_VOPAMP3_ADC2>();
    inline constexpr AdcChannelId AdcChannelId::kChannelVOPAmp3Adc3 =
        AdcChannelId::from_ll<LL_ADC_CHANNEL_VOPAMP3_ADC3>();
    inline constexpr AdcChannelId AdcChannelId::kChannelVOPAmp4 = AdcChannelId::from_ll<LL_ADC_CHANNEL_VOPAMP4>();
    inline constexpr AdcChannelId AdcChannelId::kChannelVOPAmp5 = AdcChannelId::from_ll<LL_ADC_CHANNEL_VOPAMP5>();
    inline constexpr AdcChannelId AdcChannelId::kChannelVOPAmp6 = AdcChannelId::from_ll<LL_ADC_CHANNEL_VOPAMP6>();

    struct AdcChannelSpec
    {
        AdcControllerId controller_id;
        AdcChannelId    channel_id;

        [[nodiscard]] constexpr VALLE_FORCE_INLINE AdcControllerSpec controller_spec() const
        {
            return AdcControllerSpec{controller_id};
        }
    };

    inline constexpr auto kAdc1Channel0Spec  = AdcChannelSpec{AdcControllerId::kAdc1, AdcChannelId::kChannel0};
    inline constexpr auto kAdc1Channel1Spec  = AdcChannelSpec{AdcControllerId::kAdc1, AdcChannelId::kChannel1};
    inline constexpr auto kAdc1Channel2Spec  = AdcChannelSpec{AdcControllerId::kAdc1, AdcChannelId::kChannel2};
    inline constexpr auto kAdc1Channel3Spec  = AdcChannelSpec{AdcControllerId::kAdc1, AdcChannelId::kChannel3};
    inline constexpr auto kAdc1Channel4Spec  = AdcChannelSpec{AdcControllerId::kAdc1, AdcChannelId::kChannel4};
    inline constexpr auto kAdc1Channel5Spec  = AdcChannelSpec{AdcControllerId::kAdc1, AdcChannelId::kChannel5};
    inline constexpr auto kAdc1Channel6Spec  = AdcChannelSpec{AdcControllerId::kAdc1, AdcChannelId::kChannel6};
    inline constexpr auto kAdc1Channel7Spec  = AdcChannelSpec{AdcControllerId::kAdc1, AdcChannelId::kChannel7};
    inline constexpr auto kAdc1Channel8Spec  = AdcChannelSpec{AdcControllerId::kAdc1, AdcChannelId::kChannel8};
    inline constexpr auto kAdc1Channel9Spec  = AdcChannelSpec{AdcControllerId::kAdc1, AdcChannelId::kChannel9};
    inline constexpr auto kAdc1Channel10Spec = AdcChannelSpec{AdcControllerId::kAdc1, AdcChannelId::kChannel10};
    inline constexpr auto kAdc1Channel11Spec = AdcChannelSpec{AdcControllerId::kAdc1, AdcChannelId::kChannel11};
    inline constexpr auto kAdc1Channel12Spec = AdcChannelSpec{AdcControllerId::kAdc1, AdcChannelId::kChannel12};
    inline constexpr auto kAdc1Channel13Spec = AdcChannelSpec{AdcControllerId::kAdc1, AdcChannelId::kChannel13};
    inline constexpr auto kAdc1Channel14Spec = AdcChannelSpec{AdcControllerId::kAdc1, AdcChannelId::kChannel14};
    inline constexpr auto kAdc1Channel15Spec = AdcChannelSpec{AdcControllerId::kAdc1, AdcChannelId::kChannel15};
    inline constexpr auto kAdc1Channel16Spec = AdcChannelSpec{AdcControllerId::kAdc1, AdcChannelId::kChannel16};
    inline constexpr auto kAdc1Channel17Spec = AdcChannelSpec{AdcControllerId::kAdc1, AdcChannelId::kChannel17};
    inline constexpr auto kAdc1Channel18Spec = AdcChannelSpec{AdcControllerId::kAdc1, AdcChannelId::kChannel18};
    inline constexpr auto kAdc1ChannelVRefIntSpec =
        AdcChannelSpec{AdcControllerId::kAdc1, AdcChannelId::kChannelVRefInt};
    inline constexpr auto kAdc1ChannelTempSensorSpec =
        AdcChannelSpec{AdcControllerId::kAdc1, AdcChannelId::kChannelTempSensorAdc1};
    inline constexpr auto kAdc1ChannelVBatSpec = AdcChannelSpec{AdcControllerId::kAdc1, AdcChannelId::kChannelVBat};
    inline constexpr auto kAdc1ChannelVOPAmp1Spec =
        AdcChannelSpec{AdcControllerId::kAdc1, AdcChannelId::kChannelVOPAmp1};
    inline constexpr auto kAdc1ChannelVOPAmp2Spec =
        AdcChannelSpec{AdcControllerId::kAdc1, AdcChannelId::kChannelVOPAmp2};
    inline constexpr auto kAdc1ChannelVOPAmp4Spec =
        AdcChannelSpec{AdcControllerId::kAdc1, AdcChannelId::kChannelVOPAmp4};
    inline constexpr auto kAdc1ChannelVOPAmp5Spec =
        AdcChannelSpec{AdcControllerId::kAdc1, AdcChannelId::kChannelVOPAmp5};
    inline constexpr auto kAdc1ChannelVOPAmp6Spec =
        AdcChannelSpec{AdcControllerId::kAdc1, AdcChannelId::kChannelVOPAmp6};

    inline constexpr auto kAdc2Channel0Spec  = AdcChannelSpec{AdcControllerId::kAdc2, AdcChannelId::kChannel0};
    inline constexpr auto kAdc2Channel1Spec  = AdcChannelSpec{AdcControllerId::kAdc2, AdcChannelId::kChannel1};
    inline constexpr auto kAdc2Channel2Spec  = AdcChannelSpec{AdcControllerId::kAdc2, AdcChannelId::kChannel2};
    inline constexpr auto kAdc2Channel3Spec  = AdcChannelSpec{AdcControllerId::kAdc2, AdcChannelId::kChannel3};
    inline constexpr auto kAdc2Channel4Spec  = AdcChannelSpec{AdcControllerId::kAdc2, AdcChannelId::kChannel4};
    inline constexpr auto kAdc2Channel5Spec  = AdcChannelSpec{AdcControllerId::kAdc2, AdcChannelId::kChannel5};
    inline constexpr auto kAdc2Channel6Spec  = AdcChannelSpec{AdcControllerId::kAdc2, AdcChannelId::kChannel6};
    inline constexpr auto kAdc2Channel7Spec  = AdcChannelSpec{AdcControllerId::kAdc2, AdcChannelId::kChannel7};
    inline constexpr auto kAdc2Channel8Spec  = AdcChannelSpec{AdcControllerId::kAdc2, AdcChannelId::kChannel8};
    inline constexpr auto kAdc2Channel9Spec  = AdcChannelSpec{AdcControllerId::kAdc2, AdcChannelId::kChannel9};
    inline constexpr auto kAdc2Channel10Spec = AdcChannelSpec{AdcControllerId::kAdc2, AdcChannelId::kChannel10};
    inline constexpr auto kAdc2Channel11Spec = AdcChannelSpec{AdcControllerId::kAdc2, AdcChannelId::kChannel11};
    inline constexpr auto kAdc2Channel12Spec = AdcChannelSpec{AdcControllerId::kAdc2, AdcChannelId::kChannel12};
    inline constexpr auto kAdc2Channel13Spec = AdcChannelSpec{AdcControllerId::kAdc2, AdcChannelId::kChannel13};
    inline constexpr auto kAdc2Channel14Spec = AdcChannelSpec{AdcControllerId::kAdc2, AdcChannelId::kChannel14};
    inline constexpr auto kAdc2Channel15Spec = AdcChannelSpec{AdcControllerId::kAdc2, AdcChannelId::kChannel15};
    inline constexpr auto kAdc2Channel16Spec = AdcChannelSpec{AdcControllerId::kAdc2, AdcChannelId::kChannel16};
    inline constexpr auto kAdc2Channel17Spec = AdcChannelSpec{AdcControllerId::kAdc2, AdcChannelId::kChannel17};
    inline constexpr auto kAdc2Channel18Spec = AdcChannelSpec{AdcControllerId::kAdc2, AdcChannelId::kChannel18};
    inline constexpr auto kAdc2ChannelVRefIntSpec =
        AdcChannelSpec{AdcControllerId::kAdc2, AdcChannelId::kChannelVRefInt};
    inline constexpr auto kAdc2ChannelVBatSpec = AdcChannelSpec{AdcControllerId::kAdc2, AdcChannelId::kChannelVBat};
    inline constexpr auto kAdc2ChannelVOPAmp1Spec =
        AdcChannelSpec{AdcControllerId::kAdc2, AdcChannelId::kChannelVOPAmp1};
    inline constexpr auto kAdc2ChannelVOPAmp2Spec =
        AdcChannelSpec{AdcControllerId::kAdc2, AdcChannelId::kChannelVOPAmp2};
    inline constexpr auto kAdc2ChannelVOPAmp3Spec =
        AdcChannelSpec{AdcControllerId::kAdc2, AdcChannelId::kChannelVOPAmp3Adc2};
    inline constexpr auto kAdc2ChannelVOPAmp4Spec =
        AdcChannelSpec{AdcControllerId::kAdc2, AdcChannelId::kChannelVOPAmp4};
    inline constexpr auto kAdc2ChannelVOPAmp5Spec =
        AdcChannelSpec{AdcControllerId::kAdc2, AdcChannelId::kChannelVOPAmp5};
    inline constexpr auto kAdc2ChannelVOPAmp6Spec =
        AdcChannelSpec{AdcControllerId::kAdc2, AdcChannelId::kChannelVOPAmp6};

    inline constexpr auto kAdc3Channel0Spec  = AdcChannelSpec{AdcControllerId::kAdc3, AdcChannelId::kChannel0};
    inline constexpr auto kAdc3Channel1Spec  = AdcChannelSpec{AdcControllerId::kAdc3, AdcChannelId::kChannel1};
    inline constexpr auto kAdc3Channel2Spec  = AdcChannelSpec{AdcControllerId::kAdc3, AdcChannelId::kChannel2};
    inline constexpr auto kAdc3Channel3Spec  = AdcChannelSpec{AdcControllerId::kAdc3, AdcChannelId::kChannel3};
    inline constexpr auto kAdc3Channel4Spec  = AdcChannelSpec{AdcControllerId::kAdc3, AdcChannelId::kChannel4};
    inline constexpr auto kAdc3Channel5Spec  = AdcChannelSpec{AdcControllerId::kAdc3, AdcChannelId::kChannel5};
    inline constexpr auto kAdc3Channel6Spec  = AdcChannelSpec{AdcControllerId::kAdc3, AdcChannelId::kChannel6};
    inline constexpr auto kAdc3Channel7Spec  = AdcChannelSpec{AdcControllerId::kAdc3, AdcChannelId::kChannel7};
    inline constexpr auto kAdc3Channel8Spec  = AdcChannelSpec{AdcControllerId::kAdc3, AdcChannelId::kChannel8};
    inline constexpr auto kAdc3Channel9Spec  = AdcChannelSpec{AdcControllerId::kAdc3, AdcChannelId::kChannel9};
    inline constexpr auto kAdc3Channel10Spec = AdcChannelSpec{AdcControllerId::kAdc3, AdcChannelId::kChannel10};
    inline constexpr auto kAdc3Channel11Spec = AdcChannelSpec{AdcControllerId::kAdc3, AdcChannelId::kChannel11};
    inline constexpr auto kAdc3Channel12Spec = AdcChannelSpec{AdcControllerId::kAdc3, AdcChannelId::kChannel12};
    inline constexpr auto kAdc3Channel13Spec = AdcChannelSpec{AdcControllerId::kAdc3, AdcChannelId::kChannel13};
    inline constexpr auto kAdc3Channel14Spec = AdcChannelSpec{AdcControllerId::kAdc3, AdcChannelId::kChannel14};
    inline constexpr auto kAdc3Channel15Spec = AdcChannelSpec{AdcControllerId::kAdc3, AdcChannelId::kChannel15};
    inline constexpr auto kAdc3Channel16Spec = AdcChannelSpec{AdcControllerId::kAdc3, AdcChannelId::kChannel16};
    inline constexpr auto kAdc3Channel17Spec = AdcChannelSpec{AdcControllerId::kAdc3, AdcChannelId::kChannel17};
    inline constexpr auto kAdc3Channel18Spec = AdcChannelSpec{AdcControllerId::kAdc3, AdcChannelId::kChannel18};
    inline constexpr auto kAdc3ChannelVRefIntSpec =
        AdcChannelSpec{AdcControllerId::kAdc3, AdcChannelId::kChannelVRefInt};
    inline constexpr auto kAdc3ChannelVBatSpec = AdcChannelSpec{AdcControllerId::kAdc3, AdcChannelId::kChannelVBat};
    inline constexpr auto kAdc3ChannelVOPAmp1Spec =
        AdcChannelSpec{AdcControllerId::kAdc3, AdcChannelId::kChannelVOPAmp1};
    inline constexpr auto kAdc3ChannelVOPAmp2Spec =
        AdcChannelSpec{AdcControllerId::kAdc3, AdcChannelId::kChannelVOPAmp2};
    inline constexpr auto kAdc3ChannelVOPAmp3Spec =
        AdcChannelSpec{AdcControllerId::kAdc3, AdcChannelId::kChannelVOPAmp3Adc3};
    inline constexpr auto kAdc3ChannelVOPAmp4Spec =
        AdcChannelSpec{AdcControllerId::kAdc3, AdcChannelId::kChannelVOPAmp4};
    inline constexpr auto kAdc3ChannelVOPAmp5Spec =
        AdcChannelSpec{AdcControllerId::kAdc3, AdcChannelId::kChannelVOPAmp5};
    inline constexpr auto kAdc3ChannelVOPAmp6Spec =
        AdcChannelSpec{AdcControllerId::kAdc3, AdcChannelId::kChannelVOPAmp6};

    inline constexpr auto kAdc4Channel0Spec  = AdcChannelSpec{AdcControllerId::kAdc4, AdcChannelId::kChannel0};
    inline constexpr auto kAdc4Channel1Spec  = AdcChannelSpec{AdcControllerId::kAdc4, AdcChannelId::kChannel1};
    inline constexpr auto kAdc4Channel2Spec  = AdcChannelSpec{AdcControllerId::kAdc4, AdcChannelId::kChannel2};
    inline constexpr auto kAdc4Channel3Spec  = AdcChannelSpec{AdcControllerId::kAdc4, AdcChannelId::kChannel3};
    inline constexpr auto kAdc4Channel4Spec  = AdcChannelSpec{AdcControllerId::kAdc4, AdcChannelId::kChannel4};
    inline constexpr auto kAdc4Channel5Spec  = AdcChannelSpec{AdcControllerId::kAdc4, AdcChannelId::kChannel5};
    inline constexpr auto kAdc4Channel6Spec  = AdcChannelSpec{AdcControllerId::kAdc4, AdcChannelId::kChannel6};
    inline constexpr auto kAdc4Channel7Spec  = AdcChannelSpec{AdcControllerId::kAdc4, AdcChannelId::kChannel7};
    inline constexpr auto kAdc4Channel8Spec  = AdcChannelSpec{AdcControllerId::kAdc4, AdcChannelId::kChannel8};
    inline constexpr auto kAdc4Channel9Spec  = AdcChannelSpec{AdcControllerId::kAdc4, AdcChannelId::kChannel9};
    inline constexpr auto kAdc4Channel10Spec = AdcChannelSpec{AdcControllerId::kAdc4, AdcChannelId::kChannel10};
    inline constexpr auto kAdc4Channel11Spec = AdcChannelSpec{AdcControllerId::kAdc4, AdcChannelId::kChannel11};
    inline constexpr auto kAdc4Channel12Spec = AdcChannelSpec{AdcControllerId::kAdc4, AdcChannelId::kChannel12};
    inline constexpr auto kAdc4Channel13Spec = AdcChannelSpec{AdcControllerId::kAdc4, AdcChannelId::kChannel13};
    inline constexpr auto kAdc4Channel14Spec = AdcChannelSpec{AdcControllerId::kAdc4, AdcChannelId::kChannel14};
    inline constexpr auto kAdc4Channel15Spec = AdcChannelSpec{AdcControllerId::kAdc4, AdcChannelId::kChannel15};
    inline constexpr auto kAdc4Channel16Spec = AdcChannelSpec{AdcControllerId::kAdc4, AdcChannelId::kChannel16};
    inline constexpr auto kAdc4Channel17Spec = AdcChannelSpec{AdcControllerId::kAdc4, AdcChannelId::kChannel17};
    inline constexpr auto kAdc4Channel18Spec = AdcChannelSpec{AdcControllerId::kAdc4, AdcChannelId::kChannel18};
    inline constexpr auto kAdc4ChannelVRefIntSpec =
        AdcChannelSpec{AdcControllerId::kAdc4, AdcChannelId::kChannelVRefInt};
    inline constexpr auto kAdc4ChannelVBatSpec = AdcChannelSpec{AdcControllerId::kAdc4, AdcChannelId::kChannelVBat};
    inline constexpr auto kAdc4ChannelVOPAmp1Spec =
        AdcChannelSpec{AdcControllerId::kAdc4, AdcChannelId::kChannelVOPAmp1};
    inline constexpr auto kAdc4ChannelVOPAmp2Spec =
        AdcChannelSpec{AdcControllerId::kAdc4, AdcChannelId::kChannelVOPAmp2};
    inline constexpr auto kAdc4ChannelVOPAmp4Spec =
        AdcChannelSpec{AdcControllerId::kAdc4, AdcChannelId::kChannelVOPAmp4};
    inline constexpr auto kAdc4ChannelVOPAmp5Spec =
        AdcChannelSpec{AdcControllerId::kAdc4, AdcChannelId::kChannelVOPAmp5};
    inline constexpr auto kAdc4ChannelVOPAmp6Spec =
        AdcChannelSpec{AdcControllerId::kAdc4, AdcChannelId::kChannelVOPAmp6};

    inline constexpr auto kAdc5Channel0Spec  = AdcChannelSpec{AdcControllerId::kAdc5, AdcChannelId::kChannel0};
    inline constexpr auto kAdc5Channel1Spec  = AdcChannelSpec{AdcControllerId::kAdc5, AdcChannelId::kChannel1};
    inline constexpr auto kAdc5Channel2Spec  = AdcChannelSpec{AdcControllerId::kAdc5, AdcChannelId::kChannel2};
    inline constexpr auto kAdc5Channel3Spec  = AdcChannelSpec{AdcControllerId::kAdc5, AdcChannelId::kChannel3};
    inline constexpr auto kAdc5Channel4Spec  = AdcChannelSpec{AdcControllerId::kAdc5, AdcChannelId::kChannel4};
    inline constexpr auto kAdc5Channel5Spec  = AdcChannelSpec{AdcControllerId::kAdc5, AdcChannelId::kChannel5};
    inline constexpr auto kAdc5Channel6Spec  = AdcChannelSpec{AdcControllerId::kAdc5, AdcChannelId::kChannel6};
    inline constexpr auto kAdc5Channel7Spec  = AdcChannelSpec{AdcControllerId::kAdc5, AdcChannelId::kChannel7};
    inline constexpr auto kAdc5Channel8Spec  = AdcChannelSpec{AdcControllerId::kAdc5, AdcChannelId::kChannel8};
    inline constexpr auto kAdc5Channel9Spec  = AdcChannelSpec{AdcControllerId::kAdc5, AdcChannelId::kChannel9};
    inline constexpr auto kAdc5Channel10Spec = AdcChannelSpec{AdcControllerId::kAdc5, AdcChannelId::kChannel10};
    inline constexpr auto kAdc5Channel11Spec = AdcChannelSpec{AdcControllerId::kAdc5, AdcChannelId::kChannel11};
    inline constexpr auto kAdc5Channel12Spec = AdcChannelSpec{AdcControllerId::kAdc5, AdcChannelId::kChannel12};
    inline constexpr auto kAdc5Channel13Spec = AdcChannelSpec{AdcControllerId::kAdc5, AdcChannelId::kChannel13};
    inline constexpr auto kAdc5Channel14Spec = AdcChannelSpec{AdcControllerId::kAdc5, AdcChannelId::kChannel14};
    inline constexpr auto kAdc5Channel15Spec = AdcChannelSpec{AdcControllerId::kAdc5, AdcChannelId::kChannel15};
    inline constexpr auto kAdc5Channel16Spec = AdcChannelSpec{AdcControllerId::kAdc5, AdcChannelId::kChannel16};
    inline constexpr auto kAdc5Channel17Spec = AdcChannelSpec{AdcControllerId::kAdc5, AdcChannelId::kChannel17};
    inline constexpr auto kAdc5Channel18Spec = AdcChannelSpec{AdcControllerId::kAdc5, AdcChannelId::kChannel18};
    inline constexpr auto kAdc5ChannelVRefIntSpec =
        AdcChannelSpec{AdcControllerId::kAdc5, AdcChannelId::kChannelVRefInt};
    inline constexpr auto kAdc5ChannelTempSensorSpec =
        AdcChannelSpec{AdcControllerId::kAdc5, AdcChannelId::kChannelTempSensorAdc5};
    inline constexpr auto kAdc5ChannelVBatSpec = AdcChannelSpec{AdcControllerId::kAdc5, AdcChannelId::kChannelVBat};
    inline constexpr auto kAdc5ChannelVOPAmp1Spec =
        AdcChannelSpec{AdcControllerId::kAdc5, AdcChannelId::kChannelVOPAmp1};
    inline constexpr auto kAdc5ChannelVOPAmp2Spec =
        AdcChannelSpec{AdcControllerId::kAdc5, AdcChannelId::kChannelVOPAmp2};
    inline constexpr auto kAdc5ChannelVOPAmp4Spec =
        AdcChannelSpec{AdcControllerId::kAdc5, AdcChannelId::kChannelVOPAmp4};
    inline constexpr auto kAdc5ChannelVOPAmp5Spec =
        AdcChannelSpec{AdcControllerId::kAdc5, AdcChannelId::kChannelVOPAmp5};
    inline constexpr auto kAdc5ChannelVOPAmp6Spec =
        AdcChannelSpec{AdcControllerId::kAdc5, AdcChannelId::kChannelVOPAmp6};

    // =========================================================================
    // ADC INJECT CHANNEL RANKS
    // =========================================================================

    inline constexpr auto kAdcInjectChannelRankNumberMapping = LLDriverEnumValueSequentialNumberMapping<4>{
        .start_number = 1, .ll_id_table = {LL_ADC_INJ_RANK_1, LL_ADC_INJ_RANK_2, LL_ADC_INJ_RANK_3, LL_ADC_INJ_RANK_4}};

    struct AdcInjectChannelRank
        : public NumberedLLDriverEnumValue<AdcInjectChannelRank, kAdcInjectChannelRankNumberMapping>
    {
        static const AdcInjectChannelRank kRank1;
        static const AdcInjectChannelRank kRank2;
        static const AdcInjectChannelRank kRank3;
        static const AdcInjectChannelRank kRank4;
    };

    inline constexpr AdcInjectChannelRank AdcInjectChannelRank::kRank1 = AdcInjectChannelRank::from_number<1>();
    inline constexpr AdcInjectChannelRank AdcInjectChannelRank::kRank2 = AdcInjectChannelRank::from_number<2>();
    inline constexpr AdcInjectChannelRank AdcInjectChannelRank::kRank3 = AdcInjectChannelRank::from_number<3>();
    inline constexpr AdcInjectChannelRank AdcInjectChannelRank::kRank4 = AdcInjectChannelRank::from_number<4>();

    struct AdcInjectChannelRankSpec
    {
        AdcControllerId      controller_id;
        AdcInjectChannelRank rank;

        [[nodiscard]] constexpr VALLE_FORCE_INLINE AdcControllerSpec controller_spec() const
        {
            return AdcControllerSpec{controller_id};
        }
    };

    inline constexpr auto kAdc1InjectChannelRank1Spec =
        AdcInjectChannelRankSpec{AdcControllerId::kAdc1, AdcInjectChannelRank::kRank1};
    inline constexpr auto kAdc1InjectChannelRank2Spec =
        AdcInjectChannelRankSpec{AdcControllerId::kAdc1, AdcInjectChannelRank::kRank2};
    inline constexpr auto kAdc1InjectChannelRank3Spec =
        AdcInjectChannelRankSpec{AdcControllerId::kAdc1, AdcInjectChannelRank::kRank3};
    inline constexpr auto kAdc1InjectChannelRank4Spec =
        AdcInjectChannelRankSpec{AdcControllerId::kAdc1, AdcInjectChannelRank::kRank4};

    inline constexpr auto kAdc2InjectChannelRank1Spec =
        AdcInjectChannelRankSpec{AdcControllerId::kAdc2, AdcInjectChannelRank::kRank1};
    inline constexpr auto kAdc2InjectChannelRank2Spec =
        AdcInjectChannelRankSpec{AdcControllerId::kAdc2, AdcInjectChannelRank::kRank2};
    inline constexpr auto kAdc2InjectChannelRank3Spec =
        AdcInjectChannelRankSpec{AdcControllerId::kAdc2, AdcInjectChannelRank::kRank3};
    inline constexpr auto kAdc2InjectChannelRank4Spec =
        AdcInjectChannelRankSpec{AdcControllerId::kAdc2, AdcInjectChannelRank::kRank4};

    inline constexpr auto kAdc3InjectChannelRank1Spec =
        AdcInjectChannelRankSpec{AdcControllerId::kAdc3, AdcInjectChannelRank::kRank1};
    inline constexpr auto kAdc3InjectChannelRank2Spec =
        AdcInjectChannelRankSpec{AdcControllerId::kAdc3, AdcInjectChannelRank::kRank2};
    inline constexpr auto kAdc3InjectChannelRank3Spec =
        AdcInjectChannelRankSpec{AdcControllerId::kAdc3, AdcInjectChannelRank::kRank3};
    inline constexpr auto kAdc3InjectChannelRank4Spec =
        AdcInjectChannelRankSpec{AdcControllerId::kAdc3, AdcInjectChannelRank::kRank4};

    inline constexpr auto kAdc4InjectChannelRank1Spec =
        AdcInjectChannelRankSpec{AdcControllerId::kAdc4, AdcInjectChannelRank::kRank1};
    inline constexpr auto kAdc4InjectChannelRank2Spec =
        AdcInjectChannelRankSpec{AdcControllerId::kAdc4, AdcInjectChannelRank::kRank2};
    inline constexpr auto kAdc4InjectChannelRank3Spec =
        AdcInjectChannelRankSpec{AdcControllerId::kAdc4, AdcInjectChannelRank::kRank3};
    inline constexpr auto kAdc4InjectChannelRank4Spec =
        AdcInjectChannelRankSpec{AdcControllerId::kAdc4, AdcInjectChannelRank::kRank4};

    inline constexpr auto kAdc5InjectChannelRank1Spec =
        AdcInjectChannelRankSpec{AdcControllerId::kAdc5, AdcInjectChannelRank::kRank1};
    inline constexpr auto kAdc5InjectChannelRank2Spec =
        AdcInjectChannelRankSpec{AdcControllerId::kAdc5, AdcInjectChannelRank::kRank2};
    inline constexpr auto kAdc5InjectChannelRank3Spec =
        AdcInjectChannelRankSpec{AdcControllerId::kAdc5, AdcInjectChannelRank::kRank3};
    inline constexpr auto kAdc5InjectChannelRank4Spec =
        AdcInjectChannelRankSpec{AdcControllerId::kAdc5, AdcInjectChannelRank::kRank4};

    // =========================================================================
    // ADC REGULAR CHANNEL RANKS
    // =========================================================================

    inline constexpr auto kAdcRegularChannelRankNumberMapping =
        LLDriverEnumValueSequentialNumberMapping<16>{.start_number = 1,
                                                     .ll_id_table  = {LL_ADC_REG_RANK_1,
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
                                                                      LL_ADC_REG_RANK_16}};
    struct AdcRegularChannelRank
        : public NumberedLLDriverEnumValue<AdcRegularChannelRank, kAdcRegularChannelRankNumberMapping>
    {
        static const AdcRegularChannelRank kRank1;
        static const AdcRegularChannelRank kRank2;
        static const AdcRegularChannelRank kRank3;
        static const AdcRegularChannelRank kRank4;
        static const AdcRegularChannelRank kRank5;
        static const AdcRegularChannelRank kRank6;
        static const AdcRegularChannelRank kRank7;
        static const AdcRegularChannelRank kRank8;
        static const AdcRegularChannelRank kRank9;
        static const AdcRegularChannelRank kRank10;
        static const AdcRegularChannelRank kRank11;
        static const AdcRegularChannelRank kRank12;
        static const AdcRegularChannelRank kRank13;
        static const AdcRegularChannelRank kRank14;
        static const AdcRegularChannelRank kRank15;
        static const AdcRegularChannelRank kRank16;
    };

    inline constexpr AdcRegularChannelRank AdcRegularChannelRank::kRank1  = AdcRegularChannelRank::from_number<1>();
    inline constexpr AdcRegularChannelRank AdcRegularChannelRank::kRank2  = AdcRegularChannelRank::from_number<2>();
    inline constexpr AdcRegularChannelRank AdcRegularChannelRank::kRank3  = AdcRegularChannelRank::from_number<3>();
    inline constexpr AdcRegularChannelRank AdcRegularChannelRank::kRank4  = AdcRegularChannelRank::from_number<4>();
    inline constexpr AdcRegularChannelRank AdcRegularChannelRank::kRank5  = AdcRegularChannelRank::from_number<5>();
    inline constexpr AdcRegularChannelRank AdcRegularChannelRank::kRank6  = AdcRegularChannelRank::from_number<6>();
    inline constexpr AdcRegularChannelRank AdcRegularChannelRank::kRank7  = AdcRegularChannelRank::from_number<7>();
    inline constexpr AdcRegularChannelRank AdcRegularChannelRank::kRank8  = AdcRegularChannelRank::from_number<8>();
    inline constexpr AdcRegularChannelRank AdcRegularChannelRank::kRank9  = AdcRegularChannelRank::from_number<9>();
    inline constexpr AdcRegularChannelRank AdcRegularChannelRank::kRank10 = AdcRegularChannelRank::from_number<10>();
    inline constexpr AdcRegularChannelRank AdcRegularChannelRank::kRank11 = AdcRegularChannelRank::from_number<11>();
    inline constexpr AdcRegularChannelRank AdcRegularChannelRank::kRank12 = AdcRegularChannelRank::from_number<12>();
    inline constexpr AdcRegularChannelRank AdcRegularChannelRank::kRank13 = AdcRegularChannelRank::from_number<13>();
    inline constexpr AdcRegularChannelRank AdcRegularChannelRank::kRank14 = AdcRegularChannelRank::from_number<14>();
    inline constexpr AdcRegularChannelRank AdcRegularChannelRank::kRank15 = AdcRegularChannelRank::from_number<15>();
    inline constexpr AdcRegularChannelRank AdcRegularChannelRank::kRank16 = AdcRegularChannelRank::from_number<16>();

    struct AdcRegularChannelRankSpec
    {
        AdcControllerId       controller_id;
        AdcRegularChannelRank rank;

        [[nodiscard]] constexpr VALLE_FORCE_INLINE AdcControllerSpec controller_spec() const
        {
            return AdcControllerSpec{controller_id};
        }
    };

    inline constexpr auto kAdc1RegularChannelRank1Spec =
        AdcRegularChannelRankSpec{AdcControllerId::kAdc1, AdcRegularChannelRank::kRank1};
    inline constexpr auto kAdc1RegularChannelRank2Spec =
        AdcRegularChannelRankSpec{AdcControllerId::kAdc1, AdcRegularChannelRank::kRank2};
    inline constexpr auto kAdc1RegularChannelRank3Spec =
        AdcRegularChannelRankSpec{AdcControllerId::kAdc1, AdcRegularChannelRank::kRank3};
    inline constexpr auto kAdc1RegularChannelRank4Spec =
        AdcRegularChannelRankSpec{AdcControllerId::kAdc1, AdcRegularChannelRank::kRank4};
    inline constexpr auto kAdc1RegularChannelRank5Spec =
        AdcRegularChannelRankSpec{AdcControllerId::kAdc1, AdcRegularChannelRank::kRank5};
    inline constexpr auto kAdc1RegularChannelRank6Spec =
        AdcRegularChannelRankSpec{AdcControllerId::kAdc1, AdcRegularChannelRank::kRank6};
    inline constexpr auto kAdc1RegularChannelRank7Spec =
        AdcRegularChannelRankSpec{AdcControllerId::kAdc1, AdcRegularChannelRank::kRank7};
    inline constexpr auto kAdc1RegularChannelRank8Spec =
        AdcRegularChannelRankSpec{AdcControllerId::kAdc1, AdcRegularChannelRank::kRank8};
    inline constexpr auto kAdc1RegularChannelRank9Spec =
        AdcRegularChannelRankSpec{AdcControllerId::kAdc1, AdcRegularChannelRank::kRank9};
    inline constexpr auto kAdc1RegularChannelRank10Spec =
        AdcRegularChannelRankSpec{AdcControllerId::kAdc1, AdcRegularChannelRank::kRank10};
    inline constexpr auto kAdc1RegularChannelRank11Spec =
        AdcRegularChannelRankSpec{AdcControllerId::kAdc1, AdcRegularChannelRank::kRank11};
    inline constexpr auto kAdc1RegularChannelRank12Spec =
        AdcRegularChannelRankSpec{AdcControllerId::kAdc1, AdcRegularChannelRank::kRank12};
    inline constexpr auto kAdc1RegularChannelRank13Spec =
        AdcRegularChannelRankSpec{AdcControllerId::kAdc1, AdcRegularChannelRank::kRank13};
    inline constexpr auto kAdc1RegularChannelRank14Spec =
        AdcRegularChannelRankSpec{AdcControllerId::kAdc1, AdcRegularChannelRank::kRank14};
    inline constexpr auto kAdc1RegularChannelRank15Spec =
        AdcRegularChannelRankSpec{AdcControllerId::kAdc1, AdcRegularChannelRank::kRank15};
    inline constexpr auto kAdc1RegularChannelRank16Spec =
        AdcRegularChannelRankSpec{AdcControllerId::kAdc1, AdcRegularChannelRank::kRank16};

    inline constexpr auto kAdc2RegularChannelRank1Spec =
        AdcRegularChannelRankSpec{AdcControllerId::kAdc2, AdcRegularChannelRank::kRank1};
    inline constexpr auto kAdc2RegularChannelRank2Spec =
        AdcRegularChannelRankSpec{AdcControllerId::kAdc2, AdcRegularChannelRank::kRank2};
    inline constexpr auto kAdc2RegularChannelRank3Spec =
        AdcRegularChannelRankSpec{AdcControllerId::kAdc2, AdcRegularChannelRank::kRank3};
    inline constexpr auto kAdc2RegularChannelRank4Spec =
        AdcRegularChannelRankSpec{AdcControllerId::kAdc2, AdcRegularChannelRank::kRank4};
    inline constexpr auto kAdc2RegularChannelRank5Spec =
        AdcRegularChannelRankSpec{AdcControllerId::kAdc2, AdcRegularChannelRank::kRank5};
    inline constexpr auto kAdc2RegularChannelRank6Spec =
        AdcRegularChannelRankSpec{AdcControllerId::kAdc2, AdcRegularChannelRank::kRank6};
    inline constexpr auto kAdc2RegularChannelRank7Spec =
        AdcRegularChannelRankSpec{AdcControllerId::kAdc2, AdcRegularChannelRank::kRank7};
    inline constexpr auto kAdc2RegularChannelRank8Spec =
        AdcRegularChannelRankSpec{AdcControllerId::kAdc2, AdcRegularChannelRank::kRank8};
    inline constexpr auto kAdc2RegularChannelRank9Spec =
        AdcRegularChannelRankSpec{AdcControllerId::kAdc2, AdcRegularChannelRank::kRank9};
    inline constexpr auto kAdc2RegularChannelRank10Spec =
        AdcRegularChannelRankSpec{AdcControllerId::kAdc2, AdcRegularChannelRank::kRank10};
    inline constexpr auto kAdc2RegularChannelRank11Spec =
        AdcRegularChannelRankSpec{AdcControllerId::kAdc2, AdcRegularChannelRank::kRank11};
    inline constexpr auto kAdc2RegularChannelRank12Spec =
        AdcRegularChannelRankSpec{AdcControllerId::kAdc2, AdcRegularChannelRank::kRank12};
    inline constexpr auto kAdc2RegularChannelRank13Spec =
        AdcRegularChannelRankSpec{AdcControllerId::kAdc2, AdcRegularChannelRank::kRank13};
    inline constexpr auto kAdc2RegularChannelRank14Spec =
        AdcRegularChannelRankSpec{AdcControllerId::kAdc2, AdcRegularChannelRank::kRank14};
    inline constexpr auto kAdc2RegularChannelRank15Spec =
        AdcRegularChannelRankSpec{AdcControllerId::kAdc2, AdcRegularChannelRank::kRank15};
    inline constexpr auto kAdc2RegularChannelRank16Spec =
        AdcRegularChannelRankSpec{AdcControllerId::kAdc2, AdcRegularChannelRank::kRank16};

    inline constexpr auto kAdc3RegularChannelRank1Spec =
        AdcRegularChannelRankSpec{AdcControllerId::kAdc3, AdcRegularChannelRank::kRank1};
    inline constexpr auto kAdc3RegularChannelRank2Spec =
        AdcRegularChannelRankSpec{AdcControllerId::kAdc3, AdcRegularChannelRank::kRank2};
    inline constexpr auto kAdc3RegularChannelRank3Spec =
        AdcRegularChannelRankSpec{AdcControllerId::kAdc3, AdcRegularChannelRank::kRank3};
    inline constexpr auto kAdc3RegularChannelRank4Spec =
        AdcRegularChannelRankSpec{AdcControllerId::kAdc3, AdcRegularChannelRank::kRank4};
    inline constexpr auto kAdc3RegularChannelRank5Spec =
        AdcRegularChannelRankSpec{AdcControllerId::kAdc3, AdcRegularChannelRank::kRank5};
    inline constexpr auto kAdc3RegularChannelRank6Spec =
        AdcRegularChannelRankSpec{AdcControllerId::kAdc3, AdcRegularChannelRank::kRank6};
    inline constexpr auto kAdc3RegularChannelRank7Spec =
        AdcRegularChannelRankSpec{AdcControllerId::kAdc3, AdcRegularChannelRank::kRank7};
    inline constexpr auto kAdc3RegularChannelRank8Spec =
        AdcRegularChannelRankSpec{AdcControllerId::kAdc3, AdcRegularChannelRank::kRank8};
    inline constexpr auto kAdc3RegularChannelRank9Spec =
        AdcRegularChannelRankSpec{AdcControllerId::kAdc3, AdcRegularChannelRank::kRank9};
    inline constexpr auto kAdc3RegularChannelRank10Spec =
        AdcRegularChannelRankSpec{AdcControllerId::kAdc3, AdcRegularChannelRank::kRank10};
    inline constexpr auto kAdc3RegularChannelRank11Spec =
        AdcRegularChannelRankSpec{AdcControllerId::kAdc3, AdcRegularChannelRank::kRank11};
    inline constexpr auto kAdc3RegularChannelRank12Spec =
        AdcRegularChannelRankSpec{AdcControllerId::kAdc3, AdcRegularChannelRank::kRank12};
    inline constexpr auto kAdc3RegularChannelRank13Spec =
        AdcRegularChannelRankSpec{AdcControllerId::kAdc3, AdcRegularChannelRank::kRank13};
    inline constexpr auto kAdc3RegularChannelRank14Spec =
        AdcRegularChannelRankSpec{AdcControllerId::kAdc3, AdcRegularChannelRank::kRank14};
    inline constexpr auto kAdc3RegularChannelRank15Spec =
        AdcRegularChannelRankSpec{AdcControllerId::kAdc3, AdcRegularChannelRank::kRank15};
    inline constexpr auto kAdc3RegularChannelRank16Spec =
        AdcRegularChannelRankSpec{AdcControllerId::kAdc3, AdcRegularChannelRank::kRank16};

    inline constexpr auto kAdc4RegularChannelRank1Spec =
        AdcRegularChannelRankSpec{AdcControllerId::kAdc4, AdcRegularChannelRank::kRank1};
    inline constexpr auto kAdc4RegularChannelRank2Spec =
        AdcRegularChannelRankSpec{AdcControllerId::kAdc4, AdcRegularChannelRank::kRank2};
    inline constexpr auto kAdc4RegularChannelRank3Spec =
        AdcRegularChannelRankSpec{AdcControllerId::kAdc4, AdcRegularChannelRank::kRank3};
    inline constexpr auto kAdc4RegularChannelRank4Spec =
        AdcRegularChannelRankSpec{AdcControllerId::kAdc4, AdcRegularChannelRank::kRank4};
    inline constexpr auto kAdc4RegularChannelRank5Spec =
        AdcRegularChannelRankSpec{AdcControllerId::kAdc4, AdcRegularChannelRank::kRank5};
    inline constexpr auto kAdc4RegularChannelRank6Spec =
        AdcRegularChannelRankSpec{AdcControllerId::kAdc4, AdcRegularChannelRank::kRank6};
    inline constexpr auto kAdc4RegularChannelRank7Spec =
        AdcRegularChannelRankSpec{AdcControllerId::kAdc4, AdcRegularChannelRank::kRank7};
    inline constexpr auto kAdc4RegularChannelRank8Spec =
        AdcRegularChannelRankSpec{AdcControllerId::kAdc4, AdcRegularChannelRank::kRank8};
    inline constexpr auto kAdc4RegularChannelRank9Spec =
        AdcRegularChannelRankSpec{AdcControllerId::kAdc4, AdcRegularChannelRank::kRank9};
    inline constexpr auto kAdc4RegularChannelRank10Spec =
        AdcRegularChannelRankSpec{AdcControllerId::kAdc4, AdcRegularChannelRank::kRank10};
    inline constexpr auto kAdc4RegularChannelRank11Spec =
        AdcRegularChannelRankSpec{AdcControllerId::kAdc4, AdcRegularChannelRank::kRank11};
    inline constexpr auto kAdc4RegularChannelRank12Spec =
        AdcRegularChannelRankSpec{AdcControllerId::kAdc4, AdcRegularChannelRank::kRank12};
    inline constexpr auto kAdc4RegularChannelRank13Spec =
        AdcRegularChannelRankSpec{AdcControllerId::kAdc4, AdcRegularChannelRank::kRank13};
    inline constexpr auto kAdc4RegularChannelRank14Spec =
        AdcRegularChannelRankSpec{AdcControllerId::kAdc4, AdcRegularChannelRank::kRank14};
    inline constexpr auto kAdc4RegularChannelRank15Spec =
        AdcRegularChannelRankSpec{AdcControllerId::kAdc4, AdcRegularChannelRank::kRank15};
    inline constexpr auto kAdc4RegularChannelRank16Spec =
        AdcRegularChannelRankSpec{AdcControllerId::kAdc4, AdcRegularChannelRank::kRank16};

    inline constexpr auto kAdc5RegularChannelRank1Spec =
        AdcRegularChannelRankSpec{AdcControllerId::kAdc5, AdcRegularChannelRank::kRank1};
    inline constexpr auto kAdc5RegularChannelRank2Spec =
        AdcRegularChannelRankSpec{AdcControllerId::kAdc5, AdcRegularChannelRank::kRank2};
    inline constexpr auto kAdc5RegularChannelRank3Spec =
        AdcRegularChannelRankSpec{AdcControllerId::kAdc5, AdcRegularChannelRank::kRank3};
    inline constexpr auto kAdc5RegularChannelRank4Spec =
        AdcRegularChannelRankSpec{AdcControllerId::kAdc5, AdcRegularChannelRank::kRank4};
    inline constexpr auto kAdc5RegularChannelRank5Spec =
        AdcRegularChannelRankSpec{AdcControllerId::kAdc5, AdcRegularChannelRank::kRank5};
    inline constexpr auto kAdc5RegularChannelRank6Spec =
        AdcRegularChannelRankSpec{AdcControllerId::kAdc5, AdcRegularChannelRank::kRank6};
    inline constexpr auto kAdc5RegularChannelRank7Spec =
        AdcRegularChannelRankSpec{AdcControllerId::kAdc5, AdcRegularChannelRank::kRank7};
    inline constexpr auto kAdc5RegularChannelRank8Spec =
        AdcRegularChannelRankSpec{AdcControllerId::kAdc5, AdcRegularChannelRank::kRank8};
    inline constexpr auto kAdc5RegularChannelRank9Spec =
        AdcRegularChannelRankSpec{AdcControllerId::kAdc5, AdcRegularChannelRank::kRank9};
    inline constexpr auto kAdc5RegularChannelRank10Spec =
        AdcRegularChannelRankSpec{AdcControllerId::kAdc5, AdcRegularChannelRank::kRank10};
    inline constexpr auto kAdc5RegularChannelRank11Spec =
        AdcRegularChannelRankSpec{AdcControllerId::kAdc5, AdcRegularChannelRank::kRank11};
    inline constexpr auto kAdc5RegularChannelRank12Spec =
        AdcRegularChannelRankSpec{AdcControllerId::kAdc5, AdcRegularChannelRank::kRank12};
    inline constexpr auto kAdc5RegularChannelRank13Spec =
        AdcRegularChannelRankSpec{AdcControllerId::kAdc5, AdcRegularChannelRank::kRank13};
    inline constexpr auto kAdc5RegularChannelRank14Spec =
        AdcRegularChannelRankSpec{AdcControllerId::kAdc5, AdcRegularChannelRank::kRank14};
    inline constexpr auto kAdc5RegularChannelRank15Spec =
        AdcRegularChannelRankSpec{AdcControllerId::kAdc5, AdcRegularChannelRank::kRank15};
    inline constexpr auto kAdc5RegularChannelRank16Spec =
        AdcRegularChannelRankSpec{AdcControllerId::kAdc5, AdcRegularChannelRank::kRank16};

    // =========================================================================
    // ADC OFFSET BLOCKS
    // =========================================================================

    inline constexpr auto kAdcOffsetBlockIdNumberMapping = LLDriverEnumValueSequentialNumberMapping<4>{
        .start_number = 1, .ll_id_table = {LL_ADC_OFFSET_1, LL_ADC_OFFSET_2, LL_ADC_OFFSET_3, LL_ADC_OFFSET_4}};

    struct AdcOffsetBlockId : public NumberedLLDriverEnumValue<AdcOffsetBlockId, kAdcOffsetBlockIdNumberMapping>
    {
        static const AdcOffsetBlockId kOffset1;
        static const AdcOffsetBlockId kOffset2;
        static const AdcOffsetBlockId kOffset3;
        static const AdcOffsetBlockId kOffset4;
    };

    inline constexpr AdcOffsetBlockId AdcOffsetBlockId::kOffset1 = AdcOffsetBlockId::from_number<1>();
    inline constexpr AdcOffsetBlockId AdcOffsetBlockId::kOffset2 = AdcOffsetBlockId::from_number<2>();
    inline constexpr AdcOffsetBlockId AdcOffsetBlockId::kOffset3 = AdcOffsetBlockId::from_number<3>();
    inline constexpr AdcOffsetBlockId AdcOffsetBlockId::kOffset4 = AdcOffsetBlockId::from_number<4>();

    struct AdcOffsetBlockSpec
    {
        AdcControllerId  controller_id;
        AdcOffsetBlockId block_id;

        [[nodiscard]] constexpr VALLE_FORCE_INLINE AdcControllerSpec controller_spec() const
        {
            return AdcControllerSpec{controller_id};
        }
    };

    inline constexpr auto kAdc1OffsetBlock1Spec =
        AdcOffsetBlockSpec{AdcControllerId::kAdc1, AdcOffsetBlockId::kOffset1};
    inline constexpr auto kAdc1OffsetBlock2Spec =
        AdcOffsetBlockSpec{AdcControllerId::kAdc1, AdcOffsetBlockId::kOffset2};
    inline constexpr auto kAdc1OffsetBlock3Spec =
        AdcOffsetBlockSpec{AdcControllerId::kAdc1, AdcOffsetBlockId::kOffset3};
    inline constexpr auto kAdc1OffsetBlock4Spec =
        AdcOffsetBlockSpec{AdcControllerId::kAdc1, AdcOffsetBlockId::kOffset4};

    inline constexpr auto kAdc2OffsetBlock1Spec =
        AdcOffsetBlockSpec{AdcControllerId::kAdc2, AdcOffsetBlockId::kOffset1};
    inline constexpr auto kAdc2OffsetBlock2Spec =
        AdcOffsetBlockSpec{AdcControllerId::kAdc2, AdcOffsetBlockId::kOffset2};
    inline constexpr auto kAdc2OffsetBlock3Spec =
        AdcOffsetBlockSpec{AdcControllerId::kAdc2, AdcOffsetBlockId::kOffset3};
    inline constexpr auto kAdc2OffsetBlock4Spec =
        AdcOffsetBlockSpec{AdcControllerId::kAdc2, AdcOffsetBlockId::kOffset4};

    inline constexpr auto kAdc3OffsetBlock1Spec =
        AdcOffsetBlockSpec{AdcControllerId::kAdc3, AdcOffsetBlockId::kOffset1};
    inline constexpr auto kAdc3OffsetBlock2Spec =
        AdcOffsetBlockSpec{AdcControllerId::kAdc3, AdcOffsetBlockId::kOffset2};
    inline constexpr auto kAdc3OffsetBlock3Spec =
        AdcOffsetBlockSpec{AdcControllerId::kAdc3, AdcOffsetBlockId::kOffset3};
    inline constexpr auto kAdc3OffsetBlock4Spec =
        AdcOffsetBlockSpec{AdcControllerId::kAdc3, AdcOffsetBlockId::kOffset4};

    inline constexpr auto kAdc4OffsetBlock1Spec =
        AdcOffsetBlockSpec{AdcControllerId::kAdc4, AdcOffsetBlockId::kOffset1};
    inline constexpr auto kAdc4OffsetBlock2Spec =
        AdcOffsetBlockSpec{AdcControllerId::kAdc4, AdcOffsetBlockId::kOffset2};
    inline constexpr auto kAdc4OffsetBlock3Spec =
        AdcOffsetBlockSpec{AdcControllerId::kAdc4, AdcOffsetBlockId::kOffset3};
    inline constexpr auto kAdc4OffsetBlock4Spec =
        AdcOffsetBlockSpec{AdcControllerId::kAdc4, AdcOffsetBlockId::kOffset4};

    inline constexpr auto kAdc5OffsetBlock1Spec =
        AdcOffsetBlockSpec{AdcControllerId::kAdc5, AdcOffsetBlockId::kOffset1};
    inline constexpr auto kAdc5OffsetBlock2Spec =
        AdcOffsetBlockSpec{AdcControllerId::kAdc5, AdcOffsetBlockId::kOffset2};
    inline constexpr auto kAdc5OffsetBlock3Spec =
        AdcOffsetBlockSpec{AdcControllerId::kAdc5, AdcOffsetBlockId::kOffset3};
    inline constexpr auto kAdc5OffsetBlock4Spec =
        AdcOffsetBlockSpec{AdcControllerId::kAdc5, AdcOffsetBlockId::kOffset4};

    // =========================================================================
    // ADC ANALOG WATCHDOGS
    // =========================================================================
    inline constexpr auto kAdcAnalogWatchdogIdNumberMapping = LLDriverEnumValueSequentialNumberMapping<4>{
        .start_number = 1, .ll_id_table = {LL_ADC_AWD1, LL_ADC_AWD2, LL_ADC_AWD3}};

    struct AdcAnalogWatchdogId
        : public NumberedLLDriverEnumValue<AdcAnalogWatchdogId, kAdcAnalogWatchdogIdNumberMapping>
    {
        static const AdcAnalogWatchdogId kWatchdog1;
        static const AdcAnalogWatchdogId kWatchdog2;
        static const AdcAnalogWatchdogId kWatchdog3;
    };

    inline constexpr AdcAnalogWatchdogId AdcAnalogWatchdogId::kWatchdog1 = AdcAnalogWatchdogId::from_number<1>();
    inline constexpr AdcAnalogWatchdogId AdcAnalogWatchdogId::kWatchdog2 = AdcAnalogWatchdogId::from_number<2>();
    inline constexpr AdcAnalogWatchdogId AdcAnalogWatchdogId::kWatchdog3 = AdcAnalogWatchdogId::from_number<3>();

    struct AdcAnalogWatchdogSpec
    {
        AdcControllerId     controller_id;
        AdcAnalogWatchdogId watchdog_id;

        [[nodiscard]] constexpr VALLE_FORCE_INLINE AdcControllerSpec controller_spec() const
        {
            return AdcControllerSpec{controller_id};
        }
    };

    inline constexpr auto kAdc1AnalogWatchdog1Spec =
        AdcAnalogWatchdogSpec{AdcControllerId::kAdc1, AdcAnalogWatchdogId::kWatchdog1};
    inline constexpr auto kAdc1AnalogWatchdog2Spec =
        AdcAnalogWatchdogSpec{AdcControllerId::kAdc1, AdcAnalogWatchdogId::kWatchdog2};
    inline constexpr auto kAdc1AnalogWatchdog3Spec =
        AdcAnalogWatchdogSpec{AdcControllerId::kAdc1, AdcAnalogWatchdogId::kWatchdog3};

    inline constexpr auto kAdc2AnalogWatchdog1Spec =
        AdcAnalogWatchdogSpec{AdcControllerId::kAdc2, AdcAnalogWatchdogId::kWatchdog1};
    inline constexpr auto kAdc2AnalogWatchdog2Spec =
        AdcAnalogWatchdogSpec{AdcControllerId::kAdc2, AdcAnalogWatchdogId::kWatchdog2};
    inline constexpr auto kAdc2AnalogWatchdog3Spec =
        AdcAnalogWatchdogSpec{AdcControllerId::kAdc2, AdcAnalogWatchdogId::kWatchdog3};

    inline constexpr auto kAdc3AnalogWatchdog1Spec =
        AdcAnalogWatchdogSpec{AdcControllerId::kAdc3, AdcAnalogWatchdogId::kWatchdog1};
    inline constexpr auto kAdc3AnalogWatchdog2Spec =
        AdcAnalogWatchdogSpec{AdcControllerId::kAdc3, AdcAnalogWatchdogId::kWatchdog2};
    inline constexpr auto kAdc3AnalogWatchdog3Spec =
        AdcAnalogWatchdogSpec{AdcControllerId::kAdc3, AdcAnalogWatchdogId::kWatchdog3};

    inline constexpr auto kAdc4AnalogWatchdog1Spec =
        AdcAnalogWatchdogSpec{AdcControllerId::kAdc4, AdcAnalogWatchdogId::kWatchdog1};
    inline constexpr auto kAdc4AnalogWatchdog2Spec =
        AdcAnalogWatchdogSpec{AdcControllerId::kAdc4, AdcAnalogWatchdogId::kWatchdog2};
    inline constexpr auto kAdc4AnalogWatchdog3Spec =
        AdcAnalogWatchdogSpec{AdcControllerId::kAdc4, AdcAnalogWatchdogId::kWatchdog3};

    inline constexpr auto kAdc5AnalogWatchdog1Spec =
        AdcAnalogWatchdogSpec{AdcControllerId::kAdc5, AdcAnalogWatchdogId::kWatchdog1};
    inline constexpr auto kAdc5AnalogWatchdog2Spec =
        AdcAnalogWatchdogSpec{AdcControllerId::kAdc5, AdcAnalogWatchdogId::kWatchdog2};
    inline constexpr auto kAdc5AnalogWatchdog3Spec =
        AdcAnalogWatchdogSpec{AdcControllerId::kAdc5, AdcAnalogWatchdogId::kWatchdog3};

}  // namespace valle::platform