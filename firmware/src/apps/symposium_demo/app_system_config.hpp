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
    static constexpr platform::HrtimPeripheralId kVcaHrtimPwmId = platform::HrtimPeripheralId::kHrtim1;
    struct HrtimControllerCTConfig : public platform::HrtimControllerCTDefaultConfig
    {
    };

    static constexpr uint8_t kIntakeValveVcaCurrentLoopDriverId = 0;
    struct IntakeValveVcaCurrentLoopDriverCTConfig
    {
        using PWMOutput1PinT              = platform::GpioPinA8Device;
        using PWMOutput2PinT              = platform::GpioPinA9Device;
        using CurrentSensorAdcDmaChannelT = platform::Dma1Channel2Device;

        static constexpr platform::HrtimPeripheralId skVcaHrtimPwmId      = kVcaHrtimPwmId;
        static constexpr platform::HrtimTimerId      skVcaHrtimPwmTimerId = platform::HrtimTimerId::kTimerA;

        static constexpr platform::AdcPeripheralId skCurrentSensorAdcId        = platform::AdcPeripheralId::kAdc1;
        static constexpr platform::AdcChannelId    skCurrentSensorAdcChannelId = platform::AdcChannelId::kChannel1;

        static constexpr ACS724Model skCurrentSensorModel = ACS724Model::k2P5ABi;
        static constexpr auto        skCurrentSensorAdcHrtimTriggerSource =
            platform::AdcInjectGroupTriggerSource::kExtHrtimTRG2;
    };

    static constexpr uint8_t kExhaustValveVcaCurrentLoopDriverId = 1;
    struct ExhaustValveVcaCurrentLoopDriverCTConfig
    {
        using PWMOutput1PinT              = platform::GpioPinB14Device;
        using PWMOutput2PinT              = platform::GpioPinB15Device;
        using CurrentSensorAdcDmaChannelT = platform::Dma1Channel3Device;

        static constexpr platform::HrtimPeripheralId skVcaHrtimPwmId      = kVcaHrtimPwmId;
        static constexpr platform::HrtimTimerId      skVcaHrtimPwmTimerId = platform::HrtimTimerId::kTimerD;

        static constexpr platform::AdcPeripheralId skCurrentSensorAdcId        = platform::AdcPeripheralId::kAdc2;
        static constexpr platform::AdcChannelId    skCurrentSensorAdcChannelId = platform::AdcChannelId::kChannel2;

        static constexpr ACS724Model skCurrentSensorModel = ACS724Model::k2P5ABi;
        static constexpr auto        skCurrentSensorAdcHrtimTriggerSource =
            platform::AdcInjectGroupTriggerSource::kExtHrtimTRG2;
    };

    // Position Sensor I2C Controller Config
    constexpr platform::I2cPeripheralId kPositionSensorI2cId             = platform::I2cPeripheralId::kI2c1;
    constexpr uint16_t                  kPositionSensorI2cAddress        = 0x2B;
    constexpr bool                      kPositionSensorI2cAddressIs10Bit = false;

    struct PositionSensorI2cControllerCTConfig : public platform::I2cControllerCTDefaultConfig
    {
        using DmaChannelRxT = platform::Dma1Channel4Device;
        using DmaChannelTxT = platform::Dma1Channel5Device;
        using SCLPinT       = platform::GpioPinB8Device;
        using SDAPinT       = platform::GpioPinB9Device;
    };

    // TIM for Quadrature Encoder
    constexpr auto kCrankEncoderTimId = platform::TimPeripheralId::kTim2;
    struct CrankEncoderTimControllerCTConfig : public platform::TimControllerCTDefaultConfig
    {
        using Ch1PinT = platform::GpioPinA15Device;
        using Ch2PinT = platform::GpioPinB3Device;
    };
}  // namespace valle::app

VALLE_DEFINE_HRTIM_CONTROLLER_CT_CONFIG(valle::app::kVcaHrtimPwmId, valle::app::HrtimControllerCTConfig{});
VALLE_DEFINE_VCA_CURRENT_LOOP_DRIVER_CT_CONFIG(Intake,
                                               valle::app::kIntakeValveVcaCurrentLoopDriverId,
                                               valle::app::IntakeValveVcaCurrentLoopDriverCTConfig{});
VALLE_DEFINE_VCA_CURRENT_LOOP_DRIVER_CT_CONFIG(Exhaust,
                                               valle::app::kExhaustValveVcaCurrentLoopDriverId,
                                               valle::app::ExhaustValveVcaCurrentLoopDriverCTConfig{});
VALLE_DEFINE_I2C_CONTROLLER_CT_CONFIG(valle::app::kPositionSensorI2cId,
                                      valle::app::PositionSensorI2cControllerCTConfig{});
VALLE_DEFINE_TIMER_CONTROLLER_CT_CONFIG(valle::app::kCrankEncoderTimId,
                                        valle::app::CrankEncoderTimControllerCTConfig{});
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

        // Position Sensor (LDC161X) Configurations
        using PositionSensorI2cControllerT  = platform::I2cCommandBufferDevice<kPositionSensorI2cId>;
        using PositionSensorI2cSlaveDeviceT = platform::I2cCommandBufferSlaveDevice<kPositionSensorI2cId,
                                                                                    kPositionSensorI2cAddress,
                                                                                    kPositionSensorI2cAddressIs10Bit>;

        using PositionSensorINTBPinT = platform::GpioPinC11Device;
        using PositionSensorModuleT =
            platform::app::LDC161XSensorModule<PositionSensorI2cSlaveDeviceT, 2, PositionSensorINTBPinT>;
        using PositionSensorModuleConfigT = typename PositionSensorModuleT::ConfigT;
        using PositionSensorConfigT       = typename PositionSensorModuleT::SensorConfigT;

        using PositionSensorConverterT       = LDC161XFrequencyToDistanceTableConverter<9>;
        using PositionSensorConverterConfigT = typename PositionSensorConverterT::ConfigT;

        using IntakeValvePositionLoopDriverVcaControllerInterfaceT =
            platform::app::LDC161XPositionLoopDriverVcaValveInterface<IntakeValveVcaCurrentLoopDriverT>;
        using IntakeValvePositionVcaControllerInterfaceConfigT =
            typename IntakeValvePositionLoopDriverVcaControllerInterfaceT::ConfigT;

        using ExhaustValvePositionLoopDriverVcaControllerInterfaceT =
            platform::app::LDC161XPositionLoopDriverVcaValveInterface<ExhaustValveVcaCurrentLoopDriverT>;
        using ExhaustValvePositionVcaControllerInterfaceConfigT =
            typename ExhaustValvePositionLoopDriverVcaControllerInterfaceT::ConfigT;

        using PositionLoopDriverT =
            platform::app::LDC161XPositionLoopDriver<PositionSensorModuleT,
                                                     PositionSensorConverterT,
                                                     ExhaustValvePositionLoopDriverVcaControllerInterfaceT,
                                                     IntakeValvePositionLoopDriverVcaControllerInterfaceT>;
        using PositionLoopDriverConfigT = typename PositionLoopDriverT::ConfigT;

        static constexpr LDC161XChannel kExhaustValvePositionChannel = LDC161XChannel::kChannel0;
        static constexpr LDC161XChannel kIntakeValvePositionChannel  = LDC161XChannel::kChannel1;

        // AMT10x Crank Encoder Configurations
        using AMT10xTimControllerT = platform::TimControllerDevice<kCrankEncoderTimId>;
        using AMT10xTimEncoderModuleT =
            platform::app::AMT10xTimEncoderModule<AMT10xTimControllerT, platform::GpioPinB5Device, AMT10xPPR::k2048>;
        using AMT10xTimEncoderModuleConfigT = typename AMT10xTimEncoderModuleT::ConfigT;

        using CrankEncoderModuleT       = platform::app::AMT10xCrankEncoderModuleX<AMT10xTimEncoderModuleT>;
        using CrankEncoderModuleConfigT = typename CrankEncoderModuleT::ConfigT;

        // Test GPIO Driver
        using TestGpioDriverT = platform::GpioDigitalOutDriver<platform::GpioPinB6Device>;

        // Declare Main Driver List
        using MainDriversT = TypeList<platform::CoreSystemDriver,
                                      UartLoggerT,
                                      IntakeValveVcaCurrentLoopDriverT,
                                      ExhaustValveVcaCurrentLoopDriverT,
                                      PositionLoopDriverT,
                                      CrankEncoderModuleT,
                                      TestGpioDriverT>;

        // ============================================================================
        // Root Driver
        // ============================================================================
        using RootDevicesT = system_build::RootDevicesFromDriverList<MainDriversT>;
        struct RootDriver : PackedDriverBase<RootDevicesT>
        {
            using BaseT = PackedDriverBase<RootDevicesT>;
            using BaseT::BaseT;

            VALLE_DEFINE_PACKED_DEVICE_DRIVER_ACCESSOR(i2c1, platform::I2c1CommandBufferDevice<>);
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
            PositionLoopDriverT               position_loop_driver;
            CrankEncoderModuleT               crank_encoder;
            TestGpioDriverT                   test_gpio;
        };

    }  // namespace app

    struct AppSystemBuildConfig : SystemBuildConfigBase<app::Drivers>
    {
    };

}  // namespace valle
