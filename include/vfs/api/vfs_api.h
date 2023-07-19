//
// Created by igor on 7/18/23.
//

#ifndef INCLUDE_VFS_API_VFS_API_H
#define INCLUDE_VFS_API_VFS_API_H

#if defined(__cplusplus)
#define VFS_EXTERN_C extern "C"
#else
#define VFS_EXTERN_C
#endif

struct vfs_api_filesystem;

/**
 * Main module
 */
struct vfs_api_module {
  /**
   * returns the name of this module, the returned value never will be deleted/freed
   */
  const char* (*get_name)();
  /**
   * Module destructor
   * @param self
   */
  void (*destroy)(struct vfs_api_module* self);
  /**
   * Filesystem constructor
   */
  vfs_api_filesystem* (*create_filesystem)();
};
/**
 * Filesystem object
 */
struct vfs_api_filesystem {
  void* opaque; /* private data to be implemented by provider */
  /**
   * Returns human readable description of parameters. This string will never be deleted/freed
   */
  const char* (*describe_parameters)();
  /**
   * Initializes filesystem
   *
   * @param self this->opaque
   * @param params initialization params
   * @return
   */
  int (*init)(void* self, const char* params);
  /**
   * Destructor
   */
  void (*destroy)(vfs_api_filesystem* self);
};


#define VFS_MODULE_FACTORY_NAME vfs_module_factory
#define VFS_MODULE_FACTORY VFS_EXTERN_C __attribute__((visibility("default"))) struct vfs_api_module* VFS_MODULE_FACTORY_NAME
#endif //INCLUDE_VFS_API_VFS_API_H
