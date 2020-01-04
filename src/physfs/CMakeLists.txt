set (src physfs.cc physfs_inode.cc)
set (hdr physfs_inode.cc)

add_library(vfs_api_physfs ${src} ${hdr})
add_library(vfs_api_physfs::vfs_api_physfs ALIAS vfs_api_physfs)


target_include_directories(vfs_api_physfs
        PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${PROJECT_ROOT}/include>
        $<BUILD_INTERFACE:${PROJECT_ROOT}/src>
        $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include>
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        )

target_compile_options(vfs_api_physfs PRIVATE
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
        -Wall>
        $<$<CXX_COMPILER_ID:MSVC>: "/W4" >)

target_link_libraries(vfs_api_physfs PUBLIC
        Threads::Threads
        std::filesystem
        )

target_compile_definitions(vfs_api_physfs
        PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:
        _CRT_SECURE_NO_WARNINGS
        _SCL_SECURE_NO_WARNINGS
        _CRT_SECURE_NO_DEPRECATE
        _CRT_NONSTDC_NO_DEPRECATE
        NOMINMAX >
        )

set_target_properties(vfs_api_physfs PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
        )

