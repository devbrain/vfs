cmake_minimum_required (VERSION 3.24)

project(vfs VERSION 1.0.0 LANGUAGES CXX)

string(COMPARE EQUAL "${CMAKE_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}" is_top_level)
option(vfs_INCLUDE_PACKAGING "Include packaging rules for vfs" "${is_top_level}")
option(vfs_ENABLE_TESTING "Enable testing for vfs" TRUE)

set(CMAKE_MODULE_PATH "${CMAKE_BINARY_DIR};${CMAKE_SOURCE_DIR}/cmake")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads)

set(PROJECT_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
set(PROJECT_INCLUDE_PRIVATE ${PROJECT_ROOT}/src)
set(PROJECT_INCLUDE ${PROJECT_ROOT}/include)
set(PROJECT_CONFIG_INCLUDE ${CMAKE_CURRENT_BINARY_DIR}/include)

add_subdirectory(ext)
add_subdirectory(src/api)

if (vfs_ENABLE_TESTING)
    add_subdirectory(unittest)
endif()

if (vfs_INCLUDE_PACKAGING)
    #   add_subdirectory(packaging)
endif ()

option(BUILD_SHARED_LIBS "Build shared library" ON)
option(BUILD_TEST "Build unittests" ON)




