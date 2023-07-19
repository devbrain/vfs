include(GenerateExportHeader)

# SOURCES ------------------------------------------------------------------------
set (src
        ${PROJECT_INCLUDE}/vfs/api/vfs_api.h
        ${PROJECT_INCLUDE}/vfs/api/vfs_cpp_api.hh
        ${PROJECT_INCLUDE}/vfs/vfs.hh
        vfs.cc
        modules_registry.cc
        modules_registry.hh
        )
# --------------------------------------------------------------------------------
# LIBRARY
# --------------------------------------------------------------------------------

add_library(vfs
        ${src}
        )

if (DEFINED vfs_SHARED_LIBS)
    set(BUILD_SHARED_LIBS "${vfs_SHARED_LIBS}")
endif ()

generate_export_header(vfs EXPORT_FILE_NAME ${PROJECT_CONFIG_INCLUDE}/vfs/export.h)
target_compile_definitions(
        vfs
        PUBLIC "$<$<NOT:$<BOOL:${BUILD_SHARED_LIBS}>>:VFS_STATIC_DEFINE>"
        )
if (MSVC)
    target_compile_options(vfs PRIVATE /W4 /WX)
else()
    target_compile_options(vfs PRIVATE -Wall -Wextra -pedantic -Werror)
endif()

add_library(vfs::vfs ALIAS vfs)
set_target_properties(vfs PROPERTIES
        VERSION ${vfs_VERSION}
        SOVERSION ${vfs_VERSION_MAJOR})

target_include_directories(vfs
        PUBLIC
            "$<BUILD_INTERFACE:${PROJECT_INCLUDE}>"
            "$<BUILD_INTERFACE:${PROJECT_CONFIG_INCLUDE}>"
            "$<INSTALL_INTERFACE:include>"

        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
        )
target_compile_features(vfs PUBLIC cxx_std_17)
target_link_libraries(vfs
        PRIVATE physfs zipfs
        PUBLIC bsw
        )


