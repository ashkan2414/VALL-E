#pragma once

#include "valle/platform/hardware/dma/interface.hpp"

namespace valle::platform
{
    // ============================================================================
    // INTERRUPT TRAITS
    // ============================================================================

    template <DmaChannelSpec tkChannelSpec, DmaChannelInterruptSource tkIntSource>
    struct DmaChannelInterruptSourceInterface
    {
    private:
        static constexpr DmaChannelInterface<tkChannelSpec> skInterface{};

    public:
        static constexpr bool skShouldClear = true;

        static void enable()
        {
            skInterface.enable_interrupt(tkIntSource);
        }

        static void disable()
        {
            skInterface.disable_interrupt(tkIntSource);
        }

        static void clear()
        {
            skInterface.clear_interrupt_flag(tkIntSource);
        }

        static bool is_enabled()
        {
            return skInterface.is_interrupt_enabled(tkIntSource);
        }

        static bool is_flag_active()
        {
            return skInterface.is_interrupt_flag_active(tkIntSource);
        }

        static bool is_pending()
        {
            return is_flag_active() && is_enabled();
        }
    };

    // ===========================================================================
    // INTERRUPT CONTROLLER
    // ===========================================================================
    struct DmaChannelInterruptMask
    {
        union
        {
            uint32_t mask;  // Full mask (for easy initialization)
            struct
            {
                bool transfer_complete : 1 = false;  // Transfer Complete Interrupt Enable
                bool half_transfer : 1     = false;  // Half Transfer Interrupt Enable
                bool transfer_error : 1    = false;  // Transfer Error Interrupt Enable
            };
        };
    };

    /**
     * @brief Configuration for DMA Interrupts.
     */
    struct DmaChannelInterruptConfig
    {
        uint32_t                priority = 5;  // NVIC Priority (0 = Highest, 15 = Lowest)
        DmaChannelInterruptMask interrupts{};  // Which interrupts to enable
    };

    template <DmaChannelSpec tkChannelSpec>
    struct DmaChannelInterruptController
    {
    public:
        static constexpr DmaChannelSpec skChannelSpec = tkChannelSpec;
        using ChannelTraitsT                          = DmaChannelTraits<skChannelSpec>;

        /**
         * @brief Enable interrupts for this channel.
         * @param config Configuration for DMA Interrupts.
         */
        static void enable_interrupts(const DmaChannelInterruptConfig& config)
        {
            if (config.interrupts.mask == 0)
            {
                return;  // No interrupts to enable
            }

            if (config.interrupts.transfer_complete)
            {
                DmaChannelInterruptSourceInterface<tkChannelSpec,
                                                   DmaChannelInterruptSource::kTransferComplete>::clear();
                DmaChannelInterruptSourceInterface<tkChannelSpec,
                                                   DmaChannelInterruptSource::kTransferComplete>::enable();
            }
            if (config.interrupts.half_transfer)
            {
                DmaChannelInterruptSourceInterface<tkChannelSpec, DmaChannelInterruptSource::kHalfTransfer>::clear();
                DmaChannelInterruptSourceInterface<tkChannelSpec, DmaChannelInterruptSource::kHalfTransfer>::enable();
            }

            if (config.interrupts.transfer_error)
            {
                DmaChannelInterruptSourceInterface<tkChannelSpec, DmaChannelInterruptSource::kTransferError>::clear();
                DmaChannelInterruptSourceInterface<tkChannelSpec, DmaChannelInterruptSource::kTransferError>::enable();
            }

            NVIC_SetPriority(ChannelTraitsT::skIRQn, config.priority);
            NVIC_EnableIRQ(ChannelTraitsT::skIRQn);
        }

        /**
         * @brief Disable interrupts for this channel.
         */
        static void disable_interrupts()
        {
            DmaChannelInterruptSourceInterface<tkChannelSpec, DmaChannelInterruptSource::kTransferComplete>::disable();
            DmaChannelInterruptSourceInterface<tkChannelSpec, DmaChannelInterruptSource::kHalfTransfer>::disable();
            DmaChannelInterruptSourceInterface<tkChannelSpec, DmaChannelInterruptSource::kTransferError>::disable();
            NVIC_DisableIRQ(ChannelTraitsT::skIRQn);
        }
    };

    // ===========================================================================
    // INTERRUPT ROUTERS
    // ===========================================================================

    // ---------------------------------------------------------------------------
    // GLOBAL ISR ROUTER
    // ---------------------------------------------------------------------------

    /**
     * @brief Global ISR Router for a specific DMA.
     * Specializing this allows you to handle the entire ISR in one function
     * (e.g., when delegating to the ST HAL).
     *
     * @tparam tkControllerId DMA Controller ID.
     * @tparam tkChannelId    DMA Channel ID.
     */
    template <DmaChannelSpec tkChannelSpec>
    struct DmaIrqRouter
    {
        using UnboundIsrHandlerTag = void;

        static void handle()
        {
        }
    };

    // ---------------------------------------------------------------------------
    // GRANULAR ISR ROUTER
    // ---------------------------------------------------------------------------

    /**
     * @brief DMA ISR Router
     *
     * Specialize this template to handle specific DMA interrupts for a given
     * controller and channel.
     *
     * @tparam tkControllerId DMA Controller ID (1 or 2)
     * @tparam tkChannelId    DMA Channel ID (1-8)
     * @tparam tkIntSource      DMA Interrupt Type
     */
    template <DmaChannelSpec tkChannelSpec, DmaChannelInterruptSource tkIntSource>
    struct DmaIsrRouter
    {
        using UnboundIsrHandlerTag = void;
        static void handle()
        {
            // Do nothing by default (optimized away)
        }
    };

    using DmaChannelInterruptIrqRouterContext = InterruptIrqRouterContext<DmaChannelSpec,
                                                                          DmaChannelInterruptSource,
                                                                          DmaChannelInterruptSourceInterface,
                                                                          DmaIrqRouter,
                                                                          DmaIsrRouter>;

}  // namespace valle::platform
