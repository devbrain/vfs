//
// Created by igor on 7/18/23.
//

#include <vfs/vfs.hh>
#include <bsw/exception.hh>
#include <bsw/macros.hh>
#include <physfs.hh>
#include <zipfs.hh>

#include "modules_registry.hh"
#include "error_module.hh"
#include "default_logger.hh"
#include "sanity_check.hh"
#include "path.hh"

namespace vfs {
  struct context {
    modules_registry m_modules_registry;
    default_logger m_logger;
  };

  static context* vfs_ctx = nullptr;
  // ----------------------------------------------------------
  void init() {
    if (!vfs_ctx) {
      vfs_ctx = new context;
      sanity_check::test (&vfs_ctx->m_logger);
      ENFORCE(register_module(create_physfs));
      ENFORCE(register_module(create_zipfs));
    }
  }
  // ----------------------------------------------------------
  void done() noexcept {
      delete vfs_ctx;
  }
  // ----------------------------------------------------------
  bool register_module(module_factory_method_t factory) {
    ENFORCE(vfs_ctx);
    return vfs_ctx->m_modules_registry.insert (factory);
  }
  // ----------------------------------------------------------
  bool register_module(const std::filesystem::path& path_to_dll) {
    ENFORCE(vfs_ctx);
    return vfs_ctx->m_modules_registry.insert (path_to_dll);
  }
  // ----------------------------------------------------------
  std::size_t enumerate_registered_modules(const std::function<void(const module_meta_data&)>& iterator) {
    ENFORCE(vfs_ctx);
    return vfs_ctx->m_modules_registry.enumerate (iterator);
  }
  // ----------------------------------------------------------
  int get_last_error () {
    return error_module::get_last_error();
  }
  // ----------------------------------------------------------
  std::string error_code_to_string (int error_code) {
    ENFORCE(vfs_ctx);
    return error_module::to_string (&vfs_ctx->m_modules_registry, error_code);
  }
  // ----------------------------------------------------------
  static std::vector<std::string> transform_path(const std::string& path) {
    auto npath = path::normpath (path);
    if (npath.empty()) {
      RAISE_EX("Empty path is invalid");
    }
    if (npath[0] != CPPPATH_SEP[0]) {
      RAISE_EX("Only absolute path is accepted");
    }
    return path::split (npath);
  }
  // ----------------------------------------------------------
  void mount([[maybe_unused]] const std::string& path, const std::string& module_name, const std::string& parameters) {
    ENFORCE(vfs_ctx);

    vfs_api_module* api_module = vfs_ctx->m_modules_registry.get (module_name);
    if (!api_module) {
      RAISE_EX("Can not find module ", module_name);
    }
    vfs_api_filesystem* fs = api_module->create_filesystem(api_module, parameters.c_str());
    if (!fs) {
      auto err = get_last_error();
      RAISE_EX("Can not create filesystem with parameters [" + parameters + "], error code ",
               err, " : ", error_code_to_string (err));
    }
    sanity_check::test (fs);

    auto path_components = transform_path (path);
    if (path_components.empty()) {

    }

  }
}
