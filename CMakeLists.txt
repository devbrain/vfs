cmake_minimum_required (VERSION 3.11)

project(vfs)

option(BUILD_SHARED_LIBS "Build shared library" ON)
set (CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
string(COMPARE EQUAL "${CMAKE_CFG_INTDIR}" "." ONYX_IS_SINGLE_CONFIGURATION)

if(NOT EXISTS "${CMAKE_SOURCE_DIR}/cmake/conan/conan.cmake")
  if (NOT EXISTS ${CMAKE_BINARY_DIR}/conan.cmake)
        message(STATUS "Downloading conan.cmake from https://github.com/memsharded/cmake-conan")
        file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/master/conan.cmake"
                "${CMAKE_BINARY_DIR}/conan.cmake"
                SHOW_PROGRESS STATUS status)
        message(STATUS "Download status = ${status}")
  else ()
        message (STATUS "Using existing ${CMAKE_BINARY_DIR}/conan.cmake")
  endif ()
  include(${CMAKE_BINARY_DIR}/conan.cmake)
else()
  include(${CMAKE_SOURCE_DIR}/cmake/conan/conan.cmake)
endif ()

conan_cmake_run(CONANFILE conanfile.txt 
  BASIC_SETUP CMAKE_TARGETS
  BUILD missing)

conan_load_buildinfo ()

conan_basic_setup(NO_OUTPUT_DIRS)

# ===========================================================
include(GenerateExportHeader)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads)

set (PROJECT_ROOT ${CMAKE_SOURCE_DIR})
# ===========================================================
add_subdirectory(unittest)
add_subdirectory(src/api)
add_subdirectory(src/physfs)

