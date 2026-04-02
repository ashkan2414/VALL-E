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
            .template install<UartLoggerT>()
            .template install<TestAdcDriverT>()
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
            [](platform::Adc12CommonController& dev)
            {
                expect(dev.init(platform::AdcCommonConfig{
                           .clock_config =
                               platform::AdcCommonAsyncClockConfig{.prescaler =
                                                                       platform::AdcCommonAsyncClockPrescaler::kDiv8}}),
                       "Failed to initialize Adc12 Common Device");
            },
            [](platform::Adc1Controller& dev) { expect(dev.init(), "Failed to initialize Test ADC Controller"); },
            [](platform::DmaMux1Controller& dev)
            { expect(dev.init(), "Failed to initialize DmaMux1 Controller Device"); },

            [](platform::Dma1Controller& dev) { expect(dev.init(), "Failed to initialize Dma1 Controller Device"); },

            [](platform::Dma2Controller& dev) { expect(dev.init(), "Failed to initialize Dma2 Controller Device"); },

            [](platform::GpioPortA& dev) { expect(dev.init(), "Failed to initialize GPIO Port A Device"); },
        }  // namespace valle
        );
    }

    /**
     * @brief Initialize drivers.
     *
     */
    static void init_drivers()
    {
        expect(g_drivers.uart_logger.init(platform::UartControllerConfig{
                   .baud_rate         = platform::UartBaudRate::kBaud230400,
                   .word_length       = platform::UartWordLength::kBits8,
                   .stop_bits         = platform::UartStopBits::kBits1,
                   .parity            = platform::UartParity::kNone,
                   .transfer_mode     = platform::UartTransferMode::kTxRx,
                   .hw_flow_ctrl      = platform::UartHardwareFlowControl::kNone,
                   .dma_priority      = platform::DmaChannelPriority::kHigh,
                   .dma_int_priority  = 5,
                   .uart_int_priority = 5,
               }),
               "Failed to initialize UART Logger Driver");

        expect(g_drivers.test_adc.init(platform::AdcAnalogSensorDriverConfig<TestAdcConverterT>{
                   .channel_config =
                       platform::AdcChannelConfig{.sampling_time = platform::AdcChannelSampleTime::k12Cycles5,
                                                  .input_mode    = platform::AdcChannelInputMode::kSingleEnded,
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
                   platform::AdcControllerConfig{
                       .resolution     = platform::AdcResolution::k12Bit,
                       .data_alignment = platform::AdcDataAlignment::kRight,
                       .low_power      = platform::AdcLowPowerMode::kNone,
                       .inj =
                           platform::AdcInjectGroupConfig{
                               .trigger_source = platform::AdcInjectGroupTriggerSource::kSoftware,
                               .trigger_edge   = platform::AdcInjectGroupTriggerEdge::kRisingFalling,
                           },
                       .reg =
                           platform::AdcRegularGroupConfig{
                               .trigger_source = platform::AdcRegularGroupTriggerSource::kSoftware,
                               .trigger_edge   = platform::AdcRegularGroupTriggerEdge::kRisingFalling,
                               .dma =
                                   platform::AdcRegularGroupDmaConfig{
                                       .priority      = platform::DmaChannelPriority::kHigh,
                                       .circular_mode = true,
                                       .interrupts =
                                           platform::DmaChannelInterruptConfig{
                                               .priority = 10,
                                               .interrupts =
                                                   platform::DmaChannelInterruptMask{
                                                       .transfer_complete = true,
                                                       .half_transfer     = false,
                                                       .transfer_error    = false,
                                                   },
                                           },
                                   },
                               .overrun           = platform::AdcRegularGroupOverrunBehavior::kOverwrite,
                               .conversion_mode   = platform::AdcRegularGroupConversionMode::kSingleShot,
                               .oversampling_mode = platform::AdcRegularGroupOversamplingMode::kContinuous},
                       .oversampling = std::nullopt  // No oversampling by default
                   }),
               "Failed to post-initialize Adc1 Controller Device");
    }

}  // namespace valle::app
