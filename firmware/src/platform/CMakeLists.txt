cmake_minimum_required(VERSION 3.22)


file(GLOB_RECURSE PLATFORM_PRIVATE_SOURCES
    "${VALLE_TARGET_PLATFORM_DIR}/src/private/*.cpp"
    "${VALLE_TARGET_PLATFORM_DIR}/src/private/*.c"
    "${VALLE_TARGET_PLATFORM_DIR}/src/private/**/*.cpp"
    "${VALLE_TARGET_PLATFORM_DIR}/src/private/**/*.c"
)
file(GLOB_RECURSE PLATFORM_INTERFACE_SOURCES
    "${VALLE_TARGET_PLATFORM_DIR}/src/interface/*.cpp"
    "${VALLE_TARGET_PLATFORM_DIR}/src/interface/*.c"
    "${VALLE_TARGET_PLATFORM_DIR}/src/interface/**/*.cpp"
    "${VALLE_TARGET_PLATFORM_DIR}/src/interface/**/*.c"
)
file(GLOB_RECURSE PLATFORM_STUB_SOURCES
    "${VALLE_TARGET_PLATFORM_DIR}/src/stubs/*.cpp"
    "${VALLE_TARGET_PLATFORM_DIR}/src/stubs/*.c"
    "${VALLE_TARGET_PLATFORM_DIR}/src/stubs/*.s"
    "${VALLE_TARGET_PLATFORM_DIR}/src/stubs/**/*.cpp"
    "${VALLE_TARGET_PLATFORM_DIR}/src/stubs/**/*.c"
    "${VALLE_TARGET_PLATFORM_DIR}/src/stubs/**/*.s"
)

## Source library
add_library(valle_platform_source STATIC)

# Add source files to the platform library

target_sources(valle_platform_source PRIVATE ${PLATFORM_PRIVATE_SOURCES} INTERFACE ${PLATFORM_INTERFACE_SOURCES})

# Add include directories for the platform library
target_include_directories(valle_platform_source
    PUBLIC
        ${VALLE_TARGET_PLATFORM_DIR}/inc
        ${VALLE_APP_INTEGRATION_INCLUDE_DIRS}
    PRIVATE
        ${VALLE_TARGET_PLATFORM_DIR}/src/private
)

# Link libraries
target_link_libraries(valle_platform_source PUBLIC
    valle_base
    stm32g4_drivers
)

# Precompile common headers for faster builds
target_precompile_headers(valle_platform_source PRIVATE "${VALLE_TARGET_PLATFORM_DIR}/pch.h")


## Add stubs for platform-specific implementations
add_library(valle_platform_stubs OBJECT)
target_sources(valle_platform_stubs PRIVATE ${PLATFORM_STUB_SOURCES})

## Interface wrapper to bundle source and stubs together
add_library(valle_platform INTERFACE)
target_link_libraries(valle_platform INTERFACE valle_platform_source)
target_sources(valle_platform INTERFACE
    $<TARGET_OBJECTS:valle_platform_stubs>
)
