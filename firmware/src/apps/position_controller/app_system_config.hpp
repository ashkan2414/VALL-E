#pragma once

#include "valle/app/platform/ldc161x_position_loop_driver.hpp"
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

    // Position Sensor I2C Controller Config
    constexpr platform::I2CControllerID kPositionSensorI2CID             = platform::I2CControllerID::kI2C1;
    constexpr uint16_t                  kPositionSensorI2CAddress        = 0x2B;
    constexpr bool                      kPositionSensorI2CAddressIs10Bit = false;

    struct PositionSensorI2CControllerCTConfig : public platform::I2CControllerCTDefaultConfig
    {
        using DMAChannelRxT = platform::DMA1Channel4Device;
        using DMAChannelTxT = platform::DMA1Channel5Device;
        using SCLPinT       = platform::GPIOPinB8Device;
        using SDAPinT       = platform::GPIOPinB9Device;
    };
}  // namespace valle::app

VALLE_DEFINE_HRTIM_CONTROLLER_CT_CONFIG(valle::app::kVCAHRTIMPWMControllerID, valle::app::HRTIMControllerCTConfig{});
VALLE_DEFINE_VCA_CURRENT_LOOP_DRIVER_CT_CONFIG(Intake,
                                               valle::app::kIntakeValveVCACurrentLoopDriverID,
                                               valle::app::IntakeValveVCACurrentLoopDriverCTConfig{});
VALLE_DEFINE_VCA_CURRENT_LOOP_DRIVER_CT_CONFIG(Exhaust,
                                               valle::app::kExhaustValveVCACurrentLoopDriverID,
                                               valle::app::ExhaustValveVCACurrentLoopDriverCTConfig{});
VALLE_DEFINE_I2C_CONTROLLER_CT_CONFIG(valle::app::kPositionSensorI2CID,
                                      valle::app::PositionSensorI2CControllerCTConfig{});

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

        // Position Sensor (LDC161X) Configurations
        using PositionSensorI2CControllerT  = platform::I2CCommandBufferDevice<kPositionSensorI2CID>;
        using PositionSensorI2CSlaveDeviceT = platform::I2CCommandBufferSlaveDevice<kPositionSensorI2CID,
                                                                                    kPositionSensorI2CAddress,
                                                                                    kPositionSensorI2CAddressIs10Bit>;

        using PositionSensorINTBPinT = platform::GPIOPinC11Device;
        using PositionSensorModuleT =
            platform::app::LDC161XSensorModule<PositionSensorI2CSlaveDeviceT, 2, PositionSensorINTBPinT>;
        using PositionSensorModuleConfigT = typename PositionSensorModuleT::ConfigT;
        using PositionSensorConfigT       = typename PositionSensorModuleT::SensorConfigT;

        using PositionSensorConverterT       = LDC161XFrequencyToDistanceTableConverter<9>;
        using PositionSensorConverterConfigT = typename PositionSensorConverterT::ConfigT;

        using IntakeValvePositionLoopDriverVCAControllerInterfaceT =
            platform::app::LDC161XPositionLoopDriverVCAValveInterface<IntakeValveVCACurrentLoopDriverT>;
        using IntakeValvePositionVCAControllerInterfaceConfigT =
            typename IntakeValvePositionLoopDriverVCAControllerInterfaceT::ConfigT;

        using ExhaustValvePositionLoopDriverVCAControllerInterfaceT =
            platform::app::LDC161XPositionLoopDriverVCAValveInterface<ExhaustValveVCACurrentLoopDriverT>;
        using ExhaustValvePositionVCAControllerInterfaceConfigT =
            typename ExhaustValvePositionLoopDriverVCAControllerInterfaceT::ConfigT;

        using PositionLoopDriverT =
            platform::app::LDC161XPositionLoopDriver<PositionSensorModuleT,
                                                     PositionSensorConverterT,
                                                     ExhaustValvePositionLoopDriverVCAControllerInterfaceT,
                                                     IntakeValvePositionLoopDriverVCAControllerInterfaceT>;
        using PositionLoopDriverConfigT = typename PositionLoopDriverT::ConfigT;

        static constexpr LDC161XChannel kExhaustValvePositionChannel = LDC161XChannel::kChannel0;
        static constexpr LDC161XChannel kIntakeValvePositionChannel  = LDC161XChannel::kChannel1;

        // Test GPIO Driver
        using TestGPIODriverT = platform::GPIODigitalOutDriver<platform::GPIOPinB6Device>;

        // Declare Main Driver List
        using MainDriversT = TypeList<platform::CoreSystemDriver,
                                      UARTLoggerT,
                                      IntakeValveVCACurrentLoopDriverT,
                                      ExhaustValveVCACurrentLoopDriverT,
                                      PositionLoopDriverT,
                                      TestGPIODriverT>;

        // ============================================================================
        // Root Driver
        // ============================================================================
        using RootDevicesT = system_build::RootDevicesFromDriverList<MainDriversT>;
        struct RootDriver : PackedDriverBase<RootDevicesT>
        {
            using BaseT = PackedDriverBase<RootDevicesT>;
            using BaseT::BaseT;

            VALLE_DEFINE_PACKED_DEVICE_DRIVER_ACCESSOR(i2c1, platform::I2C1CommandBufferDevice<>);
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
            PositionLoopDriverT               position_loop_driver;
            TestGPIODriverT                   test_gpio;
        };

    }  // namespace app

    struct AppSystemBuildConfig : SystemBuildConfigBase<app::Drivers>
    {
    };

}  // namespace valle
