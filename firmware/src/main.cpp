#include "valle/app_bridge.hpp"
#include "valle/system/init.hpp"

extern "C" int main()
{
    valle::system::init();
    valle::app::run();

    // Should never reach here
    while (true)
    {
    }

    return 0;
}