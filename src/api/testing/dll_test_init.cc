//
// Created by igor on 2/1/24.
//
#define DOCTEST_CONFIG_IMPLEMENTATION_IN_DLL
#include <doctest/doctest.h>
#include <vfs/api/vfs_api.h>

VFS_API void from_dll();   // to silence "-Wmissing-declarations" with GCC
VFS_API void from_dll() {} // force the creation of a .lib file with MSVC