#pragma once

#include "Valle/Drivers/adc.hpp"

/**
 * @brief ACS724 models.
 *
 */
enum class ACS724Model : uint8_t
{
    k2P5AB = 0,  // 2P5AB-S ±2.5
    k05AU,       // 05AU-S 5
    k05AB,       // 05AB-S ±5
    k10AU,       // 10AU-S 10
    k10AB,       // 10AB-S ±10
    k20AU,       // 20AU-S 20
    k20AB,       // 20AB-S ±20
    k30AU,       // 30AU-S 30
    k30AB,       // 30AB-S ±30
    k50AB,       // 50AB-S ±50
};

// ============================================================================
// ACS724 Model Traits
// ============================================================================

template <ACS724Model tkModel>
struct ACS724Traits;

template <>
struct ACS724Traits<ACS724Model::k2P5AB>
{
    static constexpr bool  skBiDirectional = true;
    static constexpr float skRangeAmps     = 5.0f;
    static constexpr float skMinAmps       = -2.5f;
    static constexpr float skMaxAmps       = 2.5f;
};

template <>
struct ACS724Traits<ACS724Model::k05AU>
{
    static constexpr bool  skBiDirectional = false;
    static constexpr float skRangeAmps     = 5.0f;
    static constexpr float skMinAmps       = 0.0f;
    static constexpr float skMaxAmps       = 5.0f;
};

template <>
struct ACS724Traits<ACS724Model::k05AB>
{
    static constexpr bool  skBiDirectional = true;
    static constexpr float skRangeAmps     = 10.0f;
    static constexpr float skMinAmps       = -5.0f;
    static constexpr float skMaxAmps       = 5.0f;
};

template <>
struct ACS724Traits<ACS724Model::k10AU>
{
    static constexpr bool  skBiDirectional = false;
    static constexpr float skRangeAmps     = 10.0f;
    static constexpr float skMinAmps       = 0.0f;
    static constexpr float skMaxAmps       = 10.0f;
};

template <>
struct ACS724Traits<ACS724Model::k10AB>
{
    static constexpr bool  skBiDirectional = true;
    static constexpr float skRangeAmps     = 20.0f;
    static constexpr float skMinAmps       = -10.0f;
    static constexpr float skMaxAmps       = 10.0f;
};

template <>
struct ACS724Traits<ACS724Model::k20AU>
{
    static constexpr bool  skBiDirectional = false;
    static constexpr float skRangeAmps     = 20.0f;
    static constexpr float skMinAmps       = 0.0f;
    static constexpr float skMaxAmps       = 20.0f;
};

template <>
struct ACS724Traits<ACS724Model::k20AB>
{
    static constexpr bool  skBiDirectional = true;
    static constexpr float skRangeAmps     = 40.0f;
    static constexpr float skMinAmps       = -20.0f;
    static constexpr float skMaxAmps       = 20.0f;
};

template <>
struct ACS724Traits<ACS724Model::k30AU>
{
    static constexpr bool  skBiDirectional = false;
    static constexpr float skRangeAmps     = 30.0f;
    static constexpr float skMinAmps       = 0.0f;
    static constexpr float skMaxAmps       = 30.0f;
};

template <>
struct ACS724Traits<ACS724Model::k30AB>
{
    static constexpr bool  skBiDirectional = true;
    static constexpr float skRangeAmps     = 60.0f;
    static constexpr float skMinAmps       = -30.0f;
    static constexpr float skMaxAmps       = 30.0f;
};

// ============================================================================
// ACS724 Module Drivers
// ============================================================================

struct ACS724Config
{
    ADCChannelConfig channel_config;  /// ADC Channel Configuration
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
        .offset = -ModelTraits::skMinAmps,
    };

    ADCSensorDriver<TADCChannel, ConverterT> m_driver;

public:
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
    void init(const ACS724Config& config)
    {
        m_driver.init(ADCSensorDriverConfig<ConverterT>{.channel_config   = config.channel_config,
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
