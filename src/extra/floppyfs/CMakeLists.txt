neutrino_library_type(VFS _vfs_lib_type)

set(src
    ${PROJECT_BINARY_DIR}/include/vfs/extra/floppyfs_api.h
    ${PROJECT_SOURCE_DIR}/include/vfs/extra/floppyfs.hh
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

add_library(floppyfs ${_vfs_lib_type} ${src})
target_link_libraries(floppyfs PUBLIC neutrino::vfs_api bsw::bsw)

target_include_directories(floppyfs
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>
        $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

include(GenerateExportHeader)
generate_export_header(floppyfs
    EXPORT_MACRO_NAME FLOPPYFS_API
    EXPORT_FILE_NAME ${PROJECT_BINARY_DIR}/include/vfs/extra/floppyfs_api.h
)

neutrino_target_warnings(floppyfs)
