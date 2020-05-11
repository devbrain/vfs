cmake_minimum_required(VERSION 3.11)
project(PackageTest CXX)
set (CMAKE_MODULE_PATH "${CMAKE_BINARY_DIR}")
find_package(vfs REQUIRED)

set(BINARY_OUT_DIR ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BINARY_OUT_DIR})

add_executable(example example.cpp)
target_compile_features(example PUBLIC cxx_std_17)
target_link_libraries(example PRIVATE vfs::vfs)
