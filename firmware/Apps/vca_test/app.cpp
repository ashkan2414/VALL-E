#include "app.hpp"

namespace valle::app
{
    // Forward declaration
    float read_vca_current();

    DriverBuilderReturnT install_drivers(DriverBuilderT&& builder)
    {
        return std::move(builder)
            .template install<CurrentSensorT>()
            .template install<VCAControllerT>(delegate::Delegate<float>(&read_vca_current))
            .yield();
    }

    /**
     * @brief Read current sensor helper function. Used as feedback for VCA controller.
     *
     * @return float Current in Amps.
     */
    float read_vca_current()
    {
        return g_drivers.current_sensor.read_amps();
    }

    /**
     * @brief Initialize shared devices.
     *
     */
    static void init_shared()
    {
        g_ref_registry.foreach_shared(Overloaded{
            [](ADCControllerDevice<1>& dev)
            {
                dev.init(ADCControllerConfig{
                    .resolution     = ADCResolution::k12Bit,
                    .data_alignment = ADCDataAlignment::kRight,
                    .low_power      = ADCLowPowerMode::kNone,
                    .inj =
                        ADCInjectGroupConfig{
                            .trigger_source = ADCInjectGroupTriggerSource::kSoftware,
                            .trigger_edge   = ADCInjectGroupTriggerEdge::kRising,
                        },
                    .reg          = ADCRegularGroupConfig{.trigger_source = ADCRegularGroupTriggerSource::kSoftware,
                                                          .trigger_edge   = ADCRegularGroupTriggerEdge::kRising,
                                                          .dma =
                                                     ADCRegularGroupDMAConfig{
                                                                  .priority      = DMAPriority::kHigh,
                                                                  .circular_mode = true,
                                                                  .interrupts =
                                                             DMAInterruptConfig{
                                                                          .priority  = 5,
                                                                          .enable_tc = true,
                                                                          .enable_ht = false,
                                                                          .enable_te = true,
                                                             },
                                                     },
                                                          .overrun           = ADCRegularGroupOverrunBehavior::kOverwrite,
                                                          .conversion_mode   = ADCRegularGroupConversionMode::kSingleShot,
                                                          .oversampling_mode = ADCRegularGroupOversamplingMode::kDiscontinuous},
                    .oversampling = std::nullopt  // No oversampling by default
                });
            },

            [](DMA1ControllerDevice& dev) { dev.init(); },
            [](GPIOPortADevice& dev) { dev.init(); },
            [](HRTIM1ControllerDevice& dev) { dev.init(); },
        }  // namespace valle
        );
    }

    /**
     * @brief Initialize drivers.
     *
     */
    static void init_drivers()
    {
        g_drivers.vca.init();
        g_drivers.current_sensor.init(ACS724Config{.channel_config = ADCChannelConfig{
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
        g_ref_registry.foreach_shared(Overloaded{
            [](ADCControllerDevice<1>& dev) { dev.post_init(true, false); },
            [](DMA1ControllerDevice& dev) { dev.post_init(); },
            [](GPIOPortADevice& dev) { dev.post_init(); },
            [](HRTIM1ControllerDevice& dev) { dev.post_init(); },
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
        g_ref_registry.template get<CurrentSensorADCControllerT>().start_inject();
        g_drivers.vca.enable();
    }

}  // namespace valle::app