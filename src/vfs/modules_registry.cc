//
// Created by igor on 7/18/23.
//

#include <bsw/dlfcn.hh>
#include <bsw/macros.hh>
#include <bsw/exception.hh>

#include "error_module.hh"
#include "modules_registry.hh"
#include "sanity_check.hh"


namespace {
  struct dll_guard {
    explicit dll_guard(void* dll)
    : m_dll(dll) {

    }

    ~dll_guard() {
      if (m_dll) {
        dlclose (m_dll);
      }
    }

    void* release() {
      auto temp = m_dll;
      m_dll = nullptr;
      return temp;
    }
    void* m_dll;
  };
}

namespace vfs {

  modules_registry::~modules_registry () {
    for (const auto& [_, module_data] : m_module_map) {
      if (module_data.fs_module && module_data.fs_module->destroy) {
        module_data.fs_module->destroy(module_data.fs_module);
        delete module_data.error_handler;
      }
      if (module_data.dll) {
        dlclose (module_data.dll);
      }
    }
  }

  vfs_api_module* modules_registry::get (const std::string& module_name) const {
    const auto itr = m_module_map.find (module_name);
    if (itr == m_module_map.end()) {
      return nullptr;
    }
    return itr->second.fs_module;
  }

  bool modules_registry::insert (const std::filesystem::path& path_to_dll) {
    std::string so_path = path_to_dll.u8string();
    auto dll = dlopen (so_path.c_str(), RTLD_LAZY);
    if (dll == nullptr) {
      RAISE_EX("Failed to load shared object from ", so_path);
    }
    dll_guard guard(dll);

    module_factory_method_t factory = nullptr;
    *(void **) (&factory) = dlsym (dll, STRINGIZE(VFS_MODULE_FACTORY_NAME));
    if (!factory) {
      RAISE_EX("Can not find symbol ", STRINGIZE(VFS_MODULE_FACTORY_NAME), " in ", so_path);
    }
    auto fs_module = factory();
    sanity_check::test (fs_module);

    std::string name = fs_module->get_name(fs_module);
    if (m_module_map.find (name) != m_module_map.end()) {
      fs_module->destroy(fs_module);
      return false;
    }
    module_info mi;
    _setup_module (mi, fs_module);
    mi.dll = guard.release();
    mi.so_path = path_to_dll;
    m_module_map.insert (std::make_pair(name, mi));
    return true;
  }

  bool modules_registry::insert (module_factory_method_t factory) {
    auto fs_module = factory();
    sanity_check::test (fs_module);
    std::string name = fs_module->get_name(fs_module);
    if (m_module_map.find (name) != m_module_map.end()) {
      fs_module->destroy(fs_module);
      return false;
    }
    module_info mi;
    _setup_module (mi, fs_module);
    m_module_map.insert (std::make_pair(name, mi));
    return true;
  }

  std::size_t modules_registry::enumerate (const std::function<void (const module_meta_data&)>& iterator) const {
    for (const auto& [_, module_data] : m_module_map) {
      iterator(module_data);
    }
    return m_module_map.size();
  }

  void modules_registry::set_logger (vfs_logger_module* logger) {
    m_logger = logger;
  }

  void modules_registry::_setup_module (module_info& mi, vfs_api_module* fs_module) {
    std::string name = fs_module->get_name(fs_module);
    mi.fs_module = fs_module;
    mi.error_handler = new error_module(name);
    mi.fs_module->init_logger_module(fs_module, m_logger);
    mi.fs_module->init_error_module(fs_module, mi.error_handler);
    mi.name = name;
    mi.help = mi.fs_module->describe_parameters(mi.fs_module);
  }
} // vfs