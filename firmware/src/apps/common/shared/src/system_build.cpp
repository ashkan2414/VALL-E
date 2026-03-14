#include "valle/base/system_build/build.hpp"

namespace valle
{
    namespace system_build::internal
    {
        // Make device storage and install drivers
        SystemBuild g_system_build  = SystemBuild(build_device_storage_from_driver_list<DriversT>());
        void*       gp_system_build = &g_system_build;
    }  // namespace system_build::internal

    namespace app
    {
        namespace dont_use_me
        {
            static auto g_temp_installed_drivers = install_drivers(system_build::internal::boot_driver_builder());
        }

        DeviceRefRegistryT g_ref_registry = std::move(dont_use_me::g_temp_installed_drivers.first);

        DriverContainerT g_drivers =
            std::make_from_tuple<DriverContainerT>(std::move(dont_use_me::g_temp_installed_drivers.second));

    }  // namespace app
}  // namespace valle
