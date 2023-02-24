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

    # Create a custom target to install the extension every time we build
    add_custom_target("install_extension" ALL)

    # Copy the extension to the Gecko SDK directory
    add_custom_command(
        TARGET install_extension
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${SOURCE_DIR} ${gsdk_dir}/extension/${repo_folder_name}
        COMMENT "Copying extension ${SOURCE_DIR} to ${gsdk_dir}/extension"
    )

    # Trust the extension using SLC
    add_custom_command(
        TARGET install_extension
        COMMAND slc signature trust -extpath ${gsdk_dir}/extension/${repo_folder_name} --sdk ${gsdk_dir}
        COMMENT "Trusting extension using SLC"
    )

endfunction()
