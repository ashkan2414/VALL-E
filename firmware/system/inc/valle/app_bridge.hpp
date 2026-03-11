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
            while (true)
            {
            }
        }
    }  // namespace app

}  // namespace valle