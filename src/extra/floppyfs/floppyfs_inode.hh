//
// Created by igor on 3/5/24.
//

#ifndef VFS_SRC_EXTRA_FLOPPYFS_FLOPPYFS_INODE_HH_
#define VFS_SRC_EXTRA_FLOPPYFS_FLOPPYFS_INODE_HH_

#include <vfs/api/vfs_module.h>
#include "fat_12.hh"

namespace vfs::extra {
	class floppyfs_inode : public vfs::module::inode {
	 public:
		floppyfs_inode (fat12* fat, uint16_t cluster, bool is_dir, uint32_t size);

		inode* lookup (const char* name) override;
		uint64_t size () override;
		vfs::module::directory_iterator* get_directory_iterator () override;
		bool mkdir (const char* name) override;
		bool mkfile (const char* name) override;
		int unlink () override;
		[[nodiscard]] bool is_sequential() const override;
		vfs::module::file* open_file (open_mode_type mode_type) override;
	 private:
		fat12* m_fat;
		uint16_t m_cluster;
		uint32_t m_size;
	};
};

#endif //VFS_SRC_EXTRA_FLOPPYFS_FLOPPYFS_INODE_HH_
