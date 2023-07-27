//
// Created by igor on 7/18/23.
//

#ifndef SRC_VFS_MODULES_REGISTRY_HH
#define SRC_VFS_MODULES_REGISTRY_HH

#include <map>

#include <vfs/vfs.hh>

namespace vfs {
  class error_module;

  struct module_info : public module_meta_data {
    module_info()
    : dll(nullptr),
      fs_module(nullptr) {
    }

    void* dll;
    vfs_api_module* fs_module;
    error_module* error_handler;
  };

  class modules_registry {
    public:
      modules_registry() = default;
      ~modules_registry();

      modules_registry(const modules_registry&) = delete;
      modules_registry& operator = (const modules_registry&) = delete;

      void set_logger(vfs_logger_module* logger);

      [[nodiscard]] vfs_api_module* get(const std::string& module_name) const;
      [[nodiscard]] bool insert(const std::filesystem::path& path_to_dll);
      [[nodiscard]] bool insert(module_factory_method_t factory);
      [[nodiscard]] std::size_t enumerate(const std::function<void(const module_meta_data&)>& iterator) const;
    private:
      void _setup_module(module_info& mi, vfs_api_module* fs_module);
    private:
      std::map<std::string, module_info> m_module_map;
      vfs_logger_module* m_logger;
  };

} // vfs

#endif //SRC_VFS_MODULES_REGISTRY_HH
