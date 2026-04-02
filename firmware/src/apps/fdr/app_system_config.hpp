#pragma once

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
    static constexpr platform::HrtimControllerId kVcaHrtimPwmControllerId = platform::HrtimControllerId::kHrtim1;
    struct HrtimControllerCTConfig : public platform::HrtimControllerCTDefaultConfig
    {
    };

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

    // TIM for Quadrature Encoder
    constexpr auto kCrankEncoderTimControllerId = platform::TimControllerId::kTim2;
    struct CrankEncoderTimControllerCTConfig : public platform::TimControllerCTDefaultConfig
    {
        using Ch1PinT = platform::GpioPinA15;
        using Ch2PinT = platform::GpioPinB3;
    };
}  // namespace valle::app

VALLE_DEFINE_HRTIM_CONTROLLER_CT_CONFIG(valle::app::kVcaHrtimPwmControllerId, valle::app::HrtimControllerCTConfig{});
VALLE_DEFINE_VCA_CURRENT_LOOP_DRIVER_CT_CONFIG(Test,
                                               valle::app::kVcaCurrentLoopDriverId,
                                               valle::app::VCACurrentLoopDriverCTConfig{});
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
            CrankEncoderModuleT        crank_encoder;
            TestGpioDriverT            test_gpio;
        };

    }  // namespace app

    struct AppSystemBuildConfig : SystemBuildConfigBase<app::Drivers>
    {
    };

}  // namespace valle
