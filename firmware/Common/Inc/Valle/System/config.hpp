#pragma once

namespace valle::system
{
    template <typename TDriverList, typename TDriverContainer>
    struct ConfigBase
    {
        using DriversT         = TDriverList;
        using DriverContainerT = TDriverContainer;
    };
}  // namespace valle::system