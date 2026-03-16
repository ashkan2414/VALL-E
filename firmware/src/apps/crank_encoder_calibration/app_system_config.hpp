#pragma once

#include "valle/app/platform/modules/amt10x_crank_encoder.hpp"
#include "valle/app/platform/uart_logger_config.hpp"
#include "valle/base/system_build/config_base.hpp"
#include "valle/base/system_build/traits.hpp"
#include "valle/platform/drivers/core_system.hpp"
#include "valle/platform/drivers/uart/logger.hpp"

namespace valle::app
{
    // TIM for Quadrature CrankEncoder
    constexpr auto kCrankEncoderTIMControllerID = platform::TIMControllerID::kTim2;
    struct CrankEncoderTIMControllerCTConfig : platform::TIMControllerCTDefaultConfig
    {
        using Ch1PinT = platform::GPIOPinA15Device;
        using Ch2PinT = platform::GPIOPinB3Device;
    };

}  // namespace valle::app

VALLE_DEFINE_TIMER_CONTROLLER_CT_CONFIG(valle::app::kCrankEncoderTIMControllerID,
                                        valle::app::CrankEncoderTIMControllerCTConfig{});

namespace valle
{
    namespace app
    {
        // ============================================================================
        // Drivers
        // ============================================================================
        using AMT10xTIMControllerT                  = platform::TIMControllerDevice<kCrankEncoderTIMControllerID>;
        using AMT10xTIMEncoderModuleT =
            platform::app::AMT10xTIMEncoderModule<AMT10xTIMControllerT, platform::GPIOPinB5Device, AMT10xPPR::k2048>;
        using AMT10xTIMEncoderModuleConfigT = typename AMT10xTIMEncoderModuleT::ConfigT;

        using CrankEncoderModuleT       = platform::app::AMT10xCrankEncoderModuleX<AMT10xTIMEncoderModuleT>;
        using CrankEncoderModuleConfigT = typename CrankEncoderModuleT::ConfigT;

        // Declare Main Driver List
        using MainDriversT = TypeList<platform::CoreSystemDriver, UARTLoggerT, CrankEncoderModuleT>;

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
            CrankEncoderModuleT        crank_encoder;
        };

    }  // namespace app

    struct AppSystemBuildConfig : SystemBuildConfigBase<app::Drivers>
    {
    };

}  // namespace valle
