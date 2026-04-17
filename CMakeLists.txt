cmake_minimum_required(VERSION 3.25)
project(main LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ── Dependencies ──
include(FetchContent)
set(FETCHCONTENT_UPDATES_DISCONNECTED ON)

FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG        v1.15.3
    FIND_PACKAGE_ARGS
)
FetchContent_MakeAvailable(spdlog)

# ── Core library (no hardware deps, testable on host) ──
file(GLOB_RECURSE CORE_SOURCES CONFIGURE_DEPENDS src/core/*.cpp)
add_library(warden_core ${CORE_SOURCES})
target_include_directories(warden_core PUBLIC include src/core)
target_link_libraries(warden_core PUBLIC spdlog::spdlog)

# ── Main executable ──
add_executable(main src/main.cpp)
target_include_directories(main PRIVATE src)
target_link_libraries(main PRIVATE warden_core)
