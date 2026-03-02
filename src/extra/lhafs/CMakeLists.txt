neutrino_library_type(VFS _vfs_lib_type)

set(lhasa_src
    lhasa/crc16.h
    lhasa/lha_file_header.h
    lhasa/lk7_decoder.c
    lhasa/macbinary.c
    lhasa/public/lha_file_header.h
    lhasa/public/lha_decoder.h
    lhasa/public/lha_input_stream.h
    lhasa/public/lhasa.h
    lhasa/public/lha_reader.h
    lhasa/lha_endian.h
    lhasa/lhx_decoder.c
    lhasa/lha_reader.c
    lhasa/lha_basic_reader.h
    lhasa/lha_decoder.h
    lhasa/ext_header.h
    lhasa/lh5_decoder.c
    lhasa/lha_arch.h
    lhasa/lha_file_header.c
    lhasa/lha_basic_reader.c
    lhasa/lha_endian.c
    lhasa/lzs_decoder.c
    lhasa/lha_input_stream.h
    lhasa/ext_header.c
    lhasa/pm1_decoder.c
    lhasa/lha_input_stream.c
    lhasa/lh6_decoder.c
    lhasa/lh7_decoder.c
    lhasa/lh1_decoder.c
    lhasa/crc16.c
    lhasa/null_decoder.c
    lhasa/lha_decoder.c
    lhasa/lha_arch_win32.c
    lhasa/pm2_decoder.c
    lhasa/lha_arch_unix.c
    lhasa/macbinary.h
    lhasa/lz5_decoder.c
)

set(src
    ${PROJECT_BINARY_DIR}/include/vfs/extra/lhafs_api.h
    ${PROJECT_SOURCE_DIR}/include/vfs/extra/lhafs.hh
    ${lhasa_src}
    input_stream.cc
    input_stream.hh
    lha_archive.cc
    lha_archive.hh
    lhafs.cc
    lhafs.hh
)

add_library(lhafs ${_vfs_lib_type} ${src})
target_link_libraries(lhafs
    PUBLIC neutrino::vfs_api bsw::bsw
    PRIVATE vfs_extra_tools
)

target_include_directories(lhafs
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>
        $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/lhasa/public
)

include(GenerateExportHeader)
generate_export_header(lhafs
    EXPORT_MACRO_NAME LHAFS_API
    EXPORT_FILE_NAME ${PROJECT_BINARY_DIR}/include/vfs/extra/lhafs_api.h
)

neutrino_target_warnings(lhafs)

target_compile_features(lhafs PUBLIC cxx_std_17)
set_target_properties(lhafs PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    POSITION_INDEPENDENT_CODE ON
)

# Suppress warnings for third-party C code (lhasa library)
file(GLOB _lhasa_c_files "${CMAKE_CURRENT_SOURCE_DIR}/lhasa/*.c")
set_source_files_properties(${_lhasa_c_files} PROPERTIES COMPILE_OPTIONS "-w")
