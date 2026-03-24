#pragma once

#include "valle/app/platform/modules/acs724.hpp"
#include "valle/app/platform/modules/amt10x_crank_encoder.hpp"
#include "valle/app/platform/modules/ldc161x.hpp"
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

    // VCA Current Loop Driver
    static constexpr uint8_t kVCACurrentLoopDriverID = 0;
    struct VCACurrentLoopDriverCTConfig
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

    // Position Sensor I2C Controller Config
    constexpr platform::I2CControllerID kPositionSensorI2CID             = platform::I2CControllerID::kI2C1;
    constexpr uint16_t                  kPositionSensorI2CAddress        = 0x2A;
    constexpr bool                      kPositionSensorI2CAddressIs10Bit = false;

    struct PositionSensorI2CControllerCTConfig : public platform::I2CControllerCTDefaultConfig
    {
        using DMAChannelRxT = platform::DMA1Channel3Device;
        using DMAChannelTxT = platform::DMA1Channel4Device;
    };

    // TIM for Quadrature Encoder
    constexpr auto kCrankEncoderTIMControllerID = platform::TIMControllerID::kTim2;
    struct CrankEncoderTIMControllerCTConfig : public platform::TIMControllerCTDefaultConfig
    {
        using Ch1PinT = platform::GPIOPinA15Device;
        using Ch2PinT = platform::GPIOPinB3Device;
    };

}  // namespace valle::app

// Bind compile-time configurations
VALLE_DEFINE_HRTIM_CONTROLLER_CT_CONFIG(valle::app::kVCAHRTIMPWMControllerID, valle::app::HRTIMControllerCTConfig{});
VALLE_DEFINE_VCA_CURRENT_LOOP_DRIVER_CT_CONFIG(valle::app::kVCACurrentLoopDriverID,
                                               valle::app::VCACurrentLoopDriverCTConfig{});
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
        using VCACurrentLoopDriverT       = platform::app::VCACurrentLoopDriver<kVCACurrentLoopDriverID>;
        using VCACurrentLoopDriverConfigT = typename VCACurrentLoopDriverT::ConfigT;

        using PositionSensorI2CControllerT  = platform::I2CCommandBufferDevice<kPositionSensorI2CID>;
        using PositionSensorI2CSlaveDeviceT = platform::I2CCommandBufferSlaveDevice<kPositionSensorI2CID,
                                                                                    kPositionSensorI2CAddress,
                                                                                    kPositionSensorI2CAddressIs10Bit>;
        using PositionSensorModuleT         = platform::app::LDC161XSensorModule<PositionSensorI2CSlaveDeviceT, 1>;
        using PositionSensorModuleConfigT   = typename PositionSensorModuleT::ConfigT;
        using PositionSensorConfigT         = typename PositionSensorModuleT::SensorConfigT;

        using AMT10xTIMControllerT = platform::TIMControllerDevice<kCrankEncoderTIMControllerID>;
        using AMT10xTIMEncoderModuleT =
            platform::app::AMT10xTIMEncoderModule<AMT10xTIMControllerT, platform::GPIOPinB5Device, AMT10xPPR::k2048>;
        using AMT10xTIMEncoderModuleConfigT = typename AMT10xTIMEncoderModuleT::ConfigT;

        using CrankEncoderModuleT       = platform::app::AMT10xCrankEncoderModuleX<AMT10xTIMEncoderModuleT>;
        using CrankEncoderModuleConfigT = typename CrankEncoderModuleT::ConfigT;

        using TestGPIODriverT = platform::GPIODigitalOutDriver<platform::GPIOPinB6Device>;

        // Declare Main Driver List
        using MainDriversT = TypeList<platform::CoreSystemDriver,
                                      UARTLoggerT,
                                      VCACurrentLoopDriverT,
                                      PositionSensorModuleT,
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

            VALLE_DEFINE_PACKED_DEVICE_DRIVER_ACCESSOR(i2c1, platform::I2C1CommandBufferDevice<>);
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
            PositionSensorModuleT      position_sensor;
            CrankEncoderModuleT        crank_encoder;
            TestGPIODriverT            test_gpio;
        };

    }  // namespace app

    struct AppSystemBuildConfig : SystemBuildConfigBase<app::Drivers>
    {
    };

}  // namespace valle
