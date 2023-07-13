add_library(sequencer-control SHARED)

target_compile_features(sequencer-control PRIVATE cxx_std_11)

set_target_properties(sequencer-control PROPERTIES
  EXPORT_NAME sequencer-control
  SOVERSION ${SUP_SEQUENCER_PLUGIN_CONTROL_SOVERSION}
  VERSION ${SUP_SEQUENCER_PLUGIN_CONTROL_BUILDVERSION}
  LIBRARY_OUTPUT_DIRECTORY ${PLUGIN_RUNTIME_DIRECTORY}
)

target_sources(sequencer-control
  PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/internal_instruction_wrapper.cpp
    ${CMAKE_CURRENT_LIST_DIR}/wait_for_condition_instruction.cpp
)

target_include_directories(sequencer-control PUBLIC
  $<INSTALL_INTERFACE:include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/../..>
)

target_link_libraries(sequencer-control PUBLIC sequencer::sequencer)

install(TARGETS sequencer-control DESTINATION ${PLUGIN_PATH})
