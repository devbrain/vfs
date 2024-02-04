find_package(Qt6 REQUIRED COMPONENTS Core Widgets)
qt_standard_project_setup()

qt_add_executable(vfs_commander
        main.cc
        mainwindow.cc
        mainwindow.hh
)

target_link_libraries(vfs_commander PRIVATE Qt6::Widgets Qt6::Core neutrino::vfs)

if (WIN32)
    set_target_properties(vfs_commander PROPERTIES WIN32_EXECUTABLE ON)
else()
    if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
        set_target_properties(vfs_commander PROPERTIES MACOS_BUNDLE ON)
    endif()
endif()
