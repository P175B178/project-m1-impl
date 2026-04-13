# Cross-compilation toolchain for Raspberry Pi 5 (aarch64 / ARM Cortex-A76)
# Requires: gcc-aarch64-linux-gnu, g++-aarch64-linux-gnu, libgpiod-dev:arm64
# (all installed in the devcontainer)

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR aarch64)

set(CMAKE_C_COMPILER        aarch64-linux-gnu-gcc)
set(CMAKE_CXX_COMPILER      aarch64-linux-gnu-g++)
set(CMAKE_LIBRARY_ARCHITECTURE aarch64-linux-gnu)

find_program(CROSS_CXX aarch64-linux-gnu-g++)
if(NOT CROSS_CXX)
  message(FATAL_ERROR
    "Cross-compiler aarch64-linux-gnu-g++ not found. "
    "Install with: apt-get install gcc-aarch64-linux-gnu g++-aarch64-linux-gnu")
endif()

# Ubuntu multiarch installs arm64 packages under /usr/lib/aarch64-linux-gnu/.
# Setting CMAKE_FIND_ROOT_PATH to /usr makes find_library() search there.
set(CMAKE_FIND_ROOT_PATH /usr)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

# Prevent CMake from trying to execute cross-compiled test binaries
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
