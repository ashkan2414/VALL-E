cmake_minimum_required(VERSION 3.22)

# ==============================================================================
# APPLICATION SELECTOR
# ==============================================================================
# Change "Blinky" by passing -DTARGET_APP=Controller to the command line.
set(TARGET_APP "hello_world" CACHE STRING "Select App to build: Blinky, Controller, etc.")

# Sanity Check: Ensure the requested App exists
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/Apps/${TARGET_APP}")
    message(FATAL_ERROR "❌ Error: Application folder 'Apps/${TARGET_APP}' not found! Check spelling or create the folder.")
else()
    message(STATUS "✅ CONFIGURING FOR TARGET APPLICATION: ${TARGET_APP}")
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
# 3. STM32 Driver INTEGRATION
# ==============================================================================
# Create the final executable artifact
add_executable(${CMAKE_PROJECT_NAME})

# Add the STM32 Drivers CMake
# This brings in the HAL, the startup code, and the MCU definition flags
add_subdirectory(Drivers)

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
target_sources(VALL-E_Firmware PRIVATE ${CORE_SOURCES})

# ==============================================================================
# INCLUDE DIRECTORIES
# ==============================================================================
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Core/Inc                  # CubeMX generated headers (main.h)
    ${CMAKE_CURRENT_SOURCE_DIR}/Common/Inc                # Your Common library headers (PID.h)
    ${CMAKE_CURRENT_SOURCE_DIR}/Apps/${TARGET_APP}        # The specific App's local headers
)

# ==============================================================================
# LINKING
# ==============================================================================
# Fix for linker error regarding 'libob.a' (Standard ST fix)
list(REMOVE_ITEM CMAKE_C_IMPLICIT_LINK_LIBRARIES ob)

# Link the stm32_drivers generated library (HAL + Drivers) to our executable
target_link_libraries(${CMAKE_PROJECT_NAME} stm32_drivers)

# Print final success message
message(STATUS "---------------------------------------------------")
message(STATUS " VALL-E Firmware Configured")
message(STATUS " Target App: ${TARGET_APP}")
message(STATUS " Compiler:   ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "---------------------------------------------------")