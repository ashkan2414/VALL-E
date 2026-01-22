#pragma once

// ============================================================================
// STANDARD LIBRARY (Heavy Templates)
// ============================================================================
// These are the most "expensive" headers to parse repeatedly.

#ifdef __cplusplus
#include <algorithm>
#include <array>
#include <atomic>
#include <bit>  // C++20 bit manipulation
#include <cmath>
#include <concepts>  // C++20 concepts
#include <cstdint>
#include <functional>  // std::reference_wrapper
#include <limits>
#include <memory>
#include <tuple>        // Essential for your Registry
#include <type_traits>  // Essential for your Metaprogramming
#include <utility>      // std::move, std::forward
#include <variant>
#endif

// ============================================================================
// STM32 HAL & LL DRIVERS
// ============================================================================
// Including these here prevents the compiler from re-parsing thousands
// of #define macros for every single .cpp file.

#include "stm32g4xx_hal.h"

// Low Layer (LL) Drivers
#include "stm32g4xx_ll_adc.h"
#include "stm32g4xx_ll_bus.h"
#include "stm32g4xx_ll_comp.h"
#include "stm32g4xx_ll_cordic.h"
#include "stm32g4xx_ll_cortex.h"
#include "stm32g4xx_ll_crc.h"
#include "stm32g4xx_ll_crs.h"
#include "stm32g4xx_ll_dac.h"
#include "stm32g4xx_ll_dma.h"
#include "stm32g4xx_ll_dmamux.h"
#include "stm32g4xx_ll_exti.h"
#include "stm32g4xx_ll_fmac.h"
#include "stm32g4xx_ll_fmc.h"
#include "stm32g4xx_ll_gpio.h"
#include "stm32g4xx_ll_hrtim.h"
#include "stm32g4xx_ll_i2c.h"
#include "stm32g4xx_ll_iwdg.h"
#include "stm32g4xx_ll_lptim.h"
#include "stm32g4xx_ll_lpuart.h"
#include "stm32g4xx_ll_opamp.h"
#include "stm32g4xx_ll_pwr.h"
#include "stm32g4xx_ll_rcc.h"
#include "stm32g4xx_ll_rng.h"
#include "stm32g4xx_ll_rtc.h"
#include "stm32g4xx_ll_spi.h"
#include "stm32g4xx_ll_system.h"
#include "stm32g4xx_ll_tim.h"
#include "stm32g4xx_ll_ucpd.h"
#include "stm32g4xx_ll_usart.h"
#include "stm32g4xx_ll_usb.h"
#include "stm32g4xx_ll_utils.h"
#include "stm32g4xx_ll_wwdg.h"

// ============================================================================
// Thirdparty LIBRARIES
// ============================================================================
#ifdef __cplusplus

#include <Eigen/Dense>

#include "delegate/delegate.h"

#endif

// ============================================================================
// PROJECT CORE (Optional)
// ============================================================================
// Only include your headers here if they are STABLE (not changing often).

#ifdef __cplusplus
#include "Valle/Device/device_core.hpp"
#include "Valle/Device/registry.hpp"
#endif