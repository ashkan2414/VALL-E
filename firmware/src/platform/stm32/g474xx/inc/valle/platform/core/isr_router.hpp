#pragma once

namespace valle::platform
{

    template <typename T>
    concept CUnboundIsrRouter = requires { typename T::UnboundIsrHandlerTag; };

    template <typename T>
    concept CBoundIsrRouter = !CUnboundIsrRouter<T>;

    template <bool tkAck = true>
    struct ISRRouterConfig
    {
        static constexpr bool skAck = tkAck;  // Whether to acknowledge the interrupt in the router before handling
    };

    template <typename T>
    concept CIsrRouterHasConfig = requires { typename T::skConfig; };

    template <typename T>
    static constexpr auto kIsrRouterConfig = []
    {
        if constexpr (CIsrRouterHasConfig<T>)
        {
            return T::skConfig;
        }
        else
        {
            return ISRRouterConfig<>{};
        }
    }();

    template <typename T>
    static constexpr bool kIsrRouterConfigAck = kIsrRouterConfig<T>.skAck;

}  // namespace valle::platform