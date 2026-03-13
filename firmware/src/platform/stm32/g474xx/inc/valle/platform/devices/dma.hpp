#pragma once

#include "stm32g4xx_ll_bus.h"
#include "valle/platform/core.hpp"
#include "valle/platform/devices/dmamux.hpp"
#include "valle/platform/hardware/dma.hpp"


namespace valle::platform
{

    // ============================================================================
    // FORWARD DECLARATIONS
    // ============================================================================

    class DMARootDevice;

    template <DMAControllerID tkControllerID>
        requires(kValidDMAControllerID<tkControllerID>)
    class DMAControllerDevice;

    template <DMAControllerID tkControllerID, DMAChannelID tkChannelID>
        requires(kValidDMAControllerID<tkControllerID> && kValidDMAChannel<tkChannelID>)
    class DMAChannelDevice;

    // ============================================================================
    // DMA ROOT DEVICE (INTERFACE)
    // ============================================================================
    class DMARootDevice
    {
    public:
        struct Descriptor : public InterfaceDeviceDescriptor
        {
            using Children = DeviceTreeList<DMAControllerDevice<1>, DMAControllerDevice<2>>;
        };
    };

    // ============================================================================
    // DMA CONTROLLER (SHARED)
    // ============================================================================
    template <DMAControllerID tkControllerID>
        requires(kValidDMAControllerID<tkControllerID>)
    class DMAControllerDevice
    {
    public:
        struct Descriptor : public SharedDeviceDescriptor
        {
            // Declares ownership of all 8 channels for this controller
            using Children = DeviceTreeList<DMAChannelDevice<tkControllerID, 1>,
                                            DMAChannelDevice<tkControllerID, 2>,
                                            DMAChannelDevice<tkControllerID, 3>,
                                            DMAChannelDevice<tkControllerID, 4>,
                                            DMAChannelDevice<tkControllerID, 5>,
                                            DMAChannelDevice<tkControllerID, 6>,
                                            DMAChannelDevice<tkControllerID, 7>,
                                            DMAChannelDevice<tkControllerID, 8>>;
        };

        static constexpr DMAControllerID skControllerID = tkControllerID;

        using ControllerTraitsT    = DMAControllerTraits<tkControllerID>;
        using MuxControllerDeviceT = DMAMuxControllerDevice<ControllerTraitsT::skMuxControllerID>;
        using DependDevices        = TypeList<DMARootDevice, MuxControllerDeviceT>;

        static inline bool init()
        {
            // Enable Controller Clock
            ControllerTraitsT::enable_clock();
            return true;
        }

        static inline bool post_init()
        {
            // Additional post-initialization if needed
            return true;
        }
    };

    using DMA1ControllerDevice = DMAControllerDevice<1>;
    using DMA2ControllerDevice = DMAControllerDevice<2>;

    // ============================================================================
    // DMA CHANNEL (UNIQUE)
    // ============================================================================

    // ----------------------------------------------------------------------------
    // CONFIGURATIONS
    // ----------------------------------------------------------------------------

    /**
     * @brief Data Width for DMA Transfers.
     */
    enum class DMADataWidth : uint32_t
    {
        kByte,
        kHalfWord,
        kWord,
    };

    /**
     * @brief Configuration for a specific DMA Channel Transfer.
     */
    struct DMAChannelConfig
    {
        DMADirection direction         = DMADirection::kPeriphToMem;
        DMAPriority  priority          = DMAPriority::kLow;
        DMAMode      mode              = DMAMode::kNormal;
        DMADataWidth periph_data_width = DMADataWidth::kByte;
        DMADataWidth memory_data_width = DMADataWidth::kByte;

        // Increment settings
        bool inc_periph = false;  // Typically FALSE for ADC/SPI
        bool inc_memory = true;   // Typically TRUE for Buffers

        // DMAMUX Request ID (The "Wire" connection)
        DMAMuxRequestID request_id = DMAMuxRequestID::kMem2Mem;
    };

    // ----------------------------------------------------------------------------
    // DEVICE CLASS
    // ----------------------------------------------------------------------------

    template <DMAControllerID tkControllerID, DMAChannelID tkChannelID>
        requires(kValidDMAControllerID<tkControllerID> && kValidDMAChannel<tkChannelID>)
    class DMAChannelDevice
    {
    public:
        struct Descriptor : public UniqueDeviceDescriptor
        {
        };

        static constexpr DMAControllerID skControllerID = tkControllerID;
        static constexpr DMAChannelID    skChannelID    = tkChannelID;

        using ControllerT          = DMAControllerDevice<skControllerID>;
        using ControllerTraitsT    = DMAControllerTraits<skControllerID>;
        using ChannelTraitsT       = DMAChannelTraits<skControllerID, skChannelID>;
        using MuxControllerDeviceT = typename ControllerT::MuxControllerDeviceT;
        using InterruptControllerT = DMAChannelInterruptController<skControllerID, skChannelID>;

        using DependDevices = TypeList<ControllerT>;
        using InjectDevices = TypeList<MuxControllerDeviceT>;

    private:
        [[no_unique_address]] DeviceRef<MuxControllerDeviceT> m_dmamux;

    public:
        explicit DMAChannelDevice(DeviceRef<MuxControllerDeviceT>&& dmamux) : m_dmamux(std::move(dmamux))
        {
        }

        /**
         * @brief Initialize the DMA Channel and route the DMAMUX.
         */
        [[nodiscard]] bool init(const DMAChannelConfig& config)
        {
            // Configure Transfer Parameters
            LL_DMA_ConfigTransfer(
                ControllerTraitsT::skInstance,
                ChannelTraitsT::skChannelLLID,
                static_cast<uint32_t>(config.direction) | static_cast<uint32_t>(config.priority) |
                    static_cast<uint32_t>(config.mode) |
                    static_cast<uint32_t>(map_data_width<true>(config.periph_data_width)) |   // Peripheral Width
                    static_cast<uint32_t>(map_data_width<false>(config.memory_data_width)) |  // Memory Width
                    (config.inc_periph ? LL_DMA_PERIPH_INCREMENT : LL_DMA_PERIPH_NOINCREMENT) |
                    (config.inc_memory ? LL_DMA_MEMORY_INCREMENT : LL_DMA_MEMORY_NOINCREMENT));

            // Configure Routing (DMAMUX)
            // This is what connects "ADC1" to "DMA1 Channel 1"
            m_dmamux->route_request(ChannelTraitsT::skMuxChannelID, config.request_id);
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
                LL_DMA_GetDataTransferDirection(ControllerTraitsT::skInstance, ChannelTraitsT::skChannelLLID);

            // LL_DMA_ConfigAddresses always takes (PeriphAddr, MemAddr, Direction)
            if (direction == LL_DMA_DIRECTION_MEMORY_TO_PERIPH)
            {
                // src_addr is Memory, dst_addr is Peripheral
                start_impl(dst_addr, src_addr, direction, length);
            }
            else
            {
                // src_addr is Peripheral, dst_addr is Memory (or MemToMem)
                start_impl(src_addr, dst_addr, direction, length);
            }
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
                LL_DMA_GetDataTransferDirection(ControllerTraitsT::skInstance, ChannelTraitsT::skChannelLLID);
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
                LL_DMA_GetDataTransferDirection(ControllerTraitsT::skInstance, ChannelTraitsT::skChannelLLID);
            expect(direction == LL_DMA_DIRECTION_MEMORY_TO_PERIPH, "DMA not configured for MemToPeriph");

            start_impl(periph_addr, mem_addr, direction, length);
        }

        static void stop()
        {
            LL_DMA_DisableChannel(ControllerTraitsT::skInstance, ChannelTraitsT::skChannelLLID);

            // Wait for disable to complete (typically 1-2 AHB cycles)
            while (LL_DMA_IsEnabledChannel(ControllerTraitsT::skInstance, ChannelTraitsT::skChannelLLID));

            // Now safe to clear all flags
            if constexpr (tkChannelID == 1)
            {
                LL_DMA_ClearFlag_GI1(ControllerTraitsT::skInstance);
            }
            else if constexpr (tkChannelID == 2)
            {
                LL_DMA_ClearFlag_GI2(ControllerTraitsT::skInstance);
            }
            else if constexpr (tkChannelID == 3)
            {
                LL_DMA_ClearFlag_GI3(ControllerTraitsT::skInstance);
            }
            else if constexpr (tkChannelID == 4)
            {
                LL_DMA_ClearFlag_GI4(ControllerTraitsT::skInstance);
            }
            else if constexpr (tkChannelID == 5)
            {
                LL_DMA_ClearFlag_GI5(ControllerTraitsT::skInstance);
            }
            else if constexpr (tkChannelID == 6)
            {
                LL_DMA_ClearFlag_GI6(ControllerTraitsT::skInstance);
            }
            else if constexpr (tkChannelID == 7)
            {
                LL_DMA_ClearFlag_GI7(ControllerTraitsT::skInstance);
            }
            else if constexpr (tkChannelID == 8)
            {
                LL_DMA_ClearFlag_GI8(ControllerTraitsT::skInstance);
            }
            else
            {
                static_assert(false, "Invalid DMA Channel ID");
            }
        }

        static void reconfigure(const DMAChannelConfig& config)
        {
            // TODO: Handle reconfiguration while transfer is ongoing?
            init(config);
        }

        /**
         * @brief Enable interrupts for this channel.
         * @param config Configuration for DMA Interrupts.
         */
        static void enable_interrupts(const DMAChannelInterruptConfig& config)
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
         * @brief Map generic DMADataWidth to Peripheral or Memory specific width.
         *
         * @tparam tkForPeripheral true to map for peripheral, false for memory
         * @param width Generic DMADataWidth
         * @return Mapped width enum
         */
        template <bool tkForPeripheral>
        static auto map_data_width(const DMADataWidth width)
        {
            if constexpr (tkForPeripheral)
            {
                switch (width)
                {
                    case DMADataWidth::kByte:
                        return DMAPeripheralDataWidth::kByte;
                    case DMADataWidth::kHalfWord:
                        return DMAPeripheralDataWidth::kHalfWord;
                    case DMADataWidth::kWord:
                        return DMAPeripheralDataWidth::kWord;
                    default:
                        panic("Invalid DMA Data Width");
                        return DMAPeripheralDataWidth::kByte;
                }
            }
            else
            {
                switch (width)
                {
                    case DMADataWidth::kByte:
                        return DMAMemoryDataWidth::kByte;
                    case DMADataWidth::kHalfWord:
                        return DMAMemoryDataWidth::kHalfWord;
                    case DMADataWidth::kWord:
                        return DMAMemoryDataWidth::kWord;
                    default:
                        panic("Invalid DMA Data Width");
                        return DMAMemoryDataWidth::kByte;
                }
            }
        }

        /**
         * @brief Internal start implementation.
         *
         * @param periph_addr peripheral address
         * @param mem_addr memory address
         * @param direction DMA direction
         * @param length number of data items
         */
        static void start_impl(const uint32_t periph_addr,
                               const uint32_t mem_addr,
                               const uint32_t direction,
                               const uint32_t length)
        {
            LL_DMA_ConfigAddresses(
                ControllerTraitsT::skInstance, ChannelTraitsT::skChannelLLID, periph_addr, mem_addr, direction);
            LL_DMA_SetDataLength(ControllerTraitsT::skInstance, ChannelTraitsT::skChannelLLID, length);
            LL_DMA_EnableChannel(ControllerTraitsT::skInstance, ChannelTraitsT::skChannelLLID);
        }
    };

    template <typename T>
    concept CDMAChannelDevice = std::is_base_of_v<DMAChannelDevice<T::skControllerID, T::skChannelID>, T>;

    class DMANullChannelDevice
    {
    public:
        struct Descriptor : public NullDeviceDescriptor
        {
        };
    };

    template <typename T>
    concept CNullDMAChannel = std::is_same_v<T, DMANullChannelDevice>;

    // ----------------------------------------------------------------------------
    // DEVICE ALIASES
    // ----------------------------------------------------------------------------

    using DMA1Channel1Device = DMAChannelDevice<1, 1>;
    using DMA1Channel2Device = DMAChannelDevice<1, 2>;
    using DMA1Channel3Device = DMAChannelDevice<1, 3>;
    using DMA1Channel4Device = DMAChannelDevice<1, 4>;
    using DMA1Channel5Device = DMAChannelDevice<1, 5>;
    using DMA1Channel6Device = DMAChannelDevice<1, 6>;
    using DMA1Channel7Device = DMAChannelDevice<1, 7>;
    using DMA1Channel8Device = DMAChannelDevice<1, 8>;

    using DMA2Channel1Device = DMAChannelDevice<2, 1>;
    using DMA2Channel2Device = DMAChannelDevice<2, 2>;
    using DMA2Channel3Device = DMAChannelDevice<2, 3>;
    using DMA2Channel4Device = DMAChannelDevice<2, 4>;
    using DMA2Channel5Device = DMAChannelDevice<2, 5>;
    using DMA2Channel6Device = DMAChannelDevice<2, 6>;
    using DMA2Channel7Device = DMAChannelDevice<2, 7>;
    using DMA2Channel8Device = DMAChannelDevice<2, 8>;

}  // namespace valle::platform