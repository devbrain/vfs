if (NOT DEFINED CMAKE_CXX_VISIBILITY_PRESET AND
        NOT DEFINED CMAKE_VISIBILITY_INLINES_HIDDEN)
    set(CMAKE_CXX_VISIBILITY_PRESET hidden)
    set(CMAKE_VISIBILITY_INLINES_HIDDEN YES)
endif ()

if (DEFINED vfs_SHARED_LIBS)
    set(BUILD_SHARED_LIBS "${vfs_SHARED_LIBS}")
endif ()

set (src
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
        ${CMAKE_BINARY_DIR}/include/vfs/extra/rarfs_api.h
#        ${PROJECT_ROOT}/include/vfs/extra/rarfs.hh

)

add_library(rarfs ${src})
target_link_libraries(rarfs PUBLIC neutrino::vfs_api bsw
                            PRIVATE popl)



target_compile_definitions(rarfs PRIVATE
        _FILE_OFFSET_BITS=64
        _LARGEFILE_SOURCE
        RAR_SMP
        RARDLL
)


target_include_directories(rarfs PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${PROJECT_ROOT}/include>
        $<BUILD_INTERFACE:${PROJECT_ROOT}/src>
        $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include>
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR})


include(GenerateExportHeader)
generate_export_header(rarfs
        EXPORT_MACRO_NAME ZIPFS_API
        EXPORT_FILE_NAME ${CMAKE_BINARY_DIR}/include/vfs/extra/rarfs_api.h
)

