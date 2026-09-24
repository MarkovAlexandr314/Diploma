add_executable(unit_tests
    main_test.cpp
)

target_link_libraries(unit_tests PRIVATE
    cgns_core
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(unit_tests)
