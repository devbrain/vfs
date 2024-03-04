find_package(Qt6 COMPONENTS Core)
if (Qt6_FOUND)
    message("QT6 found")
    add_subdirectory(ui)
endif()

add_subdirectory(popl)
add_subdirectory(zipfs)
#add_subdirectory(rarfs)
add_subdirectory(floppyfs)



