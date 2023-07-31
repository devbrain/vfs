//
// Created by igor on 7/31/23.
//
#include <vfs/api/vfs_cxx_api.hh>

namespace vfs::api::detail {
  metadata_bridge::metadata_bridge (std::unique_ptr<file_object_metadata> impl)
      : vfs_api_dentry(), m_impl (std::move (impl)), m_is_owner (true) {
    _setup (m_impl.get ());
  }

  metadata_bridge::metadata_bridge (file_object_metadata* impl)
      : vfs_api_dentry(), m_impl (impl), m_is_owner (false) {
    _setup (m_impl.get ());
  }

  metadata_bridge::~metadata_bridge () {
    if (!m_is_owner) {
      (void) m_impl.release ();
    }
  }

  void metadata_bridge::_setup (const file_object_metadata* impl) {
    opaque = this;
    destroy = _destroy;
    get_type = _get_type;
    get_ctime = _get_ctime;
    get_mtime = _get_mtime;
    if (dynamic_cast<const directory_metadata*>(impl)) {
      iterate = _iterate;
      load_dentry = _load_dentry;
      get_size = nullptr;
      get_target = nullptr;
    }
    else if (dynamic_cast<const file_metadata*>(impl)) {
      iterate = nullptr;
      get_size = _get_size;
      get_target = nullptr;
    }
    else {
      iterate = nullptr;
      get_size = nullptr;
      get_target = _get_target;
    }
  }

  vfs_api_dentry_type metadata_bridge::_get_type (struct vfs_api_dentry* self) {
    auto my_type = reinterpret_cast<metadata_bridge*>(self)->m_impl->get_type ();
    switch (my_type) {
      case file_object_metadata::type_t::DIRECTORY:
        return VFS_API_DIRECTORY;
      case file_object_metadata::type_t::FILE:
        return VFS_API_FILE;
      case file_object_metadata::type_t::LINK:
        return VFS_API_LINK;
    }
    return VFS_API_LINK; // Should not be here
  }

  time_t metadata_bridge::_get_ctime (struct vfs_api_dentry* self) {
    return reinterpret_cast<metadata_bridge*>(self)->m_impl->get_creation_time ();
  }

  time_t metadata_bridge::_get_mtime (struct vfs_api_dentry* self) {
    return reinterpret_cast<metadata_bridge*>(self)->m_impl->get_modification_time ();
  }

  uint64_t metadata_bridge::_get_size (struct vfs_api_dentry* self) {
    return reinterpret_cast<metadata_bridge*>(self)->m_impl->get_size ();
  }

  const char* metadata_bridge::_get_target (struct vfs_api_dentry* self) {
    return reinterpret_cast<metadata_bridge*>(self)->m_impl->get_target ();
  }

  void metadata_bridge::_iterate (struct vfs_api_dentry* self, void* context, vfs_api_directory_iterator_t iterator) {
    reinterpret_cast<metadata_bridge*>(self)->m_impl->iterate ([context, iterator] (const std::string& name,
                                                                                    const file_object_metadata& md) {
      metadata_bridge d (const_cast<file_object_metadata*>(&md));
      iterator (context, name.c_str (), &d);
    });
  }

  vfs_api_dentry* metadata_bridge::_load_dentry (struct vfs_api_dentry* self, const char* name) {
    auto e = reinterpret_cast<metadata_bridge*>(self)->m_impl->load_entry (name);
    if (e) {
      return new metadata_bridge (std::move (e));
    }
    return nullptr;
  }

  void metadata_bridge::_destroy (vfs_api_dentry* self) {
    delete reinterpret_cast<metadata_bridge*>(self);
  }

  // ============================================================================================
  filesystem_bridge::filesystem_bridge (std::unique_ptr<filesystem> impl)
      : vfs_api_filesystem{},
        m_impl (std::move (impl)) {
    destroy = _destroy;
    get_root = _get_root;
    get_module = _get_module;
  }

  void filesystem_bridge::_destroy (vfs_api_filesystem* self) {
    delete reinterpret_cast<filesystem_bridge*>(self);
  }

  vfs_api_dentry* filesystem_bridge::_get_root (struct vfs_api_filesystem* self) {
    auto root = reinterpret_cast<filesystem_bridge*>(self)->m_impl->get_root ();
    if (!root) {
      return nullptr;
    }
    return new metadata_bridge (std::move (root));
  }

  vfs_api_module* filesystem_bridge::_get_module(struct vfs_api_filesystem* self) {
    return reinterpret_cast<filesystem_bridge*>(self)->m_impl->get_module()->m_module;
  }
  // ===================================================================================
  fs_module_bridge::fs_module_bridge (std::unique_ptr<fs_module> impl)
      : vfs_api_module{},
        m_pimpl (std::move (impl)) {
    opaque = this;
    describe_parameters = _describe_parameters;
    get_name = _get_name;
    destroy = _destructor;
    create_filesystem = _create_fs;
    init_error_module = _init_error_module;
    error_to_string = _error_to_string;
    init_logger_module = _init_logger_module;
    get_error_module = _get_error_module;
    m_pimpl->m_module = this;
  }

  const char* fs_module_bridge::_describe_parameters (struct vfs_api_module* self) {
    return reinterpret_cast<fs_module_bridge*>(self)->m_pimpl->describe_parameters ();
  }

  const char* fs_module_bridge::_get_name (struct vfs_api_module* self) {
    return reinterpret_cast<fs_module_bridge*>(self)->m_pimpl->get_name ();
  }

  void fs_module_bridge::_destructor (struct vfs_api_module* self) {
    delete reinterpret_cast<fs_module_bridge*>(self);
  }

  struct vfs_api_filesystem* fs_module_bridge::_create_fs (struct vfs_api_module* self, const char* params) {
    auto fs_ptr = reinterpret_cast<fs_module_bridge*>(self)->m_pimpl->create_filesystem (params);
    if (!fs_ptr) {
      return nullptr;
    }

    return new filesystem_bridge (std::move (fs_ptr));
  }

  void fs_module_bridge::_init_error_module (struct vfs_api_module* self, struct vfs_error_module* error_module) {
    reinterpret_cast<fs_module_bridge*>(self)->m_pimpl->set_error_module (error_module);
  }

  void fs_module_bridge::_init_logger_module (struct vfs_api_module* self, struct vfs_logger_module* logger_module) {
    reinterpret_cast<fs_module_bridge*>(self)->m_pimpl->set_logger_module (logger_module);
  }

  const char* fs_module_bridge::_error_to_string (struct vfs_api_module* self, int error_code) {
    return reinterpret_cast<fs_module_bridge*>(self)->m_pimpl->error_to_string (error_code);
  }

  vfs_error_module* fs_module_bridge::_get_error_module(struct vfs_api_module* self) {
    return reinterpret_cast<fs_module_bridge*>(self)->m_pimpl->m_error_module;
  }
}