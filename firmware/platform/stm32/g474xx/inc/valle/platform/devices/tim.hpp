#pragma once

#include "stm32g4xx_ll_tim.h"
#include "valle/core.hpp"
#include "valle/core/device/device.hpp"
#include "valle/platform/core/timing.hpp"
#include "valle/platform/hardware/tim.hpp"

namespace valle
{
    // =============================================================================
    // FORWARD DECLARATIONS
    // =============================================================================

    class TIMRootDevice;

    template <TIMControllerID tkControllerID>
    class TIMControllerDevice;

    template <TIMControllerID tkControllerID, bool tkShared>
    class TIMMultiChannelControllerDevice;

    template <TIMControllerID tkControllerID, TIMChannelID tkChannelID>
    class TIMChannelDevice;

    // =============================================================================
    // TIMER ROOT DEVICE (INTERFACE DEVICE)
    // =============================================================================

    class TIMRootDevice
    {
    public:
        struct Descriptor : public InterfaceDeviceDescriptor
        {
            using Children = DeviceTreeList<TIMControllerDevice<TIMControllerID::kTim1>,
                                            TIMControllerDevice<TIMControllerID::kTim2>,
                                            TIMControllerDevice<TIMControllerID::kTim3>,
                                            TIMControllerDevice<TIMControllerID::kTim4>,
                                            TIMControllerDevice<TIMControllerID::kTim5>,
                                            TIMControllerDevice<TIMControllerID::kTim6>,
                                            TIMControllerDevice<TIMControllerID::kTim7>,
                                            TIMControllerDevice<TIMControllerID::kTim8>,
                                            TIMControllerDevice<TIMControllerID::kTim15>,
                                            TIMControllerDevice<TIMControllerID::kTim16>,
                                            TIMControllerDevice<TIMControllerID::kTim17>,
                                            TIMControllerDevice<TIMControllerID::kTim20>>;
        };
    };

    // =============================================================================
    // TIMER CONTROLLER DEVICE (SHARED DEVICE)
    // =============================================================================

    // -----------------------------------------------------------------------------
    // COMPILE TIME CONFIGURATIONS
    // -----------------------------------------------------------------------------
    struct TIMControllerCTConfigDefaults
    {
        using Ch1PinT   = GPIONullPinDevice;
        using Ch2PinT   = GPIONullPinDevice;
        using Ch3PinT   = GPIONullPinDevice;
        using Ch4PinT   = GPIONullPinDevice;
        using Ch1NPinT  = GPIONullPinDevice;
        using Ch2NPinT  = GPIONullPinDevice;
        using Ch3NPinT  = GPIONullPinDevice;
        using Ch4NPinT  = GPIONullPinDevice;
        using ETRPinT   = GPIONullPinDevice;
        using BKINPinT  = GPIONullPinDevice;
        using BKIN2PinT = GPIONullPinDevice;
    };

    template <typename T, TIMControllerID tkControllerID>
    concept CValidTIMControllerCTConfig =
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
        (CNullGPIOPinDevice<typename T::Ch1PinT> ||
         CValidTIMPin<tkControllerID, TIMGPIOPinType::kCh1, T::Ch1PinT::skPortID, T::Ch1PinT::skPinID>) &&
        (CNullGPIOPinDevice<typename T::Ch2PinT> ||
         CValidTIMPin<tkControllerID, TIMGPIOPinType::kCh2, T::Ch2PinT::skPortID, T::Ch2PinT::skPinID>) &&
        (CNullGPIOPinDevice<typename T::Ch3PinT> ||
         CValidTIMPin<tkControllerID, TIMGPIOPinType::kCh3, T::Ch3PinT::skPortID, T::Ch3PinT::skPinID>) &&
        (CNullGPIOPinDevice<typename T::Ch4PinT> ||
         CValidTIMPin<tkControllerID, TIMGPIOPinType::kCh4, T::Ch4PinT::skPortID, T::Ch4PinT::skPinID>) &&
        (CNullGPIOPinDevice<typename T::Ch1NPinT> ||
         CValidTIMPin<tkControllerID, TIMGPIOPinType::kCh1N, T::Ch1NPinT::skPortID, T::Ch1NPinT::skPinID>) &&
        (CNullGPIOPinDevice<typename T::Ch2NPinT> ||
         CValidTIMPin<tkControllerID, TIMGPIOPinType::kCh2N, T::Ch2NPinT::skPortID, T::Ch2NPinT::skPinID>) &&
        (CNullGPIOPinDevice<typename T::Ch3NPinT> ||
         CValidTIMPin<tkControllerID, TIMGPIOPinType::kCh3N, T::Ch3NPinT::skPortID, T::Ch3NPinT::skPinID>) &&
        (CNullGPIOPinDevice<typename T::Ch4NPinT> ||
         CValidTIMPin<tkControllerID, TIMGPIOPinType::kCh4N, T::Ch4NPinT::skPortID, T::Ch4NPinT::skPinID>) &&
        (CNullGPIOPinDevice<typename T::ETRPinT> ||
         CValidTIMPin<tkControllerID, TIMGPIOPinType::kETR, T::ETRPinT::skPortID, T::ETRPinT::skPinID>) &&
        (CNullGPIOPinDevice<typename T::BKINPinT> ||
         CValidTIMPin<tkControllerID, TIMGPIOPinType::kBKIN, T::BKINPinT::skPortID, T::BKINPinT::skPinID>) &&
        (CNullGPIOPinDevice<typename T::BKIN2PinT> ||
         CValidTIMPin<tkControllerID, TIMGPIOPinType::kBKIN2, T::BKIN2PinT::skPortID, T::BKIN2PinT::skPinID>);

    template <TIMControllerID tkControllerID>
    struct TIMControllerCTConfigTraits
    {
        static constexpr auto skConfig = TIMControllerCTConfigDefaults{};
    };

#define VALLE_DEFINE_TIMER_CONTROLLER_CT_CONFIG(tkControllerID, config)                      \
    namespace valle                                                                          \
    {                                                                                        \
        template <>                                                                          \
        struct TIMControllerCTConfigTraits<(tkControllerID)>                                 \
        {                                                                                    \
            static constexpr auto skConfig = (config);                                       \
            static_assert(CValidTIMControllerCTConfig<decltype(skConfig), (tkControllerID)>, \
                          "Invalid TIM Controller CT Config");                               \
        };                                                                                   \
    }

    // -----------------------------------------------------------------------------
    // CONFIGURATIONS
    // -----------------------------------------------------------------------------
    struct TIMChannelComplimentaryOutputCaptureConfig
    {
        TIMChannelOutputCapturePolarity  polarity{};
        TIMChannelOutputCaptureIdleState idle_state{};
    };

    struct TIMChannelOutputCaptureConfig
    {
        TIMChannelOutputCaptureMode                               mode{};
        TIMChannelOutputCapturePolarity                           polarity{};
        TIMChannelOutputCaptureIdleState                          idle_state{};
        bool                                                      enable_preload{};
        bool                                                      enable_fast_mode{};
        bool                                                      enable_clear{};
        std::optional<TIMChannelComplimentaryOutputCaptureConfig> complementary_output_config{};
        uint32_t                                                  compare_value{};
    };

    struct TIMChannelInputCaptureConfig
    {
        TIMChannelInputCaptureActiveInput active_input{};
        TIMChannelInputCapturePrescaler   prescaler{};
        TIMChannelInputCaptureFilter      filter{};
        TIMChannelInputCapturePolarity    polarity{};
    };

    struct TIMControllerEncoderConfig
    {
        TIMControllerEncoderMode       mode{};
        TIMChannelInputCaptureConfig   ch_config{};
        TIMChannelInputCapturePolarity ch2_polarity{};
        uint32_t                       auto_reload = 0xFFFFFFFFU;
    };

    // -----------------------------------------------------------------------------
    // HELPERS
    // -----------------------------------------------------------------------------
    namespace detail
    {
        template <TIMControllerID tkControllerID, TIMGPIOPinType tkPinType, typename TPinDevice>
        struct TIMGPIOPinDriverHelper
        {
            using type = GPIOAlternativeFunctionDriver<
                TPinDevice,
                kTIMPinAF<tkControllerID, tkPinType, TPinDevice::skPortID, TPinDevice::skPinID>>;
        };

        template <TIMControllerID tkControllerID, TIMGPIOPinType tkPinType>
        struct TIMGPIOPinDriverHelper<tkControllerID, tkPinType, GPIONullPinDevice>
        {
            using type = std::monostate;
        };
    }  // namespace detail

    template <TIMControllerID tkControllerID, TIMGPIOPinType tkPinType, typename TPinDevice>
    using TIMGPIOPinDriver = typename detail::TIMGPIOPinDriverHelper<tkControllerID, tkPinType, TPinDevice>::type;

    // -----------------------------------------------------------------------------
    // DEVICE CLASS
    // -----------------------------------------------------------------------------

    template <TIMControllerID tkControllerID>
    class TIMControllerDevice
    {
    public:
        struct Descriptor : public UniqueDeviceDescriptor
        {
        };

        static constexpr TIMControllerID skControllerID = tkControllerID;

        using TraitsT       = TIMControllerTraits<tkControllerID>;
        using InterfaceT    = TIMControllerInterface<tkControllerID>;
        using CounterValueT = typename TraitsT::CounterValueT;

        template <TIMChannelID tkChannelID>
        using ChannelInterfaceT = TIMChannelInterface<tkControllerID, tkChannelID>;

        using CTConfigTraitsT            = TIMControllerCTConfigTraits<tkControllerID>;
        static constexpr auto skCTConfig = CTConfigTraitsT::skConfig;
        using CTConfigT                  = decltype(skCTConfig);

        // Pin Aliases
        template <TIMGPIOPinType tkPinType>
        using PinDeviceT = std::conditional_t<
            tkPinType == TIMGPIOPinType::kCh1,
            typename CTConfigT::Ch1PinT,
            std::conditional_t<
                tkPinType == TIMGPIOPinType::kCh2,
                typename CTConfigT::Ch2PinT,
                std::conditional_t<
                    tkPinType == TIMGPIOPinType::kCh3,
                    typename CTConfigT::Ch3PinT,
                    std::conditional_t<
                        tkPinType == TIMGPIOPinType::kCh4,
                        typename CTConfigT::Ch4PinT,
                        std::conditional_t<
                            tkPinType == TIMGPIOPinType::kCh1N,
                            typename CTConfigT::Ch1NPinT,
                            std::conditional_t<
                                tkPinType == TIMGPIOPinType::kCh2N,
                                typename CTConfigT::Ch2NPinT,
                                std::conditional_t<
                                    tkPinType == TIMGPIOPinType::kCh3N,
                                    typename CTConfigT::Ch3NPinT,
                                    std::conditional_t<
                                        tkPinType == TIMGPIOPinType::kCh4N,
                                        typename CTConfigT::Ch4NPinT,
                                        std::conditional_t<tkPinType == TIMGPIOPinType::kETR,
                                                           typename CTConfigT::ETRPinT,
                                                           std::conditional_t<tkPinType == TIMGPIOPinType::kBKIN,
                                                                              typename CTConfigT::BKINPinT,
                                                                              typename CTConfigT::BKIN2PinT>>>>>>>>>>;

        using Ch1PinT   = PinDeviceT<TIMGPIOPinType::kCh1>;
        using Ch2PinT   = PinDeviceT<TIMGPIOPinType::kCh2>;
        using Ch3PinT   = PinDeviceT<TIMGPIOPinType::kCh3>;
        using Ch4PinT   = PinDeviceT<TIMGPIOPinType::kCh4>;
        using Ch1NPinT  = PinDeviceT<TIMGPIOPinType::kCh1N>;
        using Ch2NPinT  = PinDeviceT<TIMGPIOPinType::kCh2N>;
        using Ch3NPinT  = PinDeviceT<TIMGPIOPinType::kCh3N>;
        using Ch4NPinT  = PinDeviceT<TIMGPIOPinType::kCh4N>;
        using ETRPinT   = PinDeviceT<TIMGPIOPinType::kETR>;
        using BKINPinT  = PinDeviceT<TIMGPIOPinType::kBKIN>;
        using BKIN2PinT = PinDeviceT<TIMGPIOPinType::kBKIN2>;

        // Check availability
        template <TIMGPIOPinType tkPinType>
        static constexpr bool skHasPin = !CNullGPIOPinDevice<PinDeviceT<tkPinType>>;

        static constexpr bool skHasCh1Pin   = skHasPin<TIMGPIOPinType::kCh1>;
        static constexpr bool skHasCh2Pin   = skHasPin<TIMGPIOPinType::kCh2>;
        static constexpr bool skHasCh3Pin   = skHasPin<TIMGPIOPinType::kCh3>;
        static constexpr bool skHasCh4Pin   = skHasPin<TIMGPIOPinType::kCh4>;
        static constexpr bool skHasCh1NPin  = skHasPin<TIMGPIOPinType::kCh1N>;
        static constexpr bool skHasCh2NPin  = skHasPin<TIMGPIOPinType::kCh2N>;
        static constexpr bool skHasCh3NPin  = skHasPin<TIMGPIOPinType::kCh3N>;
        static constexpr bool skHasCh4NPin  = skHasPin<TIMGPIOPinType::kCh4N>;
        static constexpr bool skHasETRPin   = skHasPin<TIMGPIOPinType::kETR>;
        static constexpr bool skHasBKINPin  = skHasPin<TIMGPIOPinType::kBKIN>;
        static constexpr bool skHasBKIN2Pin = skHasPin<TIMGPIOPinType::kBKIN2>;

        template <TIMGPIOPinType tkPinType>
        using PinDriverT = TIMGPIOPinDriver<tkControllerID, tkPinType, PinDeviceT<tkPinType>>;

        using Ch1PinDriverT   = PinDriverT<TIMGPIOPinType::kCh1>;
        using Ch2PinDriverT   = PinDriverT<TIMGPIOPinType::kCh2>;
        using Ch3PinDriverT   = PinDriverT<TIMGPIOPinType::kCh3>;
        using Ch4PinDriverT   = PinDriverT<TIMGPIOPinType::kCh4>;
        using Ch1NPinDriverT  = PinDriverT<TIMGPIOPinType::kCh1N>;
        using Ch2NPinDriverT  = PinDriverT<TIMGPIOPinType::kCh2N>;
        using Ch3NPinDriverT  = PinDriverT<TIMGPIOPinType::kCh3N>;
        using Ch4NPinDriverT  = PinDriverT<TIMGPIOPinType::kCh4N>;
        using ETRPinDriverT   = PinDriverT<TIMGPIOPinType::kETR>;
        using BKINPinDriverT  = PinDriverT<TIMGPIOPinType::kBKIN>;
        using BKIN2PinDriverT = PinDriverT<TIMGPIOPinType::kBKIN2>;

        using DependDevices = TypeList<TIMRootDevice>;
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
        TIMControllerDevice() = delete;

        template <typename... TArgs>
        explicit TIMControllerDevice(TArgs&&... args)
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
            delay_cycles(10U);  // NOLINT(readability-magic-numbers)

            return true;
        }

        template <TIMGPIOPinType tkPinType>
            requires(skHasPin<tkPinType>)
        [[nodiscard]] bool init_pin(const GPIOAlternativeFunctionConfig& pin_cfg)
        {
            return get_pin_driver<tkPinType>().init(pin_cfg);
        }

        template <TIMChannelID tkChannel>
        [[nodiscard]] bool init_channel_output_capture(const TIMChannelOutputCaptureConfig& config)
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
        }

        template <TIMChannelID tkChannel>
        [[nodiscard]] bool init_channel_input_capture(const TIMChannelInputCaptureConfig& config)
        {
            using CInterfaceT = ChannelInterfaceT<tkChannel>;

            CInterfaceT::set_input_capture_active_input(config.active_input);
            CInterfaceT::set_input_capture_prescaler(config.prescaler);
            CInterfaceT::set_input_capture_filter(config.filter);
            CInterfaceT::set_input_capture_polarity(config.polarity);
        }

        [[nodiscard]] bool init_encoder_gpio(const GPIOAlternativeFunctionConfig& gpio_config)
        {
            return init_pin<TIMGPIOPinType::kCh1>(gpio_config) && init_pin<TIMGPIOPinType::kCh2>(gpio_config);
        }

        [[nodiscard]] bool init_encoder(const TIMControllerEncoderConfig& config)
        {
            InterfaceT::set_encoder_mode(config.mode);
            init_channel_input_capture<TIMChannelID::kCh1>(config.ch_config);

            if (config.ch_config.polarity == config.ch2_polarity)
            {
                init_channel_input_capture<TIMChannelID::kCh2>(config.ch_config);
            }
            else
            {
                TIMChannelInputCaptureConfig ch2_config = config.ch_config;
                ch2_config.polarity                     = config.ch2_polarity;
                init_channel_input_capture<TIMChannelID::kCh2>(ch2_config);
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

        template <TIMChannelID tkChannel>
        void enable_channel()
        {
            ChannelInterfaceT<tkChannel>::enable();
        }

        template <TIMChannelID tkChannel>
        void disable_channel()
        {
            ChannelInterfaceT<tkChannel>::disable();
        }

        template <TIMChannelID tkChannel>
        void enable_channel_complementary()
        {
            ChannelInterfaceT<tkChannel>::enable_complementary();
        }

        template <TIMChannelID tkChannel>
        void disable_channel_complementary()
        {
            ChannelInterfaceT<tkChannel>::disable_complementary();
        }

        [[nodiscard]] CounterValueT get_counter() const
        {
            return InterfaceT::get_counter();
        }

    private:
        template <TIMGPIOPinType tkPinType>
            requires(skHasPin<tkPinType>)
        [[nodiscard]] constexpr PinDriverT<tkPinType>& get_pin_driver()
        {
            if constexpr (tkPinType == TIMGPIOPinType::kCh1)
            {
                return m_ch1_pin;
            }

            if constexpr (tkPinType == TIMGPIOPinType::kCh2)
            {
                return m_ch2_pin;
            }

            if constexpr (tkPinType == TIMGPIOPinType::kCh3)
            {
                return m_ch3_pin;
            }

            if constexpr (tkPinType == TIMGPIOPinType::kCh4)
            {
                return m_ch4_pin;
            }

            if constexpr (tkPinType == TIMGPIOPinType::kCh1N)
            {
                return m_ch1n_pin;
            }

            if constexpr (tkPinType == TIMGPIOPinType::kCh2N)
            {
                return m_ch2n_pin;
            }

            if constexpr (tkPinType == TIMGPIOPinType::kCh3N)
            {
                return m_ch3n_pin;
            }

            if constexpr (tkPinType == TIMGPIOPinType::kCh4N)
            {
                return m_ch4n_pin;
            }

            if constexpr (tkPinType == TIMGPIOPinType::kETR)
            {
                return m_etr_pin;
            }

            if constexpr (tkPinType == TIMGPIOPinType::kBKIN)
            {
                return m_bkin_pin;
            }

            if constexpr (tkPinType == TIMGPIOPinType::kBKIN2)
            {
                return m_bkin2_pin;
            }
        }
    };

    // ---------------------------------------------------------------------------
    // DEVICE ALIASES
    // ---------------------------------------------------------------------------

    using TIM1ControllerDevice  = TIMControllerDevice<TIMControllerID::kTim1>;
    using TIM2ControllerDevice  = TIMControllerDevice<TIMControllerID::kTim2>;
    using TIM3ControllerDevice  = TIMControllerDevice<TIMControllerID::kTim3>;
    using TIM4ControllerDevice  = TIMControllerDevice<TIMControllerID::kTim4>;
    using TIM5ControllerDevice  = TIMControllerDevice<TIMControllerID::kTim5>;
    using TIM6ControllerDevice  = TIMControllerDevice<TIMControllerID::kTim6>;
    using TIM7ControllerDevice  = TIMControllerDevice<TIMControllerID::kTim7>;
    using TIM8ControllerDevice  = TIMControllerDevice<TIMControllerID::kTim8>;
    using TIM15ControllerDevice = TIMControllerDevice<TIMControllerID::kTim15>;
    using TIM16ControllerDevice = TIMControllerDevice<TIMControllerID::kTim16>;
    using TIM17ControllerDevice = TIMControllerDevice<TIMControllerID::kTim17>;
    using TIM20ControllerDevice = TIMControllerDevice<TIMControllerID::kTim20>;

    // =============================================================================
    // MULTI-CHANNEL CONTROLLER DEVICE (SHARED DEVICE)
    // Consumes the Unique Controller to allow multiple Channel Devices to coexist.
    // =============================================================================

    template <TIMControllerID tkControllerID, bool tkShared = true>
    class TIMMultiChannelControllerDevice
    {
    public:
        struct Descriptor : public std::conditional_t<tkShared, SharedDeviceDescriptor, UniqueDeviceDescriptor>
        {
        };
        using ControllerT   = TIMControllerDevice<tkControllerID>;
        using InjectDevices = TypeList<ControllerT>;

    private:
        DeviceRef<ControllerT> m_controller;

    public:
        TIMMultiChannelControllerDevice() = delete;

        explicit TIMMultiChannelControllerDevice(DeviceRef<ControllerT>&& controller)
            : m_controller(std::move(controller))
        {
        }

        [[nodiscard]] inline bool init()
        {
            return m_controller->init();
        }

        [[nodiscard]] inline bool post_init()
        {
            return true;
        }
    };

    // =============================================================================
    // TIMER CHANNEL DEVICE (UNIQUE DEVICE)
    // Manages a single CCR register and its associated CHx/CHxN pins.
    // =============================================================================

    struct TIMChannelCTConfig
    {
        using ChPinT  = GPIONullPinDevice;
        using ChNPinT = GPIONullPinDevice;
    };

    template <TIMControllerID tkControllerID, TIMChannelID tkChannel>
    class TIMChannelDevice
    {
    public:
        struct Descriptor : public UniqueDeviceDescriptor
        {
        };

        using MultiControllerT = TIMMultiChannelControllerDevice<tkControllerID, true>;

        // Pins for this specific channel
        using ChPinT  = typename TIMChannelCTConfig::ChPinT;
        using ChNPinT = typename TIMChannelCTConfig::ChNPinT;

        using InjectDevices = FilterNullDevices<TypeList<MultiControllerT, ChPinT, ChNPinT>>;

    private:
        DeviceRef<MultiControllerT> m_multi;

    public:
        TIMChannelDevice() = delete;

        explicit TIMChannelDevice(DeviceRef<MultiControllerT>&& multi) : m_multi(std::move(multi))
        {
        }

        [[nodiscard]] inline bool init()
        {
            return true;
        }
    };

}  // namespace valle