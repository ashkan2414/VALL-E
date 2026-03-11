#pragma once

#include <cstdint>

#include "stm32g4xx_ll_system.h"
#include "valle/platform/hardware/power.hpp"

namespace valle
{
    // =============================================================================
    // ENUMERATIONS
    // =============================================================================
    /**
     * @brief FLASH wait-state latency configuration.
     *
     * These values control the number of wait states used when reading from flash.
     * Higher core / HCLK frequencies require higher latency settings.
     */
    enum class FlashLatency : uint32_t
    {
        k0  = LL_FLASH_LATENCY_0,
        k1  = LL_FLASH_LATENCY_1,
        k2  = LL_FLASH_LATENCY_2,
        k3  = LL_FLASH_LATENCY_3,
        k4  = LL_FLASH_LATENCY_4,
        k5  = LL_FLASH_LATENCY_5,
        k6  = LL_FLASH_LATENCY_6,
        k7  = LL_FLASH_LATENCY_7,
        k8  = LL_FLASH_LATENCY_8,
        k9  = LL_FLASH_LATENCY_9,
        k10 = LL_FLASH_LATENCY_10,
        k11 = LL_FLASH_LATENCY_11,
        k12 = LL_FLASH_LATENCY_12,
        k13 = LL_FLASH_LATENCY_13,
        k14 = LL_FLASH_LATENCY_14,
        k15 = LL_FLASH_LATENCY_15
    };

    // =============================================================================
    // INTERFACE
    // =============================================================================
    /**
     * @brief Interface for FLASH latency / wait-state control.
     */
    struct FlashInterface
    {
        static constexpr uint32_t skDefaultLatencySwitchTimeoutCount = 1'000U;

        /**
         * @brief Set FLASH latency.
         *
         * @param latency Desired FLASH wait-state setting.
         */
        static void set_latency(const FlashLatency latency)
        {
            LL_FLASH_SetLatency(static_cast<uint32_t>(latency));
        }

        /**
         * @brief Get current FLASH latency.
         *
         * @return Current FLASH wait-state setting.
         */
        [[nodiscard]] static FlashLatency get_latency()
        {
            return static_cast<FlashLatency>(LL_FLASH_GetLatency());
        }

        /**
         * @brief Check whether the requested FLASH latency is currently active.
         *
         * @param latency FLASH wait-state setting to test.
         * @return true if the requested latency is active, false otherwise.
         */
        [[nodiscard]] static bool is_latency_active(const FlashLatency latency)
        {
            return get_latency() == latency;
        }

        [[nodiscard]] static bool wait_for_latency(const FlashLatency latency, const uint32_t timeout_count)
        {
            return PlatformTimingUtils::wait_for_with_timeout_countdown(
                [&]() -> bool { return is_latency_active(latency); }, timeout_count);
        }

        /**
         * @brief Get the raw numeric wait-state count corresponding to the latency enum.
         *
         * @param latency FLASH latency enum value.
         * @return Number of wait states.
         */
        [[nodiscard]] static constexpr uint32_t latency_to_wait_states(const FlashLatency latency)
        {
            // NOLINTBEGIN(readability-magic-numbers)
            switch (latency)
            {
                case FlashLatency::k0:
                    return 0;
                case FlashLatency::k1:
                    return 1;
                case FlashLatency::k2:
                    return 2;
                case FlashLatency::k3:
                    return 3;
                case FlashLatency::k4:
                    return 4;
                case FlashLatency::k5:
                    return 5;
                case FlashLatency::k6:
                    return 6;
                case FlashLatency::k7:
                    return 7;
                case FlashLatency::k8:
                    return 8;
                case FlashLatency::k9:
                    return 9;
                case FlashLatency::k10:
                    return 10;
                case FlashLatency::k11:
                    return 11;
                case FlashLatency::k12:
                    return 12;
                case FlashLatency::k13:
                    return 13;
                case FlashLatency::k14:
                    return 14;
                case FlashLatency::k15:
                    return 15;
            }
            // NOLINTEND(readability-magic-numbers)

            return 0;
        }

        // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
        [[nodiscard]] static constexpr FlashLatency calculate_flash_latency(const uint32_t              hclk_hz,
                                                                            const PowerVoltageRangeMode range)
        {
            // NOLINTBEGIN(readability-magic-numbers,bugprone-branch-clone)
            switch (range)
            {
                case PowerVoltageRangeMode::kRange1Normal:

                    if (hclk_hz <= 30'000'000U)
                    {
                        return FlashLatency::k0;
                    }
                    else if (hclk_hz <= 60'000'000U)
                    {
                        return FlashLatency::k1;
                    }
                    else if (hclk_hz <= 90'000'000U)
                    {
                        return FlashLatency::k2;
                    }
                    else if (hclk_hz <= 120'000'000U)
                    {
                        return FlashLatency::k3;
                    }
                    else
                    {
                        return FlashLatency::k4;
                    }

                case PowerVoltageRangeMode::kRange1Boost:

                    if (hclk_hz <= 34'000'000U)
                    {
                        return FlashLatency::k0;
                    }
                    else if (hclk_hz <= 68'000'000U)
                    {
                        return FlashLatency::k1;
                    }
                    else if (hclk_hz <= 102'000'000U)
                    {
                        return FlashLatency::k2;
                    }
                    else if (hclk_hz <= 136'000'000U)
                    {
                        return FlashLatency::k3;
                    }
                    else
                    {
                        return FlashLatency::k4;
                    }

                case PowerVoltageRangeMode::kRange2:
                    if (hclk_hz <= 12'000'000U)
                    {
                        return FlashLatency::k0;
                    }
                    else
                    {
                        return FlashLatency::k1;
                    }
            }
            // NOLINTEND(readability-magic-numbers,bugprone-branch-clone)
        }
    };

}  // namespace valle