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
    static constexpr platform::HRTIMControllerID kVCAHRTIMPWMControllerID = 1;
    struct HRTIMControllerCTConfig : public platform::HRTIMControllerCTDefaultConfig
    {
    };

    static constexpr uint8_t kVCACurrentLoopDriverID = 0;
    struct VCACurrentLoopDriverCTConfig
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

    // TIM for Quadrature Encoder
    constexpr auto kCrankEncoderTIMControllerID = platform::TIMControllerID::kTim2;
    struct CrankEncoderTIMControllerCTConfig : public platform::TIMControllerCTDefaultConfig
    {
        using Ch1PinT = platform::GPIOPinA15Device;
        using Ch2PinT = platform::GPIOPinB3Device;
    };
}  // namespace valle::app

VALLE_DEFINE_HRTIM_CONTROLLER_CT_CONFIG(valle::app::kVCAHRTIMPWMControllerID, valle::app::HRTIMControllerCTConfig{});
VALLE_DEFINE_VCA_CURRENT_LOOP_DRIVER_CT_CONFIG(valle::app::kVCACurrentLoopDriverID,
                                               valle::app::VCACurrentLoopDriverCTConfig{});
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

            RootDriver                 root;
            platform::CoreSystemDriver core;
            UARTLoggerT                uart_logger;
            VCACurrentLoopDriverT      vca_current_loop_driver;
            CrankEncoderModuleT        crank_encoder;
            TestGPIODriverT            test_gpio;
        };

    }  // namespace app

    struct AppSystemBuildConfig : SystemBuildConfigBase<app::Drivers>
    {
    };

}  // namespace valle
