#pragma once

#include "valle/base/modules/acs724.hpp"
#include "valle/platform/drivers/adc/analog_sensor.hpp"

namespace valle::platform
{
    using ACS724ModuleADCChannelInterfaceConfig = ADCChannelConfig;

    /**
     * @brief ACS724 Current Sensor Driver.
     *
     * @tparam TADCChannel ADC Channel Device type.
     * @tparam tkModel   ACS724 Model type.
     */
    template <typename TADCChannel, ACS724Model tkModel>
    class ACS724ModuleADCChannelInterface
        : public ACS724ModuleADCChannelInterfaceX<ACS724ModuleADCChannelInterface<TADCChannel, tkModel>,
                                                  ACS724ModuleADCChannelInterfaceConfig>
    {
    public:
        using ConfigT                        = ACS724ModuleADCChannelInterfaceConfig;
        using ChannelT                       = TADCChannel;
        static constexpr ACS724Model skModel = tkModel;

        using InjectDevices = TypeList<ChannelT>;
        using ModelTraits   = ACS724Traits<tkModel>;

    private:
        using ConverterT = ADCNormalizedConverter<LinearConverter<float>>;

        static constexpr LinearConverter<float>::Config skConverterConfig = {
            .scale  = ModelTraits::skRangeAmps,
            .offset = ModelTraits::skMinAmps,
        };

        ADCAnalogSensorDriver<ChannelT, ConverterT> m_driver;

    public:
        ACS724ModuleADCChannelInterface() = delete;

        ACS724ModuleADCChannelInterface(DeviceRef<ChannelT>&& channel) : m_driver(std::move(channel))
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
        [[nodiscard]] bool init_impl(const ACS724ModuleADCChannelInterfaceConfig& config)
        {
            return m_driver.init(ADCAnalogSensorDriverConfig<ConverterT>{.channel_config   = config,
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

    using ACS724ModuleConfig = ACS724ModuleConfigX<ACS724ModuleADCChannelInterfaceConfig>;

    template <typename TADCChannel, ACS724Model tkModel>
    using ACS724Module = ACS724ModuleX<ACS724ModuleADCChannelInterface<TADCChannel, tkModel>, tkModel>;

}  // namespace valle::platform