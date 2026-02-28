#include "valle/platform/core/clock.hpp"

#include "stm32g4xx_hal.h"
#include "stm32g4xx_hal_rcc.h"
#include "valle/core/error.hpp"

namespace valle
{

    /**
     * @brief System Clock Configuration
     * @retval None
     */
    void config_system_clock(void)
    {
        // Configure the main internal regulator output voltage
        HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1_BOOST);

        // Initializes the RCC Oscillators according to the specified parameters in the RCC_OscInitTypeDef structure.
        RCC_OscInitTypeDef rcc_osc_config{};
        rcc_osc_config.OscillatorType      = RCC_OSCILLATORTYPE_HSI;
        rcc_osc_config.HSIState            = RCC_HSI_ON;
        rcc_osc_config.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
        rcc_osc_config.PLL.PLLState        = RCC_PLL_ON;
        rcc_osc_config.PLL.PLLSource       = RCC_PLLSOURCE_HSI;
        rcc_osc_config.PLL.PLLM            = RCC_PLLM_DIV4;
        rcc_osc_config.PLL.PLLN            = 85;  // 4 MHz / 4 * 85 = 85 MHz // NOLINT(readability-magic-numbers)
        rcc_osc_config.PLL.PLLP            = RCC_PLLP_DIV2;
        rcc_osc_config.PLL.PLLQ            = RCC_PLLQ_DIV2;
        rcc_osc_config.PLL.PLLR            = RCC_PLLR_DIV2;
        valle::expect(HAL_RCC_OscConfig(&rcc_osc_config) == HAL_OK, "RCC Oscillator configuration failed");

        // Initializes the CPU, AHB and APB buses clocks
        RCC_ClkInitTypeDef rcc_clk_config{};
        rcc_clk_config.ClockType =
            RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
        rcc_clk_config.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
        rcc_clk_config.AHBCLKDivider  = RCC_SYSCLK_DIV1;
        rcc_clk_config.APB1CLKDivider = RCC_HCLK_DIV1;
        rcc_clk_config.APB2CLKDivider = RCC_HCLK_DIV1;
        valle::expect(HAL_RCC_ClockConfig(&rcc_clk_config, FLASH_LATENCY_4) == HAL_OK,
                      "RCC Clock configuration failed");
    }
}  // namespace valle