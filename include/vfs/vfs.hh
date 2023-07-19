//
// Created by igor on 7/18/23.
//

#ifndef INCLUDE_VFS_VFS_HH
#define INCLUDE_VFS_VFS_HH

#include <filesystem>
#include <string>
#include <optional>
#include <functional>

#include <vfs/export.h>
#include <vfs/api/vfs_api.h>


namespace vfs {
  /**
   * Initializes VFS. Throws exception if something bad happened
   */
  VFS_EXPORT void init();

  /**
   * De-initializes VFS
   */
  VFS_EXPORT void done() noexcept;

  struct VFS_EXPORT auto_init {
      auto_init() {
        init();
      }

      ~auto_init() {
        done();
      }
  };

  using module_t = vfs_api_module*;
  using module_factory_method_t = module_t (*) ();
  /**
   * Registers VFS module
   * @returns false if module with the same name is already registered,
   * can throw exception if something bad happened
   */
  [[nodiscard]] VFS_EXPORT bool register_module(module_factory_method_t factory);
  [[nodiscard]] VFS_EXPORT bool register_module(const std::filesystem::path& path_to_dll);

  struct module_meta_data {
    std::string name;
    std::optional<std::filesystem::path> so_path; // this field is set if the module was loaded from SO
  };

  /**
   * Iterates over registered modules
   * @param iterator user supplied function
   * @return number of modules
   */
  VFS_EXPORT std::size_t enumerate_registered_modules(const std::function<void(const module_meta_data&)>& iterator);
}

#endif //INCLUDE_VFS_VFS_HH
