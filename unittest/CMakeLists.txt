add_executable(unittest
        sandbox.cc
        sandbox.hh
        main.cpp
        vfs_basic_test.cc
        vfs_unlink_test.cc
        )

target_link_libraries(unittest PRIVATE CONAN_PKG::gtest vfs_api::vfs_api)
target_include_directories(unittest PRIVATE ${PROJECT_ROOT}/src)



