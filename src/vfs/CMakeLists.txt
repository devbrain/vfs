# =============================================================================
# VFS core library
# =============================================================================

neutrino_library_type(VFS _vfs_lib_type)

set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN YES)

# API (header-only)
add_library(vfs_api INTERFACE ${PROJECT_SOURCE_DIR}/include/vfs/api/vfs_module.h)
add_library(neutrino::vfs_api ALIAS vfs_api)
target_include_directories(vfs_api INTERFACE
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

# Source files
set(VFS_SOURCES
    detail/modules_table.cc
    detail/module_loader.cc
    detail/fstab.cc
    detail/dentry.cc
    detail/stats_converter.cc
    detail/path.cc
    detail/filesystem.cc
    detail/mount_point.cc
    detail/io_iface.cc

    physfs/physfs.cc
    physfs/physfs_inode.cc

    system.cc
    modules.cc
    mounts.cc
    directory_iterator.cc
    io.cc
    stream.cc
    archive.cc
)

set(VFS_PRIVATE_HEADERS
    detail/module_loader.hh
    detail/modules_table.hh
    detail/fstab.hh
    detail/dentry.hh
    detail/wrapped_pointer.hh
    detail/stats_converter.hh
    detail/path.hh
    detail/file_system.hh
    detail/mount_point.hh
    detail/io_iface.hh
    system_iface.hh

    physfs/physfs_inode.hh
    physfs/physfs.hh
)

set(VFS_PUBLIC_HEADERS
    ${PROJECT_BINARY_DIR}/include/vfs/api/vfs_api.h
    ${PROJECT_SOURCE_DIR}/include/vfs/system.hh
    ${PROJECT_SOURCE_DIR}/include/vfs/modules.hh
    ${PROJECT_SOURCE_DIR}/include/vfs/mounts.hh
    ${PROJECT_SOURCE_DIR}/include/vfs/exception.hh
    ${PROJECT_SOURCE_DIR}/include/vfs/stats.hh
    ${PROJECT_SOURCE_DIR}/include/vfs/directory_iterator.hh
    ${PROJECT_SOURCE_DIR}/include/vfs/detail/wrapped_iterator.hh
    ${PROJECT_SOURCE_DIR}/include/vfs/io.hh
    ${PROJECT_SOURCE_DIR}/include/vfs/stream.hh
    ${PROJECT_SOURCE_DIR}/include/vfs/archive.hh
)

if(NEUTRINO_VFS_BUILD_TESTS)
    set(VFS_TESTING_SOURCES
        testing/dll_test_init.cc
        testing/test_path.cc
    )
else()
    set(VFS_TESTING_SOURCES)
endif()

# Create library
add_library(vfs ${_vfs_lib_type}
    ${VFS_SOURCES}
    ${VFS_PRIVATE_HEADERS}
    ${VFS_PUBLIC_HEADERS}
    ${VFS_TESTING_SOURCES}
)
add_library(neutrino::vfs ALIAS vfs)

# C++ standard
target_compile_features(vfs PUBLIC cxx_std_17)

# Export header
include(GenerateExportHeader)
generate_export_header(vfs
    EXPORT_MACRO_NAME VFS_API
    EXPORT_FILE_NAME ${PROJECT_BINARY_DIR}/include/vfs/api/vfs_api.h
)

# Include directories
target_include_directories(vfs
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>
        $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# Testing support
if(NEUTRINO_VFS_BUILD_TESTS)
    set(_vfs_testing_lib doctest::doctest)
    target_compile_definitions(vfs PRIVATE VFS_ENABLE_TESTING)
else()
    set(_vfs_testing_lib "")
endif()

# Dependencies
target_link_libraries(vfs
    PUBLIC
        vfs_api
        bsw::bsw
        neutrino::failsafe
        ${CMAKE_DL_LIBS}
    PRIVATE
        ${_vfs_testing_lib}
)

# MSVC definitions
target_compile_definitions(vfs
    PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:
            _CRT_SECURE_NO_WARNINGS
            _SCL_SECURE_NO_WARNINGS
            _CRT_SECURE_NO_DEPRECATE
            _CRT_NONSTDC_NO_DEPRECATE
            NOMINMAX
        >
)

# Warnings
neutrino_target_warnings(vfs)

# Properties
set_target_properties(vfs PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    POSITION_INDEPENDENT_CODE ON
)
