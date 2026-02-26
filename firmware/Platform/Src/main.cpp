#include "Valle/Core/app_bridge.hpp"
#include "Valle/core.hpp"

extern "C" int main()
{
    valle::core_init();
    valle::app::run();

    // Should never reach here
    while (1)
    {
    }

    return 0;
}