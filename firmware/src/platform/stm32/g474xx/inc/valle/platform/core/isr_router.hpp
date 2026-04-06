#pragma once

namespace valle::platform
{

    template <typename TInterface, auto tkIntSource, bool tkShouldClear = true>
    struct InterruptSourceInterfaceBase
    {
    private:
        static constexpr TInterface skInterface{};

    public:
        static constexpr bool skShouldClear = tkShouldClear;

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

    template <typename TIrqSpec,
              typename TInterruptSource,
              template <TIrqSpec, TInterruptSource> typename TInterruptSourceInterface,
              template <TIrqSpec, TInterruptSource> typename TInterruptSourceRouter>
    struct IrqInterruptSourceIsrRouterContext
    {
        using IrqSpecT         = TIrqSpec;
        using InterruptSourceT = TInterruptSource;

        template <IrqSpecT tkSpec, InterruptSourceT tkIntSource>
        using InterfaceT = TInterruptSourceInterface<tkSpec, tkIntSource>;

        template <IrqSpecT tkSpec, InterruptSourceT Source>
        using RouterT = TInterruptSourceRouter<tkSpec, Source>;

        template <IrqSpecT tkSpec>
        [[nodiscard]] consteval bool has_registered_handlers()
        {
            constexpr auto values = InterruptSourceT.values();

            return [values]<std::size_t... Is>(std::index_sequence<Is...>)
            {
                return (CBoundIsrRouter<RouterT<tkSpec, values[Is]>> || ...);
            }(std::make_index_sequence<values.size()>{});
        }

        template <IrqSpecT tkSpec, InterruptSourceT tkIntSource>
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

        template <IrqSpecT tkSpec>
        static void dispatch_all_handlers()
        {
            constexpr auto values = InterruptSourceT.values();

            [values]<std::size_t... Is>(std::index_sequence<Is...>)
            { (dispatch_handler<tkSpec, values[Is]>(), ...); }(std::make_index_sequence<values.size()>{});
        }
    };

    template <typename TIrqSpec,
              typename TInterruptSource,
              template <TIrqSpec, TInterruptSource> typename TInterruptSourceInterface,
              template <TIrqSpec> typename TIrqRouter,
              template <TIrqSpec, TInterruptSource> typename TInterruptSourceRouter>
    struct IrqRouterContext
    {
        using IrqSpecT         = TIrqSpec;
        using InterruptSourceT = TInterruptSource;

        template <IrqSpecT tkSpec, InterruptSourceT tkIntSource>
        using SourceRouterT = TSourceRouter<tkSpec, tkIntSource>;

        template <IrqSpecT tkSpec>
        using IrqRouterT = TIrqRouter<tkSpec>;

        template <IrqSpecT tkSpec, InterruptSourceT tkIntSource>
        using InterruptSourceRouterT = TInterruptSourceRouter<tkSpec, tkIntSource>;

        using IrqInterruptSourceIsrRouterContextT = IrqInterruptSourceIsrRouterContext<TIrqSpec,
                                                                                       TInterruptSource,
                                                                                       TInterruptSourceInterface,
                                                                                       TInterruptSourceRouter>;

        template <IrqSpecT tkSpec>
        [[nodiscard]] consteval bool has_registered_interrupt_source_handlers()
        {
            return IrqInterruptSourceIsrRouterContextT::has_registered_handlers<tkSpec>();
        }

        template <IrqSpecT tkSpec>
        [[nodiscard]] consteval bool has_registered_irq_handler()
        {
            return CBoundIsrRouter<IrqRouterT<tkSpec>>;
        }

        template <IrqSpecT tkSpec>
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

            IrqInterruptSourceIsrRouterContextT::dispatch_all_handlers<tkSpec>();
        }
    };

}  // namespace valle::platform