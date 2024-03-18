if (DEFINED vfs_SHARED_LIBS)
    set(BUILD_SHARED_LIBS "${vfs_SHARED_LIBS}")
endif ()

set (src
        ${CMAKE_BINARY_DIR}/include/vfs/extra/tarfs_api.h
        ${PROJECT_ROOT}/include/vfs/extra/tarfs.hh
        microtar.c
        microtar.h
        tar_stream.hh
        tar_stream.cc
        tarfile.cc
        tarfile.hh
        tarfs_impl.cc
        tarfs_impl.hh
        tarfs_inode.cc
        tarfs_inode.hh
        tarfs_file.cc
        tarfs_file.hh
)

add_library(tarfs ${src})
target_link_libraries(tarfs PUBLIC neutrino::vfs_api bsw)

target_include_directories(tarfs PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${PROJECT_ROOT}/include>
        $<BUILD_INTERFACE:${PROJECT_ROOT}/src>
        $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include>
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR})


include(GenerateExportHeader)
generate_export_header(tarfs
        EXPORT_MACRO_NAME TARFS_API
        EXPORT_FILE_NAME ${CMAKE_BINARY_DIR}/include/vfs/extra/tarfs_api.h
)

add_executable(test_tarfs
        testme.cc
        microtar.c
        microtar.h
        tar_stream.hh
        tar_stream.cc
        tarfile.cc
        tarfile.hh
)
target_compile_definitions(test_tarfs PRIVATE
        _LARGEFILE64_SOURCE
)
target_link_libraries(test_tarfs PRIVATE bsw)
target_include_directories(test_tarfs PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})