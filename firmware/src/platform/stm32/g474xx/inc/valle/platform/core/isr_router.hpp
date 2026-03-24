#pragma once

namespace valle::platform
{

    template <typename T>
    concept CUnboundISRRouter = requires { typename T::UnboundIsrHandlerTag; };

    template <typename T>
    concept CBoundISRRouter = !CUnboundISRRouter<T>;

    template <bool tkAck = true>
    struct ISRRouterConfig
    {
        static constexpr bool skAck = tkAck;  // Whether to acknowledge the interrupt in the router before handling
    };

    template <typename T>
    concept CISRRouterHasConfig = requires { typename T::skConfig; };

    template <typename T>
    static constexpr auto kISRRouterConfig = []
    {
        if constexpr (CISRRouterHasConfig<T>)
        {
            return T::skConfig;
        }
        else
        {
            return ISRRouterConfig<>{};
        }
    }();

    template <typename T>
    static constexpr bool kISRRouterConfigAck = kISRRouterConfig<T>.skAck;

}  // namespace valle::platform