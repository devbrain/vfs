//
// Created by igor on 7/27/23.
//

#ifndef SRC_VFS_FS_TREE_HH
#define SRC_VFS_FS_TREE_HH

#include <map>
#include <stack>
#include <string>
#include <memory>
#include <vfs/vfs_path.hh>
#include <vfs/api/vfs_api.h>

namespace vfs {
  struct fs_tree_node;



  class fs_tree {
    public:
      fs_tree();
      ~fs_tree();
      void set_root(vfs_api_filesystem* fs);
      vfs_api_dentry* resolve(const path& pth);
    private:
      std::unique_ptr<fs_tree_node> m_root;
  };
}

#endif //SRC_VFS_FS_TREE_HH
