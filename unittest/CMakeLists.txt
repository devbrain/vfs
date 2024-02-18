add_executable(unittest
        main.cpp
        vfs_basic_test.cc
        vfs_unlink_test.cc
        vfs_open_file_test.cc
        vfs_read_write_test.cc
        vfs_stream_test.cc
        zipfs_test.cc
        )
target_compile_definitions(unittest
        PRIVATE VFS_TEST_DATA="${VFS_TEST_DATA_FILES}"
)
target_link_libraries(unittest PRIVATE doctest::doctest neutrino::vfs test_utils zipfs)
target_include_directories(unittest PRIVATE ${PROJECT_ROOT}/src)



