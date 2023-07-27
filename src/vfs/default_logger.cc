//
// Created by igor on 7/27/23.
//

#include <iostream>

#include "default_logger.hh"


namespace vfs {

  static void do_print(const char* pfx,
                       const char* module_name,
                       [[maybe_unused]] const char* source_file,
                       [[maybe_unused]] int line,
                       const char* string) {
    std::cout << pfx << " [" << module_name <<"] " << string << std::endl;
  }


  void default_logger::_debug (const char* module_name, const char* source_file, int line, const char* string) {
    do_print ("DBG", module_name, source_file, line, string);
  }

  bool default_logger::_is_debug_enabled () const {
    return true;
  }

  void default_logger::_info (const char* module_name, const char* source_file, int line, const char* string) {
    do_print ("INF", module_name, source_file, line, string);
  }

  bool default_logger::_is_info_enabled () const {
    return true;
  }

  void default_logger::_warn (const char* module_name, const char* source_file, int line, const char* string) {
    do_print ("WRN", module_name, source_file, line, string);
  }

  bool default_logger::_is_warn_enabled () const {
    return true;
  }

  void default_logger::_error (const char* module_name, const char* source_file, int line, const char* string) {
    do_print ("ERR", module_name, source_file, line, string);
  }

  bool default_logger::_is_error_enabled () const {
    return true;
  }
}