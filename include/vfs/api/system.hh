//
// Created by igor on 26/12/2019.
//

#ifndef SYSTEM_HH
#define SYSTEM_HH


#include "vfs/api/stdfilesystem.hh"
#include "vfs/api/vfs_api.h"

namespace vfs
{
	VFS_API void load_module(const stdfs::path& path_to_module);
} // ns vfs

#endif //SYSTEM_HH
