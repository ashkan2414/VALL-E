#include <random>

#include "app.hpp"
#include "valle/app_bridge.hpp"
#include "valle/system/timing.hpp"

namespace valle::app
{
    system::TimePointMillis g_last_read_time;

    void read_auto_calibrate_drive_current()
    {
        VALLE_LOG_INFO("Reading auto-calibrate IDrive current...");
        const auto auto_calibrate_drive_current =
            g_drivers.position_sensor.read_auto_calibrate_drive_current<LDC161XChannel::kChannel0>();
        if (auto_calibrate_drive_current.has_value())
        {
            VALLE_LOG_INFO("Auto-calibrate IDrive current: {:#x}", auto_calibrate_drive_current.value());
        }
        else
        {
            VALLE_LOG_ERROR("Failed to read auto-calibrate IDrive current");
        }
    }

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

    void print_raw(const LDC161XChannel channel, const LDC161XDataRaw& data)
    {
        if (data.err_underrun || data.err_overrun || data.err_watchdog || data.err_amplitude)
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

    void print_frequency(const LDC161XChannel channel, const LDC161XDataFrequency& data)
    {
        if (data.err_underrun || data.err_overrun || data.err_watchdog || data.err_amplitude)
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

    void check_result()
    {
        std::optional<PositionSensorAsyncReadResultT> read_result;
        g_position_sensor_read_result.with_lock(
            [&](auto& rs)
            {
                read_result = rs;
                if (read_result.has_value())
                {
                    rs.reset();  // Clear the result after reading to prevent processing the same result multiple times
                }
            });

        if (!read_result.has_value())
        {
            return;
        }

        g_last_read_time = valle::system::MillisClock::now();

        std::visit(
            Overloaded{
                [](const PositionSensorT::ErrorCallbackResultT&) { VALLE_LOG_ERROR("Error!"); },
                [](const PositionSensorT::ReadStatusCallbackResultT& result) { print_status(result.status); },
                [](const PositionSensorT::ReadDataRawCallbackResultT& result)
                { print_raw(LDC161XChannel::kChannel0, result.data); },
                [](const PositionSensorT::ReadFrequencyCallbackResultT& result)
                { print_frequency(LDC161XChannel::kChannel0, result.data); },
                [](const PositionSensorT::ReadDataRawMultiCallbackResultT& result)
                {
                    for (uint8_t i = 0; i < result.data.size(); ++i)
                    {
                        print_raw(ldc161x_get_channel_from_index(i), result.data[i]);
                    }
                },
                [](const PositionSensorT::ReadFrequencyMultiCallbackResultT& result)
                {
                    for (uint8_t i = 0; i < result.data.size(); ++i)
                    {
                        print_frequency(ldc161x_get_channel_from_index(i), result.data[i]);
                    }
                },
                [](const PositionSensorT::ReadStatusDataRawCallbackResultT& result)
                {
                    // print_status(result.status);
                    print_raw(LDC161XChannel::kChannel0, result.data);
                },
                [](const PositionSensorT::ReadStatusFrequencyCallbackResultT& result)
                {
                    // print_status(result.status);
                    print_frequency(LDC161XChannel::kChannel0, result.data);
                },
                [](const PositionSensorT::ReadStatusDataRawMultiCallbackResultT& result)
                {
                    // print_status(result.status);
                    for (uint8_t i = 0; i < result.data.size(); ++i)
                    {
                        print_raw(ldc161x_get_channel_from_index(i), result.data[i]);
                    }
                },
                [](const PositionSensorT::ReadStatusFrequencyMultiCallbackResultT& result)
                {
                    // print_status(result.status);
                    for (uint8_t i = 0; i < result.data.size(); ++i)
                    {
                        print_frequency(ldc161x_get_channel_from_index(i), result.data[i]);
                    }
                },
            },
            read_result.value());
    }

    void main()
    {
        init();

        // Trigger a read to get interrupt pin working.
        expect(g_drivers.position_sensor.read_status_data_raw_multi_async(),
               "Failed to trigger initial async read on position sensor");

        while (true)
        {
            check_result();

            const auto duration_since_last_read = valle::system::MillisClock::now() - g_last_read_time;
            if (duration_since_last_read > system::DurationMillis(10))
            {
                VALLE_LOG_WARN("Haven't received a read result in over 10 milliseconds. Triggering a new read...");
                (void)g_drivers.position_sensor.read_status_data_raw_multi_async();
                g_last_read_time = valle::system::MillisClock::now();
            }

            system::TimingContext::delay_ms(1);
        }
    }

}  // namespace valle::app