cmake_minimum_required(VERSION 3.22)

add_library(valle_base STATIC)

# Add source files to the base library
file(GLOB_RECURSE BASE_SOURCES
    "${VALLE_BASE_DIR}/src/*.cpp"
    "${VALLE_BASE_DIR}/src/*.c"
    "${VALLE_BASE_DIR}/src/**/*.cpp"
    "${VALLE_BASE_DIR}/src/**/*.c"
)
target_sources(valle_base PRIVATE ${BASE_SOURCES})

# Add include directories for the base library
target_include_directories(valle_base
    PUBLIC
        ${VALLE_BASE_DIR}/inc
        ${VALLE_APP_INTEGRATION_INCLUDE_DIRS}
    PRIVATE
        ${VALLE_BASE_DIR}/src
)


# Link againts common thirdparty libraries
target_link_libraries(valle_base PUBLIC
    Eigen3::Eigen
    delegate
    portable_bitfields
    magic_enum::magic_enum
    fmt::fmt-header-only
)

target_compile_definitions(valle_base PUBLIC
    FMT_USE_CHRONO=0
    FMT_USE_WCHAR=0
    FMT_USE_LOCALE=0
)

# Precompile common headers for faster builds
target_precompile_headers(valle_base PRIVATE "${VALLE_BASE_DIR}/pch.h")