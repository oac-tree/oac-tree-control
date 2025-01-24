# control library compatibility
add_library(sequencer-control SHARED)

set_target_properties(sequencer-control PROPERTIES
  EXPORT_NAME sequencer-control
  SOVERSION ${LIBSOVERSION}
  VERSION ${LIBVERSION}
  LIBRARY_OUTPUT_DIRECTORY ${PLUGIN_RUNTIME_DIRECTORY}
)

target_sources(sequencer-control
  PRIVATE
  dummy.cpp
)

target_link_libraries(sequencer-control PUBLIC oac-tree-control)

install(TARGETS sequencer-control DESTINATION ${PLUGIN_PATH})
