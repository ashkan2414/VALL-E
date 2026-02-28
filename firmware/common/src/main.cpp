#include "valle/core.hpp"
#include "valle/core/app_bridge.hpp"
#include "valle/platform/core.hpp"

extern "C" int main()
{
    valle::platform_init();
    valle::core_init();
    valle::app::run();

    // Should never reach here
    while (1)
    {
    }

    return 0;
}