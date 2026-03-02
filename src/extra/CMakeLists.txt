neutrino_is_top_level(_vfs_is_top_level)

find_package(Qt6 QUIET COMPONENTS Core)
if(Qt6_FOUND AND _vfs_is_top_level)
    message(STATUS "QT6 found")
    add_subdirectory(ui)
endif()

add_subdirectory(extra_tools)
add_subdirectory(popl)
add_subdirectory(zipfs)
add_subdirectory(floppyfs)
add_subdirectory(tarfs)
add_subdirectory(lhafs)




