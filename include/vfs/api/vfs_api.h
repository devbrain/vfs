//
// Created by igor on 7/18/23.
//

#ifndef INCLUDE_VFS_API_VFS_API_H
#define INCLUDE_VFS_API_VFS_API_H


#if defined(__cplusplus)
#include <cstdint>
#include <ctime>
#define VFS_EXTERN_C extern "C"
#else
#include <stdint.h>
#include <time.h>
#define VFS_EXTERN_C
#endif

/**
 * Errors
 */

enum vfs_error_code_t {
  VFS_ERROR_OK,
  VFS_ERROR_NO_ENTRY,
  VFS_ERROR_DIRECTORY_EXPECTED,
  VFS_FILE_EXPECTED,

  VFS_ERROR_USER
};

struct vfs_error_module {
  void* opaque;

  void (*set_error)(struct vfs_error_module* self, int error_code);
  void (*clear_error)(struct vfs_error_module* self);
};

struct vfs_logger_module {
  void* opaque;
  void (*debug)(struct vfs_logger_module* self, const char* module_name, const char* source_file, int line, const char* string);
  int (*is_debug_enabled)(struct vfs_logger_module* self);
  void (*info)(struct vfs_logger_module* self, const char* module_name, const char* source_file, int line, const char* string);
  int (*is_info_enabled)(struct vfs_logger_module* self);
  void (*warn)(struct vfs_logger_module* self, const char* module_name, const char* source_file, int line, const char* string);
  int (*is_warn_enabled)(struct vfs_logger_module* self);
  void (*error)(struct vfs_logger_module* self, const char* module_name, const char* source_file, int line, const char* string);
  int (*is_error_enabled)(struct vfs_logger_module* self);

};

struct vfs_api_filesystem;
struct vfs_api_dentry;

/**
 * Main module
 */
struct vfs_api_module {
  void* opaque;
  /**
   * Returns human readable description of parameters. This string will never be deleted/freed
   */
  const char* (*describe_parameters)(struct vfs_api_module* self);
  /**
   * returns the name of this module, the returned value never will be deleted/freed
   */
  const char* (*get_name)(struct vfs_api_module* self);
  /**
   * Module destructor
   * @param self
   */
  void (*destroy)(struct vfs_api_module* self);

  void (*init_error_module)(struct vfs_api_module* self, struct vfs_error_module* error_module);
  void (*init_logger_module)(struct vfs_api_module* self, struct vfs_logger_module* error_module);
  /**
   * converts module specific errors to strings
   * @param error_code > VFS_ERROR_USER
   * @return non disposable string
   */
  const char* (*error_to_string) (struct vfs_api_module* self, int error_code);
  struct vfs_error_module* (*get_error_module)(struct vfs_api_module* self);
  /**
   * Filesystem constructor
   */
  struct vfs_api_filesystem* (*create_filesystem)(struct vfs_api_module* self, const char* params);
};
/**
 * Filesystem object
 */
struct vfs_api_filesystem {
  void* opaque; /* private data to be implemented by provider */

  /**
   * Destructor
   */
  void (*destroy)(struct vfs_api_filesystem* self);
  struct vfs_api_module* (*get_module)(struct vfs_api_filesystem* self);
  struct vfs_api_dentry* (*get_root)(struct vfs_api_filesystem* self);
};

typedef enum {
  VFS_API_DIRECTORY,
  VFS_API_FILE,
  VFS_API_LINK
} vfs_api_dentry_type;

typedef void (*vfs_api_directory_iterator_t)(void* context, const char* name, const struct vfs_api_dentry* entry);

struct vfs_api_dentry {
  void* opaque;

  vfs_api_dentry_type (*get_type)(struct vfs_api_dentry* self);
  time_t (*get_ctime)(struct vfs_api_dentry* self);
  time_t (*get_mtime)(struct vfs_api_dentry* self);
  uint64_t (*get_size)(struct vfs_api_dentry* self); /* for files only */
  const char* (*get_target)(struct vfs_api_dentry* self); /* for links only */
  void (*iterate)(struct vfs_api_dentry* self, void* context, vfs_api_directory_iterator_t iterator); /* for dirs only */
  vfs_api_dentry* (*load_dentry)(struct vfs_api_dentry* self, const char* name);
  void (* destroy)(struct vfs_api_dentry* victim);
};


#define VFS_MODULE_FACTORY_NAME vfs_module_factory
#define VFS_MODULE_FACTORY VFS_EXTERN_C __attribute__((visibility("default"))) struct vfs_api_module* VFS_MODULE_FACTORY_NAME ()
#endif //INCLUDE_VFS_API_VFS_API_H
