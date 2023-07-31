//
// Created by igor on 7/23/23.
//

#include <bsw/exception.hh>
#include "modules_registry.hh"
#include "error_module.hh"


namespace vfs {
  struct error_info {
    error_info()
    : module_hash{},
      error_code(VFS_ERROR_OK) {
    }

    std::string module_hash;
    int error_code;
  };

  thread_local error_info global_error;

  error_module::error_module(const std::string& module_hash)
  : vfs_error_module {},
    m_module_name(module_hash) {
    opaque = this;
    set_error = _set_error;
    clear_error = _clear_error;
  }

  void error_module::_set_error(struct vfs_error_module* self, int error_code) {
    global_error.module_hash = reinterpret_cast<error_module*>(self)->m_module_name;
    global_error.error_code = error_code;
  }

  void error_module::_clear_error(struct vfs_error_module* self) {
    global_error.module_hash = reinterpret_cast<error_module*>(self)->m_module_name;
    global_error.error_code = VFS_ERROR_OK;
  }

  int error_module::get_last_error() {
    return global_error.error_code;
  }

  std::string error_module::to_string(const modules_registry* registry, int error_code) {
    if (error_code == VFS_ERROR_USER) {
      RAISE_EX("Error code should never be VFS_ERROR_USER");
    }

    if (error_code < VFS_ERROR_USER) {
      switch (error_code) {
        case VFS_ERROR_NO_ENTRY:
          return "Not found";
        case VFS_ERROR_DIRECTORY_EXPECTED:
          return "Directory is expected";
        case VFS_FILE_EXPECTED:
          return "File is expected";
        case VFS_RECURSION_TOO_DEEP:
          return "Recursion is too deep";
        default:
          ENFORCE(false)
      }
    }

    auto fs_module = registry->get (global_error.module_hash);
    if (fs_module) {
      return fs_module->error_to_string(fs_module, error_code);
    }
    return "No module specific error conversion found";
  }
}