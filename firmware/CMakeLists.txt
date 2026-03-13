cmake_minimum_required(VERSION 3.22)

# ==============================================================================
# APPLICATION SELECTOR
# ==============================================================================
set(TARGET_APP "hello_world" CACHE STRING "Select App to build: Blinky, Controller, etc.")
set(TARGET_PLATFORM "stm32/g474xx" CACHE STRING "Select Target Platform: stm32/g474xx, etc.")


set(VALLE_FIRMWARE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(VALLE_THIRDPARTY_DIR "${VALLE_FIRMWARE_DIR}/thirdparty")
set(VALLE_SOURCE_DIR ${VALLE_FIRMWARE_DIR}/src)
set(VALLE_BASE_DIR "${VALLE_SOURCE_DIR}/base")
set(VALLE_PLATFORM_DIR "${VALLE_SOURCE_DIR}/platform")
set(VALLE_TARGET_PLATFORM_DIR "${VALLE_PLATFORM_DIR}/${TARGET_PLATFORM}")
set(VALLE_SYSTEM_DIR "${VALLE_SOURCE_DIR}/system")
set(VALLE_APP_DIR "${VALLE_SOURCE_DIR}/apps")
set(VALLE_APP_COMMON_DIR "${VALLE_APP_DIR}/common")
set(VALLE_TARGET_APP_DIR "${VALLE_APP_DIR}/${TARGET_APP}")

# Sanity Check: Ensure the requested Application exists
if(NOT EXISTS "${VALLE_TARGET_APP_DIR}")
    message(FATAL_ERROR "Error: Application folder 'src/apps/${TARGET_APP}' not found! Check spelling or create the folder.")
else()
    message(STATUS "CONFIGURING FOR TARGET APPLICATION: ${TARGET_APP}")
endif()

# Sanity Check: Ensure the requested Platform exists
if(NOT EXISTS "${VALLE_TARGET_PLATFORM_DIR}")
    message(FATAL_ERROR "Error: Platform folder 'src/platform/${TARGET_PLATFORM}' not found! Check spelling or create the folder.")
else()
    message(STATUS "CONFIGURING FOR TARGET PLATFORM: ${TARGET_PLATFORM}")
endif()

set(VALLE_APP_INTEGRATION_INCLUDE_DIRS
    ${VALLE_APP_COMMON_DIR}/inc
    ${VALLE_TARGET_APP_DIR}
)

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
add_executable(${CMAKE_PROJECT_NAME} "${VALLE_SOURCE_DIR}/main.cpp")

# Add the Thirdparty directory
add_subdirectory(thirdparty)

# Add the main source directories (base, platform, system, apps)
add_subdirectory(src)

# ==============================================================================
# LINKING
# ==============================================================================
target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC
    ${TOOLCHAIN_LINK_LIBRARIES}
    valle_base
    valle_platform
    valle_system
    valle_app
)


# ==============================================================================
# COMPILER FLAGS
# ==============================================================================

# Add the map file to the list of files to be removed with 'clean' target
set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES ADDITIONAL_CLEAN_FILES ${CMAKE_PROJECT_NAME}.map)


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