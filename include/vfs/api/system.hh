//
// Created by igor on 26/12/2019.
//

#ifndef SYSTEM_HH
#define SYSTEM_HH


#include "vfs/api/stdfilesystem.hh"
#include "vfs/api/vfs_api.h"
#include "vfs/api/path.hh"

namespace vfs
{
	VFS_API void load_module(const stdfs::path& path_to_module);

	VFS_API void mount(const std::string& fstype, const std::string& args, const path& mount_point);
	VFS_API void unmount (const path& mount_point);
} // ns vfs

#endif //SYSTEM_HH
