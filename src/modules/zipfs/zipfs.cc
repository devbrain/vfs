//
// Created by igor on 7/19/23.
//
#include <vfs/api/vfs_cpp_api.hh>
#include "zipfs.hh"

namespace {
  struct zipfs_filesystem{
    static const char* describe_parameters() {
      return "Path to phisical directory";
    }

    int init([[maybe_unused]] const char* params) {
      return 0;
    }
  };

  struct zipfs_provider {
    using filesystem_type = zipfs_filesystem;

    static const char* get_name() {
      return "zip";
    }
  };
}


vfs_api_module* create_zipfs() {
  return vfs_api::fs_module_factory<zipfs_provider>();
}
