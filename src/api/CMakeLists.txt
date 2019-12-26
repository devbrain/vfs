find_package(Filesystem REQUIRED Experimental Final)

set (src
        detail/spooky.cc
        hasher.cc)

set(hdr
        detail/lru.hh
        detail/spooky.hh
        ${CMAKE_BINARY_DIR}/include/vfs/api/vfs_api.h
        hasher.hh
        ${PROJECT_ROOT}/include/vfs/api/vfs_module.h ../physfs/physfs.cc detail/filesystem.hh detail/module_loader.hh detail/stdfilesystem.hh detail/module_loader.cc)




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
        -Wall>
        $<$<CXX_COMPILER_ID:MSVC>: "/W4" >)

target_link_libraries(vfs_api PUBLIC
            Threads::Threads
            std::filesystem
        )
configure_file(vfs_config.h.in  ${CMAKE_BINARY_DIR}/include/vfs/vfs_config.h)

target_compile_definitions(vfs_api
                PRIVATE
                $<$<CXX_COMPILER_ID:MSVC>:
                        _CRT_SECURE_NO_WARNINGS
                        _SCL_SECURE_NO_WARNINGS
                        _CRT_SECURE_NO_DEPRECATE
                        _CRT_NONSTDC_NO_DEPRECATE
                        NOMINMAX >
        )

set_target_properties(vfs_api PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
