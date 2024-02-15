add_custom_target( templates
    SOURCES
        gdextension.in
)

add_dependencies( ${PROJECT_NAME} templates )

set( GD_EXTENSION_FILE_INPUT gdextension.in )

# Workaround to add the "lib" prefix to the library in our template file if using MSYS2.
if ( MINGW )
    set( LIB_PREFIX "lib")
endif()

# Generate our project's .gdextension file from the template
set( GD_EXTENSION_FILE ${PROJECT_NAME}.gdextension )
configure_file( ${GD_EXTENSION_FILE_INPUT} ${PROJECT_BINARY_DIR}/${PROJECT_NAME}/${GD_EXTENSION_FILE} )

# Install the gdextension file from the build directory
install(
    FILES ${BUILD_OUTPUT_DIR}/${GD_EXTENSION_FILE}
    DESTINATION ${INSTALL_DIR}
)
