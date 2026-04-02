#pragma once

#include "stm32g4xx_ll_i2c.h"
#include "valle/platform/hardware/dma.hpp"
#include "valle/platform/hardware/i2c/id.hpp"

namespace valle::platform
{

    // ============================================================================
    // ENUMERATIONS
    // ============================================================================
    enum class I2cTransferEndMode : uint32_t
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

    enum class I2cTransferRequest : uint32_t
    {
        kNoStart = 0,
        kStart,
        kRestart
    };

    // ============================================================================
    // HARDWARE TRAITS
    // ============================================================================

    template <I2cControllerId tkControllerId>
    struct I2cControllerTraits;

    template <>
    struct I2cControllerTraits<I2cControllerId::kI2c1>
    {
        static inline I2c_TypeDef* const skInstance        = I2c1;
        static constexpr uint32_t        skClock           = LL_APB1_GRP1_PERIPH_I2C1;
        static constexpr DmaMuxRequestId skDmaMuxRequestTx = DmaMuxRequestId::kI2c1Tx;
        static constexpr DmaMuxRequestId skDmaMuxRequestRx = DmaMuxRequestId::kI2c1Rx;
        static constexpr IRQn_Type       skEventIRQn       = I2c1_EV_IRQn;
        static constexpr IRQn_Type       skErrorIRQn       = I2c1_ER_IRQn;
    };

    template <>
    struct I2cControllerTraits<I2cControllerId::kI2c2>
    {
        static inline I2c_TypeDef* const skInstance        = I2c2;
        static constexpr uint32_t        skClock           = LL_APB1_GRP1_PERIPH_I2C2;
        static constexpr DmaMuxRequestId skDmaMuxRequestTx = DmaMuxRequestId::kI2c2Tx;
        static constexpr DmaMuxRequestId skDmaMuxRequestRx = DmaMuxRequestId::kI2c2Rx;
        static constexpr IRQn_Type       skEventIRQn       = I2c2_EV_IRQn;
        static constexpr IRQn_Type       skErrorIRQn       = I2c2_ER_IRQn;
    };

    template <>
    struct I2cControllerTraits<I2cControllerId::kI2c3>
    {
        static inline I2c_TypeDef* const skInstance        = I2c3;
        static constexpr uint32_t        skClock           = LL_APB1_GRP1_PERIPH_I2C3;
        static constexpr DmaMuxRequestId skDmaMuxRequestTx = DmaMuxRequestId::kI2c3Tx;
        static constexpr DmaMuxRequestId skDmaMuxRequestRx = DmaMuxRequestId::kI2c3Rx;
        static constexpr IRQn_Type       skEventIRQn       = I2c3_EV_IRQn;
        static constexpr IRQn_Type       skErrorIRQn       = I2c3_ER_IRQn;
    };

    template <>
    struct I2cControllerTraits<I2cControllerId::kI2c4>
    {
        static inline I2c_TypeDef* const skInstance        = I2c4;
        static constexpr uint32_t        skClock           = LL_APB1_GRP2_PERIPH_I2C4;
        static constexpr DmaMuxRequestId skDmaMuxRequestTx = DmaMuxRequestId::kI2c4Tx;
        static constexpr DmaMuxRequestId skDmaMuxRequestRx = DmaMuxRequestId::kI2c4Rx;
        static constexpr IRQn_Type       skEventIRQn       = I2c4_EV_IRQn;
        static constexpr IRQn_Type       skErrorIRQn       = I2c4_ER_IRQn;
    };

}  // namespace valle::platform