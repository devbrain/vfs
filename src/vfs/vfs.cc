//
// Created by igor on 7/18/23.
//

#include <vfs/vfs.hh>
#include <bsw/exception.hh>
#include <physfs.hh>
#include <zipfs.hh>
#include "modules_registry.hh"

namespace vfs {
  struct context {
    modules_registry m_modules_registry;
  };

  static context* vfs_ctx = nullptr;
  // ----------------------------------------------------------
  void init() {
    if (!vfs_ctx) {
      vfs_ctx = new context;
      ENFORCE(vfs_ctx->m_modules_registry.insert (create_physfs));
      ENFORCE(vfs_ctx->m_modules_registry.insert (create_zipfs));
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
}
