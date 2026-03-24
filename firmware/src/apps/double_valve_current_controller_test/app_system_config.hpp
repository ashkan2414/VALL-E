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
    static constexpr platform::HRTIMControllerID kVCAHRTIMPWMControllerID = platform::HRTIMControllerID::kHRTIM1;
    struct HRTIMControllerCTConfig : public platform::HRTIMControllerCTDefaultConfig
    {
    };

    static constexpr uint8_t kIntakeValveVCACurrentLoopDriverID = 0;
    struct IntakeValveVCACurrentLoopDriverCTConfig
    {
        using PWMOutput1PinT              = platform::GPIOPinA8Device;
        using PWMOutput2PinT              = platform::GPIOPinA9Device;
        using CurrentSensorADCDMAChannelT = platform::DMA1Channel2Device;

        static constexpr platform::HRTIMControllerID skVCAHRTIMPWMControllerID = kVCAHRTIMPWMControllerID;
        static constexpr platform::HRTIMTimerID      skVCAHRTIMPWMTimerID      = platform::HRTIMTimerID::kA;

        static constexpr platform::ADCControllerID skCurrentSensorADCControllerID = platform::ADCControllerID::kADC1;
        static constexpr platform::ADCChannelID    skCurrentSensorADCChannelId    = platform::ADCChannelID::kChannel1;

        static constexpr ACS724Model skCurrentSensorModel = ACS724Model::k2P5ABi;
        static constexpr auto        skCurrentSensorADCHRTIMTriggerSource =
            platform::ADCInjectGroupTriggerSource::kExtHrtimTRG2;
    };

    static constexpr uint8_t kExhaustValveVCACurrentLoopDriverID = 1;
    struct ExhaustValveVCACurrentLoopDriverCTConfig
    {
        using PWMOutput1PinT              = platform::GPIOPinB14Device;
        using PWMOutput2PinT              = platform::GPIOPinB15Device;
        using CurrentSensorADCDMAChannelT = platform::DMA1Channel3Device;

        static constexpr platform::HRTIMControllerID skVCAHRTIMPWMControllerID = kVCAHRTIMPWMControllerID;
        static constexpr platform::HRTIMTimerID      skVCAHRTIMPWMTimerID      = platform::HRTIMTimerID::kD;

        static constexpr platform::ADCControllerID skCurrentSensorADCControllerID = platform::ADCControllerID::kADC2;
        static constexpr platform::ADCChannelID    skCurrentSensorADCChannelId    = platform::ADCChannelID::kChannel2;

        static constexpr ACS724Model skCurrentSensorModel = ACS724Model::k2P5ABi;
        static constexpr auto        skCurrentSensorADCHRTIMTriggerSource =
            platform::ADCInjectGroupTriggerSource::kExtHrtimTRG2;
    };

}  // namespace valle::app

VALLE_DEFINE_HRTIM_CONTROLLER_CT_CONFIG(valle::app::kVCAHRTIMPWMControllerID, valle::app::HRTIMControllerCTConfig{});
VALLE_DEFINE_VCA_CURRENT_LOOP_DRIVER_CT_CONFIG(Intake,
                                               valle::app::kIntakeValveVCACurrentLoopDriverID,
                                               valle::app::IntakeValveVCACurrentLoopDriverCTConfig{});
VALLE_DEFINE_VCA_CURRENT_LOOP_DRIVER_CT_CONFIG(Exhaust,
                                               valle::app::kExhaustValveVCACurrentLoopDriverID,
                                               valle::app::ExhaustValveVCACurrentLoopDriverCTConfig{});
namespace valle
{
    namespace app
    {
        // ============================================================================
        // Drivers
        // ============================================================================

        // VCA Current Loop Driver Configurations
        using IntakeValveVCACurrentLoopDriverT =
            platform::app::VCACurrentLoopDriver<kIntakeValveVCACurrentLoopDriverID>;
        using IntakeValveVCACurrentLoopDriverConfigT = typename IntakeValveVCACurrentLoopDriverT::ConfigT;

        using ExhaustValveVCACurrentLoopDriverT =
            platform::app::VCACurrentLoopDriver<kExhaustValveVCACurrentLoopDriverID>;
        using ExhaustValveVCACurrentLoopDriverConfigT = typename ExhaustValveVCACurrentLoopDriverT::ConfigT;

        // Test GPIO Driver
        using TestGPIODriverT = platform::GPIODigitalOutDriver<platform::GPIOPinB6Device>;

        // Declare Main Driver List
        using MainDriversT = TypeList<platform::CoreSystemDriver,
                                      UARTLoggerT,
                                      IntakeValveVCACurrentLoopDriverT,
                                      ExhaustValveVCACurrentLoopDriverT,
                                      TestGPIODriverT>;

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
            UARTLoggerT                       uart_logger;
            IntakeValveVCACurrentLoopDriverT  intake_vca_current_loop_driver;
            ExhaustValveVCACurrentLoopDriverT exhaust_vca_current_loop_driver;
            TestGPIODriverT                   test_gpio;
        };

    }  // namespace app

    struct AppSystemBuildConfig : SystemBuildConfigBase<app::Drivers>
    {
    };

}  // namespace valle
