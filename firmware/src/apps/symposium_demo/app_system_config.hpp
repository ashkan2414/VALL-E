#pragma once

#include "valle/app/platform/ldc161x_position_loop_driver.hpp"
#include "valle/app/platform/modules/acs724.hpp"
#include "valle/app/platform/modules/amt10x_crank_encoder.hpp"
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
    static constexpr platform::HRTIMControllerID kVCAHRTIMPWMControllerID = 1;
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

        static constexpr platform::ADCControllerID skCurrentSensorADCControllerID = 1;
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

        static constexpr platform::ADCControllerID skCurrentSensorADCControllerID = 2;
        static constexpr platform::ADCChannelID    skCurrentSensorADCChannelId    = platform::ADCChannelID::kChannel2;

        static constexpr ACS724Model skCurrentSensorModel = ACS724Model::k2P5ABi;
        static constexpr auto        skCurrentSensorADCHRTIMTriggerSource =
            platform::ADCInjectGroupTriggerSource::kExtHrtimTRG2;
    };

    // Position Sensor I2C Controller Config
    constexpr platform::I2CControllerID kPositionSensorI2CID             = 1;
    constexpr uint16_t                  kPositionSensorI2CAddress        = 0x2B;
    constexpr bool                      kPositionSensorI2CAddressIs10Bit = false;

    struct PositionSensorI2CControllerCTConfig : public platform::I2CControllerCTDefaultConfig
    {
        using DMAChannelRxT = platform::DMA1Channel4Device;
        using DMAChannelTxT = platform::DMA1Channel5Device;
        using SCLPinT       = platform::GPIOPinB8Device;
        using SDAPinT       = platform::GPIOPinB9Device;
    };

    // TIM for Quadrature Encoder
    constexpr auto kCrankEncoderTIMControllerID = platform::TIMControllerID::kTim2;
    struct CrankEncoderTIMControllerCTConfig : public platform::TIMControllerCTDefaultConfig
    {
        using Ch1PinT = platform::GPIOPinA15Device;
        using Ch2PinT = platform::GPIOPinB3Device;
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
VALLE_DEFINE_TIMER_CONTROLLER_CT_CONFIG(valle::app::kCrankEncoderTIMControllerID,
                                        valle::app::CrankEncoderTIMControllerCTConfig{});
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

        using PositionSensorINTBPinT = platform::GPIOPinA10Device;
        using PositionSensorModuleT =
            platform::app::LDC161XSensorModule<PositionSensorI2CSlaveDeviceT, 2, PositionSensorINTBPinT>;
        using PositionSensorModuleConfigT = typename PositionSensorModuleT::ConfigT;
        using PositionSensorConfigT       = typename PositionSensorModuleT::SensorConfigT;

        static constexpr uint8_t kPositionSensorFrequencyToDistanceTableSize = 15;
        using PositionSensorConverterT =
            LDC161XFrequencyToDistanceConverter<kPositionSensorFrequencyToDistanceTableSize>;
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
                                                     IntakeValvePositionLoopDriverVCAControllerInterfaceT,
                                                     ExhaustValvePositionLoopDriverVCAControllerInterfaceT>;
        using PositionLoopDriverConfigT = typename PositionLoopDriverT::ConfigT;

        // AMT10x Crank Encoder Configurations
        using AMT10xTIMControllerT = platform::TIMControllerDevice<kCrankEncoderTIMControllerID>;
        using AMT10xTIMEncoderModuleT =
            platform::app::AMT10xTIMEncoderModule<AMT10xTIMControllerT, platform::GPIOPinB5Device, AMT10xPPR::k2048>;
        using AMT10xTIMEncoderModuleConfigT = typename AMT10xTIMEncoderModuleT::ConfigT;

        using CrankEncoderModuleT       = platform::app::AMT10xCrankEncoderModuleX<AMT10xTIMEncoderModuleT>;
        using CrankEncoderModuleConfigT = typename CrankEncoderModuleT::ConfigT;

        // Test GPIO Driver
        using TestGPIODriverT = platform::GPIODigitalOutDriver<platform::GPIOPinB6Device>;

        // Declare Main Driver List
        using MainDriversT = TypeList<platform::CoreSystemDriver,
                                      UARTLoggerT,
                                      IntakeValveVCACurrentLoopDriverT,
                                      ExhaustValveVCACurrentLoopDriverT,
                                      PositionLoopDriverT,
                                      CrankEncoderModuleT,
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
            PositionLoopDriverT               position_loop_driver;
            CrankEncoderModuleT               crank_encoder;
            TestGPIODriverT                   test_gpio;
        };

    }  // namespace app

    struct AppSystemBuildConfig : SystemBuildConfigBase<app::Drivers>
    {
    };

}  // namespace valle
