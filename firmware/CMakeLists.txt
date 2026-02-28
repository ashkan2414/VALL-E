cmake_minimum_required(VERSION 3.22)

# ==============================================================================
# APPLICATION SELECTOR
# ==============================================================================
set(TARGET_APP "hello_world" CACHE STRING "Select App to build: Blinky, Controller, etc.")
set(TARGET_PLATFORM "stm32/g474xx" CACHE STRING "Select Target Platform: stm32/g474xx, etc.")


set(VALLE_FIRMWARE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(VALLE_COMMON_DIR "${VALLE_FIRMWARE_DIR}/common")
set(VALLE_PLATFORM_DIR "${VALLE_FIRMWARE_DIR}/platform/${TARGET_PLATFORM}")
set(VALLE_APP_DIR "${VALLE_FIRMWARE_DIR}/apps/${TARGET_APP}")
set(VALLE_THIRDPARTY_DIR "${VALLE_FIRMWARE_DIR}/thirdparty")

# Sanity Check: Ensure the requested Application exists
if(NOT EXISTS "${VALLE_APP_DIR}")
    message(FATAL_ERROR "Error: Application folder 'apps/${TARGET_APP}' not found! Check spelling or create the folder.")
else()
    message(STATUS "CONFIGURING FOR TARGET APPLICATION: ${TARGET_APP}")
endif()

IF (NOT EXISTS "${VALLE_PLATFORM_DIR}")
    message(FATAL_ERROR "Error: Platform folder 'platform/${TARGET_PLATFORM}' not found! Check spelling or create the folder.")
ELSE()
    message(STATUS "CONFIGURING FOR TARGET PLATFORM: ${TARGET_PLATFORM}")
ENDIF()

# ==============================================================================
# PROJECT SETUP & STANDARDS
# ==============================================================================
# Setup C/C++ Standards
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

# Define the build type (Default to Debug)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME VALL-E_Firmware)

# Enable compile command export (for VS Code IntelliSense)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Platform project settings
project(${CMAKE_PROJECT_NAME})

# Enable Assembly, C, and C++
enable_language(C CXX ASM)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# ==============================================================================
# THIRDPARTY LIBRARY INTEGRATION
# ==============================================================================
# Create the final executable artifact
add_executable(${CMAKE_PROJECT_NAME})

target_link_libraries(${CMAKE_PROJECT_NAME} ${TOOLCHAIN_LINK_LIBRARIES})

# Add the Thirdparty directory
add_subdirectory(thirdparty)

# ==============================================================================
# SOURCE FILE MANAGEMENT
# ==============================================================================

# A. Add the Selected Application Entry Point
file(GLOB TARGET_APP_SOURCES
    "${VALLE_APP_DIR}/*.cpp"
    "${VALLE_APP_DIR}/*.c"
    "${VALLE_APP_DIR}/**/*.cpp"
    "${VALLE_APP_DIR}/**/*.c"
)
target_sources(${CMAKE_PROJECT_NAME} PRIVATE ${TARGET_APP_SOURCES})

# B. Add Common User Libraries (PID, Sensors, Utils)
# We use GLOB_RECURSE to automatically find any .c/.cpp files in Common/
file(GLOB_RECURSE COMMON_SOURCES
    "${VALLE_COMMON_DIR}/src/*.cpp"
    "${VALLE_COMMON_DIR}/src/*.c"
    "${VALLE_COMMON_DIR}/src/**/*.cpp"
    "${VALLE_COMMON_DIR}/src/**/*.c"
)
target_sources(${CMAKE_PROJECT_NAME} PRIVATE ${COMMON_SOURCES})

# C. Auto-Discovery in Platform
# This finds any .cpp or .c file you add to platform/src,
file(GLOB PLATFORM_SOURCES
    "${VALLE_PLATFORM_DIR}/src/*.cpp"
    "${VALLE_PLATFORM_DIR}/src/*.c"
    "${VALLE_PLATFORM_DIR}/src/**/*.cpp"
    "${VALLE_PLATFORM_DIR}/src/**/*.c"
)
set(PLATFORM_SOURCES ${PLATFORM_SOURCES} "${VALLE_PLATFORM_DIR}/startup.s")
target_sources(${CMAKE_PROJECT_NAME} PRIVATE ${PLATFORM_SOURCES})


# ==============================================================================
# INCLUDE DIRECTORIES
# ==============================================================================
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    ${VALLE_PLATFORM_DIR}/inc                  # Platform headers (stm32g4xx_hal_conf.h, etc)
    ${VALLE_COMMON_DIR}/inc                    # Application Common headers
    ${VALLE_APP_DIR}                           # The specific App's local headers
)

# ==============================================================================
# COMPILER FLAGS
# ==============================================================================
# target_precompile_headers(
#     ${CMAKE_PROJECT_NAME} PRIVATE "${VALLE_COMMON_DIR}/pch.hpp"
#     ${CMAKE_PROJECT_NAME} PRIVATE "${VALLE_PLATFORM_DIR}/pch.hpp"
#     ${CMAKE_PROJECT_NAME} PRIVATE "${VALLE_THIRDPARTY_DIR}/pch.hpp"
# )


# Add the map file to the list of files to be removed with 'clean' target
set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES ADDITIONAL_CLEAN_FILES ${CMAKE_PROJECT_NAME}.map)

# ==============================================================================
# LINKING
# ==============================================================================
# Fix for linker error regarding 'libob.a' (Standard ST fix)
list(REMOVE_ITEM CMAKE_C_IMPLICIT_LINK_LIBRARIES ob)

# ==============================================================================
# SUMMARY
# ==============================================================================

# Print final success message
set(VALLE_BANNER_LINES
    "---------------------------------------------------"
    " VALL-E Firmware"
    " Target App: ${TARGET_APP}"
    " Build Type: ${CMAKE_BUILD_TYPE}"
    " Compiler:   ${CMAKE_CXX_COMPILER_ID}"
    "---------------------------------------------------"
)

set(VALLE_BANNER_COMMANDS "")

foreach(line IN LISTS VALLE_BANNER_LINES)
    message(STATUS "${line}")
    list(APPEND VALLE_BANNER_COMMANDS COMMAND ${CMAKE_COMMAND} -E echo "${line}")
endforeach()

add_custom_command(
    TARGET ${CMAKE_PROJECT_NAME}
    POST_BUILD
    ${VALLE_BANNER_COMMANDS}
)