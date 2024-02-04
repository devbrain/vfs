//
// Created by igor on 2/4/24.
//

#include "zipfs_impl.hh"
#include <vfs/extra/zipfs.hh>

namespace vfs::extra {
	zipfs::zipfs ()
		: vfs::module::filesystem ("zipfs") {

	}

	vfs::module::inode* zipfs::load_root (const std::string& params) {
		return nullptr;
	}

	size_t zipfs::max_name_length () {
		return 64;
	}

	int zipfs::sync () {
		return 1;
	}

	int zipfs::sync_inode (vfs::module::inode* inod) {
		return 1;
	}

	[[nodiscard]] bool zipfs::is_readonly () const {
		return false;
	}

	vfs::module::filesystem* create_zipfs() {
		return new zipfs;
	}
}

#if !defined(ZIPFS_STATIC_DEFINE)
REGISTER_VFS_MODULE(vfs::extra::zipfs)
#endif

