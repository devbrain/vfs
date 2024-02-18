if (NOT DEFINED CMAKE_CXX_VISIBILITY_PRESET AND
        NOT DEFINED CMAKE_VISIBILITY_INLINES_HIDDEN)
    set(CMAKE_CXX_VISIBILITY_PRESET hidden)
    set(CMAKE_VISIBILITY_INLINES_HIDDEN YES)
endif ()

if (DEFINED vfs_SHARED_LIBS)
    set(BUILD_SHARED_LIBS "${vfs_SHARED_LIBS}")
endif ()

add_library(test_utils
        doctest_entry.hh
        sandbox.cc
        sandbox.hh
        test_data.cc
        test_data.hh
)

target_compile_definitions(test_utils
        PRIVATE VFS_TEST_DATA="${VFS_TEST_DATA_FILES}"
)

target_link_libraries(test_utils neutrino::vfs)

target_include_directories(test_utils
        PUBLIC
            ${CMAKE_BINARY_DIR}/include/test_utils/
            ${CMAKE_CURRENT_SOURCE_DIR}
)

include(GenerateExportHeader)
generate_export_header(test_utils
        EXPORT_MACRO_NAME TEST_API
        EXPORT_FILE_NAME ${CMAKE_BINARY_DIR}/include/test_utils/test_api.h
)
