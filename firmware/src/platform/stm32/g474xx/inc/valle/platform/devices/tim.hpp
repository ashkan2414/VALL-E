#pragma once

#include "stm32g4xx_ll_tim.h"
#include "valle/platform/core.hpp"
#include "valle/platform/devices/gpio.hpp"
#include "valle/platform/drivers/gpio/alternate_function.hpp"
#include "valle/platform/hdi/tim.hpp"

namespace valle::platform
{
    // =============================================================================
    // TIMER CONTROLLER DEVICE (SHARED DEVICE)
    // =============================================================================

    // -----------------------------------------------------------------------------
    // COMPILE TIME CONFIGURATIONS
    // -----------------------------------------------------------------------------
    struct TimControllerCTDefaultConfig
    {
        using Ch1PinT   = NullDevice;
        using Ch2PinT   = NullDevice;
        using Ch3PinT   = NullDevice;
        using Ch4PinT   = NullDevice;
        using Ch1NPinT  = NullDevice;
        using Ch2NPinT  = NullDevice;
        using Ch3NPinT  = NullDevice;
        using Ch4NPinT  = NullDevice;
        using ETRPinT   = NullDevice;
        using BKINPinT  = NullDevice;
        using BKIN2PinT = NullDevice;
    };

    template <typename T, TimControllerId tkControllerId>
    concept CValidTimControllerCTConfig =
        requires {
            typename T::Ch1PinT;
            typename T::Ch2PinT;
            typename T::Ch3PinT;
            typename T::Ch4PinT;
            typename T::Ch1NPinT;
            typename T::Ch2NPinT;
            typename T::Ch3NPinT;
            typename T::Ch4NPinT;
            typename T::ETRPinT;
            typename T::BKINPinT;
            typename T::BKIN2PinT;
        } &&
        (CNullDevice<typename T::Ch1PinT> ||
         CValidTimPin<tkControllerId, TimGpioPinType::kCh1, T::Ch1PinT::skPortId, T::Ch1PinT::skPinId>) &&
        (CNullDevice<typename T::Ch2PinT> ||
         CValidTimPin<tkControllerId, TimGpioPinType::kCh2, T::Ch2PinT::skPortId, T::Ch2PinT::skPinId>) &&
        (CNullDevice<typename T::Ch3PinT> ||
         CValidTimPin<tkControllerId, TimGpioPinType::kCh3, T::Ch3PinT::skPortId, T::Ch3PinT::skPinId>) &&
        (CNullDevice<typename T::Ch4PinT> ||
         CValidTimPin<tkControllerId, TimGpioPinType::kCh4, T::Ch4PinT::skPortId, T::Ch4PinT::skPinId>) &&
        (CNullDevice<typename T::Ch1NPinT> ||
         CValidTimPin<tkControllerId, TimGpioPinType::kCh1N, T::Ch1NPinT::skPortId, T::Ch1NPinT::skPinId>) &&
        (CNullDevice<typename T::Ch2NPinT> ||
         CValidTimPin<tkControllerId, TimGpioPinType::kCh2N, T::Ch2NPinT::skPortId, T::Ch2NPinT::skPinId>) &&
        (CNullDevice<typename T::Ch3NPinT> ||
         CValidTimPin<tkControllerId, TimGpioPinType::kCh3N, T::Ch3NPinT::skPortId, T::Ch3NPinT::skPinId>) &&
        (CNullDevice<typename T::Ch4NPinT> ||
         CValidTimPin<tkControllerId, TimGpioPinType::kCh4N, T::Ch4NPinT::skPortId, T::Ch4NPinT::skPinId>) &&
        (CNullDevice<typename T::ETRPinT> ||
         CValidTimPin<tkControllerId, TimGpioPinType::kETR, T::ETRPinT::skPortId, T::ETRPinT::skPinId>) &&
        (CNullDevice<typename T::BKINPinT> ||
         CValidTimPin<tkControllerId, TimGpioPinType::kBKIN, T::BKINPinT::skPortId, T::BKINPinT::skPinId>) &&
        (CNullDevice<typename T::BKIN2PinT> ||
         CValidTimPin<tkControllerId, TimGpioPinType::kBKIN2, T::BKIN2PinT::skPortId, T::BKIN2PinT::skPinId>);

    template <TimControllerId tkControllerId>
    struct TimControllerCTConfigRegistry
    {
        static constexpr auto skConfig = TimControllerCTDefaultConfig{};
    };

#define VALLE_DEFINE_TIMER_CONTROLLER_CT_CONFIG(tkControllerId, config)                      \
    namespace valle::platform                                                                \
    {                                                                                        \
        template <>                                                                          \
        struct TimControllerCTConfigRegistry<(tkControllerId)>                               \
        {                                                                                    \
            static constexpr auto skConfig = (config);                                       \
            static_assert(CValidTimControllerCTConfig<decltype(skConfig), (tkControllerId)>, \
                          "Invalid TIM Controller CT Config");                               \
        };                                                                                   \
    }

    // -----------------------------------------------------------------------------
    // CONFIGURATIONS
    // -----------------------------------------------------------------------------
    struct TimChannelComplimentaryOutputCaptureConfig
    {
        TimChannelOutputCapturePolarity  polarity{};
        TimChannelOutputCaptureIdleState idle_state{};
    };

    struct TimChannelOutputCaptureConfig
    {
        TimChannelOutputCaptureMode                               mode{};
        TimChannelOutputCapturePolarity                           polarity{};
        TimChannelOutputCaptureIdleState                          idle_state{};
        bool                                                      enable_preload{};
        bool                                                      enable_fast_mode{};
        bool                                                      enable_clear{};
        std::optional<TimChannelComplimentaryOutputCaptureConfig> complementary_output_config{};
        uint32_t                                                  compare_value{};
    };

    struct TimChannelInputCaptureConfig
    {
        TimChannelInputCaptureActiveInput active_input{};
        TimChannelInputCapturePrescaler   prescaler{};
        TimChannelInputCaptureFilter      filter{};
        TimChannelInputCapturePolarity    polarity{};
    };

    struct TimControllerEncoderConfig
    {
        TimControllerEncoderMode       mode{};
        TimChannelInputCaptureConfig   ch_config{};
        TimChannelInputCapturePolarity ch2_polarity{};
        uint32_t                       auto_reload = 0xFFFFFFFFU;
    };

    // -----------------------------------------------------------------------------
    // HELPERS
    // -----------------------------------------------------------------------------
    namespace detail
    {
        template <TimControllerId tkControllerId, TimGpioPinType tkPinType, typename TPin>
        struct TimGpioPinDriverHelper
        {
            using type =
                GpioAlternateFunctionDriver<TPin, kTimPinAF<tkControllerId, tkPinType, TPin::skPortId, TPin::skPinId>>;
        };

        template <TimControllerId tkControllerId, TimGpioPinType tkPinType>
        struct TimGpioPinDriverHelper<tkControllerId, tkPinType, NullDevice>
        {
            using type = std::monostate;
        };
    }  // namespace detail

    template <TimControllerId tkControllerId, TimGpioPinType tkPinType, typename TPin>
    using TimGpioPinDriver = typename detail::TimGpioPinDriverHelper<tkControllerId, tkPinType, TPin>::type;

    // -----------------------------------------------------------------------------
    // DEVICE CLASS
    // -----------------------------------------------------------------------------

    template <TimControllerId tkControllerId>
    class TimController
    {
    public:
        struct Descriptor : public UniqueDeviceDescriptor
        {
        };

        static constexpr TimControllerId skControllerId = tkControllerId;

        using TraitsT       = TimControllerTraits<tkControllerId>;
        using InterfaceT    = TimControllerInterface<tkControllerId>;
        using CounterValueT = typename TraitsT::CounterValueT;

        template <TimChannelId tkChannelId>
        using ChannelInterfaceT = TimChannelInterface<tkControllerId, tkChannelId>;

        using CTConfigRegistryT          = TimControllerCTConfigRegistry<tkControllerId>;
        static constexpr auto skCTConfig = CTConfigRegistryT::skConfig;
        using CTConfigT                  = decltype(skCTConfig);

        // Pin Aliases
        template <TimGpioPinType tkPinType>
        using PinDeviceT = std::conditional_t<
            tkPinType == TimGpioPinType::kCh1,
            typename CTConfigT::Ch1PinT,
            std::conditional_t<
                tkPinType == TimGpioPinType::kCh2,
                typename CTConfigT::Ch2PinT,
                std::conditional_t<
                    tkPinType == TimGpioPinType::kCh3,
                    typename CTConfigT::Ch3PinT,
                    std::conditional_t<
                        tkPinType == TimGpioPinType::kCh4,
                        typename CTConfigT::Ch4PinT,
                        std::conditional_t<
                            tkPinType == TimGpioPinType::kCh1N,
                            typename CTConfigT::Ch1NPinT,
                            std::conditional_t<
                                tkPinType == TimGpioPinType::kCh2N,
                                typename CTConfigT::Ch2NPinT,
                                std::conditional_t<
                                    tkPinType == TimGpioPinType::kCh3N,
                                    typename CTConfigT::Ch3NPinT,
                                    std::conditional_t<
                                        tkPinType == TimGpioPinType::kCh4N,
                                        typename CTConfigT::Ch4NPinT,
                                        std::conditional_t<tkPinType == TimGpioPinType::kETR,
                                                           typename CTConfigT::ETRPinT,
                                                           std::conditional_t<tkPinType == TimGpioPinType::kBKIN,
                                                                              typename CTConfigT::BKINPinT,
                                                                              typename CTConfigT::BKIN2PinT>>>>>>>>>>;

        using Ch1PinT   = PinDeviceT<TimGpioPinType::kCh1>;
        using Ch2PinT   = PinDeviceT<TimGpioPinType::kCh2>;
        using Ch3PinT   = PinDeviceT<TimGpioPinType::kCh3>;
        using Ch4PinT   = PinDeviceT<TimGpioPinType::kCh4>;
        using Ch1NPinT  = PinDeviceT<TimGpioPinType::kCh1N>;
        using Ch2NPinT  = PinDeviceT<TimGpioPinType::kCh2N>;
        using Ch3NPinT  = PinDeviceT<TimGpioPinType::kCh3N>;
        using Ch4NPinT  = PinDeviceT<TimGpioPinType::kCh4N>;
        using ETRPinT   = PinDeviceT<TimGpioPinType::kETR>;
        using BKINPinT  = PinDeviceT<TimGpioPinType::kBKIN>;
        using BKIN2PinT = PinDeviceT<TimGpioPinType::kBKIN2>;

        // Check availability
        template <TimGpioPinType tkPinType>
        static constexpr bool skHasPin = !CNullDevice<PinDeviceT<tkPinType>>;

        static constexpr bool skHasCh1Pin   = skHasPin<TimGpioPinType::kCh1>;
        static constexpr bool skHasCh2Pin   = skHasPin<TimGpioPinType::kCh2>;
        static constexpr bool skHasCh3Pin   = skHasPin<TimGpioPinType::kCh3>;
        static constexpr bool skHasCh4Pin   = skHasPin<TimGpioPinType::kCh4>;
        static constexpr bool skHasCh1NPin  = skHasPin<TimGpioPinType::kCh1N>;
        static constexpr bool skHasCh2NPin  = skHasPin<TimGpioPinType::kCh2N>;
        static constexpr bool skHasCh3NPin  = skHasPin<TimGpioPinType::kCh3N>;
        static constexpr bool skHasCh4NPin  = skHasPin<TimGpioPinType::kCh4N>;
        static constexpr bool skHasETRPin   = skHasPin<TimGpioPinType::kETR>;
        static constexpr bool skHasBKINPin  = skHasPin<TimGpioPinType::kBKIN>;
        static constexpr bool skHasBKIN2Pin = skHasPin<TimGpioPinType::kBKIN2>;

        template <TimGpioPinType tkPinType>
        using PinDriverT = TimGpioPinDriver<tkControllerId, tkPinType, PinDeviceT<tkPinType>>;

        using Ch1PinDriverT   = PinDriverT<TimGpioPinType::kCh1>;
        using Ch2PinDriverT   = PinDriverT<TimGpioPinType::kCh2>;
        using Ch3PinDriverT   = PinDriverT<TimGpioPinType::kCh3>;
        using Ch4PinDriverT   = PinDriverT<TimGpioPinType::kCh4>;
        using Ch1NPinDriverT  = PinDriverT<TimGpioPinType::kCh1N>;
        using Ch2NPinDriverT  = PinDriverT<TimGpioPinType::kCh2N>;
        using Ch3NPinDriverT  = PinDriverT<TimGpioPinType::kCh3N>;
        using Ch4NPinDriverT  = PinDriverT<TimGpioPinType::kCh4N>;
        using ETRPinDriverT   = PinDriverT<TimGpioPinType::kETR>;
        using BKINPinDriverT  = PinDriverT<TimGpioPinType::kBKIN>;
        using BKIN2PinDriverT = PinDriverT<TimGpioPinType::kBKIN2>;

        using InjectDevices = FilterNullDevices<TypeList<Ch1PinT,
                                                         Ch2PinT,
                                                         Ch3PinT,
                                                         Ch4PinT,
                                                         Ch1NPinT,
                                                         Ch2NPinT,
                                                         Ch3NPinT,
                                                         Ch4NPinT,
                                                         ETRPinT,
                                                         BKINPinT,
                                                         BKIN2PinT>>;

    private:
        Ch1PinDriverT   m_ch1_pin{};
        Ch2PinDriverT   m_ch2_pin{};
        Ch3PinDriverT   m_ch3_pin{};
        Ch4PinDriverT   m_ch4_pin{};
        Ch1NPinDriverT  m_ch1n_pin{};
        Ch2NPinDriverT  m_ch2n_pin{};
        Ch3NPinDriverT  m_ch3n_pin{};
        Ch4NPinDriverT  m_ch4n_pin{};
        ETRPinDriverT   m_etr_pin{};
        BKINPinDriverT  m_bkin_pin{};
        BKIN2PinDriverT m_bkin2_pin{};

    public:
        TimController() = delete;

        template <typename... TArgs>
        explicit TimController(TArgs&&... args)
            : m_ch1_pin(extract_device_ref<skHasCh1Pin, Ch1PinT>(std::forward<TArgs>(args)...))
            , m_ch2_pin(extract_device_ref<skHasCh2Pin, Ch2PinT>(std::forward<TArgs>(args)...))
            , m_ch3_pin(extract_device_ref<skHasCh3Pin, Ch3PinT>(std::forward<TArgs>(args)...))
            , m_ch4_pin(extract_device_ref<skHasCh4Pin, Ch4PinT>(std::forward<TArgs>(args)...))
            , m_ch1n_pin(extract_device_ref<skHasCh1NPin, Ch1NPinT>(std::forward<TArgs>(args)...))
            , m_ch2n_pin(extract_device_ref<skHasCh2NPin, Ch2NPinT>(std::forward<TArgs>(args)...))
            , m_ch3n_pin(extract_device_ref<skHasCh3NPin, Ch3NPinT>(std::forward<TArgs>(args)...))
            , m_ch4n_pin(extract_device_ref<skHasCh4NPin, Ch4NPinT>(std::forward<TArgs>(args)...))
            , m_etr_pin(extract_device_ref<skHasETRPin, ETRPinT>(std::forward<TArgs>(args)...))
            , m_bkin_pin(extract_device_ref<skHasBKINPin, BKINPinT>(std::forward<TArgs>(args)...))
            , m_bkin2_pin(extract_device_ref<skHasBKIN2Pin, BKIN2PinT>(std::forward<TArgs>(args)...))
        {
        }

        [[nodiscard]] bool init()
        {
            TraitsT::enable_clock();
            TimingContext::delay_cycles(10U);  // NOLINT(readability-magic-numbers)

            return true;
        }

        template <TimGpioPinType tkPinType>
            requires(skHasPin<tkPinType>)
        [[nodiscard]] bool init_pin(const GpioAlternateFunctionConfig& pin_cfg)
        {
            return get_pin_driver<tkPinType>().init(pin_cfg);
        }

        template <TimChannelId tkChannel>
        [[nodiscard]] bool init_channel_output_capture(const TimChannelOutputCaptureConfig& config)
        {
            using CInterfaceT = ChannelInterfaceT<tkChannel>;

            CInterfaceT::set_output_capture_mode(config.mode);
            CInterfaceT::set_output_capture_polarity(config.polarity);
            CInterfaceT::set_output_capture_idle_state(config.idle_state);
            CInterfaceT::set_output_capture_preload(config.enable_preload);
            CInterfaceT::set_output_capture_fast_mode(config.enable_fast_mode);
            CInterfaceT::set_output_capture_clear(config.enable_clear);

            if (config.complementary_output_config.has_value())
            {
                const auto& comp_cfg = config.complementary_output_config.value();
                CInterfaceT::set_complementary_output_capture_polarity(comp_cfg.polarity);
                CInterfaceT::set_complementary_output_capture_idle_state(comp_cfg.idle_state);
                CInterfaceT::enable_complementary();
            }
            else
            {
                CInterfaceT::disable_complementary();
            }

            CInterfaceT::set_compare(config.compare_value);
            return true;
        }

        template <TimChannelId tkChannel>
        [[nodiscard]] bool init_channel_input_capture(const TimChannelInputCaptureConfig& config)
        {
            using CInterfaceT = ChannelInterfaceT<tkChannel>;

            CInterfaceT::set_input_capture_active_input(config.active_input);
            CInterfaceT::set_input_capture_prescaler(config.prescaler);
            CInterfaceT::set_input_capture_filter(config.filter);
            CInterfaceT::set_input_capture_polarity(config.polarity);
            return true;
        }

        [[nodiscard]] bool init_encoder_gpio(const GpioAlternateFunctionConfig& gpio_config)
        {
            return init_pin<TimGpioPinType::kCh1>(gpio_config) && init_pin<TimGpioPinType::kCh2>(gpio_config);
        }

        [[nodiscard]] bool init_encoder(const TimControllerEncoderConfig& config)
        {
            InterfaceT::set_encoder_mode(config.mode);
            if (!init_channel_input_capture<TimChannelId::kCh1>(config.ch_config))
            {
                return false;
            }

            TimChannelInputCaptureConfig ch2_config = config.ch_config;
            ch2_config.polarity                     = config.ch2_polarity;
            if (!init_channel_input_capture<TimChannelId::kCh2>(ch2_config))
            {
                return false;
            }

            InterfaceT::set_auto_reload(config.auto_reload);
            return true;
        }

        void enable_counter()
        {
            InterfaceT::enable_counter();
        }

        void disable_counter()
        {
            InterfaceT::disable_counter();
        }

        template <TimChannelId tkChannel>
        void enable_channel()
        {
            ChannelInterfaceT<tkChannel>::enable();
        }

        template <TimChannelId tkChannel>
        void disable_channel()
        {
            ChannelInterfaceT<tkChannel>::disable();
        }

        template <TimChannelId tkChannel>
        void enable_channel_complementary()
        {
            ChannelInterfaceT<tkChannel>::enable_complementary();
        }

        template <TimChannelId tkChannel>
        void disable_channel_complementary()
        {
            ChannelInterfaceT<tkChannel>::disable_complementary();
        }

        [[nodiscard]] CounterValueT get_counter() const
        {
            return InterfaceT::get_counter();
        }

    private:
        template <TimGpioPinType tkPinType>
            requires(skHasPin<tkPinType>)
        [[nodiscard]] constexpr PinDriverT<tkPinType>& get_pin_driver()
        {
            if constexpr (tkPinType == TimGpioPinType::kCh1)
            {
                return m_ch1_pin;
            }

            if constexpr (tkPinType == TimGpioPinType::kCh2)
            {
                return m_ch2_pin;
            }

            if constexpr (tkPinType == TimGpioPinType::kCh3)
            {
                return m_ch3_pin;
            }

            if constexpr (tkPinType == TimGpioPinType::kCh4)
            {
                return m_ch4_pin;
            }

            if constexpr (tkPinType == TimGpioPinType::kCh1N)
            {
                return m_ch1n_pin;
            }

            if constexpr (tkPinType == TimGpioPinType::kCh2N)
            {
                return m_ch2n_pin;
            }

            if constexpr (tkPinType == TimGpioPinType::kCh3N)
            {
                return m_ch3n_pin;
            }

            if constexpr (tkPinType == TimGpioPinType::kCh4N)
            {
                return m_ch4n_pin;
            }

            if constexpr (tkPinType == TimGpioPinType::kETR)
            {
                return m_etr_pin;
            }

            if constexpr (tkPinType == TimGpioPinType::kBKIN)
            {
                return m_bkin_pin;
            }

            if constexpr (tkPinType == TimGpioPinType::kBKIN2)
            {
                return m_bkin2_pin;
            }
        }
    };

    // ---------------------------------------------------------------------------
    // DEVICE ALIASES
    // ---------------------------------------------------------------------------

    using Tim1Controller  = TimController<TimControllerId::kTim1>;
    using Tim2Controller  = TimController<TimControllerId::kTim2>;
    using Tim3Controller  = TimController<TimControllerId::kTim3>;
    using Tim4Controller  = TimController<TimControllerId::kTim4>;
    using Tim5Controller  = TimController<TimControllerId::kTim5>;
    using Tim6Controller  = TimController<TimControllerId::kTim6>;
    using Tim7Controller  = TimController<TimControllerId::kTim7>;
    using Tim8Controller  = TimController<TimControllerId::kTim8>;
    using Tim15Controller = TimController<TimControllerId::kTim15>;
    using Tim16Controller = TimController<TimControllerId::kTim16>;
    using Tim17Controller = TimController<TimControllerId::kTim17>;
    using Tim20Controller = TimController<TimControllerId::kTim20>;

    // =============================================================================
    // MULTI-CHANNEL CONTROLLER DEVICE (SHARED DEVICE)
    // Consumes the Unique Controller to allow multiple Channel Devices to coexist.
    // =============================================================================

    template <TimControllerId tkControllerId, bool tkShared = true>
    class TimMultiChannelController
    {
    public:
        struct Descriptor : public std::conditional_t<tkShared, SharedDeviceDescriptor, UniqueDeviceDescriptor>
        {
        };
        using ControllerT   = TimController<tkControllerId>;
        using InjectDevices = TypeList<ControllerT>;

    private:
        DeviceRef<ControllerT> m_controller;

    public:
        TimMultiChannelController() = delete;

        explicit TimMultiChannelController(DeviceRef<ControllerT>&& controller) : m_controller(std::move(controller))
        {
        }

        [[nodiscard]] inline bool init()
        {
            return m_controller->init();
        }
    };

    // =============================================================================
    // TIMER CHANNEL DEVICE (UNIQUE DEVICE)
    // Manages a single CCR register and its associated CHx/CHxN pins.
    // =============================================================================

    struct TimChannelCTConfig
    {
        using ChPinT  = NullDevice;
        using ChNPinT = NullDevice;
    };

    template <TimControllerId tkControllerId, TimChannelId tkChannel>
    class TimChannel
    {
    public:
        struct Descriptor : public UniqueDeviceDescriptor
        {
        };

        using MultiControllerT = TimMultiChannelController<tkControllerId, true>;

        // Pins for this specific channel
        using ChPinT  = typename TimChannelCTConfig::ChPinT;
        using ChNPinT = typename TimChannelCTConfig::ChNPinT;

        using InjectDevices = FilterNullDevices<TypeList<MultiControllerT, ChPinT, ChNPinT>>;

    private:
        DeviceRef<MultiControllerT> m_multi;

    public:
        TimChannel() = delete;

        explicit TimChannel(DeviceRef<MultiControllerT>&& multi) : m_multi(std::move(multi))
        {
        }

        [[nodiscard]] inline bool init()
        {
            return true;
        }
    };

}  // namespace valle::platform