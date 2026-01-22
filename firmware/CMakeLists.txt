cmake_minimum_required(VERSION 3.22)

# ==============================================================================
# APPLICATION SELECTOR
# ==============================================================================
set(TARGET_APP "hello_world" CACHE STRING "Select App to build: Blinky, Controller, etc.")

# Sanity Check: Ensure the requested App exists
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/Apps/${TARGET_APP}")
    message(FATAL_ERROR "Error: Application folder 'Apps/${TARGET_APP}' not found! Check spelling or create the folder.")
else()
    message(STATUS "CONFIGURING FOR TARGET APPLICATION: ${TARGET_APP}")
endif()

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

# Core project settings
project(${CMAKE_PROJECT_NAME})

# Enable Assembly, C, and C++
enable_language(C CXX ASM)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# ==============================================================================
# STM32 Driver INTEGRATION
# ==============================================================================
# Create the final executable artifact
add_executable(${CMAKE_PROJECT_NAME})

# Add the Thirdparty directory
# This brings in the HAL, the startup code, and the MCU definition flags
add_subdirectory(Thirdparty)

# ==============================================================================
# SOURCE FILE MANAGEMENT
# ==============================================================================

# A. Add the Selected Application Entry Point
file(GLOB TARGET_APP_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/Apps/${TARGET_APP}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Apps/${TARGET_APP}/*.c"

)
target_sources(${CMAKE_PROJECT_NAME} PRIVATE ${TARGET_APP_SOURCES})

# B. Add Common User Libraries (PID, Sensors, Utils)
# We use GLOB_RECURSE to automatically find any .c/.cpp files in Common/
file(GLOB_RECURSE COMMON_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/Common/Src/**/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Common/Src/**/*.c"
)
target_sources(${CMAKE_PROJECT_NAME} PRIVATE ${COMMON_SOURCES})

# C. Auto-Discovery in Core
# This finds any .cpp or .c file you add to Core/Src,
# BUT excludes main.c if you accidentally have a main.cpp to avoid conflicts.
file(GLOB CORE_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/Core/Src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Core/Src/*.c"
)
target_sources(${CMAKE_PROJECT_NAME} PRIVATE ${CORE_SOURCES})

# ==============================================================================
# INCLUDE DIRECTORIES
# ==============================================================================
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Core/Inc                  # CubeMX generated headers (main.h)
    ${CMAKE_CURRENT_SOURCE_DIR}/Common/Inc                # Your Common library headers (PID.h)
    ${CMAKE_CURRENT_SOURCE_DIR}/Apps/${TARGET_APP}        # The specific App's local headers
)

# ==============================================================================
# COMPILER FLAGS
# ==============================================================================
target_precompile_headers(${CMAKE_PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/Common/Inc/Valle/pch.hpp")

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