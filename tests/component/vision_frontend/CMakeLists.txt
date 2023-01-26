set(target_name gtest_vision_frontend)
set(build_dir ${CMAKE_BINARY_DIR}/component/vision/frontend)
set(gsdk_flag "-s" "${GSDK_DIR}")
set(gsdk_print_flag "-s ${GSDK_DIR}")
set(FRONTEND_DIR ${SOURCE_DIR}/component/vision_frontend)

add_executable(
  ${target_name}
  test_image_utils.cc
  test_centroids.cc
  test_histogram.cc
  test_bboxes.cc
  ${FRONTEND_DIR}/src/bboxes.cc
  ${FRONTEND_DIR}/src/centroids.cc
  ${FRONTEND_DIR}/src/histogram.cc
  ${FRONTEND_DIR}/src/image_utils.cc
  # GSDK Dependency
  ${GSDK_DIR}/platform/common/src/sl_slist.c
)

target_include_directories(
  ${target_name}
  PUBLIC

  ${GSDK_DIR}/platform/common/inc
  ${FRONTEND_DIR}/inc
)

target_compile_definitions(${target_name} PUBLIC UNIT_TEST)

target_link_libraries(
  ${target_name}
  GTest::gtest_main
)

gtest_discover_tests(${target_name})