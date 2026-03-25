#pragma once

#include <atomic>
#include <concepts>

#include "valle/math/converters.hpp"
#include "valle/platform/devices/adc.hpp"

namespace valle::platform
{

    enum class AdcConverterTag
    {
        kRawValue,
        kVoltage,
        kNormalized
    };

    template <typename TConverter>
        requires(std::convertible_to<typename TConverter::InputT, AdcValue>)
    struct AdcRawValueConverter
    {
        static constexpr AdcConverterTag skTag = AdcConverterTag::kRawValue;
        using ConvertedT                       = typename TConverter::ConvertedT;
        using ConfigT                          = typename TConverter::ConfigT;

    private:
        TConverter m_converter;

    public:
        constexpr inline void init(const ConfigT& config)
        {
            m_converter.init(config);
        }

        [[nodiscard]] constexpr inline ConvertedT convert(const AdcValue raw) const
        {
            return m_converter.convert(static_cast<typename TConverter::InputT>(raw));
        }
    };

    template <typename TConverter>
        requires(std::convertible_to<typename TConverter::InputT, float>)
    class AdcVoltageConverter
    {
    public:
        static constexpr AdcConverterTag skTag = AdcConverterTag::kVoltage;
        using ConfigT                          = typename TConverter::ConfigT;
        using ConvertedT                       = typename TConverter::ConvertedT;

    private:
        TConverter m_converter;

    public:
        constexpr void init(const ConfigT& config)
        {
            m_converter.init(config);
        }

        [[nodiscard]] constexpr ConvertedT convert(const float voltage) const
        {
            return m_converter.convert(static_cast<typename TConverter::InputT>(voltage));
        }
    };

    template <typename TConverter>
        requires(std::convertible_to<typename TConverter::InputT, float>)
    class AdcNormalizedConverter
    {
    public:
        static constexpr AdcConverterTag skTag = AdcConverterTag::kNormalized;
        using ConfigT                          = typename TConverter::ConfigT;
        using ConvertedT                       = typename TConverter::ConvertedT;

    private:
        TConverter m_converter;

    public:
        constexpr void init(const ConfigT& config)
        {
            m_converter.init(config);
        }

        [[nodiscard]] constexpr ConvertedT convert(const float normalized) const
        {
            return m_converter.convert(static_cast<typename TConverter::InputT>(normalized));
        }
    };

    template <typename T>
    concept CAdcConverter = requires {
        { T::skTag } -> std::convertible_to<AdcConverterTag>;
    };

    template <CAdcConverter TConverter = AdcRawValueConverter<IdentityConverter<AdcValue>>>
    struct AdcAnalogSensorDriverConfig
    {
        AdcChannelConfig             channel_config{};
        typename TConverter::ConfigT converter_config{};
    };

    template <CDevice TAdcChannel, CAdcConverter TConverter = AdcRawValueConverter<IdentityConverter<AdcValue>>>
    class AdcAnalogSensorDriver
    {
    public:
        using ChannelT      = TAdcChannel;
        using InjectDevices = TypeList<ChannelT>;
        using ConvertedT    = typename TConverter::ConvertedT;

    private:
        [[no_unique_address]] DeviceRef<ChannelT> m_channel;
        TConverter                                m_converter;
        std::atomic<ConvertedT>                   m_last_value;

    public:
        AdcAnalogSensorDriver() = delete;

        AdcAnalogSensorDriver(DeviceRef<ChannelT>&& channel)
            : m_channel(std::move(channel)), m_last_value(static_cast<ConvertedT>(0))
        {
        }

        // Move constructor (needed because of std::atomic)
        AdcAnalogSensorDriver(AdcAnalogSensorDriver&& other) noexcept
            : m_channel(std::move(other.m_channel))
            , m_converter(std::move(other.m_converter))
            , m_last_value(other.m_last_value.load(std::memory_order_relaxed))
        {
        }

        // Move assignment (needed because of std::atomic)
        AdcAnalogSensorDriver& operator=(AdcAnalogSensorDriver&& other) noexcept
        {
            if (this != &other)
            {
                m_channel   = std::move(other.m_channel);
                m_converter = std::move(other.m_converter);
                m_last_value.store(other.m_last_value.load(std::memory_order_relaxed), std::memory_order_relaxed);
            }
            return *this;
        }

        [[nodiscard]] bool init(const AdcAnalogSensorDriverConfig<TConverter>& config)
        {
            m_converter.init(config.converter_config);
            return m_channel.get().init(config.channel_config);
        }

        [[nodiscard]] inline ConvertedT read() const
        {
            return m_last_value.load(std::memory_order_relaxed);
        }

        void on_data_available()
        {
            m_last_value.store(read_and_convert(), std::memory_order_relaxed);
        }

    private:
        [[nodiscard]] inline ConvertedT read_and_convert() const
        {
            if constexpr (TConverter::skTag == AdcConverterTag::kRawValue)
            {
                const AdcValue raw = m_channel.get().read();
                return m_converter.convert(raw);
            }
            else if constexpr (TConverter::skTag == AdcConverterTag::kVoltage)
            {
                const float voltage = m_channel.get().read_voltage();
                return m_converter.convert(voltage);
            }
            else if constexpr (TConverter::skTag == AdcConverterTag::kNormalized)
            {
                const float normalized = m_channel.get().read_normalized();
                return m_converter.convert(normalized);
            }
            else
            {
                static_assert(false, "Unsupported AdcConverterTag");
            }
        }
    };

}  // namespace valle::platform