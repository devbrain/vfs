//
// Created by igor on 7/20/23.
//

#include <vfs/api/vfs_cxx_api.hh>

namespace vfs::api {

  fs_module::fs_module (const char* name)
  : m_name(name),
    m_error_module(nullptr) {
  }

  const char* fs_module::get_name () const {
    return m_name.c_str();
  }

  void fs_module::set_error_module(vfs_error_module* error_module) {
    m_error_module = error_module;
  }

  void fs_module::set_error(int error_code) {
    if (m_error_module) {
      m_error_module->set_error(m_error_module, error_code);
    }
  }

  void fs_module::clear_error() {
    if (m_error_module) {
      m_error_module->clear_error(m_error_module);
    }
  }
  // ========================================================================
  filesystem::filesystem(fs_module* owner)
  : m_owner(owner) {

  }

  void filesystem::set_error(int error_code) {
    m_owner->set_error (error_code);
  }

  void filesystem::clear_error() {
    m_owner->clear_error();
  }

  // ===================================================================
  file_object::file_object (file_object_metadata* metadata)
  : m_metadata (metadata) {
  }

  file_object_metadata* file_object::get_metadata() {
    return m_metadata;
  }

  const file_object_metadata* file_object::get_metadata() const {
    return m_metadata;
  }

  void file_object::set_error(int error_code) {
      m_metadata->set_error (error_code);
  }

  void file_object::clear_error() {
    m_metadata->clear_error();
  }

  // ========================================================================
  file_object_metadata::file_object_metadata (filesystem* owner)
  : m_owner (owner) {

  }
  // ------------------------------------------------------------------------
  const filesystem* file_object_metadata::get_filesystem() const {
    return m_owner;
  }

  filesystem* file_object_metadata::get_filesystem() {
    return m_owner;
  }

  void file_object_metadata::set_error(int error_code) {
    m_owner->set_error (error_code);
  }

  void file_object_metadata::clear_error() {
    m_owner->clear_error();
  }

  file_object_file::file_object_file (file_object_metadata* metadata)
      : file_object (metadata) {
  }
}

