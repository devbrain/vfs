//
// Created by igor on 7/19/23.
//

#ifndef INCLUDE_VFS_API_VFS_CPP_API_HH
#define INCLUDE_VFS_API_VFS_CPP_API_HH

#include <vfs/api/vfs_api.h>

namespace vfs_api {
  template <class FSProvider>
  class filesystem : public vfs_api_filesystem, public FSProvider {
    public:
      filesystem() : vfs_api_filesystem (), FSProvider () {
        opaque = this;
        init = _init;
        describe_parameters = _describe_parameters;
        destroy = _destroy;
      }
      ~filesystem() = default;
    private:
      static const char* _describe_parameters() {
        return FSProvider::describe_parameters();
      }

      static int _init(void* self, const char* params) {
        auto fs_provider = reinterpret_cast<FSProvider*>(self);
        return fs_provider->init(params);
      }

      static void _destroy(vfs_api_filesystem* self) {
        auto victim = reinterpret_cast<filesystem<FSProvider>*>(self);
        delete victim;
      }
  };

  template <class Provider>
  class fs_module : public vfs_api_module {
    public:
      fs_module() : vfs_api_module() {
        get_name = _get_name;
        destroy = _destroy;
        create_filesystem = _create_filesystem;
      }
      ~fs_module() = default;

    private:
      static const char* _get_name() {
        return Provider::get_name();
      }

      static void _destroy(vfs_api_module* victim) {
        auto self = reinterpret_cast<fs_module<Provider>*>(victim);
        delete self;
      }

      static vfs_api_filesystem* _create_filesystem() {
        return new filesystem<typename Provider::filesystem_type>;
      }
  };
  // ==============================================================================
  template <typename Provider>
  inline
  vfs_api_module* fs_module_factory() {
    return new fs_module<Provider>;
  }
} // vfs_api

#endif //INCLUDE_VFS_API_VFS_CPP_API_HH
