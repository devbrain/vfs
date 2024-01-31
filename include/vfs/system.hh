//
// Created by igor on 26/12/2019.
//

#ifndef VFS_SYSTEM_HH
#define VFS_SYSTEM_HH


#include <filesystem>

#include <vfs/api/vfs_api.h>
#include <vfs/modules.hh>
#include <vfs/mounts.hh>

#include <vfs/api/vfs_module.h>


#include <bsw/warn/push.hh>
#include <bsw/warn/dll_interface>

namespace vfs {
	VFS_API void deinitialize ();

	VFS_API void load_module (const std::filesystem::path& path_to_module);
	VFS_API void load_module (std::unique_ptr<module::filesystem> fsptr);


	VFS_API void mount (const std::string& fstype, const std::string& args, const std::string& mount_point);
	VFS_API mounts get_mounts ();

} // ns vfs

#include <bsw/warn/pop.hh>

#endif //VFS_SYSTEM_HH
