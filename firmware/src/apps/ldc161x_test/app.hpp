#include "valle/base/system_build/build.hpp"
#include "valle/system/sync.hpp"

namespace valle::app
{
    using PositionSensorAsyncReadResultT = PositionSensorT::ReadCallbackResultT;
    extern system::SynchronizedCriticalSection<std::optional<PositionSensorAsyncReadResultT>>
        g_position_sensor_read_result;

    void init();
    void intb_asserted_isr();
    void position_sensor_async_read_callback();
}  // namespace valle::app
