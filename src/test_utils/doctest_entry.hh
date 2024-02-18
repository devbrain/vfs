//
// Created by igor on 2/18/24.
//

#ifndef VFS_SRC_TEST_UTILS_DOCTEST_ENTRY_HH_
#define VFS_SRC_TEST_UTILS_DOCTEST_ENTRY_HH_

#include <doctest/doctest.h>
#include "vfs/api/vfs_module.h"

VFS_ALWAYS_EXPORTS void from_dll();   // to silence "-Wmissing-declarations" with GCC
VFS_ALWAYS_EXPORTS void from_dll() {} // force the creation of a .lib file with MSVC

#endif //VFS_SRC_TEST_UTILS_DOCTEST_ENTRY_HH_
