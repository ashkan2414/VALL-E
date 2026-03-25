#pragma once

#include "stm32g4xx_ll_tim.h"
#include "valle/platform/core.hpp"
#include "valle/platform/devices/gpio.hpp"
#include "valle/platform/drivers/gpio/alternate_function.hpp"
#include "valle/platform/hardware/tim.hpp"

namespace valle::platform
{
    // =============================================================================
    // TIMER CONTROLLER DEVICE (SHARED DEVICE)
    // =============================================================================

    // -----------------------------------------------------------------------------
    // COMPILE TimE CONFIGURATIONS
    // -----------------------------------------------------------------------------
    struct TimControllerCTDefaultConfig
    {
        using Ch1PinT   = GpioNullPinDevice;
        using Ch2PinT   = GpioNullPinDevice;
        using Ch3PinT   = GpioNullPinDevice;
        using Ch4PinT   = GpioNullPinDevice;
        using Ch1NPinT  = GpioNullPinDevice;
        using Ch2NPinT  = GpioNullPinDevice;
        using Ch3NPinT  = GpioNullPinDevice;
        using Ch4NPinT  = GpioNullPinDevice;
        using ETRPinT   = GpioNullPinDevice;
        using BKINPinT  = GpioNullPinDevice;
        using BKIN2PinT = GpioNullPinDevice;
    };

    template <typename T, TimPeripheralId tkPeripheralId>
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
        (CNullGpioPinDevice<typename T::Ch1PinT> ||
         CValidTimPin<tkPeripheralId, TimGpioPinType::kCh1, T::Ch1PinT::skPortId, T::Ch1PinT::skPinId>) &&
        (CNullGpioPinDevice<typename T::Ch2PinT> ||
         CValidTimPin<tkPeripheralId, TimGpioPinType::kCh2, T::Ch2PinT::skPortId, T::Ch2PinT::skPinId>) &&
        (CNullGpioPinDevice<typename T::Ch3PinT> ||
         CValidTimPin<tkPeripheralId, TimGpioPinType::kCh3, T::Ch3PinT::skPortId, T::Ch3PinT::skPinId>) &&
        (CNullGpioPinDevice<typename T::Ch4PinT> ||
         CValidTimPin<tkPeripheralId, TimGpioPinType::kCh4, T::Ch4PinT::skPortId, T::Ch4PinT::skPinId>) &&
        (CNullGpioPinDevice<typename T::Ch1NPinT> ||
         CValidTimPin<tkPeripheralId, TimGpioPinType::kCh1N, T::Ch1NPinT::skPortId, T::Ch1NPinT::skPinId>) &&
        (CNullGpioPinDevice<typename T::Ch2NPinT> ||
         CValidTimPin<tkPeripheralId, TimGpioPinType::kCh2N, T::Ch2NPinT::skPortId, T::Ch2NPinT::skPinId>) &&
        (CNullGpioPinDevice<typename T::Ch3NPinT> ||
         CValidTimPin<tkPeripheralId, TimGpioPinType::kCh3N, T::Ch3NPinT::skPortId, T::Ch3NPinT::skPinId>) &&
        (CNullGpioPinDevice<typename T::Ch4NPinT> ||
         CValidTimPin<tkPeripheralId, TimGpioPinType::kCh4N, T::Ch4NPinT::skPortId, T::Ch4NPinT::skPinId>) &&
        (CNullGpioPinDevice<typename T::ETRPinT> ||
         CValidTimPin<tkPeripheralId, TimGpioPinType::kETR, T::ETRPinT::skPortId, T::ETRPinT::skPinId>) &&
        (CNullGpioPinDevice<typename T::BKINPinT> ||
         CValidTimPin<tkPeripheralId, TimGpioPinType::kBKIN, T::BKINPinT::skPortId, T::BKINPinT::skPinId>) &&
        (CNullGpioPinDevice<typename T::BKIN2PinT> ||
         CValidTimPin<tkPeripheralId, TimGpioPinType::kBKIN2, T::BKIN2PinT::skPortId, T::BKIN2PinT::skPinId>);

    template <TimPeripheralId tkPeripheralId>
    struct TimControllerCTConfigRegistry
    {
        static constexpr auto skConfig = TimControllerCTDefaultConfig{};
    };

#define VALLE_DEFINE_TIMER_CONTROLLER_CT_CONFIG(tkPeripheralId, config)                      \
    namespace valle::platform                                                                \
    {                                                                                        \
        template <>                                                                          \
        struct TimControllerCTConfigRegistry<(tkPeripheralId)>                               \
        {                                                                                    \
            static constexpr auto skConfig = (config);                                       \
            static_assert(CValidTimControllerCTConfig<decltype(skConfig), (tkPeripheralId)>, \
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
        template <TimPeripheralId tkPeripheralId, TimGpioPinType tkPinType, typename TPinDevice>
        struct TimGpioPinDriverHelper
        {
            using type = GpioAlternateFunctionDriver<
                TPinDevice,
                kTimPinAF<tkPeripheralId, tkPinType, TPinDevice::skPortId, TPinDevice::skPinId>>;
        };

        template <TimPeripheralId tkPeripheralId, TimGpioPinType tkPinType>
        struct TimGpioPinDriverHelper<tkPeripheralId, tkPinType, GpioNullPinDevice>
        {
            using type = std::monostate;
        };
    }  // namespace detail

    template <TimPeripheralId tkPeripheralId, TimGpioPinType tkPinType, typename TPinDevice>
    using TimGpioPinDriver = typename detail::TimGpioPinDriverHelper<tkPeripheralId, tkPinType, TPinDevice>::type;

    // -----------------------------------------------------------------------------
    // DEVICE CLASS
    // -----------------------------------------------------------------------------

    template <TimPeripheralId tkPeripheralId>
    class TimControllerDevice
    {
    public:
        struct Descriptor : public UniqueDeviceDescriptor
        {
        };

        static constexpr TimPeripheralId skPeripheralId = tkPeripheralId;

        using TraitsT       = TimControllerTraits<tkPeripheralId>;
        using InterfaceT    = TimControllerInterface<tkPeripheralId>;
        using CounterValueT = typename TraitsT::CounterValueT;

        template <TimChannelId tkChannelId>
        using ChannelInterfaceT = TimChannelInterface<tkPeripheralId, tkChannelId>;

        using CTConfigRegistryT          = TimControllerCTConfigRegistry<tkPeripheralId>;
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
        static constexpr bool skHasPin = !CNullGpioPinDevice<PinDeviceT<tkPinType>>;

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
        using PinDriverT = TimGpioPinDriver<tkPeripheralId, tkPinType, PinDeviceT<tkPinType>>;

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
        TimControllerDevice() = delete;

        template <typename... TArgs>
        explicit TimControllerDevice(TArgs&&... args)
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

    using Tim1ControllerDevice  = TimControllerDevice<TimPeripheralId::kTim1>;
    using Tim2ControllerDevice  = TimControllerDevice<TimPeripheralId::kTim2>;
    using Tim3ControllerDevice  = TimControllerDevice<TimPeripheralId::kTim3>;
    using Tim4ControllerDevice  = TimControllerDevice<TimPeripheralId::kTim4>;
    using Tim5ControllerDevice  = TimControllerDevice<TimPeripheralId::kTim5>;
    using Tim6ControllerDevice  = TimControllerDevice<TimPeripheralId::kTim6>;
    using Tim7ControllerDevice  = TimControllerDevice<TimPeripheralId::kTim7>;
    using Tim8ControllerDevice  = TimControllerDevice<TimPeripheralId::kTim8>;
    using Tim15ControllerDevice = TimControllerDevice<TimPeripheralId::kTim15>;
    using Tim16ControllerDevice = TimControllerDevice<TimPeripheralId::kTim16>;
    using Tim17ControllerDevice = TimControllerDevice<TimPeripheralId::kTim17>;
    using Tim20ControllerDevice = TimControllerDevice<TimPeripheralId::kTim20>;

    // =============================================================================
    // MULTI-CHANNEL CONTROLLER DEVICE (SHARED DEVICE)
    // Consumes the Unique Controller to allow multiple Channel Devices to coexist.
    // =============================================================================

    template <TimPeripheralId tkPeripheralId, bool tkShared = true>
    class TimMultiChannelControllerDevice
    {
    public:
        struct Descriptor : public std::conditional_t<tkShared, SharedDeviceDescriptor, UniqueDeviceDescriptor>
        {
        };
        using ControllerT   = TimControllerDevice<tkPeripheralId>;
        using InjectDevices = TypeList<ControllerT>;

    private:
        DeviceRef<ControllerT> m_controller;

    public:
        TimMultiChannelControllerDevice() = delete;

        explicit TimMultiChannelControllerDevice(DeviceRef<ControllerT>&& controller)
            : m_controller(std::move(controller))
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
        using ChPinT  = GpioNullPinDevice;
        using ChNPinT = GpioNullPinDevice;
    };

    template <TimPeripheralId tkPeripheralId, TimChannelId tkChannel>
    class TimChannelDevice
    {
    public:
        struct Descriptor : public UniqueDeviceDescriptor
        {
        };

        using MultiControllerT = TimMultiChannelControllerDevice<tkPeripheralId, true>;

        // Pins for this specific channel
        using ChPinT  = typename TimChannelCTConfig::ChPinT;
        using ChNPinT = typename TimChannelCTConfig::ChNPinT;

        using InjectDevices = FilterNullDevices<TypeList<MultiControllerT, ChPinT, ChNPinT>>;

    private:
        DeviceRef<MultiControllerT> m_multi;

    public:
        TimChannelDevice() = delete;

        explicit TimChannelDevice(DeviceRef<MultiControllerT>&& multi) : m_multi(std::move(multi))
        {
        }

        [[nodiscard]] inline bool init()
        {
            return true;
        }
    };

}  // namespace valle::platform