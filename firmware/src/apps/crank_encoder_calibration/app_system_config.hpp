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
    constexpr auto kCrankEncoderTimControllerId = platform::TimControllerId::kTim2;
    struct CrankEncoderTimControllerCTConfig : public platform::TimControllerCTDefaultConfig
    {
        using Ch1PinT = platform::GpioPinA15;
        using Ch2PinT = platform::GpioPinB3;
    };

}  // namespace valle::app

VALLE_DEFINE_TIMER_CONTROLLER_CT_CONFIG(valle::app::kCrankEncoderTimControllerId,
                                        valle::app::CrankEncoderTimControllerCTConfig{});

namespace valle
{
    namespace app
    {
        // ============================================================================
        // Drivers
        // ============================================================================
        using AMT10xTimControllerT = platform::TimController<kCrankEncoderTimControllerId>;
        using AMT10xTimEncoderModuleT =
            platform::app::AMT10xTimEncoderModule<AMT10xTimControllerT, platform::GpioPinB5, AMT10xPPR::k2048>;
        using AMT10xTimEncoderModuleConfigT = typename AMT10xTimEncoderModuleT::ConfigT;

        using CrankEncoderModuleT       = platform::app::AMT10xCrankEncoderModuleX<AMT10xTimEncoderModuleT>;
        using CrankEncoderModuleConfigT = typename CrankEncoderModuleT::ConfigT;

        // Declare Main Driver List
        using MainDriversT = TypeList<platform::CoreSystemDriver, UartLoggerT, CrankEncoderModuleT>;

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
            CrankEncoderModuleT        crank_encoder;
        };

    }  // namespace app

    struct AppSystemBuildConfig : SystemBuildConfigBase<app::Drivers>
    {
    };

}  // namespace valle
