#pragma once

#include <chrono>
#include <span>

#include "Valle/Core/Logging/logger.hpp"
#include "Valle/Device/Devices/dma.hpp"
#include "Valle/Device/Traits/uart.hpp"
#include "Valle/Device/device_core.hpp"
#include "Valle/Drivers/gpio.hpp"

namespace valle
{
    // ============================================================================
    // CONFIGURATIONS
    // ============================================================================

    struct UARTControllerConfig
    {
        UARTBaudRate            baud_rate     = UARTBaudRate::kBaud115200;
        UARTWordLength          word_length   = UARTWordLength::kBits8;
        UARTStopBits            stop_bits     = UARTStopBits::kBits1;
        UARTParity              parity        = UARTParity::kNone;
        UARTTransferMode        transfer_mode = UARTTransferMode::kTxRx;
        UARTHardwareFlowControl hw_flow_ctrl  = UARTHardwareFlowControl::kNone;

        // DMA Settings
        DMAPriority dma_priority      = DMAPriority::kHigh;
        uint8_t     dma_int_priority  = 5;  // NVIC Priority (0-15)  // NOLINT(readability-magic-numbers)
        uint8_t     uart_int_priority = 5;  // NVIC Priority (0-15)  // NOLINT(readability-magic-numbers)
    };

    template <UARTControllerID tkControllerID>
    using UARTDefaultTxGPIOPin =
        GPIOPinDevice<UARTTraits<tkControllerID>::skDefaultGPIOTxPort, UARTTraits<tkControllerID>::skDefaultGPIOTxPin>;

    template <UARTControllerID tkControllerID>
    using UARTDefaultRxGPIOPin =
        GPIOPinDevice<UARTTraits<tkControllerID>::skDefaultGPIORxPort, UARTTraits<tkControllerID>::skDefaultGPIORxPin>;

    // =============================================================================
    // COMPILE TIME CONFIGURATIONS
    // =============================================================================
    // TODO: Add RX DMA Channel as well
    template <UARTControllerID tkControllerID>
    struct UARTControllerCTConfigDefaults
    {
        using TxPinT        = UARTDefaultTxGPIOPin<tkControllerID>;
        using RxPinT        = UARTDefaultRxGPIOPin<tkControllerID>;
        using CKPinT        = GPIONullPinDevice;
        using CTSPinT       = GPIONullPinDevice;
        using RTSPinT       = GPIONullPinDevice;
        using DMAChannelTxT = DMANullChannelDevice;
    };

    template <typename T>
    concept CValidUARTControllerCTConfig =
        requires {
            typename T::TxPinT;
            typename T::RxPinT;
            typename T::CKPinT;
            typename T::CTSPinT;
            typename T::RTSPinT;
            typename T::DMAChannelTxT;
        } && (CGPIOPinDevice<typename T::TxPinT> || CNullGPIOPinDevice<typename T::TxPinT>) &&
        (CGPIOPinDevice<typename T::RxPinT> || CNullGPIOPinDevice<typename T::RxPinT>) &&
        (CGPIOPinDevice<typename T::CKPinT> || CNullGPIOPinDevice<typename T::CKPinT>) &&
        (CGPIOPinDevice<typename T::CTSPinT> || CNullGPIOPinDevice<typename T::CTSPinT>) &&
        (CGPIOPinDevice<typename T::RTSPinT> || CNullGPIOPinDevice<typename T::RTSPinT>) &&
        (CDMAChannelDevice<typename T::DMAChannelTxT> || CNullDMAChannel<typename T::DMAChannelTxT>);

    template <UARTControllerID tkControllerID>
    struct UARTControllerCTConfigTraits
    {
        static constexpr auto skConfig = UARTControllerCTConfigDefaults<tkControllerID>{};
    };

#define VALLE_DEFINE_UART_CONTROLLER_CT_CONFIG(tkControllerID, config)                                            \
    namespace valle                                                                                               \
    {                                                                                                             \
        template <>                                                                                               \
        struct UARTControllerCTConfigTraits<(tkControllerID)>                                                     \
        {                                                                                                         \
            static constexpr auto skConfig = (config);                                                            \
            static_assert(CValidUARTControllerCTConfig<decltype(skConfig)>, "Invalid UART Controller CT Config"); \
        };                                                                                                        \
    }

    // ============================================================================
    // INTERRUPT TRAITS
    // ============================================================================

    enum class UARTInterruptType : uint8_t
    {
        // --- Basic ---
        kTxEmpty,                // TXE / TXFNF: Transmit data register empty (or FIFO not full)
        kTxComplete,             // TC: Transmission complete
        kRxNotEmpty,             // RXNE / RXFNE: Read data register not empty (or FIFO not empty)
        kIdle,                   // IDLE: Idle line detected
        kTxCompleteBeforeGuard,  // TCBGT (G4 specific, used in Smartcard/RS485)

        // --- Error ---
        kOverrun,           // ORE: Overrun error
        kParityError,       // PE: Parity error
        kFramingError,      // FE: Framing error
        kNoiseError,        // NE: Noise error
        kSpiSlaveUnderrun,  // UDR (Specific to SPI slave mode)

        // --- Hardware Flow Control ---
        kCTS,  // CTSIF (CTS Interrupt Flag)

        // --- FIFO Management (G4 Specific) ---
        kTxFifoEmpty,      // TXFE: TX FIFO is completely empty
        kRxFifoFull,       // RXFF: RX FIFO is completely full
        kTxFifoThreshold,  // TXFT: TX FIFO reached threshold (set in CR3)
        kRxFifoThreshold,  // RXFT: RX FIFO reached threshold (set in CR3)

        // --- Management / Protocol ---
        kCharMatch,        // CMF: Character match
        kReceiverTimeout,  // RTO: Receiver timeout (no data for specific duration)
        kEndOfBlock,       // EOB: End of Block (Smartcard mode)
        kLinBreak,         // LBD: LIN break detection
        kWakeup,           // WUF: Wakeup from Stop mode

        // --- Auto Baud Rate ---
        kAutoBaudError,    // ABRE: Auto-baud rate error
        kAutoBaudFinished  // ABRF: Auto-baud rate finished
    };

    template <UARTControllerID tkControllerID, UARTInterruptType tkIntType>
    struct UARTInterruptTraits;

#define DEFINE_UART_INT_TRAIT(tkIntType, ll_name)                                                                      \
    template <UARTControllerID tkControllerID>                                                                         \
    struct UARTInterruptTraits<tkControllerID, (tkIntType)>                                                            \
    {                                                                                                                  \
        static inline void enable()                                                                                    \
        {                                                                                                              \
            LL_USART_EnableIT_##ll_name(UARTTraits<tkControllerID>::skInstance);                                       \
        }                                                                                                              \
        static inline void disable()                                                                                   \
        {                                                                                                              \
            LL_USART_DisableIT_##ll_name(UARTTraits<tkControllerID>::skInstance);                                      \
        }                                                                                                              \
        static inline bool is_enabled()                                                                                \
        {                                                                                                              \
            return LL_USART_IsEnabledIT_##ll_name(UARTTraits<tkControllerID>::skInstance);                             \
        }                                                                                                              \
        static inline bool flag_active()                                                                               \
        {                                                                                                              \
            return LL_USART_IsActiveFlag_##ll_name(UARTTraits<tkControllerID>::skInstance);                            \
        }                                                                                                              \
        static inline bool is_pending()                                                                                \
        {                                                                                                              \
            return flag_active() && is_enabled();                                                                      \
        }                                                                                                              \
        static inline void ack()                                                                                       \
        {                                                                                                              \
            /* Note: Some flags (TXE, RXNE) have no clear register; */                                                 \
            /* they are cleared by RDR/TDR access. LL handles this. */                                                 \
            if constexpr ((tkIntType) != UARTInterruptType::kTxEmpty && (tkIntType) != UARTInterruptType::kRxNotEmpty) \
            {                                                                                                          \
                LL_USART_ClearFlag_##ll_name(UARTTraits<tkControllerID>::skInstance);                                  \
            }                                                                                                          \
        }                                                                                                              \
    };

    // Basic
    DEFINE_UART_INT_TRAIT(UARTInterruptType::kTxEmpty, TXE_TXFNF);
    DEFINE_UART_INT_TRAIT(UARTInterruptType::kTxComplete, TC);
    DEFINE_UART_INT_TRAIT(UARTInterruptType::kTxCompleteBeforeGuard, TCBGT);
    DEFINE_UART_INT_TRAIT(UARTInterruptType::kRxNotEmpty, RXNE_RXFNE);
    DEFINE_UART_INT_TRAIT(UARTInterruptType::kIdle, IDLE);

    // Errors
    DEFINE_UART_INT_TRAIT(UARTInterruptType::kOverrun, ORE);
    DEFINE_UART_INT_TRAIT(UARTInterruptType::kParityError, PE);
    DEFINE_UART_INT_TRAIT(UARTInterruptType::kFramingError, FE);
    DEFINE_UART_INT_TRAIT(UARTInterruptType::kNoiseError, NE);
    DEFINE_UART_INT_TRAIT(UARTInterruptType::kSpiSlaveUnderrun, UDR);

    // Flow Control
    DEFINE_UART_INT_TRAIT(UARTInterruptType::kCTS, CTS);  // LL uses 'CTS' for the CTSIF trait

    // FIFO
    DEFINE_UART_INT_TRAIT(UARTInterruptType::kTxFifoEmpty, TXFE);
    DEFINE_UART_INT_TRAIT(UARTInterruptType::kRxFifoFull, RXFF);
    DEFINE_UART_INT_TRAIT(UARTInterruptType::kTxFifoThreshold, TXFT);
    DEFINE_UART_INT_TRAIT(UARTInterruptType::kRxFifoThreshold, RXFT);

    // Protocol
    DEFINE_UART_INT_TRAIT(UARTInterruptType::kCharMatch, CM);
    DEFINE_UART_INT_TRAIT(UARTInterruptType::kReceiverTimeout, RTO);
    DEFINE_UART_INT_TRAIT(UARTInterruptType::kEndOfBlock, EOB);
    DEFINE_UART_INT_TRAIT(UARTInterruptType::kLinBreak, LBD);
    DEFINE_UART_INT_TRAIT(UARTInterruptType::kWakeup, WUF);

#undef DEFINE_UART_INT_TRAIT

    // ============================================================================
    // GLOBAL ISR ROUTER
    // ============================================================================

    /**
     * @brief Global ISR Router for a specific UART.
     * Specializing this allows you to handle the entire ISR in one function
     * (e.g., when delegating to the ST HAL).
     *
     * @tparam tkControllerID UART Controller ID.
     */
    template <UARTControllerID tkControllerID>
    struct UARTGlobalISRRouter
    {
        using UnboundIsrHandlerTag = void;

        static void handle()
        {
        }
    };

    // ============================================================================
    // GRANULAR ISR ROUTER
    // ============================================================================

    /**
     * @brief UART ISR Router
     *
     * Specialize this template in your application or driver to bind
     * logic to specific UART interrupts.
     *
     * @tparam tkControllerID UART Controller ID.
     * @tparam tkIntType      UART Interrupt Type.
     */
    template <UARTControllerID tkControllerID, UARTInterruptType tkIntType>
    struct UARTISRRouter
    {
        using UnboundIsrHandlerTag = void;

        static void handle()
        {
        }
    };

    // ============================================================================
    // FORWARD DECLARATIONS
    // ============================================================================
    class UARTRootDevice;

    template <UARTControllerID tkControllerID>
    class UARTControllerDevice;

    // ============================================================================
    // UART ROOT DEVICE
    // ============================================================================
    class UARTRootDevice
    {
    public:
        struct Descriptor : public InterfaceDeviceDescriptor
        {
            using Children = DeviceTreeList<UARTControllerDevice<kUSART1>,
                                            UARTControllerDevice<kUSART2>,
                                            UARTControllerDevice<kUSART3>,
                                            UARTControllerDevice<kUART4>,
                                            UARTControllerDevice<kUART5>,
                                            UARTControllerDevice<kLPUART1>>;
        };
    };

    // ============================================================================
    // UART CONTROLLER DEVICE
    // ============================================================================
    namespace detail
    {
        template <UARTControllerID tkID, typename TPinDevice, UARTPinType tkType>
        struct ValidUARTPinDeviceHelper
            : public std::bool_constant<CValidUARTPin<tkID, TPinDevice::skPortID, TPinDevice::skPinID, tkType>>
        {
        };

        template <UARTControllerID tkID, UARTPinType tkType>
        struct ValidUARTPinDeviceHelper<tkID, GPIONullPinDevice, tkType> : public std::bool_constant<true>
        {
        };

        template <UARTControllerID tkControllerID, typename TPinDevice, UARTPinType tkType>
        struct UARTGPIOPinDriverHelper
        {
            using type = GPIOAlternativeFunctionDriver<
                TPinDevice,
                UARTGPIOPinAF<tkControllerID, TPinDevice::skPortID, TPinDevice::skPinID, tkType>::skAF>;
        };

        template <UARTControllerID tkControllerID, UARTPinType tkType>
        struct UARTGPIOPinDriverHelper<tkControllerID, GPIONullPinDevice, tkType>
        {
            using type = std::monostate;
        };

    }  // namespace detail

    template <UARTControllerID tkID, typename TPinDevice, UARTPinType tkType>
    concept CValidUARTPinDevice = detail::ValidUARTPinDeviceHelper<tkID, TPinDevice, tkType>::value;

    template <UARTControllerID tkID, typename TPinDevice>
    concept CValidUARTRxPinDevice = CValidUARTPinDevice<tkID, TPinDevice, UARTPinType::kRx>;

    template <UARTControllerID tkID, typename TPinDevice>
    concept CValidUARTTxPinDevice = CValidUARTPinDevice<tkID, TPinDevice, UARTPinType::kTx>;

    template <UARTControllerID tkControllerID, typename TPinDevice>
    concept CValidUARTCKPinDevice = CValidUARTPinDevice<tkControllerID, TPinDevice, UARTPinType::kCK>;

    template <UARTControllerID tkControllerID, typename TPinDevice>
    concept CValidUARTCTSPinDevice = CValidUARTPinDevice<tkControllerID, TPinDevice, UARTPinType::kCTS>;

    template <UARTControllerID tkControllerID, typename TPinDevice>
    concept CValidUARTRTSPinDevice = CValidUARTPinDevice<tkControllerID, TPinDevice, UARTPinType::kRTS>;

    template <UARTControllerID tkControllerID, typename TPinDevice, UARTPinType tkType>
    using UARTGPIOPinDriver = typename detail::UARTGPIOPinDriverHelper<tkControllerID, TPinDevice, tkType>::type;

    template <UARTControllerID tkControllerID, typename TPinDevice>
    using UARTTxPinDriver = UARTGPIOPinDriver<tkControllerID, TPinDevice, UARTPinType::kTx>;

    template <UARTControllerID tkControllerID, typename TPinDevice>
    using UARTRxPinDriver = UARTGPIOPinDriver<tkControllerID, TPinDevice, UARTPinType::kRx>;

    template <UARTControllerID tkControllerID, typename TPinDevice>
    using UARTCKPinDriver = UARTGPIOPinDriver<tkControllerID, TPinDevice, UARTPinType::kCK>;

    template <UARTControllerID tkControllerID, typename TPinDevice>
    using UARTCTSPinDriver = UARTGPIOPinDriver<tkControllerID, TPinDevice, UARTPinType::kCTS>;

    template <UARTControllerID tkControllerID, typename TPinDevice>
    using UARTRTSPinDriver = UARTGPIOPinDriver<tkControllerID, TPinDevice, UARTPinType::kRTS>;

    template <UARTControllerID tkControllerID>
    class UARTControllerDevice
    {
    public:
        struct Descriptor : public UniqueDeviceDescriptor
        {
        };

        using ControllerTraitsT                          = UARTTraits<tkControllerID>;
        static constexpr UARTControllerID skControllerID = tkControllerID;

        using CTConfigTraitT             = UARTControllerCTConfigTraits<tkControllerID>;
        static constexpr auto skCTConfig = CTConfigTraitT::skConfig;
        using CTConfigT                  = decltype(skCTConfig);

        using TxPinT = typename CTConfigT::TxPinT;
        static_assert(CValidUARTTxPinDevice<tkControllerID, TxPinT>, "Invalid UART TX Pin Device");
        using RxPinT = typename CTConfigT::RxPinT;
        static_assert(CValidUARTRxPinDevice<tkControllerID, RxPinT>, "Invalid UART RX Pin Device");
        using TxPinDriverT   = UARTTxPinDriver<tkControllerID, TxPinT>;
        using RxPinDriverT   = UARTRxPinDriver<tkControllerID, RxPinT>;
        using InjectDevices1 = TypeList<TxPinT, RxPinT>;

        using CKPinT = typename CTConfigT::CKPinT;
        static_assert(CVoid<CKPinT> || CValidUARTCKPinDevice<tkControllerID, CKPinT>, "Invalid UART CK Pin Device");
        static constexpr bool skHasCKPin = !CNullGPIOPinDevice<CKPinT>;
        using CKPinDriverT               = UARTCKPinDriver<tkControllerID, CKPinT>;
        using InjectDevices2             = std::conditional_t<skHasCKPin, TypeList<CKPinT>, TypeList<>>;

        using CTSPinT = typename CTConfigT::CTSPinT;
        static_assert(CVoid<CTSPinT> || CValidUARTCTSPinDevice<tkControllerID, CTSPinT>, "Invalid UART CTS Pin Device");
        static constexpr bool skHasCTSPin = !CNullGPIOPinDevice<CTSPinT>;
        using CTSPinDriverT               = UARTCTSPinDriver<tkControllerID, CTSPinT>;
        using InjectDevices3              = std::conditional_t<skHasCTSPin, TypeList<CTSPinT>, TypeList<>>;

        using RTSPinT = typename CTConfigT::RTSPinT;
        static_assert(CVoid<RTSPinT> || CValidUARTRTSPinDevice<tkControllerID, RTSPinT>, "Invalid UART RTS Pin Device");
        static constexpr bool skHasRTSPin = !CNullGPIOPinDevice<RTSPinT>;
        using RTSPinDriverT               = UARTRTSPinDriver<tkControllerID, RTSPinT>;
        using InjectDevices4              = std::conditional_t<skHasRTSPin, TypeList<RTSPinT>, TypeList<>>;

        using DMAChannelTxT              = typename CTConfigT::DMAChannelTxT;
        static constexpr bool skHasDMATx = !CNullDMAChannel<DMAChannelTxT>;
        using InjectDevices5             = std::conditional_t<skHasDMATx, TypeList<DMAChannelTxT>, TypeList<>>;

        using DependentDevices = TypeList<UARTRootDevice>;
        using InjectDevices =
            typename TypeListConcat<InjectDevices1, InjectDevices2, InjectDevices3, InjectDevices4, InjectDevices5>::
                type;

    private:
        [[no_unique_address]] TxPinDriverT                                                   m_tx_pin{};
        [[no_unique_address]] RxPinDriverT                                                   m_rx_pin{};
        [[no_unique_address]] std::conditional_t<skHasCKPin, CKPinDriverT, std::monostate>   m_ck_pin{};
        [[no_unique_address]] std::conditional_t<skHasCTSPin, CTSPinDriverT, std::monostate> m_cts_pin{};
        [[no_unique_address]] std::conditional_t<skHasRTSPin, RTSPinDriverT, std::monostate> m_rts_pin{};
        [[no_unique_address]] ConditionalDeviceRef<skHasDMATx, DMAChannelTxT>                m_dma_tx{};

        // HAL handles
        UART_HandleTypeDef                                                m_huart{};
        std::conditional_t<skHasDMATx, DMA_HandleTypeDef, std::monostate> m_hdma_tx{};

    public:
        template <typename... Args>
        explicit UARTControllerDevice(Args&&... args)
            : m_tx_pin(extract_device_ref<true, TxPinT>(std::forward<Args>(args)...))
            , m_rx_pin(extract_device_ref<true, RxPinT>(std::forward<Args>(args)...))
            , m_ck_pin(extract_device_ref<skHasCKPin, CKPinT>(std::forward<Args>(args)...))
            , m_cts_pin(extract_device_ref<skHasCTSPin, CTSPinT>(std::forward<Args>(args)...))
            , m_rts_pin(extract_device_ref<skHasRTSPin, RTSPinT>(std::forward<Args>(args)...))
            , m_dma_tx(extract_device_ref<skHasDMATx, DMAChannelTxT>(std::forward<Args>(args)...))
        {
        }

        [[nodiscard]] bool init(const UARTControllerConfig& config)
        {
            // Enable Peripheral Clock
            ControllerTraitsT::enable_clock();

            // Init Pins (AF mode)
            if (!m_tx_pin.init(GPIOAlternativeFunctionConfig{
                    .mode = GPIOAlternateFunctionMode::kPushPull,
                    // Use Medium speed for bauds > 1M to keep edges sharp
                    .speed = (static_cast<uint32_t>(config.baud_rate) > 1000000) ? GPIOSpeedMode::kMedium
                                                                                 : GPIOSpeedMode::kLow,
                    .pull  = GPIOPullMode::kPullUp,
                }))
            {
                VALLE_LOG_FATAL("Failed to initialize UART TX Pin");
                return false;
            }

            if (!m_rx_pin.init(GPIOAlternativeFunctionConfig{
                    .mode  = GPIOAlternateFunctionMode::kPushPull,
                    .speed = GPIOSpeedMode::kLow,
                    .pull  = GPIOPullMode::kPullUp,
                }))
            {
                VALLE_LOG_FATAL("Failed to initialize UART RX Pin");
                return false;
            }

            if constexpr (skHasCKPin)
            {
                if (!m_ck_pin.init(GPIOAlternativeFunctionConfig{
                        .mode  = GPIOAlternateFunctionMode::kPushPull,
                        .speed = GPIOSpeedMode::kHigh,
                        .pull  = GPIOPullMode::kNoPull,
                    }))
                {
                    VALLE_LOG_FATAL("Failed to initialize UART CK Pin");
                    return false;
                }
            }

            if constexpr (skHasCTSPin)
            {
                if (!m_cts_pin.init(GPIOAlternativeFunctionConfig{
                        .mode  = GPIOAlternateFunctionMode::kPushPull,
                        .speed = GPIOSpeedMode::kLow,
                        .pull  = GPIOPullMode::kPullUp,
                    }))
                {
                    VALLE_LOG_FATAL("Failed to initialize UART CTS Pin");
                    return false;
                }
            }

            if constexpr (skHasRTSPin)
            {
                if (!m_rts_pin.init(GPIOAlternativeFunctionConfig{
                        .mode  = GPIOAlternateFunctionMode::kPushPull,
                        .speed = GPIOSpeedMode::kMedium,
                        .pull  = GPIOPullMode::kPullUp,
                    }))
                {
                    VALLE_LOG_FATAL("Failed to initialize UART RTS Pin");
                    return false;
                }
            }

            // Configure UART Handle
            std::fill_n(reinterpret_cast<uint8_t*>(&m_huart), sizeof(m_huart), 0);
            m_huart.Instance            = ControllerTraitsT::skInstance;
            m_huart.Init.BaudRate       = static_cast<uint32_t>(config.baud_rate);
            m_huart.Init.WordLength     = static_cast<uint32_t>(config.word_length);
            m_huart.Init.StopBits       = static_cast<uint32_t>(config.stop_bits);
            m_huart.Init.Parity         = static_cast<uint32_t>(config.parity);
            m_huart.Init.Mode           = static_cast<uint32_t>(config.transfer_mode);
            m_huart.Init.HwFlowCtl      = static_cast<uint32_t>(config.hw_flow_ctrl);
            m_huart.Init.OverSampling   = UART_OVERSAMPLING_16;
            m_huart.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
            m_huart.Init.ClockPrescaler = UART_PRESCALER_DIV1;

            HAL_UART_Init(&m_huart);

            // Link DMA if present
            if constexpr (skHasDMATx)
            {
                // We don't use the DMA device to init the DMA channel since HAL handles DMA differently
                // We only hold the dma channel device to lock it.

                m_hdma_tx.Instance                 = DMAChannelTxT::ChannelTraitsT::skInstance;
                m_hdma_tx.Init.Request             = static_cast<uint32_t>(ControllerTraitsT::skDMAMuxRequestTx);
                m_hdma_tx.Init.Direction           = DMA_MEMORY_TO_PERIPH;
                m_hdma_tx.Init.PeriphInc           = DMA_PINC_DISABLE;
                m_hdma_tx.Init.MemInc              = DMA_MINC_ENABLE;
                m_hdma_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
                m_hdma_tx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
                m_hdma_tx.Init.Mode                = DMA_NORMAL;
                m_hdma_tx.Init.Priority            = static_cast<uint32_t>(config.dma_priority);

                HAL_DMA_Init(&m_hdma_tx);
                __HAL_LINKDMA(&m_huart, hdmatx, m_hdma_tx);

                // Enable UART Interrupt (required for HAL DMA mode to clean up after transfer)
                m_dma_tx->enable_interrupts(DMAInterruptConfig{
                    .priority  = config.dma_int_priority,
                    .enable_tc = true,
                    .enable_ht = true,
                    .enable_te = true,
                });
            }

            HAL_NVIC_SetPriority(ControllerTraitsT::skIRQn, config.uart_int_priority, 0);
            HAL_NVIC_EnableIRQ(ControllerTraitsT::skIRQn);

            return true;
        }

        // --- API ---
        HAL_StatusTypeDef transmit(std::span<const std::byte> data, std::chrono::milliseconds timeout_ms)
        {
            return HAL_UART_Transmit(&m_huart,
                                     reinterpret_cast<const uint8_t*>(data.data()),
                                     static_cast<uint16_t>(data.size()),
                                     timeout_ms.count());
        }

        HAL_StatusTypeDef transmit_async(std::span<const std::byte> data)
        {
            if constexpr (skHasDMATx)  // NOLINT(bugprone-branch-clone)
            {
                return HAL_UART_Transmit_DMA(&m_huart,
                                             const_cast<uint8_t*>(reinterpret_cast<const uint8_t*>(data.data())),
                                             static_cast<uint16_t>(data.size()));
            }
            else
            {
                return HAL_ERROR;
            }
        }

        // Required for HAL to work: needs to be called from the actual ISR
        void handle_dma_irq()
        {
            HAL_DMA_IRQHandler(&m_hdma_tx);
        }

        void handle_uart_irq()
        {
            HAL_UART_IRQHandler(&m_huart);
        }
    };

    using USART1ControllerDevice  = UARTControllerDevice<kUSART1>;
    using USART2ControllerDevice  = UARTControllerDevice<kUSART2>;
    using USART3ControllerDevice  = UARTControllerDevice<kUSART3>;
    using UART4ControllerDevice   = UARTControllerDevice<kUART4>;
    using UART5ControllerDevice   = UARTControllerDevice<kUART5>;
    using LPUART1ControllerDevice = UARTControllerDevice<kLPUART1>;  // Usually the main USB UART

}  // namespace valle

// Put these in app_isr_bindings.hpp to bind your instance
#define VALLE_BIND_UART_DMA_ISR_ROUTER(instance)                                                            \
    namespace valle                                                                                         \
    {                                                                                                       \
        static_assert(std::remove_cvref_t<decltype((instance))>::skHasDMATx,                                \
                      "Instance does not have DMA TX capability");                                          \
        template <>                                                                                         \
        struct DMAGlobalISRRouter<std::remove_cvref_t<decltype((instance))>::DMAChannelTxT::skControllerID, \
                                  std::remove_cvref_t<decltype((instance))>::DMAChannelTxT::skChannelID>    \
        {                                                                                                   \
            static void handle()                                                                            \
            {                                                                                               \
                (instance).handle_dma_irq();                                                                \
            }                                                                                               \
        };                                                                                                  \
    }

#define VALLE_BIND_UART_ISR_ROUTER(instance)                                                  \
    namespace valle                                                                           \
    {                                                                                         \
        template <>                                                                           \
        struct UARTGlobalISRRouter<std::remove_cvref_t<decltype((instance))>::skControllerID> \
        {                                                                                     \
            static void handle()                                                              \
            {                                                                                 \
                (instance).handle_uart_irq();                                                 \
            }                                                                                 \
        };                                                                                    \
    }
