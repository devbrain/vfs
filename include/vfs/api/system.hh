//
// Created by igor on 26/12/2019.
//

#ifndef SYSTEM_HH
#define SYSTEM_HH

#include <map>
#include <optional>

#include <vfs/api/stdfilesystem.hh>
#include <vfs/api/vfs_api.h>
#include <api/detail/path.hh>
#include <vfs/api/modules.hh>
#include <vfs/api/mounts.hh>
#include <vfs/api/stats.hh>
#include <vfs/api/directory_iterator.hh>

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning( disable : 4251 )
#endif

namespace vfs
{
	VFS_API void deinitialize();

	VFS_API void load_module(const stdfs::path& path_to_module);
	VFS_API modules get_modules();

	VFS_API void mount(const std::string& fstype, const std::string& args, const std::string& mount_point);
	VFS_API mounts get_mounts();
	VFS_API void unmount (const std::string& mount_point);


	VFS_API std::optional<stats> get_stats(const std::string& pth);
	VFS_API  directory open_directory (const std::string& pth);
	VFS_API void create_directory (const std::string& pth);
	VFS_API void unlink (const std::string& pth);

} // ns vfs

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

#endif //SYSTEM_HH
