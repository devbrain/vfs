set (src
        archived_fs.hh
        entry_props.hh
        read_only_fs.hh
)

add_library(vfs_extra_tools INTERFACE ${src})
target_include_directories(vfs_extra_tools
        INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${PROJECT_ROOT}/include
        ${PROJECT_ROOT}/src
        ${CMAKE_BINARY_DIR}/include)
target_link_libraries(vfs_extra_tools INTERFACE neutrino::vfs_api bsw)