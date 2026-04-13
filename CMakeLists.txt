cmake_minimum_required(VERSION 3.25)
project(warden-m1 VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(ENABLE_HARDWARE "Build hardware drivers (requires libgpiod, only for Raspberry Pi)" OFF)

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

# ── Core library (no hardware deps, testable on host) ────────────────────────
add_library(warden_core
    src/StateMachine.cpp
    src/ConfigLoader.cpp
    src/WardenApp.cpp
)
target_include_directories(warden_core PUBLIC include)
target_link_libraries(warden_core PUBLIC spdlog::spdlog tomlplusplus::tomlplusplus)

# ── Hardware library (Raspberry Pi only) ─────────────────────────────────────
if(ENABLE_HARDWARE)
    find_library(GPIOD_LIB  gpiod  REQUIRED)
    find_path(GPIOD_INCLUDE gpiod.h REQUIRED)

    add_library(warden_hardware
        src/hardware/led/SysfsLed.cpp
        src/hardware/led/StatusLed.cpp
        src/hardware/sensor/Dht22Sensor.cpp
        src/hardware/buzzer/GpioPin.cpp
        src/hardware/buzzer/GpioBuzzer.cpp
    )
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

# ── Tests ─────────────────────────────────────────────────────────────────────
include(CTest)
if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
