if (NOT DEFINED CMAKE_CXX_VISIBILITY_PRESET AND
        NOT DEFINED CMAKE_VISIBILITY_INLINES_HIDDEN)
    set(CMAKE_CXX_VISIBILITY_PRESET hidden)
    set(CMAKE_VISIBILITY_INLINES_HIDDEN YES)
endif ()

if (DEFINED vfs_SHARED_LIBS)
    set(BUILD_SHARED_LIBS "${assets_SHARED_LIBS}")
endif ()

add_library(vfs_api INTERFACE ${PROJECT_ROOT}/include/vfs/api/vfs_module.h)
add_library(neutrino::vfs_api ALIAS vfs_api)
target_include_directories(vfs_api INTERFACE ${PROJECT_ROOT}/include/vfs/api/)

set(src
        detail/modules_table.cc
        detail/module_loader.cc
        detail/fstab.cc
        detail/dentry.cc
        detail/stats_converter.cc
        detail/path.cc
        detail/filesystem.cc
        detail/mount_point.cc

        physfs/physfs.cc
        physfs/physfs_inode.cc

        system.cc
        modules.cc
        mounts.cc
        directory_iterator.cc
        detail/module_loader.hh
        detail/modules_table.hh
        detail/fstab.hh
        detail/dentry.hh
        detail/wrapped_pointer.hh
        detail/stats_converter.hh
        detail/path.hh
        detail/file_system.hh
        detail/mount_point.hh

        physfs/physfs_inode.hh
        physfs/physfs.hh

        ${CMAKE_BINARY_DIR}/include/vfs/api/vfs_api.h
        ${PROJECT_ROOT}/include/vfs/system.hh
        ${PROJECT_ROOT}/include/vfs/modules.hh
        ${PROJECT_ROOT}/include/vfs/mounts.hh
        ${PROJECT_ROOT}/include/vfs/exception.hh
        ${PROJECT_ROOT}/include/vfs/stats.hh
        ${PROJECT_ROOT}/include/vfs/directory_iterator.hh
        ${PROJECT_ROOT}/include/vfs/detail/wrapped_iterator.hh
        ${PROJECT_ROOT}/include/vfs/io.hh
        io.cc
        ${PROJECT_ROOT}/include/vfs/stream.hh
        stream.cc

        ${PROJECT_ROOT}/include/vfs/archive.hh
        archive.cc
        system_iface.hh
        detail/io_iface.cc
        detail/io_iface.hh
        )

if (vfs_ENABLE_TESTING)
    set(testing_src
            testing/dll_test_init.cc
            testing/test_path.cc
    )
else()
    set(testing_src
    )
endif()

add_library(vfs ${src} ${testing_src})
add_library(neutrino::vfs ALIAS vfs)

include(GenerateExportHeader)
generate_export_header(vfs
        EXPORT_MACRO_NAME VFS_API
        EXPORT_FILE_NAME ${CMAKE_BINARY_DIR}/include/vfs/api/vfs_api.h
        )

target_include_directories(vfs
        PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${PROJECT_ROOT}/include>
        $<BUILD_INTERFACE:${PROJECT_ROOT}/src>
        $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include>
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        )

target_compile_options(vfs PRIVATE
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
        -Wall -pedantic -ansi -std=c++17 -fvisibility=hidden -fvisibility-inlines-hidden >
        $<$<CXX_COMPILER_ID:MSVC>: "/W4" >)

if (vfs_ENABLE_TESTING)
    set (vfs_TESTING_LIB doctest::doctest)
    target_compile_definitions(vfs PRIVATE VFS_ENABLE_TESTING)
else()
    set (vfs_TESTING_LIB "")
endif()

target_link_libraries(vfs PUBLIC vfs_api bsw::bsw ${vfs_TESTING_LIB} ${CMAKE_DL_LIBS})






target_compile_definitions(vfs
        PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:
        _CRT_SECURE_NO_WARNINGS
        _SCL_SECURE_NO_WARNINGS
        _CRT_SECURE_NO_DEPRECATE
        _CRT_NONSTDC_NO_DEPRECATE
        NOMINMAX >
        )

target_compile_features(vfs PUBLIC cxx_std_17)

