neutrino_library_type(VFS _vfs_lib_type)

set(src
    ${PROJECT_BINARY_DIR}/include/vfs/extra/zipfs_api.h
    ${PROJECT_SOURCE_DIR}/include/vfs/extra/zipfs.hh
    zipfs_impl.cc
    zipfs_impl.hh
    zip_archive.cc
    zip_archive.hh
    miniz.c
    miniz.h
)

add_library(zipfs ${_vfs_lib_type} ${src})
target_link_libraries(zipfs
    PUBLIC neutrino::vfs_api bsw::bsw
    PRIVATE popl vfs_extra_tools
)

if("${CMAKE_C_BYTE_ORDER}" STREQUAL "LITTLE_ENDIAN")
    set(defMINIZ_LITTLE_ENDIAN 1)
else()
    set(defMINIZ_LITTLE_ENDIAN 0)
endif()

target_compile_definitions(zipfs PRIVATE
    MINIZ_NO_STDIO
    MINIZ_NO_ZLIB_APIS
    MINIZ_NO_TIME
    MINIZ_LITTLE_ENDIAN=${defMINIZ_LITTLE_ENDIAN}
    _LARGEFILE64_SOURCE
)

target_include_directories(zipfs
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>
        $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

include(GenerateExportHeader)
generate_export_header(zipfs
    EXPORT_MACRO_NAME ZIPFS_API
    EXPORT_FILE_NAME ${PROJECT_BINARY_DIR}/include/vfs/extra/zipfs_api.h
)

neutrino_target_warnings(zipfs)

target_compile_features(zipfs PUBLIC cxx_std_17)
set_target_properties(zipfs PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    POSITION_INDEPENDENT_CODE ON
)

# Suppress warnings for third-party C code
set_source_files_properties(miniz.c PROPERTIES COMPILE_OPTIONS "-w")
