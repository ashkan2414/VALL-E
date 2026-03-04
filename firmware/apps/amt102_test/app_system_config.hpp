#pragma once

#include "valle/core/device/device.hpp"
#include "valle/core/system/config.hpp"
#include "valle/platform/drivers/uart/logger.hpp"
#include "valle/platform/modules/amt10x.hpp"

namespace valle::app
{
    // UART for Logging
    constexpr UARTControllerID kLoggerUARTID = UARTControllerID::kLPUART1;
    struct UARTControllerCTConfig : UARTControllerCTConfigDefaults<kLoggerUARTID>
    {
        using DMAChannelTxT = DMA1Channel2Device;
    };
    static constexpr UARTControllerCTConfig kLoggerUARTCTConfig{};

    // TIM for Quadrature Encoder
    constexpr TIMControllerID kEncoderTIMControllerID = TIMControllerID::kTim2;
    struct TIMControllerCTConfig : TIMControllerCTConfigDefaults
    {
        using Ch1PinT = GPIOPinA0Device;
        using Ch2PinT = GPIOPinA1Device;
    };

}  // namespace valle::app

VALLE_DEFINE_UART_CONTROLLER_CT_CONFIG(app::kLoggerUARTID, app::kLoggerUARTCTConfig);
VALLE_DEFINE_TIMER_CONTROLLER_CT_CONFIG(app::kEncoderTIMControllerID, app::TIMControllerCTConfig{});

namespace valle::app
{

    // ============================================================================
    // Device Configurations
    // ============================================================================
    using LoggerUARTControllerT = UARTControllerDevice<kLoggerUARTID>;
    using UARTLoggerT           = UARTLogger<LoggerUARTControllerT>;

    using EncoderTIMControllerT            = TIMControllerDevice<kEncoderTIMControllerID>;
    static constexpr AMT10xPPR kEncoderPPR = AMT10xPPR::k4096;
    using AMT102ModuleT                    = AMT10xTIMEncoderModule<EncoderTIMControllerT, kEncoderPPR>;
    using AMT102ModuleConfigT              = typename AMT102ModuleT::ConfigT;

    struct Drivers
    {
        using DriversT = TypeList<UARTLoggerT, AMT102ModuleT>;

        UARTLoggerT   uart_logger;
        AMT102ModuleT amt102;
    };

    struct Devices
    {
        using DevicesT = TypeList<DMAMux1ControllerDevice, DMA1ControllerDevice, GPIOPortADevice>;

        [[no_unique_address]] DeviceRef<DMAMux1ControllerDevice> dmamux1;
        [[no_unique_address]] DeviceRef<DMA1ControllerDevice>    dma1;
        [[no_unique_address]] DeviceRef<GPIOPortADevice>         gpioa;
    };

}  // namespace valle::app

namespace valle::system
{
    struct Config : ConfigBase<app::Drivers, app::Devices>
    {
    };

}  // namespace valle::system