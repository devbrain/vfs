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
        detail/filesystem.hh
        detail/mount_point.hh

        physfs/physfs_inode.hh
        physfs/physfs.hh

        ${CMAKE_BINARY_DIR}/include/vfs/api/vfs_api.h
        ${CMAKE_BINARY_DIR}/include/vfs/vfs_config.h
        ${PROJECT_ROOT}/include/vfs/api/vfs_module.h
        ${PROJECT_ROOT}/include/vfs/api/stdfilesystem.hh
        ${PROJECT_ROOT}/include/vfs/api/system.hh
        ${PROJECT_ROOT}/include/vfs/api/modules.hh
        ${PROJECT_ROOT}/include/vfs/api/mounts.hh
        ${PROJECT_ROOT}/include/vfs/api/exception.hh
        ${PROJECT_ROOT}/include/vfs/api/stats.hh
        ${PROJECT_ROOT}/include/vfs/api/directory_iterator.hh
        ${PROJECT_ROOT}/include/vfs/api/detail/wrapped_iterator.hh
        )


add_library(vfs_api ${src} ${hdr})
add_library(vfs_api::vfs_api ALIAS vfs_api)

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
        Threads::Threads
        bsw::bsw
        )

find_library(DL_LIBRARY dl)
if (DL_LIBRARY)
    target_link_libraries(vfs_api PUBLIC ${DL_LIBRARY})
endif ()


configure_file(vfs_config.h.in ${CMAKE_BINARY_DIR}/include/vfs/vfs_config.h)

target_compile_definitions(vfs_api
        PRIVATE
        ONYX_MODULE_NAME=vfs_api
        $<$<CXX_COMPILER_ID:MSVC>:
        _CRT_SECURE_NO_WARNINGS
        _SCL_SECURE_NO_WARNINGS
        _CRT_SECURE_NO_DEPRECATE
        _CRT_NONSTDC_NO_DEPRECATE
        NOMINMAX >
        )
target_compile_features(vfs_api PUBLIC cxx_std_17)

