//
// Created by igor on 3/18/24.
//


#include <vfs/extra/tarfs.hh>
#include <fstream>

#include "tarfs_impl.hh"

namespace vfs::extra {

	tarfs_impl::tarfs_impl ()
		: archived_fs<tar_entry, true> ("tarfs") {
	}

	std::unique_ptr<archive_io<tar_entry>> tarfs_impl::create_archive_io (const std::string& params) {
		return std::make_unique<tar_archive>(params);
	}

	std::unique_ptr<vfs::module::filesystem> create_tarfs() {
		return std::make_unique<tarfs_impl>();
	}

}

#if !defined(TARFS_STATIC_DEFINE)
REGISTER_VFS_MODULE(vfs::extra::tarfs_impl)
#endif