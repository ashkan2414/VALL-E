#pragma once

namespace valle::system
{
    struct EmptyDeviceContainer
    {
        using DevicesT = TypeList<>;
    };

    template <typename TDriverContainer, typename TDeviceContainer = EmptyDeviceContainer>
    struct ConfigBase
    {
        using DriverContainerT = TDriverContainer;
        using DeviceContainerT = TDeviceContainer;
    };
}  // namespace valle::system