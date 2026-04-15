add_executable(warden_tests
    test_state_machine.cpp
    test_averaging.cpp
    test_warden_app.cpp
)

target_link_libraries(warden_tests PRIVATE
    warden_core
    GTest::gtest_main
    GTest::gmock
)

target_include_directories(warden_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/tests
    ${CMAKE_SOURCE_DIR}/src/core
)

include(GoogleTest)
gtest_discover_tests(warden_tests)
