if (DEFINED vfs_SHARED_LIBS)
    set(BUILD_SHARED_LIBS "${vfs_SHARED_LIBS}")
endif ()

set (lhasa_src
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
        ${CMAKE_BINARY_DIR}/include/vfs/extra/lhafs_api.h
        ${PROJECT_ROOT}/include/vfs/extra/lhafs.hh
        ${lhasa_src}
        input_stream.cc
        input_stream.hh
        lha_archive.cc
        lha_archive.hh
        lhafs.cc
        lhafs.hh
)

add_library(lhafs ${src})
target_link_libraries(lhafs PUBLIC vfs_extra_tools)

target_include_directories(lhafs PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${PROJECT_ROOT}/include>
        $<BUILD_INTERFACE:${PROJECT_ROOT}/src>
        $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include>
        PRIVATE
        ${PROJECT_ROOT}/src
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/lhasa/public
        )



include(GenerateExportHeader)
generate_export_header(lhafs
        EXPORT_MACRO_NAME LHAFS_API
        EXPORT_FILE_NAME ${CMAKE_BINARY_DIR}/include/vfs/extra/lhafs_api.h
)

add_executable(test_lhafs
        test_me.cc
        ${lhasa_src}
        lha_archive.cc
        lha_archive.hh
        input_stream.hh
        input_stream.cc

)
target_compile_definitions(test_lhafs PRIVATE
        _LARGEFILE64_SOURCE
)
target_link_libraries(test_lhafs PRIVATE bsw)
target_include_directories(test_lhafs PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${PROJECT_ROOT}/src)