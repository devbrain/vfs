//
// Created by igor on 2/4/24.
//

#ifndef VFS_SRC_EXTRA_ZIPFS_ZIPFS_IMPL_HH_
#define VFS_SRC_EXTRA_ZIPFS_ZIPFS_IMPL_HH_

#include <memory>
#include "zip_archive.hh"

namespace vfs::extra {
	class zipfs : public archived_fs<zip_entry> {
	 public:
		zipfs ();
	 private:
		std::unique_ptr<archive_io<zip_entry>> create_archive_io (const std::string& params) override;
	};
}

#endif //VFS_SRC_EXTRA_ZIPFS_ZIPFS_IMPL_HH_
