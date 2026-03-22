#pragma once

#include "valle/app/modules/ldc161x.hpp"
#include "valle/platform/core.hpp"
#include "valle/platform/devices/i2c.hpp"
#include "valle/platform/drivers/gpio/digital_in.hpp"

namespace valle::platform::app
{
    struct LDC161XSensorModuleI2CInterfaceConfig
    {
        uint32_t intb_interrupt_priority = 5;
    };

    template <typename TI2CSlaveDevice, uint8_t tkNumChannels, typename TINTBPin = GPIONullPinDevice>
        requires(valle::app::CLDC161XValidNumChannels<tkNumChannels>)
    class LDC161XSensorModuleI2CInterface
        : public valle::app::LDC161XSensorModuleI2CInterfaceX<
              LDC161XSensorModuleI2CInterface<TI2CSlaveDevice, tkNumChannels, TINTBPin>,
              LDC161XSensorModuleI2CInterfaceConfig>
    {
    public:
        using I2CSlaveDeviceT                  = TI2CSlaveDevice;
        using INTBPinT                         = TINTBPin;
        static constexpr uint8_t skNumChannels = tkNumChannels;
        static constexpr bool    skHasINTBPin  = !CNullGPIOPinDevice<INTBPinT>;

        using ConfigT                   = LDC161XSensorModuleI2CInterfaceConfig;
        constexpr static auto skAddress = TI2CSlaveDevice::skAddress;

        using InjectDevices = FilterNullDevices<TypeList<I2CSlaveDeviceT, INTBPinT>>;

    private:
        [[no_unique_address]] DeviceRef<I2CSlaveDeviceT>    m_i2c;
        ConditionalGPIODigitalInDriverT<INTBPinT>           m_intb_pin;
        valle::app::LDC161XSensorModuleI2CInterfaceCallback m_callback;

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

        template <typename... TArgs>
        explicit LDC161XSensorModuleI2CInterface(TArgs&&... args)
            : m_i2c(extract_device_ref<true, I2CSlaveDeviceT>(std::forward<TArgs>(args)...))
            , m_intb_pin(extract_device_ref<skHasINTBPin, INTBPinT>(std::forward<TArgs>(args)...))
        {
        }

        [[nodiscard]] bool init_impl(const ConfigT& config)
        {
            if (!m_i2c->init())
            {
                return false;
            }

            if constexpr (skHasINTBPin)
            {
                if (!m_intb_pin.init(GPIODigitalInConfig{
                        .pull = GPIOPullMode::kNoPull,
                        .interrupt =
                            GPIODigitalInInterruptConfig{
                                .priority = config.intb_interrupt_priority,
                                .trigger  = GPIOInputInterruptTrigger::kIntFallingEdge,  // INTB is
                                                                                         // active low
                                .action = GPIOInputInterruptAction::kInterrupt,
                            },
                        .inverted = false,
                    }))
                {
                    return false;
                }
            }

            return true;
        }

        void set_async_callback_impl(valle::app::LDC161XSensorModuleI2CInterfaceCallback&& callback)
        {
            m_callback = std::move(callback);
        }

        [[nodiscard]] bool transaction_in_progress_impl() const
        {
            return m_i2c->transaction_in_progress();
        }

        template <uint8_t tkWriteBytes>
        [[nodiscard]] bool register_write_blocking_impl(const std::span<const std::byte, tkWriteBytes>& write_data,
                                                        system::TimeoutMillis                           timeout_ms)
        {
            if (transaction_in_progress_impl())
            {
                return false;
            }

            // Prepare data in the stable member buffer and build the command
            prepare_write<tkWriteBytes>(write_data);

            // Submit
            const I2CBlockingTransactionError error = m_i2c->submit_transaction_blocking(m_write_commands, timeout_ms);

            return error == I2CBlockingTransactionError::kNone;
        }

        template <uint8_t tkReadBytes>
        [[nodiscard]] bool register_read_blocking_impl(valle::app::LDC161XReg                   reg,
                                                       const std::span<std::byte, tkReadBytes>& data_out,
                                                       system::TimeoutMillis                    timeout_ms)
        {
            if (transaction_in_progress_impl())
            {
                return false;
            }

            // Prepare the command using persistent arrays for both commands and data
            prepare_read<tkReadBytes>(reg);

            // Submit
            const I2CBlockingTransactionError error = m_i2c->submit_transaction_blocking(m_read_commands, timeout_ms);

            // If successful, copy the persistent RX buffer back to the user's output
            if (error == I2CBlockingTransactionError::kNone)
            {
                // Copy the persistent RX buffer back to the user's output
                std::copy(m_reg_rx_buffer.begin(), m_reg_rx_buffer.begin() + tkReadBytes, data_out.begin());
                return true;
            }

            return false;
        }

        template <uint8_t tkWriteBytes>
        [[nodiscard]] bool register_write_async_impl(const std::span<const std::byte, tkWriteBytes>& write_data)
        {
            if (transaction_in_progress_impl())
            {
                return false;
            }

            // Prepare data in the stable member buffer and build the command
            prepare_write<tkWriteBytes>(write_data);

            // Submit
            return m_i2c->submit_transaction(
                m_write_commands,
                [this](I2CCallbackType status)
                {
                    if (m_callback)
                    {
                        switch (status)
                        {
                            case I2CCallbackType::kComplete:
                                m_callback(valle::app::LDC161XSensorModuleI2CInterfaceCallbackStatus::kComplete);
                                break;
                            case I2CCallbackType::kAborted:
                                m_callback(valle::app::LDC161XSensorModuleI2CInterfaceCallbackStatus::kAborted);
                                break;
                            case I2CCallbackType::kError:
                                m_callback(valle::app::LDC161XSensorModuleI2CInterfaceCallbackStatus::kError);
                                break;
                        }
                    }
                });
        }

        template <uint8_t tkReadBytes>
        [[nodiscard]] bool register_read_async_impl(const valle::app::LDC161XReg reg)
        {
            if (transaction_in_progress_impl())
            {
                return false;
            }

            // Prepare the command using persistent arrays for both commands and data
            prepare_read<tkReadBytes>(reg);

            // Submit
            return m_i2c->submit_transaction(
                m_read_commands,
                [this](I2CCallbackType status)
                {
                    if (m_callback)
                    {
                        switch (status)
                        {
                            case I2CCallbackType::kComplete:
                                m_callback(valle::app::LDC161XSensorModuleI2CInterfaceCallbackStatus::kComplete);
                                break;
                            case I2CCallbackType::kAborted:
                                m_callback(valle::app::LDC161XSensorModuleI2CInterfaceCallbackStatus::kAborted);
                                break;
                            case I2CCallbackType::kError:
                                m_callback(valle::app::LDC161XSensorModuleI2CInterfaceCallbackStatus::kError);
                                break;
                        }
                    }
                });
        }

        template <uint8_t tkReadBytes>
        [[nodiscard]] std::optional<std::span<const std::byte>> get_async_read_data_impl() const
        {
            if (transaction_in_progress_impl())
            {
                return std::nullopt;
            }

            return std::span<const std::byte>(m_reg_rx_buffer.data(), tkReadBytes);
        }

    private:
        template <uint8_t tkWriteBytes>
        void prepare_write(const std::span<const std::byte, tkWriteBytes>& write_data)
        {
            static_assert(tkWriteBytes <= 3, "TX buffer max size exceeded");

            // Copy data from the stack into the persistent class buffer
            std::copy(write_data.begin(), write_data.end(), m_reg_tx_buffer.begin());

            // Build the command using the persistent class command array and TX buffer
            m_write_commands =
                I2CTransactionGenerator<>::begin().write(std::span(m_reg_tx_buffer.data(), tkWriteBytes)).build();
        }

        template <uint8_t tkReadBytes>
        void prepare_read(const valle::app::LDC161XReg reg)
        {
            static_assert(sizeof(reg) == 1, "Register address must be 1 byte");
            static_assert(tkReadBytes <= 2, "RX buffer max size exceeded");

            // Store the register address in the persistent TX buffer
            m_reg_tx_buffer[0] = static_cast<std::byte>(reg);

            // Build the command using persistent arrays for both commands and data
            m_read_commands = I2CTransactionGenerator<>::begin()
                                  .write_restart(std::span(m_reg_tx_buffer.data(), 1))
                                  .read(std::span(m_reg_rx_buffer.data(), tkReadBytes))
                                  .build();
        }
    };

    template <uint8_t tkNumChannels>
        requires(valle::app::CLDC161XValidNumChannels<tkNumChannels>)
    using LDC161XSensorModuleConfig =
        valle::app::LDC161XSensorModuleConfigX<LDC161XSensorModuleI2CInterfaceConfig, tkNumChannels>;

    template <typename TI2CSlaveDevice, uint8_t tkNumChannels, typename TINTBPin = GPIONullPinDevice>
        requires(valle::app::CLDC161XValidNumChannels<tkNumChannels>)
    using LDC161XSensorModule =
        valle::app::LDC161XSensorModuleX<LDC161XSensorModuleI2CInterface<TI2CSlaveDevice, tkNumChannels, TINTBPin>,
                                         tkNumChannels>;

}  // namespace valle::platform::app