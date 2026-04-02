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

    template <typename TSpec,
              typename TInterruptSource,
              template <TSpec, TInterruptSource> typename TInterruptSourceInterface,
              template <TSpec, TInterruptSource> typename TInterruptSourceRouter>
    struct InterruptSourceIsrRouterContext
    {
        using SpecT            = TSpec;
        using InterruptSourceT = TInterruptSource;

        template <SpecT tkSpec, InterruptSourceT tkIntSource>
        using InterfaceT = TInterruptSourceInterface<tkSpec, tkIntSource>;

        template <TSpec Spec, InterruptSourceT Source>
        using RouterT = TInterruptSourceRouter<Spec, Source>;

        template <SpecT tkSpec>
        [[nodiscard]] consteval bool has_registered_handlers()
        {
            constexpr auto values = magic_enum::enum_values<InterruptSourceT>();

            return [values]<std::size_t... Is>(std::index_sequence<Is...>)
            {
                return (CBoundIsrRouter<RouterT<tkSpec, values[Is]>> || ...);
            }(std::make_index_sequence<values.size()>{});
        }

        template <SpecT tkSpec, InterruptSourceT tkIntSource>
        static void dispatch_handler()
        {
            using IntRouterT    = RouterT<tkSpec, tkIntSource>;
            using IntInterfaceT = TInterface<tkSpec, tkIntSource>;
            if constexpr (CBoundIsrRouter<IntRouterT>)
            {
                if (IntInterfaceT::is_pending())
                {
                    if constexpr (kIsrRouterConfigAck<IntRouterT>)
                    {
                        IntInterfaceT::clear();
                    }
                    IntRouterT::handle();
                }
            }
            else
            {
                if constexpr (IntInterfaceT::skShouldClear)
                {
                    if (IntInterfaceT::is_pending())
                    {
                        IntInterfaceT::clear();
                    }
                }
            }
        }

        template <SpecT tkSpec>
        static void dispatch_all_handlers()
        {
            constexpr auto values = magic_enum::enum_values<InterruptSourceT>();

            [values]<std::size_t... Is>(std::index_sequence<Is...>)
            { (dispatch_handler<tkSpec, values[Is]>(), ...); }(std::make_index_sequence<values.size()>{});
        }
    };

    template <typename TSpec,
              typename TInterruptSource,
              template <TSpec, TInterruptSource> typename TInterruptSourceInterface,
              template <TSpec> typename TIrqRouter,
              template <TSpec, TInterruptSource> typename TInterruptSourceRouter>
    struct InterruptIrqRouterContext
    {
        using SpecT            = TSpec;
        using InterruptSourceT = TInterruptSource;

        template <SpecT tkSpec, InterruptSourceT tkIntSource>
        using SourceRouterT = TSourceRouter<tkSpec, tkIntSource>;

        template <SpecT tkSpec>
        using IrqRouterT = TIrqRouter<tkSpec>;

        template <SpecT tkSpec, InterruptSourceT tkIntSource>
        using InterruptSourceRouterT = TInterruptSourceRouter<tkSpec, tkIntSource>;

        using InterruptSourceIsrRouterContextT =
            InterruptSourceIsrRouterContext<TSpec, TInterruptSource, TInterruptSourceInterface, TInterruptSourceRouter>;

        template <SpecT tkSpec>
        [[nodiscard]] consteval bool has_registered_interrupt_source_handlers()
        {
            return InterruptSourceIsrRouterContextT::has_registered_handlers<tkSpec>();
        }

        template <SpecT tkSpec>
        [[nodiscard]] consteval bool has_registered_irq_handler()
        {
            return CBoundIsrRouter<IrqRouterT<tkSpec>>;
        }

        template <SpecT tkSpec>
        static void dispatch()
        {
            constexpr bool has_irq_handler     = has_registered_irq_handler<tkSpec>();
            constexpr bool has_source_handlers = has_registered_interrupt_source_handlers<tkSpec>();
            static_assert(!(has_irq_handler && has_source_handlers),
                          "VALLE CONFLICT: IRQ and Source ISR handlers detected.");

            if constexpr (has_irq_handler)
            {
                IrqRouterT<tkSpec>::handle();
                return;
            }

            InterruptSourceIsrRouterContextT::dispatch_all_handlers<tkSpec>();
        }
    };

}  // namespace valle::platform