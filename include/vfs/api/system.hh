//
// Created by igor on 26/12/2019.
//

#ifndef SYSTEM_HH
#define SYSTEM_HH

#include <map>
#include <optional>

#include "vfs/api/stdfilesystem.hh"
#include "vfs/api/vfs_api.h"
#include "vfs/api/path.hh"
#include "vfs/api/modules.hh"
#include "vfs/api/mounts.hh"

namespace vfs
{
	VFS_API void load_module(const stdfs::path& path_to_module);
	VFS_API modules get_modules();

	VFS_API void mount(const std::string& fstype, const std::string& args, const path& mount_point);
	VFS_API mounts get_mounts();

	VFS_API void unmount (const path& mount_point);

	struct VFS_API stats
	{
		uint64_t size;
		std::map<std::string, std::string> attribs;

		enum type_t
		{
			eLINK,
			eFILE,
			eDIRECTORY
		};

		type_t type;
	};

	VFS_API std::optional<stats> get_stats(const std::string& pth);
} // ns vfs

#endif //SYSTEM_HH
