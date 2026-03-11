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
    struct VCACurrentLoopDriverCTConfig
    {
        using PWMOutput1PinT              = platform::GPIOPinA8Device;
        using PWMOutput2PinT              = platform::GPIOPinA9Device;
        using CurrentSensorADCDMAChannelT = platform::DMA1Channel2Device;

        static constexpr platform::HRTIMControllerID skVCAHRTIMPWMControllerID = 1;
        static constexpr platform::HRTIMTimerID      skVCAHRTIMPWMTimerID      = platform::HRTIMTimerID::kA;

        static constexpr platform::ADCControllerID skCurrentSensorADCControllerID = 1;
        static constexpr platform::ADCChannelID    skCurrentSensorADCChannelId    = platform::ADCChannelID::kChannel1;

        static constexpr ACS724Model skCurrentSensorModel = ACS724Model::k2P5ABi;
        static constexpr auto        skCurrentSensorADCHRTIMTriggerSource =
            platform::ADCInjectGroupTriggerSource::kExtHrtimTRG2;
    };
}  // namespace valle::app

VALLE_DEFINE_VCA_CURRENT_LOOP_DRIVER_CT_CONFIG(valle::app::VCACurrentLoopDriverCTConfig{});

namespace valle
{
    namespace app
    {
        // ============================================================================
        // Drivers
        // ============================================================================
        using VCACurrentLoopDriverT       = platform::app::VCACurrentLoopDriver<>;
        using VCACurrentLoopDriverConfigT = typename VCACurrentLoopDriverT::ConfigT;

        using TestGPIODriverT = platform::GPIODigitalOutDriver<platform::GPIOPinB6Device>;

        // Declare Main Driver List
        using MainDriversT = TypeList<platform::CoreSystemDriver, UARTLoggerT, VCACurrentLoopDriverT, TestGPIODriverT>;

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
            UARTLoggerT                uart_logger;
            VCACurrentLoopDriverT      vca_current_loop_driver;
            TestGPIODriverT            test_gpio;
        };

    }  // namespace app

    struct AppSystemBuildConfig : SystemBuildConfigBase<app::Drivers>
    {
    };

}  // namespace valle
