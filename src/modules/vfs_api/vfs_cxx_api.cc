//
// Created by igor on 7/20/23.
//

#include <vfs/api/vfs_cxx_api.hh>

namespace vfs::api {

  fs_module::fs_module (const char* name)
  : m_name(name),
    m_error_module(nullptr),
    m_logger (nullptr) {
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

  void fs_module::set_logger_module(vfs_logger_module* logger_module) {
    m_logger = logger_module;
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

  fs_module* filesystem::get_module () {
    return m_owner;
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

  // =====================================================================
  directory_metadata::directory_metadata(filesystem* owner)
  : file_object_metadata (owner) {

  }

  file_object_metadata::type_t directory_metadata::get_type() const {
    return type_t::DIRECTORY;
  }

  uint64_t directory_metadata::get_size() const {
    return 0;
  }

  const char* directory_metadata::get_target() {
    return nullptr;
  }

  std::unique_ptr<file_object> directory_metadata::open() {
    return nullptr;
  }
  // =======================================================================
  file_metadata::file_metadata(filesystem* owner)
  : file_object_metadata (owner) {

  }

  file_object_metadata::type_t file_metadata::get_type() const {
    return type_t::FILE;
  }

  const char* file_metadata::get_target() {
    return nullptr;
  }

  void file_metadata::iterate([[maybe_unused]] std::function<void(const std::string& name, const file_object_metadata& md)> itr) {

  }

  [[nodiscard]] std::unique_ptr<file_object_metadata> file_metadata::load_entry([[maybe_unused]] const std::string& name) {
    return nullptr;
  }
  // ========================================================================
  link_metadata::link_metadata (filesystem* owner)
  : file_object_metadata (owner) {

  }

  file_object_metadata::type_t link_metadata::get_type () const {
    return type_t::LINK;
  }

  uint64_t link_metadata::get_size () const {
    return 0;
  }

  void link_metadata::iterate ([[maybe_unused]] std::function<void (const std::string& name, const file_object_metadata& md)> itr) {

  }

  [[nodiscard]] std::unique_ptr<file_object_metadata> link_metadata::load_entry([[maybe_unused]] const std::string& name) {
    return nullptr;
  }

  std::unique_ptr<file_object> link_metadata::open () {
    return nullptr;
  }
}

