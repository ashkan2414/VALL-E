#pragma once

namespace valle
{
    namespace app
    {
        void main();  // Application main entry point

        inline void run()
        {
            app::main();

            /* Trap: We should never get here. If the App exits, we hang. */
            while (1)
            {
            }
        }
    }  // namespace app

}  // namespace valle