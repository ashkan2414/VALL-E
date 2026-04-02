#pragma once

#include "valle/app/platform/modules/acs724.hpp"
#include "valle/app/platform/modules/vca.hpp"
#include "valle/app/platform/uart_logger_config.hpp"
#include "valle/app/platform/vca_current_loop_driver.hpp"
#include "valle/base/system_build/config_base.hpp"
#include "valle/base/system_build/traits.hpp"
#include "valle/platform/drivers/core_system.hpp"
#include "valle/platform/drivers/gpio/digital_out.hpp"
#include "valle/platform/drivers/uart/logger.hpp"

namespace valle::app
{
    static constexpr platform::HrtimControllerId kVcaHrtimPwmControllerId = platform::HrtimControllerId::kHrtim1;
    struct HrtimControllerCTConfig : public platform::HrtimControllerCTDefaultConfig
    {
    };

    static constexpr uint8_t kIntakeValveVcaCurrentLoopDriverId = 0;
    struct IntakeValveVcaCurrentLoopDriverCTConfig
    {
        using PWMOutput1PinT              = platform::GpioPinA8;
        using PWMOutput2PinT              = platform::GpioPinA9;
        using CurrentSensorAdcDmaChannelT = platform::Dma1Channel2;

        static constexpr platform::HrtimControllerId skVcaHrtimPwmControllerId = kVcaHrtimPwmControllerId;
        static constexpr platform::HrtimTimerId      skVcaHrtimPwmTimerId      = platform::HrtimTimerId::kTimerA;

        static constexpr platform::AdcControllerId skCurrentSensorAdcControllerId = platform::AdcControllerId::kAdc1;
        static constexpr platform::AdcChannelId    skCurrentSensorAdcChannelId    = platform::AdcChannelId::kChannel1;

        static constexpr ACS724Model skCurrentSensorModel = ACS724Model::k2P5ABi;
        static constexpr auto        skCurrentSensorAdcHrtimTriggerSource =
            platform::AdcInjectGroupTriggerSource::kExtHrtimTRG2;
    };

    static constexpr uint8_t kExhaustValveVcaCurrentLoopDriverId = 1;
    struct ExhaustValveVcaCurrentLoopDriverCTConfig
    {
        using PWMOutput1PinT              = platform::GpioPinB14;
        using PWMOutput2PinT              = platform::GpioPinB15;
        using CurrentSensorAdcDmaChannelT = platform::Dma1Channel3;

        static constexpr platform::HrtimControllerId skVcaHrtimPwmControllerId = kVcaHrtimPwmControllerId;
        static constexpr platform::HrtimTimerId      skVcaHrtimPwmTimerId      = platform::HrtimTimerId::kTimerD;

        static constexpr platform::AdcControllerId skCurrentSensorAdcControllerId = platform::AdcControllerId::kAdc2;
        static constexpr platform::AdcChannelId    skCurrentSensorAdcChannelId    = platform::AdcChannelId::kChannel2;

        static constexpr ACS724Model skCurrentSensorModel = ACS724Model::k2P5ABi;
        static constexpr auto        skCurrentSensorAdcHrtimTriggerSource =
            platform::AdcInjectGroupTriggerSource::kExtHrtimTRG2;
    };

}  // namespace valle::app

VALLE_DEFINE_HRTIM_CONTROLLER_CT_CONFIG(valle::app::kVcaHrtimPwmControllerId, valle::app::HrtimControllerCTConfig{});
VALLE_DEFINE_VCA_CURRENT_LOOP_DRIVER_CT_CONFIG(Intake,
                                               valle::app::kIntakeValveVcaCurrentLoopDriverId,
                                               valle::app::IntakeValveVcaCurrentLoopDriverCTConfig{});
VALLE_DEFINE_VCA_CURRENT_LOOP_DRIVER_CT_CONFIG(Exhaust,
                                               valle::app::kExhaustValveVcaCurrentLoopDriverId,
                                               valle::app::ExhaustValveVcaCurrentLoopDriverCTConfig{});
namespace valle
{
    namespace app
    {
        // ============================================================================
        // Drivers
        // ============================================================================

        // VCA Current Loop Driver Configurations
        using IntakeValveVcaCurrentLoopDriverT =
            platform::app::VCACurrentLoopDriver<kIntakeValveVcaCurrentLoopDriverId>;
        using IntakeValveVcaCurrentLoopDriverConfigT = typename IntakeValveVcaCurrentLoopDriverT::ConfigT;

        using ExhaustValveVcaCurrentLoopDriverT =
            platform::app::VCACurrentLoopDriver<kExhaustValveVcaCurrentLoopDriverId>;
        using ExhaustValveVcaCurrentLoopDriverConfigT = typename ExhaustValveVcaCurrentLoopDriverT::ConfigT;

        // Test GPIO Driver
        using TestGpioDriverT = platform::GpioDigitalOutDriver<platform::GpioPinB6>;

        // Declare Main Driver List
        using MainDriversT = TypeList<platform::CoreSystemDriver,
                                      UartLoggerT,
                                      IntakeValveVcaCurrentLoopDriverT,
                                      ExhaustValveVcaCurrentLoopDriverT,
                                      TestGpioDriverT>;

        // ============================================================================
        // Root Driver
        // ============================================================================
        using RootDevicesT = system_build::RootDevicesFromDriverList<MainDriversT>;
        struct RootDriver : PackedDriverBase<RootDevicesT>
        {
            using BaseT = PackedDriverBase<RootDevicesT>;
            using BaseT::BaseT;
        };

        // ============================================================================
        // Drivers Container
        // ============================================================================

        struct Drivers
        {
            using DriversT = typename TypeListAddFront<RootDriver, MainDriversT>::type;

            RootDriver                        root;
            platform::CoreSystemDriver        core;
            UartLoggerT                       uart_logger;
            IntakeValveVcaCurrentLoopDriverT  intake_vca_current_loop_driver;
            ExhaustValveVcaCurrentLoopDriverT exhaust_vca_current_loop_driver;
            TestGpioDriverT                   test_gpio;
        };

    }  // namespace app

    struct AppSystemBuildConfig : SystemBuildConfigBase<app::Drivers>
    {
    };

}  // namespace valle
