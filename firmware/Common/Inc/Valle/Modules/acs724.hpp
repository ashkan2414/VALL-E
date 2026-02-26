#pragma once

#include "Valle/Board/Drivers/adc.hpp"

namespace valle
{

    /**
     * @brief ACS724 models.
     *
     */
    enum class ACS724Model : uint8_t
    {
        k2P5ABi = 0,  // 2P5AB-S ±2.5
        k05AUni,      // 05AU-S 5
        k05ABi,       // 05AB-S ±5
        k10AUni,      // 10AU-S 10
        k10ABi,       // 10AB-S ±10
        k20AUni,      // 20AU-S 20
        k20ABi,       // 20AB-S ±20
        k30AUni,      // 30AU-S 30
        k30ABi,       // 30AB-S ±30
        k50ABi,       // 50AB-S ±50
    };

    // ============================================================================
    // ACS724 Model Traits
    // ============================================================================

    template <ACS724Model tkModel>
    struct ACS724Traits;

    template <>
    struct ACS724Traits<ACS724Model::k2P5ABi>
    {
        static constexpr bool  skBiDirectional = true;
        static constexpr float skRangeAmps     = 5.0F;
        static constexpr float skMinAmps       = -2.5F;
        static constexpr float skMaxAmps       = 2.5F;
    };

    template <>
    struct ACS724Traits<ACS724Model::k05AUni>
    {
        static constexpr bool  skBiDirectional = false;
        static constexpr float skRangeAmps     = 5.0F;
        static constexpr float skMinAmps       = 0.0F;
        static constexpr float skMaxAmps       = 5.0F;
    };

    template <>
    struct ACS724Traits<ACS724Model::k05ABi>
    {
        static constexpr bool  skBiDirectional = true;
        static constexpr float skRangeAmps     = 10.0F;
        static constexpr float skMinAmps       = -5.0F;
        static constexpr float skMaxAmps       = 5.0F;
    };

    template <>
    struct ACS724Traits<ACS724Model::k10AUni>
    {
        static constexpr bool  skBiDirectional = false;
        static constexpr float skRangeAmps     = 10.0F;
        static constexpr float skMinAmps       = 0.0F;
        static constexpr float skMaxAmps       = 10.0F;
    };

    template <>
    struct ACS724Traits<ACS724Model::k10ABi>
    {
        static constexpr bool  skBiDirectional = true;
        static constexpr float skRangeAmps     = 20.0F;
        static constexpr float skMinAmps       = -10.0F;
        static constexpr float skMaxAmps       = 10.0F;
    };

    template <>
    struct ACS724Traits<ACS724Model::k20AUni>
    {
        static constexpr bool  skBiDirectional = false;
        static constexpr float skRangeAmps     = 20.0F;
        static constexpr float skMinAmps       = 0.0F;
        static constexpr float skMaxAmps       = 20.0F;
    };

    template <>
    struct ACS724Traits<ACS724Model::k20ABi>
    {
        static constexpr bool  skBiDirectional = true;
        static constexpr float skRangeAmps     = 40.0F;
        static constexpr float skMinAmps       = -20.0F;
        static constexpr float skMaxAmps       = 20.0F;
    };

    template <>
    struct ACS724Traits<ACS724Model::k30AUni>
    {
        static constexpr bool  skBiDirectional = false;
        static constexpr float skRangeAmps     = 30.0F;
        static constexpr float skMinAmps       = 0.0F;
        static constexpr float skMaxAmps       = 30.0F;
    };

    template <>
    struct ACS724Traits<ACS724Model::k30ABi>
    {
        static constexpr bool  skBiDirectional = true;
        static constexpr float skRangeAmps     = 60.0F;
        static constexpr float skMinAmps       = -30.0F;
        static constexpr float skMaxAmps       = 30.0F;
    };

    // ============================================================================
    // ACS724 Module Drivers
    // ============================================================================

    struct ACS724Config
    {
        ADCChannelConfig channel_config{};  /// ADC Channel Configuration
    };

    /**
     * @brief ACS724 Current Sensor Driver.
     *
     * @tparam TADCChannel ADC Channel Device type.
     * @tparam tkModel   ACS724 Model type.
     */
    template <typename TADCChannel, ACS724Model tkModel>
    class ACS724Module
    {
    public:
        using ChannelT      = TADCChannel;
        using InjectDevices = TypeList<TADCChannel>;
        using ModelTraits   = ACS724Traits<tkModel>;

        static constexpr ACS724Model skModel = tkModel;

    private:
        using ConverterT = ADCNormalizedConverter<LinearConverter<float>>;

        static constexpr LinearConverter<float>::Config skConverterConfig = {
            .scale  = ModelTraits::skRangeAmps,
            .offset = ModelTraits::skMinAmps,
        };

        ADCSensorDriver<TADCChannel, ConverterT> m_driver;

    public:
        ACS724Module() = delete;

        ACS724Module(DeviceRef<TADCChannel>&& channel) : m_driver(std::move(channel))
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
        [[nodiscard]] bool init(const ACS724Config& config)
        {
            return m_driver.init(ADCSensorDriverConfig<ConverterT>{.channel_config   = config.channel_config,
                                                                   .converter_config = skConverterConfig});
        }

        /**
         * @brief Read the converted current value.
         *
         * @return float Current in Amps.
         */
        [[nodiscard]] inline float read_amps() const
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

}  // namespace valle