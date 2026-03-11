#pragma once

#include "valle/core/device/device.hpp"
#include "valle/core/system/config.hpp"
#include "valle/platform/drivers/core_system.hpp"
#include "valle/platform/drivers/hrtim/half_bridge.hpp"
#include "valle/platform/drivers/uart/logger.hpp"

namespace valle::app
{
    // Logger
    constexpr UARTControllerID kLoggerUARTControllerID = UARTControllerID::kLPUART1;
    struct UARTControllerCTConfig : UARTControllerCTDefaultConfig<kLoggerUARTControllerID>
    {
        using DMAChannelTxT = DMA1Channel1Device;
    };

    // HRTIM Controller
    constexpr HRTIMControllerID kHRTIMControllerID = 1;
    struct HRTIMControllerCTConfig : HRTIMControllerCTDefaultConfig
    {
    };

    // HRTIM Timer
    constexpr HRTIMTimerID kHRTIMTimerID = HRTIMTimerID::kA;
    struct HRTIMTimerCTConfig : HRTIMTimerCTDefaultConfig
    {
        using Output1PinT = GPIOPinA8Device;
        using Output2PinT = GPIOPinA9Device;
    };

}  // namespace valle::app

// Bind compile-time configurations
VALLE_DEFINE_UART_CONTROLLER_CT_CONFIG(app::kLoggerUARTControllerID, app::UARTControllerCTConfig{});
VALLE_DEFINE_HRTIM_CONTROLLER_CT_CONFIG(app::kHRTIMControllerID, app::HRTIMControllerCTConfig{});
VALLE_DEFINE_HRTIM_TIMER_CT_CONFIG(app::kHRTIMControllerID, app::kHRTIMTimerID, app::HRTIMTimerCTConfig{});

namespace valle
{
    namespace app
    {
        // ============================================================================
        // Drivers
        // ============================================================================
        using UARTLoggerT = UARTLogger<UARTControllerDevice<kLoggerUARTControllerID>>;

        using HRTIMTimerDeviceT      = HRTIMTimerDevice<kHRTIMControllerID, kHRTIMTimerID>;
        using HRTIMHalfBridgeDriverT = HRTIMHalfBridgeDriver<HRTIMTimerDeviceT>;

        // Declare Main Driver List
        using MainDriversT = TypeList<CoreSystemDriver, UARTLoggerT, HRTIMHalfBridgeDriverT>;

        // ============================================================================
        // Root Driver
        // ============================================================================
        using RootDevicesT = RootDevicesFromDrivers<MainDriversT>;
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

            RootDriver             root;
            CoreSystemDriver       core;
            UARTLoggerT            uart_logger;
            HRTIMHalfBridgeDriverT hb_driver;
        };

    }  // namespace app

    struct AppSystemConfig : SystemConfigBase<app::Drivers>
    {
    };

}  // namespace valle
