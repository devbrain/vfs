if (NOT DEFINED CMAKE_CXX_VISIBILITY_PRESET AND
        NOT DEFINED CMAKE_VISIBILITY_INLINES_HIDDEN)
    set(CMAKE_CXX_VISIBILITY_PRESET hidden)
    set(CMAKE_VISIBILITY_INLINES_HIDDEN YES)
endif ()

if (DEFINED vfs_SHARED_LIBS)
    set(BUILD_SHARED_LIBS "${vfs_SHARED_LIBS}")
endif ()

#set (src
#        ${CMAKE_BINARY_DIR}/include/vfs/extra/floppyfs_api.h
#        ${PROJECT_ROOT}/include/vfs/extra/floppyfs.hh
#        floppyfs_impl.cc
#        floppyfs_impl.hh
#        floppyfs_inode.cc
#        floppyfs_inode.hh
#        zip_archive.cc
#        zip_archive.hh
#        floppyfs_uncompressed_file.cc
#        floppyfs_uncompressed_file.hh
#        floppyfs_inmem_file.cc
#        floppyfs_inmem_file.hh
#        floppyfs_stream_file.cc
#        floppyfs_stream_file.hh
#        miniz.c
#        miniz.h
#)
#
#add_library(floppyfs ${src})
#target_link_libraries(floppyfs PUBLIC neutrino::vfs_api bsw
#                            PRIVATE popl)
#
#if ("${CMAKE_C_BYTE_ORDER}" STREQUAL "LITTLE_ENDIAN")
#    set (defMINIZ_LITTLE_ENDIAN 1)
#else()
#    set (defMINIZ_LITTLE_ENDIAN 0)
#endif()
#
#target_compile_definitions(floppyfs PRIVATE
#        MINIZ_NO_STDIO
#        MINIZ_NO_ZLIB_APIS
#        MINIZ_NO_TIME
#        MINIZ_LITTLE_ENDIAN=${defMINIZ_LITTLE_ENDIAN}
#        _LARGEFILE64_SOURCE
#)
#
#
#target_include_directories(floppyfs PUBLIC
#        $<INSTALL_INTERFACE:include>
#        $<BUILD_INTERFACE:${PROJECT_ROOT}/include>
#        $<BUILD_INTERFACE:${PROJECT_ROOT}/src>
#        $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include>
#        PRIVATE
#        ${CMAKE_CURRENT_SOURCE_DIR})
#
#
#include(GenerateExportHeader)
#generate_export_header(floppyfs
#        EXPORT_MACRO_NAME ZIPFS_API
#        EXPORT_FILE_NAME ${CMAKE_BINARY_DIR}/include/vfs/extra/floppyfs_api.h
#)

add_executable(test_floppy
        testme.cc
        bpb.hh
        bpb.cc
        )
target_compile_definitions(test_floppy PRIVATE
        _LARGEFILE64_SOURCE
)
target_link_libraries(test_floppy PRIVATE bsw)