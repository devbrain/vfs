neutrino_library_type(VFS _vfs_lib_type)

set(src
    rarsoft/rar.cpp
    rarsoft/strlist.cpp
    rarsoft/strfn.cpp
    rarsoft/pathfn.cpp
    rarsoft/smallfn.cpp
    rarsoft/global.cpp
    rarsoft/file.cpp
    rarsoft/filefn.cpp
    rarsoft/filcreat.cpp
    rarsoft/archive.cpp
    rarsoft/arcread.cpp
    rarsoft/unicode.cpp
    rarsoft/system.cpp
    rarsoft/crypt.cpp
    rarsoft/crc.cpp
    rarsoft/rawread.cpp
    rarsoft/encname.cpp
    rarsoft/resource.cpp
    rarsoft/match.cpp
    rarsoft/timefn.cpp
    rarsoft/rdwrfn.cpp
    rarsoft/consio.cpp
    rarsoft/options.cpp
    rarsoft/errhnd.cpp
    rarsoft/rarvm.cpp
    rarsoft/secpassword.cpp
    rarsoft/rijndael.cpp
    rarsoft/getbits.cpp
    rarsoft/sha1.cpp
    rarsoft/sha256.cpp
    rarsoft/blake2s.cpp
    rarsoft/hash.cpp
    rarsoft/extinfo.cpp
    rarsoft/extract.cpp
    rarsoft/volume.cpp
    rarsoft/list.cpp
    rarsoft/find.cpp
    rarsoft/unpack.cpp
    rarsoft/headers.cpp
    rarsoft/threadpool.cpp
    rarsoft/rs16.cpp
    rarsoft/cmddata.cpp
    rarsoft/ui.cpp
    rarsoft/filestr.cpp
    rarsoft/scantree.cpp
    rarsoft/dll.cpp
    rarsoft/qopen.cpp
    ${PROJECT_BINARY_DIR}/include/vfs/extra/rarfs_api.h
)

add_library(rarfs ${_vfs_lib_type} ${src})
target_link_libraries(rarfs
    PUBLIC neutrino::vfs_api bsw::bsw
    PRIVATE popl
)

target_compile_definitions(rarfs PRIVATE
    _FILE_OFFSET_BITS=64
    _LARGEFILE_SOURCE
    RAR_SMP
    RARDLL
)

target_include_directories(rarfs
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>
        $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

include(GenerateExportHeader)
generate_export_header(rarfs
    EXPORT_MACRO_NAME RARFS_API
    EXPORT_FILE_NAME ${PROJECT_BINARY_DIR}/include/vfs/extra/rarfs_api.h
)

neutrino_target_warnings(rarfs)

# Suppress warnings for third-party C++ code (rarsoft)
file(GLOB _rarsoft_files "${CMAKE_CURRENT_SOURCE_DIR}/rarsoft/*.cpp")
set_source_files_properties(${_rarsoft_files} PROPERTIES COMPILE_OPTIONS "-w")
