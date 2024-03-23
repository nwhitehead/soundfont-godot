cmake_minimum_required( VERSION 3.19 )
project( soundfont-godot )

set( CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH} )

set( CMAKE_CXX_STANDARD 17 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )
set( CMAKE_POSITION_INDEPENDENT_CODE ON )
set( LIB_ARCH ${CMAKE_SYSTEM_PROCESSOR} )

set( GODOT_CPP_VERSION v4.1-binary )
set( GODOT_CPP_LOCATION https://github.com/nwhitehead/godot-cpp/releases/download/${GODOT_CPP_VERSION} )
if( APPLE )
    set( GODOT_CPP_BINARY_URL ${GODOT_CPP_LOCATION}/godot-cpp-macos-universal-release.zip )
else()
    if( WIN32 )
        set( GODOT_CPP_BINARY_URL ${GODOT_CPP_LOCATION}/godot-cpp-windows-msvc2019-x86_64-release.zip )
    else()
        set( GODOT_CPP_BINARY_URL ${GODOT_CPP_LOCATION}/godot-cpp-linux-glibc2.27-x86_64-release.zip )
    endif()
endif()
set( GODOT_CPP_INCLUDE_URL ${GODOT_CPP_LOCATION}/godot-cpp-include.zip )
set( GODOT_CPP_LOCAL_INCLUDE_URL ${GODOT_CPP_LOCATION}/godot-cpp-local-include.zip )

include( FetchContent )
FetchContent_Declare( godot_cpp_binary URL ${GODOT_CPP_BINARY_URL} )
FetchContent_Declare( godot_cpp_include URL ${GODOT_CPP_INCLUDE_URL} )
FetchContent_Declare( godot_cpp_local_include URL ${GODOT_CPP_LOCAL_INCLUDE_URL} )
FetchContent_MakeAvailable( godot_cpp_binary godot_cpp_include godot_cpp_local_include )

project( soundfont )

add_library(${PROJECT_NAME} SHARED)

target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")
target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC "${CMAKE_CURRENT_BINARY_DIR}/_deps/godot_cpp_include-src/gen/include")
target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC "${CMAKE_CURRENT_BINARY_DIR}/_deps/godot_cpp_local_include-src/include")
target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC "${CMAKE_CURRENT_BINARY_DIR}/_deps/godot_cpp_local_include-src/gdextension")

#target_link_libraries(${PROJECT_NAME} PUBLIC godot::cpp) 
if ( "${CMAKE_SYSTEM_NAME}" MATCHES "Windows" )
    set ( LIB_ARCH "AMD64" )
endif()

target_compile_features( ${PROJECT_NAME}
    PRIVATE
        cxx_std_17
)

if (MSVC)
    add_compile_options(/W4 /WX)
else()
    if ( NOT APPLE )
        target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic -Werror -Wno-use-after-free -Werror=suggest-override)
    endif()
endif()

# LIB_ARCH is the architecture being built. It is set to the build system's architecture.
# For macOS, we build a universal library (both arm64 and x86_64).
if ( APPLE )
    set( LIB_ARCH "universal" )
endif()

# LIB_DIR is where the actual library ends up. This is used in both the build directory and the
# install directory and needs to be consistent with the paths in the gdextension file.
#   e.g. linux.release.x86_64 = "lib/Linux-x86_64/libGDExtensionTemplate.so"
set( LIB_DIR "lib/${CMAKE_SYSTEM_NAME}-${LIB_ARCH}" )

message( STATUS "Building ${PROJECT_NAME} for ${LIB_ARCH} on ${CMAKE_SYSTEM_NAME}")

set( BUILD_OUTPUT_DIR "${PROJECT_BINARY_DIR}/${PROJECT_NAME}/" )

set_target_properties( ${PROJECT_NAME}
    PROPERTIES
        CXX_VISIBILITY_PRESET hidden
        VISIBILITY_INLINES_HIDDEN true
        RUNTIME_OUTPUT_DIRECTORY "${BUILD_OUTPUT_DIR}/${LIB_DIR}"
        LIBRARY_OUTPUT_DIRECTORY "${BUILD_OUTPUT_DIR}/${LIB_DIR}"
)

if( NOT DEFINED CMAKE_DEBUG_POSTFIX )
    set_target_properties( ${PROJECT_NAME}
        PROPERTIES
            DEBUG_POSTFIX "-d"
    )
endif()

# Copy over icons
add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/icons"
            ${BUILD_OUTPUT_DIR}/icons
)

# Install into addons of demo project
add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${BUILD_OUTPUT_DIR}"
            "${PROJECT_SOURCE_DIR}/demo/addons/${PROJECT_NAME}"
)

add_subdirectory( src )

# Install library, extension file, and support files in ${CMAKE_INSTALL_PREFIX}/${PROJECT_NAME}
set( INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/${PROJECT_NAME}/" )

add_subdirectory( templates )
