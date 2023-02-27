# Installs this extension by copying it to the Gecko SDK directory, in the extension folder.
# Then run SLC to trust the extension.
#
# Requires
# ========
# SOURCE_DIR: must be set to the repo root
#
# Parameters
# ==========
# gsdk_dir: Override the default `slc` Gecko SDK directory. Only used if the directory exists.
function(install_gsdk_extension gsdk_dir)
    message(STATUS "Configuring: install extension to ${gsdk_dir}")
    
    # Get the name of the repo folder
    get_filename_component(repo_folder_name ${SOURCE_DIR} NAME)

    # Find and track all source files recursively, the CONFIGURE_DEPENDS flag ensures the file changes are checked during build
    file(GLOB_RECURSE absolute_src_files CONFIGURE_DEPENDS ${SOURCE_DIR}/component/* ${SOURCE_DIR}/*.slcc ${SOURCE_DIR}/*.slce )
    foreach(absolute_src_file ${absolute_src_files})
        # Convert from absolute to relative path
        file(RELATIVE_PATH relative_src_file ${SOURCE_DIR} ${absolute_src_file})
        # Add the file to the list of dependencies
        list(APPEND relative_src_files ${relative_src_file})
        list(APPEND output_src_files "${gsdk_dir}/extension/${repo_folder_name}/${relative_src_file}")
    endforeach()
    # Copy the extension to the Gecko SDK directory
    add_custom_command(
        OUTPUT ${output_src_files}
        DEPENDS ${absolute_src_files}
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${SOURCE_DIR} ${gsdk_dir}/extension/${repo_folder_name}
        COMMAND slc signature trust -extpath ${gsdk_dir}/extension/${repo_folder_name} --sdk ${gsdk_dir}
        COMMENT "Installing extension to ${gsdk_dir}"
    )

    add_custom_target(install_gsdk_extension ALL DEPENDS ${output_src_files})


endfunction()
