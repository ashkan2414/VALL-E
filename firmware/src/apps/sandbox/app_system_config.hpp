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
    static constexpr platform::HrtimControllerId kVcaHrtimPwmControllerId = platform::HrtimControllerId::kHrtim1;
    struct HrtimControllerCTConfig : public platform::HrtimControllerCTDefaultConfig
    {
    };

    // VCA Current Loop Driver
    static constexpr uint8_t kVcaCurrentLoopDriverId = 0;
    struct VCACurrentLoopDriverCTConfig
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

    // Position Sensor I2C Controller Config
    constexpr platform::I2cControllerId kPositionSensorI2cId             = platform::I2cControllerId::kI2c1;
    constexpr uint16_t                  kPositionSensorI2cAddress        = 0x2A;
    constexpr bool                      kPositionSensorI2cAddressIs10Bit = false;

    struct PositionSensorI2cControllerCTConfig : public platform::I2cControllerCTDefaultConfig
    {
        using DmaChannelRxT = platform::Dma1Channel3;
        using DmaChannelTxT = platform::Dma1Channel4;
    };

    // TIM for Quadrature Encoder
    constexpr auto kCrankEncoderTimControllerId = platform::TimControllerId::kTim2;
    struct CrankEncoderTimControllerCTConfig : public platform::TimControllerCTDefaultConfig
    {
        using Ch1PinT = platform::GpioPinA15;
        using Ch2PinT = platform::GpioPinB3;
    };

}  // namespace valle::app

// Bind compile-time configurations
VALLE_DEFINE_HRTIM_CONTROLLER_CT_CONFIG(valle::app::kVcaHrtimPwmControllerId, valle::app::HrtimControllerCTConfig{});
VALLE_DEFINE_VCA_CURRENT_LOOP_DRIVER_CT_CONFIG(valle::app::kVcaCurrentLoopDriverId,
                                               valle::app::VCACurrentLoopDriverCTConfig{});
VALLE_DEFINE_I2C_CONTROLLER_CT_CONFIG(valle::app::kPositionSensorI2cId,
                                      valle::app::PositionSensorI2cControllerCTConfig{});
VALLE_DEFINE_TIMER_CONTROLLER_CT_CONFIG(valle::app::kCrankEncoderTimControllerId,
                                        valle::app::CrankEncoderTimControllerCTConfig{});

namespace valle
{
    namespace app
    {
        // ============================================================================
        // Drivers
        // ============================================================================
        using VCACurrentLoopDriverT       = platform::app::VCACurrentLoopDriver<kVcaCurrentLoopDriverId>;
        using VCACurrentLoopDriverConfigT = typename VCACurrentLoopDriverT::ConfigT;

        using PositionSensorI2cControllerT  = platform::I2cCommandBuffer<kPositionSensorI2cId>;
        using PositionSensorI2cSlaveDeviceT = platform::
            I2cCommandBufferSlave<kPositionSensorI2cId, kPositionSensorI2cAddress, kPositionSensorI2cAddressIs10Bit>;
        using PositionSensorModuleT       = platform::app::LDC161XSensorModule<PositionSensorI2cSlaveDeviceT, 1>;
        using PositionSensorModuleConfigT = typename PositionSensorModuleT::ConfigT;
        using PositionSensorConfigT       = typename PositionSensorModuleT::SensorConfigT;

        using AMT10xTimControllerT = platform::TimController<kCrankEncoderTimControllerId>;
        using AMT10xTimEncoderModuleT =
            platform::app::AMT10xTimEncoderModule<AMT10xTimControllerT, platform::GpioPinB5, AMT10xPPR::k2048>;
        using AMT10xTimEncoderModuleConfigT = typename AMT10xTimEncoderModuleT::ConfigT;

        using CrankEncoderModuleT       = platform::app::AMT10xCrankEncoderModuleX<AMT10xTimEncoderModuleT>;
        using CrankEncoderModuleConfigT = typename CrankEncoderModuleT::ConfigT;

        using TestGpioDriverT = platform::GpioDigitalOutDriver<platform::GpioPinB6>;

        // Declare Main Driver List
        using MainDriversT = TypeList<platform::CoreSystemDriver,
                                      UartLoggerT,
                                      VCACurrentLoopDriverT,
                                      PositionSensorModuleT,
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

            VALLE_DEFINE_PACKED_DEVICE_DRIVER_ACCESSOR(i2c1, platform::I2c1CommandBuffer<>);
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
            PositionSensorModuleT      position_sensor;
            CrankEncoderModuleT        crank_encoder;
            TestGpioDriverT            test_gpio;
        };

    }  // namespace app

    struct AppSystemBuildConfig : SystemBuildConfigBase<app::Drivers>
    {
    };

}  // namespace valle
