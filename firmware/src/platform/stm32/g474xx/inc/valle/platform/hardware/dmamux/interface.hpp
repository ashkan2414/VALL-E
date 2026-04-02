#pragma once

#include "stm32g4xx_ll_bus.h"
#include "stm32g4xx_ll_dmamux.h"
#include "valle/platform/hardware/dmamux/id.hpp"

namespace valle::platform
{
    // ============================================================================
    // ENUMERATIONS
    // ============================================================================
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
        kLPUart1Rx   = LL_DMAMUX_REQ_LPUART1_RX,
        kLPUart1Tx   = LL_DMAMUX_REQ_LPUART1_TX,
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

    enum class DmaMuxSyncPolarity : uint32_t
    {
        kNoEvent = LL_DMAMUX_SYNC_NO_EVENT,
        kRising  = LL_DMAMUX_SYNC_POL_RISING,
        kFalling = LL_DMAMUX_SYNC_POL_FALLING,
        kBoth    = LL_DMAMUX_SYNC_POL_RISING_FALLING,
    };

    enum class DmaMuxSyncId : uint32_t
    {
        kExtiLine0  = LL_DMAMUX_SYNC_EXTI_LINE0,
        kExtiLine1  = LL_DMAMUX_SYNC_EXTI_LINE1,
        kExtiLine2  = LL_DMAMUX_SYNC_EXTI_LINE2,
        kExtiLine3  = LL_DMAMUX_SYNC_EXTI_LINE3,
        kExtiLine4  = LL_DMAMUX_SYNC_EXTI_LINE4,
        kExtiLine5  = LL_DMAMUX_SYNC_EXTI_LINE5,
        kExtiLine6  = LL_DMAMUX_SYNC_EXTI_LINE6,
        kExtiLine7  = LL_DMAMUX_SYNC_EXTI_LINE7,
        kExtiLine8  = LL_DMAMUX_SYNC_EXTI_LINE8,
        kExtiLine9  = LL_DMAMUX_SYNC_EXTI_LINE9,
        kExtiLine10 = LL_DMAMUX_SYNC_EXTI_LINE10,
        kExtiLine11 = LL_DMAMUX_SYNC_EXTI_LINE11,
        kExtiLine12 = LL_DMAMUX_SYNC_EXTI_LINE12,
        kExtiLine13 = LL_DMAMUX_SYNC_EXTI_LINE13,
        kExtiLine14 = LL_DMAMUX_SYNC_EXTI_LINE14,
        kExtiLine15 = LL_DMAMUX_SYNC_EXTI_LINE15,
        kDmaMuxCh0  = LL_DMAMUX_SYNC_DMAMUX_CH0,
        kDmaMuxCh1  = LL_DMAMUX_SYNC_DMAMUX_CH1,
        kDmaMuxCh2  = LL_DMAMUX_SYNC_DMAMUX_CH2,
        kDmaMuxCh3  = LL_DMAMUX_SYNC_DMAMUX_CH3,
        kLptim1Out  = LL_DMAMUX_SYNC_LPTIM1_OUT,
    };

    enum class DmaMuxRequestGenPolarity : uint32_t
    {
        kNoEvent = LL_DMAMUX_REQ_GEN_NO_EVENT,
        kRising  = LL_DMAMUX_REQ_GEN_POL_RISING,
        kFalling = LL_DMAMUX_REQ_GEN_POL_FALLING,
        kBoth    = LL_DMAMUX_REQ_GEN_POL_RISING_FALLING
    };

    enum class DmaRequestGenSignalId : uint32_t
    {
        kExtiLine0  = LL_DMAMUX_REQ_GEN_EXTI_LINE0,
        kExtiLine1  = LL_DMAMUX_REQ_GEN_EXTI_LINE1,
        kExtiLine2  = LL_DMAMUX_REQ_GEN_EXTI_LINE2,
        kExtiLine3  = LL_DMAMUX_REQ_GEN_EXTI_LINE3,
        kExtiLine4  = LL_DMAMUX_REQ_GEN_EXTI_LINE4,
        kExtiLine5  = LL_DMAMUX_REQ_GEN_EXTI_LINE5,
        kExtiLine6  = LL_DMAMUX_REQ_GEN_EXTI_LINE6,
        kExtiLine7  = LL_DMAMUX_REQ_GEN_EXTI_LINE7,
        kExtiLine8  = LL_DMAMUX_REQ_GEN_EXTI_LINE8,
        kExtiLine9  = LL_DMAMUX_REQ_GEN_EXTI_LINE9,
        kExtiLine10 = LL_DMAMUX_REQ_GEN_EXTI_LINE10,
        kExtiLine11 = LL_DMAMUX_REQ_GEN_EXTI_LINE11,
        kExtiLine12 = LL_DMAMUX_REQ_GEN_EXTI_LINE12,
        kExtiLine13 = LL_DMAMUX_REQ_GEN_EXTI_LINE13,
        kExtiLine14 = LL_DMAMUX_REQ_GEN_EXTI_LINE14,
        kExtiLine15 = LL_DMAMUX_REQ_GEN_EXTI_LINE15,
        kDmaMuxCh0  = LL_DMAMUX_REQ_GEN_DMAMUX_CH0,
        kDmaMuxCh1  = LL_DMAMUX_REQ_GEN_DMAMUX_CH1,
        kDmaMuxCh2  = LL_DMAMUX_REQ_GEN_DMAMUX_CH2,
        kDmaMuxCh3  = LL_DMAMUX_REQ_GEN_DMAMUX_CH3,
        kLptim1Out  = LL_DMAMUX_REQ_GEN_LPTIM1_OUT,
    };

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
