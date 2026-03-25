#pragma once

#include "valle/app/platform/uart_logger_config.hpp"
#include "valle/base/system_build/config_base.hpp"
#include "valle/base/system_build/traits.hpp"
#include "valle/platform/drivers/core_system.hpp"
#include "valle/platform/drivers/hrtim/half_bridge.hpp"
#include "valle/platform/drivers/uart/logger.hpp"

namespace valle::app
{
    // HRTIM Controller
    constexpr platform::HrtimPeripheralId kHrtimPeripheralId = 1;
    struct HrtimControllerCTConfig : public platform::HrtimControllerCTDefaultConfig
    {
    };

    // HRTIM Timer
    constexpr auto kHrtimTimerId = platform::HrtimTimerId::kTimerA;
    struct HrtimTimerCTConfig : public platform::HrtimTimerCTDefaultConfig
    {
        using Output1PinT = platform::GpioPinA8Device;
        using Output2PinT = platform::GpioPinA9Device;
    };

}  // namespace valle::app

// Bind compile-time configurations
VALLE_DEFINE_HRTIM_CONTROLLER_CT_CONFIG(valle::app::kHrtimPeripheralId, valle::app::HrtimControllerCTConfig{});
VALLE_DEFINE_HRTIM_TIMER_CT_CONFIG(valle::app::kHrtimPeripheralId,
                                   valle::app::kHrtimTimerId,
                                   valle::app::HrtimTimerCTConfig{});

namespace valle
{
    namespace app
    {
        // ============================================================================
        // Drivers
        // ============================================================================
        using HrtimTimerDeviceT      = platform::HrtimTimerDevice<kHrtimPeripheralId, kHrtimTimerId>;
        using HrtimHalfBridgeDriverT = platform::HrtimHalfBridgeDriver<HrtimTimerDeviceT>;

        // Declare Main Driver List
        using MainDriversT = TypeList<platform::CoreSystemDriver, UartLoggerT, HrtimHalfBridgeDriverT>;

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
            HrtimHalfBridgeDriverT     hb_driver;
        };

    }  // namespace app

    struct AppSystemBuildConfig : SystemBuildConfigBase<app::Drivers>
    {
    };

}  // namespace valle
