//
// Created by igor on 3/18/24.
//

#ifndef VFS_SRC_EXTRA_TARFS_TARFS_INODE_HH_
#define VFS_SRC_EXTRA_TARFS_TARFS_INODE_HH_

#include <vfs/api/vfs_module.h>
#include "tarfile.hh"

namespace vfs::extra {
	class tarfs_inode : public vfs::module::inode {
	 public:
		tarfs_inode (tarfile* archive, tar_tree* node);

		inode* lookup (const char* name) override;
		uint64_t size () override;
		vfs::module::directory_iterator* get_directory_iterator () override;
		bool mkdir (const char* name) override;
		bool mkfile (const char* name) override;
		int unlink () override;
		[[nodiscard]] bool is_sequential () const override;
		vfs::module::file* open_file (open_mode_type mode_type) override;
	 private:
		tarfile* m_archive;
		tar_tree* m_node;
	};
}
#endif //VFS_SRC_EXTRA_TARFS_TARFS_INODE_HH_
