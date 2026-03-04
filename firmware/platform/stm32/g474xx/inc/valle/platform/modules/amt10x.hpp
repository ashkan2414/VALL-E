#pragma once

#include "valle/modules/amt10x.hpp"
#include "valle/platform/drivers/tim/quad_encoder.hpp"

namespace valle
{
    using AMT10xModuleTIMEncoderInterfaceConfig = TIMQuadEncoderConfig;

    /**
     * @brief Platform implementation of AMT10x encoder interface using STM32 Timer Encoder Mode.
     *
     * @tparam TControllerDevice The Timer controller device (e.g. TIM1, TIM2, etc.).
     * @tparam tkPPR The PPR setting for the encoder.
     */
    template <typename TControllerDevice, AMT10xPPR tkPPR>
    class AMT10xModuleTIMEncoderInterface
        : public AMT10xModuleEncoderInterfaceX<AMT10xModuleTIMEncoderInterface<TControllerDevice, tkPPR>,
                                               TIMQuadEncoderConfig>
    {
    public:
        using ControllerT    = TControllerDevice;
        using EncoderDriverT = TIMQuadEncoderDriver<ControllerT>;
        using ConfigT        = TIMQuadEncoderConfig;

        using InjectDevices = TypeList<ControllerT>;

    private:
        EncoderDriverT m_driver{};
        uint32_t       m_counts_per_pulse{4};

    public:
        template <typename... TArgs>
        explicit AMT10xModuleTIMEncoderInterface(TArgs&&... args) : m_driver(std::forward<TArgs>(args)...)
        {
        }

        [[nodiscard]] bool init_impl(const ConfigT& config)
        {
            switch (config.encoder_config.mode)
            {
                case TIMControllerEncoderMode::kX4TimerInput12:
                    m_counts_per_pulse = 4;
                    break;

                case TIMControllerEncoderMode::kX2TimerInput1:
                case TIMControllerEncoderMode::kX2TimerInput2:
                case TIMControllerEncoderMode::kClockPlusDirectionX2:
                case TIMControllerEncoderMode::kDirectionalClockX2:
                    m_counts_per_pulse = 2;
                    break;

                case TIMControllerEncoderMode::kX1TimerInput1:
                case TIMControllerEncoderMode::kX1TimerInput2:
                case TIMControllerEncoderMode::kClockPlusDirectionX1:
                case TIMControllerEncoderMode::kDirectionalClockX1TimerInput12:
                    m_counts_per_pulse = 1;
                    break;

                default:
                    m_counts_per_pulse = 0;
                    return false;
            }

            return m_driver.init(config);
        }

        void enable_impl()
        {
            m_driver.enable();
        }

        void disable_impl()
        {
            m_driver.disable();
        }

        [[nodiscard]] auto get_count_impl() const
        {
            return m_driver.get_count();
        }

        [[nodiscard]] auto get_counts_per_pulse_impl() const
        {
            return m_counts_per_pulse;
        }

        void reset_count_impl()
        {
            m_driver.set_count(0);
        }

        [[nodiscard]] EncoderDriverT& get_driver()
        {
            return m_driver;
        }
    };

    using AMT10xTIMEncoderModuleConfig = AMT10xModuleConfigX<AMT10xModuleTIMEncoderInterfaceConfig>;

    template <typename TControllerDevice, AMT10xPPR tkPPR>
    using AMT10xTIMEncoderModule = AMT10xModuleX<AMT10xModuleTIMEncoderInterface<TControllerDevice, tkPPR>, tkPPR>;

}  // namespace valle
