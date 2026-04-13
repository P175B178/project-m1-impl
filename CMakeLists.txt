cmake_minimum_required(VERSION 3.25)
project(warden-m1 VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(ENABLE_HARDWARE "Build hardware drivers (requires libgpiod, only for Raspberry Pi)" OFF)
option(ENABLE_CLANG_TIDY "Run clang-tidy on every translation unit during build" OFF)

# ── Dependencies ──────────────────────────────────────────────────────────────
include(FetchContent)
set(FETCHCONTENT_UPDATES_DISCONNECTED ON)  # skip re-fetching once downloaded

FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG        v1.15.3
    FIND_PACKAGE_ARGS
)
FetchContent_MakeAvailable(spdlog)

FetchContent_Declare(
    tomlplusplus
    GIT_REPOSITORY https://github.com/marzer/tomlplusplus.git
    GIT_TAG        v3.4.0
    FIND_PACKAGE_ARGS
)
FetchContent_MakeAvailable(tomlplusplus)

if(BUILD_TESTING)
    FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG        v3.8.1
        FIND_PACKAGE_ARGS 3
    )
    FetchContent_MakeAvailable(Catch2)
endif()

# ── clang-tidy (set after FetchContent so it only applies to project targets) ─
if(ENABLE_CLANG_TIDY)
    find_program(CLANG_TIDY_EXE clang-tidy REQUIRED)
    set(CMAKE_CXX_CLANG_TIDY ${CLANG_TIDY_EXE})
endif()

# ── Core library (no hardware deps, testable on host) ────────────────────────
file(GLOB_RECURSE CORE_SOURCES CONFIGURE_DEPENDS src/core/*.cpp)

add_library(warden_core ${CORE_SOURCES})
target_include_directories(warden_core PUBLIC include)
target_link_libraries(warden_core PUBLIC spdlog::spdlog tomlplusplus::tomlplusplus)

# ── Hardware library (Raspberry Pi only) ─────────────────────────────────────
if(ENABLE_HARDWARE)
    find_library(GPIOD_LIB  gpiod  REQUIRED)
    find_path(GPIOD_INCLUDE gpiod.h REQUIRED)

    file(GLOB_RECURSE HARDWARE_SOURCES CONFIGURE_DEPENDS src/hardware/*.cpp)
    add_library(warden_hardware ${HARDWARE_SOURCES})
    target_include_directories(warden_hardware PUBLIC include src/hardware ${GPIOD_INCLUDE})
    target_link_libraries(warden_hardware PUBLIC warden_core ${GPIOD_LIB})
endif()

# ── Main executable ───────────────────────────────────────────────────────────
add_executable(warden src/main.cpp)
# Inject project version so --version can report it without hardcoding.
target_compile_definitions(warden PRIVATE WARDEN_VERSION="${PROJECT_VERSION}")
if(ENABLE_HARDWARE)
    target_link_libraries(warden PRIVATE warden_core warden_hardware)
else()
    target_link_libraries(warden PRIVATE warden_core)
    target_compile_definitions(warden PRIVATE HARDWARE_DISABLED)
endif()

# ── Install rules ─────────────────────────────────────────────────────────────
install(TARGETS warden RUNTIME DESTINATION bin)
install(FILES config/config.cfg DESTINATION etc/warden)

# ── Packaging (CPack) ─────────────────────────────────────────────────────────
set(CPACK_PACKAGE_NAME            "warden")
set(CPACK_PACKAGE_VERSION         ${PROJECT_VERSION})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Environmental monitor for Raspberry Pi")
set(CPACK_GENERATOR               "TGZ")

if(ENABLE_HARDWARE)
    # DEB package for Raspberry Pi OS (Debian-based)
    list(APPEND CPACK_GENERATOR "DEB")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER   "warden")
    set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "arm64")
    set(CPACK_DEBIAN_PACKAGE_DEPENDS      "libgpiod2")
    set(CPACK_DEBIAN_PACKAGE_DESCRIPTION
        "Monitors temperature and humidity via DHT22 on a Raspberry Pi.\n Signals state through built-in LEDs and a buzzer.")
endif()

include(CPack)

# ── Format target ────────────────────────────────────────────────────────────
find_program(CLANG_FORMAT_EXE clang-format)
if(CLANG_FORMAT_EXE)
    file(GLOB_RECURSE ALL_SOURCE_FILES
        "${CMAKE_SOURCE_DIR}/include/*.hpp"
        "${CMAKE_SOURCE_DIR}/src/*.hpp"
        "${CMAKE_SOURCE_DIR}/src/*.cpp"
    )
    add_custom_target(format
        COMMAND ${CLANG_FORMAT_EXE} -i ${ALL_SOURCE_FILES}
        COMMENT "Formatting all source files"
    )
endif()

# ── Tests ─────────────────────────────────────────────────────────────────────
include(CTest)
if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
