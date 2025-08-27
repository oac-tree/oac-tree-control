set(unit-tests oac-tree-control-unit-tests)

add_executable(${unit-tests})

set_target_properties(${unit-tests} PROPERTIES OUTPUT_NAME "unit-tests")
set_target_properties(${unit-tests} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${TEST_OUTPUT_DIRECTORY})

target_sources(${unit-tests}
  PRIVATE
  achieve_condition_tests.cpp
  achieve_condition_with_override_tests.cpp
  achieve_condition_with_timeout_tests.cpp
  execute_while_tests.cpp
  non_owning_instruction_wrapper_tests.cpp
  test_user_interface.cpp
  unit_test_helper.cpp
  wait_for_condition_tests.cpp
)

target_include_directories(${unit-tests}
  PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}
)

find_package(GTest REQUIRED)

target_link_libraries(${unit-tests} PUBLIC GTest::GTest GTest::Main oac-tree-control)

gtest_discover_tests(${unit-tests} DISCOVERY_MODE PRE_TEST)
