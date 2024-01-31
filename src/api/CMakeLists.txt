if (NOT DEFINED CMAKE_CXX_VISIBILITY_PRESET AND
        NOT DEFINED CMAKE_VISIBILITY_INLINES_HIDDEN)
    set(CMAKE_CXX_VISIBILITY_PRESET hidden)
    set(CMAKE_VISIBILITY_INLINES_HIDDEN YES)
endif ()

if (DEFINED vfs_SHARED_LIBS)
    set(BUILD_SHARED_LIBS "${assets_SHARED_LIBS}")
endif ()

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
        )

set(hdr
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
        ${PROJECT_ROOT}/include/vfs/api/vfs_module.h
        ${PROJECT_ROOT}/include/vfs/system.hh
        ${PROJECT_ROOT}/include/vfs/modules.hh
        ${PROJECT_ROOT}/include/vfs/mounts.hh
        ${PROJECT_ROOT}/include/vfs/api/exception.hh
        ${PROJECT_ROOT}/include/vfs/stats.hh
        ${PROJECT_ROOT}/include/vfs/directory_iterator.hh
        ${PROJECT_ROOT}/include/vfs/api/detail/wrapped_iterator.hh
        ${PROJECT_ROOT}/include/vfs/io.hh
        ${PROJECT_ROOT}/include/vfs/stream.hh
        stream.cc
        )


add_library(vfs_api ${src} ${hdr})
add_library(vfs_api::vfs_api ALIAS vfs_api)

include(GenerateExportHeader)
generate_export_header(vfs_api
        EXPORT_MACRO_NAME VFS_API
        EXPORT_FILE_NAME ${CMAKE_BINARY_DIR}/include/vfs/api/vfs_api.h
        )

target_include_directories(vfs_api
        PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${PROJECT_ROOT}/include>
        $<BUILD_INTERFACE:${PROJECT_ROOT}/src>
        $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include>
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        )

target_compile_options(vfs_api PRIVATE
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
        -Wall -fvisibility=hidden -fvisibility-inlines-hidden >
        $<$<CXX_COMPILER_ID:MSVC>: "/W4" >)


target_link_libraries(vfs_api PUBLIC
        bsw::bsw
        )

find_library(DL_LIBRARY dl)
if (DL_LIBRARY)
    target_link_libraries(vfs_api PUBLIC ${DL_LIBRARY})
endif ()


target_compile_definitions(vfs_api
        PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:
        _CRT_SECURE_NO_WARNINGS
        _SCL_SECURE_NO_WARNINGS
        _CRT_SECURE_NO_DEPRECATE
        _CRT_NONSTDC_NO_DEPRECATE
        NOMINMAX >
        )
target_compile_features(vfs_api PUBLIC cxx_std_17)

