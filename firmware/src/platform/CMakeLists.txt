cmake_minimum_required(VERSION 3.22)

add_library(valle_platform STATIC)

# Add source files to the platform library
file(GLOB_RECURSE PLATFORM_PRIVATE_SOURCES
    "${VALLE_TARGET_PLATFORM_DIR}/src/private/*.cpp"
    "${VALLE_TARGET_PLATFORM_DIR}/src/private/*.c"
    "${VALLE_TARGET_PLATFORM_DIR}/src/private/**/*.cpp"
    "${VALLE_TARGET_PLATFORM_DIR}/src/private/**/*.c"
)
set(PLATFORM_PRIVATE_SOURCES ${PLATFORM_PRIVATE_SOURCES} "${VALLE_TARGET_PLATFORM_DIR}/startup.s")
file(GLOB_RECURSE PLATFORM_INTERFACE_SOURCES
    "${VALLE_TARGET_PLATFORM_DIR}/src/interface/*.cpp"
    "${VALLE_TARGET_PLATFORM_DIR}/src/interface/*.c"
    "${VALLE_TARGET_PLATFORM_DIR}/src/interface/**/*.cpp"
    "${VALLE_TARGET_PLATFORM_DIR}/src/interface/**/*.c"
)
target_sources(valle_platform PRIVATE ${PLATFORM_PRIVATE_SOURCES} INTERFACE ${PLATFORM_INTERFACE_SOURCES})

# Add include directories for the platform library
target_include_directories(valle_platform
    PUBLIC
        ${VALLE_TARGET_PLATFORM_DIR}/inc
        ${VALLE_APP_INTEGRATION_INCLUDE_DIRS}
    PRIVATE
        ${VALLE_TARGET_PLATFORM_DIR}/src/private
)

# Link libraries
target_link_libraries(valle_platform PUBLIC
    valle_base
    stm32g4_drivers
)

# Precompile common headers for faster builds
target_precompile_headers(valle_platform PRIVATE "${VALLE_TARGET_PLATFORM_DIR}/pch.h")