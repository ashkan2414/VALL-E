set(CMAKE_SYSTEM_NAME               Generic)
set(CMAKE_SYSTEM_PROCESSOR          arm)

set(CMAKE_C_COMPILER_ID GNU)
set(CMAKE_CXX_COMPILER_ID GNU)

set(TARGET_APP "hello_world" CACHE STRING "Select App to build: Blinky, Controller, etc.")
set(TARGET_PLATFORM "stm32/g474xx" CACHE STRING "Select Target Platform: stm32/g474xx, etc.")


set(VALLE_FIRMWARE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(VALLE_COMMON_DIR "${VALLE_FIRMWARE_DIR}/common")
set(VALLE_PLATFORM_DIR "${VALLE_FIRMWARE_DIR}/platform/${TARGET_PLATFORM}")
set(VALLE_APP_DIR "${VALLE_FIRMWARE_DIR}/apps/${TARGET_APP}")
set(VALLE_THIRDPARTY_DIR "${VALLE_FIRMWARE_DIR}/thirdparty")


# Some default GCC settings
# arm-none-eabi- must be part of path environment
set(TOOLCHAIN_PREFIX                arm-none-eabi-)

set(CMAKE_C_COMPILER                ${TOOLCHAIN_PREFIX}gcc)
set(CMAKE_ASM_COMPILER              ${CMAKE_C_COMPILER})
set(CMAKE_CXX_COMPILER              ${TOOLCHAIN_PREFIX}g++)
set(CMAKE_LINKER                    ${TOOLCHAIN_PREFIX}g++)
set(CMAKE_OBJCOPY                   ${TOOLCHAIN_PREFIX}objcopy)
set(CMAKE_SIZE                      ${TOOLCHAIN_PREFIX}size)
set(CMAKE_AR                        ${TOOLCHAIN_PREFIX}gcc-ar)
set(CMAKE_RANLIB                    ${TOOLCHAIN_PREFIX}gcc-ranlib)
set(CMAKE_NM                        ${TOOLCHAIN_PREFIX}gcc-nm)

set(CMAKE_EXECUTABLE_SUFFIX_ASM     ".elf")
set(CMAKE_EXECUTABLE_SUFFIX_C       ".elf")
set(CMAKE_EXECUTABLE_SUFFIX_CXX     ".elf")

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# MCU specific flags
set(TARGET_FLAGS "-mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${TARGET_FLAGS}")
set(CMAKE_ASM_FLAGS "${CMAKE_C_FLAGS} -x assembler-with-cpp -MMD -MP")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -fdata-sections -ffunction-sections -fno-common")

set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -fno-rtti -fno-exceptions -fno-threadsafe-statics -fno-use-cxa-atexit")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wdouble-promotion -Wconversion -fconcepts-diagnostics-depth=10")

set(CMAKE_C_FLAGS_DEBUG "-O0 -g3")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g3")

set(CMAKE_C_FLAGS_RELEASE "-Os -g0")
set(CMAKE_CXX_FLAGS_RELEASE "-Os -g0")

set(CMAKE_EXE_LINKER_FLAGS "${TARGET_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -T \"${VALLE_PLATFORM_DIR}/flash.ld\"")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --specs=nano.specs")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Map=${CMAKE_PROJECT_NAME}.map -Wl,--gc-sections")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--print-memory-usage")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "-flto")

set(TOOLCHAIN_LINK_LIBRARIES "m")
