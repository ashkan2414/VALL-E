#include <random>

#include "app.hpp"
#include "valle/app_bridge.hpp"
#include "valle/system/timing.hpp"

namespace valle::app
{

    auto channel0_converter = LDC161XFrequencyToDistanceTableConverter<7>(
        LDC161XFrequencyToDistanceTableConverterConfig<7>{.table = {LookupTablePoint{3.461042F, 1.19F},
                                                                    LookupTablePoint{3.372453F, 3.75F},
                                                                    LookupTablePoint{3.366891F, 4.16F},
                                                                    LookupTablePoint{3.360423F, 4.57F},
                                                                    LookupTablePoint{3.357199F, 4.97F},
                                                                    LookupTablePoint{3.352718F, 5.36F},
                                                                    LookupTablePoint{3.350428F, 5.77F}}});

    auto channel1_converter = LDC161XFrequencyToDistanceTableConverter<9>(
        LDC161XFrequencyToDistanceTableConverterConfig<9>{.table = {LookupTablePoint{3.484316F, 0.0F},
                                                                    LookupTablePoint{3.45249F, 0.5F},
                                                                    LookupTablePoint{3.407546F, 1.19F},
                                                                    LookupTablePoint{3.335775F, 3.75F},
                                                                    LookupTablePoint{3.329356F, 4.16F},
                                                                    LookupTablePoint{3.326385F, 4.57F},
                                                                    LookupTablePoint{3.323087F, 4.97F},
                                                                    LookupTablePoint{3.319487F, 5.36F},
                                                                    LookupTablePoint{3.315132F, 5.77F}}});

    void print_status(const LDC161XStatus& status)
    {
        VALLE_LOG_INFO(
            "Status: ERR_CHANNEL={}, ERR_UNDERRUN={}, ERR_OVERRUN={}, "
            "ERR_WATCHDOG={}, ERR_AMPLITUDE_HIGH={}, ERR_AMPLITUDE_LOW={}, "
            "ERR_ZERO_COUNT={}, DRDY={}, UNDREAD_CONV0={}, UNDREAD_CONV1={}, "
            "UNDREAD_CONV2={}, UNDREAD_CONV3={}",
            status.err_channel,
            status.err_underrun,
            status.err_overrun,
            status.err_watchdog,
            status.err_amplitude_high,
            status.err_amplitude_low,
            status.err_zero_count,
            status.drdy,
            status.undread_conv0,
            status.undread_conv1,
            status.undread_conv2,
            status.undread_conv3);
    }

    void print_data(const LDC161XDataRaw& channel0_data, const LDC161XDataRaw& channel1_data, bool check_errors = false)
    {
        if (check_errors)
        {
            if (channel0_data.err_underrun || channel0_data.err_overrun || channel0_data.err_watchdog ||
                channel0_data.err_amplitude)
            {
                VALLE_LOG_WARN(
                    "Raw data reading for channel 0 has errors. ERR_UNDERRUN={}, ERR_OVERRUN={}, "
                    "ERR_WATCHDOG={}, ERR_AMPLITUDE={}",
                    channel0_data.err_underrun,
                    channel0_data.err_overrun,
                    channel0_data.err_watchdog,
                    channel0_data.err_amplitude);
            }

            if (channel1_data.err_underrun || channel1_data.err_overrun || channel1_data.err_watchdog ||
                channel1_data.err_amplitude)
            {
                VALLE_LOG_WARN(
                    "Raw data reading for channel 1 has errors. ERR_UNDERRUN={}, ERR_OVERRUN={}, "
                    "ERR_WATCHDOG={}, ERR_AMPLITUDE={}",
                    channel1_data.err_underrun,
                    channel1_data.err_overrun,
                    channel1_data.err_watchdog,
                    channel1_data.err_amplitude);
            }
        }

        const auto channel0_freq_mhz =
            g_drivers.position_sensor.frequency_from_raw_data<LDC161XChannel::kChannel0>(channel0_data.value);
        const auto channel1_freq_mhz =
            g_drivers.position_sensor.frequency_from_raw_data<LDC161XChannel::kChannel1>(channel1_data.value);
        VALLE_LOG_INFO("Channel 0: {:#x} ({:.6f} MHz, {:.6f} mm), Channel 1: {:#x} ({:.6f} MHz, {:.6f} mm)",
                       channel0_data.value,
                       channel0_freq_mhz,
                       channel0_converter.convert(channel0_freq_mhz),
                       channel1_data.value,
                       channel1_freq_mhz,
                       channel1_converter.convert(channel1_freq_mhz));
    }

    void main()
    {
        init();

        while (true)
        {
            const auto channel0_data = g_drivers.position_sensor.read_data_raw_blocking<LDC161XChannel::kChannel0>();
            if (!channel0_data.has_value())
            {
                VALLE_LOG_ERROR("Failed to read raw data from position sensor");
                continue;
            }

            const auto channel1_data = g_drivers.position_sensor.read_data_raw_blocking<LDC161XChannel::kChannel1>();
            if (!channel1_data.has_value())
            {
                VALLE_LOG_ERROR("Failed to read raw data from position sensor");
                continue;
            }

            print_data(channel0_data.value(), channel1_data.value(), false);

            system::TimingContext::delay_ms(10);
        }
    }

}  // namespace valle::app