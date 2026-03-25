#pragma once

#include <chrono>
#include <span>

#include "valle/platform/core.hpp"
#include "valle/platform/devices/dma.hpp"
#include "valle/platform/drivers/gpio/alternate_function.hpp"
#include "valle/platform/hardware/uart.hpp"

namespace valle::platform
{
    // ============================================================================
    // UART CONTROLLER DEVICE
    // ============================================================================

    // -----------------------------------------------------------------------------
    // COMPILE TimE CONFIGURATIONS
    // -----------------------------------------------------------------------------

    template <UartPeripheralId tkPeripheralId>
    using UartDefaultTxGpioPin = GpioPinDevice<UartControllerTraits<tkPeripheralId>::skDefaultGpioTxPort,
                                               UartControllerTraits<tkPeripheralId>::skDefaultGpioTxPin>;

    template <UartPeripheralId tkPeripheralId>
    using UartDefaultRxGpioPin = GpioPinDevice<UartControllerTraits<tkPeripheralId>::skDefaultGpioRxPort,
                                               UartControllerTraits<tkPeripheralId>::skDefaultGpioRxPin>;

    // TODO: Add RX DMA Channel as well
    template <UartPeripheralId tkPeripheralId>
    struct UartControllerCTDefaultConfig
    {
        using TxPinT        = UartDefaultTxGpioPin<tkPeripheralId>;
        using RxPinT        = UartDefaultRxGpioPin<tkPeripheralId>;
        using CKPinT        = GpioNullPinDevice;
        using CTSPinT       = GpioNullPinDevice;
        using RTSPinT       = GpioNullPinDevice;
        using DmaChannelTxT = DmaNullChannelDevice;
    };

    template <typename T>
    concept CValidUartControllerCTConfig =
        requires {
            typename T::TxPinT;
            typename T::RxPinT;
            typename T::CKPinT;
            typename T::CTSPinT;
            typename T::RTSPinT;
            typename T::DmaChannelTxT;
        } && (CGpioPinDevice<typename T::TxPinT> || CNullGpioPinDevice<typename T::TxPinT>) &&
        (CGpioPinDevice<typename T::RxPinT> || CNullGpioPinDevice<typename T::RxPinT>) &&
        (CGpioPinDevice<typename T::CKPinT> || CNullGpioPinDevice<typename T::CKPinT>) &&
        (CGpioPinDevice<typename T::CTSPinT> || CNullGpioPinDevice<typename T::CTSPinT>) &&
        (CGpioPinDevice<typename T::RTSPinT> || CNullGpioPinDevice<typename T::RTSPinT>) &&
        (CDmaChannelDevice<typename T::DmaChannelTxT> || CNullDmaChannel<typename T::DmaChannelTxT>);

    template <UartPeripheralId tkPeripheralId>
    struct UartControllerCTConfigRegistry
    {
        static constexpr auto skConfig = UartControllerCTDefaultConfig<tkPeripheralId>{};
    };

#define VALLE_DEFINE_UART_CONTROLLER_CT_CONFIG(tkPeripheralId, config)                                            \
    namespace valle::platform                                                                                     \
    {                                                                                                             \
        template <>                                                                                               \
        struct UartControllerCTConfigRegistry<(tkPeripheralId)>                                                   \
        {                                                                                                         \
            static constexpr auto skConfig = (config);                                                            \
            static_assert(CValidUartControllerCTConfig<decltype(skConfig)>, "Invalid UART Controller CT Config"); \
        };                                                                                                        \
    }

    // -----------------------------------------------------------------------------
    // CONFIGURATIONS
    // -----------------------------------------------------------------------------

    struct UartControllerConfig
    {
        UartBaudRate            baud_rate     = UartBaudRate::kBaud115200;
        UartWordLength          word_length   = UartWordLength::kBits8;
        UartStopBits            stop_bits     = UartStopBits::kBits1;
        UartParity              parity        = UartParity::kNone;
        UartTransferMode        transfer_mode = UartTransferMode::kTxRx;
        UartHardwareFlowControl hw_flow_ctrl  = UartHardwareFlowControl::kNone;

        // DMA Settings
        DmaPriority dma_priority      = DmaPriority::kHigh;
        uint8_t     dma_int_priority  = 5;  // NVIC Priority (0-15)  // NOLINT(readability-magic-numbers)
        uint8_t     uart_int_priority = 5;  // NVIC Priority (0-15)  // NOLINT(readability-magic-numbers)
    };

    // -----------------------------------------------------------------------------
    // HELPERS
    // -----------------------------------------------------------------------------

    namespace detail
    {
        template <UartPeripheralId tkPeripheralId, typename TPinDevice, UartPinType tkType>
        struct ValidUartPinDeviceHelper
            : public std::bool_constant<
                  CValidUartPin<tkPeripheralId, TPinDevice::skPortId, TPinDevice::skPinId, tkType>>
        {
        };

        template <UartPeripheralId tkPeripheralId, UartPinType tkType>
        struct ValidUartPinDeviceHelper<tkPeripheralId, GpioNullPinDevice, tkType> : public std::bool_constant<true>
        {
        };

        template <UartPeripheralId tkPeripheralId, typename TPinDevice, UartPinType tkType>
        struct UartGpioPinDriverHelper
        {
            using type = GpioAlternateFunctionDriver<
                TPinDevice,
                UartGpioPinAF<tkPeripheralId, TPinDevice::skPortId, TPinDevice::skPinId, tkType>::skAF>;
        };

        template <UartPeripheralId tkPeripheralId, UartPinType tkType>
        struct UartGpioPinDriverHelper<tkPeripheralId, GpioNullPinDevice, tkType>
        {
            using type = std::monostate;
        };

    }  // namespace detail

    template <UartPeripheralId tkPeripheralId, typename TPinDevice, UartPinType tkType>
    concept CValidUartPinDevice = detail::ValidUartPinDeviceHelper<tkPeripheralId, TPinDevice, tkType>::value;

    template <UartPeripheralId tkPeripheralId, typename TPinDevice>
    concept CValidUartRxPinDevice = CValidUartPinDevice<tkPeripheralId, TPinDevice, UartPinType::kRx>;

    template <UartPeripheralId tkPeripheralId, typename TPinDevice>
    concept CValidUartTxPinDevice = CValidUartPinDevice<tkPeripheralId, TPinDevice, UartPinType::kTx>;

    template <UartPeripheralId tkPeripheralId, typename TPinDevice>
    concept CValidUartCKPinDevice = CValidUartPinDevice<tkPeripheralId, TPinDevice, UartPinType::kCK>;

    template <UartPeripheralId tkPeripheralId, typename TPinDevice>
    concept CValidUartCTSPinDevice = CValidUartPinDevice<tkPeripheralId, TPinDevice, UartPinType::kCTS>;

    template <UartPeripheralId tkPeripheralId, typename TPinDevice>
    concept CValidUartRTSPinDevice = CValidUartPinDevice<tkPeripheralId, TPinDevice, UartPinType::kRTS>;

    template <UartPeripheralId tkPeripheralId, typename TPinDevice, UartPinType tkType>
    using UartGpioPinDriver = typename detail::UartGpioPinDriverHelper<tkPeripheralId, TPinDevice, tkType>::type;

    template <UartPeripheralId tkPeripheralId, typename TPinDevice>
    using UartTxPinDriver = UartGpioPinDriver<tkPeripheralId, TPinDevice, UartPinType::kTx>;

    template <UartPeripheralId tkPeripheralId, typename TPinDevice>
    using UartRxPinDriver = UartGpioPinDriver<tkPeripheralId, TPinDevice, UartPinType::kRx>;

    template <UartPeripheralId tkPeripheralId, typename TPinDevice>
    using UartCKPinDriver = UartGpioPinDriver<tkPeripheralId, TPinDevice, UartPinType::kCK>;

    template <UartPeripheralId tkPeripheralId, typename TPinDevice>
    using UartCTSPinDriver = UartGpioPinDriver<tkPeripheralId, TPinDevice, UartPinType::kCTS>;

    template <UartPeripheralId tkPeripheralId, typename TPinDevice>
    using UartRTSPinDriver = UartGpioPinDriver<tkPeripheralId, TPinDevice, UartPinType::kRTS>;

    // -----------------------------------------------------------------------------
    // DEVICE CLASS
    // -----------------------------------------------------------------------------

    template <UartPeripheralId tkPeripheralId>
    class UartControllerDevice
    {
    public:
        struct Descriptor : public UniqueDeviceDescriptor
        {
        };

        using ControllerTraitsT                          = UartControllerTraits<tkPeripheralId>;
        static constexpr UartPeripheralId skPeripheralId = tkPeripheralId;

        using CTConfigRegistryT          = UartControllerCTConfigRegistry<tkPeripheralId>;
        static constexpr auto skCTConfig = CTConfigRegistryT::skConfig;
        using CTConfigT                  = decltype(skCTConfig);

        using TxPinT = typename CTConfigT::TxPinT;
        static_assert(CValidUartTxPinDevice<tkPeripheralId, TxPinT>, "Invalid UART TX Pin Device");
        using RxPinT = typename CTConfigT::RxPinT;
        static_assert(CValidUartRxPinDevice<tkPeripheralId, RxPinT>, "Invalid UART RX Pin Device");
        using TxPinDriverT   = UartTxPinDriver<tkPeripheralId, TxPinT>;
        using RxPinDriverT   = UartRxPinDriver<tkPeripheralId, RxPinT>;
        using InjectDevices1 = TypeList<TxPinT, RxPinT>;

        using CKPinT = typename CTConfigT::CKPinT;
        static_assert(CVoid<CKPinT> || CValidUartCKPinDevice<tkPeripheralId, CKPinT>, "Invalid UART CK Pin Device");
        static constexpr bool skHasCKPin = !CNullGpioPinDevice<CKPinT>;
        using CKPinDriverT               = UartCKPinDriver<tkPeripheralId, CKPinT>;
        using InjectDevices2             = std::conditional_t<skHasCKPin, TypeList<CKPinT>, TypeList<>>;

        using CTSPinT = typename CTConfigT::CTSPinT;
        static_assert(CVoid<CTSPinT> || CValidUartCTSPinDevice<tkPeripheralId, CTSPinT>, "Invalid UART CTS Pin Device");
        static constexpr bool skHasCTSPin = !CNullGpioPinDevice<CTSPinT>;
        using CTSPinDriverT               = UartCTSPinDriver<tkPeripheralId, CTSPinT>;
        using InjectDevices3              = std::conditional_t<skHasCTSPin, TypeList<CTSPinT>, TypeList<>>;

        using RTSPinT = typename CTConfigT::RTSPinT;
        static_assert(CVoid<RTSPinT> || CValidUartRTSPinDevice<tkPeripheralId, RTSPinT>, "Invalid UART RTS Pin Device");
        static constexpr bool skHasRTSPin = !CNullGpioPinDevice<RTSPinT>;
        using RTSPinDriverT               = UartRTSPinDriver<tkPeripheralId, RTSPinT>;
        using InjectDevices4              = std::conditional_t<skHasRTSPin, TypeList<RTSPinT>, TypeList<>>;

        using DmaChannelTxT              = typename CTConfigT::DmaChannelTxT;
        static constexpr bool skHasDmaTx = !CNullDmaChannel<DmaChannelTxT>;
        using InjectDevices5             = std::conditional_t<skHasDmaTx, TypeList<DmaChannelTxT>, TypeList<>>;

        using InjectDevices =
            typename TypeListConcat<InjectDevices1, InjectDevices2, InjectDevices3, InjectDevices4, InjectDevices5>::
                type;

    private:
        [[no_unique_address]] ConditionalDeviceRef<skHasDmaTx, DmaChannelTxT> m_dma_tx{};
        TxPinDriverT                                                          m_tx_pin{};
        RxPinDriverT                                                          m_rx_pin{};
        CKPinDriverT                                                          m_ck_pin{};
        CTSPinDriverT                                                         m_cts_pin{};
        RTSPinDriverT                                                         m_rts_pin{};

        // HAL handles
        Uart_HandleTypeDef                                                m_huart{};
        std::conditional_t<skHasDmaTx, Dma_HandleTypeDef, std::monostate> m_hdma_tx{};

    public:
        template <typename... Args>
        explicit UartControllerDevice(Args&&... args)
            : m_tx_pin(extract_device_ref<true, TxPinT>(std::forward<Args>(args)...))
            , m_rx_pin(extract_device_ref<true, RxPinT>(std::forward<Args>(args)...))
            , m_ck_pin(extract_device_ref<skHasCKPin, CKPinT>(std::forward<Args>(args)...))
            , m_cts_pin(extract_device_ref<skHasCTSPin, CTSPinT>(std::forward<Args>(args)...))
            , m_rts_pin(extract_device_ref<skHasRTSPin, RTSPinT>(std::forward<Args>(args)...))
            , m_dma_tx(extract_device_ref<skHasDmaTx, DmaChannelTxT>(std::forward<Args>(args)...))
        {
        }

        [[nodiscard]] bool init(const UartControllerConfig& config)
        {
            // Enable Peripheral Clock
            ControllerTraitsT::enable_clock();

            // Init Pins (AF mode)
            if (!m_tx_pin.init(GpioAlternateFunctionConfig{
                    .mode = GpioAlternateFunctionMode::kPushPull,
                    // Use Medium speed for bauds > 1M to keep edges sharp
                    .speed = (static_cast<uint32_t>(config.baud_rate) > 1000000) ? GpioSpeedMode::kMedium
                                                                                 : GpioSpeedMode::kLow,
                    .pull  = GpioPullMode::kPullUp,
                }))
            {
                VALLE_LOG_FATAL("Failed to initialize UART TX Pin");
                return false;
            }

            if (!m_rx_pin.init(GpioAlternateFunctionConfig{
                    .mode  = GpioAlternateFunctionMode::kPushPull,
                    .speed = GpioSpeedMode::kLow,
                    .pull  = GpioPullMode::kPullUp,
                }))
            {
                VALLE_LOG_FATAL("Failed to initialize UART RX Pin");
                return false;
            }

            if constexpr (skHasCKPin)
            {
                if (!m_ck_pin.init(GpioAlternateFunctionConfig{
                        .mode  = GpioAlternateFunctionMode::kPushPull,
                        .speed = GpioSpeedMode::kHigh,
                        .pull  = GpioPullMode::kNoPull,
                    }))
                {
                    VALLE_LOG_FATAL("Failed to initialize UART CK Pin");
                    return false;
                }
            }

            if constexpr (skHasCTSPin)
            {
                if (!m_cts_pin.init(GpioAlternateFunctionConfig{
                        .mode  = GpioAlternateFunctionMode::kPushPull,
                        .speed = GpioSpeedMode::kLow,
                        .pull  = GpioPullMode::kPullUp,
                    }))
                {
                    VALLE_LOG_FATAL("Failed to initialize UART CTS Pin");
                    return false;
                }
            }

            if constexpr (skHasRTSPin)
            {
                if (!m_rts_pin.init(GpioAlternateFunctionConfig{
                        .mode  = GpioAlternateFunctionMode::kPushPull,
                        .speed = GpioSpeedMode::kMedium,
                        .pull  = GpioPullMode::kPullUp,
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
            m_huart.Init.OverSampling   = Uart_OVERSAMPLING_16;
            m_huart.Init.OneBitSampling = Uart_ONE_BIT_SAMPLE_DISABLE;
            m_huart.Init.ClockPrescaler = Uart_PRESCALER_DIV1;

            HAL_UART_Init(&m_huart);

            // Link DMA if present
            if constexpr (skHasDmaTx)
            {
                // We don't use the DMA device to init the DMA channel since HAL handles DMA differently
                // We only hold the dma channel device to lock it.

                m_hdma_tx.Instance                 = DmaChannelTxT::ChannelTraitsT::skInstance;
                m_hdma_tx.Init.Request             = static_cast<uint32_t>(ControllerTraitsT::skDmaMuxRequestTx);
                m_hdma_tx.Init.Direction           = Dma_MEMORY_TO_PERIPH;
                m_hdma_tx.Init.PeriphInc           = Dma_PINC_DISABLE;
                m_hdma_tx.Init.MemInc              = Dma_MINC_ENABLE;
                m_hdma_tx.Init.PeriphDataAlignment = Dma_PDATAALIGN_BYTE;
                m_hdma_tx.Init.MemDataAlignment    = Dma_MDATAALIGN_BYTE;
                m_hdma_tx.Init.Mode                = Dma_NORMAL;
                m_hdma_tx.Init.Priority            = static_cast<uint32_t>(config.dma_priority);

                HAL_DMA_Init(&m_hdma_tx);
                __HAL_LINKDMA(&m_huart, hdmatx, m_hdma_tx);

                // Enable UART Interrupt (required for HAL DMA mode to clean up after transfer)
                m_dma_tx->enable_interrupts(DmaChannelInterruptConfig{.priority   = config.dma_int_priority,
                                                                      .interrupts = {
                                                                          .transfer_complete = true,
                                                                          .half_transfer     = true,
                                                                          .transfer_error    = true,
                                                                      }});
            }

            HAL_NVIC_SetPriority(ControllerTraitsT::skIRQn, config.uart_int_priority, 0);
            HAL_NVIC_EnableIRQ(ControllerTraitsT::skIRQn);

            return true;
        }

        // --- API ---
        // NOLINTNEXTLINE(bugprone-easily-swapable-parameters)
        HAL_StatusTypeDef transmit(std::span<const std::byte> data, TimingContext::TimeoutMillisT timeout)
        {
            return HAL_UART_Transmit(&m_huart,
                                     reinterpret_cast<const uint8_t*>(data.data()),
                                     static_cast<uint16_t>(data.size()),
                                     timeout.count());
        }

        HAL_StatusTypeDef transmit_async(std::span<const std::byte> data)
        {
            if constexpr (skHasDmaTx)  // NOLINT(bugprone-branch-clone)
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

    // -----------------------------------------------------------------------------
    // DEVICE ALIASES
    // -----------------------------------------------------------------------------

    using USART1ControllerDevice  = UartControllerDevice<UartPeripheralId::kUSART1>;
    using USART2ControllerDevice  = UartControllerDevice<UartPeripheralId::kUSART2>;
    using USART3ControllerDevice  = UartControllerDevice<UartPeripheralId::kUSART3>;
    using Uart4ControllerDevice   = UartControllerDevice<UartPeripheralId::kUart4>;
    using Uart5ControllerDevice   = UartControllerDevice<UartPeripheralId::kUart5>;
    using LPUart1ControllerDevice = UartControllerDevice<UartPeripheralId::kLPUart1>;  // Usually the main USB UART

}  // namespace valle::platform

// Put these in app_isr_bindings.hpp to bind your instance
#define VALLE_BIND_UART_DMA_ISR_ROUTER(instance)                                                            \
    namespace valle::platform                                                                               \
    {                                                                                                       \
        static_assert(std::remove_cvref_t<decltype((instance))>::skHasDmaTx,                                \
                      "Instance does not have DMA TX capability");                                          \
        template <>                                                                                         \
        struct DmaGlobalIsrRouter<std::remove_cvref_t<decltype((instance))>::DmaChannelTxT::skPeripheralId, \
                                  std::remove_cvref_t<decltype((instance))>::DmaChannelTxT::skChannelId>    \
        {                                                                                                   \
            static void handle()                                                                            \
            {                                                                                               \
                (instance).handle_dma_irq();                                                                \
            }                                                                                               \
        };                                                                                                  \
    }

#define VALLE_BIND_UART_ISR_ROUTER(instance)                                                  \
    namespace valle::platform                                                                 \
    {                                                                                         \
        template <>                                                                           \
        struct UartGlobalIsrRouter<std::remove_cvref_t<decltype((instance))>::skPeripheralId> \
        {                                                                                     \
            static void handle()                                                              \
            {                                                                                 \
                (instance).handle_uart_irq();                                                 \
            }                                                                                 \
        };                                                                                    \
    }
