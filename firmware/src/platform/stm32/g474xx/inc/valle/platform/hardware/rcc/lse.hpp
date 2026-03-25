#pragma once

#include "stm32g4xx_ll_rcc.h"

namespace valle::platform
{
    // =============================================================================
    // ENUMERATIONS
    // =============================================================================
    enum class LseOscillatorMode
    {
        kCrystal,
        kBypass
    };

    enum class LseOscillatorDriveCapability : uint32_t
    {
        kLow        = LL_RCC_LSEDRIVE_LOW,
        kMediumLow  = LL_RCC_LSEDRIVE_MEDIUMLOW,
        kMediumHigh = LL_RCC_LSEDRIVE_MEDIUMHIGH,
        kHigh       = LL_RCC_LSEDRIVE_HIGH
    };

    // =============================================================================
    // INTERFACE
    // =============================================================================
    struct LseOscillatorInterface
    {
        static constexpr uint32_t skFrequencyHz                = Lse_VALUE;
        static constexpr uint32_t skDefaultEnableTimeoutCount  = 500'000'000U;
        static constexpr uint32_t skDefaultDisableTimeoutCount = 500'000'000U;

        static void enable(const LseOscillatorMode mode = LseOscillatorMode::kCrystal)
        {
            switch (mode)
            {
                case LseOscillatorMode::kCrystal:
                    LL_RCC_LSE_DisableBypass();
                    LL_RCC_LSE_Enable();
                    break;

                case LseOscillatorMode::kBypass:
                    LL_RCC_LSE_EnableBypass();
                    LL_RCC_LSE_Enable();
                    break;
            }
        }

        static void disable()
        {
            LL_RCC_LSE_Disable();
            LL_RCC_LSE_DisableBypass();
        }

        [[nodiscard]] static bool is_ready()
        {
            return LL_RCC_LSE_IsReady() == 1;
        }

        static void set_drive_capability(const LseOscillatorDriveCapability drive)
        {
            LL_RCC_LSE_SetDriveCapability(static_cast<uint32_t>(drive));
        }

        [[nodiscard]] static LseOscillatorDriveCapability get_drive_capability()
        {
            return static_cast<LseOscillatorDriveCapability>(LL_RCC_LSE_GetDriveCapability());
        }

        static void enable_css()
        {
            LL_RCC_LSE_EnableCSS();
        }

        static void disable_css()
        {
            LL_RCC_LSE_DisableCSS();
        }

        [[nodiscard]] static bool is_css_detected()
        {
            return LL_RCC_LSE_IsCSSDetected() == 1;
        }
    };

}  // namespace valle::platform