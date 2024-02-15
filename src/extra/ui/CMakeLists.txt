find_package(Qt6 REQUIRED COMPONENTS Core Widgets)
qt_standard_project_setup()

qt_add_executable(vfs_commander
        main.cc
        navigation_model.cc
        navigation_model.hh
        navigation_view.cc
        navigation_view.hh
        navigation_cursor.hh
        navigation_panel.hh
        navigation_panel.cc
        mainwindow.cc
        mainwindow.hh
)

target_link_libraries(vfs_commander PRIVATE Qt6::Widgets Qt6::Core neutrino::vfs)
target_include_directories(vfs_commander PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
if (WIN32)
    set_target_properties(vfs_commander PROPERTIES WIN32_EXECUTABLE ON)
else()
    if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
        set_target_properties(vfs_commander PROPERTIES MACOS_BUNDLE ON)
    endif()
endif()
