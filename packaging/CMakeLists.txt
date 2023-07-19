include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

if (NOT DEFINED vfs_INSTALL_CMAKEDIR)
    set(vfs_INSTALL_CMAKEDIR "${CMAKE_INSTALL_LIBDIR}/cmake/vfs" CACHE STRING "Path to vfs CMake files")
endif ()

install(TARGETS vfs EXPORT vfs_Targets
        RUNTIME COMPONENT vfs_Runtime
        LIBRARY COMPONENT vfs_Runtime
        NAMELINK_COMPONENT vfs_Development
        ARCHIVE COMPONENT vfs_Development
        INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")

install(DIRECTORY
            "${vfs_SOURCE_DIR}/include/"
            "${vfs_BINARY_DIR}/src/include/"
        TYPE INCLUDE
        COMPONENT vfs_Development)

if (BUILD_SHARED_LIBS)
    set(type shared)
else ()
    set(type static)
endif ()

install(EXPORT vfs_Targets
        DESTINATION "${vfs_INSTALL_CMAKEDIR}"
        NAMESPACE vfs::
        FILE vfs-${type}-targets.cmake
        COMPONENT vfs_Development)

write_basic_package_version_file(
    vfsConfigVersion.cmake
    COMPATIBILITY SameMajorVersion)

install(FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/vfsConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/vfsConfigVersion.cmake"
        DESTINATION "${vfs_INSTALL_CMAKEDIR}"
        COMPONENT vfs_Development)

