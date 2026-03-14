#include "app.hpp"

#include "valle/app/platform/core_system_config.hpp"
#include "valle/base/panic.hpp"

VALLE_DEFINE_UART_LOGGER_HANDLER(valle::app::g_drivers.uart_logger);

namespace valle::app
{
    DriverBuilderReturnT install_drivers(DriverBuilderT&& builder)
    {
        return std::move(builder)
            .template install<RootDriver>()
            .template install<platform::CoreSystemDriver>()
            .template install<UARTLoggerT>()
            .template install<TestADCDriverT>()
            .yield();
    }

    /**
     * @brief Initialize shared devices.
     *
     */
    static void init_root()
    {
        g_drivers.root.foreach (DeviceInitOverloaded{
            [](platform::CoreSystemDriver& dev) { (void)dev; },
            [](platform::ADC12CommonDevice& dev)
            {
                expect(dev.init(platform::ADC12CommonConfig{
                           .clock_config =
                               platform::ADCCommonAsyncClockConfig{.prescaler =
                                                                       platform::ADCCommonAsyncClockPrescaler::kDiv8}}),
                       "Failed to initialize ADC12 Common Device");
            },
            [](platform::ADC1ControllerDevice& dev) { expect(dev.init(), "Failed to initialize Test ADC Controller"); },
            [](platform::DMAMux1ControllerDevice& dev)
            { expect(dev.init(), "Failed to initialize DMAMux1 Controller Device"); },

            [](platform::DMA1ControllerDevice& dev)
            { expect(dev.init(), "Failed to initialize DMA1 Controller Device"); },

            [](platform::DMA2ControllerDevice& dev)
            { expect(dev.init(), "Failed to initialize DMA2 Controller Device"); },

            [](platform::GPIOPortADevice& dev) { expect(dev.init(), "Failed to initialize GPIO Port A Device"); },
        }  // namespace valle
        );
    }

    /**
     * @brief Initialize drivers.
     *
     */
    static void init_drivers()
    {
        expect(g_drivers.uart_logger.init(platform::UARTControllerConfig{
                   .baud_rate         = platform::UARTBaudRate::kBaud230400,
                   .word_length       = platform::UARTWordLength::kBits8,
                   .stop_bits         = platform::UARTStopBits::kBits1,
                   .parity            = platform::UARTParity::kNone,
                   .transfer_mode     = platform::UARTTransferMode::kTxRx,
                   .hw_flow_ctrl      = platform::UARTHardwareFlowControl::kNone,
                   .dma_priority      = platform::DMAPriority::kHigh,
                   .dma_int_priority  = 5,
                   .uart_int_priority = 5,
               }),
               "Failed to initialize UART Logger Driver");

        expect(g_drivers.test_adc.init(platform::ADCAnalogSensorDriverConfig<TestADCConverterT>{
                   .channel_config =
                       platform::ADCChannelConfig{.sampling_time = platform::ADCChannelSampleTime::k12Cycles5,
                                                  .input_mode    = platform::ADCChannelInputMode::kSingleEnded,
                                                  .offset        = std::nullopt},
                   .converter_config{}}),
               "Failed to initialize Test ADC Driver");
    }

    /**
     * @brief Initialize the system hardware.
     *
     */
    void init()
    {
        // Core System Device must be initialized first since other devices may depend on it for clock configuration
        expect(g_drivers.core.init(platform::app::kDefaultCoreSystemConfig), "Failed to initialize Core System Device");

        init_root();
        init_drivers();

        expect(g_drivers.root.adc1().post_init(
                   platform::ADCControllerConfig{
                       .resolution     = platform::ADCResolution::k12Bit,
                       .data_alignment = platform::ADCDataAlignment::kRight,
                       .low_power      = platform::ADCLowPowerMode::kNone,
                       .inj =
                           platform::ADCInjectGroupConfig{
                               .trigger_source = platform::ADCInjectGroupTriggerSource::kSoftware,
                               .trigger_edge   = platform::ADCInjectGroupTriggerEdge::kRisingFalling,
                           },
                       .reg =
                           platform::ADCRegularGroupConfig{
                               .trigger_source = platform::ADCRegularGroupTriggerSource::kSoftware,
                               .trigger_edge   = platform::ADCRegularGroupTriggerEdge::kRisingFalling,
                               .dma =
                                   platform::ADCRegularGroupDMAConfig{
                                       .priority      = platform::DMAPriority::kHigh,
                                       .circular_mode = true,
                                       .interrupts =
                                           platform::DMAChannelInterruptConfig{
                                               .priority = 10,
                                               .interrupts =
                                                   platform::DMAChannelInterruptMask{
                                                       .transfer_complete = true,
                                                       .half_transfer     = false,
                                                       .transfer_error    = false,
                                                   },
                                           },
                                   },
                               .overrun           = platform::ADCRegularGroupOverrunBehavior::kOverwrite,
                               .conversion_mode   = platform::ADCRegularGroupConversionMode::kSingleShot,
                               .oversampling_mode = platform::ADCRegularGroupOversamplingMode::kContinuous},
                       .oversampling = std::nullopt  // No oversampling by default
                   }),
               "Failed to post-initialize ADC1 Controller Device");
    }

}  // namespace valle::app