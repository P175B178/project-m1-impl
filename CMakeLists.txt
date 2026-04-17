cmake_minimum_required(VERSION 3.25)
project(main LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(ENABLE_CLANG_TIDY "Run clang-tidy on every translation unit during build" OFF)

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

FetchContent_Declare(
    tomlplusplus
    GIT_REPOSITORY https://github.com/marzer/tomlplusplus.git
    GIT_TAG        v3.4.0
    FIND_PACKAGE_ARGS
)
FetchContent_MakeAvailable(tomlplusplus)

include(CTest)

if(BUILD_TESTING)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        v1.16.0
        FIND_PACKAGE_ARGS
    )
    FetchContent_MakeAvailable(googletest)
endif()

# ── clang-tidy (set after FetchContent so it only applies to project targets) ──
if(ENABLE_CLANG_TIDY)
    find_program(CLANG_TIDY_EXE clang-tidy REQUIRED)
    set(CMAKE_CXX_CLANG_TIDY
        ${CLANG_TIDY_EXE}
        "--header-filter=^${CMAKE_SOURCE_DIR}/(include|src|tests)/")
endif()

# ── Core library (no hardware deps, testable on host) ──
file(GLOB_RECURSE CORE_SOURCES CONFIGURE_DEPENDS src/core/*.cpp)
add_library(warden_core ${CORE_SOURCES})
target_include_directories(warden_core PUBLIC include src/core)
target_link_libraries(warden_core PUBLIC spdlog::spdlog tomlplusplus::tomlplusplus)

# ── Main executable ──
add_executable(main src/main.cpp)
target_include_directories(main PRIVATE src)
target_link_libraries(main PRIVATE warden_core)

# ── Format target ──
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
    add_custom_target(format-check
        COMMAND ${CLANG_FORMAT_EXE} --dry-run --Werror ${ALL_SOURCE_FILES}
        COMMENT "Checking formatting (dry run)"
    )
endif()

# ── Tests ──
if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
