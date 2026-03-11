#include "valle/app_bridge.hpp"
#include "valle/platform/core.hpp"

extern "C" int main()
{
    valle::core_init();
    valle::platform_init();
    valle::app::run();

    // Should never reach here
    while (true)
    {
    }

    return 0;
}