#pragma once

#include "Valle/Board/Device/device.hpp"
#include "Valle/Board/Drivers/uart_logger.hpp"
#include "Valle/Modules/acs724.hpp"
#include "Valle/Modules/ldc1612.hpp"
#include "Valle/Modules/vca.hpp"
#include "Valle/System/config.hpp"

namespace valle::app
{
    // Logger
    constexpr UARTControllerID kLoggerUARTControllerID = UARTControllerID::kLPUART1;
    struct UARTControllerCTConfig : UARTControllerCTConfigDefaults<kLoggerUARTControllerID>
    {
        using DMAChannelTxT = DMA1Channel1Device;
    };

    // HRTIM Controller
    constexpr HRTIMControllerID kHRTIMControllerID = 1;
    struct HRTIMControllerCTConfig : HRTIMControllerCTConfigDefaults
    {
    };

    // HRTIM Timer
    constexpr HRTIMTimerID kHRTIMTimerID = HRTIMTimerID::kA;
    struct HRTIMTimerCTConfig : HRTIMTimerCTConfigDefaults
    {
        using Output1PinT = HRTIMTimerDefaultOutput1PinDevice<kHRTIMControllerID, kHRTIMTimerID>;
        using Output2PinT = HRTIMTimerDefaultOutput2PinDevice<kHRTIMControllerID, kHRTIMTimerID>;
    };

}  // namespace valle::app

// Bind compile-time configurations
VALLE_DEFINE_UART_CONTROLLER_CT_CONFIG(app::kLoggerUARTControllerID, app::UARTControllerCTConfig{});
VALLE_DEFINE_HRTIM_CONTROLLER_CT_CONFIG(app::kHRTIMControllerID, app::HRTIMControllerCTConfig{});
VALLE_DEFINE_HRTIM_TIMER_CT_CONFIG(app::kHRTIMControllerID, app::kHRTIMTimerID, app::HRTIMTimerCTConfig{});

namespace valle::app
{

    // ============================================================================
    // Driver Configurations
    // ============================================================================
    using UARTLoggerT = UARTLogger<UARTControllerDevice<kLoggerUARTControllerID>>;

    using HRTIMTimerDeviceT      = HRTIMTimerDevice<kHRTIMControllerID, kHRTIMTimerID>;
    using HRTIMHalfBridgeDriverT = HRTIMHalfBridgeDriver<HRTIMTimerDeviceT>;

    struct Drivers
    {
        using DriversT = TypeList<UARTLoggerT, HRTIMHalfBridgeDriverT>;

        UARTLoggerT            uart_logger;
        HRTIMHalfBridgeDriverT hb_driver;
    };

    struct Devices
    {
        using DevicesT =
            TypeList<GPIOPortADevice, HRTIM1ControllerDevice, DMAMux1ControllerDevice, DMA1ControllerDevice>;

        [[no_unique_address]] DeviceRef<GPIOPortADevice>         gpioa;
        [[no_unique_address]] DeviceRef<HRTIM1ControllerDevice>  hrtim1;
        [[no_unique_address]] DeviceRef<DMAMux1ControllerDevice> dmamux1;
        [[no_unique_address]] DeviceRef<DMA1ControllerDevice>    dma1;
    };

}  // namespace valle::app

namespace valle::system
{
    struct Config : ConfigBase<app::Drivers, app::Devices>
    {
    };

}  // namespace valle::system