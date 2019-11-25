set(header_files ${CMAKE_CURRENT_SOURCE_DIR}/platform/proper_export.hh)

add_library(vfs_platform INTERFACE)
target_sources(vfs_platform INTERFACE "${header_files}")
target_include_directories(vfs_platform INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_features(vfs_platform INTERFACE cxx_std_17)
