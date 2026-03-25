#pragma once

#include <delegate/delegate.h>

#include <cstdint>
#include <expected>
#include <optional>
#include <span>
#include <variant>

#include "valle/platform/core.hpp"
#include "valle/platform/devices/dma.hpp"
#include "valle/platform/drivers/gpio/alternate_function.hpp"
#include "valle/platform/hardware/i2c.hpp"
#include "valle/utils/circular_queue.hpp"
#include "valle/utils/sync.hpp"

namespace valle::platform
{
    // ============================================================================
    // I2C CONTROLLER (THE UNIQUE DEVICE)
    // ============================================================================
    // ---------------------------------------------------------------------------
    // COMPILE TimE CONFIGURATIONS
    // ---------------------------------------------------------------------------
    struct I2cControllerCTDefaultConfig
    {
        using DmaChannelRxT = DmaNullChannelDevice;
        using DmaChannelTxT = DmaNullChannelDevice;
        using SCLPinT       = GpioNullPinDevice;
        using SDAPinT       = GpioNullPinDevice;
        using SMBAPinT      = GpioNullPinDevice;
    };

    template <typename T, I2cPeripheralId tkPeripheralId>
    concept CValidI2cControllerCTConfig =
        requires {
            typename T::DmaChannelRxT;
            typename T::DmaChannelTxT;
            typename T::SCLPinT;
            typename T::SDAPinT;
            typename T::SMBAPinT;
        } &&
        ((CNullDmaChannel<typename T::DmaChannelRxT> && CNullDmaChannel<typename T::DmaChannelTxT>) ||
         (CDmaChannelDevice<typename T::DmaChannelRxT> && CDmaChannelDevice<typename T::DmaChannelTxT>)) &&
        (CNullGpioPinDevice<typename T::SCLPinT> || CValidI2cControllerPin<tkPeripheralId,
                                                                           I2cControllerGpioPinType::kSCL,
                                                                           T::SCLPinT::skPortId,
                                                                           T::SCLPinT::skPinId>) &&
        (CNullGpioPinDevice<typename T::SDAPinT> || CValidI2cControllerPin<tkPeripheralId,
                                                                           I2cControllerGpioPinType::kSDA,
                                                                           T::SDAPinT::skPortId,
                                                                           T::SDAPinT::skPinId>) &&
        (CNullGpioPinDevice<typename T::SMBAPinT> || CValidI2cControllerPin<tkPeripheralId,
                                                                            I2cControllerGpioPinType::kSMBA,
                                                                            T::SMBAPinT::skPortId,
                                                                            T::SMBAPinT::skPinId>);

    template <I2cPeripheralId tkPeripheralId>
    struct I2cControllerCTConfigRegistry
    {
        static constexpr auto skConfig = I2cControllerCTDefaultConfig{};
    };

#define VALLE_DEFINE_I2C_CONTROLLER_CT_CONFIG(tkPeripheralId, config)                        \
    namespace valle::platform                                                                \
    {                                                                                        \
        template <>                                                                          \
        struct I2cControllerCTConfigRegistry<(tkPeripheralId)>                               \
        {                                                                                    \
            static constexpr auto skConfig = (config);                                       \
            static_assert(CValidI2cControllerCTConfig<decltype(skConfig), (tkPeripheralId)>, \
                          "Invalid I2C Controller CT Config");                               \
        };                                                                                   \
    }

    // ----------------------------------------------------------------------------
    // CONFIGURATIONS
    // ----------------------------------------------------------------------------

    /**
     * @brief Configuration for the Bus (Speed)
     */
    struct I2cControllerConfig
    {
        // Pre-calculated timing value (TODO: create a helper to calculate this from desired I2C speed and source clock)
        uint32_t    timing_reg           = 0x40621236;  // 400kHz @ 170MHz APB1 clock (values from STM32CubeMX)
        bool        enable_analog_filter = true;
        DmaPriority dma_priority         = DmaPriority::kHigh;
    };

    /**
     * @brief Configuration for a single I2C Transfer
     */
    struct I2cTransferConfig
    {
        I2cSlaveAddressVariant slave_address{};
        I2cTransferEndMode     end_mode = I2cTransferEndMode::kAutoEnd;
        I2cTransferRequest     request  = I2cTransferRequest::kStart;
    };

    // -----------------------------------------------------------------------------
    // HELPERS
    // -----------------------------------------------------------------------------
    namespace detail
    {
        template <I2cPeripheralId tkPeripheralId, I2cControllerGpioPinType tkPinType, typename TPinDevice>
        struct I2cControllerGpioPinDriverHelper
        {
            using type = GpioAlternateFunctionDriver<
                TPinDevice,
                kI2cControllerPinAF<tkPeripheralId, tkPinType, TPinDevice::skPortId, TPinDevice::skPinId>>;
        };

        template <I2cPeripheralId tkPeripheralId, I2cControllerGpioPinType tkPinType>
        struct I2cControllerGpioPinDriverHelper<tkPeripheralId, tkPinType, GpioNullPinDevice>
        {
            using type = std::monostate;
        };
    }  // namespace detail

    template <I2cPeripheralId tkPeripheralId, I2cControllerGpioPinType tkPinType, typename TPinDevice>
    using I2cControllerGpioPinDriver =
        typename detail::I2cControllerGpioPinDriverHelper<tkPeripheralId, tkPinType, TPinDevice>::type;

    // ---------------------------------------------------------------------------
    // DEVICE CLASS
    // ---------------------------------------------------------------------------

    /**
     * @brief Logic for the I2C Controller
     * @note This device is unique because despite a bus being shared, its state can only be managed
     * by one slave at a time. Users should write an arbitration layer if multiple slaves are to be used.
     */
    template <I2cPeripheralId tkPeripheralId>
    class I2cControllerDevice
    {
    public:
        struct Descriptor : public UniqueDeviceDescriptor
        {
        };

        static constexpr I2cPeripheralId skPeripheralId = tkPeripheralId;
        using ControllerTraitsT                         = I2cControllerTraits<skPeripheralId>;
        using InterruptControllerT                      = I2cInterruptController<skPeripheralId>;

        using CTConfigRegistryT          = I2cControllerCTConfigRegistry<skPeripheralId>;
        static constexpr auto skCTConfig = CTConfigRegistryT::skConfig;
        using CTConfigT                  = decltype(skCTConfig);

        using DmaChannelRxT            = typename CTConfigT::DmaChannelRxT;
        using DmaChannelTxT            = typename CTConfigT::DmaChannelTxT;
        static constexpr bool skHasDMA = !CNullDmaChannel<DmaChannelRxT> && !CNullDmaChannel<DmaChannelTxT>;

        template <I2cControllerGpioPinType tkPinType>
        using PinDeviceT = std::conditional_t<tkPinType == I2cControllerGpioPinType::kSCL,
                                              typename CTConfigT::SCLPinT,
                                              std::conditional_t<tkPinType == I2cControllerGpioPinType::kSDA,
                                                                 typename CTConfigT::SDAPinT,
                                                                 typename CTConfigT::SMBAPinT>>;

        using SCLPinT  = PinDeviceT<I2cControllerGpioPinType::kSCL>;
        using SDAPinT  = PinDeviceT<I2cControllerGpioPinType::kSDA>;
        using SMBAPinT = PinDeviceT<I2cControllerGpioPinType::kSMBA>;

        // Check availability
        template <I2cControllerGpioPinType tkPinType>
        static constexpr bool skHasPin = !CNullGpioPinDevice<PinDeviceT<tkPinType>>;

        static constexpr bool skHasSCLPin  = skHasPin<I2cControllerGpioPinType::kSCL>;
        static constexpr bool skHasSDAPin  = skHasPin<I2cControllerGpioPinType::kSDA>;
        static constexpr bool skHasSMBAPin = skHasPin<I2cControllerGpioPinType::kSMBA>;

        template <I2cControllerGpioPinType tkPinType>
        using PinDriverT = I2cControllerGpioPinDriver<tkPeripheralId, tkPinType, PinDeviceT<tkPinType>>;

        using SCLPinDriverT  = PinDriverT<I2cControllerGpioPinType::kSCL>;
        using SDAPinDriverT  = PinDriverT<I2cControllerGpioPinType::kSDA>;
        using SMBAPinDriverT = PinDriverT<I2cControllerGpioPinType::kSMBA>;

        using InjectDevices = FilterNullDevices<TypeList<DmaChannelRxT, DmaChannelTxT, SCLPinT, SDAPinT, SMBAPinT>>;

    private:
        static inline uint32_t kReadRegisterAddr  = reinterpret_cast<uint32_t>(&ControllerTraitsT::skInstance->RXDR);
        static inline uint32_t kWriteRegisterAddr = reinterpret_cast<uint32_t>(&ControllerTraitsT::skInstance->TXDR);

        [[no_unique_address]] ConditionalDeviceRef<skHasDMA, DmaChannelRxT> m_dma_rx;
        [[no_unique_address]] ConditionalDeviceRef<skHasDMA, DmaChannelTxT> m_dma_tx;
        SCLPinDriverT                                                       m_scl_pin{};
        SDAPinDriverT                                                       m_sda_pin{};
        SMBAPinDriverT                                                      m_smba_pin{};

    public:
        template <typename... TArgs>
        explicit I2cControllerDevice(TArgs&&... args)
            : m_dma_rx(extract_device_ref<skHasDMA, DmaChannelRxT>(std::forward<TArgs>(args)...))
            , m_dma_tx(extract_device_ref<skHasDMA, DmaChannelTxT>(std::forward<TArgs>(args)...))
            , m_scl_pin(extract_device_ref<skHasSCLPin, SCLPinT>(std::forward<TArgs>(args)...))
            , m_sda_pin(extract_device_ref<skHasSDAPin, SDAPinT>(std::forward<TArgs>(args)...))
            , m_smba_pin(extract_device_ref<skHasSMBAPin, SMBAPinT>(std::forward<TArgs>(args)...))
        {
        }

        // ------------------------------------------------------------------------
        // Initialization
        // ------------------------------------------------------------------------
        [[nodiscard]] bool init(const I2cControllerConfig& config)
        {
            if constexpr (skHasDMA)
            {
                // Configure DMA Requests
                dma_init<true>(config.dma_priority);   // RX
                dma_init<false>(config.dma_priority);  // TX
            }

            // Initialize Pins
            if (!m_scl_pin.init(GpioAlternateFunctionConfig{
                    .mode  = GpioAlternateFunctionMode::kOpenDrain,
                    .speed = GpioSpeedMode::kHigh,
                    .pull  = GpioPullMode::kPullUp,
                }))
            {
                return false;
            }

            if (!m_sda_pin.init(GpioAlternateFunctionConfig{
                    .mode  = GpioAlternateFunctionMode::kOpenDrain,
                    .speed = GpioSpeedMode::kHigh,
                    .pull  = GpioPullMode::kPullUp,
                }))
            {
                return false;
            }

            // Enable Clock and Configure I2C Peripheral
            LL_APB1_GRP1_EnableClock(ControllerTraitsT::skClock);

            LL_I2C_Disable(ControllerTraitsT::skInstance);

            LL_I2C_SetTiming(ControllerTraitsT::skInstance, config.timing_reg);
            LL_I2C_SetMode(ControllerTraitsT::skInstance, LL_I2C_MODE_I2C);
            if (config.enable_analog_filter)
            {
                LL_I2C_EnableAnalogFilter(ControllerTraitsT::skInstance);
            }
            // TODO: digital filter?

            LL_I2C_EnableAutoEndMode(ControllerTraitsT::skInstance);
            LL_I2C_EnableClockStretching(ControllerTraitsT::skInstance);

            LL_I2C_SetOwnAddress1(ControllerTraitsT::skInstance, 0, LL_I2C_OWNADDRESS1_7BIT);
            LL_I2C_DisableOwnAddress1(ControllerTraitsT::skInstance);
            LL_I2C_SetOwnAddress2(ControllerTraitsT::skInstance, 0, LL_I2C_OWNADDRESS2_NOMASK);
            LL_I2C_DisableOwnAddress2(ControllerTraitsT::skInstance);
            LL_I2C_DisableGeneralCall(ControllerTraitsT::skInstance);

            if constexpr (skHasDMA)
            {
                // Configure DMA Requests
                LL_I2C_EnableDMAReq_RX(ControllerTraitsT::skInstance);
                LL_I2C_EnableDMAReq_TX(ControllerTraitsT::skInstance);
            }

            LL_I2C_Enable(ControllerTraitsT::skInstance);

            return true;
        }

        [[nodiscard]] bool init_smba_pin(const GpioAlternateFunctionConfig& pin_cfg)
            requires(skHasSMBAPin)
        {
            return m_smba_pin.init(pin_cfg);
        }

        void reset()
        {
            if constexpr (skHasDMA)
            {
                m_dma_rx->stop();
                m_dma_tx->stop();
            }

            LL_I2C_Disable(ControllerTraitsT::skInstance);

            __NOP();
            __NOP();
            __NOP();  // Small delay to ensure gates are settled

            if constexpr (skHasDMA)
            {
                // Configure DMA Requests
                LL_I2C_EnableDMAReq_RX(ControllerTraitsT::skInstance);
                LL_I2C_EnableDMAReq_TX(ControllerTraitsT::skInstance);
            }

            LL_I2C_Enable(ControllerTraitsT::skInstance);
        }

        // ------------------------------------------------------------------------
        // Status
        // ------------------------------------------------------------------------

        bool is_busy() const
        {
            return LL_I2C_IsActiveFlag_BUSY(ControllerTraitsT::skInstance) != 0;
        }

        // ------------------------------------------------------------------------
        // Interrupts
        // ------------------------------------------------------------------------
        /**
         * @brief Enable interrupts for this channel.
         * @param config Configuration for I2C Interrupts.
         */
        void enable_interrupts(const I2cInterruptConfig& config)
        {
            InterruptControllerT::enable_interrupts(config);
        }

        /**
         * @brief Enable DMA interrupts for this channel.
         * @param config Configuration for DMA Interrupts.
         */
        void enable_tx_dma_interrupts(const DmaChannelInterruptConfig& config)
            requires(skHasDMA)
        {
            m_dma_tx->enable_interrupts(config);
        }

        /**
         * @brief Enable DMA interrupts for this channel.
         * @param config Configuration for DMA Interrupts.
         */
        void enable_rx_dma_interrupts(const DmaChannelInterruptConfig& config)
            requires(skHasDMA)
        {
            m_dma_rx->enable_interrupts(config);
        }

        /**
         * @brief Disable interrupts for this channel.
         */
        void disable_interrupts()
        {
            InterruptControllerT::disable_interrupts();
        }

        // ------------------------------------------------------------------------
        // Main API
        // ------------------------------------------------------------------------
        template <bool tkRead>
        void initiate_transfer(const I2cTransferConfig& transfer_config, const uint32_t buffer_size)
        {
            const uint16_t raw_address = std::visit(
                Overloaded{[](const I2c7BitSlaveAddress& addr) { return static_cast<uint16_t>(addr.address << 1); },
                           [](const I2c10BitSlaveAddress& addr) { return static_cast<uint16_t>(addr.address); }},
                transfer_config.slave_address);

            const bool is_10bit_address = std::holds_alternative<I2c10BitSlaveAddress>(transfer_config.slave_address);
            const uint32_t address_size = is_10bit_address ? LL_I2C_ADDRESSING_MODE_10BIT : LL_I2C_ADDRESSING_MODE_7BIT;

            uint32_t request = 0;
            switch (transfer_config.request)
            {
                case I2cTransferRequest::kNoStart:
                    request = LL_I2C_GENERATE_NOSTARTSTOP;
                    break;
                case I2cTransferRequest::kStart:
                    request = tkRead ? LL_I2C_GENERATE_START_READ : LL_I2C_GENERATE_START_WRITE;
                    break;
                case I2cTransferRequest::kRestart:
                    request = is_10bit_address
                                  ? (tkRead ? LL_I2C_GENERATE_RESTART_10BIT_READ : LL_I2C_GENERATE_RESTART_10BIT_WRITE)
                                  : (tkRead ? LL_I2C_GENERATE_RESTART_7BIT_READ : LL_I2C_GENERATE_RESTART_7BIT_WRITE);
                    break;
                default:
                    return;  // Invalid request, TODO: error handling
            }

            LL_I2C_HandleTransfer(ControllerTraitsT::skInstance,
                                  raw_address,
                                  address_size,
                                  buffer_size,
                                  static_cast<uint32_t>(transfer_config.end_mode),
                                  request);
        }

        void stop_transfer()
        {
            LL_I2C_HandleTransfer(ControllerTraitsT::skInstance,
                                  0,
                                  LL_I2C_ADDRESSING_MODE_7BIT,
                                  0,
                                  LL_I2C_MODE_SOFTEND,
                                  LL_I2C_GENERATE_STOP);
        }

        // ------------------------------------------------------------------------
        // DMA API
        // ------------------------------------------------------------------------
        inline bool initiate_dma_read(const I2cTransferConfig& transfer_config, const std::span<std::byte>& buffer)
            requires(skHasDMA)
        {
            return initiate_dma_transfer<true>(transfer_config, buffer);
        }

        inline bool initiate_dma_write(const I2cTransferConfig&          transfer_config,
                                       const std::span<const std::byte>& buffer)
            requires(skHasDMA)
        {
            return initiate_dma_transfer<false>(transfer_config, buffer);
        }

        template <bool tkRead>
        bool initiate_dma_transfer(const I2cTransferConfig& transfer_config,
                                   const std::span<std::conditional_t<tkRead, std::byte, const std::byte>>& buffer)
            requires(skHasDMA)
        {
            // Configure DMA Transfer
            dma_configure_transfer<tkRead>(buffer);

            // Initiate I2C Transfer
            initiate_transfer<tkRead>(transfer_config, static_cast<uint32_t>(buffer.size()));

            return true;
        }

    private:
        template <bool tkRead>
        inline void dma_init(const DmaPriority& priority)
            requires(skHasDMA)
        {
            constexpr DmaDirection    direction = tkRead ? DmaDirection::kPeriphToMem : DmaDirection::kMemToPeriph;
            constexpr DmaMuxRequestId request_id =
                tkRead ? ControllerTraitsT::skDmaMuxRequestRx : ControllerTraitsT::skDmaMuxRequestTx;

            DmaChannelConfig dma_cfg = DmaChannelConfig{
                .direction         = direction,
                .priority          = priority,
                .mode              = DmaMode::kNormal,
                .periph_data_width = DmaDataWidth::kByte,
                .memory_data_width = DmaDataWidth::kByte,
                .inc_periph        = false,
                .inc_memory        = true,
                .request_id        = request_id,
            };

            if constexpr (tkRead)
            {
                m_dma_rx->init(dma_cfg);
            }
            else
            {
                m_dma_tx->init(dma_cfg);
            }
        }

        template <bool tkRead>
        inline void dma_configure_transfer(
            const std::span<std::conditional_t<tkRead, std::byte, const std::byte>>& data)
            requires(skHasDMA)
        {
            if constexpr (tkRead)
            {
                m_dma_rx->start_periph_to_mem(
                    (uint32_t)LL_I2C_DMA_GetRegAddr(ControllerTraitsT::skInstance, LL_I2C_DMA_REG_DATA_RECEIVE),
                    reinterpret_cast<uint32_t>(data.data()),
                    static_cast<uint32_t>(data.size()));
            }
            else
            {
                m_dma_tx->start_mem_to_periph(
                    reinterpret_cast<uint32_t>(data.data()),
                    (uint32_t)LL_I2C_DMA_GetRegAddr(ControllerTraitsT::skInstance, LL_I2C_DMA_REG_DATA_TRANSMIT),
                    static_cast<uint32_t>(data.size()));
            }
        }
    };

    // -----------------------------------------------------------------------------
    // DEVICE ALIASES
    // -----------------------------------------------------------------------------
    using I2c1ControllerDevice = I2cControllerDevice<I2cPeripheralId::kI2c1>;
    using I2c2ControllerDevice = I2cControllerDevice<I2cPeripheralId::kI2c2>;
    using I2c3ControllerDevice = I2cControllerDevice<I2cPeripheralId::kI2c3>;
    using I2c4ControllerDevice = I2cControllerDevice<I2cPeripheralId::kI2c4>;

    // ==========================================================================
    // I2C COMMAND BUFFER DEVICE (SHARED DEVICE)
    // ==========================================================================

    // --------------------------------------------------------------------------------
    // CONFIGURATIONS
    // --------------------------------------------------------------------------------

    /**
     * @brief Configuration for I2C Command Buffer Device
     */
    struct I2cCommandBufferDeviceConfig
    {
        I2cControllerConfig controller_config{};
        uint8_t             event_int_priority = 5;  // NVIC Priority  // NOLINT(readability-magic-numbers)
        uint8_t             error_int_priority = 5;  // NVIC Priority  // NOLINT(readability-magic-numbers)
    };

    // --------------------------------------------------------------------------
    // I2C Command Buffer
    // ---------------------------------------------------------------------------
    /**
     * @brief I2C Write command.
     * Generates a start and stop condition.
     *
     */
    struct I2cWriteCommand
    {
        std::span<const std::byte> data{};
    };

    /**
     * @brief I2C Read command.
     * Generates a start and stop condition with the read bit set.
     */
    struct I2cReadCommand
    {
        std::span<std::byte> data{};
    };

    /**
     * @brief I2C Write command with Restart.
     * Generates a start condition but no stop condition. This can be changed with another command to handle multiple
     * commands.
     */
    struct I2cWriteRestartCommand
    {
        std::span<const std::byte> data{};
    };

    /**
     * @brief I2C Read command with Restart.
     * Generates a start condition but no stop condition. This can be changed with another command to handle multiple
     * commands.
     */
    struct I2cReadRestartCommand
    {
        std::span<std::byte> data{};
    };

    /**
     * @brief I2C Stop command.
     *
     */
    struct I2cStopCommand
    {
    };

    using I2cCommand =
        std::variant<I2cWriteCommand, I2cReadCommand, I2cWriteRestartCommand, I2cReadRestartCommand, I2cStopCommand>;

    template <size_t tkSize>
    using I2cCommandBuffer = std::array<I2cCommand, tkSize>;

    /**
     * @brief Compile-time I2C Transaction Builder.
     * @tparam tkCount Current number of commands in the sequence.
     * @tparam tkBusOwned Tracks if the last command was a Restart type (SCL stretched).
     */
    template <size_t tkCount = 0, bool tkBusOwned = false>
    class I2cTransactionGenerator
    {
    private:
        template <size_t tkC, bool tkBO>
        friend class I2cTransactionGenerator;

        I2cCommandBuffer<tkCount> m_commands{};

        // Internal constructor to carry the array through the chain
        constexpr explicit I2cTransactionGenerator(I2cCommandBuffer<tkCount>&& cmds) : m_commands(std::move(cmds))
        {
        }

        // Helper to append a command and return a new generator of size N+1
        template <typename TCommand, bool tkNextOwned>
        constexpr auto append(TCommand cmd) &&
        {
            I2cCommandBuffer<tkCount + 1> next_cmds{};
            for (size_t i = 0; i < tkCount; ++i)
            {
                next_cmds[i] = std::move(m_commands[i]);
            }
            next_cmds[tkCount] = I2cCommand(cmd);
            return I2cTransactionGenerator<tkCount + 1, tkNextOwned>(std::move(next_cmds));
        }

    public:
        constexpr I2cTransactionGenerator() = default;

        // --------------------------------------------------------------------
        // CHAINING METHODS
        // --------------------------------------------------------------------

        constexpr auto write(std::span<const std::byte> data) &&
        {
            return std::move(*this).template append<I2cWriteCommand, false>({data});
        }

        constexpr auto read(std::span<std::byte> data) &&
        {
            return std::move(*this).template append<I2cReadCommand, false>({data});
        }

        constexpr auto write_restart(std::span<const std::byte> data) &&
        {
            return std::move(*this).template append<I2cWriteRestartCommand, true>({data});
        }

        constexpr auto read_restart(std::span<std::byte> data) &&
        {
            return std::move(*this).template append<I2cReadRestartCommand, true>({data});
        }

        constexpr auto stop() &&
        {
            return std::move(*this).template append<I2cStopCommand, false>({});
        }

        // --------------------------------------------------------------------
        // FINALIZER
        // --------------------------------------------------------------------

        /**
         * @brief Converts the builder into a fixed-size I2cTransaction.
         * @note Validates that the bus is not left in a stretched state.
         */
        constexpr auto build() &&
        {
            static_assert(tkCount > 0, "Cannot build an empty I2C transaction!");

            static_assert(!tkBusOwned,
                          "CRITICAL: I2C Transaction ends in a Restart mode! "
                          "The bus would be left stretched LOW. You must end with a Write(), Read() or Stop().");

            return m_commands;
        }

        // Allow starting a new chain
        static constexpr auto begin()
        {
            return I2cTransactionGenerator<0, false>();
        }
    };

    // --------------------------------------------------------------------------------
    // TRANSACTION AND STATE MANAGEMENT
    // --------------------------------------------------------------------------------

    enum class I2cTransactionStatus : uint8_t
    {
        kPending,
        kBusy,
        kDone,
        kError
    };

    enum class I2cTransactionError : uint8_t
    {
        kNACK,
        kArbitrationLoss,
        kBusError,
        kOverrunUnderrun,
        kSMBusTimeout,
        kSMBusPecError,
        kSMBusAlert,
        kAborted,
        kUnknown
    };

    enum class I2cBlockingTransactionError : uint8_t
    {
        kNone,
        kQueueFull,
        kNACK,
        kArbitrationLoss,
        kBusError,
        kOverrunUnderrun,
        kSMBusTimeout,
        kSMBusPecError,
        kSMBusAlert,
        kTimeout,
        kUnknown
    };

    enum class I2cCallbackType : uint8_t
    {
        kComplete,
        kAborted,
        kError
    };

    using I2cTransactionCompleteCallback = delegate::Delegate<void, I2cCallbackType>;

    struct I2cTransactionHandle
    {
        I2cSlaveAddressVariant             address{};                             // Associated Slave Address
        std::span<const I2cCommand>        commands{};                            // Commands to execute
        I2cTransactionCompleteCallback     callback{};                            // Optional callback on completion
        uint8_t                            current{};                             // Progress tracker for ISR
        I2cTransactionStatus               status = I2cTransactionStatus::kDone;  // Pending, Busy, Done, Error
        std::optional<I2cTransactionError> last_error{};                          // Last error encountered
    };

    // --------------------------------------------------------------------------------
    // INTERNAL STATE MANAGER
    // --------------------------------------------------------------------------------

    template <typename TController, uint8_t tkTransactionQueueSize>
    class I2cCommandBufferDeviceInternalStateManager
    {
    public:
        using ControllerT                               = TController;
        constexpr static I2cPeripheralId skPeripheralId = ControllerT::skPeripheralId;

    private:
        [[no_unique_address]] DeviceRef<ControllerT>                 m_controller;
        CircularQueue<I2cTransactionHandle*, tkTransactionQueueSize> m_transaction_queue;
        I2cTransactionHandle*                                        mp_active_transaction;
        I2cCommandBufferDeviceConfig                                 m_config;

    public:
        I2cCommandBufferDeviceInternalStateManager() = delete;
        I2cCommandBufferDeviceInternalStateManager(DeviceRef<TController>&& controller)
            : m_controller(std::move(controller)), m_transaction_queue(), mp_active_transaction(nullptr)
        {
        }

        [[nodiscard]] inline bool init(const I2cCommandBufferDeviceConfig& config)
        {
            m_config = config;
            if (!m_controller->init(config.controller_config))
            {
                return false;
            }

            init_interrupts();
            return true;
        }

        inline void reset(bool clear_queue = true)
        {
            m_controller->reset();
            mp_active_transaction = nullptr;
            init_interrupts();

            if (clear_queue)
            {
                m_transaction_queue.clear();
            }
        }

        /**
         * @brief Submits a new I2C Transaction to the queue.
         * If no transaction is active, starts it immediately.
         *
         * @param address The slave address to communicate with.
         * @param commands The sequence of commands to execute.
         * @param transaction Output handle for tracking the transaction status.
         * @return true if the transaction was accepted, false if the queue is full.
         */
        bool submit_transaction(const I2cSlaveAddressVariant&      address,
                                const std::span<const I2cCommand>& commands,
                                I2cTransactionCompleteCallback&&   callback,
                                I2cTransactionHandle&              transaction)
        {
            if (m_transaction_queue.full())
            {
                return false;
            }

            transaction = I2cTransactionHandle{
                .address    = address,
                .commands   = commands,
                .callback   = std::move(callback),
                .current    = 0,
                .status     = I2cTransactionStatus::kPending,
                .last_error = std::nullopt,
            };

            // If nothing is running, start immediately
            if (mp_active_transaction == nullptr)
            {
                mp_active_transaction = &transaction;
                execute_current_command();
                return true;
            }

            return m_transaction_queue.push(&transaction);
        }

        void cancel_transaction(I2cTransactionHandle& transaction)
        {
            // If the transaction is active, mark it as cancelled
            if (mp_active_transaction == &transaction)
            {
                // Reset controller but keep the queue
                reset(false);

                // Invoke the callback to notify of abortion
                transaction.callback(I2cCallbackType::kAborted);

                // Clear it so we don't accidentally touch it in the ISR
                mp_active_transaction = nullptr;

                // Start the next transaction
                start_next_transaction();
            }

            // Correctly mark the targeted transaction as invalid so the ISR ignores it when popped
            transaction.status     = I2cTransactionStatus::kError;
            transaction.last_error = I2cTransactionError::kAborted;
        }

        /**
         * @brief Handles the completion of the current command.
         * Advances to the next command or transaction as needed.
         */
        void handle_completion()
        {
            if (mp_active_transaction != nullptr)
            {
                start_next_command();
            }
            else
            {
                start_next_transaction();
            }
        }

        /**
         * @brief Handles Errors (NACK, Bus Error, etc.)
         * @param error The error encountered.
         */
        void handle_error(I2cTransactionError error)
        {
            if (mp_active_transaction)
            {
                mp_active_transaction->status     = I2cTransactionStatus::kError;
                mp_active_transaction->last_error = error;
                mp_active_transaction->callback(I2cCallbackType::kError);
                mp_active_transaction = nullptr;
            }

            // Nuclear reset of the peripheral to release the bus
            m_controller->reset();

            // Abort this transaction and move to the next one
            start_next_transaction();
        }

    private:
        void init_interrupts()
        {
            m_controller->enable_interrupts(I2cInterruptConfig{.event_priority = m_config.event_int_priority,
                                                               .error_priority = m_config.error_int_priority,
                                                               .interrupts     = I2cInterruptMask{
                                                                       .event_tx         = false,
                                                                       .event_rx         = false,
                                                                       .event_addr_match = false,
                                                                       .event_nack       = true,
                                                                       .event_stop       = true,
                                                                       .event_tc         = true,
                                                                       .error            = true,
                                                               }});
        }

        /**
         * @brief Starts the next transaction in the queue, if any.
         */
        void start_next_transaction()
        {
            // TODO: sanitize bus

            mp_active_transaction = nullptr;

            // Pop the next transaction from the queue
            while (auto next_transaction = m_transaction_queue.pop())
            {
                // Transaction was marked as error, notify and skip
                I2cTransactionHandle* const p_next = next_transaction.value();
                assert(p_next != nullptr && "Popped null transaction from queue!");

                if (p_next->status == I2cTransactionStatus::kError)
                {
                    // Notify abortion
                    p_next->callback((p_next->last_error.value() == I2cTransactionError::kAborted)
                                         ? I2cCallbackType::kAborted
                                         : I2cCallbackType::kError);
                    continue;  // Try next transaction
                }

                // Valid transaction, start it
                mp_active_transaction = p_next;
                execute_current_command();
            }
        }

        /**
         * @brief Advances to the next command in the active transaction.
         * If all commands are done, marks the transaction as complete and starts the next one.
         */
        void start_next_command()
        {
            if (mp_active_transaction == nullptr)
            {
                return;
            }

            mp_active_transaction->current++;
            if (mp_active_transaction->current < mp_active_transaction->commands.size())
            {
                execute_current_command();
            }
            else
            {
                mp_active_transaction->status = I2cTransactionStatus::kDone;
                mp_active_transaction->callback(I2cCallbackType::kComplete);
                mp_active_transaction = nullptr;

                start_next_transaction();
            }
        }

        /**
         * @brief Executes the current command in the active transaction.
         */
        void execute_current_command()
        {
            if (!mp_active_transaction)
            {
                return;
            }

            // Setup metadata for this specific step
            const auto& cmd = mp_active_transaction->commands[mp_active_transaction->current];

            // If the previous command was a Restart type, the bus is stretched/owned
            bool bus_currently_owned = false;
            if (mp_active_transaction->current > 0)
            {
                const auto& previous_command = mp_active_transaction->commands[mp_active_transaction->current - 1];
                bus_currently_owned          = std::holds_alternative<I2cWriteRestartCommand>(previous_command) ||
                                      std::holds_alternative<I2cReadRestartCommand>(previous_command);
            }

            mp_active_transaction->status = I2cTransactionStatus::kBusy;

            std::visit(Overloaded{[this, bus_currently_owned](const I2cWriteCommand& command)
                                  { dispatch_to_controller<false, true>(command.data, bus_currently_owned); },
                                  [this, bus_currently_owned](const I2cReadCommand& command)
                                  { dispatch_to_controller<true, true>(command.data, bus_currently_owned); },
                                  [this, bus_currently_owned](const I2cWriteRestartCommand& command)
                                  { dispatch_to_controller<false, false>(command.data, bus_currently_owned); },
                                  [this, bus_currently_owned](const I2cReadRestartCommand& command)
                                  { dispatch_to_controller<true, false>(command.data, bus_currently_owned); },
                                  [this](const I2cStopCommand& /*command*/) { m_controller->stop_transfer(); }},
                       cmd);
        }

        /**
         * @brief Dispatches the command to the controller with appropriate parameters.
         *
         * @tparam tkRead Whether the command is a read operation.
         * @tparam tkAutoEnd Whether to use AutoEnd mode (true) or SoftEnd mode (false).
         * @param data The data buffer for the operation.
         * @param bus_owned Whether the bus is currently owned (stretched).
         */
        template <bool tkRead, bool tkAutoEnd>
        void dispatch_to_controller(const std::span<std::conditional_t<tkRead, std::byte, const std::byte>>& data,
                                    const bool                                                               bus_owned)
        {
            // Determine Request Type
            const I2cTransferRequest request = bus_owned ? I2cTransferRequest::kRestart : I2cTransferRequest::kStart;

            // Determine End Mode
            constexpr I2cTransferEndMode end_mode =
                tkAutoEnd ? I2cTransferEndMode::kAutoEnd : I2cTransferEndMode::kSoftEnd;

            I2cTransferConfig cfg{
                .slave_address = mp_active_transaction->address,
                .end_mode      = end_mode,
                .request       = request,
            };

            if constexpr (tkRead)
            {
                m_controller->initiate_dma_read(cfg, data);
            }
            else
            {
                m_controller->initiate_dma_write(cfg, data);
            }
        }
    };

    // --------------------------------------------------------------------------------
    // DRIVER CLASS
    // --------------------------------------------------------------------------------

    template <I2cPeripheralId tkPeripheralId, uint8_t tkTransactionQueueSize = 10>
    class I2cCommandBufferDevice
    {
    public:
        struct Descriptor : public SharedDeviceDescriptor
        {
        };

        constexpr static I2cPeripheralId skPeripheralId = tkPeripheralId;
        using ControllerT                               = I2cControllerDevice<skPeripheralId>;
        constexpr static uint8_t skTransactionQueueSize = tkTransactionQueueSize;

        using InjectDevices = TypeList<ControllerT>;

    private:
        Synchronized<I2cCommandBufferDeviceInternalStateManager<ControllerT, tkTransactionQueueSize>, CriticalSection>
            m_shared_state;

    public:
        /**
         * @brief Constructs the I2C Command Buffer Device with the given controller device.
         *
         * @param controller The I2C controller device reference.
         */
        I2cCommandBufferDevice(DeviceRef<ControllerT>&& controller) : m_shared_state(std::move(controller))
        {
        }

        /**
         * @brief Initializes the I2C Command Buffer Device.
         *
         */
        [[nodiscard]] inline bool init(const I2cCommandBufferDeviceConfig& config)
        {
            return m_shared_state.with_lock([&](auto& state) { return state.init(config); });
        }

        /**
         * @brief Submits a new I2C Transaction to the queue.
         * If no transaction is active, starts it immediately.
         *
         * @param address The slave address to communicate with.
         * @param commands The sequence of commands to execute.
         * @param callback Optional callback to be invoked upon transaction completion.
         * @param transaction Output handle for tracking the transaction status.
         * @return true if the transaction was accepted, false if the queue is full.
         */
        inline bool submit_transaction(const I2cSlaveAddressVariant&      address,
                                       const std::span<const I2cCommand>& commands,
                                       I2cTransactionCompleteCallback&&   callback,
                                       I2cTransactionHandle&              transaction)
        {
            return m_shared_state.with_lock(
                [&](auto& state)
                { return state.submit_transaction(address, commands, std::move(callback), transaction); });
        }

        template <typename TTimeoutDuration>
        I2cBlockingTransactionError submit_transaction_blocking(const I2cSlaveAddressVariant&      address,
                                                                const std::span<const I2cCommand>& commands,
                                                                I2cTransactionHandle&              transaction,
                                                                const TTimeoutDuration             timeout)
        {
            struct BlockingCallbackHandler
            {
                std::atomic<bool> done = false;

                void on_complete()
                {
                    done.store(true, std::memory_order_release);
                }

                bool wait(TTimeoutDuration timeout)
                {
                    TimingContext::TimerCycleT timer;

                    while (!done.load(std::memory_order_acquire))
                    {
                        if (timer.has_passed(timeout))
                        {
                            return false;
                        }

                        __WFI();  // Wait For Interrupt
                    }
                    return true;
                }
            };

            BlockingCallbackHandler callback_handler;

            if (!submit_transaction(
                    address,
                    commands,
                    I2cTransactionCompleteCallback([&](I2cCallbackType) { callback_handler.on_complete(); }),
                    transaction))
            {
                return I2cBlockingTransactionError::kQueueFull;
            }

            // Wait for completion or timeout
            const bool timeout_occurred = callback_handler.wait(timeout) == false;

            // Timeout?
            if (timeout_occurred)
            {
                cancel_transaction(transaction);
                return I2cBlockingTransactionError::kTimeout;
            }

            // Check success
            if (transaction.status == I2cTransactionStatus::kDone)
            {
                return I2cBlockingTransactionError::kNone;
            }

            // Map errors
            if (!transaction.last_error.has_value())
            {
                return I2cBlockingTransactionError::kUnknown;
            }

            switch (transaction.last_error.value())
            {
                case I2cTransactionError::kNACK:
                    return I2cBlockingTransactionError::kNACK;
                case I2cTransactionError::kArbitrationLoss:
                    return I2cBlockingTransactionError::kArbitrationLoss;
                case I2cTransactionError::kBusError:
                    return I2cBlockingTransactionError::kBusError;
                case I2cTransactionError::kOverrunUnderrun:
                    return I2cBlockingTransactionError::kOverrunUnderrun;
                case I2cTransactionError::kSMBusTimeout:
                    return I2cBlockingTransactionError::kSMBusTimeout;
                case I2cTransactionError::kSMBusPecError:
                    return I2cBlockingTransactionError::kSMBusPecError;
                case I2cTransactionError::kSMBusAlert:
                    return I2cBlockingTransactionError::kSMBusAlert;
                default:
                    return I2cBlockingTransactionError::kUnknown;
            }
        }

        /**
         * @brief Cancels an ongoing or queued transaction.
         *
         * @param transaction The transaction handle to cancel.
         */
        inline void cancel_transaction(I2cTransactionHandle& transaction)
        {
            m_shared_state.with_lock([&](auto& state) { state.cancel_transaction(transaction); });
        }

        /**
         * @brief Query the status of a transaction.
         *
         * @param transaction The transaction handle to query.
         * @return I2cTransactionStatus The current status of the transaction.
         */
        inline I2cTransactionStatus query_status(const I2cTransactionHandle& transaction) const
        {
            return transaction.status;
        }

        /**
         * @brief Query the last error of a transaction.
         *
         * @param transaction The transaction handle to query.
         * @return std::optional<I2cTransactionError> The last error encountered, or std::nullopt if none.
         */
        inline std::optional<I2cTransactionError> query_last_error(const I2cTransactionHandle& transaction) const
        {
            return transaction.last_error;
        }

        // =============================================================================
        // ISR METHODS (Called from I2cIsrRouter)
        // =============================================================================

        /**
         * @brief Handles the TC (Transfer Complete) event.
         * Occurs when a SOFTEND command finishes. We trigger the next command in the span.
         */
        inline void handle_tc_event_isr()
        {
            m_shared_state.with_lock([](auto& state) { state.handle_completion(); });
        }

        /**
         * @brief Handles the STOPF (Stop Detection) event.
         */
        inline void handle_stop_event_isr()
        {
            m_shared_state.with_lock([](auto& state) { state.handle_completion(); });
        }

        /**
         * @brief Handles the NACK (Not Acknowledge) event.
         */
        inline void handle_nack_event_isr()
        {
            m_shared_state.with_lock([](auto& state) { state.handle_error(I2cTransactionError::kNACK); });
        }

        /**
         * @brief Called from all error ISRs (ARLO, BERR, OVR, TimEOUT, PECERR, ALERT, etc.)
         *
         */
        inline void handle_error_isr(const I2cErrorInterruptType& error)
        {
            I2cTransactionError mapped_error;
            switch (error)
            {
                case I2cErrorInterruptType::kArbitrationLoss:
                    mapped_error = I2cTransactionError::kArbitrationLoss;
                    break;
                case I2cErrorInterruptType::kBusError:
                    mapped_error = I2cTransactionError::kBusError;
                    break;
                case I2cErrorInterruptType::kOverrunUnderrun:
                    mapped_error = I2cTransactionError::kOverrunUnderrun;
                    break;
                case I2cErrorInterruptType::kSMBusTimeout:
                    mapped_error = I2cTransactionError::kSMBusTimeout;
                    break;
                case I2cErrorInterruptType::kSMBusPecError:
                    mapped_error = I2cTransactionError::kSMBusPecError;
                    break;
                case I2cErrorInterruptType::kSMBusAlert:
                    mapped_error = I2cTransactionError::kSMBusAlert;
                    break;
                default:
                    mapped_error = I2cTransactionError::kUnknown;
                    break;
            }

            m_shared_state.with_lock([mapped_error](auto& state) { state.handle_error(mapped_error); });
        }
    };

    template <I2cPeripheralId tkPeripheralId, uint16_t tkAddress, bool tkIs10BitAddress = false>
    class I2cCommandBufferSlaveDevice
    {
    public:
        struct Descriptor : public UniqueDeviceDescriptor
        {
        };

        constexpr static uint8_t skPeripheralId = tkPeripheralId;
        using ArbitratorT                       = I2cCommandBufferDevice<tkPeripheralId>;
        constexpr static auto skAddress         = I2cSlaveAddress<tkIs10BitAddress>(tkAddress);

        using InjectDevices = TypeList<ArbitratorT>;

    private:
        [[no_unique_address]] DeviceRef<ArbitratorT> m_controller;

        I2cTransactionHandle m_transaction;

    public:
        /**
         * @brief Constructs the I2C Command Buffer Slave Device with the given controller device.
         *
         * @param controller The I2C command buffer controller device reference.
         */
        I2cCommandBufferSlaveDevice(DeviceRef<ArbitratorT>&& controller) : m_controller(std::move(controller))
        {
        }

        /**
         * @brief Initializes the I2C Command Buffer Slave Device.
         *
         */
        [[nodiscard]] inline bool init()
        {
            return true;  // Nothing to do, the controller handles everything
        }

        /**
         * @brief Submits a new I2C Transaction to the queue.
         * If no transaction is active, starts it immediately.
         *
         * @param commands The sequence of commands to execute.
         * @param callback Optional callback to be invoked upon transaction completion.
         * @return true if the transaction was accepted, false if the queue is full.
         */
        [[nodiscard]] bool submit_transaction(const std::span<const I2cCommand>& commands,
                                              I2cTransactionCompleteCallback&&   callback)
        {
            return m_controller->submit_transaction(skAddress, commands, std::move(callback), m_transaction);
        }

        /**
         * @brief Submits a new I2C Transaction to the queue and blocks until completion or timeout.
         *
         * @tparam TTimeoutDuration The type of the timeout duration.
         * @param commands The sequence of commands to execute.
         * @param timeout The maximum duration to wait for completion.
         * @return I2cBlockingTransactionError The result of the blocking transaction.
         */
        template <typename TTimeoutDuration>
        [[nodiscard]] I2cBlockingTransactionError submit_transaction_blocking(
            const std::span<const I2cCommand>& commands, const TTimeoutDuration timeout)
        {
            return m_controller->template submit_transaction_blocking(skAddress, commands, m_transaction, timeout);
        }

        /**
         * @brief Cancels an ongoing or queued transaction.
         *
         */
        void cancel_transaction()
        {
            m_controller->cancel_transaction(m_transaction);
        }

        /**
         * @brief Query the status of the last transaction.
         *
         * @return I2cTransactionStatus The current status of the transaction.
         */
        [[nodiscard]] I2cTransactionStatus query_status() const
        {
            return m_controller->query_status(m_transaction);
        }

        /**
         * @brief Query the last error of the last transaction.
         *
         * @return std::optional<I2cTransactionError> The last error encountered, or std::nullopt if none.
         */
        [[nodiscard]] std::optional<I2cTransactionError> query_last_error() const
        {
            return m_controller->query_last_error(m_transaction);
        }

        /**
         * @brief Checks if a transaction is currently in progress (either pending or busy).
         * @return true if a transaction is pending or busy, false if done or error.
         */
        [[nodiscard]] bool transaction_in_progress() const
        {
            const auto status = query_status();
            return status == I2cTransactionStatus::kPending || status == I2cTransactionStatus::kBusy;
        }
    };

    // --------------------------------------------------------------------------------
    // DEVICE ALIASES
    // --------------------------------------------------------------------------------

    template <uint8_t tkTransactionQueueSize = 10>
    using I2c1CommandBufferDevice = I2cCommandBufferDevice<I2cPeripheralId::kI2c1, tkTransactionQueueSize>;

    template <uint8_t tkTransactionQueueSize = 10>
    using I2c2CommandBufferDevice = I2cCommandBufferDevice<I2cPeripheralId::kI2c2, tkTransactionQueueSize>;

    template <uint8_t tkTransactionQueueSize = 10>
    using I2c3CommandBufferDevice = I2cCommandBufferDevice<I2cPeripheralId::kI2c3, tkTransactionQueueSize>;

    template <uint8_t tkTransactionQueueSize = 10>
    using I2c4CommandBufferDevice = I2cCommandBufferDevice<I2cPeripheralId::kI2c4, tkTransactionQueueSize>;

}  // namespace valle::platform

#define VALLE_BIND_I2C_COMMAND_BUFFER_DRIVER_ISR(instance)                                                       \
    namespace valle::platform                                                                                    \
    {                                                                                                            \
        template <>                                                                                              \
        struct I2cIsrRouter<std::remove_cvref_t<decltype((instance))>::skPeripheralId,                           \
                            I2cInterruptType::kTransferComplete>                                                 \
        {                                                                                                        \
            static void handle()                                                                                 \
            {                                                                                                    \
                (instance).handle_tc_event_isr();                                                                \
            }                                                                                                    \
        };                                                                                                       \
                                                                                                                 \
        template <>                                                                                              \
        struct I2cIsrRouter<std::remove_cvref_t<decltype((instance))>::skPeripheralId,                           \
                            I2cInterruptType::kStopDetection>                                                    \
        {                                                                                                        \
            static void handle()                                                                                 \
            {                                                                                                    \
                (instance).handle_stop_event_isr();                                                              \
            }                                                                                                    \
        };                                                                                                       \
                                                                                                                 \
        template <>                                                                                              \
        struct I2cIsrRouter<std::remove_cvref_t<decltype((instance))>::skPeripheralId,                           \
                            I2cInterruptType::kNACKReceived>                                                     \
        {                                                                                                        \
            static void handle()                                                                                 \
            {                                                                                                    \
                (instance).handle_nack_event_isr();                                                              \
            }                                                                                                    \
        };                                                                                                       \
                                                                                                                 \
        template <>                                                                                              \
        struct I2cIsrRouter<std::remove_cvref_t<decltype((instance))>::skPeripheralId, I2cInterruptType::kError> \
        {                                                                                                        \
            static void handle(I2cErrorInterruptType error)                                                      \
            {                                                                                                    \
                (instance).handle_error_isr(error);                                                              \
            }                                                                                                    \
        };                                                                                                       \
    }
