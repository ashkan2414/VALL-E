#pragma once

#include "stm32g4xx_ll_i2c.h"
#include "valle/platform/hardware/dma.hpp"
#include "valle/platform/hardware/i2c/id.hpp"

namespace valle::platform
{

    // ============================================================================
    // ENUMERATIONS
    // ============================================================================
    enum class I2CTransferEndMode : uint32_t
    {
        kReload              = LL_I2C_MODE_RELOAD,
        kAutoEnd             = LL_I2C_MODE_AUTOEND,
        kSoftEnd             = LL_I2C_MODE_SOFTEND,
        kSMBusReload         = LL_I2C_MODE_SMBUS_RELOAD,
        kSMBusAutoEndNoPec   = LL_I2C_MODE_SMBUS_AUTOEND_NO_PEC,
        kSMBusSoftEndNoPec   = LL_I2C_MODE_SMBUS_SOFTEND_NO_PEC,
        kSMBusAutoEndWithPec = LL_I2C_MODE_SMBUS_AUTOEND_WITH_PEC,
        kSMBusSoftEndWithPec = LL_I2C_MODE_SMBUS_SOFTEND_WITH_PEC
    };

    enum class I2CTransferRequest : uint32_t
    {
        kNoStart = 0,
        kStart,
        kRestart
    };

    // ============================================================================
    // HARDWARE TRAITS
    // ============================================================================

    template <I2CControllerID tkID>
    struct I2CControllerTraits;

    template <>
    struct I2CControllerTraits<1>
    {
        static inline I2C_TypeDef* const skInstance        = I2C1;
        static constexpr uint32_t        skClock           = LL_APB1_GRP1_PERIPH_I2C1;
        static constexpr DMAMuxRequestID skDMAMuxRequestTx = DMAMuxRequestID::kI2C1Tx;
        static constexpr DMAMuxRequestID skDMAMuxRequestRx = DMAMuxRequestID::kI2C1Rx;
        static constexpr IRQn_Type       skEventIRQn       = I2C1_EV_IRQn;
        static constexpr IRQn_Type       skErrorIRQn       = I2C1_ER_IRQn;
    };

    template <>
    struct I2CControllerTraits<2>
    {
        static inline I2C_TypeDef* const skInstance        = I2C2;
        static constexpr uint32_t        skClock           = LL_APB1_GRP1_PERIPH_I2C2;
        static constexpr DMAMuxRequestID skDMAMuxRequestTx = DMAMuxRequestID::kI2C2Tx;
        static constexpr DMAMuxRequestID skDMAMuxRequestRx = DMAMuxRequestID::kI2C2Rx;
        static constexpr IRQn_Type       skEventIRQn       = I2C2_EV_IRQn;
        static constexpr IRQn_Type       skErrorIRQn       = I2C2_ER_IRQn;
    };

    template <>
    struct I2CControllerTraits<3>
    {
        static inline I2C_TypeDef* const skInstance        = I2C3;
        static constexpr uint32_t        skClock           = LL_APB1_GRP1_PERIPH_I2C3;
        static constexpr DMAMuxRequestID skDMAMuxRequestTx = DMAMuxRequestID::kI2C3Tx;
        static constexpr DMAMuxRequestID skDMAMuxRequestRx = DMAMuxRequestID::kI2C3Rx;
        static constexpr IRQn_Type       skEventIRQn       = I2C3_EV_IRQn;
        static constexpr IRQn_Type       skErrorIRQn       = I2C3_ER_IRQn;
    };

    template <>
    struct I2CControllerTraits<4>
    {
        static inline I2C_TypeDef* const skInstance        = I2C4;
        static constexpr uint32_t        skClock           = LL_APB1_GRP2_PERIPH_I2C4;
        static constexpr DMAMuxRequestID skDMAMuxRequestTx = DMAMuxRequestID::kI2C4Tx;
        static constexpr DMAMuxRequestID skDMAMuxRequestRx = DMAMuxRequestID::kI2C4Rx;
        static constexpr IRQn_Type       skEventIRQn       = I2C4_EV_IRQn;
        static constexpr IRQn_Type       skErrorIRQn       = I2C4_ER_IRQn;
    };

}  // namespace valle::platform