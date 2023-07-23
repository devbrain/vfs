set(src
        zipfs.cc
        zipfs.hh
        miniz.c
        miniz.h
        )

add_library(zipfs STATIC ${src})
target_compile_definitions(zipfs PRIVATE __USE_LARGEFILE64 _LARGEFILE64_SOURCE)
target_compile_definitions(zipfs PRIVATE BUILD_VFS_MODULE_AS_STATIC)
target_link_libraries(zipfs PUBLIC vfs_api)

target_include_directories(zipfs
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
        PRIVATE
        "$<BUILD_INTERFACE:${PROJECT_INCLUDE}>"
        "$<BUILD_INTERFACE:${PROJECT_CONFIG_INCLUDE}>")

if (DEFINED vfs_SHARED_LIBS)
    set_property(TARGET zipfs PROPERTY POSITION_INDEPENDENT_CODE ON)
endif()

if (MSVC)
    target_compile_options(zipfs PRIVATE /W4 /WX)
else()
    target_compile_options(zipfs PRIVATE -Wall -Wextra -pedantic -Werror)
endif()
