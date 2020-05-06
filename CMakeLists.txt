cmake_minimum_required (VERSION 3.11)

project(vfs)

option(BUILD_SHARED_LIBS "Build shared library" ON)
option(BUILD_TEST "Build unittests" ON)

set (CMAKE_MODULE_PATH "${CMAKE_BINARY_DIR};${CMAKE_SOURCE_DIR}/cmake-scripts")
set(CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})
string(COMPARE EQUAL "${CMAKE_CFG_INTDIR}" "." IS_SINGLE_CONFIGURATION)

include("onyx")

onyx_setup_conan(
  REQUIRES
  bsw/0.0.1@devbrain/stable
  gtest/1.8.1@bincrafters/stable
  OPTIONS
  bsw:shared=True
)


# ===========================================================
include(GenerateExportHeader)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads)
# ===========================================================

# Organize unrelated targets to clean IDE hierarchy.
set(DEPENDENCY_FOLDER "Dependencies")

# This will also clean up the CMake ALL_BUILD, INSTALL, RUN_TESTS and ZERO_CHECK projects.
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER ${DEPENDENCY_FOLDER})

# Output binary to predictable location.
set(BINARY_OUT_DIR ${CMAKE_BINARY_DIR}/bin)
set(LIB_OUT_DIR ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BINARY_OUT_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${LIB_OUT_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${LIB_OUT_DIR})


# ===========================================================
set (PROJECT_ROOT ${CMAKE_SOURCE_DIR})
# ===========================================================

#message (STATUS "CMAKE_MODULE_PATH == ${CMAKE_MODULE_PATH}")


find_package(bsw REQUIRED)
#find_package(gtest REQUIRED)

add_subdirectory(src/api)


if (BUILD_TEST)
#    add_subdirectory(unittest)
endif()


