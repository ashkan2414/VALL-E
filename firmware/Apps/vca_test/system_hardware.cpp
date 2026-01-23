#include "system_hardware.hpp"

namespace valle::system
{

    float read_vca_current();

    // Make Device Storage
    DeviceStorageT g_storage = build_device_storage_from_drivers<CurrentSensorT, VCAControllerT>();

    // Install drivers
    namespace dont_use_me
    {
        static auto g_installed_devices = boot_driver_builder(g_storage)
                                              .install<CurrentSensorT>()
                                              .install<VCAControllerT>(delegate::Delegate<float>(&read_vca_current))
                                              .yield();
    }

    // Create Device Reference Registry and Drivers (for global use)
    DeviceRefRegistryT g_device_ref_registry = std::move(std::get<0>(dont_use_me::g_installed_devices));
    CurrentSensorT     g_current_sensor      = std::move(std::get<1>(dont_use_me::g_installed_devices));
    VCAControllerT     g_vca                 = std::move(std::get<2>(dont_use_me::g_installed_devices));

    /**
     * @brief Read current sensor helper function. Used as feedback for VCA controller.
     *
     * @return float Current in Amps.
     */
    float read_vca_current()
    {
        return g_current_sensor.read_amps();
    }

    /**
     * @brief Initialize shared devices.
     *
     */
    static void init_shared()
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
            [](GPIOPortADevice& dev) { dev.init(); },
            [](HRTIMDevice& dev) { dev.init(); },
        });
    }

    /**
     * @brief Initialize drivers.
     *
     */
    static void init_drivers()
    {
        g_vca.init();
        g_current_sensor.init(ACS724Config{.channel_config = ADCChannelConfig{
                                               .sampling_time = ADCChannelSampleTime::k12Cycles5,
                                               .input_mode    = ADCChannelInputMode::kSingleEnded,
                                               .offset        = std::nullopt,
                                           }});
    }

    /**
     * @brief Post-initialize shared devices.
     *
     */
    static void post_init_shared()
    {
        g_device_ref_registry.foreach_shared(Overloaded{
            [](ADCPeripheralDevice<1>& dev) { dev.post_init(true, false); },
            [](GPIOPortADevice& dev) { dev.post_init(); },
            [](HRTIMDevice& dev) { dev.post_init(); },
        });
    }

    /**
     * @brief Initialize the system hardware.
     *
     */
    void init()
    {
        init_shared();
        init_drivers();
        post_init_shared();
    }

    void start_vca_controller()
    {
        g_device_ref_registry.template get<CurrentSensorADCP>().start_inject();
        g_vca.enable();
    }

}  // namespace valle::system