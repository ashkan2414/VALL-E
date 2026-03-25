#pragma once

#include "valle/app/modules/acs724.hpp"
#include "valle/platform/drivers/adc/analog_sensor.hpp"

namespace valle::platform::app
{
    using ACS724ModuleAdcChannelInterfaceConfig = AdcChannelConfig;

    /**
     * @brief ACS724 Current Sensor Driver.
     *
     * @tparam TAdcChannel ADC Channel Device type.
     * @tparam tkModel   ACS724 Model type.
     */
    template <typename TAdcChannel, valle::app::ACS724Model tkModel>
    class ACS724ModuleAdcChannelInterface
        : public valle::app::ACS724ModuleAdcChannelInterfaceX<ACS724ModuleAdcChannelInterface<TAdcChannel, tkModel>,
                                                              ACS724ModuleAdcChannelInterfaceConfig>
    {
    public:
        using ConfigT                                    = ACS724ModuleAdcChannelInterfaceConfig;
        using ChannelT                                   = TAdcChannel;
        static constexpr valle::app::ACS724Model skModel = tkModel;

        using InjectDevices = TypeList<ChannelT>;
        using ModelTraits   = valle::app::ACS724Traits<tkModel>;

    private:
        using ConverterT = AdcNormalizedConverter<LinearConverter<float>>;

        static constexpr LinearConverterConfig skConverterConfig = {
            .scale  = ModelTraits::skRangeAmps,
            .offset = ModelTraits::skMinAmps,
        };

        AdcAnalogSensorDriver<ChannelT, ConverterT> m_driver;

    public:
        ACS724ModuleAdcChannelInterface() = delete;

        ACS724ModuleAdcChannelInterface(DeviceRef<ChannelT>&& channel) : m_driver(std::move(channel))
        {
        }

        // ------------------------------------------------------------------------
        // Initialization
        // ------------------------------------------------------------------------
        /**
         * @brief Initialize ACS724 Module.
         *
         * @param config Configuration structure.
         */
        [[nodiscard]] bool init_impl(const ACS724ModuleAdcChannelInterfaceConfig& config)
        {
            return m_driver.init(AdcAnalogSensorDriverConfig<ConverterT>{.channel_config   = config,
                                                                         .converter_config = skConverterConfig});
        }

        // ------------------------------------------------------------------------
        // Control Interface
        // ------------------------------------------------------------------------

        /**
         * @brief Read the converted current value.
         *
         * @return float Current in Amps.
         */
        [[nodiscard]] inline float read_amps_impl() const
        {
            return m_driver.read();
        }

        /**
         * @brief Notify that new data is available (to be called from ADC ISR).
         *
         */
        void on_data_available()
        {
            m_driver.on_data_available();
        }
    };

    using ACS724ModuleConfig = valle::app::ACS724ModuleConfigX<ACS724ModuleAdcChannelInterfaceConfig>;

    template <typename TAdcChannel, valle::app::ACS724Model tkModel>
    using ACS724Module = valle::app::ACS724ModuleX<ACS724ModuleAdcChannelInterface<TAdcChannel, tkModel>, tkModel>;

}  // namespace valle::platform::app