#include "Valle/Policy/hrtim.hpp"

#ifdef HAL_HRTIM_MODULE_ENABLED

/**
 * @brief HRTIM Interrupt Handler Router
 *
 * @tparam tkIndex
 */
template <uint8_t tkIndex>
static inline void hrtim_irq_handler()
    requires(kValidHRTIMIndex<tkIndex>)
{
    HRTIMIsrRouter<tkIndex>::handle();
    HRTIMPolicy<tkIndex>::clear_interrupt_flag();
}

extern "C"
{
    // --- TIMER A (Index 0) ---
    void HRTIM1_TIMA_IRQHandler(void)  // NOLINT(readability-identifier-naming)
    {
        hrtim_irq_handler<0>();
    }

    // --- TIMER B (Index 1) ---
    void HRTIM1_TIMB_IRQHandler(void)  // NOLINT(readability-identifier-naming)
    {
        hrtim_irq_handler<1>();
    }

    // --- TIMER C (Index 2) ---
    void HRTIM1_TIMC_IRQHandler(void)  // NOLINT(readability-identifier-naming)
    {
        hrtim_irq_handler<2>();
    }

    // --- TIMER D (Index 3) ---
    void HRTIM1_TIMD_IRQHandler(void)  // NOLINT(readability-identifier-naming)
    {
        hrtim_irq_handler<3>();
    }

    // --- TIMER E (Index 4) ---
    void HRTIM1_TIME_IRQHandler(void)  // NOLINT(readability-identifier-naming)
    {
        hrtim_irq_handler<4>();
    }

    // --- TIMER F (Index 5) ---
    void HRTIM1_TIMF_IRQHandler(void)  // NOLINT(readability-identifier-naming)
    {
        hrtim_irq_handler<5>();
    }
}

#endif  // HAL_HRTIM_MODULE_ENABLED