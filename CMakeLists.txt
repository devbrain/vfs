cmake_minimum_required(VERSION 3.20)

project(vfs
    VERSION 1.0.0
    DESCRIPTION "Virtual File System library"
    LANGUAGES CXX C
)

# =============================================================================
# Neutrino CMake Integration
# =============================================================================

include(FetchContent)

if(NOT DEFINED NEUTRINO_CMAKE_DIR)
    FetchContent_Declare(neutrino_cmake
        GIT_REPOSITORY https://github.com/devbrain/neutrino-cmake.git
        GIT_TAG master
        GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(neutrino_cmake)
    set(NEUTRINO_CMAKE_DIR "${neutrino_cmake_SOURCE_DIR}/cmake")
endif()

list(APPEND CMAKE_MODULE_PATH "${NEUTRINO_CMAKE_DIR}")
include(NeutrinoInit)

# =============================================================================
# Options
# =============================================================================

# VFS modules are designed as plugins - default to shared builds
set(NEUTRINO_VFS_BUILD_SHARED ON CACHE BOOL "Build VFS as shared library")

neutrino_define_library_options(VFS)

# =============================================================================
# Dependencies
# =============================================================================

include(${NEUTRINO_CMAKE_DIR}/deps/bsw.cmake)
neutrino_fetch_bsw()

include(${NEUTRINO_CMAKE_DIR}/deps/failsafe.cmake)
neutrino_fetch_failsafe()

# =============================================================================
# Library
# =============================================================================

set(VFS_TEST_DATA_FILES ${CMAKE_CURRENT_SOURCE_DIR}/test_data)

add_subdirectory(src/vfs)
add_subdirectory(src/extra)

# =============================================================================
# Testing
# =============================================================================

if(NEUTRINO_VFS_BUILD_TESTS)
    include(${NEUTRINO_CMAKE_DIR}/deps/doctest.cmake)
    neutrino_fetch_doctest()

    enable_testing()
    add_subdirectory(unittest)
endif()

# =============================================================================
# Installation
# =============================================================================

if(NEUTRINO_VFS_INSTALL)
    include(NeutrinoInstall)

    install(FILES
        ${PROJECT_BINARY_DIR}/include/vfs/api/vfs_api.h
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/vfs/api
    )

    neutrino_install_library(vfs
        NAMESPACE neutrino::
        COMPATIBILITY SameMajorVersion
        SKIP_EXPORT
        DEPENDENCIES "find_dependency(bsw REQUIRED)" "find_dependency(failsafe REQUIRED)"
    )
endif()

# =============================================================================
# Summary
# =============================================================================

neutrino_is_top_level(_vfs_is_top_level)
if(_vfs_is_top_level)
    neutrino_print_options(VFS)
endif()
