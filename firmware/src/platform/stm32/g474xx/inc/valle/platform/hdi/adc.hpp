#pragma once

#include "valle/platform/core.hpp"
#include "valle/platform/hardware/adc.hpp"

namespace valle::platform
{
    template <AdcControllerId tkControllerId>
    struct AdcControllerHdi : public AdcControllerInterface<tkControllerId>
    {
        struct Descriptor : public UniqueDeviceDescriptor
        {
        };

        static constexpr AdcControllerId skControllerId = tkControllerId;
    };

    template <AdcControllerId tkControllerId, AdcChannelId tkChannelId>
    struct AdcChannelHdi : public AdcChannelInterface<tkControllerId, tkChannelId>
    {
        struct Descriptor : public UniqueDeviceDescriptor
        {
        };

        static constexpr AdcControllerId skControllerId = tkControllerId;
        static constexpr AdcChannelId    skChannelId    = tkChannelId;
    };

    template <AdcControllerId tkControllerId, AdcInjectChannelRank tkInjectChannelRank>
    struct AdcInjectChannelHdi : public AdcInjectChannelInterface<tkControllerId, tkInjectChannelRank>
    {
        struct Descriptor : public UniqueDeviceDescriptor
        {
        };

        static constexpr AdcControllerId      skControllerId      = tkControllerId;
        static constexpr AdcInjectChannelRank skInjectChannelRank = tkInjectChannelRank;
    };

    template <AdcControllerId tkControllerId, AdcRegularChannelRank tkRegularChannelRank>
    struct AdcRegularChannelHdi : public AdcRegularChannelInterface<tkControllerId, tkRegularChannelRank>
    {
        struct Descriptor : public UniqueDeviceDescriptor
        {
        };

        static constexpr AdcControllerId       skControllerId       = tkControllerId;
        static constexpr AdcRegularChannelRank skRegularChannelRank = tkRegularChannelRank;
    };

}  // namespace valle::platform