add_executable(warden_tests
    test_state_machine.cpp
    test_averaging.cpp
)

target_link_libraries(warden_tests PRIVATE warden_core GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(warden_tests)
