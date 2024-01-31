add_executable(unittest
        sandbox.cc
        sandbox.hh
        main.cpp
        vfs_basic_test.cc
        vfs_unlink_test.cc
        vfs_open_file_test.cc
        vfs_read_write_test.cc
        vfs_stream_test.cc
        )

target_link_libraries(unittest PRIVATE doctest::doctest vfs_api::vfs_api)
target_include_directories(unittest PRIVATE ${PROJECT_ROOT}/src)



