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
    static constexpr platform::HrtimPeripheralId kVcaHrtimPwmPeripheralId = platform::HrtimPeripheralId::kHrtim1;
    struct HrtimControllerCTConfig : public platform::HrtimControllerCTDefaultConfig
    {
    };

    static constexpr uint8_t kVcaCurrentLoopDriverId = 0;
    struct VCACurrentLoopDriverCTConfig
    {
        using PWMOutput1PinT              = platform::GpioPinA8Device;
        using PWMOutput2PinT              = platform::GpioPinA9Device;
        using CurrentSensorAdcDmaChannelT = platform::Dma1Channel2Device;

        static constexpr platform::HrtimPeripheralId skVcaHrtimPwmPeripheralId = kVcaHrtimPwmPeripheralId;
        static constexpr platform::HrtimTimerId      skVcaHrtimPwmTimerId      = platform::HrtimTimerId::kTimerA;

        static constexpr platform::AdcPeripheralId skCurrentSensorAdcPeripheralId = platform::AdcPeripheralId::kAdc1;
        static constexpr platform::AdcChannelId    skCurrentSensorAdcChannelId    = platform::AdcChannelId::kChannel1;

        static constexpr ACS724Model skCurrentSensorModel = ACS724Model::k2P5ABi;
        static constexpr auto        skCurrentSensorAdcHrtimTriggerSource =
            platform::AdcInjectGroupTriggerSource::kExtHrtimTRG2;
    };
}  // namespace valle::app

VALLE_DEFINE_HRTIM_CONTROLLER_CT_CONFIG(valle::app::kVcaHrtimPwmPeripheralId, valle::app::HrtimControllerCTConfig{});
VALLE_DEFINE_VCA_CURRENT_LOOP_DRIVER_CT_CONFIG(Test,
                                               valle::app::kVcaCurrentLoopDriverId,
                                               valle::app::VCACurrentLoopDriverCTConfig{});

namespace valle
{
    namespace app
    {
        // ============================================================================
        // Drivers
        // ============================================================================
        using VCACurrentLoopDriverT       = platform::app::VCACurrentLoopDriver<kVcaCurrentLoopDriverId>;
        using VCACurrentLoopDriverConfigT = typename VCACurrentLoopDriverT::ConfigT;

        using TestGpioDriverT = platform::GpioDigitalOutDriver<platform::GpioPinB6Device>;

        // Declare Main Driver List
        using MainDriversT = TypeList<platform::CoreSystemDriver, UartLoggerT, VCACurrentLoopDriverT, TestGpioDriverT>;

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

            RootDriver                 root;
            platform::CoreSystemDriver core;
            UartLoggerT                uart_logger;
            VCACurrentLoopDriverT      vca_current_loop_driver;
            TestGpioDriverT            test_gpio;
        };

    }  // namespace app

    struct AppSystemBuildConfig : SystemBuildConfigBase<app::Drivers>
    {
    };

}  // namespace valle
