//
// Created by igor on 7/19/23.
//
#include <vfs/api/vfs_cxx_api.hh>
#include "physfs.hh"

namespace {
  class physfs_filesystem : public vfs::api::filesystem {
    public:
      physfs_filesystem (vfs::api::fs_module* owner,
                         [[maybe_unused]] const std::string& args)
          : vfs::api::filesystem (owner) {
      }

      std::unique_ptr<vfs::api::file_object_metadata> get_root () override {
        return nullptr;
      }
  };

  class physfs : public vfs::api::fs_module {
    public:
      physfs ()
          : vfs::api::fs_module ("physfs") {
      }

      [[nodiscard]] const char* describe_parameters () const override {
        return "Path to phisical directory";
      }

      std::unique_ptr<vfs::api::filesystem> create_filesystem (const std::string& args) override {
        return std::make_unique<physfs_filesystem> (this, args);
      }

      const char* error_to_string([[maybe_unused]] int error_code) override {
        return "";
      }
  };

}

VFS_CPP_MODULE_FACTORY(physfs)