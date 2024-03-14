//
// Created by igor on 3/5/24.
//

#ifndef VFS_INCLUDE_VFS_EXTRA_FLOPPYFS_HH_
#define VFS_INCLUDE_VFS_EXTRA_FLOPPYFS_HH_

#include <memory>
#include <vfs/extra/floppyfs_api.h>
#include <vfs/api/vfs_module.h>


namespace vfs::extra {
	FLOPPYFS_API std::unique_ptr<vfs::module::filesystem> create_floppyfs();
}

#endif //VFS_INCLUDE_VFS_EXTRA_FLOPPYFS_HH_
