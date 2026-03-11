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
    constexpr platform::HRTIMControllerID kHRTIMControllerID = 1;
    struct HRTIMControllerCTConfig : platform::HRTIMControllerCTDefaultConfig
    {
    };

    // HRTIM Timer
    constexpr auto kHRTIMTimerID = platform::HRTIMTimerID::kA;
    struct HRTIMTimerCTConfig : platform::HRTIMTimerCTDefaultConfig
    {
        using Output1PinT = platform::GPIOPinA8Device;
        using Output2PinT = platform::GPIOPinA9Device;
    };

}  // namespace valle::app

// Bind compile-time configurations
VALLE_DEFINE_HRTIM_CONTROLLER_CT_CONFIG(valle::app::kHRTIMControllerID, valle::app::HRTIMControllerCTConfig{});
VALLE_DEFINE_HRTIM_TIMER_CT_CONFIG(valle::app::kHRTIMControllerID,
                                   valle::app::kHRTIMTimerID,
                                   valle::app::HRTIMTimerCTConfig{});

namespace valle
{
    namespace app
    {
        // ============================================================================
        // Drivers
        // ============================================================================
        using HRTIMTimerDeviceT      = platform::HRTIMTimerDevice<kHRTIMControllerID, kHRTIMTimerID>;
        using HRTIMHalfBridgeDriverT = platform::HRTIMHalfBridgeDriver<HRTIMTimerDeviceT>;

        // Declare Main Driver List
        using MainDriversT = TypeList<platform::CoreSystemDriver, UARTLoggerT, HRTIMHalfBridgeDriverT>;

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
            HRTIMHalfBridgeDriverT     hb_driver;
        };

    }  // namespace app

    struct AppSystemBuildConfig : SystemBuildConfigBase<app::Drivers>
    {
    };

}  // namespace valle
