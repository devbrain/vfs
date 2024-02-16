find_package(Qt6 REQUIRED COMPONENTS Core Widgets)
qt_standard_project_setup()

qt_add_executable(vfs_commander
        main.cc
        navigation/navigation_model.cc
        navigation/navigation_model.hh
        navigation/navigation_view.cc
        navigation/navigation_view.hh
        navigation/navigation_cursor.hh
        navigation/navigation_panel.hh
        navigation/navigation_panel.cc
        mainwindow.cc
        mainwindow.hh
        mainwindow.ui
        mounts/mounts_dialog.cc
        mounts/mounts_dialog.hh
        mounts/mounts_model.cc
        mounts/mounts_model.hh
        mounts/mountsdialog.ui
        modules/modules_dialog.cc
        modules/modules_dialog.hh
        modules/modules.ui
        modules/modules_model.cc
        modules/modules_model.hh
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
