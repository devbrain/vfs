//
// Created by igor on 2/18/24.
//

#ifndef VFS_SRC_EXTRA_ZIPFS_ZIPFS_INODE_HH_
#define VFS_SRC_EXTRA_ZIPFS_ZIPFS_INODE_HH_

#include <vfs/api/vfs_module.h>
#include <iosfwd>
#include "miniz.h"

namespace vfs::detail {
	class zipfs_inode : public vfs::module::inode {
	 public:
		explicit zipfs_inode (std::istream* stream);

		inode* lookup (const char* name) override;
		uint64_t size () override;
		vfs::module::directory_iterator* get_directory_iterator () override;
		bool mkdir (const char* name) override;
		bool mkfile (const char* name) override;
		int unlink () override;

		vfs::module::file* open_file (open_mode_type mode_type) override;
	 private:
		std::istream* m_stream;
		mz_zip_archive m_archive;
	};
}


#endif //VFS_SRC_EXTRA_ZIPFS_ZIPFS_INODE_HH_
