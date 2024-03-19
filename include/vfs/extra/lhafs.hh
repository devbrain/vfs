//
// Created by igor on 3/5/24.
//

#ifndef VFS_INCLUDE_VFS_EXTRA_TARFS_HH_
#define VFS_INCLUDE_VFS_EXTRA_TARFS_HH_

#include <memory>
#include <vfs/extra/lhafs_api.h>
#include <vfs/api/vfs_module.h>


namespace vfs::extra {
	LHAFS_API std::unique_ptr<vfs::module::filesystem> create_lhafs();
}

#endif //VFS_INCLUDE_VFS_EXTRA_FLOPPYFS_HH_
