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
set(STM32CUBE_DIRECTORY ${FIRMWARE_DIRECTORY}/Thirdparty/STM32CubeG4)

# STM32CubeMX generated include paths
set(STM32_Include_Dirs
    ${STM32CUBE_DIRECTORY}/Drivers/STM32G4xx_HAL_Driver/Inc
    ${STM32CUBE_DIRECTORY}/Drivers/STM32G4xx_HAL_Driver/Inc/Legacy
    ${STM32CUBE_DIRECTORY}/Drivers/CMSIS/Device/ST/STM32G4xx/Include
    ${STM32CUBE_DIRECTORY}/Drivers/CMSIS/Core/Include
    ${FIRMWARE_DIRECTORY}/Core/Inc
)

# STM32CubeMX generated application sources
set(STM32_Application_Src
    ${FIRMWARE_DIRECTORY}/startup_stm32g474xx.s
)

# STM32 HAL/LL Drivers sources (exclude templates)
file(GLOB STM32_Drivers_Src
    ${STM32CUBE_DIRECTORY}/Drivers/STM32G4xx_HAL_Driver/Src/*.c
    ${STM32CUBE_DIRECTORY}/Drivers/STM32G4xx_HAL_Driver/Src/Legacy/*.c
    ${STM32CUBE_DIRECTORY}/Drivers/CMSIS/Device/ST/STM32G4xx/Source/Templates/system_stm32g4xx.c
)
list(FILTER STM32_Drivers_Src EXCLUDE REGEX "template")

# Drivers Midllewares
# (none for now)

# Link directories setup
set(STM32_LINK_DIRS

)

# Project static libraries
set(STM32_LINK_LIBS
    stm32_drivers
    ${TOOLCHAIN_LINK_LIBRARIES}

)

add_library(stm32_drivers_interface INTERFACE)


# Create STM32_Drivers static library
add_library(stm32_drivers OBJECT)
target_sources(stm32_drivers PRIVATE ${STM32_Drivers_Src})
target_include_directories(stm32_drivers PUBLIC ${STM32_Include_Dirs})
target_compile_definitions(stm32_drivers PUBLIC ${STM32_Defines_Syms})

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
