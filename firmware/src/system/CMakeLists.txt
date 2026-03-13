cmake_minimum_required(VERSION 3.22)

add_library(valle_system STATIC)

# Add source files to the system library
file(GLOB_RECURSE SYSTEM_SOURCES
    "${VALLE_SYSTEM_DIR}/src/*.cpp"
    "${VALLE_SYSTEM_DIR}/src/*.c"
    "${VALLE_SYSTEM_DIR}/src/**/*.cpp"
    "${VALLE_SYSTEM_DIR}/src/**/*.c"
)
target_sources(valle_system PRIVATE ${SYSTEM_SOURCES})

# Add include directories for the system library
target_include_directories(valle_system
    PUBLIC
        ${VALLE_SYSTEM_DIR}/inc
        ${VALLE_APP_INTEGRATION_INCLUDE_DIRS}
    PRIVATE
        ${VALLE_SYSTEM_DIR}/src
)

# Link againts other valle modules
target_link_libraries(valle_system PUBLIC
    valle_base
    valle_platform
)

# Precompile common headers for faster builds
target_precompile_headers(valle_system PRIVATE "${VALLE_SYSTEM_DIR}/pch.h")