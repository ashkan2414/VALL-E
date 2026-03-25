#pragma once

#include "stm32g4xx_ll_bus.h"
#include "valle/platform/core.hpp"
#include "valle/platform/devices/dmamux.hpp"
#include "valle/platform/hardware/dma.hpp"

namespace valle::platform
{
    // ============================================================================
    // DMA CONTROLLER (SHARED)
    // ============================================================================
    template <DmaPeripheralId tkPeripheralId>
    class DmaControllerDevice
    {
    public:
        struct Descriptor : public SharedDeviceDescriptor
        {
        };

        static constexpr DmaPeripheralId skPeripheralId = tkPeripheralId;

        using ControllerTraitsT    = DmaControllerTraits<tkPeripheralId>;
        using MuxControllerDeviceT = DmaMuxControllerDevice<ControllerTraitsT::skMuxId>;
        using DependDevices        = TypeList<MuxControllerDeviceT>;

        static inline bool init()
        {
            // Enable Controller Clock
            ControllerTraitsT::enable_clock();
            return true;
        }
    };

    using Dma1ControllerDevice = DmaControllerDevice<DmaPeripheralId::kDma1>;
    using Dma2ControllerDevice = DmaControllerDevice<DmaPeripheralId::kDma2>;

    // ============================================================================
    // DMA CHANNEL (UNIQUE)
    // ============================================================================

    // ----------------------------------------------------------------------------
    // CONFIGURATIONS
    // ----------------------------------------------------------------------------

    /**
     * @brief Data Width for DMA Transfers.
     */
    enum class DmaDataWidth : uint32_t
    {
        kByte,
        kHalfWord,
        kWord,
    };

    /**
     * @brief Configuration for a specific DMA Channel Transfer.
     */
    struct DmaChannelConfig
    {
        DmaDirection direction         = DmaDirection::kPeriphToMem;
        DmaPriority  priority          = DmaPriority::kLow;
        DmaMode      mode              = DmaMode::kNormal;
        DmaDataWidth periph_data_width = DmaDataWidth::kByte;
        DmaDataWidth memory_data_width = DmaDataWidth::kByte;

        // Increment settings
        bool inc_periph = false;  // Typically FALSE for ADC/SPI
        bool inc_memory = true;   // Typically TRUE for Buffers

        // DmaMUX Request ID (The "Wire" connection)
        DmaMuxRequestId request_id = DmaMuxRequestId::kMem2Mem;
    };

    // ----------------------------------------------------------------------------
    // DEVICE CLASS
    // ----------------------------------------------------------------------------

    template <DmaPeripheralId tkPeripheralId, DmaChannelId tkChannelId>
    class DmaChannelDevice
    {
    public:
        struct Descriptor : public UniqueDeviceDescriptor
        {
        };

        static constexpr DmaPeripheralId skPeripheralId = tkPeripheralId;
        static constexpr DmaChannelId    skChannelId    = tkChannelId;

        using ControllerT          = DmaControllerDevice<skPeripheralId>;
        using ControllerTraitsT    = DmaControllerTraits<skPeripheralId>;
        using ChannelTraitsT       = DmaChannelTraits<skPeripheralId, skChannelId>;
        using MuxControllerDeviceT = typename ControllerT::MuxControllerDeviceT;
        using InterruptControllerT = DmaChannelInterruptController<skPeripheralId, skChannelId>;

        using DependDevices = TypeList<ControllerT>;
        using InjectDevices = TypeList<MuxControllerDeviceT>;

    private:
        [[no_unique_address]] DeviceRef<MuxControllerDeviceT> m_dmamux;

    public:
        explicit DmaChannelDevice(DeviceRef<MuxControllerDeviceT>&& dmamux) : m_dmamux(std::move(dmamux))
        {
        }

        /**
         * @brief Initialize the DMA Channel and route the DmaMUX.
         */
        [[nodiscard]] bool init(const DmaChannelConfig& config)
        {
            // Configure Transfer Parameters
            LL_DMA_ConfigTransfer(
                ControllerTraitsT::skInstance,
                ChannelTraitsT::skChannelLLId,
                static_cast<uint32_t>(config.direction) | static_cast<uint32_t>(config.priority) |
                    static_cast<uint32_t>(config.mode) |
                    static_cast<uint32_t>(map_data_width<true>(config.periph_data_width)) |   // Peripheral Width
                    static_cast<uint32_t>(map_data_width<false>(config.memory_data_width)) |  // Memory Width
                    (config.inc_periph ? LL_DMA_PERIPH_INCREMENT : LL_DMA_PERIPH_NOINCREMENT) |
                    (config.inc_memory ? LL_DMA_MEMORY_INCREMENT : LL_DMA_MEMORY_NOINCREMENT));

            // Configure Routing (DmaMUX)
            // This is what connects "Adc1" to "Dma1 Channel 1"
            m_dmamux->route_request(ChannelTraitsT::skMuxChannelId, config.request_id);
            return true;
        }

        /**
         * @brief Configure memory addresses and start the transfer.
         * @param src_addr Source Address (Peripheral or Memory)
         * @param dst_addr Destination Address (Peripheral or Memory)
         * @param length   Number of data items (not bytes, but items based on data_width)
         * @warning Parameter interpretation depends on configured direction:
         *          - PeriphToMem: src_addr=Peripheral, dst_addr=Memory
         *          - MemToPeriph: src_addr=Memory, dst_addr=Peripheral
         *
         * @deprecated Consider using start_periph_to_mem() or start_mem_to_periph() for clarity.
         */
        static void start(const uint32_t src_addr, const uint32_t dst_addr, const uint32_t length)
        {
            expect(src_addr != 0, "Source address must not be null");
            expect(dst_addr != 0, "Destination address must not be null");

            const uint32_t direction =
                LL_DMA_GetDataTransferDirection(ControllerTraitsT::skInstance, ChannelTraitsT::skChannelLLId);
            start_impl(src_addr, dst_addr, direction, length);
        }

        /**
         * @brief Start DMA transfer from peripheral to memory (clearer API).
         * @param periph_addr Peripheral register address
         * @param mem_addr Memory buffer address
         * @param length Number of data items
         */
        static void start_periph_to_mem(const uint32_t periph_addr, const uint32_t mem_addr, const uint32_t length)
        {
            expect(periph_addr != 0, "Peripheral address must not be null");
            expect(mem_addr != 0, "Memory address must not be null");

            const uint32_t direction =
                LL_DMA_GetDataTransferDirection(ControllerTraitsT::skInstance, ChannelTraitsT::skChannelLLId);
            expect(direction == LL_DMA_DIRECTION_PERIPH_TO_MEMORY, "DMA not configured for PeriphToMem");

            start_impl(periph_addr, mem_addr, direction, length);
        }

        /**
         * @brief Start DMA transfer from memory to peripheral (clearer API).
         * @param mem_addr Memory buffer address
         * @param periph_addr Peripheral register address
         * @param length Number of data items
         */
        static void start_mem_to_periph(const uint32_t mem_addr, const uint32_t periph_addr, const uint32_t length)
        {
            expect(mem_addr != 0, "Memory address must not be null");
            expect(periph_addr != 0, "Peripheral address must not be null");

            const uint32_t direction =
                LL_DMA_GetDataTransferDirection(ControllerTraitsT::skInstance, ChannelTraitsT::skChannelLLId);
            expect(direction == LL_DMA_DIRECTION_MEMORY_TO_PERIPH, "DMA not configured for MemToPeriph");

            start_impl(mem_addr, periph_addr, direction, length);
        }

        static void stop()
        {
            LL_DMA_DisableChannel(ControllerTraitsT::skInstance, ChannelTraitsT::skChannelLLId);

            // Wait for disable to complete (typically 1-2 AHB cycles)
            while (LL_DMA_IsEnabledChannel(ControllerTraitsT::skInstance, ChannelTraitsT::skChannelLLId));

            // Now safe to clear all flags
            if constexpr (tkChannelId == DmaChannelId::kChannel1)
            {
                LL_DMA_ClearFlag_GI1(ControllerTraitsT::skInstance);
            }
            else if constexpr (tkChannelId == DmaChannelId::kChannel2)
            {
                LL_DMA_ClearFlag_GI2(ControllerTraitsT::skInstance);
            }
            else if constexpr (tkChannelId == DmaChannelId::kChannel3)
            {
                LL_DMA_ClearFlag_GI3(ControllerTraitsT::skInstance);
            }
            else if constexpr (tkChannelId == DmaChannelId::kChannel4)
            {
                LL_DMA_ClearFlag_GI4(ControllerTraitsT::skInstance);
            }
            else if constexpr (tkChannelId == DmaChannelId::kChannel5)
            {
                LL_DMA_ClearFlag_GI5(ControllerTraitsT::skInstance);
            }
            else if constexpr (tkChannelId == DmaChannelId::kChannel6)
            {
                LL_DMA_ClearFlag_GI6(ControllerTraitsT::skInstance);
            }
            else if constexpr (tkChannelId == DmaChannelId::kChannel7)
            {
                LL_DMA_ClearFlag_GI7(ControllerTraitsT::skInstance);
            }
            else if constexpr (tkChannelId == DmaChannelId::kChannel8)
            {
                LL_DMA_ClearFlag_GI8(ControllerTraitsT::skInstance);
            }
            else
            {
                static_assert(false, "Invalid DMA Channel ID");
            }
        }

        static void reconfigure(const DmaChannelConfig& config)
        {
            // TODO: Handle reconfiguration while transfer is ongoing?
            init(config);
        }

        /**
         * @brief Enable interrupts for this channel.
         * @param config Configuration for DMA Interrupts.
         */
        static void enable_interrupts(const DmaChannelInterruptConfig& config)
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

    private:
        /**
         * @brief Map generic DmaDataWidth to Peripheral or Memory specific width.
         *
         * @tparam tkForPeripheral true to map for peripheral, false for memory
         * @param width Generic DmaDataWidth
         * @return Mapped width enum
         */
        template <bool tkForPeripheral>
        static auto map_data_width(const DmaDataWidth width)
        {
            if constexpr (tkForPeripheral)
            {
                switch (width)
                {
                    case DmaDataWidth::kByte:
                        return DmaPeripheralDataWidth::kByte;
                    case DmaDataWidth::kHalfWord:
                        return DmaPeripheralDataWidth::kHalfWord;
                    case DmaDataWidth::kWord:
                        return DmaPeripheralDataWidth::kWord;
                    default:
                        panic("Invalid DMA Data Width");
                        return DmaPeripheralDataWidth::kByte;
                }
            }
            else
            {
                switch (width)
                {
                    case DmaDataWidth::kByte:
                        return DmaMemoryDataWidth::kByte;
                    case DmaDataWidth::kHalfWord:
                        return DmaMemoryDataWidth::kHalfWord;
                    case DmaDataWidth::kWord:
                        return DmaMemoryDataWidth::kWord;
                    default:
                        panic("Invalid DMA Data Width");
                        return DmaMemoryDataWidth::kByte;
                }
            }
        }

        /**
         * @brief Internal start implementation.
         *
         * @param src_addr Source Address (Peripheral or Memory)
         * @param dst_addr Destination Address (Peripheral or Memory)
         * @param direction DMA direction
         * @param length number of data items
         */
        static void start_impl(const uint32_t src_addr,
                               const uint32_t dst_addr,
                               const uint32_t direction,
                               const uint32_t length)
        {
            stop();
            LL_DMA_ConfigAddresses(
                ControllerTraitsT::skInstance, ChannelTraitsT::skChannelLLId, src_addr, dst_addr, direction);
            LL_DMA_SetDataLength(ControllerTraitsT::skInstance, ChannelTraitsT::skChannelLLId, length);
            LL_DMA_EnableChannel(ControllerTraitsT::skInstance, ChannelTraitsT::skChannelLLId);
        }
    };

    template <typename T>
    concept CDmaChannelDevice = std::is_base_of_v<DmaChannelDevice<T::skPeripheralId, T::skChannelId>, T>;

    class DmaNullChannelDevice
    {
    public:
        struct Descriptor : public NullDeviceDescriptor
        {
        };
    };

    template <typename T>
    concept CNullDmaChannel = std::is_same_v<T, DmaNullChannelDevice>;

    // ----------------------------------------------------------------------------
    // DEVICE ALIASES
    // ----------------------------------------------------------------------------

    using Dma1Channel1Device = DmaChannelDevice<DmaPeripheralId::kDma1, DmaChannelId::kChannel1>;
    using Dma1Channel2Device = DmaChannelDevice<DmaPeripheralId::kDma1, DmaChannelId::kChannel2>;
    using Dma1Channel3Device = DmaChannelDevice<DmaPeripheralId::kDma1, DmaChannelId::kChannel3>;
    using Dma1Channel4Device = DmaChannelDevice<DmaPeripheralId::kDma1, DmaChannelId::kChannel4>;
    using Dma1Channel5Device = DmaChannelDevice<DmaPeripheralId::kDma1, DmaChannelId::kChannel5>;
    using Dma1Channel6Device = DmaChannelDevice<DmaPeripheralId::kDma1, DmaChannelId::kChannel6>;
    using Dma1Channel7Device = DmaChannelDevice<DmaPeripheralId::kDma1, DmaChannelId::kChannel7>;
    using Dma1Channel8Device = DmaChannelDevice<DmaPeripheralId::kDma1, DmaChannelId::kChannel8>;

    using Dma2Channel1Device = DmaChannelDevice<DmaPeripheralId::kDma2, DmaChannelId::kChannel1>;
    using Dma2Channel2Device = DmaChannelDevice<DmaPeripheralId::kDma2, DmaChannelId::kChannel2>;
    using Dma2Channel3Device = DmaChannelDevice<DmaPeripheralId::kDma2, DmaChannelId::kChannel3>;
    using Dma2Channel4Device = DmaChannelDevice<DmaPeripheralId::kDma2, DmaChannelId::kChannel4>;
    using Dma2Channel5Device = DmaChannelDevice<DmaPeripheralId::kDma2, DmaChannelId::kChannel5>;
    using Dma2Channel6Device = DmaChannelDevice<DmaPeripheralId::kDma2, DmaChannelId::kChannel6>;
    using Dma2Channel7Device = DmaChannelDevice<DmaPeripheralId::kDma2, DmaChannelId::kChannel7>;
    using Dma2Channel8Device = DmaChannelDevice<DmaPeripheralId::kDma2, DmaChannelId::kChannel8>;

}  // namespace valle::platform
