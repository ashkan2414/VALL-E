#include <random>

#include "Valle/Device/device.hpp"
#include "Valle/Drivers/adc.hpp"
#include "Valle/Math/converters.hpp"
#include "Valle/Modules/vca.hpp"
#include "app_bridge.h"

using VCAControllerT = VCAControllerModule<HRTIMTimerDeviceA>;

using CurrentSensorADCChannel = ADCInjectChannelDevice1<1, ADCChannelId::kChannel1>;
using CurrentSensorT          = ADCSensorDriver<CurrentSensorADCChannel>;
using CurrentSensorADCP       = CurrentSensorADCChannel::ChannelT::PeripheralT;

float read_current_sensor();

auto g_storage = build_device_storage_from_drivers<VCAControllerT, CurrentSensorT>();
auto [g_device_ref_registry, g_current_sensor, g_vca] =
    boot_driver_builder(g_storage)
        .install<CurrentSensorT>()
        .install<VCAControllerT>(delegate::Delegate<float>(&read_current_sensor))
        .yield();

float read_current_sensor()
{
    return g_current_sensor.read();
}

template <>
struct ADCIsrRouter<CurrentSensorT::ChannelT::skADCIdx, ADCInterruptType::kEndOfInjectSequence>
{
    static_assert(CurrentSensorT::ChannelT::skGroup == ADCChannelGroup::kInject,
                  "ADCIsrRouter specialization used for non-inject channel");

    static void handle()
    {
        // First read the current sensor and convert
        g_current_sensor.on_data_available();

        // Then run VCA control loop
        g_vca.run_ctrl_loop();
    }
};

void init_shared()
{
    g_device_ref_registry.foreach_shared(Overloaded{
        [](ADCPeripheralDevice<1>& dev)
        {
            dev.init(ADCPeripheralConfig{
                .resolution     = ADCResolution::k12Bit,
                .data_alignment = ADCDataAlignment::kRight,
                .low_power      = ADCLowPowerMode::kNone,
                .inj =
                    ADCInjectGroupConfig{
                        .trigger_source = ADCInjectGroupTriggerSource::kSoftware,
                        .trigger_edge   = ADCInjectGroupTriggerEdge::kRising,
                    },
                .reg          = ADCRegularGroupConfig{.trigger_source    = ADCRegularGroupTriggerSource::kSoftware,
                                                      .trigger_edge      = ADCRegularGroupTriggerEdge::kRising,
                                                      .dma_transfer      = ADCRegularGroupDMATransfer::kUnlimited,
                                                      .overrun           = ADCRegularGroupOverrunBehavior::kOverwrite,
                                                      .conversion_mode   = ADCRegularGroupConversionMode::kSingleShot,
                                                      .oversampling_mode = ADCRegularGroupOversamplingMode::kDiscontinuous},
                .oversampling = std::nullopt  // No oversampling by default
            });
        },
        [](HRTIMDevice& dev) { dev.init(); },
    });
}

void init_drivers()
{
    g_vca.init();
    g_current_sensor.init(ADCSensorDriverConfig<ADCRawValueConverter<IdentityConverter<ADCValue>>>{
        .channel_config =
            ADCChannelConfig{
                .sampling_time = ADCChannelSampleTime::k12Cycles5,
                .input_mode    = ADCChannelInputMode::kSingleEnded,
                .offset        = std::nullopt,
            },
        .converter_config = ADCRawValueConverter<IdentityConverter<ADCValue>>::Config{}});
}

void post_init_shared()
{
    g_device_ref_registry.foreach_shared(Overloaded{
        [](ADCPeripheralDevice<1>& dev) { dev.post_init(true, false); },
        [](HRTIMDevice& dev) { dev.post_init(); },
    });
}

void init()
{
    init_shared();
    init_drivers();
    post_init_shared();
}

void app_entry(void)
{
    init();

    g_vca.enable();
    g_device_ref_registry.template get<CurrentSensorADCP>().start_inject();

    while (1)
    {
        g_vca.set_target_current(random() % 5);  // Set random target between 0-5A
    }
}