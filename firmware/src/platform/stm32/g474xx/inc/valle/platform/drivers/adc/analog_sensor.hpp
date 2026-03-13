#pragma once

#include <atomic>
#include <concepts>

#include "valle/math/converters.hpp"
#include "valle/platform/devices/adc.hpp"

namespace valle::platform
{

    enum class ADCConverterTag
    {
        kRawValue,
        kVoltage,
        kNormalized
    };

    template <typename TConverter>
        requires(std::convertible_to<typename TConverter::InputT, ADCValue>)
    struct ADCRawValueConverter
    {
        static constexpr ADCConverterTag skTag = ADCConverterTag::kRawValue;
        using ConvertedT                       = typename TConverter::ConvertedT;
        using Config                           = typename TConverter::Config;

    private:
        TConverter m_converter;

    public:
        constexpr inline void init(const Config& config)
        {
            m_converter.init(config);
        }

        [[nodiscard]] constexpr inline ConvertedT convert(const ADCValue raw) const
        {
            return m_converter.convert(static_cast<typename TConverter::InputT>(raw));
        }
    };

    template <typename TConverter>
        requires(std::convertible_to<typename TConverter::InputT, float>)
    class ADCVoltageConverter
    {
    public:
        static constexpr ADCConverterTag skTag = ADCConverterTag::kVoltage;
        using Config                           = typename TConverter::Config;
        using ConvertedT                       = typename TConverter::ConvertedT;

    private:
        TConverter m_converter;

    public:
        constexpr void init(const Config& config)
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
    class ADCNormalizedConverter
    {
    public:
        static constexpr ADCConverterTag skTag = ADCConverterTag::kNormalized;
        using Config                           = typename TConverter::Config;
        using ConvertedT                       = typename TConverter::ConvertedT;

    private:
        TConverter m_converter;

    public:
        constexpr void init(const Config& config)
        {
            m_converter.init(config);
        }

        [[nodiscard]] constexpr ConvertedT convert(const float normalized) const
        {
            return m_converter.convert(static_cast<typename TConverter::InputT>(normalized));
        }
    };

    template <typename T>
    concept CADCConverter = requires {
        { T::skTag } -> std::convertible_to<ADCConverterTag>;
    };

    template <CADCConverter TConverter = ADCRawValueConverter<IdentityConverter<ADCValue>>>
    struct ADCAnalogSensorDriverConfig
    {
        ADCChannelConfig            channel_config{};
        typename TConverter::Config converter_config{};
    };

    template <CDevice TADCChannel, CADCConverter TConverter = ADCRawValueConverter<IdentityConverter<ADCValue>>>
    class ADCAnalogSensorDriver
    {
    public:
        using ChannelT      = TADCChannel;
        using InjectDevices = TypeList<ChannelT>;
        using ConvertedT    = typename TConverter::ConvertedT;

    private:
        [[no_unique_address]] DeviceRef<ChannelT> m_channel;
        TConverter                                m_converter;
        std::atomic<ConvertedT>                   m_last_value;

    public:
        ADCAnalogSensorDriver() = delete;

        ADCAnalogSensorDriver(DeviceRef<ChannelT>&& channel)
            : m_channel(std::move(channel)), m_last_value(static_cast<ConvertedT>(0))
        {
        }

        // Move constructor (needed because of std::atomic)
        ADCAnalogSensorDriver(ADCAnalogSensorDriver&& other) noexcept
            : m_channel(std::move(other.m_channel))
            , m_converter(std::move(other.m_converter))
            , m_last_value(other.m_last_value.load(std::memory_order_relaxed))
        {
        }

        // Move assignment (needed because of std::atomic)
        ADCAnalogSensorDriver& operator=(ADCAnalogSensorDriver&& other) noexcept
        {
            if (this != &other)
            {
                m_channel   = std::move(other.m_channel);
                m_converter = std::move(other.m_converter);
                m_last_value.store(other.m_last_value.load(std::memory_order_relaxed), std::memory_order_relaxed);
            }
            return *this;
        }

        [[nodiscard]] bool init(const ADCAnalogSensorDriverConfig<TConverter>& config)
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
            if constexpr (TConverter::skTag == ADCConverterTag::kRawValue)
            {
                const ADCValue raw = m_channel.get().read();
                return m_converter.convert(raw);
            }
            else if constexpr (TConverter::skTag == ADCConverterTag::kVoltage)
            {
                const float voltage = m_channel.get().read_voltage();
                return m_converter.convert(voltage);
            }
            else if constexpr (TConverter::skTag == ADCConverterTag::kNormalized)
            {
                const float normalized = m_channel.get().read_normalized();
                return m_converter.convert(normalized);
            }
            else
            {
                static_assert(false, "Unsupported ADCConverterTag");
            }
        }
    };

}  // namespace valle::platform