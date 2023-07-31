set(src
        physfs.cc
        physfs.hh
)

add_library(physfs STATIC ${src})
target_compile_definitions(physfs PRIVATE BUILD_VFS_MODULE_AS_STATIC)
target_link_libraries(physfs PUBLIC vfs_api)

target_include_directories(physfs
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
        PRIVATE
            "$<BUILD_INTERFACE:${PROJECT_INCLUDE}>"
            "$<BUILD_INTERFACE:${PROJECT_CONFIG_INCLUDE}>")

#if (DEFINED vfs_SHARED_LIBS)
    set_property(TARGET physfs PROPERTY POSITION_INDEPENDENT_CODE ON)
#endif()

if (MSVC)
    target_compile_options(physfs PRIVATE /W4 /WX)
else()
    target_compile_options(physfs PRIVATE -Wall -Wextra -pedantic -Werror)
endif()
