#pragma once

#include "valle/platform/devices/tim.hpp"

namespace valle::platform
{
    struct TIMQuadEncoderConfig
    {
        GPIOAlternativeFunctionConfig gpio_config{};     // GPIO config for encoder pins
        TIMControllerEncoderConfig    encoder_config{};  // TIM Encoder mode config (input capture settings)
    };

    /**
     * @brief Driver for timer quadrature encoder mode. This mode uses two input channels to decode quadrature signals
     * from an encoder.
     */
    template <typename TControllerDevice>
    class TIMQuadEncoderDriver
    {
    public:
        using ControllerT                               = TControllerDevice;
        static constexpr TIMControllerID skControllerID = ControllerT::skControllerID;

        using TraitsT       = typename ControllerT::TraitsT;
        using InterfaceT    = TIMControllerInterface<skControllerID>;
        using CounterValueT = typename TraitsT::CounterValueT;

        using InjectDevices = TypeList<ControllerT>;

        static_assert(ControllerT::skHasCh1Pin, "TIMQuadEncoderDriver requires CH1 pin");
        static_assert(ControllerT::skHasCh2Pin, "TIMQuadEncoderDriver requires CH2 pin");

    private:
        DeviceRef<ControllerT> m_controller;

    public:
        TIMQuadEncoderDriver() = delete;

        template <typename... TArgs>
        explicit TIMQuadEncoderDriver(DeviceRef<ControllerT>&& controller) : m_controller(std::move(controller))
        {
        }

        [[nodiscard]] bool init(const TIMQuadEncoderConfig& config)
        {
            if (!m_controller->init())
            {
                return false;
            }

            // Initialize Pins
            if (!m_controller->init_encoder_gpio(config.gpio_config))
            {
                return false;
            }

            // Initialize Encoder mode with input capture settings
            if (!m_controller->template init_encoder(config.encoder_config))
            {
                return false;
            }

            return true;
        }

        void enable()
        {
            m_controller->enable_counter();
        }

        void disable()
        {
            m_controller->disable_counter();
        }

        /**
         * @brief Returns raw counter value.
         * Note: On TIM2/TIM5 this is 32-bit. Others 16-bit.
         */
        [[nodiscard]] CounterValueT get_count() const
        {
            return m_controller->get_counter();
        }

        /**
         * @brief Sets raw counter value.
         */
        void set_count(const CounterValueT val)
        {
            InterfaceT::set_counter(val);
        }
    };
}  // namespace valle::platform