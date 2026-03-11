#pragma once

#include "valle/core/device/device.hpp"
#include "valle/core/system/config.hpp"
#include "valle/platform/drivers/core_system.hpp"
#include "valle/platform/drivers/uart/logger.hpp"
#include "valle/platform/modules/acs724.hpp"
#include "valle/platform/modules/ldc1612.hpp"
#include "valle/platform/modules/vca.hpp"

namespace valle::app
{
    constexpr UARTControllerID kLoggerUARTID = UARTControllerID::kLPUART1;
    struct UARTControllerCTConfig : UARTControllerCTDefaultConfig<kLoggerUARTID>
    {
        using DMAChannelTxT = DMA1Channel1Device;
    };

    // HRTIM PWM Config
    constexpr HRTIMControllerID kVCAPWMHRTIMControllerID = 1;
    constexpr HRTIMTimerID      kVCAPWMHRTIMTimerID      = HRTIMTimerID::kA;

    struct HRTIMControllerCTConfig : HRTIMControllerCTDefaultConfig
    {
    };

    struct HRTIMTimerCTConfig : HRTIMTimerCTDefaultConfig
    {
        using Output1PinT = HRTIMTimerDefaultOutput1PinDevice<kVCAPWMHRTIMControllerID, kVCAPWMHRTIMTimerID>;
        using Output2PinT = HRTIMTimerDefaultOutput2PinDevice<kVCAPWMHRTIMControllerID, kVCAPWMHRTIMTimerID>;
    };

    // Current Sensor ADC Config
    constexpr ADCControllerID kCurrentSensorADCID        = 3;
    constexpr ADCChannelID    kCurrentSensorADCChannelId = ADCChannelID::kChannel12;
    struct ADCControllerCTConfig : ADCControllerCTDefaultConfig
    {
    };

    // Position Sensor I2C Controller Config
    constexpr I2CControllerID kPositionSensorI2CID             = 1;
    constexpr uint16_t        kPositionSensorI2CAddress        = 0x2A;
    constexpr bool            kPositionSensorI2CAddressIs10Bit = false;

    struct I2CControllerCTConfig : I2CControllerCTDefaultConfig
    {
        using DMAChannelRxT = DMA2Channel1Device;
        using DMAChannelTxT = DMA2Channel2Device;
    };

}  // namespace valle::app

VALLE_DEFINE_UART_CONTROLLER_CT_CONFIG(app::kLoggerUARTID, app::UARTControllerCTConfig{});
VALLE_DEFINE_HRTIM_CONTROLLER_CT_CONFIG(app::kVCAPWMHRTIMControllerID, app::HRTIMControllerCTConfig{});
VALLE_DEFINE_HRTIM_TIMER_CT_CONFIG(app::kVCAPWMHRTIMControllerID, app::kVCAPWMHRTIMTimerID, app::HRTIMTimerCTConfig{});
VALLE_DEFINE_ADC_CONTROLLER_CT_CONFIG(app::kCurrentSensorADCID, app::ADCControllerCTConfig{});
VALLE_DEFINE_I2C_CONTROLLER_CT_CONFIG(app::kPositionSensorI2CID, app::I2CControllerCTConfig{});

namespace valle
{
    namespace app
    {
        // ============================================================================
        // Drivers
        // ============================================================================
        using LoggerUARTControllerT = UARTControllerDevice<app::kLoggerUARTID>;
        using UARTLoggerT           = UARTLogger<LoggerUARTControllerT>;

        using VCAPWMHRTIMTimerDeviceT = HRTIMTimerDevice<kVCAPWMHRTIMControllerID, kVCAPWMHRTIMTimerID>;

        using VCAControllerSystemControllerT       = VCAClosedLoopCurrentFeedbackController<float>;
        using VCAControllerSystemControllerConfigT = typename VCAControllerSystemControllerT::ConfigT;

        using VCAControllerT       = VCAControllerHRTIMModule<VCAPWMHRTIMTimerDeviceT, VCAControllerSystemControllerT>;
        using VCAControllerConfigT = typename VCAControllerT::ConfigT;

        using CurrentSensorADCChannelT = ADCInjectChannelRank1Device<kCurrentSensorADCID, kCurrentSensorADCChannelId>;
        using CurrentSensorADCControllerT = CurrentSensorADCChannelT::ChannelT::ControllerT;
        using CurrentSensorT              = ACS724Module<CurrentSensorADCChannelT, ACS724Model::k2P5ABi>;

        using PositionSensorI2CControllerT  = I2CCommandBufferDevice<kPositionSensorI2CID>;
        using PositionSensorI2CSlaveDeviceT = I2CCommandBufferSlaveDevice<kPositionSensorI2CID,
                                                                          kPositionSensorI2CAddress,
                                                                          kPositionSensorI2CAddressIs10Bit>;
        using PositionSensorT               = LDC161XSensorModule<PositionSensorI2CSlaveDeviceT, 1>;

        // Declare Main Driver List
        using MainDriversT = TypeList<CoreSystemDriver, UARTLoggerT, VCAControllerT, CurrentSensorT, PositionSensorT>;

        // ============================================================================
        // Root Driver
        // ============================================================================
        using RootDevicesT = RootDevicesFromDrivers<MainDriversT>;
        struct RootDriver : PackedDriverBase<RootDevicesT>
        {
            using BaseT = PackedDriverBase<RootDevicesT>;
            using BaseT::BaseT;

            VALLE_DEFINE_PACKED_DEVICE_DRIVER_ACCESSOR(adc1, ADC1ControllerDevice);
            VALLE_DEFINE_PACKED_DEVICE_DRIVER_ACCESSOR(i2c1, I2C1CommandBufferDevice<>);
        };

        // ============================================================================
        // Drivers Container
        // ============================================================================

        struct Drivers
        {
            using DriversT = typename TypeListAddFront<RootDriver, MainDriversT>::type;

            RootDriver       root;
            CoreSystemDriver core;
            UARTLoggerT     uart_logger;
            VCAControllerT  vca_controller;
            CurrentSensorT  current_sensor;
            PositionSensorT position_sensor;
        };

    }  // namespace app

    struct AppSystemConfig : SystemConfigBase<app::Drivers>
    {
    };

}  // namespace valle
