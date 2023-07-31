//
// Created by igor on 7/27/23.
//

#include <bsw/exception.hh>
#include "fs_tree.hh"
#include "sanity_check.hh"

namespace vfs {
  struct fs_tree_node {
      fs_tree_node(fs_tree_node* parent, vfs_api_filesystem* filesystem); // from filesystem mount
      fs_tree_node(fs_tree_node* parent, vfs_api_dentry* dentry); // from filesystem mount

      ~fs_tree_node();

      fs_tree_node* m_parent;
      std::map<std::string, fs_tree_node*> m_children;
      std::stack<fs_tree_node*> m_previous_mount;
      bool m_is_mount_point;
      vfs_api_filesystem* m_filesystem;
      vfs_api_dentry* m_dentry;
  };


  fs_tree_node::fs_tree_node(fs_tree_node* parent, vfs_api_filesystem* filesystem)
  : m_parent(parent),
    m_is_mount_point(true),
    m_filesystem(filesystem),
    m_dentry (filesystem->get_root(filesystem))
    {
      sanity_check::test (m_dentry);
    }

  fs_tree_node::fs_tree_node(fs_tree_node* parent, vfs_api_dentry* dentry)
  : m_parent(parent),
    m_is_mount_point(false),
    m_filesystem(m_parent->m_filesystem),
    m_dentry (dentry) {
    sanity_check::test (m_dentry);
  }

  fs_tree_node::~fs_tree_node() {
    m_dentry->destroy(m_dentry);
    for (auto& [_, child] : m_children) {
      delete child;
    }
    if (m_is_mount_point) {
      m_filesystem->destroy(m_filesystem);
    }
  }

  static void set_error(fs_tree_node* node, int err) {
    ENFORCE(node);
    auto* m = node->m_filesystem->get_module(node->m_filesystem);
    ENFORCE(m);
    auto* errm = m->get_error_module(m);
    if (errm) {
      errm->set_error(errm, err);
    }
  }
  // ===================================================================================
  static fs_tree_node* resolve(fs_tree_node* root, const std::vector<std::string>& path_components, int count) {
    fs_tree_node* node = root;
    for (const auto& name : path_components) {
      auto itr = node->m_children.find (name);
      if (itr != node->m_children.end()) {
        node = itr->second;
      } else {
        ENFORCE(node->m_dentry);
        auto type = node->m_dentry->get_type(node->m_dentry);
        if (type == VFS_API_DIRECTORY) {
          vfs_api_dentry* de = node->m_dentry->load_dentry(node->m_dentry, name.c_str());
          if (!de) {
            set_error (node, VFS_ERROR_NO_ENTRY);
            return nullptr;
          }
          auto* new_node = new fs_tree_node(node, de);
          node->m_children.insert (std::make_pair (name, new_node));
          node = new_node;
        } else if (type == VFS_API_LINK) {
          const char* target = node->m_dentry->get_target(node->m_dentry);
          ENFORCE(target);
          auto link_components = path::normalize (path(target)).split();
          if (count >= 16) {
            set_error (node, VFS_RECURSION_TOO_DEEP);
            return nullptr;
          }
          node = resolve(root, link_components, count+1);
          if (!node) {
            set_error (node, VFS_ERROR_NO_ENTRY);
            return nullptr;
          }
        } else {
          set_error (node, VFS_ERROR_NO_ENTRY);
          return nullptr;
        }
      }
    }
    return node;
  }
  // ===================================================================================
  fs_tree::fs_tree() = default;

  fs_tree::~fs_tree() = default;

  void fs_tree::set_root([[maybe_unused]] vfs_api_filesystem* fs) {
    if (m_root) {
      RAISE_EX("Root filesystem node is already defined");
    }
    m_root = std::make_unique<fs_tree_node>(nullptr, fs);
  }

  vfs_api_dentry* fs_tree::resolve (const path& pth) {
    if (!m_root) {
      return nullptr;
    }
    auto components = path::normalize (pth).split();
    fs_tree_node* node = vfs::resolve(m_root.get(), components, 0);
    if (node) {
      ENFORCE(node->m_dentry);
      return node->m_dentry;
    }
    return nullptr;
  }
}