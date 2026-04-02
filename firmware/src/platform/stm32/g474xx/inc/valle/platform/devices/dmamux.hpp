#pragma once

#include "stm32g4xx_ll_bus.h"
#include "valle/platform/core.hpp"
#include "valle/platform/hdi/dmamux.hpp"

namespace valle::platform
{
    // ============================================================================
    // DmaMux CONTROLLER (SHARED)
    // ============================================================================
    template <DmaMuxControllerId tkControllerId>
    class DmaMuxController
    {
    public:
        struct Descriptor : public SharedDeviceDescriptor
        {
        };

        static constexpr DmaMuxControllerId skControllerId = tkControllerId;

        using ControllerTraitsT = DmaMuxControllerTraits<tkControllerId>;

        using ControllerHdiT = DmaMuxControllerHdi<tkControllerId>;

    private:
        [[no_unique_address]] DeviceRef<ControllerHdiT> m_controller_hw;

    public:
        DmaMuxController() = delete;

        explicit DmaMuxController(DeviceRef<ControllerHdiT>&& hardware_key) : m_controller_hw(std::move(hardware_key))
        {
        }

        [[nodiscard]] bool init()
        {
            m_controller_hw->enable_clock();
            return true;
        }
    };

    // ----------------------------------------------------------------------------
    // DEVICE ALIASES
    // ----------------------------------------------------------------------------

    using DmaMux1Controller = DmaMuxController<DmaMuxControllerId::kDmaMux1>;

    // ============================================================================
    // CHANNEL DEVICE (UNIQUE)
    // ============================================================================

    template <DmaMuxControllerId tkControllerId, DmaMuxChannelId tkChannelId>
    class DmaMuxChannel
    {
    public:
        static constexpr DmaMuxControllerId skControllerId = tkControllerId;
        static constexpr DmaMuxChannelId    skChannelId    = tkChannelId;

        using ControllerTraitsT = DmaMuxControllerTraits<skControllerId>;
        using ChannelTraitsT    = DmaMuxChannelTraits<skChannelId>;

        using ControllerT = DmaMuxController<skControllerId>;
        using HdiT        = DmaMuxChannelHdi<skControllerId, skChannelId>;

        using DependDevices = TypeList<ControllerT>;
        using InjectDevices = TypeList<HdiT>;

    private:
        [[no_unique_address]] DeviceRef<HdiT> m_channel_hw;

    public:
        explicit DmaMuxChannel(DeviceRef<HdiT>&& channel_hw) : m_channel_hw(std::move(channel_hw))
        {
        }

        [[nodiscard]] bool init()
        {
            // No initialization needed for DmaMux Channels as of now.
            return true;
        }

        void route_request(const DmaMuxRequestId request_id)
        {
            m_controller_hw->set_request_id(request_id);
        }
    };

    // ----------------------------------------------------------------------------
    // DEVICE ALIASES
    // ----------------------------------------------------------------------------

    using DmaMux1Channel0  = DmaMuxChannel<DmaMuxControllerId::kDmaMux1, DmaMuxChannelId::kChannel0>;
    using DmaMux1Channel1  = DmaMuxChannel<DmaMuxControllerId::kDmaMux1, DmaMuxChannelId::kChannel1>;
    using DmaMux1Channel2  = DmaMuxChannel<DmaMuxControllerId::kDmaMux1, DmaMuxChannelId::kChannel2>;
    using DmaMux1Channel3  = DmaMuxChannel<DmaMuxControllerId::kDmaMux1, DmaMuxChannelId::kChannel3>;
    using DmaMux1Channel4  = DmaMuxChannel<DmaMuxControllerId::kDmaMux1, DmaMuxChannelId::kChannel4>;
    using DmaMux1Channel5  = DmaMuxChannel<DmaMuxControllerId::kDmaMux1, DmaMuxChannelId::kChannel5>;
    using DmaMux1Channel6  = DmaMuxChannel<DmaMuxControllerId::kDmaMux1, DmaMuxChannelId::kChannel6>;
    using DmaMux1Channel7  = DmaMuxChannel<DmaMuxControllerId::kDmaMux1, DmaMuxChannelId::kChannel7>;
    using DmaMux1Channel8  = DmaMuxChannel<DmaMuxControllerId::kDmaMux1, DmaMuxChannelId::kChannel8>;
    using DmaMux1Channel9  = DmaMuxChannel<DmaMuxControllerId::kDmaMux1, DmaMuxChannelId::kChannel9>;
    using DmaMux1Channel10 = DmaMuxChannel<DmaMuxControllerId::kDmaMux1, DmaMuxChannelId::kChannel10>;
    using DmaMux1Channel11 = DmaMuxChannel<DmaMuxControllerId::kDmaMux1, DmaMuxChannelId::kChannel11>;
    using DmaMux1Channel12 = DmaMuxChannel<DmaMuxControllerId::kDmaMux1, DmaMuxChannelId::kChannel12>;
    using DmaMux1Channel13 = DmaMuxChannel<DmaMuxControllerId::kDmaMux1, DmaMuxChannelId::kChannel13>;
    using DmaMux1Channel14 = DmaMuxChannel<DmaMuxControllerId::kDmaMux1, DmaMuxChannelId::kChannel14>;
    using DmaMux1Channel15 = DmaMuxChannel<DmaMuxControllerId::kDmaMux1, DmaMuxChannelId::kChannel15>;

}  // namespace valle::platform
