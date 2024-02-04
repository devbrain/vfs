//
// Created by igor on 2/4/24.
//

#ifndef VFS_SRC_API_SYSTEM_IFACE_HH_
#define VFS_SRC_API_SYSTEM_IFACE_HH_

namespace vfs {
	namespace core {
		class dentry_tree;
		class fstab;
	}

	core::fstab* get_system_fstab();
	core::dentry_tree* get_system_dentry_tree();


}

#endif //VFS_SRC_API_SYSTEM_IFACE_HH_
