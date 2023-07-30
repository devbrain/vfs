//
// Created by igor on 7/19/23.
//
#include <utility>
#include <filesystem>
#include <vfs/api/vfs_cxx_api.hh>
#include "physfs.hh"

namespace {
  enum physfs_errors_t {
    NO_MOUNT_POINT = VFS_ERROR_USER + 1
  };

  class physfs_filesystem : public vfs::api::filesystem {
    public:
      physfs_filesystem (vfs::api::fs_module* owner,
                        std::string  root,
                        bool read_only)
          : vfs::api::filesystem (owner),
          m_root(std::move(root)),
          m_read_only(read_only) {
      }

      bool open() {
        if (!std::filesystem::exists (m_root)) {
          set_error (VFS_ERROR_NO_ENTRY);
          return false;
        }
        if (!std::filesystem::is_directory (m_root)) {
          set_error (VFS_ERROR_DIRECTORY_EXPECTED);
          return false;
        }
        VFS_LOG_DEBUG("Opening ", m_root);
        return true;
      }

      std::unique_ptr<vfs::api::file_object_metadata> get_root () override {

        return nullptr;
      }
    private:
      std::string m_root;
      bool        m_read_only;
  };

  class physfs : public vfs::api::fs_module {
    public:
      physfs ()
          : vfs::api::fs_module ("physfs") {
        cmd_line.add_switch("-r", "--read-only")
                .add_positional ("path");
      }

      [[nodiscard]] const char* describe_parameters () const override {
        return cmd_line.describe().c_str();
      }

      std::unique_ptr<vfs::api::filesystem> create_filesystem (const std::string& args) override {
        std::string path;
        cmd_line.parse (args);
        if (auto p = cmd_line.get ("path")) {
          path = *p;
        }
        if (path.empty()) {
          set_error (NO_MOUNT_POINT);
          return nullptr;
        }
        bool read_only = false;
        if (auto p = cmd_line.get ("-r")) {
          read_only = !p->empty();
        }
        auto fs = std::make_unique<physfs_filesystem> (this, path, read_only);
        if (!fs->open()) {
          return nullptr;
        }
        return fs;
      }

      const char* error_to_string([[maybe_unused]] int error_code) override {
        switch (error_code) {
          case NO_MOUNT_POINT:
            return "No mounting point present in arguments";
        }
        return "";
      }
    private:
      vfs::api::cmd_opts::cmd_line cmd_line;
  };

}

VFS_CPP_MODULE_FACTORY(physfs)