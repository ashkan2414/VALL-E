#pragma once

#include <cstdint>
#include <variant>

namespace valle::platform
{

    using I2CControllerID = uint8_t;

    template <I2CControllerID tkControllerID>
    constexpr bool kValidI2CControllerID = (tkControllerID >= 1 && tkControllerID <= 4);

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
        explicit constexpr operator ValueT() const
        {
            return address;
        }
    };

    using I2C7BitSlaveAddress    = I2CSlaveAddress<false>;
    using I2C10BitSlaveAddress   = I2CSlaveAddress<true>;
    using I2CSlaveAddressVariant = std::variant<I2C7BitSlaveAddress, I2C10BitSlaveAddress>;
    using I2CDMATransferContext  = I2CSlaveAddressVariant;

}  // namespace valle::platform