cmake_minimum_required(VERSION 3.22)

add_library(valle_app_common_platform STATIC)

# Add source files to the app common library
file(GLOB_RECURSE APP_COMMON_PLATFORM_SOURCES
    "${VALLE_APP_COMMON_TARGET_PLATFORM_DIR}/src/*.cpp"
    "${VALLE_APP_COMMON_TARGET_PLATFORM_DIR}/src/*.c"
    "${VALLE_APP_COMMON_TARGET_PLATFORM_DIR}/src/**/*.cpp"
    "${VALLE_APP_COMMON_TARGET_PLATFORM_DIR}/src/**/*.c"
)
target_sources(valle_app_common_platform PRIVATE ${APP_COMMON_PLATFORM_SOURCES})

# Add include directories for the app common library
target_include_directories(valle_app_common_platform
    PUBLIC
        ${VALLE_APP_COMMON_TARGET_PLATFORM_DIR}/inc
    PRIVATE
        ${VALLE_APP_COMMON_TARGET_PLATFORM_DIR}/src
        ${VALLE_APP_INTEGRATION_INCLUDE_DIRS}
)

# Link againts other valle modules
target_link_libraries(valle_app_common_platform PUBLIC
    valle_base
    valle_platform
    valle_system
    valle_app_common_shared
)
