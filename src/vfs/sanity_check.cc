//
// Created by igor on 7/26/23.
//

#include <bsw/exception.hh>
#include <bsw/macros.hh>
#include "sanity_check.hh"

#define ASSERT_METHOD(MODULE, OBJ, METHOD) \
  if (!OBJ->METHOD) RAISE_EX(STRINGIZE(MODULE) "::" STRINGIZE(METHOD) " is null")

void vfs::sanity_check::test (const vfs_api_module* obj) {
  if (!obj) {
    RAISE_EX("vfs_api_module is null");
  }
#define ASSERT_API_MODULE(METHOD) ASSERT_METHOD(vfs_api_module, obj, METHOD)
  ASSERT_API_MODULE(destroy);
  ASSERT_API_MODULE(error_to_string);
  ASSERT_API_MODULE(init_error_module);
  ASSERT_API_MODULE(create_filesystem);
  ASSERT_API_MODULE(get_name);
  ASSERT_API_MODULE(describe_parameters);
  ASSERT_API_MODULE(init_logger_module);
#undef ASSERT_API_MODULE
}

void vfs::sanity_check::test(const vfs_logger_module* obj) {
#define ASSERT_LOGGER_MODULE(METHOD) ASSERT_METHOD(vfs_logger_module, obj, METHOD)
  if (!obj) {
    RAISE_EX("vfs_logger_module is null");
  }
  ASSERT_LOGGER_MODULE(debug);
  ASSERT_LOGGER_MODULE(is_debug_enabled);
  ASSERT_LOGGER_MODULE(info);
  ASSERT_LOGGER_MODULE(is_info_enabled);
  ASSERT_LOGGER_MODULE(warn);
  ASSERT_LOGGER_MODULE(is_warn_enabled);
  ASSERT_LOGGER_MODULE(error);
  ASSERT_LOGGER_MODULE(is_error_enabled);
#undef ASSERT_LOGGER_MODULE
}

void vfs::sanity_check::test(const vfs_api_filesystem* obj) {
  if (!obj) {
    RAISE_EX("vfs_api_filesystem is null");
  }
#define ASSERT_VFS_FILESYSTEM(METHOD) ASSERT_METHOD(vfs_api_filesystem, obj, METHOD)
  ASSERT_VFS_FILESYSTEM(destroy);
  ASSERT_VFS_FILESYSTEM(get_root);
  ASSERT_VFS_FILESYSTEM(get_module);
#undef ASSERT_VFS_FILESYSTEM
}

void vfs::sanity_check::test(const vfs_api_dentry* obj) {
  if (!obj) {
    RAISE_EX("vfs_api_dentry is null");
  }
#define ASSERT_VFS_DENTRY(METHOD) ASSERT_METHOD(vfs_api_filesystem, obj, METHOD)
  ASSERT_VFS_DENTRY(destroy);
  ASSERT_VFS_DENTRY(get_type);
  ASSERT_VFS_DENTRY(get_mtime);
  ASSERT_VFS_DENTRY(get_ctime);
  auto type = obj->get_type(const_cast<vfs_api_dentry*>(obj));
  if (type == VFS_API_DIRECTORY) {
    ASSERT_VFS_DENTRY(iterate);
    ASSERT_VFS_DENTRY(load_dentry);
  } else if (type == VFS_API_FILE) {
    ASSERT_VFS_DENTRY(get_size);
  } else {
    ASSERT_VFS_DENTRY(get_target);
  }
#undef ASSERT_VFS_DENTRY
}
