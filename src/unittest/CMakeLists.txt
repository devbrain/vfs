add_executable(unittest main.cpp)

target_link_libraries(unittest PRIVATE CONAN_PKG::gtest vfs_platform)
target_include_directories(unittest PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

