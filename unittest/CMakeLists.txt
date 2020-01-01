add_executable(unittest main.cpp lru_test.cc path_test.cc)
target_link_libraries(unittest PRIVATE CONAN_PKG::gtest vfs_api::vfs_api)
target_include_directories(unittest PRIVATE ${PROJECT_ROOT}/src)


add_executable(shell shell.cc)
target_link_libraries(shell PRIVATE CONAN_PKG::gtest vfs_api::vfs_api)
target_include_directories(shell PRIVATE ${PROJECT_ROOT}/src)
