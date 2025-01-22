add_library(oac-tree-control SHARED)

set_target_properties(oac-tree-control PROPERTIES
  EXPORT_NAME oac-tree-control
  SOVERSION ${LIBSOVERSION}
  VERSION ${LIBVERSION}
  LIBRARY_OUTPUT_DIRECTORY ${PLUGIN_RUNTIME_DIRECTORY}
)

target_sources(oac-tree-control
  PRIVATE
    achieve_condition_instruction.cpp
    achieve_condition_with_override_instruction.cpp
    achieve_condition_with_timeout_instruction.cpp
    context_override_instruction_wrapper.cpp
    execute_while_instruction.cpp
    non_owning_instruction_wrapper.cpp
    wait_for_condition_instruction.cpp
    wrapped_instruction_manager.cpp
    wrapped_user_interface.cpp
)

target_include_directories(oac-tree-control PUBLIC
  $<INSTALL_INTERFACE:include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/../..>
)

target_link_libraries(oac-tree-control PUBLIC oac-tree::oac-tree)

install(TARGETS oac-tree-control DESTINATION ${PLUGIN_PATH})
