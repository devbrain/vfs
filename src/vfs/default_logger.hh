//
// Created by igor on 7/27/23.
//

#ifndef SRC_VFS_LOGGER_HH
#define SRC_VFS_LOGGER_HH

#include <vfs/api/vfs_cxx_api.hh>

namespace vfs {
  class default_logger : public api::logger {
    private:
      void _debug(const char* module_name, const char* source_file, int line, const char* string) override;
      [[nodiscard]] bool _is_debug_enabled() const override;

      void _info(const char* module_name, const char* source_file, int line, const char* string) override;
      [[nodiscard]] bool _is_info_enabled() const override;

      void _warn(const char* module_name, const char* source_file, int line, const char* string) override;
      [[nodiscard]] bool _is_warn_enabled() const override;

      void _error(const char* module_name, const char* source_file, int line, const char* string) override;
      [[nodiscard]] bool _is_error_enabled() const override;
  };
}

#endif //SRC_VFS_LOGGER_HH
