//
// Created by igor on 7/27/23.
//

#ifndef SRC_VFS_FS_TREE_HH
#define SRC_VFS_FS_TREE_HH

#include <map>
#include <stack>
#include <string>
#include <memory>
#include <vfs/api/vfs_api.h>

namespace vfs {
  class fs_tree_node {
    public:
    private:
      fs_tree_node* m_parent;
      std::string   m_name;
      std::map<std::string, fs_tree_node*> m_children;
      std::stack<fs_tree_node*> m_previous_mount;
      bool is_mount_point;
      vfs_api_filesystem* m_filesystem;
      vfs_api_dentry* m_dentry;
  };

  class fs_tree {
    public:
      fs_tree();
      ~fs_tree();
      void set_root(vfs_api_filesystem* fs);
    private:
      std::unique_ptr<fs_tree_node> m_root;
  };
}

#endif //SRC_VFS_FS_TREE_HH
