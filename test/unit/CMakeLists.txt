add_executable(unit-tests)

target_sources(unit-tests
  PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/test_user_interface.cpp
    ${CMAKE_CURRENT_LIST_DIR}/unit_test_helper.cpp
    ${CMAKE_CURRENT_LIST_DIR}/wait_for_condition_tests.cpp
)

target_include_directories(unit-tests
  PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
)

find_package(GTest REQUIRED)

target_link_libraries(unit-tests PUBLIC GTest::GTest GTest::Main sequencer-control)

set_target_properties(unit-tests PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${TEST_OUTPUT_DIRECTORY})

gtest_discover_tests(unit-tests)
