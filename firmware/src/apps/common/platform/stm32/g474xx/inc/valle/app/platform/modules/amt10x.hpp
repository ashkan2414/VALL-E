#pragma once

#include "valle/app/modules/amt10x.hpp"
#include "valle/platform/drivers/gpio/digital_in.hpp"
#include "valle/platform/drivers/tim/quad_encoder.hpp"

namespace valle::platform::app
{
    using AMT10xModuleTimEncoderInterfaceConfig = TimQuadEncoderConfig;

    /**
     * @brief Platform implementation of AMT10x encoder interface using STM32 Timer Encoder Mode.
     *
     * @tparam TControllerDevice The Timer controller device (e.g. Tim1, Tim2, etc.).
     * @tparam tkPPR The PPR setting for the encoder.
     */
    template <typename TControllerDevice, typename TIndexGpioPinDevice, valle::app::AMT10xPPR tkPPR>
    class AMT10xModuleTimEncoderInterface
        : public valle::app::AMT10xModuleEncoderInterfaceX<
              AMT10xModuleTimEncoderInterface<TControllerDevice, TIndexGpioPinDevice, tkPPR>,
              TimQuadEncoderConfig>
    {
    public:
        using ControllerT         = TControllerDevice;
        using IndexGpioPinDeviceT = TIndexGpioPinDevice;

        using EncoderDriverT            = TimQuadEncoderDriver<ControllerT>;
        using IndexGpioDigitalInDriverT = GpioDigitalInDriver<IndexGpioPinDeviceT>;
        using ConfigT                   = TimQuadEncoderConfig;
        using CounterValueT             = typename EncoderDriverT::CounterValueT;

        using InjectDevices = TypeList<ControllerT, IndexGpioPinDeviceT>;

    private:
        EncoderDriverT            m_driver{};
        IndexGpioDigitalInDriverT m_index_pin{};
        uint32_t                  m_counts_per_pulse{4};

    public:
        explicit AMT10xModuleTimEncoderInterface(DeviceRef<ControllerT>&&         controller,
                                                 DeviceRef<IndexGpioPinDeviceT>&& index_pin)
            : m_driver(std::move(controller)), m_index_pin(std::move(index_pin))
        {
        }

        [[nodiscard]] bool init_impl(const ConfigT& config)
        {
            if (!m_index_pin.init(GpioDigitalInConfig{
                    .pull      = GpioPullMode::kNoPull,
                    .interrupt = std::nullopt,
                    .inverted  = false,
                }))
            {
                return false;
            }

            switch (config.encoder_config.mode)
            {
                case TimControllerEncoderMode::kX4TimerInput12:
                    m_counts_per_pulse = 4;
                    break;

                case TimControllerEncoderMode::kX2TimerInput1:
                case TimControllerEncoderMode::kX2TimerInput2:
                case TimControllerEncoderMode::kClockPlusDirectionX2:
                case TimControllerEncoderMode::kDirectionalClockX2:
                    m_counts_per_pulse = 2;
                    break;

                case TimControllerEncoderMode::kX1TimerInput1:
                case TimControllerEncoderMode::kX1TimerInput2:
                case TimControllerEncoderMode::kClockPlusDirectionX1:
                case TimControllerEncoderMode::kDirectionalClockX1TimerInput12:
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

        void set_count_impl(const CounterValueT count)
        {
            m_driver.set_count(count);
        }

        [[nodiscard]] bool read_index_channel_impl() const
        {
            return m_index_pin.read();
        }

        [[nodiscard]] EncoderDriverT& get_driver()
        {
            return m_driver;
        }
    };

    using AMT10xTimEncoderModuleConfig = valle::app::AMT10xModuleConfigX<AMT10xModuleTimEncoderInterfaceConfig>;

    template <typename TControllerDevice, typename TIndexGpioPinDevice, valle::app::AMT10xPPR tkPPR>
    using AMT10xTimEncoderModule =
        valle::app::AMT10xModuleX<AMT10xModuleTimEncoderInterface<TControllerDevice, TIndexGpioPinDevice, tkPPR>,
                                  tkPPR>;

}  // namespace valle::platform::app
