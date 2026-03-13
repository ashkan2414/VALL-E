#pragma once

#include "valle/base/system_build/config_base.hpp"
#include "valle/platform/drivers/core_system.hpp"
#include "valle/platform/drivers/uart/logger.hpp"
#include "valle/platform/modules/amt10x.hpp"

namespace valle::app
{
    // UART for Logging
    constexpr UARTControllerID kLoggerUARTID = UARTControllerID::kLPUART1;
    struct UARTControllerCTConfig : UARTControllerCTDefaultConfig<kLoggerUARTID>
    {
        using DMAChannelTxT = DMA1Channel2Device;
    };
    static constexpr UARTControllerCTConfig kLoggerUARTCTConfig{};

    // TIM for Quadrature Encoder
    constexpr TIMControllerID kEncoderTIMControllerID = TIMControllerID::kTim2;
    struct TIMControllerCTConfig : TIMControllerCTDefaultConfig
    {
        using Ch1PinT = GPIOPinA0Device;
        using Ch2PinT = GPIOPinA1Device;
    };

}  // namespace valle::app

VALLE_DEFINE_UART_CONTROLLER_CT_CONFIG(app::kLoggerUARTID, app::kLoggerUARTCTConfig);
VALLE_DEFINE_TIMER_CONTROLLER_CT_CONFIG(app::kEncoderTIMControllerID, app::TIMControllerCTConfig{});

namespace valle
{
    namespace app
    {
        // ============================================================================
        // Drivers
        // ============================================================================
        using LoggerUARTControllerT = UARTControllerDevice<kLoggerUARTID>;
        using UARTLoggerT           = UARTLogger<LoggerUARTControllerT>;

        using EncoderTIMControllerT            = TIMControllerDevice<kEncoderTIMControllerID>;
        static constexpr AMT10xPPR kEncoderPPR = AMT10xPPR::k4096;
        using AMT102ModuleT                    = AMT10xTIMEncoderModule<EncoderTIMControllerT, kEncoderPPR>;
        using AMT102ModuleConfigT              = typename AMT102ModuleT::ConfigT;

        // Declare Main Driver List
        using MainDriversT = TypeList<CoreSystemDriver, UARTLoggerT, AMT102ModuleT>;

        // ============================================================================
        // Root Driver
        // ============================================================================
        using RootDevicesT = RootDevicesFromDriverList<MainDriversT>;
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

            RootDriver       root;
            CoreSystemDriver core;
            UARTLoggerT      uart_logger;
            AMT102ModuleT    amt102;
        };

    }  // namespace app

    struct AppSystemBuildConfig : SystemBuildConfigBase<app::Drivers>
    {
    };

}  // namespace valle
