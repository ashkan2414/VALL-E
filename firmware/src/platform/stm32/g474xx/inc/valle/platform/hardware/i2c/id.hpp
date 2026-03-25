#pragma once

#include <cstdint>
#include <variant>

namespace valle::platform
{

    enum class I2cPeripheralId : uint8_t
    {
        kI2c1 = 1,
        kI2c2 = 2,
        kI2c3 = 3,
        kI2c4 = 4,
    };

    template <bool tkIs10BitAddress>
    struct I2cSlaveAddress
    {
        using ValueT                      = std::conditional_t<tkIs10BitAddress, uint16_t, uint8_t>;
        static constexpr bool   skIs10Bit = tkIs10BitAddress;
        static constexpr ValueT skMask    = tkIs10BitAddress ? 0x3FF : 0x7F;

        ValueT address = 0U;

        explicit constexpr I2cSlaveAddress() = default;

        explicit constexpr I2cSlaveAddress(const ValueT addr) : address(addr & skMask)
        {
        }

        // implicit conversion to raw address
        explicit constexpr operator ValueT() const
        {
            return address;
        }
    };

    using I2c7BitSlaveAddress    = I2cSlaveAddress<false>;
    using I2c10BitSlaveAddress   = I2cSlaveAddress<true>;
    using I2cSlaveAddressVariant = std::variant<I2c7BitSlaveAddress, I2c10BitSlaveAddress>;
    using I2cDmaTransferContext  = I2cSlaveAddressVariant;

}  // namespace valle::platform