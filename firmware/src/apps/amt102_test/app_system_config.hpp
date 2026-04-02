#pragma once

#include "valle/app/platform/modules/amt10x.hpp"
#include "valle/app/platform/uart_logger_config.hpp"
#include "valle/base/system_build/config_base.hpp"
#include "valle/base/system_build/traits.hpp"
#include "valle/platform/drivers/core_system.hpp"
#include "valle/platform/drivers/uart/logger.hpp"

namespace valle::app
{
    // TIM for Quadrature Encoder
    constexpr auto kEncoderTimControllerId = platform::TimControllerId::kTim2;
    struct TimControllerCTConfig : public platform::TimControllerCTDefaultConfig
    {
        using Ch1PinT = platform::GpioPinA15;
        using Ch2PinT = platform::GpioPinB3;
    };

}  // namespace valle::app

VALLE_DEFINE_TIMER_CONTROLLER_CT_CONFIG(valle::app::kEncoderTimControllerId, valle::app::TimControllerCTConfig{});

namespace valle
{
    namespace app
    {
        // ============================================================================
        // Drivers
        // ============================================================================
        using EncoderTimControllerT            = platform::TimController<kEncoderTimControllerId>;
        static constexpr AMT10xPPR kEncoderPPR = AMT10xPPR::k2048;
        using AMT102ModuleT =
            platform::app::AMT10xTimEncoderModule<EncoderTimControllerT, platform::GpioPinB5, kEncoderPPR>;
        using AMT102ModuleConfigT = typename AMT102ModuleT::ConfigT;

        // Declare Main Driver List
        using MainDriversT = TypeList<platform::CoreSystemDriver, UartLoggerT, AMT102ModuleT>;

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
            AMT102ModuleT              amt102;
        };

    }  // namespace app

    struct AppSystemBuildConfig : SystemBuildConfigBase<app::Drivers>
    {
    };

}  // namespace valle
