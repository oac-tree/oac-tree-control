if(NOT COA_PARASOFT_INTEGRATION)
  return()
endif()

include(COASetupParasoft)

# Add parasoft project for oac-tree-control-target
# This generates the build time information needed by Parasoft for analysis
cpptest_add_executable(oac-tree-control-cpptest
  CPPTEST_PROJECT_LOC ${CMAKE_CURRENT_BINARY_DIR}
  CPPTEST_PROJECT_FOLDERS
  oac-tree-control=${CMAKE_SOURCE_DIR}/src/lib/oac-tree/control
  TARGETS oac-tree-control
)
get_target_property(OAC_TREE_CONTROL_INCLUDE_DIRECTORIES oac-tree-control INCLUDE_DIRECTORIES)
target_include_directories(oac-tree-control-cpptest PUBLIC ${OAC_TREE_CONTROL_INCLUDE_DIRECTORIES})

target_link_libraries(oac-tree-control-cpptest
  PUBLIC
  oac-tree-control
)
