set(target_name gtest_htm)
set(build_dir ${CMAKE_BINARY_DIR}/component/htm)
set(gsdk_flag "-s" "${GSDK_DIR}")
set(gsdk_print_flag "-s ${GSDK_DIR}")
set(COMPONENT_DIR ${SOURCE_DIR}/component/htm)
set(LIBFORT_DIR ${SOURCE_DIR}/component/libfort)

add_executable(
  ${target_name}
  test_sdr.cc
  test_sp.cc
  test_tm.cc
  test_encoder.cc
  test_htm.cc
  ${COMPONENT_DIR}/src/sl_htm_sdr.c
  ${COMPONENT_DIR}/src/sl_htm_sp.c
  ${COMPONENT_DIR}/src/sl_htm_tm.c
  ${COMPONENT_DIR}/src/sl_htm_tm_cell.c
  ${COMPONENT_DIR}/src/sl_htm_tm_column.c
  ${COMPONENT_DIR}/src/sl_htm_tm_segment.c
  ${COMPONENT_DIR}/src/sl_htm_tm_synapse.c
  ${COMPONENT_DIR}/src/sl_htm_tm_types.c
  ${COMPONENT_DIR}/src/sl_htm.c
  ${COMPONENT_DIR}/src/sl_htm_encoder.c
  ${COMPONENT_DIR}/src/sl_htm_utils.c

  ${LIBFORT_DIR}/fort.c
)

target_include_directories(
  ${target_name}
  PUBLIC

  ${COMPONENT_DIR}/inc
  
  ${LIBFORT_DIR}
)

target_compile_definitions(${target_name} PUBLIC UNIT_TEST)

target_link_libraries(
  ${target_name}
  GTest::gtest_main
)

gtest_discover_tests(${target_name})