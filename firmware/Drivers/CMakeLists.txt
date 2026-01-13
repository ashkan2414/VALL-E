cmake_minimum_required(VERSION 3.22)
# Enable CMake support for ASM and C languages
enable_language(C ASM)
# STM32CubeMX generated symbols (macros)
set(STM32_Defines_Syms
	USE_HAL_DRIVER
	STM32G474xx
    $<$<CONFIG:Debug>:DEBUG>
)

set(FIRMWARE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/..)

# STM32CubeMX generated include paths
set(STM32_Include_Dirs
    ${FIRMWARE_DIRECTORY}/Drivers/STM32G4xx_HAL_Driver/Inc
    ${FIRMWARE_DIRECTORY}/Drivers/STM32G4xx_HAL_Driver/Inc/Legacy
    ${FIRMWARE_DIRECTORY}/Drivers/CMSIS/Device/ST/STM32G4xx/Include
    ${FIRMWARE_DIRECTORY}/Drivers/CMSIS/Include
    ${FIRMWARE_DIRECTORY}/Core/Inc
)

# STM32CubeMX generated application sources
set(STM32_Application_Src
    ${FIRMWARE_DIRECTORY}/startup_stm32g474xx.s
)

# STM32 HAL/LL Drivers
set(STM32_Drivers_Src
    ${FIRMWARE_DIRECTORY}/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_tim.c
    ${FIRMWARE_DIRECTORY}/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_tim_ex.c
    ${FIRMWARE_DIRECTORY}/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_adc.c
    ${FIRMWARE_DIRECTORY}/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_adc_ex.c
    ${FIRMWARE_DIRECTORY}/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_ll_adc.c
    ${FIRMWARE_DIRECTORY}/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal.c
    ${FIRMWARE_DIRECTORY}/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_rcc.c
    ${FIRMWARE_DIRECTORY}/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_rcc_ex.c
    ${FIRMWARE_DIRECTORY}/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_flash.c
    ${FIRMWARE_DIRECTORY}/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_flash_ex.c
    ${FIRMWARE_DIRECTORY}/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_flash_ramfunc.c
    ${FIRMWARE_DIRECTORY}/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_gpio.c
    ${FIRMWARE_DIRECTORY}/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_exti.c
    ${FIRMWARE_DIRECTORY}/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_dma.c
    ${FIRMWARE_DIRECTORY}/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_dma_ex.c
    ${FIRMWARE_DIRECTORY}/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_pwr.c
    ${FIRMWARE_DIRECTORY}/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_pwr_ex.c
    ${FIRMWARE_DIRECTORY}/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_cortex.c
    ${FIRMWARE_DIRECTORY}/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_cordic.c
    ${FIRMWARE_DIRECTORY}/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_fmac.c
    ${FIRMWARE_DIRECTORY}/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_i2c.c
    ${FIRMWARE_DIRECTORY}/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_i2c_ex.c
    ${FIRMWARE_DIRECTORY}/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_uart.c
    ${FIRMWARE_DIRECTORY}/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_uart_ex.c
)

# Drivers Midllewares



# Link directories setup
set(STM32_LINK_DIRS

)
# Project static libraries
set(STM32_LINK_LIBS
    STM32_Drivers
    ${TOOLCHAIN_LINK_LIBRARIES}

)
# Interface library for includes and symbols
add_library(stm32_drivers INTERFACE)
target_include_directories(stm32_drivers INTERFACE ${STM32_Include_Dirs})
target_compile_definitions(stm32_drivers INTERFACE ${STM32_Defines_Syms})

# Create STM32_Drivers static library
add_library(STM32_Drivers OBJECT)
target_sources(STM32_Drivers PRIVATE ${STM32_Drivers_Src})
target_link_libraries(STM32_Drivers PUBLIC stm32_drivers)


# Add STM32CubeMX generated application sources to the project
target_sources(${CMAKE_PROJECT_NAME} PRIVATE ${STM32_Application_Src})

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE ${STM32_LINK_DIRS})

# Add libraries to the project
target_link_libraries(${CMAKE_PROJECT_NAME} ${STM32_LINK_LIBS})

# Add the map file to the list of files to be removed with 'clean' target
set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES ADDITIONAL_CLEAN_FILES ${CMAKE_PROJECT_NAME}.map)

# Validate that STM32CubeMX code is compatible with C standard
if((CMAKE_C_STANDARD EQUAL 90) OR (CMAKE_C_STANDARD EQUAL 99))
    message(ERROR "Generated code requires C11 or higher")
endif()
