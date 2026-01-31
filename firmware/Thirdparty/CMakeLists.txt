cmake_minimum_required(VERSION 3.22)

# Enable CMake support for ASM and C languages
enable_language(C CXX ASM)

set(THIRDPARTY_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
set(FIRMWARE_DIRECTORY ${THIRDPARTY_DIRECTORY}/..)

# ==============================================================================
# STM32Cube Integration
# ==============================================================================

set(STM32CUBE_DIRECTORY ${FIRMWARE_DIRECTORY}/Thirdparty/STM32CubeG4)

# STM32CubeMX generated symbols (macros)
set(stm32cube_defines_syms
	USE_HAL_DRIVER
	STM32G474xx
    $<$<CONFIG:Debug>:DEBUG>
)

# STM32CubeMX generated include paths
set(stm32cube_include_dirs
    ${STM32CUBE_DIRECTORY}/Drivers/STM32G4xx_HAL_Driver/Inc
    ${STM32CUBE_DIRECTORY}/Drivers/CMSIS/Device/ST/STM32G4xx/Include
    ${STM32CUBE_DIRECTORY}/Drivers/CMSIS/Core/Include
    ${FIRMWARE_DIRECTORY}/Platform/Inc
)

# STM32CubeMX generated application sources
set(stm32cube_application_src
    ${FIRMWARE_DIRECTORY}/Platform/Src/startup_stm32g474xx.s
)

# STM32 HAL/LL Drivers sources (exclude templates)
file(GLOB stm32cube_drivers_src
    ${STM32CUBE_DIRECTORY}/Drivers/STM32G4xx_HAL_Driver/Src/*.c
    ${STM32CUBE_DIRECTORY}/Drivers/STM32G4xx_HAL_Driver/Src/Legacy/*.c
    ${STM32CUBE_DIRECTORY}/Drivers/CMSIS/Device/ST/STM32G4xx/Source/Templates/system_stm32g4xx.c
)
list(FILTER stm32cube_drivers_src EXCLUDE REGEX "template")

# Drivers Middlewares
# (none for now)

# Link directories setup
set(stm32cube_link_dirs

)

# Project static libraries
set(stm32cube_link_libs
    stm32_drivers
    ${TOOLCHAIN_LINK_LIBRARIES}
)

# Create stm32cube_Drivers static library
add_library(stm32_drivers OBJECT)
target_sources(stm32_drivers PRIVATE ${stm32cube_drivers_src})
target_include_directories(stm32_drivers SYSTEM PUBLIC ${stm32cube_include_dirs})
target_compile_definitions(stm32_drivers PUBLIC ${stm32cube_defines_syms})
target_compile_options(stm32_drivers PRIVATE -w)

# Add STM32CubeMX generated application sources to the project
target_sources(${CMAKE_PROJECT_NAME} PRIVATE ${stm32cube_application_src})

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE ${stm32cube_link_dirs})

# Add libraries to the project
target_link_libraries(${CMAKE_PROJECT_NAME} ${stm32cube_link_libs})

# Add the map file to the list of files to be removed with 'clean' target
set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES ADDITIONAL_CLEAN_FILES ${CMAKE_PROJECT_NAME}.map)

# Validate that STM32CubeMX code is compatible with C standard
if((CMAKE_C_STANDARD EQUAL 90) OR (CMAKE_C_STANDARD EQUAL 99))
    message(ERROR "Generated code requires C11 or higher")
endif()

# ==============================================================================
# Eigen Integration
# ==============================================================================

add_subdirectory(eigen)
target_link_libraries(${CMAKE_PROJECT_NAME} Eigen3::Eigen)

# ==============================================================================
# Delegate Integration
# ==============================================================================

add_library(delegate INTERFACE)
target_include_directories(delegate SYSTEM INTERFACE ${THIRDPARTY_DIRECTORY})
target_link_libraries(${CMAKE_PROJECT_NAME} delegate)

# ==============================================================================
# PortableBitfields Integration
# ==============================================================================
add_library(portable_bitfields INTERFACE)
target_include_directories(portable_bitfields SYSTEM INTERFACE ${THIRDPARTY_DIRECTORY}/PortableBitfields/src)
target_link_libraries(${CMAKE_PROJECT_NAME} portable_bitfields)

# ==============================================================================
# magic enum Integration
# ==============================================================================
add_subdirectory(${THIRDPARTY_DIRECTORY}/magic_enum)
target_link_libraries(${CMAKE_PROJECT_NAME} magic_enum::magic_enum)

# ==============================================================================
# fmt Integration
# ==============================================================================
set(FMT_OS OFF)
set(FMT_UNICODE OFF)
add_subdirectory(${THIRDPARTY_DIRECTORY}/fmt)
target_link_libraries(${CMAKE_PROJECT_NAME} fmt::fmt-header-only)
target_compile_definitions(${CMAKE_PROJECT_NAME}  PRIVATE FMT_USE_CHRONO=0 FMT_USE_WCHAR=0 FMT_USE_LOCALE=0)