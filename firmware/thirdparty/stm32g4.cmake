cmake_minimum_required(VERSION 3.22)

set(STM32CUBE_DIR ${VALLE_THIRDPARTY_DIR}/STM32CubeG4)

# Enable CMake support for ASM and C languages
enable_language(C CXX ASM)

# Validate that STM32CubeMX code is compatible with C standard
if((CMAKE_C_STANDARD EQUAL 90) OR (CMAKE_C_STANDARD EQUAL 99))
    message(ERROR "Generated code requires C11 or higher")
endif()


# Create stm32cube_Drivers static library
add_library(stm32g4_drivers OBJECT)

# STM32 HAL/LL Drivers sources (exclude templates)
file(GLOB stm32cube_drivers_src
    ${STM32CUBE_DIR}/Drivers/STM32G4xx_HAL_Driver/Src/*.c
    ${STM32CUBE_DIR}/Drivers/STM32G4xx_HAL_Driver/Src/Legacy/*.c
    ${STM32CUBE_DIR}/Drivers/CMSIS/Device/ST/STM32G4xx/Source/Templates/system_stm32g4xx.c
)
list(FILTER stm32cube_drivers_src EXCLUDE REGEX "template")
target_sources(stm32g4_drivers PRIVATE ${stm32cube_drivers_src})

# STM32 HAL/LL Drivers headers
target_include_directories(stm32g4_drivers SYSTEM PUBLIC
    ${STM32CUBE_DIR}/Drivers/STM32G4xx_HAL_Driver/Inc
    ${STM32CUBE_DIR}/Drivers/CMSIS/Device/ST/STM32G4xx/Include
    ${STM32CUBE_DIR}/Drivers/CMSIS/Core/Include
    ${VALLE_TARGET_PLATFORM_DIR}/inc
)

# Define platform-specific macros for the STM32G4 series
target_compile_definitions(stm32g4_drivers PUBLIC
	USE_HAL_DRIVER
    USE_FULL_LL_DRIVER
	STM32G474xx
    $<$<CONFIG:Debug>:DEBUG>
)

target_compile_options(stm32g4_drivers PRIVATE -w)


