# Generates build scripts that can be used to check an application compiles.
#
# This is a simple wrapper which uses `slc` (Simplicity Commander) to generate `.slcp` projects,
# and adds a test which invokeds the generated `make`-based build scripts.
#
# Requires
# ========
# SOURCE_DIR: must be set to the repo root
#
# Parameters
# ==========
# slcp_path: Relative path to an `slcp` file
# target_board: Board to target when generating the `slcp`. Should correspond to a GSDK board component.
# gsdk_dir: Override the default `slc` Gecko SDK directory. Only used if the directory exists.
# arm_gcc_dir: Override the default `arm-none-eabi-*` toolchain directory. Only used if the directory exists.
function(add_slcp_project slcp_path target_board gsdk_dir arm_gcc_dir)
    cmake_path(GET slcp_path FILENAME slcp_filename)
    cmake_path(GET slcp_filename STEM application_name)
    cmake_path(GET slcp_path PARENT_PATH application_dir)
    set(target_name application_${application_name}_${target_board})
    set(build_dir ${CMAKE_BINARY_DIR}/application/${application_name}/${target_board})

    set(gsdk_flag "-s" ${gsdk_dir})
    set(gsdk_print_flag "-s ${gsdk_dir}")

    if(EXISTS ${arm_gcc_dir})
        set(ENV "ARM_GCC_DIR" ${arm_gcc_dir})
    endif()

    message(STATUS "Configuring: generate ${target_name}")

    add_custom_command(
        OUTPUT ${build_dir}/${application_name}.Makefile
        DEPENDS ${SOURCE_DIR}/${slcp_path}
        COMMAND slc generate -p ${SOURCE_DIR}/${slcp_path} ${gsdk_flag} --with ${target_board} -d ${build_dir}
        COMMENT "slc generate -p ${SOURCE_DIR}/${slcp_path} ${gsdk_print_flag} --with ${target_board} -d ${build_dir}"
    )

    add_custom_command(
        OUTPUT ${build_dir}/build/debug/${application_name}.out
        DEPENDS ${build_dir}/${application_name}.Makefile ${SOURCE_DIR}/${application_dir}/*
        COMMAND make -f ${application_name}.Makefile -j BUILD_DIR=${build_dir}/build/
        COMMENT "make -f ${application_name}.Makefile -j BUILD_DIR=${build_dir}/build/"
        WORKING_DIRECTORY ${build_dir}
    )

    add_custom_target(${target_name} ALL
        DEPENDS ${build_dir}/build/debug/${application_name}.out
    )
endfunction()
