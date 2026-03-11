cmake_minimum_required(VERSION 3.22)

add_subdirectory(common)

add_library(valle_app STATIC)

# Add source files to the app library
file(GLOB_RECURSE APP_SOURCES
    "${VALLE_TARGET_APP_DIR}/*.cpp"
    "${VALLE_TARGET_APP_DIR}/*.c"
    "${VALLE_TARGET_APP_DIR}/**/*.cpp"
    "${VALLE_TARGET_APP_DIR}/**/*.c"
)
target_sources(valle_app PRIVATE ${APP_SOURCES})

# Add include directories for the app library
target_include_directories(valle_app
    PRIVATE
        ${VALLE_TARGET_APP_DIR}
)

# Link againts other valle modules
target_link_libraries(valle_app PUBLIC
    valle_base
    valle_platform
    valle_system
    valle_app_common
)

