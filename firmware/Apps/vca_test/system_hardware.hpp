#pragma once

#include "Valle/Device/device.hpp"
#include "Valle/Modules/acs724.hpp"
#include "Valle/Modules/vca.hpp"
#include "Valle/system_build.hpp"

// ============================================================================
// Device Configurations
// ============================================================================

constexpr uint8_t      kCurrentSensorADC          = 1;
constexpr ADCChannelId kCurrentSensorADCChannelId = ADCChannelId::kChannel3;

template <>
struct ADCPeripheralCTConfigTraits<kCurrentSensorADC>
{
    static constexpr uint8_t skDMAIdx        = 1;
    static constexpr uint8_t skDMAChannelIdx = 1;
};

using CurrentSensorADCChannel = ADCInjectChannelDevice1<kCurrentSensorADC, kCurrentSensorADCChannelId>;
using CurrentSensorT          = ACS724Module<CurrentSensorADCChannel, ACS724Model::k2P5AB>;
using CurrentSensorADCP       = CurrentSensorADCChannel::ChannelT::PeripheralT;

using VCAControllerT = VCAControllerModule<HRTIMTimerDeviceA>;

// ============================================================================
// Global Device Storage and Registry
// ============================================================================
using SystemBuildTraits  = SystemBuildFromDriversTraits<CurrentSensorT, VCAControllerT>;
using DeviceStorageT     = SystemBuildTraits::DeviceStorageT;
using DeviceRefRegistryT = SystemBuildTraits::DeviceRefRegistryT;

namespace valle::system
{
    // ============================================================================
    // Global Instances
    // ============================================================================
    extern DeviceStorageT     g_storage;
    extern DeviceRefRegistryT g_device_ref_registry;
    extern CurrentSensorT     g_current_sensor;
    extern VCAControllerT     g_vca;

    void init();
    void start_vca_controller();

}  // namespace valle::system