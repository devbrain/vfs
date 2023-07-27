//
// Created by igor on 7/26/23.
//

#include <bsw/exception.hh>
#include <bsw/macros.hh>
#include "sanity_check.hh"

#define ASSERT_METHOD(MODULE, OBJ, METHOD) \
  if (!OBJ->METHOD) RAISE_EX(STRINGIZE(MODULE) "::" STRINGIZE(METHOD) " is null")

void vfs::sanity_check::test (const vfs_api_module* obj) {
  if (!obj) {
    RAISE_EX("vfs_api_module is null");
  }
#define ASSERT_API_MODULE(METHOD) ASSERT_METHOD(vfs_api_module, obj, METHOD)
  ASSERT_API_MODULE(destroy);
  ASSERT_API_MODULE(error_to_string);
  ASSERT_API_MODULE(init_error_module);
  ASSERT_API_MODULE(create_filesystem);
  ASSERT_API_MODULE(get_name);
  ASSERT_API_MODULE(describe_parameters);
  ASSERT_API_MODULE(init_logger_module);
#undef ASSERT_API_MODULE
}

void vfs::sanity_check::test(const vfs_logger_module* obj) {
#define ASSERT_LOGGER_MODULE(METHOD) ASSERT_METHOD(vfs_logger_module, obj, METHOD)
  ASSERT_LOGGER_MODULE(debug);
  ASSERT_LOGGER_MODULE(is_debug_enabled);
  ASSERT_LOGGER_MODULE(info);
  ASSERT_LOGGER_MODULE(is_info_enabled);
  ASSERT_LOGGER_MODULE(warn);
  ASSERT_LOGGER_MODULE(is_warn_enabled);
  ASSERT_LOGGER_MODULE(error);
  ASSERT_LOGGER_MODULE(is_error_enabled);
#undef ASSERT_LOGGER_MODULE
}