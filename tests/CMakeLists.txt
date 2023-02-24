cmake_minimum_required(VERSION 3.20)
project(machine_learning_applications_tests)
# Coding standards require this
add_compile_options(-Wall)
# GoogleTest requires at least C++14
set(CMAKE_CXX_STANDARD 14)

set(ARM_GCC_DIR "ARM_GCC_DIR-NOTFOUND" CACHE PATH "arm-none-eabi-* toolchain bin/ directory")
set(GSDK_DIR "GSDK_DIR-NOTFOUND" CACHE PATH "Gecko SDK directory")

cmake_path(GET CMAKE_SOURCE_DIR PARENT_PATH SOURCE_DIR)

include(cmake/import_googletest.cmake)
include(cmake/add_slcp_project.cmake)
include(cmake/install_extension.cmake)

install_gsdk_extension(${GSDK_DIR})

add_subdirectory(component)
add_subdirectory(application)