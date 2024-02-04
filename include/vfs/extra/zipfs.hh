//
// Created by igor on 2/4/24.
//

#ifndef VFS_INCLUDE_VFS_EXTRA_ZIPFS_HH_
#define VFS_INCLUDE_VFS_EXTRA_ZIPFS_HH_

#include <vfs/extra/zipfs_api.h>
#include <vfs/api/vfs_module.h>

namespace vfs::extra {
	ZIPFS_API vfs::module::filesystem* create_zipfs();
}

#endif //VFS_INCLUDE_VFS_EXTRA_ZIPFS_HH_
