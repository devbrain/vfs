neutrino_library_type(VFS _vfs_lib_type)

set(src
    ${PROJECT_BINARY_DIR}/include/vfs/extra/tarfs_api.h
    ${PROJECT_SOURCE_DIR}/include/vfs/extra/tarfs.hh
    microtar.c
    microtar.h
    tar_stream.hh
    tar_stream.cc
    tar_archive.cc
    tar_archive.hh
    tarfs_impl.cc
    tarfs_impl.hh
)

add_library(tarfs ${_vfs_lib_type} ${src})
target_link_libraries(tarfs
    PUBLIC neutrino::vfs_api bsw::bsw
    PRIVATE vfs_extra_tools
)

target_include_directories(tarfs
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>
        $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

include(GenerateExportHeader)
generate_export_header(tarfs
    EXPORT_MACRO_NAME TARFS_API
    EXPORT_FILE_NAME ${PROJECT_BINARY_DIR}/include/vfs/extra/tarfs_api.h
)

neutrino_target_warnings(tarfs)

target_compile_features(tarfs PUBLIC cxx_std_17)
set_target_properties(tarfs PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    POSITION_INDEPENDENT_CODE ON
)

# Suppress warnings for third-party C code
set_source_files_properties(microtar.c PROPERTIES COMPILE_OPTIONS "-w")
