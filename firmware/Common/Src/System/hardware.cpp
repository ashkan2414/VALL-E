#include "Valle/System/hardware.hpp"

namespace valle
{
    namespace system::internal
    {
        // Make device storage and install drivers
        Hardware g_system_hardware  = Hardware(build_device_storage_from_driver_list<DriversT>());
        void*    gp_system_hardware = &g_system_hardware;
    }  // namespace system::internal

    namespace app
    {
        namespace dont_use_me
        {
            static auto g_temp_installed_drivers = install_drivers(system::internal::boot_driver_builder());
        }

        DeviceRefRegistryT g_ref_registry = std::move(dont_use_me::g_temp_installed_drivers.first);
        DriverContainerT   g_drivers =
            std::make_from_tuple<DriverContainerT>(std::move(dont_use_me::g_temp_installed_drivers.second));

    }  // namespace app
}  // namespace valle
