//
// Created by igor on 2/18/24.
//

#ifndef VFS_SRC_EXTRA_ZIPFS_ZIPFS_INODE_HH_
#define VFS_SRC_EXTRA_ZIPFS_ZIPFS_INODE_HH_

#include <vfs/api/vfs_module.h>
#include "zip_archive.hh"

namespace vfs::extra {
	class zipfs_inode : public vfs::module::inode {
	 public:
		zipfs_inode (zip_archive* archive, zip_tree* node);

		inode* lookup (const char* name) override;
		uint64_t size () override;
		vfs::module::directory_iterator* get_directory_iterator () override;
		bool mkdir (const char* name) override;
		bool mkfile (const char* name) override;
		int unlink () override;
		bool is_sequential() const override;
		vfs::module::file* open_file (open_mode_type mode_type) override;
	 private:
		zip_archive* m_archive;
		zip_tree* m_node;
	};
}


#endif //VFS_SRC_EXTRA_ZIPFS_ZIPFS_INODE_HH_
