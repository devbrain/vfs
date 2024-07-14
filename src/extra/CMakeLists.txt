find_package(Qt6 QUIET COMPONENTS Core)
if (Qt6_FOUND AND is_top_level)
    message("QT6 found")
    add_subdirectory(ui)
endif()

add_subdirectory(extra_tools)
add_subdirectory(popl)
add_subdirectory(zipfs)
add_subdirectory(floppyfs)
add_subdirectory(tarfs)
add_subdirectory(lhafs)




