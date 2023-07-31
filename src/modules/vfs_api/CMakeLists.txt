set(src
        ${PROJECT_INCLUDE}/vfs/api/vfs_cxx_api.hh
        vfs_cxx_api.cc
        vfs_cxx_bridge.cc
        cmd_opts.cc
        argh.h
        )

add_library(vfs_api STATIC ${src})

target_include_directories(vfs_api
        PUBLIC
        "$<BUILD_INTERFACE:${PROJECT_INCLUDE}>"
        "$<BUILD_INTERFACE:${PROJECT_CONFIG_INCLUDE}>"
        "$<INSTALL_INTERFACE:include>"

        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

if (DEFINED vfs_SHARED_LIBS)
    set_property(TARGET vfs_api PROPERTY POSITION_INDEPENDENT_CODE ON)
endif()

if (MSVC)
    target_compile_options(vfs_api PRIVATE /W4 /WX)
else()
    target_compile_options(vfs_api PRIVATE -Wall -Wextra -pedantic -Werror)
endif()
