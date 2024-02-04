if (NOT DEFINED CMAKE_CXX_VISIBILITY_PRESET AND
        NOT DEFINED CMAKE_VISIBILITY_INLINES_HIDDEN)
    set(CMAKE_CXX_VISIBILITY_PRESET hidden)
    set(CMAKE_VISIBILITY_INLINES_HIDDEN YES)
endif ()

if (DEFINED vfs_SHARED_LIBS)
    set(BUILD_SHARED_LIBS "${vfs_SHARED_LIBS}")
endif ()

set (src
        ${CMAKE_BINARY_DIR}/include/vfs/extra/zipfs_api.h
        ${PROJECT_ROOT}/include/vfs/extra/zipfs.hh
        zipfs_impl.cc
        zipfs_impl.hh
        miniz.c
        miniz.h
)

add_library(zipfs ${src})
target_link_libraries(zipfs PUBLIC neutrino::vfs_api bsw)
target_include_directories(zipfs PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${PROJECT_ROOT}/include>
        $<BUILD_INTERFACE:${PROJECT_ROOT}/src>
        $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include>
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR})

target_compile_definitions(zipfs PRIVATE
        _LARGEFILE64_SOURCE
        )

include(GenerateExportHeader)
generate_export_header(zipfs
        EXPORT_MACRO_NAME ZIPFS_API
        EXPORT_FILE_NAME ${CMAKE_BINARY_DIR}/include/vfs/extra/zipfs_api.h
)