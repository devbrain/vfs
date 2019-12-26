add_executable(unittest main.cpp lru_test.cc)

target_link_libraries(unittest PRIVATE CONAN_PKG::gtest vfs_api)
target_include_directories(unittest PRIVATE ${PROJECT_ROOT}/src)

