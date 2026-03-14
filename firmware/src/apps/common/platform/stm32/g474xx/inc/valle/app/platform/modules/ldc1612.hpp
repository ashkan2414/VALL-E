#pragma once

#include "valle/app/modules/ldc1612.hpp"
#include "valle/platform/core.hpp"
#include "valle/platform/devices/i2c.hpp"

namespace valle::platform::app
{
    struct LDC161XSensorModuleI2CInterfaceConfig
    {
    };

    template <typename TI2CSlaveDevice, uint8_t tkNumChannels>
        requires(valle::app::CLDC161XValidNumChannels<tkNumChannels>)
    class LDC161XSensorModuleI2CInterface : public valle::app::LDC161XSensorModuleI2CInterfaceX<
                                                LDC161XSensorModuleI2CInterface<TI2CSlaveDevice, tkNumChannels>,
                                                LDC161XSensorModuleI2CInterfaceConfig>
    {
    public:
        using ConfigT = LDC161XSensorModuleI2CInterfaceConfig;

        using I2CSlaveDeviceT           = TI2CSlaveDevice;
        constexpr static auto skAddress = TI2CSlaveDevice::skAddress;

        using InjectDevices = TypeList<I2CSlaveDeviceT>;

    private:
        [[no_unique_address]] DeviceRef<I2CSlaveDeviceT> m_i2c;

        // --- Persistent Memory for DMA ---
        // These must live in the class so DMA can access them after the function returns
        std::array<std::byte, 3> m_reg_tx_buffer;
        std::array<std::byte, 2> m_reg_rx_buffer;

        // --- Command Queue State ---
        // The CommandBuffer array must stay alive while the I2C ISR is processing it
        I2CCommandBuffer<2> m_read_commands;
        I2CCommandBuffer<1> m_write_commands;

    public:
        LDC161XSensorModuleI2CInterface() = delete;

        explicit LDC161XSensorModuleI2CInterface(DeviceRef<I2CSlaveDeviceT>&& slave_device)
            : m_i2c(std::move(slave_device))
        {
        }

        [[nodiscard]] bool init_impl(const ConfigT& config)
        {
            return m_i2c->init();
        }

        template <uint8_t tkWriteBytes>
        [[nodiscard]] bool register_write_blocking_impl(const std::span<const std::byte, tkWriteBytes>& write_data,
                                                        system::TimeoutMillis                           timeout_ms)
        {
            // Build the command sequence (Single Write)
            auto write_commands = I2CTransactionGenerator<>::begin().write(std::span(write_data)).build();

            // Submit
            const I2CBlockingTransactionError error = m_i2c->submit_transaction_blocking(write_commands, timeout_ms);
            return error == I2CBlockingTransactionError::kNone;
        }

        template <uint8_t tkReadBytes>
        [[nodiscard]] bool register_read_blocking_impl(valle::app::LDC161XReg                   reg,
                                                       const std::span<std::byte, tkReadBytes>& data_out,
                                                       system::TimeoutMillis                    timeout_ms)
        {
            static_assert(sizeof(reg) == 1, "Register address must be 1 byte");

            // Build the "Combined Message" sequence
            // We use write_restart to hold the bus stretched before the read
            auto read_commands = I2CTransactionGenerator<>::begin()
                                     .write_restart(std::span(reinterpret_cast<std::byte*>(&reg), 1))
                                     .read(data_out)
                                     .build();

            // Submit
            const I2CBlockingTransactionError error = m_i2c->submit_transaction_blocking(read_commands, timeout_ms);
            return error == I2CBlockingTransactionError::kNone;
        }
    };

    template <uint8_t tkNumChannels>
        requires(valle::app::CLDC161XValidNumChannels<tkNumChannels>)
    using LDC161XSensorModuleConfig =
        valle::app::LDC161XSensorModuleConfigX<LDC161XSensorModuleI2CInterfaceConfig, tkNumChannels>;

    template <typename TI2CSlaveDevice, uint8_t tkNumChannels>
        requires(valle::app::CLDC161XValidNumChannels<tkNumChannels>)
    using LDC161XSensorModule =
        valle::app::LDC161XSensorModuleX<LDC161XSensorModuleI2CInterface<TI2CSlaveDevice, tkNumChannels>,
                                         tkNumChannels>;

}  // namespace valle::platform::app