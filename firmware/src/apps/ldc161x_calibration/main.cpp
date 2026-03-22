#include <random>

#include "app.hpp"
#include "valle/app_bridge.hpp"
#include "valle/system/timing.hpp"

namespace valle::app
{
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

    void print_raw(const LDC161XChannel channel, const LDC161XDataRaw& data, bool check_errors = false)
    {
        if (check_errors && (data.err_underrun || data.err_overrun || data.err_watchdog || data.err_amplitude))
        {
            VALLE_LOG_WARN(
                "Raw data reading for channel {} has errors. ERR_UNDERRUN={}, ERR_OVERRUN={}, "
                "ERR_WATCHDOG={}, ERR_AMPLITUDE={}",
                static_cast<uint8_t>(channel),
                data.err_underrun,
                data.err_overrun,
                data.err_watchdog,
                data.err_amplitude);
        }

        const auto freq_mhz = g_drivers.position_sensor.frequency_from_raw_data(channel, data.value);
        VALLE_LOG_INFO("Channel {} Raw Data: {:#x} ({} MHz)", static_cast<uint8_t>(channel), data.value, freq_mhz);
    }

    void print_frequency(const LDC161XChannel channel, const LDC161XDataFrequency& data, bool check_errors = false)
    {
        if (check_errors && (data.err_underrun || data.err_overrun || data.err_watchdog || data.err_amplitude))
        {
            VALLE_LOG_WARN(
                "Frequency reading for channel {} has errors. ERR_UNDERRUN={}, ERR_OVERRUN={}, "
                "ERR_WATCHDOG={}, ERR_AMPLITUDE={}",
                static_cast<uint8_t>(channel),
                data.err_underrun,
                data.err_overrun,
                data.err_watchdog,
                data.err_amplitude);
        }

        VALLE_LOG_INFO("Channel {} Frequency: {} MHz", static_cast<uint8_t>(channel), data.frequency_mhz);
    }

    void main()
    {
        init();

        while (true)
        {
            const auto data = g_drivers.position_sensor.read_data_raw_blocking();
            if (!data.has_value())
            {
                VALLE_LOG_ERROR("Failed to read raw data from position sensor");
                continue;
            }
            print_raw(LDC161XChannel::kChannel0, data.value(), false);
            system::TimingContext::delay_ms(10);
        }
    }

}  // namespace valle::app