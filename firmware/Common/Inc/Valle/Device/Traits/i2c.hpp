#pragma once

#include "Valle/Device/Traits/dma.hpp"
#include "stm32g4xx_ll_i2c.h"

namespace valle
{
    // ============================================================================
    // CHECKS
    // ============================================================================
    using I2CControllerID = uint8_t;

    template <I2CControllerID tkControllerID>
    constexpr bool kValidI2CControllerID = (tkControllerID >= 1 && tkControllerID <= 4);

    // ============================================================================
    // HARDWARE TRAITS
    // ============================================================================

    template <I2CControllerID tkID>
    struct I2ControllerTraits;

    template <>
    struct I2ControllerTraits<1>
    {
        static inline I2C_TypeDef* const skInstance  = I2C1;
        static constexpr uint32_t        skClock     = LL_APB1_GRP1_PERIPH_I2C1;
        static constexpr DMARequestID    skTxRequest = DMARequestID::kI2C1Tx;
        static constexpr DMARequestID    skRxRequest = DMARequestID::kI2C1Rx;
        static constexpr IRQn_Type       skEventIRQn = I2C1_EV_IRQn;
        static constexpr IRQn_Type       skErrorIRQn = I2C1_ER_IRQn;
    };

    template <>
    struct I2ControllerTraits<2>
    {
        static inline I2C_TypeDef* const skInstance  = I2C2;
        static constexpr uint32_t        skClock     = LL_APB1_GRP1_PERIPH_I2C2;
        static constexpr DMARequestID    skTxRequest = DMARequestID::kI2C2Tx;
        static constexpr DMARequestID    skRxRequest = DMARequestID::kI2C2Rx;
        static constexpr IRQn_Type       skEventIRQn = I2C2_EV_IRQn;
        static constexpr IRQn_Type       skErrorIRQn = I2C2_ER_IRQn;
    };

    template <>
    struct I2ControllerTraits<3>
    {
        static inline I2C_TypeDef* const skInstance  = I2C3;
        static constexpr uint32_t        skClock     = LL_APB1_GRP1_PERIPH_I2C3;
        static constexpr DMARequestID    skTxRequest = DMARequestID::kI2C3Tx;
        static constexpr DMARequestID    skRxRequest = DMARequestID::kI2C3Rx;
        static constexpr IRQn_Type       skEventIRQn = I2C3_EV_IRQn;
        static constexpr IRQn_Type       skErrorIRQn = I2C3_ER_IRQn;
    };

    template <>
    struct I2ControllerTraits<4>
    {
        static inline I2C_TypeDef* const skInstance  = I2C4;
        static constexpr uint32_t        skClock     = LL_APB1_GRP2_PERIPH_I2C4;
        static constexpr DMARequestID    skTxRequest = DMARequestID::kI2C4Tx;
        static constexpr DMARequestID    skRxRequest = DMARequestID::kI2C4Rx;
        static constexpr IRQn_Type       skEventIRQn = I2C4_EV_IRQn;
        static constexpr IRQn_Type       skErrorIRQn = I2C4_ER_IRQn;
    };

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
    // STRUCTS
    // ============================================================================
    template <bool tkIs10BitAddress>
    struct I2CSlaveAddress
    {
        using ValueT                      = std::conditional_t<tkIs10BitAddress, uint16_t, uint8_t>;
        static constexpr bool   skIs10Bit = tkIs10BitAddress;
        static constexpr ValueT skMask    = tkIs10BitAddress ? 0x3FF : 0x7F;

        ValueT address = 0U;

        explicit constexpr I2CSlaveAddress() = default;

        explicit constexpr I2CSlaveAddress(const ValueT addr) : address(addr & skMask)
        {
        }

        // implicit conversion to raw address
        constexpr operator ValueT() const
        {
            return address;
        }
    };

    using I2C7BitSlaveAddress    = I2CSlaveAddress<false>;
    using I2C10BitSlaveAddress   = I2CSlaveAddress<true>;
    using I2CSlaveAddressVariant = std::variant<I2C7BitSlaveAddress, I2C10BitSlaveAddress>;
    using I2CDMATransferContext  = I2CSlaveAddressVariant;

}  // namespace valle