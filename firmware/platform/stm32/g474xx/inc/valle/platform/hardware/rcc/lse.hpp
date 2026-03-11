#pragma once

#include "stm32g4xx_ll_rcc.h"

namespace valle
{
    // =============================================================================
    // ENUMERATIONS
    // =============================================================================
    enum class LSEOscillatorMode
    {
        kCrystal,
        kBypass
    };

    enum class LSEOscillatorDriveCapability : uint32_t
    {
        kLow        = LL_RCC_LSEDRIVE_LOW,
        kMediumLow  = LL_RCC_LSEDRIVE_MEDIUMLOW,
        kMediumHigh = LL_RCC_LSEDRIVE_MEDIUMHIGH,
        kHigh       = LL_RCC_LSEDRIVE_HIGH
    };

    // =============================================================================
    // INTERFACE
    // =============================================================================
    struct LSEOscillatorInterface
    {
        static constexpr uint32_t skFrequencyHz                = LSE_VALUE;
        static constexpr uint32_t skDefaultEnableTimeoutCount  = 500'000'000U;
        static constexpr uint32_t skDefaultDisableTimeoutCount = 500'000'000U;

        static void enable(const LSEOscillatorMode mode = LSEOscillatorMode::kCrystal)
        {
            switch (mode)
            {
                case LSEOscillatorMode::kCrystal:
                    LL_RCC_LSE_DisableBypass();
                    LL_RCC_LSE_Enable();
                    break;

                case LSEOscillatorMode::kBypass:
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

        static void set_drive_capability(const LSEOscillatorDriveCapability drive)
        {
            LL_RCC_LSE_SetDriveCapability(static_cast<uint32_t>(drive));
        }

        [[nodiscard]] static LSEOscillatorDriveCapability get_drive_capability()
        {
            return static_cast<LSEOscillatorDriveCapability>(LL_RCC_LSE_GetDriveCapability());
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

}  // namespace valle