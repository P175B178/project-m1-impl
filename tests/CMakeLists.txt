add_executable(warden_tests
    test_state_machine.cpp
    test_averaging.cpp
    test_warden_app.cpp
)

target_link_libraries(warden_tests PRIVATE
    warden_core
    Catch2::Catch2WithMain
)

target_include_directories(warden_tests PRIVATE ${CMAKE_SOURCE_DIR}/tests)

include(Catch)
catch_discover_tests(warden_tests)
