#pragma once

#include <delegate/delegate.h>

#include <cstdint>
#include <expected>
#include <optional>
#include <span>
#include <variant>

#include "valle/platform/core.hpp"
#include "valle/platform/devices/dma.hpp"
#include "valle/platform/hardware/i2c.hpp"
#include "valle/utils/circular_queue.hpp"


namespace valle::platform
{
    // ============================================================================
    // FOWARD DECLARATIONS
    // ============================================================================

    class I2CRootDevice;

    template <I2CControllerID tkControllerID>
        requires(kValidI2CControllerID<tkControllerID>)
    class I2CControllerDevice;

    // ============================================================================
    // I2C DEVICE (THE INTERFACE DEVICE)
    // ============================================================================

    class I2CRootDevice
    {
    public:
        struct Descriptor : public InterfaceDeviceDescriptor
        {
            using Children = DeviceTreeList<I2CControllerDevice<1>,
                                            I2CControllerDevice<2>,
                                            I2CControllerDevice<3>,
                                            I2CControllerDevice<4>>;
        };
    };

    // ============================================================================
    // I2C CONTROLLER (THE UNIQUE DEVICE)
    // ============================================================================

    // ---------------------------------------------------------------------------
    // COMPILE TIME CONFIGURATIONS
    // ---------------------------------------------------------------------------
    struct I2CControllerCTDefaultConfig
    {
        using DMAChannelRxT = DMANullChannelDevice;
        using DMAChannelTxT = DMANullChannelDevice;
    };

    template <typename T>
    concept CValidI2CControllerCTConfig =
        requires {
            typename T::DMAChannelRxT;
            typename T::DMAChannelTxT;
        } && ((CNullDMAChannel<typename T::DMAChannelRxT> && CNullDMAChannel<typename T::DMAChannelTxT>) ||
              (CDMAChannelDevice<typename T::DMAChannelRxT> && CDMAChannelDevice<typename T::DMAChannelTxT>));

    template <I2CControllerID tkControllerID>
        requires(kValidI2CControllerID<tkControllerID>)
    struct I2CControllerCTConfigTraits
    {
        static constexpr auto skConfig = I2CControllerCTDefaultConfig{};
    };

#define VALLE_DEFINE_I2C_CONTROLLER_CT_CONFIG(tkControllerID, config)                                           \
    namespace valle::platform                                                                                   \
    {                                                                                                           \
        template <>                                                                                             \
        struct I2CControllerCTConfigTraits<(tkControllerID)>                                                    \
        {                                                                                                       \
            static constexpr auto skConfig = (config);                                                          \
            static_assert(CValidI2CControllerCTConfig<decltype(skConfig)>, "Invalid I2C Controller CT Config"); \
        };                                                                                                      \
    }

    // ----------------------------------------------------------------------------
    // CONFIGURATIONS
    // ----------------------------------------------------------------------------

    /**
     * @brief Configuration for the Bus (Speed)
     */
    struct I2CControllerConfig
    {
        uint32_t    timing_reg           = 0;  // Pre-calculated timing value
        bool        enable_analog_filter = true;
        DMAPriority dma_priority         = DMAPriority::kHigh;
    };

    /**
     * @brief Configuration for a single I2C Transfer
     */
    struct I2CTransferConfig
    {
        I2CSlaveAddressVariant slave_address{};
        I2CTransferEndMode     end_mode = I2CTransferEndMode::kAutoEnd;
        I2CTransferRequest     request  = I2CTransferRequest::kStart;
    };

    // ---------------------------------------------------------------------------
    // DEVICE CLASS
    // ---------------------------------------------------------------------------

    /**
     * @brief Logic for the I2C Controller
     * @note This device is unique because despite a bus being shared, its state can only be managed
     * by one slave at a time. Users should write an arbitration layer if multiple slaves are to be used.
     */
    template <I2CControllerID tkControllerID>
        requires(kValidI2CControllerID<tkControllerID>)
    class I2CControllerDevice
    {
    public:
        struct Descriptor : public UniqueDeviceDescriptor
        {
        };

        static constexpr I2CControllerID skControllerID = tkControllerID;
        using ControllerTraitsT                         = I2CControllerTraits<skControllerID>;
        using InterruptControllerT                      = I2CInterruptController<skControllerID>;

        using CTConfigTraitT             = I2CControllerCTConfigTraits<skControllerID>;
        static constexpr auto skCTConfig = CTConfigTraitT::skConfig;
        using CTConfigT                  = decltype(skCTConfig);
        using DMAChannelRxT              = typename CTConfigT::DMAChannelRxT;
        using DMAChannelTxT              = typename CTConfigT::DMAChannelTxT;
        static constexpr bool skHasDMA   = !CNullDMAChannel<DMAChannelRxT> && !CNullDMAChannel<DMAChannelTxT>;

        using InjectDevices = std::conditional_t<skHasDMA, TypeList<DMAChannelRxT, DMAChannelTxT>, TypeList<>>;

    private:
        static inline uint32_t kReadRegisterAddr  = reinterpret_cast<uint32_t>(&ControllerTraitsT::skInstance->RXDR);
        static inline uint32_t kWriteRegisterAddr = reinterpret_cast<uint32_t>(&ControllerTraitsT::skInstance->TXDR);

        [[no_unique_address]] ConditionalDeviceRef<skHasDMA, DMAChannelRxT> m_dma_rx;
        [[no_unique_address]] ConditionalDeviceRef<skHasDMA, DMAChannelTxT> m_dma_tx;

    public:
        I2CControllerDevice(DeviceRef<DMAChannelRxT>&& dma_rx, DeviceRef<DMAChannelTxT>&& dma_tx)
            requires(skHasDMA)
            : m_dma_rx(std::move(dma_rx)), m_dma_tx(std::move(dma_tx))
        {
        }

        I2CControllerDevice()
            requires(!skHasDMA)
        {
        }

        // ------------------------------------------------------------------------
        // Initialization
        // ------------------------------------------------------------------------
        [[nodiscard]] bool init(const I2CControllerConfig& config)
        {
            LL_APB1_GRP1_EnableClock(ControllerTraitsT::skClock);
            LL_I2C_Disable(ControllerTraitsT::skInstance);
            LL_I2C_SetTiming(ControllerTraitsT::skInstance, config.timing_reg);
            if (config.enable_analog_filter)
            {
                LL_I2C_EnableAnalogFilter(ControllerTraitsT::skInstance);
            }

            if constexpr (skHasDMA)
            {
                // Configure DMA Requests
                LL_I2C_EnableDMAReq_RX(ControllerTraitsT::skInstance);
                LL_I2C_EnableDMAReq_TX(ControllerTraitsT::skInstance);

                dma_init<true>(config.dma_priority);   // RX
                dma_init<false>(config.dma_priority);  // TX
            }

            LL_I2C_Enable(ControllerTraitsT::skInstance);
            return true;
        }

        [[nodiscard]] bool post_init()
        {
            // Nothing to do for now
            return true;
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
        static void enable_interrupts(const I2CInterruptConfig& config)
        {
            InterruptControllerT::enable_interrupts(config);
        }

        /**
         * @brief Disable interrupts for this channel.
         */
        static void disable_interrupts()
        {
            InterruptControllerT::disable_interrupts();
        }

        // ------------------------------------------------------------------------
        // Main API
        // ------------------------------------------------------------------------
        template <bool tkRead>
        static inline void initiate_transfer(const I2CTransferConfig& transfer_config, const uint32_t buffer_size)
        {
            const uint16_t raw_address = std::visit(
                [](const auto& addr) { return static_cast<uint16_t>(addr.address); }, transfer_config.slave_address);

            const bool is_10bit_address = std::holds_alternative<I2C10BitSlaveAddress>(transfer_config.slave_address);
            const uint32_t address_size = is_10bit_address ? LL_I2C_ADDRESSING_MODE_10BIT : LL_I2C_ADDRESSING_MODE_7BIT;

            uint32_t request = 0;
            switch (transfer_config.request)
            {
                case I2CTransferRequest::kNoStart:
                    request = LL_I2C_GENERATE_NOSTARTSTOP;
                    break;
                case I2CTransferRequest::kStart:
                    request = tkRead ? LL_I2C_GENERATE_START_READ : LL_I2C_GENERATE_START_WRITE;
                    break;
                case I2CTransferRequest::kRestart:
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

        static inline void stop_transfer()
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
        inline bool initiate_dma_read(const I2CTransferConfig& transfer_config, const std::span<std::byte>& buffer)
            requires(skHasDMA)
        {
            return initiate_dma_transfer<true>(transfer_config, buffer);
        }

        inline bool initiate_dma_write(const I2CTransferConfig&          transfer_config,
                                       const std::span<const std::byte>& buffer)
            requires(skHasDMA)
        {
            return initiate_dma_transfer<false>(transfer_config, buffer);
        }

        template <bool tkRead>
        bool initiate_dma_transfer(const I2CTransferConfig& transfer_config,
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
        inline void dma_init(const DMAPriority& priority)
            requires(skHasDMA)
        {
            constexpr DMADirection    direction = tkRead ? DMADirection::kPeriphToMem : DMADirection::kMemToPeriph;
            constexpr DMAMuxRequestID request_id =
                tkRead ? ControllerTraitsT::skDMAMuxRequestRx : ControllerTraitsT::skDMAMuxRequestTx;

            DMAChannelConfig dma_cfg = DMAChannelConfig{
                .direction         = direction,
                .priority          = priority,
                .mode              = DMAMode::kNormal,
                .periph_data_width = DMADataWidth::kByte,
                .memory_data_width = DMADataWidth::kByte,
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
                m_dma_rx->start(
                    kReadRegisterAddr, reinterpret_cast<uint32_t>(data.data()), static_cast<uint32_t>(data.size()));
            }
            else
            {
                m_dma_tx->start(
                    reinterpret_cast<uint32_t>(data.data()), kWriteRegisterAddr, static_cast<uint32_t>(data.size()));
            }
        }
    };

    // -----------------------------------------------------------------------------
    // DEVICE ALIASES
    // -----------------------------------------------------------------------------
    using I2C1ControllerDevice = I2CControllerDevice<1>;
    using I2C2ControllerDevice = I2CControllerDevice<2>;
    using I2C3ControllerDevice = I2CControllerDevice<3>;
    using I2C4ControllerDevice = I2CControllerDevice<4>;

    // ==========================================================================
    // I2C COMMAND BUFFER DEVICE (SHARED DEVICE)
    // ==========================================================================

    // --------------------------------------------------------------------------------
    // CONFIGURATIONS
    // --------------------------------------------------------------------------------

    /**
     * @brief Configuration for I2C Command Buffer Device
     */
    struct I2CCommandBufferDeviceConfig
    {
        I2CControllerConfig controller_config{};
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
    struct I2CWriteCommand
    {
        std::span<const std::byte> data{};
    };

    /**
     * @brief I2C Read command.
     * Generates a start and stop condition with the read bit set.
     */
    struct I2CReadCommand
    {
        std::span<std::byte> data{};
    };

    /**
     * @brief I2C Write command with Restart.
     * Generates a start condition but no stop condition. This can be changed with another command to handle multiple
     * commands.
     */
    struct I2CWriteRestartCommand
    {
        std::span<const std::byte> data{};
    };

    /**
     * @brief I2C Read command with Restart.
     * Generates a start condition but no stop condition. This can be changed with another command to handle multiple
     * commands.
     */
    struct I2CReadRestartCommand
    {
        std::span<std::byte> data{};
    };

    /**
     * @brief I2C Stop command.
     *
     */
    struct I2CStopCommand
    {
    };

    using I2CCommand =
        std::variant<I2CWriteCommand, I2CReadCommand, I2CWriteRestartCommand, I2CReadRestartCommand, I2CStopCommand>;

    template <size_t tkSize>
    using I2CCommandBuffer = std::array<I2CCommand, tkSize>;

    /**
     * @brief Compile-time I2C Transaction Builder.
     * @tparam tkCount Current number of commands in the sequence.
     * @tparam tkBusOwned Tracks if the last command was a Restart type (SCL stretched).
     */
    template <size_t tkCount = 0, bool tkBusOwned = false>
    class I2CTransactionGenerator
    {
    private:
        template <size_t tkC, bool tkBO>
        friend class I2CTransactionGenerator;

        I2CCommandBuffer<tkCount> m_commands{};

        // Internal constructor to carry the array through the chain
        constexpr explicit I2CTransactionGenerator(I2CCommandBuffer<tkCount>&& cmds) : m_commands(std::move(cmds))
        {
        }

        // Helper to append a command and return a new generator of size N+1
        template <typename TCommand, bool tkNextOwned>
        constexpr auto append(TCommand cmd) &&
        {
            I2CCommandBuffer<tkCount + 1> next_cmds{};
            for (size_t i = 0; i < tkCount; ++i)
            {
                next_cmds[i] = std::move(m_commands[i]);
            }
            next_cmds[tkCount] = I2CCommand(cmd);
            return I2CTransactionGenerator<tkCount + 1, tkNextOwned>(std::move(next_cmds));
        }

    public:
        constexpr I2CTransactionGenerator() = default;

        // --------------------------------------------------------------------
        // CHAINING METHODS
        // --------------------------------------------------------------------

        constexpr auto write(std::span<const std::byte> data) &&
        {
            return std::move(*this).template append<I2CWriteCommand, false>({data});
        }

        constexpr auto read(std::span<std::byte> data) &&
        {
            return std::move(*this).template append<I2CReadCommand, false>({data});
        }

        constexpr auto write_restart(std::span<const std::byte> data) &&
        {
            return std::move(*this).template append<I2CWriteRestartCommand, true>({data});
        }

        constexpr auto read_restart(std::span<std::byte> data) &&
        {
            return std::move(*this).template append<I2CReadRestartCommand, true>({data});
        }

        constexpr auto stop() &&
        {
            return std::move(*this).template append<I2CStopCommand, false>({});
        }

        // --------------------------------------------------------------------
        // FINALIZER
        // --------------------------------------------------------------------

        /**
         * @brief Converts the builder into a fixed-size I2CTransaction.
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
            return I2CTransactionGenerator<0, false>();
        }
    };

    // --------------------------------------------------------------------------------
    // TRANSACTION AND STATE MANAGEMENT
    // --------------------------------------------------------------------------------

    enum class I2CTransactionStatus : uint8_t
    {
        kPending,
        kBusy,
        kDone,
        kError
    };

    enum class I2CTransactionError : uint8_t
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

    enum class I2CBlockingTransactionError : uint8_t
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

    enum class I2CCallbackType : uint8_t
    {
        kComplete,
        kAborted,
        kError
    };

    using I2CTransactionCompleteCallback = delegate::Delegate<void, I2CCallbackType>;

    struct I2CTransactionHandle
    {
        I2CSlaveAddressVariant             address{};     // Associated Slave Address
        std::span<const I2CCommand>        commands{};    // Commands to execute
        I2CTransactionCompleteCallback     callback{};    // Optional callback on completion
        uint8_t                            current{};     // Progress tracker for ISR
        I2CTransactionStatus               status{};      // Pending, Busy, Done, Error
        std::optional<I2CTransactionError> last_error{};  // Last error encountered
    };

    // --------------------------------------------------------------------------------
    // INTERNAL STATE MANAGER
    // --------------------------------------------------------------------------------

    template <typename TController, uint8_t tkTransactionQueueSize>
    class I2CCommandBufferDeviceInternalStateManager
    {
    public:
        using ControllerT                               = TController;
        constexpr static I2CControllerID skControllerID = ControllerT::skControllerID;

    private:
        [[no_unique_address]] DeviceRef<ControllerT>                 m_controller;
        CircularQueue<I2CTransactionHandle*, tkTransactionQueueSize> m_transaction_queue;
        I2CTransactionHandle*                                        mp_active_transaction;
        I2CCommandBufferDeviceConfig                                 m_config;

    public:
        I2CCommandBufferDeviceInternalStateManager() = delete;
        I2CCommandBufferDeviceInternalStateManager(DeviceRef<TController>&& controller)
            : m_controller(std::move(controller)), m_transaction_queue(), mp_active_transaction(nullptr)
        {
        }

        [[nodiscard]] inline bool init(const I2CCommandBufferDeviceConfig& config)
        {
            m_config = config;
            if (!m_controller->init(config.controller_config))
            {
                return false;
            }

            init_interrupts();
            return true;
        }

        [[nodiscard]] inline bool post_init()
        {
            // Nothing to do for now
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
        bool submit_transaction(const I2CSlaveAddressVariant&      address,
                                const std::span<const I2CCommand>& commands,
                                I2CTransactionCompleteCallback&&   callback,
                                I2CTransactionHandle&              transaction)
        {
            if (m_transaction_queue.full())
            {
                return false;
            }

            transaction = I2CTransactionHandle{
                .address    = address,
                .commands   = commands,
                .callback   = std::move(callback),
                .current    = 0,
                .status     = I2CTransactionStatus::kPending,
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

        void cancel_transaction(I2CTransactionHandle& transaction)
        {
            // If the transaction is active, mark it as cancelled
            if (mp_active_transaction == &transaction)
            {
                // Reset controller but keep the queue
                reset(false);

                // Invoke the callback to notify of abortion
                transaction.callback(I2CCallbackType::kAborted);

                // Start the next transaction
                start_next_transaction();
            }

            // We can't easily remove from the middle of a CircularQueue,
            // so we mark it as "Invalid/Error" so the ISR ignores it when popped.
            mp_active_transaction->status     = I2CTransactionStatus::kError;
            mp_active_transaction->last_error = I2CTransactionError::kAborted;
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
        void handle_error(I2CTransactionError error)
        {
            if (mp_active_transaction)
            {
                mp_active_transaction->status     = I2CTransactionStatus::kError;
                mp_active_transaction->last_error = error;
                mp_active_transaction->callback(I2CCallbackType::kError);
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
            m_controller->enable_interrupts(I2CInterruptConfig{.event_priority = m_config.event_int_priority,
                                                               .error_priority = m_config.error_int_priority,
                                                               .interrupts     = I2CInterruptMask{
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
                I2CTransactionHandle* const p_next = next_transaction.value();
                assert(p_next != nullptr && "Popped null transaction from queue!");

                if (p_next->status == I2CTransactionStatus::kError)
                {
                    // Notify abortion
                    p_next->callback((p_next->last_error.value() == I2CTransactionError::kAborted)
                                         ? I2CCallbackType::kAborted
                                         : I2CCallbackType::kError);
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
                mp_active_transaction->status = I2CTransactionStatus::kDone;
                mp_active_transaction->callback(I2CCallbackType::kComplete);
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
                bus_currently_owned          = std::holds_alternative<I2CWriteRestartCommand>(previous_command) ||
                                      std::holds_alternative<I2CReadRestartCommand>(previous_command);
            }

            mp_active_transaction->status = I2CTransactionStatus::kBusy;

            std::visit(Overloaded{[this, bus_currently_owned](const I2CWriteCommand& command)
                                  { dispatch_to_controller<false, true>(command.data, bus_currently_owned); },
                                  [this, bus_currently_owned](const I2CReadCommand& command)
                                  { dispatch_to_controller<true, true>(command.data, bus_currently_owned); },
                                  [this, bus_currently_owned](const I2CWriteRestartCommand& command)
                                  { dispatch_to_controller<false, false>(command.data, bus_currently_owned); },
                                  [this, bus_currently_owned](const I2CReadRestartCommand& command)
                                  { dispatch_to_controller<true, false>(command.data, bus_currently_owned); },
                                  [this](const I2CStopCommand& /*command*/) { m_controller->stop_transfer(); }},
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
            const I2CTransferRequest request = bus_owned ? I2CTransferRequest::kRestart : I2CTransferRequest::kStart;

            // Determine End Mode
            constexpr I2CTransferEndMode end_mode =
                tkAutoEnd ? I2CTransferEndMode::kAutoEnd : I2CTransferEndMode::kSoftEnd;

            I2CTransferConfig cfg{
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

    template <uint8_t tkControllerID, uint8_t tkTransactionQueueSize = 10>
        requires(kValidI2CControllerID<tkControllerID>)
    class I2CCommandBufferDevice
    {
    public:
        struct Descriptor : public SharedDeviceDescriptor
        {
        };

        constexpr static I2CControllerID skControllerID = tkControllerID;
        using ControllerT                               = I2CControllerDevice<skControllerID>;
        constexpr static uint8_t skTransactionQueueSize = tkTransactionQueueSize;

        using InjectDevices = TypeList<ControllerT>;

    private:
        Synchronized<I2CCommandBufferDeviceInternalStateManager<ControllerT, tkTransactionQueueSize>> m_shared_state;

    public:
        /**
         * @brief Constructs the I2C Command Buffer Device with the given controller device.
         *
         * @param controller The I2C controller device reference.
         */
        I2CCommandBufferDevice(DeviceRef<ControllerT>&& controller) : m_shared_state(std::move(controller))
        {
        }

        /**
         * @brief Initializes the I2C Command Buffer Device.
         *
         */
        [[nodiscard]] inline bool init(const I2CCommandBufferDeviceConfig& config)
        {
            return m_shared_state.with_lock([&](auto& state) { return state.init(config); });
        }

        /**
         * @brief Post-initialization steps for the I2C Command Buffer Device.
         *
         */
        [[nodiscard]] inline bool post_init()
        {
            // Nothing to do for now
            return true;
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
        inline bool submit_transaction(const I2CSlaveAddressVariant&      address,
                                       const std::span<const I2CCommand>& commands,
                                       I2CTransactionCompleteCallback&&   callback,
                                       I2CTransactionHandle&              transaction)
        {
            return m_shared_state.with_lock(
                [&](auto& state)
                { return state.submit_transaction(address, commands, std::move(callback), transaction); });
        }

        template <typename TTimeoutDuration>
        I2CBlockingTransactionError submit_transaction_blocking(const I2CSlaveAddressVariant&      address,
                                                                const std::span<const I2CCommand>& commands,
                                                                const TTimeoutDuration             timeout)
        {
            I2CTransactionHandle transaction;

            struct BlockingCallbackHandler
            {
                std::atomic<bool> done = false;

                void on_complete()
                {
                    done.store(true, std::memory_order_release);
                }

                bool wait(TTimeoutDuration timeout)
                {
                    PrecisionTimer timer;

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
                    I2CTransactionCompleteCallback([&](I2CCallbackType) { callback_handler.on_complete(); }),
                    transaction))
            {
                return I2CBlockingTransactionError::kQueueFull;
            }

            // Wait for completion or timeout
            const bool timeout_occurred = callback_handler.wait(timeout) == false;

            // Timeout?
            if (timeout_occurred)
            {
                cancel_transaction(transaction);
                return I2CBlockingTransactionError::kTimeout;
            }

            // Check success
            if (transaction.status == I2CTransactionStatus::kDone)
            {
                return I2CBlockingTransactionError::kNone;
            }

            // Map errors
            if (!transaction.last_error.has_value())
            {
                return I2CBlockingTransactionError::kUnknown;
            }

            switch (transaction.last_error.value())
            {
                case I2CTransactionError::kNACK:
                    return I2CBlockingTransactionError::kNACK;
                case I2CTransactionError::kArbitrationLoss:
                    return I2CBlockingTransactionError::kArbitrationLoss;
                case I2CTransactionError::kBusError:
                    return I2CBlockingTransactionError::kBusError;
                case I2CTransactionError::kOverrunUnderrun:
                    return I2CBlockingTransactionError::kOverrunUnderrun;
                case I2CTransactionError::kSMBusTimeout:
                    return I2CBlockingTransactionError::kSMBusTimeout;
                case I2CTransactionError::kSMBusPecError:
                    return I2CBlockingTransactionError::kSMBusPecError;
                case I2CTransactionError::kSMBusAlert:
                    return I2CBlockingTransactionError::kSMBusAlert;
                default:
                    return I2CBlockingTransactionError::kUnknown;
            }
        }

        /**
         * @brief Cancels an ongoing or queued transaction.
         *
         * @param transaction The transaction handle to cancel.
         */
        inline void cancel_transaction(I2CTransactionHandle& transaction)
        {
            m_shared_state.with_lock([&](auto& state) { state.cancel_transaction(transaction); });
        }

        /**
         * @brief Query the status of a transaction.
         *
         * @param transaction The transaction handle to query.
         * @return I2CTransactionStatus The current status of the transaction.
         */
        inline I2CTransactionStatus query_status(const I2CTransactionHandle& transaction) const
        {
            return transaction.status;
        }

        /**
         * @brief Query the last error of a transaction.
         *
         * @param transaction The transaction handle to query.
         * @return std::optional<I2CTransactionError> The last error encountered, or std::nullopt if none.
         */
        inline std::optional<I2CTransactionError> query_last_error(const I2CTransactionHandle& transaction) const
        {
            return transaction.last_error;
        }

        // =============================================================================
        // ISR METHODS (Called from I2CISRRouter)
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
            m_shared_state.with_lock([](auto& state) { state.handle_error(I2CTransactionError::kNACK); });
        }

        /**
         * @brief Called from all error ISRs (ARLO, BERR, OVR, TIMEOUT, PECERR, ALERT, etc.)
         *
         */
        inline void handle_error_isr(const I2CErrorInterruptType& error)
        {
            I2CTransactionError mapped_error;
            switch (error)
            {
                case I2CErrorInterruptType::kArbitrationLoss:
                    mapped_error = I2CTransactionError::kArbitrationLoss;
                    break;
                case I2CErrorInterruptType::kBusError:
                    mapped_error = I2CTransactionError::kBusError;
                    break;
                case I2CErrorInterruptType::kOverrunUnderrun:
                    mapped_error = I2CTransactionError::kOverrunUnderrun;
                    break;
                case I2CErrorInterruptType::kSMBusTimeout:
                    mapped_error = I2CTransactionError::kSMBusTimeout;
                    break;
                case I2CErrorInterruptType::kSMBusPecError:
                    mapped_error = I2CTransactionError::kSMBusPecError;
                    break;
                case I2CErrorInterruptType::kSMBusAlert:
                    mapped_error = I2CTransactionError::kSMBusAlert;
                    break;
                default:
                    mapped_error = I2CTransactionError::kUnknown;
                    break;
            }

            m_shared_state.with_lock([mapped_error](auto& state) { state.handle_error(mapped_error); });
        }
    };

    template <I2CControllerID tkControllerID, uint16_t tkAddress, bool tkIs10BitAddress = false>
        requires(kValidI2CControllerID<tkControllerID>)
    class I2CCommandBufferSlaveDevice
    {
    public:
        struct Descriptor : public UniqueDeviceDescriptor
        {
        };

        constexpr static uint8_t skControllerID = tkControllerID;
        using ArbitratorT                       = I2CCommandBufferDevice<tkControllerID>;
        constexpr static auto skAddress         = I2CSlaveAddress<tkIs10BitAddress>(tkAddress);

        using InjectDevices = TypeList<ArbitratorT>;

    private:
        [[no_unique_address]] DeviceRef<ArbitratorT> m_controller;

        I2CTransactionHandle m_transaction;

    public:
        /**
         * @brief Constructs the I2C Command Buffer Slave Device with the given controller device.
         *
         * @param controller The I2C command buffer controller device reference.
         */
        I2CCommandBufferSlaveDevice(DeviceRef<ArbitratorT>&& controller) : m_controller(std::move(controller))
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
        inline bool submit_transaction(const std::span<const I2CCommand>& commands,
                                       I2CTransactionCompleteCallback&&   callback)
        {
            return m_controller->submit_transaction(skAddress, commands, std::move(callback), m_transaction);
        }

        /**
         * @brief Submits a new I2C Transaction to the queue and blocks until completion or timeout.
         *
         * @tparam TTimeoutDuration The type of the timeout duration.
         * @param commands The sequence of commands to execute.
         * @param timeout The maximum duration to wait for completion.
         * @return I2CBlockingTransactionError The result of the blocking transaction.
         */
        template <typename TTimeoutDuration>
        I2CBlockingTransactionError submit_transaction_blocking(const std::span<const I2CCommand>& commands,
                                                                const TTimeoutDuration             timeout)
        {
            return m_controller->template submit_transaction_blocking(skAddress, commands, timeout);
        }

        /**
         * @brief Cancels an ongoing or queued transaction.
         *
         */
        inline void cancel_transaction()
        {
            m_controller->cancel_transaction(m_transaction);
        }

        /**
         * @brief Query the status of the last transaction.
         *
         * @return I2CTransactionStatus The current status of the transaction.
         */
        inline I2CTransactionStatus query_status() const
        {
            return m_controller->query_status(m_transaction);
        }

        /**
         * @brief Query the last error of the last transaction.
         *
         * @return std::optional<I2CTransactionError> The last error encountered, or std::nullopt if none.
         */
        inline std::optional<I2CTransactionError> query_last_error() const
        {
            return m_controller->query_last_error(m_transaction);
        }
    };

    // --------------------------------------------------------------------------------
    // DEVICE ALIASES
    // --------------------------------------------------------------------------------

    template <uint8_t tkTransactionQueueSize = 10>
    using I2C1CommandBufferDevice = I2CCommandBufferDevice<1, tkTransactionQueueSize>;

    template <uint8_t tkTransactionQueueSize = 10>
    using I2C2CommandBufferDevice = I2CCommandBufferDevice<2, tkTransactionQueueSize>;

    template <uint8_t tkTransactionQueueSize = 10>
    using I2C3CommandBufferDevice = I2CCommandBufferDevice<3, tkTransactionQueueSize>;

    template <uint8_t tkTransactionQueueSize = 10>
    using I2C4CommandBufferDevice = I2CCommandBufferDevice<4, tkTransactionQueueSize>;

}  // namespace valle::platform

#define VALLE_BIND_I2C_COMMAND_BUFFER_DRIVER_ISR(instance)                                                       \
    namespace valle::platform                                                                                    \
    {                                                                                                            \
        template <>                                                                                              \
        struct I2CISRRouter<std::remove_cvref_t<decltype((instance))>::skControllerID,                           \
                            I2CInterruptType::kTransferComplete>                                                 \
        {                                                                                                        \
            static void handle()                                                                                 \
            {                                                                                                    \
                (instance).handle_tc_event_isr();                                                                \
            }                                                                                                    \
        };                                                                                                       \
                                                                                                                 \
        template <>                                                                                              \
        struct I2CISRRouter<std::remove_cvref_t<decltype((instance))>::skControllerID,                           \
                            I2CInterruptType::kStopDetection>                                                    \
        {                                                                                                        \
            static void handle()                                                                                 \
            {                                                                                                    \
                (instance).handle_stop_event_isr();                                                              \
            }                                                                                                    \
        };                                                                                                       \
                                                                                                                 \
        template <>                                                                                              \
        struct I2CISRRouter<std::remove_cvref_t<decltype((instance))>::skControllerID,                           \
                            I2CInterruptType::kNACKReceived>                                                     \
        {                                                                                                        \
            static void handle()                                                                                 \
            {                                                                                                    \
                (instance).handle_nack_event_isr();                                                              \
            }                                                                                                    \
        };                                                                                                       \
                                                                                                                 \
        template <>                                                                                              \
        struct I2CISRRouter<std::remove_cvref_t<decltype((instance))>::skControllerID, I2CInterruptType::kError> \
        {                                                                                                        \
            static void handle(I2CErrorInterruptType error)                                                      \
            {                                                                                                    \
                (instance).handle_error_isr(error);                                                              \
            }                                                                                                    \
        };                                                                                                       \
    }
