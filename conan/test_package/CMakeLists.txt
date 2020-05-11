cmake_minimum_required(VERSION 3.11)
project(PackageTest CXX)
set (CMAKE_MODULE_PATH "${CMAKE_BINARY_DIR}")
find_package(vfs REQUIRED)

add_executable(example example.cpp)
target_compile_features(example PUBLIC cxx_std_17)
target_link_libraries(example PRIVATE vfs::vfs)
