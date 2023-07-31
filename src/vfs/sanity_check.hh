//
// Created by igor on 7/26/23.
//

#ifndef SRC_VFS_SANITY_CHECK_HH
#define SRC_VFS_SANITY_CHECK_HH

#include <vfs/api/vfs_api.h>

namespace vfs {
  class sanity_check {
    public:
      static void test(const vfs_api_module* obj);
      static void test(const vfs_logger_module* obj);
      static void test(const vfs_api_filesystem* obj);
      static void test(const vfs_api_dentry* obj);
  };
}
#endif //SRC_VFS_SANITY_CHECK_HH
