add_executable(unittest
        sandbox.cc
        sandbox.hh
        main.cpp
        lru_test.cc
        path_test.cc
        vfs_basic_test.cc
        vfs_unlink_test.cc
        )

target_link_libraries(unittest PRIVATE CONAN_PKG::gtest vfs_api::vfs_api)
target_include_directories(unittest PRIVATE ${PROJECT_ROOT}/src)


add_executable(shell shell.cc)
target_link_libraries(shell PRIVATE CONAN_PKG::gtest vfs_api::vfs_api)
target_include_directories(shell PRIVATE ${PROJECT_ROOT}/src)
