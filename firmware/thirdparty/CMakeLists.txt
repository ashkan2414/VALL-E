cmake_minimum_required(VERSION 3.22)

# ==============================================================================
# PLATFORM THIRDPARTY LIBRARIES
# ==============================================================================
set(VALLE_TARGET_PLATFORM_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../src/platform/${TARGET_PLATFORM}")
include(${VALLE_TARGET_PLATFORM_DIR}/thirdparty.cmake)

# ==============================================================================
# Eigen Integration
# ==============================================================================
add_subdirectory(eigen)

# ==============================================================================
# Delegate Integration
# ==============================================================================
add_library(delegate INTERFACE)
target_include_directories(delegate SYSTEM INTERFACE ${VALLE_THIRDPARTY_DIR})

# ==============================================================================
# PortableBitfields Integration
# ==============================================================================
add_library(portable_bitfields INTERFACE)
target_include_directories(portable_bitfields SYSTEM INTERFACE ${VALLE_THIRDPARTY_DIR}/PortableBitfields/src)

# ==============================================================================
# magic enum Integration
# ==============================================================================
add_subdirectory(${VALLE_THIRDPARTY_DIR}/magic_enum)

# ==============================================================================
# fmt Integration
# ==============================================================================
set(FMT_OS OFF)
set(FMT_UNICODE OFF)
add_subdirectory(${VALLE_THIRDPARTY_DIR}/fmt)
