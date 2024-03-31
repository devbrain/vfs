//
// Created by igor on 3/18/24.
//

#ifndef VFS_SRC_EXTRA_TARFS_TARFS_IMPL_HH_
#define VFS_SRC_EXTRA_TARFS_TARFS_IMPL_HH_

#include "extra/extra_tools/archived_fs.hh"
#include "tar_archive.hh"

namespace vfs::extra {
	class tarfs_impl  : public archived_fs<tar_entry, true> {
	 public:
		tarfs_impl ();
	 private:
		std::unique_ptr<archive_io<tar_entry>> create_archive_io (const std::string& params) override;
	};
}

#endif //VFS_SRC_EXTRA_TARFS_TARFS_IMPL_HH_
