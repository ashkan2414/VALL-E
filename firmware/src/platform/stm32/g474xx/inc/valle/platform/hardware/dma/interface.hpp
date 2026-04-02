#pragma once

#include "stm32g4xx_ll_bus.h"
#include "stm32g4xx_ll_dma.h"
#include "valle/platform/hardware/dma/id.hpp"
#include "valle/platform/hardware/dmamux.hpp"

namespace valle::platform
{
    // ============================================================================
    // ENUMERATIONS
    // ============================================================================

    enum class DmaTransferDirection : uint32_t
    {
        kPeriphToMem = LL_DMA_DIRECTION_PERIPH_TO_MEMORY,
        kMemToPeriph = LL_DMA_DIRECTION_MEMORY_TO_PERIPH,
        kMemToMem    = LL_DMA_DIRECTION_MEMORY_TO_MEMORY
    };

    enum class DmaChannelPriority : uint32_t
    {
        kLow      = LL_DMA_PRIORITY_LOW,
        kMedium   = LL_DMA_PRIORITY_MEDIUM,
        kHigh     = LL_DMA_PRIORITY_HIGH,
        kVeryHigh = LL_DMA_PRIORITY_VERYHIGH
    };

    enum class DmaMode : uint32_t
    {
        kNormal   = LL_DMA_MODE_NORMAL,
        kCircular = LL_DMA_MODE_CIRCULAR
    };

    enum class DmaPeripheralDataWidth : uint32_t
    {
        kByte     = LL_DMA_PDATAALIGN_BYTE,
        kHalfWord = LL_DMA_PDATAALIGN_HALFWORD,
        kWord     = LL_DMA_PDATAALIGN_WORD
    };

    enum class DmaMemoryDataWidth : uint32_t
    {
        kByte     = LL_DMA_MDATAALIGN_BYTE,
        kHalfWord = LL_DMA_MDATAALIGN_HALFWORD,
        kWord     = LL_DMA_MDATAALIGN_WORD
    };

    enum class DmaPeripheralIncMode : uint32_t
    {
        kFixed     = LL_DMA_PERIPH_NOINCREMENT,
        kIncrement = LL_DMA_PERIPH_INCREMENT
    };

    enum class DmaMemoryIncMode : uint32_t
    {
        kFixed     = LL_DMA_MEMORY_NOINCREMENT,
        kIncrement = LL_DMA_MEMORY_INCREMENT
    };

    enum class DmaChannelInterruptSource : uint8_t
    {
        kTransferComplete,
        kHalfTransfer,
        kTransferError
    };

    // ============================================================================
    // ROOT TRAITS
    // ============================================================================
    struct DmaRootTraits
    {
        static constexpr std::array<uint32_t, 8> skChannelLLIdTable = {LL_DMA_CHANNEL_1,
                                                                       LL_DMA_CHANNEL_2,
                                                                       LL_DMA_CHANNEL_3,
                                                                       LL_DMA_CHANNEL_4,
                                                                       LL_DMA_CHANNEL_5,
                                                                       LL_DMA_CHANNEL_6,
                                                                       LL_DMA_CHANNEL_7,
                                                                       LL_DMA_CHANNEL_8};

        static constexpr std::array<DmaMuxChannelId, 8> skDma1MuxChannelIdTable = {DmaMuxChannelId::kChannel0,
                                                                                   DmaMuxChannelId::kChannel1,
                                                                                   DmaMuxChannelId::kChannel2,
                                                                                   DmaMuxChannelId::kChannel3,
                                                                                   DmaMuxChannelId::kChannel4,
                                                                                   DmaMuxChannelId::kChannel5,
                                                                                   DmaMuxChannelId::kChannel6,
                                                                                   DmaMuxChannelId::kChannel7};

        static constexpr std::array<DmaMuxChannelId, 8> skDma2MuxChannelIdTable = {DmaMuxChannelId::kChannel8,
                                                                                   DmaMuxChannelId::kChannel9,
                                                                                   DmaMuxChannelId::kChannel10,
                                                                                   DmaMuxChannelId::kChannel11,
                                                                                   DmaMuxChannelId::kChannel12,
                                                                                   DmaMuxChannelId::kChannel13,
                                                                                   DmaMuxChannelId::kChannel14,
                                                                                   DmaMuxChannelId::kChannel15};

        static constexpr std::array<uint32_t, 8> skDma1ChannelIrqnTable = {DMA1_Channel1_IRQn,
                                                                           DMA1_Channel2_IRQn,
                                                                           DMA1_Channel3_IRQn,
                                                                           DMA1_Channel4_IRQn,
                                                                           DMA1_Channel5_IRQn,
                                                                           DMA1_Channel6_IRQn,
                                                                           DMA1_Channel7_IRQn,
                                                                           DMA1_Channel8_IRQn};

        static constexpr std::array<uint32_t, 8> skDma2ChannelIrqnTable = {DMA2_Channel1_IRQn,
                                                                           DMA2_Channel2_IRQn,
                                                                           DMA2_Channel3_IRQn,
                                                                           DMA2_Channel4_IRQn,
                                                                           DMA2_Channel5_IRQn,
                                                                           DMA2_Channel6_IRQn,
                                                                           DMA2_Channel7_IRQn,
                                                                           DMA2_Channel8_IRQn};

        [[nodiscard]] static constexpr uint8_t controller_id_to_index(const DmaControllerId controller_id)
        {
            static_assert(static_cast<uint8_t>(DmaControllerId::kDma1) == 1,
                          "DmaControllerId enum values must start at 1");
            return static_cast<uint8_t>(controller_id) - 1;
        }

        [[nodiscard]] static constexpr uint8_t controller_id_to_number(const DmaControllerId controller_id)
        {
            static_assert(static_cast<uint8_t>(DmaControllerId::kDma1) == 1,
                          "DmaControllerId enum values must start at 1");
            return static_cast<uint8_t>(controller_id);
        }

        [[nodiscard]] static constexpr uint8_t channel_id_to_index(const DmaChannelId channel_id)
        {
            static_assert(static_cast<uint8_t>(DmaChannelId::kChannel1) == 1,
                          "DmaChannelId enum values must start at 1");
            return static_cast<uint8_t>(channel_id) - 1;
        }

        [[nodiscard]] static constexpr uint8_t channel_id_to_number(const DmaChannelId channel_id)
        {
            static_assert(static_cast<uint8_t>(DmaChannelId::kChannel1) == 1,
                          "DmaChannelId enum values must start at 1");
            return static_cast<uint8_t>(channel_id);
        }

        [[nodiscard]] static constexpr uint8_t channel_id_to_ll_id(const DmaChannelId channel_id)
        {
            return skChannelLLIdTable[channel_id_to_index(channel_id)];
        }

        [[nodiscard]] static constexpr DMA_Channel_TypeDef *channel_spec_to_instance(const DmaChannelSpec channel_spec)
        {
            if (channel_spec.controller_id == DmaControllerId::kDma1)
            {
                if (channel_spec.channel_id == DmaChannelId::kChannel1) return DMA1_Channel1;
                if (channel_spec.channel_id == DmaChannelId::kChannel2) return DMA1_Channel2;
                if (channel_spec.channel_id == DmaChannelId::kChannel3) return DMA1_Channel3;
                if (channel_spec.channel_id == DmaChannelId::kChannel4) return DMA1_Channel4;
                if (channel_spec.channel_id == DmaChannelId::kChannel5) return DMA1_Channel5;
                if (channel_spec.channel_id == DmaChannelId::kChannel6) return DMA1_Channel6;
                if (channel_spec.channel_id == DmaChannelId::kChannel7) return DMA1_Channel7;
                if (channel_spec.channel_id == DmaChannelId::kChannel8) return DMA1_Channel8;
            }
            else
            {
                if (channel_spec.channel_id == DmaChannelId::kChannel1) return DMA2_Channel1;
                if (channel_spec.channel_id == DmaChannelId::kChannel2) return DMA2_Channel2;
                if (channel_spec.channel_id == DmaChannelId::kChannel3) return DMA2_Channel3;
                if (channel_spec.channel_id == DmaChannelId::kChannel4) return DMA2_Channel4;
                if (channel_spec.channel_id == DmaChannelId::kChannel5) return DMA2_Channel5;
                if (channel_spec.channel_id == DmaChannelId::kChannel6) return DMA2_Channel6;
                if (channel_spec.channel_id == DmaChannelId::kChannel7) return DMA2_Channel7;
                if (channel_spec.channel_id == DmaChannelId::kChannel8) return DMA2_Channel8;
            }
            return nullptr;  // Invalid controller or channel ID
        }

        [[nodiscard]] static constexpr DmaMuxChannelId channel_spec_to_mux_channel_id(const DmaChannelSpec channel_spec)
        {
            if (channel_spec.controller_id == DmaControllerId::kDma1)
            {
                return skDma1MuxChannelIdTable[channel_id_to_index(channel_spec.channel_id)];
            }
            else
            {
                return skDma2MuxChannelIdTable[channel_id_to_index(channel_spec.channel_id)];
            }
        }

        [[nodiscard]] static constexpr uint32_t channel_spec_to_irqn(const DmaChannelSpec channel_spec)
        {
            if (channel_spec.controller_id == DmaControllerId::kDma1)
            {
                return skDma1ChannelIrqnTable[channel_id_to_index(channel_spec.channel_id)];
            }
            else
            {
                return skDma2ChannelIrqnTable[channel_id_to_index(channel_spec.channel_id)];
            }
        }
    };

    // ============================================================================
    // HARDWARE TRAITS
    // ============================================================================

    template <DmaControllerSpec tkControllerSpec>
    struct DmaControllerTraits;

    template <>
    struct DmaControllerTraits<kDma1ControllerSpec>
    {
        static constexpr DmaControllerSpec  skControllerSpec = kDma1ControllerSpec;
        static constexpr DmaMuxControllerId skMuxId          = DmaMuxControllerId::kDmaMux1;
        static inline DMA_TypeDef *const    skInstance       = DMA1;
    };

    template <>
    struct DmaControllerTraits<kDma2ControllerSpec>
    {
        static constexpr DmaControllerSpec  skControllerSpec = kDma2ControllerSpec;
        static constexpr DmaMuxControllerId skMuxId          = DmaMuxControllerId::kDmaMux1;
        static inline DMA_TypeDef *const    skInstance       = DMA2;
    };

    template <DmaChannelSpec tkChannelSpec>
    struct DmaChannelTraits
    {
        static constexpr DmaChannelSpec          skChannelSpec = tkChannelSpec;
        static inline DMA_Channel_TypeDef *const skInstance    = DmaRootTraits::channel_spec_to_instance(tkChannelSpec);
        static constexpr uint32_t        skChannelLLId  = DmaRootTraits::channel_id_to_ll_id(tkChannelSpec.channel_id);
        static constexpr DmaMuxChannelId skMuxChannelId = DmaRootTraits::channel_spec_to_mux_channel_id(tkChannelSpec);
        static constexpr IRQn_Type       skIRQn         = DmaRootTraits::channel_spec_to_irqn(tkChannelSpec);
    };

    // ============================================================================
    // INTERFACES
    // ============================================================================
    template <DmaControllerSpec tkControllerSpec>
    struct DmaControllerInterface
    {
        static constexpr DmaControllerSpec skControllerSpec = tkControllerSpec;
        using ControllerTraitsT                             = DmaControllerTraits<tkControllerSpec>;

        void enable_clock() const
        {
            if constexpr (tkControllerSpec.controller_id == DmaControllerId::kDma1)
            {
                LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);
            }
            else if constexpr (tkControllerSpec.controller_id == DmaControllerId::kDma2)
            {
                LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA2);
            }
            else
            {
                static_assert(kAlwaysFalseV<tkControllerSpec>, "Invalid DMA Controller ID");
            }
        }
    };

    struct DmaChannelTransferConfig
    {
        uint32_t value = 0U;

        constexpr DmaChannelTransferConfig(const DmaTransferDirection   direction,
                                           const DmaChannelPriority     priority,
                                           const DmaMode                mode,
                                           const DmaPeripheralDataWidth periph_data_width,
                                           const DmaMemoryDataWidth     memory_data_width,
                                           const DmaPeripheralIncMode   inc_periph,
                                           const DmaMemoryIncMode       inc_memory)
            : value(static_cast<uint32_t>(direction) | static_cast<uint32_t>(priority) | static_cast<uint32_t>(mode) |
                    static_cast<uint32_t>(periph_data_width) | static_cast<uint32_t>(memory_data_width) |
                    static_cast<uint32_t>(inc_periph) | static_cast<uint32_t>(inc_memory))
        {
        }
    };

    template <DmaChannelSpec tkChannelSpec>
    struct DmaChannelInterface
    {
        static constexpr DmaChannelSpec skChannelSpec = tkChannelSpec;
        using ControllerTraitsT                       = DmaControllerTraits<tkChannelSpec.controller_spec()>;
        using ChannelTraitsT                          = DmaChannelTraits<tkChannelSpec>;

        void enable() const
        {
            LL_DMA_EnableChannel(ControllerTraitsT::skInstance, ChannelTraitsT::skChannelLLId);
        }

        void disable() const
        {
            LL_DMA_DisableChannel(ControllerTraitsT::skInstance, ChannelTraitsT::skChannelLLId);
        }

        [[nodiscard]] bool is_enabled() const
        {
            return static_cast<bool>(
                LL_DMA_IsEnabledChannel(ControllerTraitsT::skInstance, ChannelTraitsT::skChannelLLId) != 0U);
        }

        void config_transfer(const DmaChannelTransferConfig config) const
        {
            LL_DMA_ConfigTransfer(ControllerTraitsT::skInstance, ChannelTraitsT::skChannelLLId, config.value);
        }

        void set_data_transfer_direction(const DmaTransferDirection direction) const
        {
            LL_DMA_SetDataTransferDirection(
                ControllerTraitsT::skInstance, ChannelTraitsT::skChannelLLId, static_cast<uint32_t>(direction));
        }

        [[nodiscard]] DmaTransferDirection get_data_transfer_direction() const
        {
            return static_cast<DmaTransferDirection>(
                LL_DMA_GetDataTransferDirection(ControllerTraitsT::skInstance, ChannelTraitsT::skChannelLLId));
        }

        void set_mode(const DmaMode mode) const
        {
            LL_DMA_SetMode(ControllerTraitsT::skInstance, ChannelTraitsT::skChannelLLId, static_cast<uint32_t>(mode));
        }

        [[nodiscard]] DmaMode get_mode() const
        {
            return static_cast<DmaMode>(LL_DMA_GetMode(ControllerTraitsT::skInstance, ChannelTraitsT::skChannelLLId));
        }

        void set_periph_inc_mode(const DmaPeripheralIncMode mode) const
        {
            LL_DMA_SetPeriphIncMode(
                ControllerTraitsT::skInstance, ChannelTraitsT::skChannelLLId, static_cast<uint32_t>(mode));
        }

        [[nodiscard]] DmaPeripheralIncMode get_periph_inc_mode() const
        {
            return static_cast<DmaPeripheralIncMode>(
                LL_DMA_GetPeriphIncMode(ControllerTraitsT::skInstance, ChannelTraitsT::skChannelLLId));
        }

        void set_memory_inc_mode(const DmaMemoryIncMode mode) const
        {
            LL_DMA_SetMemoryIncMode(
                ControllerTraitsT::skInstance, ChannelTraitsT::skChannelLLId, static_cast<uint32_t>(mode));
        }

        [[nodiscard]] DmaMemoryIncMode get_memory_inc_mode() const
        {
            return static_cast<DmaMemoryIncMode>(
                LL_DMA_GetMemoryIncMode(ControllerTraitsT::skInstance, ChannelTraitsT::skChannelLLId));
        }

        void set_periph_size(const DmaPeripheralDataWidth size) const
        {
            LL_DMA_SetPeriphSize(
                ControllerTraitsT::skInstance, ChannelTraitsT::skChannelLLId, static_cast<uint32_t>(size));
        }

        [[nodiscard]] DmaPeripheralDataWidth get_periph_size() const
        {
            return static_cast<DmaPeripheralDataWidth>(
                LL_DMA_GetPeriphSize(ControllerTraitsT::skInstance, ChannelTraitsT::skChannelLLId));
        }

        void set_memory_size(const DmaMemoryDataWidth size) const
        {
            LL_DMA_SetMemorySize(
                ControllerTraitsT::skInstance, ChannelTraitsT::skChannelLLId, static_cast<uint32_t>(size));
        }

        [[nodiscard]] DmaMemoryDataWidth get_memory_size() const
        {
            return static_cast<DmaMemoryDataWidth>(
                LL_DMA_GetMemorySize(ControllerTraitsT::skInstance, ChannelTraitsT::skChannelLLId));
        }

        void set_channel_priority_level(const DmaChannelPriority priority) const
        {
            LL_DMA_SetChannelPriorityLevel(
                ControllerTraitsT::skInstance, ChannelTraitsT::skChannelLLId, static_cast<uint32_t>(priority));
        }

        [[nodiscard]] DmaChannelPriority get_channel_priority_level() const
        {
            return static_cast<DmaChannelPriority>(
                LL_DMA_GetChannelPriorityLevel(ControllerTraitsT::skInstance, ChannelTraitsT::skChannelLLId));
        }

        void set_data_length(const uint32_t length) const
        {
            LL_DMA_SetDataLength(ControllerTraitsT::skInstance, ChannelTraitsT::skChannelLLId, length);
        }

        [[nodiscard]] uint32_t get_data_length() const
        {
            return LL_DMA_GetDataLength(ControllerTraitsT::skInstance, ChannelTraitsT::skChannelLLId);
        }

        void config_addresses(const uint32_t             src_address,
                              const uint32_t             dst_address,
                              const DmaTransferDirection direction) const
        {
            LL_DMA_ConfigAddresses(ControllerTraitsT::skInstance,
                                   ChannelTraitsT::skChannelLLId,
                                   src_address,
                                   dst_address,
                                   static_cast<uint32_t>(direction));
        }

        void set_memory_address(const uint32_t memory_address) const
        {
            LL_DMA_SetMemoryAddress(ControllerTraitsT::skInstance, ChannelTraitsT::skChannelLLId, memory_address);
        }

        void set_periph_address(const uint32_t periph_address) const
        {
            LL_DMA_SetPeriphAddress(ControllerTraitsT::skInstance, ChannelTraitsT::skChannelLLId, periph_address);
        }

        [[nodiscard]] uint32_t get_memory_address() const
        {
            return static_cast<uint32_t>(
                LL_DMA_GetMemoryAddress(ControllerTraitsT::skInstance, ChannelTraitsT::skChannelLLId));
        }

        [[nodiscard]] uint32_t get_periph_address() const
        {
            return static_cast<uint32_t>(
                LL_DMA_GetPeriphAddress(ControllerTraitsT::skInstance, ChannelTraitsT::skChannelLLId));
        }

        void set_m2m_src_address(const uint32_t memory_address) const
        {
            LL_DMA_SetM2MSrcAddress(ControllerTraitsT::skInstance, ChannelTraitsT::skChannelLLId, memory_address);
        }

        void set_m2m_dst_address(const uint32_t memory_address) const
        {
            LL_DMA_SetM2MDstAddress(ControllerTraitsT::skInstance, ChannelTraitsT::skChannelLLId, memory_address);
        }

        [[nodiscard]] uint32_t get_m2m_src_address() const
        {
            return static_cast<uint32_t>(
                LL_DMA_GetM2MSrcAddress(ControllerTraitsT::skInstance, ChannelTraitsT::skChannelLLId));
        }

        [[nodiscard]] uint32_t get_m2m_dst_address() const
        {
            return static_cast<uint32_t>(
                LL_DMA_GetM2MDstAddress(ControllerTraitsT::skInstance, ChannelTraitsT::skChannelLLId));
        }

        // -----------------------------------------------------------------------------
        // Flag Management
        // -----------------------------------------------------------------------------

        void clear_global_interrupt_flag() const
        {
            if constexpr (tkChannelSpec.channel_id == DmaChannelId::kChannel1)
            {
                LL_DMA_ClearFlag_GI1(ControllerTraitsT::skInstance);
            }
            else if constexpr (tkChannelSpec.channel_id == DmaChannelId::kChannel2)
            {
                LL_DMA_ClearFlag_GI2(ControllerTraitsT::skInstance);
            }
            else if constexpr (tkChannelSpec.channel_id == DmaChannelId::kChannel3)
            {
                LL_DMA_ClearFlag_GI3(ControllerTraitsT::skInstance);
            }
            else if constexpr (tkChannelSpec.channel_id == DmaChannelId::kChannel4)
            {
                LL_DMA_ClearFlag_GI4(ControllerTraitsT::skInstance);
            }
            else if constexpr (tkChannelSpec.channel_id == DmaChannelId::kChannel5)
            {
                LL_DMA_ClearFlag_GI5(ControllerTraitsT::skInstance);
            }
            else if constexpr (tkChannelSpec.channel_id == DmaChannelId::kChannel6)
            {
                LL_DMA_ClearFlag_GI6(ControllerTraitsT::skInstance);
            }
            else if constexpr (tkChannelSpec.channel_id == DmaChannelId::kChannel7)
            {
                LL_DMA_ClearFlag_GI7(ControllerTraitsT::skInstance);
            }
            else if constexpr (tkChannelSpec.channel_id == DmaChannelId::kChannel8)
            {
                LL_DMA_ClearFlag_GI8(ControllerTraitsT::skInstance);
            }
            else
            {
                static_assert(kAlwaysFalseV<tkChannelSpec.channel_id>, "Invalid DMA Channel ID");
            }
        }

        [[nodiscard]] bool is_global_interrupt_flag_active() const
        {
            if constexpr (tkChannelSpec.channel_id == DmaChannelId::kChannel1)
            {
                return static_cast<bool>(LL_DMA_IsActiveFlag_GI1(ControllerTraitsT::skInstance));
            }
            else if constexpr (tkChannelSpec.channel_id == DmaChannelId::kChannel2)
            {
                return static_cast<bool>(LL_DMA_IsActiveFlag_GI2(ControllerTraitsT::skInstance));
            }
            else if constexpr (tkChannelSpec.channel_id == DmaChannelId::kChannel3)
            {
                return static_cast<bool>(LL_DMA_IsActiveFlag_GI3(ControllerTraitsT::skInstance));
            }
            else if constexpr (tkChannelSpec.channel_id == DmaChannelId::kChannel4)
            {
                return static_cast<bool>(LL_DMA_IsActiveFlag_GI4(ControllerTraitsT::skInstance));
            }
            else if constexpr (tkChannelSpec.channel_id == DmaChannelId::kChannel5)
            {
                return static_cast<bool>(LL_DMA_IsActiveFlag_GI5(ControllerTraitsT::skInstance));
            }
            else if constexpr (tkChannelSpec.channel_id == DmaChannelId::kChannel6)
            {
                return static_cast<bool>(LL_DMA_IsActiveFlag_GI6(ControllerTraitsT::skInstance));
            }
            else if constexpr (tkChannelSpec.channel_id == DmaChannelId::kChannel7)
            {
                return static_cast<bool>(LL_DMA_IsActiveFlag_GI7(ControllerTraitsT::skInstance));
            }
            else if constexpr (tkChannelSpec.channel_id == DmaChannelId::kChannel8)
            {
                return static_cast<bool>(LL_DMA_IsActiveFlag_GI8(ControllerTraitsT::skInstance));
            }
            else
            {
                static_assert(kAlwaysFalseV<tkChannelSpec.channel_id>, "Invalid DMA Channel ID");
            }
        }

        void enable_transfer_complete_interrupt() const
        {
            LL_DMA_EnableIT_TC(ControllerTraitsT::skInstance, ChannelTraitsT::skChannelLLId);
        }

        void disable_transfer_complete_interrupt() const
        {
            LL_DMA_DisableIT_TC(ControllerTraitsT::skInstance, ChannelTraitsT::skChannelLLId);
        }

        void clear_transfer_complete_flag() const
        {
            if constexpr (tkChannelSpec.channel_id == DmaChannelId::kChannel1)
            {
                LL_DMA_ClearFlag_TC1(ControllerTraitsT::skInstance);
            }
            else if constexpr (tkChannelSpec.channel_id == DmaChannelId::kChannel2)
            {
                LL_DMA_ClearFlag_TC2(ControllerTraitsT::skInstance);
            }
            else if constexpr (tkChannelSpec.channel_id == DmaChannelId::kChannel3)
            {
                LL_DMA_ClearFlag_TC3(ControllerTraitsT::skInstance);
            }
            else if constexpr (tkChannelSpec.channel_id == DmaChannelId::kChannel4)
            {
                LL_DMA_ClearFlag_TC4(ControllerTraitsT::skInstance);
            }
            else if constexpr (tkChannelSpec.channel_id == DmaChannelId::kChannel5)
            {
                LL_DMA_ClearFlag_TC5(ControllerTraitsT::skInstance);
            }
            else if constexpr (tkChannelSpec.channel_id == DmaChannelId::kChannel6)
            {
                LL_DMA_ClearFlag_TC6(ControllerTraitsT::skInstance);
            }
            else if constexpr (tkChannelSpec.channel_id == DmaChannelId::kChannel7)
            {
                LL_DMA_ClearFlag_TC7(ControllerTraitsT::skInstance);
            }
            else if constexpr (tkChannelSpec.channel_id == DmaChannelId::kChannel8)
            {
                LL_DMA_ClearFlag_TC8(ControllerTraitsT::skInstance);
            }
            else
            {
                static_assert(kAlwaysFalseV<tkChannelSpec.channel_id>, "Invalid DMA Channel ID");
            }
        }

        [[nodiscard]] bool is_transfer_complete_flag_active() const
        {
            if constexpr (tkChannelSpec.channel_id == DmaChannelId::kChannel1)
            {
                return static_cast<bool>(LL_DMA_IsActiveFlag_TC1(ControllerTraitsT::skInstance));
            }
            else if constexpr (tkChannelSpec.channel_id == DmaChannelId::kChannel2)
            {
                return static_cast<bool>(LL_DMA_IsActiveFlag_TC2(ControllerTraitsT::skInstance));
            }
            else if constexpr (tkChannelSpec.channel_id == DmaChannelId::kChannel3)
            {
                return static_cast<bool>(LL_DMA_IsActiveFlag_TC3(ControllerTraitsT::skInstance));
            }
            else if constexpr (tkChannelSpec.channel_id == DmaChannelId::kChannel4)
            {
                return static_cast<bool>(LL_DMA_IsActiveFlag_TC4(ControllerTraitsT::skInstance));
            }
            else if constexpr (tkChannelSpec.channel_id == DmaChannelId::kChannel5)
            {
                return static_cast<bool>(LL_DMA_IsActiveFlag_TC5(ControllerTraitsT::skInstance));
            }
            else if constexpr (tkChannelSpec.channel_id == DmaChannelId::kChannel6)
            {
                return static_cast<bool>(LL_DMA_IsActiveFlag_TC6(ControllerTraitsT::skInstance));
            }
            else if constexpr (tkChannelSpec.channel_id == DmaChannelId::kChannel7)
            {
                return static_cast<bool>(LL_DMA_IsActiveFlag_TC7(ControllerTraitsT::skInstance));
            }
            else if constexpr (tkChannelSpec.channel_id == DmaChannelId::kChannel8)
            {
                return static_cast<bool>(LL_DMA_IsActiveFlag_TC8(ControllerTraitsT::skInstance));
            }
            else
            {
                static_assert(kAlwaysFalseV<tkChannelSpec.channel_id>, "Invalid DMA Channel ID");
            }
        }

        void enable_half_transfer_interrupt() const
        {
            LL_DMA_EnableIT_HT(ControllerTraitsT::skInstance, ChannelTraitsT::skChannelLLId);
        }

        void disable_half_transfer_interrupt() const
        {
            LL_DMA_DisableIT_HT(ControllerTraitsT::skInstance, ChannelTraitsT::skChannelLLId);
        }

        void clear_half_transfer_flag() const
        {
            if constexpr (tkChannelSpec.channel_id == DmaChannelId::kChannel1)
            {
                LL_DMA_ClearFlag_HT1(ControllerTraitsT::skInstance);
            }
            else if constexpr (tkChannelSpec.channel_id == DmaChannelId::kChannel2)
            {
                LL_DMA_ClearFlag_HT2(ControllerTraitsT::skInstance);
            }
            else if constexpr (tkChannelSpec.channel_id == DmaChannelId::kChannel3)
            {
                LL_DMA_ClearFlag_HT3(ControllerTraitsT::skInstance);
            }
            else if constexpr (tkChannelSpec.channel_id == DmaChannelId::kChannel4)
            {
                LL_DMA_ClearFlag_HT4(ControllerTraitsT::skInstance);
            }
            else if constexpr (tkChannelSpec.channel_id == DmaChannelId::kChannel5)
            {
                LL_DMA_ClearFlag_HT5(ControllerTraitsT::skInstance);
            }
            else if constexpr (tkChannelSpec.channel_id == DmaChannelId::kChannel6)
            {
                LL_DMA_ClearFlag_HT6(ControllerTraitsT::skInstance);
            }
            else if constexpr (tkChannelSpec.channel_id == DmaChannelId::kChannel7)
            {
                LL_DMA_ClearFlag_HT7(ControllerTraitsT::skInstance);
            }
            else if constexpr (tkChannelSpec.channel_id == DmaChannelId::kChannel8)
            {
                LL_DMA_ClearFlag_HT8(ControllerTraitsT::skInstance);
            }
            else
            {
                static_assert(kAlwaysFalseV<tkChannelSpec.channel_id>, "Invalid DMA Channel ID");
            }
        }

        [[nodiscard]] bool is_half_transfer_flag_active() const
        {
            if constexpr (tkChannelSpec.channel_id == DmaChannelId::kChannel1)
            {
                return static_cast<bool>(LL_DMA_IsActiveFlag_HT1(ControllerTraitsT::skInstance));
            }
            else if constexpr (tkChannelSpec.channel_id == DmaChannelId::kChannel2)
            {
                return static_cast<bool>(LL_DMA_IsActiveFlag_HT2(ControllerTraitsT::skInstance));
            }
            else if constexpr (tkChannelSpec.channel_id == DmaChannelId::kChannel3)
            {
                return static_cast<bool>(LL_DMA_IsActiveFlag_HT3(ControllerTraitsT::skInstance));
            }
            else if constexpr (tkChannelSpec.channel_id == DmaChannelId::kChannel4)
            {
                return static_cast<bool>(LL_DMA_IsActiveFlag_HT4(ControllerTraitsT::skInstance));
            }
            else if constexpr (tkChannelSpec.channel_id == DmaChannelId::kChannel5)
            {
                return static_cast<bool>(LL_DMA_IsActiveFlag_HT5(ControllerTraitsT::skInstance));
            }
            else if constexpr (tkChannelSpec.channel_id == DmaChannelId::kChannel6)
            {
                return static_cast<bool>(LL_DMA_IsActiveFlag_HT6(ControllerTraitsT::skInstance));
            }
            else if constexpr (tkChannelSpec.channel_id == DmaChannelId::kChannel7)
            {
                return static_cast<bool>(LL_DMA_IsActiveFlag_HT7(ControllerTraitsT::skInstance));
            }
            else if constexpr (tkChannelSpec.channel_id == DmaChannelId::kChannel8)
            {
                return static_cast<bool>(LL_DMA_IsActiveFlag_HT8(ControllerTraitsT::skInstance));
            }
            else
            {
                static_assert(kAlwaysFalseV<tkChannelSpec.channel_id>, "Invalid DMA Channel ID");
            }
        }

        void enable_transfer_error_interrupt() const
        {
            LL_DMA_EnableIT_TE(ControllerTraitsT::skInstance, ChannelTraitsT::skChannelLLId);
        }

        void disable_transfer_error_interrupt() const
        {
            LL_DMA_DisableIT_TE(ControllerTraitsT::skInstance, ChannelTraitsT::skChannelLLId);
        }

        void clear_transfer_error_flag() const
        {
            if constexpr (tkChannelSpec.channel_id == DmaChannelId::kChannel1)
            {
                LL_DMA_ClearFlag_TE1(ControllerTraitsT::skInstance);
            }
            else if constexpr (tkChannelSpec.channel_id == DmaChannelId::kChannel2)
            {
                LL_DMA_ClearFlag_TE2(ControllerTraitsT::skInstance);
            }
            else if constexpr (tkChannelSpec.channel_id == DmaChannelId::kChannel3)
            {
                LL_DMA_ClearFlag_TE3(ControllerTraitsT::skInstance);
            }
            else if constexpr (tkChannelSpec.channel_id == DmaChannelId::kChannel4)
            {
                LL_DMA_ClearFlag_TE4(ControllerTraitsT::skInstance);
            }
            else if constexpr (tkChannelSpec.channel_id == DmaChannelId::kChannel5)
            {
                LL_DMA_ClearFlag_TE5(ControllerTraitsT::skInstance);
            }
            else if constexpr (tkChannelSpec.channel_id == DmaChannelId::kChannel6)
            {
                LL_DMA_ClearFlag_TE6(ControllerTraitsT::skInstance);
            }
            else if constexpr (tkChannelSpec.channel_id == DmaChannelId::kChannel7)
            {
                LL_DMA_ClearFlag_TE7(ControllerTraitsT::skInstance);
            }
            else if constexpr (tkChannelSpec.channel_id == DmaChannelId::kChannel8)
            {
                LL_DMA_ClearFlag_TE8(ControllerTraitsT::skInstance);
            }
            else
            {
                static_assert(kAlwaysFalseV<tkChannelSpec.channel_id>, "Invalid DMA Channel ID");
            }
        }

        [[nodiscard]] bool is_transfer_error_flag_active() const
        {
            if constexpr (tkChannelSpec.channel_id == DmaChannelId::kChannel1)
            {
                return static_cast<bool>(LL_DMA_IsActiveFlag_TE1(ControllerTraitsT::skInstance));
            }
            else if constexpr (tkChannelSpec.channel_id == DmaChannelId::kChannel2)
            {
                return static_cast<bool>(LL_DMA_IsActiveFlag_TE2(ControllerTraitsT::skInstance));
            }
            else if constexpr (tkChannelSpec.channel_id == DmaChannelId::kChannel3)
            {
                return static_cast<bool>(LL_DMA_IsActiveFlag_TE3(ControllerTraitsT::skInstance));
            }
            else if constexpr (tkChannelSpec.channel_id == DmaChannelId::kChannel4)
            {
                return static_cast<bool>(LL_DMA_IsActiveFlag_TE4(ControllerTraitsT::skInstance));
            }
            else if constexpr (tkChannelSpec.channel_id == DmaChannelId::kChannel5)
            {
                return static_cast<bool>(LL_DMA_IsActiveFlag_TE5(ControllerTraitsT::skInstance));
            }
            else if constexpr (tkChannelSpec.channel_id == DmaChannelId::kChannel6)
            {
                return static_cast<bool>(LL_DMA_IsActiveFlag_TE6(ControllerTraitsT::skInstance));
            }
            else if constexpr (tkChannelSpec.channel_id == DmaChannelId::kChannel7)
            {
                return static_cast<bool>(LL_DMA_IsActiveFlag_TE7(ControllerTraitsT::skInstance));
            }
            else if constexpr (tkChannelSpec.channel_id == DmaChannelId::kChannel8)
            {
                return static_cast<bool>(LL_DMA_IsActiveFlag_TE8(ControllerTraitsT::skInstance));
            }
            else
            {
                static_assert(kAlwaysFalseV<tkChannelSpec.channel_id>, "Invalid DMA Channel ID");
            }
        }

        void enable_interrupt(const DmaInterruptSource interrupt_type) const
        {
            switch (interrupt_type)
            {
                case DmaInterruptSource::kTransferComplete:
                    enable_transfer_complete_interrupt();
                    break;
                case DmaInterruptSource::kHalfTransfer:
                    enable_half_transfer_interrupt();
                    break;
                case DmaInterruptSource::kTransferError:
                    enable_transfer_error_interrupt();
                    break;
                default:
                    expect(false, "Invalid DMA interrupt type");
                    break;
            }
        }

        void disable_interrupt(const DmaInterruptSource interrupt_type) const
        {
            switch (interrupt_type)
            {
                case DmaInterruptSource::kTransferComplete:
                    disable_transfer_complete_interrupt();
                    break;
                case DmaInterruptSource::kHalfTransfer:
                    disable_half_transfer_interrupt();
                    break;
                case DmaInterruptSource::kTransferError:
                    disable_transfer_error_interrupt();
                    break;
                default:
                    expect(false, "Invalid DMA interrupt type");
                    break;
            }
        }

        void clear_interrupt_flag(const DmaInterruptSource interrupt_type) const
        {
            switch (interrupt_type)
            {
                case DmaInterruptSource::kTransferComplete:
                    clear_transfer_complete_flag();
                    break;
                case DmaInterruptSource::kHalfTransfer:
                    clear_half_transfer_flag();
                    break;
                case DmaInterruptSource::kTransferError:
                    clear_transfer_error_flag();
                    break;
                default:
                    expect(false, "Invalid DMA interrupt type");
                    break;
            }
        }

        [[nodiscard]] bool is_interrupt_flag_active(const DmaInterruptSource interrupt_type) const
        {
            switch (interrupt_type)
            {
                case DmaInterruptSource::kTransferComplete:
                    return is_transfer_complete_flag_active();
                case DmaInterruptSource::kHalfTransfer:
                    return is_half_transfer_flag_active();
                case DmaInterruptSource::kTransferError:
                    return is_transfer_error_flag_active();
                default:
                    expect(false, "Invalid DMA interrupt type");
                    return false;
            }
        }
    };

}  // namespace valle::platform