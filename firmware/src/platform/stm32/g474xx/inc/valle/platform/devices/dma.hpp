#pragma once

#include "stm32g4xx_ll_bus.h"
#include "valle/platform/core.hpp"
#include "valle/platform/devices/dmamux.hpp"
#include "valle/platform/hdi/dma.hpp"

namespace valle::platform
{
    // ============================================================================
    // DMA CONTROLLER (SHARED)
    // ============================================================================
    template <DmaControllerId tkControllerId>
    class DmaController
    {
    public:
        struct Descriptor : public SharedDeviceDescriptor
        {
        };

        static constexpr DmaControllerId skControllerId = tkControllerId;

        using ControllerTraitsT = DmaControllerTraits<tkControllerId>;

        using ControllerHdiT = DmaControllerHdi<tkControllerId>;
        using MuxControllerT = DmaMuxController<ControllerTraitsT::skMuxId>;

        using DependDevices = TypeList<MuxControllerT>;
        using InjectDevices = TypeList<ControllerHdiT>;

    private:
        [[no_unique_address]] DeviceRef<ControllerHdiT> m_controller_hw;

    public:
        DmaController() = delete;

        explicit DmaController(DeviceRef<ControllerHdiT>&& hardware_key) : m_controller_hw(std::move(hardware_key))
        {
        }

        [[nodiscard]] bool init()
        {
            // Enable Controller Clock
            m_controller_hw->enable_clock();
            return true;
        }
    };

    using Dma1Controller = DmaController<DmaControllerId::kDma1>;
    using Dma2Controller = DmaController<DmaControllerId::kDma2>;

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
        DmaTransferDirection direction         = DmaTransferDirection::kPeriphToMem;
        DmaChannelPriority   priority          = DmaChannelPriority::kLow;
        DmaMode              mode              = DmaMode::kNormal;
        DmaDataWidth         periph_data_width = DmaDataWidth::kByte;
        DmaDataWidth         memory_data_width = DmaDataWidth::kByte;

        // Increment settings
        bool inc_periph = false;  // Typically FALSE for ADC/SPI
        bool inc_memory = true;   // Typically TRUE for Buffers

        // DmaMUX Request ID (The "Wire" connection)
        DmaMuxRequestId request_id = DmaMuxRequestId::kMem2Mem;
    };

    // ----------------------------------------------------------------------------
    // DEVICE CLASS
    // ----------------------------------------------------------------------------

    template <DmaChannelSpec tkChannelSpec>
    class DmaChannel
    {
    public:
        struct Descriptor : public UniqueDeviceDescriptor
        {
        };

        static constexpr DmaChannelSpec    skChannelSpec    = tkChannelSpec;
        static constexpr DmaControllerSpec skControllerSpec = tkChannelSpec.controller_spec();

        using ControllerT          = DmaController<skControllerSpec>;
        using ControllerTraitsT    = DmaControllerTraits<skControllerSpec>;
        using ChannelTraitsT       = DmaChannelTraits<skChannelSpec>;
        using InterruptControllerT = DmaChannelInterruptController<skChannelSpec>;

        using ChannelHdiT = DmaChannelHdi<skChannelSpec>;
        using MuxChannelT = DmaMuxChannel<ControllerTraitsT::skMuxSpec>;

        using DependDevices = TypeList<ControllerT>;
        using InjectDevices = TypeList<ChannelHdiT, MuxChannelT>;

    private:
        [[no_unique_address]] DeviceRef<ChannelHdiT> m_channel_hw;
        [[no_unique_address]] DeviceRef<MuxChannelT> m_dmamux;

    public:
        explicit DmaChannel(DeviceRef<ChannelHdiT>&& m_channel_hw, DeviceRef<MuxChannelT>&& dmamux)
            : m_channel_hw(std::move(m_channel_hw)), m_dmamux(std::move(dmamux))
        {
        }

        /**
         * @brief Initialize the DMA Channel and route the DmaMUX.
         */
        [[nodiscard]] bool init(const DmaChannelConfig& config)
        {
            const DmaChannelTransferConfig transfer_config(
                config.direction,
                config.priority,
                config.mode,
                map_data_width<true>(config.periph_data_width),
                map_data_width<false>(config.memory_data_width),
                config.inc_periph ? DmaPeripheralIncMode::kIncrement : DmaPeripheralIncMode::kFixed,
                config.inc_memory ? DmaMemoryIncMode::kIncrement : DmaMemoryIncMode::kFixed);

            // Configure Transfer Parameters
            m_channel_hw->config_transfer(transfer_config);

            // Configure Routing (DmaMUX)
            // This is what connects "Adc1" to "Dma1 Channel 1"
            m_dmamux->route_request(config.request_id);
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

            const DmaTransferDirection direction = m_channel_hw->get_data_transfer_direction();
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

            const DmaTransferDirection direction = m_channel_hw->get_data_transfer_direction();
            expect(direction == DmaTransferDirection::kPeriphToMem, "DMA not configured for PeriphToMem");

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

            const DmaTransferDirection direction = m_channel_hw->get_data_transfer_direction();
            expect(direction == DmaTransferDirection::kMemToPeriph, "DMA not configured for MemToPeriph");

            start_impl(mem_addr, periph_addr, direction, length);
        }

        /**
         * @brief Start DMA transfer from memory to memory (clearer API).
         * @param src_mem_addr Source memory address
         * @param dst_mem_addr Destination memory address
         * @param length Number of data items
         */
        static void start_mem_to_mem(const uint32_t src_mem_addr, const uint32_t dst_mem_addr, const uint32_t length)
        {
            expect(src_mem_addr != 0, "Source memory address must not be null");
            expect(dst_mem_addr != 0, "Destination memory address must not be null");

            const DmaTransferDirection direction = m_channel_hw->get_data_transfer_direction();
            expect(direction == DmaTransferDirection::kMemToMem, "DMA not configured for MemToMem");

            start_impl(src_mem_addr, dst_mem_addr, direction, length);
        }

        static void stop()
        {
            m_channel_hw->disable();

            // Wait for disable to complete (typically 1-2 AHB cycles)
            while (m_channel_hw->is_enabled());

            m_channel_hw->clear_global_interrupt_flag();
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
        void start_impl(const uint32_t             src_addr,
                        const uint32_t             dst_addr,
                        const DmaTransferDirection direction,
                        const uint32_t             length)
        {
            stop();
            m_controller_hw->config_addresses(src_addr, dst_addr, static_cast<uint32_t>(direction));
            m_channel_hw->set_data_length(length);
            m_channel_hw->enable();
        }
    };

    template <typename T>
    concept CDmaChannel = std::is_base_of_v<DmaChannel<T::skControllerId, T::skChannelId>, T>;

    // ----------------------------------------------------------------------------
    // DEVICE ALIASES
    // ----------------------------------------------------------------------------

    using Dma1Channel1 = DmaChannel<DmaControllerId::kDma1, DmaChannelId::kChannel1>;
    using Dma1Channel2 = DmaChannel<DmaControllerId::kDma1, DmaChannelId::kChannel2>;
    using Dma1Channel3 = DmaChannel<DmaControllerId::kDma1, DmaChannelId::kChannel3>;
    using Dma1Channel4 = DmaChannel<DmaControllerId::kDma1, DmaChannelId::kChannel4>;
    using Dma1Channel5 = DmaChannel<DmaControllerId::kDma1, DmaChannelId::kChannel5>;
    using Dma1Channel6 = DmaChannel<DmaControllerId::kDma1, DmaChannelId::kChannel6>;
    using Dma1Channel7 = DmaChannel<DmaControllerId::kDma1, DmaChannelId::kChannel7>;
    using Dma1Channel8 = DmaChannel<DmaControllerId::kDma1, DmaChannelId::kChannel8>;

    using Dma2Channel1 = DmaChannel<DmaControllerId::kDma2, DmaChannelId::kChannel1>;
    using Dma2Channel2 = DmaChannel<DmaControllerId::kDma2, DmaChannelId::kChannel2>;
    using Dma2Channel3 = DmaChannel<DmaControllerId::kDma2, DmaChannelId::kChannel3>;
    using Dma2Channel4 = DmaChannel<DmaControllerId::kDma2, DmaChannelId::kChannel4>;
    using Dma2Channel5 = DmaChannel<DmaControllerId::kDma2, DmaChannelId::kChannel5>;
    using Dma2Channel6 = DmaChannel<DmaControllerId::kDma2, DmaChannelId::kChannel6>;
    using Dma2Channel7 = DmaChannel<DmaControllerId::kDma2, DmaChannelId::kChannel7>;
    using Dma2Channel8 = DmaChannel<DmaControllerId::kDma2, DmaChannelId::kChannel8>;

}  // namespace valle::platform
