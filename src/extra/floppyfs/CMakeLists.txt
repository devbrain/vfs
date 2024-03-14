if (DEFINED vfs_SHARED_LIBS)
    set(BUILD_SHARED_LIBS "${vfs_SHARED_LIBS}")
endif ()

set (src
        ${CMAKE_BINARY_DIR}/include/vfs/extra/floppyfs_api.h
        ${PROJECT_ROOT}/include/vfs/extra/floppyfs.hh
        fat_12.hh
        fat_12.cc
        floppyfs_impl.cc
        floppyfs_impl.hh
        floppyfs_inode.cc
        floppyfs_inode.hh
        floppyfs_file.cc
        floppyfs_file.hh

        fat/bpb.cc
        fat/bpb.hh
        fat/fat.cc
        fat/fat.hh
        fat/directory.cc
        fat/directory.hh
        fat/driver.cc
        fat/driver.hh
)

add_library(floppyfs ${src})
target_link_libraries(floppyfs PUBLIC neutrino::vfs_api bsw)

target_include_directories(floppyfs PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${PROJECT_ROOT}/include>
        $<BUILD_INTERFACE:${PROJECT_ROOT}/src>
        $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include>
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR})


include(GenerateExportHeader)
generate_export_header(floppyfs
        EXPORT_MACRO_NAME FLOPPYFS_API
        EXPORT_FILE_NAME ${CMAKE_BINARY_DIR}/include/vfs/extra/floppyfs_api.h
)

add_executable(test_floppy
        testme.cc
        fat/bpb.cc
        fat/bpb.hh
        fat/fat.cc
        fat/fat.hh
        fat/directory.cc
        fat/directory.hh
        fat/driver.cc
        fat/driver.hh
        )
target_compile_definitions(test_floppy PRIVATE
        _LARGEFILE64_SOURCE
)
target_link_libraries(test_floppy PRIVATE bsw)
target_include_directories(test_floppy PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})