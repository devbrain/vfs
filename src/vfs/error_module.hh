//
// Created by igor on 7/23/23.
//

#ifndef SRC_VFS_ERROR_MODULE_HH
#define SRC_VFS_ERROR_MODULE_HH

#include <string>
#include <vfs/api/vfs_api.h>

namespace vfs {
  class modules_registry;

  class error_module : public vfs_error_module {
    public:
      explicit error_module(const std::string& module_name);

      static int get_last_error();
      static std::string to_string(const modules_registry* registry, int error_code);

    private:
      static void _set_error(struct vfs_error_module* self, int error_code);
      static void _clear_error(struct vfs_error_module* self);
    private:
      std::string m_module_name;
  };
}

#endif //SRC_VFS_ERROR_MODULE_HH
