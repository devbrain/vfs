//
// Created by igor on 7/19/23.
//
#include <utility>
#include <filesystem>
#include <vfs/api/vfs_cxx_api.hh>
#include <sys/stat.h>
#include <ctime>

#ifdef WIN32
#define stat _stat
#endif

#include "physfs.hh"

namespace {
  enum physfs_errors_t {
    NO_MOUNT_POINT = VFS_ERROR_USER + 1,
    UNDERLYING_FS_ERROR
  };

  int
  ext_stat (const std::filesystem::path& path, time_t& ctime, time_t& mtime, vfs::api::file_metadata::type_t& type) {
    struct stat t_stat;
    auto s_pth = path.u8string ();
    if (0 == stat (s_pth.c_str (), &t_stat)) {
      ctime = t_stat.st_ctim.tv_sec;
      mtime = t_stat.st_mtim.tv_sec;
      if (S_ISDIR(t_stat.st_mode)) {
        type = vfs::api::file_object_metadata::type_t::DIRECTORY;
      }
      else if (S_ISREG(t_stat.st_mode)) {
        type = vfs::api::file_object_metadata::type_t::FILE;
      }
      else if (S_ISLNK(t_stat.st_mode)) {
        type = vfs::api::file_object_metadata::type_t::LINK;
      }
      else {
        return 1;
      }
      return 0;
    }
    return -1;
  }

  // ===================================================================
  class physfs_file : public vfs::api::file_metadata {
    public:
      physfs_file (vfs::api::filesystem* owner,
                   std::filesystem::path root,
                   time_t ctime,
                   time_t mtime)
          : vfs::api::file_metadata (owner),
            m_path (std::move (root)),
            m_ctime (ctime),
            m_mtime (mtime) {
      }

      [[nodiscard]] time_stamp_t get_creation_time () const override {
        return m_ctime;
      }

      [[nodiscard]] time_stamp_t get_modification_time () const override {
        return m_mtime;
      }

      [[nodiscard]] uint64_t get_size () const override {
        return std::filesystem::file_size (m_path);
      }

      std::unique_ptr<vfs::api::file_object> open () override {
        return nullptr;
      }

    private:
      std::filesystem::path m_path;
      time_t m_ctime;
      time_t m_mtime;
  };

  // ===================================================================
  class physfs_link : public vfs::api::link_metadata {
    public:
      physfs_link (vfs::api::filesystem* owner,
                   std::filesystem::path root,
                   time_t ctime,
                   time_t mtime)
          : vfs::api::link_metadata (owner),
            m_path (std::move (root)),
            m_ctime (ctime),
            m_mtime (mtime) {

      }

      [[nodiscard]] time_stamp_t get_creation_time () const override {
        return m_ctime;
      }

      [[nodiscard]] time_stamp_t get_modification_time () const override {
        return m_mtime;
      }

      const char* get_target () override {
        m_target = std::filesystem::read_symlink (m_path).u8string ();
        return m_target.c_str ();
      }

    private:
      std::filesystem::path m_path;
      std::string m_target;
      time_t m_ctime;
      time_t m_mtime;
  };

  // ===================================================================
  class physfs_directory : public vfs::api::directory_metadata {
    public:
      physfs_directory (vfs::api::filesystem* owner,
                        std::filesystem::path root,
                        time_t ctime,
                        time_t mtime)
          : vfs::api::directory_metadata (owner),
            m_path (std::move (root)),
            m_ctime (ctime),
            m_mtime (mtime) {
      }

      [[nodiscard]] time_stamp_t get_creation_time () const override {
        return m_ctime;
      }

      [[nodiscard]] time_stamp_t get_modification_time () const override {
        return m_mtime;
      }

      [[nodiscard]] std::unique_ptr<file_object_metadata> load_entry (const std::string& name) override {
        auto p = m_path / name;
        time_t ctime{};
        time_t mtime{};
        vfs::api::file_metadata::type_t type{};
        if (0 == ext_stat (p, ctime, mtime, type)) {
          if (type == vfs::api::file_object_metadata::type_t::FILE) {
            return std::make_unique<physfs_file> (get_filesystem (), p, ctime, mtime);
          }
          if (type == vfs::api::file_object_metadata::type_t::DIRECTORY) {
            return std::make_unique<physfs_directory> (get_filesystem (), p, ctime, mtime);
          }
          else {
            return std::make_unique<physfs_link> (get_filesystem (), p, ctime, mtime);
          }
        }
        else {
          set_error (VFS_ERROR_NO_ENTRY);
          return nullptr;
        }
      }

      void iterate (std::function<void (const std::string& name, const file_object_metadata& md)> itr) override {
        for (auto& entry: std::filesystem::directory_iterator (m_path)) {
          const auto& path = entry.path ();
          time_t ctime{};
          time_t mtime{};
          vfs::api::file_metadata::type_t type{};
          if (0 == ext_stat (path, ctime, mtime, type)) {
            if (type == vfs::api::file_object_metadata::type_t::FILE) {
              physfs_file md (get_filesystem (), path, ctime, mtime);
              itr (path.filename ().u8string (), md);
            }
            if (type == vfs::api::file_object_metadata::type_t::DIRECTORY) {
              physfs_directory md (get_filesystem (), path, ctime, mtime);
              itr (path.filename ().u8string (), md);
            }
            else {
              physfs_link md (get_filesystem (), path, ctime, mtime);
              itr (path.filename ().u8string (), md);
            }
          }
        }
      }

    private:
      std::filesystem::path m_path;
      time_t m_ctime;
      time_t m_mtime;
  };

  // ===================================================================
  class physfs_filesystem : public vfs::api::filesystem {
    public:
      physfs_filesystem (vfs::api::fs_module* owner,
                         std::string root,
                         bool read_only)
          : vfs::api::filesystem (owner),
            m_root (std::move (root)),
            m_read_only (read_only) {
      }

      bool open () {
        if (!std::filesystem::exists (m_root)) {
          set_error (VFS_ERROR_NO_ENTRY);
          return false;
        }
        if (!std::filesystem::is_directory (m_root)) {
          set_error (VFS_ERROR_DIRECTORY_EXPECTED);
          return false;
        }
        VFS_LOG_DEBUG("Opening", m_root);
        return true;
      }

      std::unique_ptr<vfs::api::file_object_metadata> get_root () override {
        time_t ctime{};
        time_t mtime{};
        vfs::api::file_metadata::type_t type{};
        const auto rc = ext_stat (m_root, ctime, mtime, type);
        if (0 == rc) {
          if (type == vfs::api::file_object_metadata::type_t::DIRECTORY) {
            return std::make_unique<physfs_directory> (this, m_root, ctime, mtime);
          }
          else {
            set_error (VFS_ERROR_DIRECTORY_EXPECTED);
            return nullptr;
          }
        }
        else if (rc > 0) {
          set_error (VFS_ERROR_DIRECTORY_EXPECTED);
          return nullptr;
        }
        else {
          set_error (UNDERLYING_FS_ERROR);
          return nullptr;
        }
      }

    private:
      std::string m_root;
      bool m_read_only;
  };

  class physfs : public vfs::api::fs_module {
    public:
      physfs ()
          : vfs::api::fs_module ("physfs") {
        cmd_line.add_switch ("-r", "--read-only")
                .add_positional ("path");
      }

      [[nodiscard]] const char* describe_parameters () const override {
        return cmd_line.describe ().c_str ();
      }

      std::unique_ptr<vfs::api::filesystem> create_filesystem (const std::string& args) override {
        std::string path;
        cmd_line.parse (args);
        if (auto p = cmd_line.get ("path")) {
          path = *p;
        }
        if (path.empty ()) {
          set_error (NO_MOUNT_POINT);
          return nullptr;
        }
        bool read_only = false;
        if (auto p = cmd_line.get ("-r")) {
          read_only = !p->empty ();
        }
        auto fs = std::make_unique<physfs_filesystem> (this, path, read_only);
        if (!fs->open ()) {
          return nullptr;
        }
        return fs;
      }

      const char* error_to_string ([[maybe_unused]] int error_code) override {
        switch (error_code) {
          case NO_MOUNT_POINT:
            return "No mounting point present in arguments";
          case UNDERLYING_FS_ERROR:
            return "Underlying physical FS error";
        }
        return "";
      }

    private:
      vfs::api::cmd_opts::cmd_line cmd_line;
  };

}

VFS_CPP_MODULE_FACTORY(physfs)