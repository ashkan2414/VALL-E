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
    constexpr auto kEncoderTIMControllerID = platform::TIMControllerID::kTim2;
    struct TIMControllerCTConfig : platform::TIMControllerCTDefaultConfig
    {
        using Ch1PinT = platform::GPIOPinA0Device;
        using Ch2PinT = platform::GPIOPinA1Device;
    };

}  // namespace valle::app

VALLE_DEFINE_TIMER_CONTROLLER_CT_CONFIG(valle::app::kEncoderTIMControllerID, valle::app::TIMControllerCTConfig{});

namespace valle
{
    namespace app
    {
        // ============================================================================
        // Drivers
        // ============================================================================
        using EncoderTIMControllerT            = platform::TIMControllerDevice<kEncoderTIMControllerID>;
        static constexpr AMT10xPPR kEncoderPPR = AMT10xPPR::k4096;
        using AMT102ModuleT       = platform::app::AMT10xTIMEncoderModule<EncoderTIMControllerT, kEncoderPPR>;
        using AMT102ModuleConfigT = typename AMT102ModuleT::ConfigT;

        // Declare Main Driver List
        using MainDriversT = TypeList<platform::CoreSystemDriver, UARTLoggerT, AMT102ModuleT>;

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
            AMT102ModuleT              amt102;
        };

    }  // namespace app

    struct AppSystemBuildConfig : SystemBuildConfigBase<app::Drivers>
    {
    };

}  // namespace valle
