cmake_minimum_required(VERSION 3.22)

# ==============================================================================
# PLATFORM THIRDPARTY LIBRARIES
# ==============================================================================
include(${VALLE_PLATFORM_DIR}/thirdparty.cmake)

# ==============================================================================
# Eigen Integration
# ==============================================================================
add_subdirectory(eigen)
target_link_libraries(${CMAKE_PROJECT_NAME} Eigen3::Eigen)

# ==============================================================================
# Delegate Integration
# ==============================================================================
add_library(delegate INTERFACE)
target_include_directories(delegate SYSTEM INTERFACE ${VALLE_THIRDPARTY_DIR})
target_link_libraries(${CMAKE_PROJECT_NAME} delegate)

# ==============================================================================
# PortableBitfields Integration
# ==============================================================================
add_library(portable_bitfields INTERFACE)
target_include_directories(portable_bitfields SYSTEM INTERFACE ${VALLE_THIRDPARTY_DIR}/PortableBitfields/src)
target_link_libraries(${CMAKE_PROJECT_NAME} portable_bitfields)

# ==============================================================================
# magic enum Integration
# ==============================================================================
add_subdirectory(${VALLE_THIRDPARTY_DIR}/magic_enum)
target_link_libraries(${CMAKE_PROJECT_NAME} magic_enum::magic_enum)

# ==============================================================================
# fmt Integration
# ==============================================================================
set(FMT_OS OFF)
set(FMT_UNICODE OFF)
add_subdirectory(${VALLE_THIRDPARTY_DIR}/fmt)
target_link_libraries(${CMAKE_PROJECT_NAME} fmt::fmt-header-only)
target_compile_definitions(${CMAKE_PROJECT_NAME}  PRIVATE FMT_USE_CHRONO=0 FMT_USE_WCHAR=0 FMT_USE_LOCALE=0)